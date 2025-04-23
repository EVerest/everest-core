import json
import os
import paho.mqtt.client as mqtt
from paho.mqtt import __version__ as paho_mqtt_version

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
        if paho_mqtt_version < '2.0':
            self._mqtt_client = mqtt.Client(self._everest_core.everest_uuid)
        else:
            self._mqtt_client = mqtt.Client(
                callback_api_version=mqtt.CallbackAPIVersion.VERSION2, client_id=self._everest_core.everest_uuid)
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

    def plug_in_ac_iso(self, connector_id=1):
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/cmd/execute_charging_session",
            "sleep 1;iso_wait_slac_matched;iso_start_v2g_session AC;iso_wait_pwr_ready;iso_draw_power_regulated 16,3;sleep 20;iso_stop_charging;iso_wait_v2g_session_stopped;unplug")

    def plug_in_dc_iso(self, connector_id=1):
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/cmd/execute_charging_session",
            "sleep 1;iso_wait_slac_matched;iso_start_v2g_session DC;iso_wait_pwr_ready;iso_wait_for_stop 20;iso_wait_v2g_session_stopped;unplug"
        )

    def plug_out(self, connector_id=1):
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/cmd/modify_charging_session",
            "unplug")

    def plug_out_iso(self, connector_id=1):
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/cmd/modify_charging_session",
            "iso_stop_charging;iso_wait_v2g_session_stopped;unplug")

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

    def diode_fail(self, connector_id=1):
        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/cmd/execute_charging_session",
            "sleep 1;iec_wait_pwr_ready;sleep 1;draw_power_regulated 32,3;sleep 5;diode_fail;sleep 36000;unplug")

    def raise_error(self, error_string="MREC6UnderVoltage", connector_id=1):
        raise_error_payload = {
            "error_type": error_string,
            "raise": "true"
        }

        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/error",
            json.dumps(raise_error_payload))

    def clear_error(self, error_string="MREC6UnderVoltage", connector_id=1):
        clear_error_payload = {
            "error_type": error_string,
            "raise": "false"
        }

        self._mqtt_client.publish(
            f"{self._mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/error",
            json.dumps(clear_error_payload))

    def publish(self, topic, payload):
        self._mqtt_client.publish(topic, payload)

    def _destroy_mqtt_client(self):
        if self._mqtt_client:
            self._mqtt_client.disconnect()
            self._mqtt_client = None
