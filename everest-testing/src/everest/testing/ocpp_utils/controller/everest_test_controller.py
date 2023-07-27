# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

import os
import json
import shutil
from pathlib import Path
from paho.mqtt import client as mqtt_client
import logging
import yaml
import tempfile
from datetime import datetime

from everest.testing.ocpp_utils.controller.test_controller_interface import TestController
from everest.testing.core_utils.everest_core import EverestCore, TestControlModuleConnection

logging.basicConfig(level=logging.DEBUG)

TEST_LOGS_DIR = "/tmp/everest_ocpp_test_logs"


class EverestTestController(TestController):

    def __init__(self, everest_core_path: Path, libocpp_path: Path, config_path: Path, chargepoint_id: str, ocpp_version: str,
                 test_function_name: str = None, ocpp_module_id: str = "ocpp") -> None:
        self.everest_core = EverestCore(everest_core_path, config_path)
        self.libocpp_path = libocpp_path
        self.config_path = config_path
        self.mqtt_client = None
        self.chargepoint_id = chargepoint_id
        self.ocpp_version = ocpp_version
        self.test_function_name = test_function_name
        self.temp_ocpp_config_file = tempfile.NamedTemporaryFile(
            delete=False, mode="w+", suffix=".json")
        self.temp_ocpp_user_config_file = tempfile.NamedTemporaryFile(
            delete=False, mode="w+", suffix=".json")
        self.temp_ocpp_database_dir = tempfile.TemporaryDirectory()
        self.temp_ocpp_log_dir = tempfile.TemporaryDirectory()
        self.temp_ocpp_certs_dir = tempfile.TemporaryDirectory()
        self.first_run = True
        self.mqtt_external_prefix = ""
        self.ocpp_module_id = ocpp_module_id

    def start(self, central_system_port=None, standalone_module=None):
        logging.info(f"Central system port: {central_system_port}")
        # modify ocpp config with given central system port and modify everest-core config as well
        everest_config = yaml.safe_load(
            self.everest_core.everest_config_path.read_text())

        if self.ocpp_version == "ocpp1.6":
            ocpp_dir = self.everest_core.prefix_path / "share/everest/modules/OCPP"
            ocpp_config_path = ocpp_dir / \
                everest_config["active_modules"][self.ocpp_module_id]["config_module"]["ChargePointConfigPath"]
            ocpp_config = json.loads(ocpp_config_path.read_text())
            charge_point_id = ocpp_config["Internal"]["ChargePointId"]
            ocpp_config["Internal"][
                "CentralSystemURI"] = f"127.0.0.1:{central_system_port}/{charge_point_id}"
        else:
            logging.critical(everest_config)
            ocpp_dir = self.everest_core.prefix_path / "share/everest/modules/OCPP201"
            ocpp_config_path = ocpp_dir / \
                everest_config["active_modules"][self.ocpp_module_id]["config_module"]["ChargePointConfigPath"]
            ocpp_config = json.loads(ocpp_config_path.read_text())
            charge_point_id = ocpp_config["InternalCtrlr"]["ChargePointId"]["attributes"][
                "Actual"]
            network_connection_profiles = json.loads(ocpp_config["InternalCtrlr"]["NetworkConnectionProfiles"]["attributes"][
                "Actual"])
            network_connection_profiles[0]["connectionData"]["ocppCsmsUrl"] =  f"ws://127.0.0.1:{central_system_port}/{charge_point_id}"
            ocpp_config["InternalCtrlr"]["NetworkConnectionProfiles"]["attributes"][
                "Actual"] = json.dumps(network_connection_profiles)

        if self.first_run:
            logging.info("First run")
            self.first_run = False
            self.temp_ocpp_config_file.write(json.dumps(ocpp_config))
            self.temp_ocpp_config_file.flush()
            self.temp_ocpp_user_config_file.write("{}")
            self.temp_ocpp_user_config_file.flush()

        if "active_modules" in everest_config and self.ocpp_module_id in everest_config["active_modules"]:
            os.makedirs(TEST_LOGS_DIR, exist_ok=True)
            everest_config["active_modules"][self.ocpp_module_id]["config_module"]["ChargePointConfigPath"] = self.temp_ocpp_config_file.name
            everest_config["active_modules"][self.ocpp_module_id]["config_module"][
                "MessageLogPath"] = f"{TEST_LOGS_DIR}/{self.test_function_name}-{datetime.utcnow().isoformat()}"
            everest_config["active_modules"][self.ocpp_module_id]["config_module"]["CertsPath"] = self.temp_ocpp_certs_dir.name
            if everest_config["active_modules"][self.ocpp_module_id]["module"] == "OCPP":
                everest_config["active_modules"][self.ocpp_module_id]["config_module"][
                    "UserConfigPath"] = self.temp_ocpp_user_config_file.name
                everest_config["active_modules"][self.ocpp_module_id]["config_module"]["DatabasePath"] = self.temp_ocpp_database_dir.name
            elif everest_config["active_modules"][self.ocpp_module_id]["module"] == "OCPP201":
                everest_config["active_modules"][self.ocpp_module_id]["config_module"]["CoreDatabasePath"] = self.temp_ocpp_database_dir.name
                everest_config["active_modules"][self.ocpp_module_id]["config_module"][
                    "DeviceModelDatabasePath"] = f"{self.temp_ocpp_database_dir.name}/device_model_storage.db"
                os.system(
                    f"python3 {str(self.libocpp_path)}/config/v201/init_device_model_db.py --out {self.temp_ocpp_database_dir.name}/device_model_storage.db --config_path {str(self.libocpp_path)}/config/v201")
                os.system(
                    f"python3 {str(self.libocpp_path)}/config/v201/insert_device_model_config.py --config {self.temp_ocpp_config_file.name} --db {str(self.temp_ocpp_database_dir.name)}/device_model_storage.db")

        self.everest_core.temp_everest_config_file.seek(0)
        yaml.dump(everest_config, self.everest_core.temp_everest_config_file)

        # install default certificates
        certs_dir = self.everest_core.etc_path / 'certs'

        shutil.copytree(
            f"{certs_dir}/ca", f"{self.temp_ocpp_certs_dir.name}/ca", dirs_exist_ok=True)
        shutil.copytree(
            f"{certs_dir}/client", f"{self.temp_ocpp_certs_dir.name}/client", dirs_exist_ok=True)

        logging.info(f"temp ocpp config: {self.temp_ocpp_config_file.name}")
        logging.info(
            f"temp ocpp user config: {self.temp_ocpp_user_config_file.name}")
        logging.info(f"temp ocpp certs path: {self.temp_ocpp_certs_dir.name}")

        modules_to_test = None
        if standalone_module == 'probe_module':
            modules_to_test = [TestControlModuleConnection(
                evse_manager_id="connector_1", car_simulator_id="car_simulator", ocpp_id="ocpp")]
        self.everest_core.start(
            standalone_module=standalone_module, modules_to_test=modules_to_test)
        self.mqtt_external_prefix = self.everest_core.mqtt_external_prefix

        mqtt_server_uri = os.environ.get("MQTT_SERVER_ADDRESS", "127.0.0.1")
        mqtt_server_port = int(os.environ.get("MQTT_SERVER_PORT", "1883"))

        self.mqtt_client = mqtt_client.Client(self.everest_core.everest_uuid)
        self.mqtt_client.connect(mqtt_server_uri, mqtt_server_port)

        self.mqtt_client.publish(
            f"{self.mqtt_external_prefix}everest_external/nodered/1/carsim/cmd/enable", "true")
        self.mqtt_client.publish(
            f"{self.mqtt_external_prefix}everest_external/nodered/2/carsim/cmd/enable", "true")

    def stop(self):
        self.everest_core.stop()

    def plug_in(self, connector_id=1):
        self.mqtt_client.publish(f"{self.mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/cmd/execute_charging_session",
                                 "sleep 1;iec_wait_pwr_ready;sleep 1;draw_power_regulated 32,1;sleep 200;unplug")

    def plug_in_ac_iso(self, payment_type='contract', connector_id=1):
        self.mqtt_client.publish(f"{self.mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/cmd/execute_charging_session",
                                 f"sleep 1;iso_wait_slac_matched;iso_start_v2g_session {payment_type},AC_three_phase_core;iso_wait_pwr_ready;iso_draw_power_regulated 16,3;sleep 20;iso_stop_charging;iso_wait_v2g_session_stopped;unplug")

    def plug_out(self, connector_id=1):
        self.mqtt_client.publish(f"{self.mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/cmd/modify_charging_session",
                                 "unplug")

    def swipe(self, token, connectors=[1]):
        provided_token = {
            "id_token": token,
            "authorization_type": "RFID",
            "connectors": connectors
        }
        self.mqtt_client.publish(
            f"{self.mqtt_external_prefix}everest_api/dummy_token_provider/cmd/provide", json.dumps(provided_token))

    def connect_websocket(self):
        self.mqtt_client.publish(
            f"{self.mqtt_external_prefix}everest_api/ocpp/cmd/connect", "on")

    def disconnect_websocket(self):
        self.mqtt_client.publish(
            f"{self.mqtt_external_prefix}everest_api/ocpp/cmd/disconnect", "off")

    def rcd_error(self, connector_id=1):
        self.mqtt_client.publish(f"{self.mqtt_external_prefix}everest_external/nodered/{connector_id}/carsim/cmd/execute_charging_session",
                                 "sleep 1;rcd_current 10.3;sleep 10;rcd_current 0.1;unplug")

    def publish(self, topic, payload):
        self.mqtt_client.publish(topic, payload)

    def copy_occp_config(self):
        ocpp_dir = self.everest_core.prefix_path / "share/everest/modules/OCPP"
        dest_file = os.path.join(ocpp_dir, self.config_path.name)
        shutil.copy(self.config_path, dest_file)
