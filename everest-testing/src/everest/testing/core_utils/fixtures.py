# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
from pathlib import Path
from typing import Optional

import pytest
import os
import paho.mqtt.client as mqtt

from ._configuration.everest_configuration_strategies.everest_configuration_strategy import \
    EverestConfigAdjustmentStrategy
from ._configuration.everest_environment_setup import \
    EverestEnvironmentProbeModuleConfiguration, \
    EverestTestEnvironmentSetup, EverestEnvironmentOCPPConfiguration, EverestEnvironmentCoreConfiguration, \
    EverestEnvironmentEvseSecurityConfiguration, EverestEnvironmentPersistentStoreConfiguration
from everest.testing.core_utils.controller.everest_test_controller import EverestTestController
from everest.testing.core_utils.everest_core import EverestCore


@pytest.fixture
def probe_module_config(request) -> Optional[EverestEnvironmentProbeModuleConfiguration]:
    marker = request.node.get_closest_marker("probe_module")
    if marker:
        return EverestEnvironmentProbeModuleConfiguration(
            **marker.kwargs
        )

    return None


@pytest.fixture
def core_config(request) -> EverestEnvironmentCoreConfiguration:
    everest_prefix = Path(request.config.getoption("--everest-prefix"))

    marker = request.node.get_closest_marker("everest_core_config")
    if marker is None:
        everest_config_path = None  # config auto-detected by everest core
    else:
        path = Path('/etc/everest') if everest_prefix == '/usr' else everest_prefix / 'etc/everest'
        everest_config_path = path / marker.args[0]

    return EverestEnvironmentCoreConfiguration(
        everest_core_path=everest_prefix,
        template_everest_config_path=everest_config_path,
    )


@pytest.fixture
def ocpp_config(request) -> Optional[EverestEnvironmentOCPPConfiguration]:
    return None

@pytest.fixture
def evse_security_config(request) -> Optional[EverestEnvironmentEvseSecurityConfiguration]:
    source_certs_dir_marker = request.node.get_closest_marker("source_certs_dir")
    if source_certs_dir_marker:
        return EverestEnvironmentEvseSecurityConfiguration(source_certificate_directory=Path(source_certs_dir_marker.args[0]))
    return None

@pytest.fixture
def persistent_store_config(request) -> Optional[EverestEnvironmentPersistentStoreConfiguration]:
    persistent_store_marker = request.node.get_closest_marker("use_temporary_persistent_store")
    if persistent_store_marker:
        return EverestEnvironmentPersistentStoreConfiguration(use_temporary_folder=True)
    return None


@pytest.fixture
def everest_config_strategies(request) -> list[EverestConfigAdjustmentStrategy]:
    additional_configuration_strategies = []
    additional_configuration_strategies_marker = request.node.get_closest_marker('everest_config_adaptions')
    if additional_configuration_strategies_marker:
        for v in additional_configuration_strategies_marker.args:
            assert isinstance(v, EverestConfigAdjustmentStrategy), "Arguments to 'everest_config_adaptions' must all be instances of EverestConfigAdjustmentStrategy"
            additional_configuration_strategies.append(v)
    return additional_configuration_strategies


@pytest.fixture
def everest_environment(request,
                 tmp_path,
                 core_config: EverestEnvironmentCoreConfiguration,
                 ocpp_config: Optional[EverestEnvironmentOCPPConfiguration],
                 probe_module_config: Optional[EverestEnvironmentProbeModuleConfiguration],
                 evse_security_config: Optional[EverestEnvironmentEvseSecurityConfiguration],
                 persistent_store_config: Optional[EverestEnvironmentPersistentStoreConfiguration],
                 everest_config_strategies
                 ):
    standalone_module_marker = request.node.get_closest_marker('standalone_module')

    environment_setup = EverestTestEnvironmentSetup(
        core_config=core_config,
        ocpp_config=ocpp_config,
        probe_config=probe_module_config,
        evse_security_config=evse_security_config,
        persistent_store_config=persistent_store_config,
        standalone_module=list(standalone_module_marker.args) if standalone_module_marker else None,
        everest_config_strategies=everest_config_strategies
    )

    environment_setup.setup_environment(tmp_path=tmp_path)

    yield environment_setup

@pytest.fixture
def everest_core(request,
                 everest_environment
                 )-> EverestCore:
    """Fixture that can be used to start and stop everest-core"""

    yield everest_environment.everest_core

    # FIXME (aw): proper life time management, shouldn't the fixure start and stop?
    everest_environment.everest_core.stop()

@pytest.fixture
def ocpp_configuration(everest_environment):
    yield everest_environment.ocpp_config

@pytest.fixture
def test_controller(request, tmp_path, everest_core) -> EverestTestController:
    """Fixture that references the test_controller that can be used for
    control events for the test cases.
    """

    test_controller = EverestTestController(everest_core=everest_core)

    yield test_controller

    # FIXME (aw): proper life time management, shouldn't the fixure start and stop?
    test_controller.stop()

@pytest.fixture
def connected_mqtt_client(everest_core: EverestCore) -> mqtt.Client:
    mqtt_server_uri = os.environ.get("MQTT_SERVER_ADDRESS", "127.0.0.1")
    mqtt_server_port = int(os.environ.get("MQTT_SERVER_PORT", "1883"))
    client = mqtt.Client(everest_core.everest_uuid)
    client.connect(mqtt_server_uri, mqtt_server_port)
    client.loop_start()

    yield client

    client.loop_stop()
