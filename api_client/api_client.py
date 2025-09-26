# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

"""EVerest API command line client application"""
import uuid
import os
import json
import argparse
import cmd2
import queue
import threading
from spyclient_api.SpyClientCommandSet import SpyClientCommandSet
from power_supply_dc_api.PowerSupplyDCAPIClientCommandSet import PowerSupplyDCAPIClientCommandSet
from auth_token_validator_api.AuthTokenValidatorAPIClientCommandSet import AuthTokenValidatorAPIClientCommandSet
from system_api.SystemAPIClientCommandSet import SystemAPIClientCommandSet
from error_history_consumer_api.ErrorHistoryConsumerAPIClientCommandSet import ErrorHistoryConsumerAPIClientCommandSet
from auth_consumer_api.AuthConsumerAPIClientCommandSet import AuthConsumerAPIClientCommandSet
from auth_token_provider_api.AuthTokenProviderAPIClientCommandSet import AuthTokenProviderAPIClientCommandSet
from evse_bsp_api.EvseBspAPIClientCommandSet import EvseBspAPIClientCommandSet
from isolation_monitor_api.IsolationMonitorAPIClientCommandSet import IsolationMonitorAPIClientCommandSet
from ocpp_consumer_api.OcppConsumerAPIClientCommandSet import OcppConsumerAPIClientCommandSet
from evse_manager_consumer_api.EvseManagerConsumerAPIClientCommandSet import EvseManagerConsumerAPIClientCommandSet
from display_message_api.DisplayMessageAPIClientCommandSet import DisplayMessageAPIClientCommandSet
from powermeter_api.PowermeterAPIClientCommandSet import PowermeterAPIClientCommandSet
from generic_error_raiser_api.GenericErrorRaiserAPIClientCommandSet import GenericErrorRaiserAPIClientCommandSet
from over_voltage_monitor_api.OverVoltageMonitorAPIClientCommandSet import OverVoltageMonitorAPIClientCommandSet
from external_energy_limits_consumer_api.ExternalEnergyLimitsConsumerAPIClientCommandSet import ExternalEnergyLimitsConsumerAPIClientCommandSet

class EVerestAPICmd(cmd2.Cmd):
    """EVerest API command line client application"""

    def __init__(self):
        super().__init__(include_py=True, persistent_history_file=os.path.expanduser("~/.api_client"))
        self.mqtt_clients = []
        self.api_clients = []
        self.stop_requested = False
        self.show_topic = ""
        self.hide_topic = ""
        self.quiet = True
        self.prettify = False
        self.replyToPlaceholder = "everest_api/1.0/{interface_type}/{module_id}/e2m/{operation_name}/{uuid}"
        self.colors = True

        self.IN_COLOR = '\033[92m'   # Green
        self.OUT_COLOR = '\033[93m'  # Yellow
        self.RESET = '\033[0m'    # Reset to default color

        self.add_settable(cmd2.Settable('show_topic', str, 'Comma separated list. If set shows ONLY the messages with topics matching the list\'s strings. If hide_topic is set too, it will be applied first, then show_topic will be applied.', self))
        self.add_settable(cmd2.Settable('hide_topic', str, 'Comma separate list. If set hides ONLY the messages with topics matching the list\'s strings. If show_topic is set too, hide_topic is applied first, then show_topic.', self))
        self.add_settable(cmd2.Settable('prettify', bool, 'Prettify the json outputs', self))
        self.add_settable(cmd2.Settable('colors', bool, 'Use colors for the outputs', self))
        self.add_settable(cmd2.Settable('replyToPlaceholder', str, 'The placeholder to search and replace in a payload', self))

        self.output_queue = queue.Queue()
        # Start a thread to process the queue and handle output
        self.queue_processing_thread = threading.Thread(target=self.process_output_queue)
        self.queue_processing_thread.daemon = True
        self.queue_processing_thread.start()

    def complete_module_id(self, text, line, begidx, endidx):
        return [module["id"] for module in self.api_clients if module["id"].startswith(text)]

    def get_instance_by_id(self, target_id):
        return next((client["instance"] for client in self.api_clients if client["id"] == target_id), None)

    def process_output_queue(self):
        while not self.stop_requested:
            try:
                message = self.output_queue.get(timeout=1)
                with self.terminal_lock:
                    self.async_alert(message)
                self.output_queue.task_done()
            except:
                continue

    def check_match(self, topic, substr_list):
        for substr in substr_list:
            if substr.strip() in topic:
                return True
        return False

    def generate_replyTo(self, payload, topic):
        return payload.replace(self.replyToPlaceholder,
                               topic.replace("m2e", "e2m") + "/" +
                               str(uuid.uuid4()))

    def log_event(self, topic, payload):
        showing = True
        if ('heartbeat' in topic or 'communication_check' in topic):
            if self.quiet:
                showing = False
        else:
            if self.show_topic and self.hide_topic:
                if self.check_match(topic, self.hide_topic.split(",")):
                    showing = False
                if self.check_match(topic, self.show_topic.split(",")):
                    showing = True
            else:
                if self.show_topic and not self.check_match(topic, self.show_topic.split(",")):
                    showing = False
                if self.hide_topic and self.check_match(topic, self.hide_topic.split(",")):
                    showing = False
        if showing:
            if "ERROR:" in topic:
                if threading.current_thread() == threading.main_thread():
                    self.poutput(self.OUT_COLOR + topic + " " + payload + self.RESET if self.colors else topic + " " + payload)
                else:
                    self.output_queue.put(topic + payload)
            else:
                prefix = self.IN_COLOR + "In: " if self.colors else "In: "
                if "m2e" in topic:
                    prefix = self.OUT_COLOR + "Out: " if self.colors else "Out: "
                message = prefix + topic + ', ' + payload + self.RESET if self.colors else prefix + topic + ', ' + payload
                self.poutput(message) if threading.current_thread() == threading.main_thread() else self.output_queue.put(message)

    client_add_parser = cmd2.Cmd2ArgumentParser()
    client_add_parser.add_subparsers(title='client_type', help='the client type to be added')
    @cmd2.with_argparser(client_add_parser)
    @cmd2.with_category('Client Management')
    def do_client_add(self, ns: argparse.Namespace):
        """Creates a client with the given module id"""
        handler = ns.cmd2_handler.get()
        if handler is not None:
            handler(ns)
        else:
            # No subcommand was provided, so call help
            self.poutput('This command does nothing without sub-parsers registered ...')
            self.do_help('client_add')

    client_remove_parser = cmd2.Cmd2ArgumentParser()
    client_remove_parser.add_argument('module_id', help='the module id for the client', completer=complete_module_id)
    @cmd2.with_argparser(client_remove_parser)
    @cmd2.with_category('Client Management')
    def do_client_remove(self, args):
        """Removes a client with the given module id"""
        for index, client in enumerate(self.mqtt_clients):
            if client["id"] == args.module_id:
                client["mqtt_client"].loop_stop()
                del self.mqtt_clients[index]
                break
        self.api_clients = [client for client in self.api_clients if client["id"] != args.module_id]

    def complete_send_communication_check_payload(self, text, line, begidx, endidx):
        return [payload for payload in ['true'] if payload.startswith(text)]

    send_communication_check_parser = cmd2.Cmd2ArgumentParser()
    send_communication_check_parser.add_argument('module_id', help='the module id for the client', completer=complete_module_id)
    send_communication_check_parser.add_argument('payload', help='the payload as json to be send', completer=complete_send_communication_check_payload)
    @cmd2.with_argparser(send_communication_check_parser, preserve_quotes=True)
    @cmd2.with_category('Client Management')
    def do_send_communication_check(self, args):
        """Sends a 'heartbeat' like signal to the EVerest API to confirm that the external module is alive"""
        api_client = self.get_instance_by_id(args.module_id)
        if api_client:
            api_client.send.send_communication_check.publish(json.dumps(json.loads(args.payload)))
        else:
            self.log_event("ERROR:", "Invalid module id: '" + args.module_id + "' Have you created the client using the 'add_client' function?")

    receive_heartbeat_parser = cmd2.Cmd2ArgumentParser()
    receive_heartbeat_parser.add_subparsers(title='client_type', help='the client type to be added')
    @cmd2.with_argparser(receive_heartbeat_parser)
    @cmd2.with_category('Client Management')
    def do_receive_heartbeat(self, ns: argparse.Namespace):
        """Blocks until it receives the heartbeat signal from the EVerest API"""
        handler = ns.cmd2_handler.get()
        if handler is not None:
            handler(ns)
        else:
            # No subcommand was provided, so call help
            self.poutput('This command does nothing without sub-parsers registered ...')
            self.do_help('client_add')

    def postloop(self):
        self.output_queue.join()
        self.stop_requested = True
        self.queue_processing_thread.join()
        for mqtt_client in self.mqtt_clients:
            mqtt_client["mqtt_client"].loop_stop()


if __name__ == '__main__':
    import sys
    cmd = EVerestAPICmd()
    sys.exit(cmd.cmdloop())
