# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

import os
import json
import shutil
from pathlib import Path
from paho.mqtt import client as mqtt_client

from everest.testing.ocpp_utils.controller.test_controller_interface import TestController
from everest.testing.core_utils.everest_core import EverestCore


class EverestTestController(TestController):

    def __init__(self, everest_core_path, config_path: Path, chargepoint_id: str,
                 test_function_name: str = None) -> None:
        self.everest_core = EverestCore(everest_core_path, config_path)
        self.config_path = config_path
        self.mqtt_client = None
        self.chargepoint_id = chargepoint_id
        self.test_function_name = test_function_name

    def start(self):

        self.copy_occp_config()
        self.everest_core.start(None)

        mqtt_server_uri = os.environ.get("MQTT_SERVER_ADDRESS", "127.0.0.1")
        mqtt_server_port = int(os.environ.get("MQTT_SERVER_PORT", "1883"))

        self.mqtt_client = mqtt_client.Client("unique_client_id")
        self.mqtt_client.connect(mqtt_server_uri, mqtt_server_port)

        self.mqtt_client.publish(
            "everest_external/nodered/1/carsim/cmd/enable", "true")
        self.mqtt_client.publish(
            "everest_external/nodered/2/carsim/cmd/enable", "true")

    def stop(self):
        self.everest_core.stop()

    def plug_in(self, connector_id=1):
        self.mqtt_client.publish(f"everest_external/nodered/{connector_id}/carsim/cmd/execute_charging_session",
                                 "sleep 1;iec_wait_pwr_ready;sleep 1;draw_power_regulated 32,1;sleep 200;unplug")

    def plug_in_ac_iso(self, payment_type='contract', connector_id=1):
        self.mqtt_client.publish(f"everest_external/nodered/{connector_id}/carsim/cmd/execute_charging_session",
                                 f"sleep 1;iso_wait_slac_matched;iso_start_v2g_session {payment_type},AC_three_phase_core;iso_wait_pwr_ready;iso_draw_power_regulated 16,3;sleep 20;iso_stop_charging;iso_wait_v2g_session_stopped;unplug")

    def plug_out(self, connector_id=1):
        self.mqtt_client.publish(f"everest_external/nodered/{connector_id}/carsim/cmd/modify_charging_session",
                                 "unplug")

    def swipe(self, token, connectors=[1]):
        provided_token = {
            "id_token": token,
            "type": "RFID",
            "connectors": connectors
        }
        self.mqtt_client.publish(
            "everest_api/dummy_token_provider/cmd/provide", json.dumps(provided_token))

    def connect_websocket(self):
        self.mqtt_client.publish("everest_api/ocpp/cmd/connect", "on")

    def disconnect_websocket(self):
        self.mqtt_client.publish("everest_api/ocpp/cmd/disconnect", "off")

    def rcd_error(self, connector_id=1):
        self.mqtt_client.publish(f"everest_external/nodered/{connector_id}/carsim/cmd/execute_charging_session",
                                 "sleep 1;rcd_current 10.3;sleep 10;rcd_current 0.1;unplug")

    def publish(self, topic, payload):
        self.mqtt_client.publish(topic, payload)

    def copy_occp_config(self):
        ocpp_dir = self.everest_core.everest_core_build_path / "dist/share/everest/ocpp"
        dest_file = os.path.join(ocpp_dir, self.config_path.name)
        shutil.copy(self.config_path, dest_file)
