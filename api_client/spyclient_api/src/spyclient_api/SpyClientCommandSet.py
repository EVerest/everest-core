# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

import json
import cmd2
import os
import argparse
import paho.mqtt.client as mqtt
from .SpyClient import SpyClient
from cmd2 import CommandSet, with_default_category, with_argparser

@with_default_category('SpyClient')
class SpyClientCommandSet(CommandSet):
  """ SpyClient """

  def __init__(self):
    super().__init__()

  def on_receive(self, topic, data):
    #self._cmd.log_event(topic, json.dumps(json.loads(data.decode('utf-8')), indent=1 if self._cmd.prettyfy else None))
    self._cmd.log_event(topic, data.decode('utf-8'))

  client_add_parser = cmd2.Cmd2ArgumentParser()
  client_add_parser.add_argument('mqtt_topic', nargs='?', help='the path/topic for the client to spy on, if empty will spy on everest')
  @cmd2.as_subcommand_to('client_add', 'SpyClient', client_add_parser)
  def client_add_SpyClient(self, ns: argparse.Namespace):
    # Call whatever subcommand function was selected
    mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    mqtt_host = (os.getenv('MQTT_BROKER') or
      os.getenv('MQTT_SERVER_ADDRESS') or
      os.getenv('MQTT_HOST') or
      'localhost')
    try:
      mqtt_port = int(os.getenv('MQTT_BROKER_PORT') or
        os.getenv('MQTT_SERVER_PORT') or
        os.getenv('MQTT_PORT') or
        1883)
    except ValueError:
      self._cmd.log_event("WARNING:", "Invalid MQTT port value, using default port 1883")
      mqtt_port = 1883
    mqtt_client.connect(mqtt_host, mqtt_port)
    self._cmd.mqtt_clients.append({"id": "" if ns.mqtt_topic == None else ns.mqtt_topic, "mqtt_client": mqtt_client})
    mqtt_client.loop_start()
    api_client = SpyClient(mqtt_client, "" if ns.mqtt_topic == None else ns.mqtt_topic)
    self._cmd.api_clients.append({"id": "" if ns.mqtt_topic == None else ns.mqtt_topic, "instance": api_client})
    lambada_receive = lambda topic, data: self.on_receive(topic, data)
    api_client.receive.spy.register_callback(lambada_receive)
