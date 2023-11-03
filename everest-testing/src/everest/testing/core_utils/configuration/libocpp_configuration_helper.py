import json
import shutil
import subprocess
from abc import ABC, abstractmethod
from pathlib import Path
from typing import Union


class LibOCPPConfigurationHelperBase(ABC):
    """ Helper for parsing / adapting the LibOCPP configuration and dumping it a database file. """

    def generate_ocpp_config(self,
                             target_ocpp_config_file: Path,
                             target_ocpp_user_config_file: Path,
                             source_ocpp_config_file: Path,
                             central_system_host: str,
                             central_system_port: Union[str, int]):
        config = self._get_occp_config(central_system_port=central_system_port,
                                       central_system_host=central_system_host,
                                       source_ocpp_config_file=source_ocpp_config_file)
        with target_ocpp_config_file.open("w") as f:
            json.dump(config, f)
        target_ocpp_user_config_file.write_text("{}")

    @abstractmethod
    def _get_occp_config(self, central_system_port, central_system_host, source_ocpp_config_file: Path):
        pass

    @staticmethod
    def install_default_ocpp_certificates(source_certs_directory: Path, target_certs_directory: Path, ):
        # install default certificates
        shutil.copytree(
            str(source_certs_directory / "ca"), str(target_certs_directory / "ca"), dirs_exist_ok=True)
        shutil.copytree(
            str(source_certs_directory / "client"), str(target_certs_directory / "client"), dirs_exist_ok=True)


class LibOCPP16ConfigurationHelper(LibOCPPConfigurationHelperBase):

    def _get_occp_config(self,  central_system_port, central_system_host, source_ocpp_config_file: Path):
        ocpp_config = json.loads(source_ocpp_config_file.read_text())
        charge_point_id = ocpp_config["Internal"]["ChargePointId"]
        ocpp_config["Internal"]["CentralSystemURI"] = f"{central_system_host}:{central_system_port}/{charge_point_id}"
        return ocpp_config


class LibOCPP201ConfigurationHelper(LibOCPPConfigurationHelperBase):



    @staticmethod
    def create_temporary_ocpp_configuration_db(libocpp_path: Path,
                                               device_model_schemas_path: Path,
                                               ocpp_configuration_file: Path,
                                               target_directory: Path):
        wd = libocpp_path / "config/v201"
        subprocess.run(
            f"python3 {libocpp_path / 'config/v201/init_device_model_db.py'} --out {target_directory / 'device_model_storage.db'} --schemas {device_model_schemas_path}",
            cwd=wd,
            check=True,
            shell=True
        )
        subprocess.run(
            f"python3 {libocpp_path / 'config/v201/insert_device_model_config.py'} --config {ocpp_configuration_file} --db {target_directory / 'device_model_storage.db'}",
            cwd=wd,
            check=True,
            shell=True
        )

    def _get_occp_config(self, central_system_port, central_system_host,  source_ocpp_config_file: Path):
        ocpp_config = json.loads(source_ocpp_config_file.read_text())
        charge_point_id = self._get_value_from_v201_config(
            ocpp_config, "InternalCtrlr", "ChargePointId", "Actual")
        network_connection_profiles = json.loads(self._get_value_from_v201_config(
            ocpp_config, "InternalCtrlr", "NetworkConnectionProfiles", "Actual"))
        network_connection_profiles[0]["connectionData"][
            "ocppCsmsUrl"] = f"ws://{central_system_host}:{central_system_port}/{charge_point_id}"
        self._set_value_in_v201_config(ocpp_config, "InternalCtrlr", "NetworkConnectionProfiles",
                                       "Actual", json.dumps(network_connection_profiles))
        return ocpp_config

    @staticmethod
    def _get_value_from_v201_config(ocpp_config: json, component_name: str, variable_name: str,
                                    variable_attribute_type: str):
        for component in ocpp_config:
            if (component["name"] == component_name):
                return component["variables"][variable_name]["attributes"][variable_attribute_type]

    @staticmethod
    def _set_value_in_v201_config(ocpp_config: json, component_name: str, variable_name: str,
                                  variable_attribute_type: str,
                                  value: str):
        for component in ocpp_config:
            if (component["name"] == component_name):
                component["variables"][variable_name]["attributes"][variable_attribute_type] = value
                return
