# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

# noinspection PyUnresolvedReferences
from everest.testing.core_utils.fixtures import *
from everest.testing.core_utils.probe_module import ProbeModule
# noinspection PyUnresolvedReferences
from everest.testing.ocpp_utils.fixtures import ocpp_config, ocpp_version, charge_point, charge_point_v201, \
    central_system, central_system_v16, central_system_v201, \
    central_system_v16_standalone, test_utility

import test_sets.everest_test_utils as everest_test_utils

from typing import Any, Callable

import logging


def pytest_addoption(parser):
    parser.addoption("--everest-prefix", action="store", default="~/checkout/everest-workspace/everest-core",
                     help="everest-core path; default = '~/checkout/everest-workspace/everest-core'")


def pytest_sessionfinish(session, exitstatus):
    pass


@pytest.fixture
def test_config(request):
    return everest_test_utils.test_config(request)


@pytest.fixture
def core_config(request) -> EverestEnvironmentCoreConfiguration:
    everest_prefix = Path(request.config.getoption("--everest-prefix"))

    marker = request.node.get_closest_marker("everest_core_config")

    if marker is None:
        test_function_name = request.function.__name__
        test_module_name = request.module.__name__
        everest_config_path = everest_test_utils.get_everest_config(test_function_name, test_module_name)
    else:
        everest_config_path = Path(__file__).parent / "test_sets/everest-aux/config" / marker.args[0]

    return EverestEnvironmentCoreConfiguration(
        everest_core_path=everest_prefix,
        template_everest_config_path=everest_config_path,
    )


@pytest.fixture
def started_test_controller(test_controller):
    test_controller.start()
    yield test_controller
    test_controller.stop()


@pytest.fixture
def skip_implementation():
    return None


@pytest.fixture
def overwrite_implementation():
    return None


def implement_command(module: ProbeModule, skip_implementation: dict, implementation_id: str, command_name: str, handler: Callable[[dict], Any]):
    skip = False
    if skip_implementation:
        if implementation_id in skip_implementation:
            to_skip = skip_implementation[implementation_id]
            if command_name in to_skip:
                logging.info(f"Skipping implementation of {command_name}")
                skip = True
    if not skip:
        module.implement_command(implementation_id, command_name, handler)


@pytest.fixture
def probe_module(started_test_controller, everest_core, skip_implementation) -> ProbeModule:
    # initiate the probe module, connecting to the same runtime session the test controller started
    module = ProbeModule(everest_core.get_runtime_session())

    logging.info(f"hello: {skip_implementation}")

    # implement necessary commands for initialization in the module
    implement_command(module, skip_implementation, "ProbeModuleConnectorA", "get_evse",
                             lambda arg: {"id": 1, "connectors": [{"id": 1}]})
    implement_command(module, skip_implementation, "ProbeModuleConnectorA", "enable_disable",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleConnectorA", "authorize_response",
                             lambda arg: None)
    implement_command(module, skip_implementation, "ProbeModuleConnectorA", "withdraw_authorization",
                             lambda arg: None)
    implement_command(module, skip_implementation, "ProbeModuleConnectorA", "reserve",
                             lambda arg: False)
    implement_command(module, skip_implementation, "ProbeModuleConnectorA", "cancel_reservation",
                             lambda arg: None)
    implement_command(module, skip_implementation, "ProbeModuleConnectorA", "set_faulted",
                             lambda arg: None)
    implement_command(module, skip_implementation, "ProbeModuleConnectorA", "pause_charging",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleConnectorA", "resume_charging",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleConnectorA", "stop_transaction",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleConnectorA", "force_unlock",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleConnectorA", "set_get_certificate_response",
                             lambda arg: None)
    implement_command(module, skip_implementation, "ProbeModuleConnectorA", "external_ready_to_start_charging",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleConnectorB", "get_evse",
                             lambda arg: {"id": 2, "connectors": [{"id": 1}]})
    implement_command(module, skip_implementation, "ProbeModuleConnectorB", "enable_disable",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleConnectorB", "authorize_response",
                             lambda arg: None)
    implement_command(module, skip_implementation, "ProbeModuleConnectorB", "withdraw_authorization",
                             lambda arg: None)
    implement_command(module, skip_implementation, "ProbeModuleConnectorB", "reserve",
                             lambda arg: False)
    implement_command(module, skip_implementation, "ProbeModuleConnectorB", "cancel_reservation",
                             lambda arg: None)
    implement_command(module, skip_implementation, "ProbeModuleConnectorB", "set_faulted",
                             lambda arg: None)
    implement_command(module, skip_implementation, "ProbeModuleConnectorB", "pause_charging",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleConnectorB", "resume_charging",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleConnectorB", "stop_transaction",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleConnectorB", "force_unlock",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleConnectorB", "set_get_certificate_response",
                             lambda arg: None)
    implement_command(module, skip_implementation, "ProbeModuleConnectorB", "external_ready_to_start_charging",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleSystem", "get_boot_reason",
                             lambda arg: "PowerUp")
    implement_command(module, skip_implementation, "ProbeModuleSystem", "update_firmware",
                             lambda arg: "Accepted")
    implement_command(module, skip_implementation, "ProbeModuleSystem", "allow_firmware_installation",
                             lambda arg: None)
    implement_command(module, skip_implementation, "ProbeModuleSystem", "upload_logs",
                             lambda arg: "Accepted")
    implement_command(module, skip_implementation, "ProbeModuleSystem", "is_reset_allowed",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleSystem", "reset",
                             lambda arg: None)
    implement_command(module, skip_implementation, "ProbeModuleSystem", "set_system_time",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "get_leaf_expiry_days_count",
                             lambda arg: 42)
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "get_v2g_ocsp_request_data",
                             lambda arg: {"ocsp_request_data_list": []})
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "get_mo_ocsp_request_data",
                             lambda arg: {"ocsp_request_data_list": []})
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "install_ca_certificate",
                             lambda arg: "Accepted")
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "delete_certificate",
                             lambda arg: "Accepted")
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "update_leaf_certificate",
                             lambda arg: "Accepted")
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "verify_certificate",
                             lambda arg: "Valid")
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "get_installed_certificates",
                             lambda arg: {"status": "Accepted", "certificate_hash_data_chain": []})
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "update_ocsp_cache",
                             lambda arg: None)
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "is_ca_certificate_installed",
                             lambda arg: False)
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "generate_certificate_signing_request",
                             lambda arg: {"status": "Accepted"})
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "get_leaf_certificate_info",
                             lambda arg: {"status": "Accepted"})
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "get_verify_file",
                             lambda arg: "")
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "verify_file_signature",
                             lambda arg: True)
    implement_command(module, skip_implementation, "ProbeModuleSecurity", "get_all_valid_certificates_info",
                             lambda arg: {"status": "NotFound", "info": []})

    return module


@pytest.fixture()
def ocpp_config_reader(ocpp_config, ocpp_configuration):
    """
        Returns a reader over the final OCPP config (after all adaptations during test setup) for convenience.
    """
    return everest_test_utils.OCPPConfigReader(ocpp_configuration)
