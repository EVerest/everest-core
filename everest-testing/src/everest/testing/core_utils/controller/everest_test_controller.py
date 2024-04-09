import json
import os
from paho.mqtt.client import Client

from everest.testing.core_utils.everest_core import EverestCore

from everest.testing.core_utils.controller.test_controller_interface import TestController


class EverestTestController(TestController):

    def __init__(self,
                 everest_core: EverestCore
                 ):
        self._everest_core = everest_core
        self._mqtt_client = None

    @property
    def _mqtt_external_prefix(self):
        return self._everest_core.mqtt_external_prefix

    def start(self):
        self._initialize_external_mqtt_client()
        self._everest_core.start()
        self._initialize_nodered_sil()

    def stop(self, *exc_details):
        self._everest_core.stop()
        self._destroy_mqtt_client()

    def _initialize_external_mqtt_client(self):
        mqtt_server_uri = os.environ.get("MQTT_SERVER_ADDRESS", "127.0.0.1")
        mqtt_server_port = int(os.environ.get("MQTT_SERVER_PORT", "1883"))
        self._mqtt_client = Client(self._everest_core.everest_uuid)
        self._mqtt_client.connect(mqtt_server_uri, mqtt_server_port)

    def _initialize_nodered_sil(self):
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_external/nodered/1/carsim/cmd/enable", "true")
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_external/nodered/2/carsim/cmd/enable", "true")

    def plug_in(self, connector_id=1):
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/cmd/execute_charging_session",
            "sleep 1;iec_wait_pwr_ready;sleep 1;draw_power_regulated 32,1;sleep 200;unplug")

    def plug_in_ac_iso(self, payment_type='contract', connector_id=1):
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/cmd/execute_charging_session",
            f"sleep 1;iso_wait_slac_matched;iso_start_v2g_session {payment_type},AC_three_phase_core;iso_wait_pwr_ready;iso_draw_power_regulated 16,3;sleep 20;iso_stop_charging;iso_wait_v2g_session_stopped;unplug")

    def plug_out(self, connector_id=1):
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/cmd/modify_charging_session",
            "unplug")

    def swipe(self, token, connectors=None):
        connectors = connectors if connectors is not None else [1]
        provided_token = {
            "id_token": {
                "value": token,
                "type": "ISO14443"
            },
            "authorization_type": "RFID",
            "connectors": connectors
        }
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_api/dummy_token_provider/cmd/provide", json.dumps(provided_token))

    def connect_websocket(self):
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_api/ocpp/cmd/connect", "on")

    def disconnect_websocket(self):
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_api/ocpp/cmd/disconnect", "off")

    def rcd_error(self, connector_id=1):
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/cmd/execute_charging_session",
            "sleep 1;rcd_current 10.3;sleep 10;rcd_current 0.1;unplug")

    def publish(self, topic, payload):
        self._mqtt_client.publish(topic, payload)

    def _destroy_mqtt_client(self):
        if self._mqtt_client:
            self._mqtt_client.disconnect()
            self._mqtt_client = None
