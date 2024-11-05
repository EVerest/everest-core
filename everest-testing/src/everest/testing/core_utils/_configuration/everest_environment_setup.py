# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
from __future__ import annotations

import logging
import shutil
from dataclasses import dataclass, field
from pathlib import Path
from typing import Optional, Dict, List, Union

import yaml

from everest.testing.core_utils.common import OCPPVersion
from everest.testing.core_utils.everest_core import EverestCore, Requirement
from .everest_configuration_strategies.everest_configuration_strategy import \
    EverestConfigAdjustmentStrategy
from .everest_configuration_strategies.evse_security_configuration_strategy import \
    EvseSecurityModuleConfigurationStrategy, EvseSecurityModuleConfiguration
from .everest_configuration_strategies.ocpp_module_configuration_strategy import \
    OCPPModuleConfigurationStrategy, \
    OCPPModulePaths16, OCPPModulePaths201
from .everest_configuration_strategies.persistent_store_configuration_strategy import \
    PersistentStoreConfigurationStrategy
from .everest_configuration_strategies.probe_module_configuration_strategy import \
    ProbeModuleConfigurationStrategy
from .libocpp_configuration_helper import \
    LibOCPP201ConfigurationHelper, LibOCPP16ConfigurationHelper


@dataclass
class EverestEnvironmentOCPPConfiguration:
    libocpp_path: Path
    ocpp_version: OCPPVersion
    central_system_port: int
    central_system_host: str = "127.0.0.1"
    ocpp_module_id: str = "ocpp"
    template_ocpp_config: Optional[
        Path] = None  # Path for OCPP config to be used; if not provided, will be determined from everest config
    device_model_component_config_path: Optional[
        Path] = None  # Path of the OCPP device model json schemas. If not set, {libocpp_path} / 'config/v201/component_config' will  be used
    configuration_strategies: list[OCPPModuleConfigurationStrategy] | None = None


@dataclass
class EverestEnvironmentEvseSecurityConfiguration:
    use_temporary_certificates_folder: bool = True  # if true, configuration will be adapted to use temporary certifcates folder, this assumes a "default" file tree structure, cf. the EvseSecurityModuleConfiguration dataclass
    module_id: Optional[str] = None  # if None, auto-detected
    source_certificate_directory: Optional[
        Path] = None  # if provided, this will be copied to temporary path; If none, the certificates of the everest-core directory / installation will be used
    module_configuration: Optional[
        EvseSecurityModuleConfiguration] = None  # if provided, will be merged into configuration; paths will be adapted if use_temporary_certificates_folder is true


@dataclass
class EverestEnvironmentPersistentStoreConfiguration:
    use_temporary_folder: bool = True  # if true, a temporary persistent storage folder will be used


@dataclass
class EverestEnvironmentCoreConfiguration:
    everest_core_path: Path
    template_everest_config_path: Union[
        Path, None]  # Underlying EVerest configuration; will be copied temporarily by EverestCore and adjusted;
    # if none, config is auto-detected by everest-core


@dataclass
class EverestEnvironmentProbeModuleConfiguration:
    connections: Dict[str, List[Requirement]] = field(default_factory=dict)
    module_id: str = "probe"


class EverestTestEnvironmentSetup:
    """
     Class that prepares the environment of EVerest core and creates the EverestCore instance of a test.

     For this:
     - receives all settings / configurations required at initialization
     - calling setup_environment:
        - creates required temporary paths / file structures
        - configures EverestCore including adjustments of the EverestCore configuration (injecting temporary paths ...)
        - sets up special modules (initiates the setup of OCPPlib such as parsing the device model database)
        - creates the EverestCore instance

    """

    @dataclass
    class _EverestEnvironmentTemporaryPaths:
        """ Paths of the temporary configuration files / data """
        certs_dir: Path  # used by both OCPP and evse security
        ocpp_config_path: Path
        ocpp_config_file: Path
        ocpp_user_config_file: Path
        ocpp_database_dir: Path
        ocpp_message_log_directory: Path
        persistent_store_db_path: Path

    def __init__(self,
                 core_config: EverestEnvironmentCoreConfiguration,
                 ocpp_config: Optional[EverestEnvironmentOCPPConfiguration] = None,
                 probe_config: Optional[EverestEnvironmentProbeModuleConfiguration] = None,
                 evse_security_config: Optional[EverestEnvironmentEvseSecurityConfiguration] = None,
                 persistent_store_config: Optional[EverestEnvironmentPersistentStoreConfiguration] = None,
                 standalone_module: Optional[Union[str, List[str]]] = None,
                 everest_config_strategies: Optional[List[EverestConfigAdjustmentStrategy]] = None
                 ) -> None:
        self._core_config = core_config
        self._ocpp_config = ocpp_config
        self._probe_config = probe_config
        self._evse_security_config = evse_security_config
        self._persistent_store_config = persistent_store_config
        self._standalone_module = standalone_module
        if not self._standalone_module and self._probe_config:
            self._standalone_module = self._probe_config.module_id
        self._additional_everest_config_strategies = everest_config_strategies if everest_config_strategies else []
        self._everest_core = None
        self._ocpp_configuration = None

    def setup_environment(self, tmp_path: Path):

        temporary_paths = self._create_temporary_directory_structure(tmp_path)

        configuration_strategies = self._create_everest_configuration_strategies(temporary_paths)

        self._everest_core = EverestCore(self._core_config.everest_core_path,
                                         self._core_config.template_everest_config_path,
                                         everest_configuration_adjustment_strategies=configuration_strategies + self._additional_everest_config_strategies,
                                         standalone_module=self._standalone_module,
                                         tmp_path=tmp_path)

        if self._ocpp_config:
            self._ocpp_configuration = self._setup_libocpp_configuration(
                temporary_paths=temporary_paths
            )

        if self._evse_security_config:
            self._setup_evse_security_configuration(temporary_paths)

    @property
    def everest_core(self) -> EverestCore:
        assert self._everest_core, "Everest Core not initialized; run 'setup_environment' first"
        return self._everest_core

    @property
    def ocpp_config(self):
        return self._ocpp_configuration

    def _create_temporary_directory_structure(self, tmp_path: Path) -> _EverestEnvironmentTemporaryPaths:
        ocpp_config_dir = tmp_path / "ocpp_config"
        ocpp_config_dir.mkdir(exist_ok=True)
        if self._ocpp_config.ocpp_version == OCPPVersion.ocpp201:
            component_config_path_standardized = ocpp_config_dir / "component_config" / "standardized"
            component_config_path_custom = ocpp_config_dir / "component_config" / "custom"
            component_config_path_standardized.mkdir(parents=True, exist_ok=True)
            component_config_path_custom.mkdir(parents=True, exist_ok=True)
        certs_dir = tmp_path / "certs"
        certs_dir.mkdir(exist_ok=True)
        ocpp_logs_dir = ocpp_config_dir / "logs"
        ocpp_logs_dir.mkdir(exist_ok=True)

        persistent_store_dir = tmp_path / "persistent_storage"
        persistent_store_dir.mkdir(exist_ok=True)

        logging.info(f"temp ocpp config files directory: {ocpp_config_dir}")

        return self._EverestEnvironmentTemporaryPaths(
            ocpp_config_path=ocpp_config_dir / "component_config",
            ocpp_config_file=ocpp_config_dir / "config.json",
            ocpp_user_config_file=ocpp_config_dir / "user_config.json",
            ocpp_database_dir=ocpp_config_dir,
            certs_dir=certs_dir,
            ocpp_message_log_directory=ocpp_logs_dir,
            persistent_store_db_path=persistent_store_dir / "persistent_store.db"
        )

    def _create_ocpp_module_configuration_strategy(self,
                                                   temporary_paths: _EverestEnvironmentTemporaryPaths) -> OCPPModuleConfigurationStrategy:

        if self._ocpp_config.ocpp_version == OCPPVersion.ocpp16:
            ocpp_paths = OCPPModulePaths16(
                ChargePointConfigPath=str(temporary_paths.ocpp_config_file),
                MessageLogPath=str(temporary_paths.ocpp_message_log_directory),
                UserConfigPath=str(temporary_paths.ocpp_user_config_file),
                DatabasePath=str(temporary_paths.ocpp_database_dir)
            )
        elif self._ocpp_config.ocpp_version == OCPPVersion.ocpp201:
            ocpp_paths = OCPPModulePaths201(
                DeviceModelConfigPath=str(temporary_paths.ocpp_config_path),
                MessageLogPath=str(temporary_paths.ocpp_message_log_directory),
                CoreDatabasePath=str(temporary_paths.ocpp_database_dir),
                DeviceModelDatabasePath=str(temporary_paths.ocpp_database_dir / "device_model_storage.db"),
            )
        else:
            raise ValueError(f"unknown  ocpp version {self._ocpp_config.ocpp_version}")

        occp_module_configuration_helper = OCPPModuleConfigurationStrategy(ocpp_paths=ocpp_paths,
                                                                           ocpp_module_id=self._ocpp_config.ocpp_module_id,
                                                                           ocpp_version=self._ocpp_config.ocpp_version)

        return occp_module_configuration_helper

    def _setup_libocpp_configuration(self, temporary_paths: _EverestEnvironmentTemporaryPaths):

        liboccp_configuration_helper = LibOCPP16ConfigurationHelper() if self._ocpp_config.ocpp_version == OCPPVersion.ocpp16 else LibOCPP201ConfigurationHelper()

        if self._ocpp_config.template_ocpp_config:
            source_ocpp_config = self._ocpp_config.template_ocpp_config
        elif self._ocpp_config.ocpp_version == OCPPVersion.ocpp16:
            source_ocpp_config = self._determine_configured_charge_point_config_path_from_everest_config()
        elif self._ocpp_config.ocpp_version == OCPPVersion.ocpp201:
            source_ocpp_config = self._ocpp_config.device_model_component_config_path \
                if self._ocpp_config.device_model_component_config_path \
                else self._ocpp_config.libocpp_path / 'config/v201/component_config'

        return liboccp_configuration_helper.generate_ocpp_config(
            central_system_port=self._ocpp_config.central_system_port,
            central_system_host=self._ocpp_config.central_system_host,
            source_ocpp_config_path=source_ocpp_config,
            target_ocpp_config_path=temporary_paths.ocpp_config_file \
                if self._ocpp_config.ocpp_version == OCPPVersion.ocpp16 \
                else temporary_paths.ocpp_config_path,
            target_ocpp_user_config_file=temporary_paths.ocpp_user_config_file,
            configuration_strategies=self._ocpp_config.configuration_strategies
        )

    def _create_everest_configuration_strategies(self, temporary_paths: _EverestEnvironmentTemporaryPaths):
        configuration_strategies = []
        if self._ocpp_config:
            configuration_strategies.append(self._create_ocpp_module_configuration_strategy(temporary_paths))
        if self._probe_config:
            configuration_strategies.append(
                ProbeModuleConfigurationStrategy(connections=self._probe_config.connections,
                                                 module_id=self._probe_config.module_id))

        if self._evse_security_config:
            configuration_strategies.append(
                EvseSecurityModuleConfigurationStrategy(module_id=self._evse_security_config.module_id,
                                                        configuration=self._evse_security_config.module_configuration,
                                                        source_certificates_directory=self._evse_security_config.source_certificate_directory,
                                                        target_certificates_directory=temporary_paths.certs_dir \
                                                            if self._evse_security_config.use_temporary_certificates_folder \
                                                            else None
                                                        ))

        if self._persistent_store_config and self._persistent_store_config.use_temporary_folder:
            configuration_strategies.append(
                PersistentStoreConfigurationStrategy(sqlite_db_file_path=temporary_paths.persistent_store_db_path)
            )

        return configuration_strategies

    def _determine_configured_charge_point_config_path_from_everest_config(self):

        if self._ocpp_config.ocpp_version == OCPPVersion.ocpp16:
            everest_template_config = yaml.safe_load(self._core_config.template_everest_config_path.read_text())

            charge_point_config_path = \
                everest_template_config["active_modules"][self._ocpp_config.ocpp_module_id]["config_module"][
                "ChargePointConfigPath"]

            ocpp_dir = self._everest_core.prefix_path / "share/everest/modules/OCPP"
        else:
            raise ValueError(f"Could not determine ChargePointConfigPath for OCPP version {self._ocpp_config.ocpp_version}")
        ocpp_config_path = ocpp_dir / charge_point_config_path
        return ocpp_config_path

    def _setup_evse_security_configuration(self, temporary_paths: _EverestEnvironmentTemporaryPaths):
        """ If configures, copies the source certificate trees"""
        if self._evse_security_config.source_certificate_directory:
            source_certs_directory = self._evse_security_config.source_certificate_directory
        else:
            source_certs_directory = self._everest_core.etc_path / 'certs'
            logging.warning(
                "No 'source_certificate_directory' configured in EverestEnvironmentEvseSecurityConfiguration. "
                f"Will use certificates from local installation {source_certs_directory}', which might lead to flaky tests.")
        shutil.copytree(source_certs_directory, temporary_paths.certs_dir, dirs_exist_ok=True)
