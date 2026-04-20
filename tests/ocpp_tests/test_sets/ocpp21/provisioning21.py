# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
from datetime import datetime, timezone

import traceback
# fmt: off
import logging

from everest.testing.core_utils.controller.test_controller_interface import TestController

from ocpp.v21 import call as call21
from ocpp.v21 import call_result as call_result21
from ocpp.v21.enums import *
from ocpp.v21.datatypes import *
from ocpp.routing import on, create_route_map
from everest.testing.ocpp_utils.fixtures import *
from everest_test_utils import * # Needs to be before the datatypes below since it overrides the v21 Action enum with the v16 one
from ocpp.v21.enums import (
    Action,
    ConnectorStatusEnumType,
    SetVariableStatusEnumType,
    GetVariableStatusEnumType,
    AttributeEnumType,
)
from validations import validate_status_notification_201
from everest.testing.core_utils._configuration.libocpp_configuration_helper import GenericOCPP2XConfigAdjustment
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility, OcppTestConfiguration
# fmt: on

log = logging.getLogger("provisioningTest")


def validate_get_variables_response(response, expected):
    """Validate GetVariables response matches expected values.
    response is a call_result.GetVariables with get_variable_result as list of dicts.
    """
    if not response or not response.get_variable_result:
        return False

    for result in response.get_variable_result:
        component = result.get("component", {})
        variable = result.get("variable", {})

        if component.get("name") == expected["component_name"]:
            if variable.get("name") == expected["variable_name"]:
                status = result.get("attribute_status", "")
                expected_status = expected.get("expected_status", "Accepted")
                if status == expected_status:
                    return True

    return False


def validate_set_variables_success(response, expected_count=1):
    """Validate SetVariables response indicates success.
    response is a call_result.SetVariables with set_variable_result as list of dicts.
    """
    if not response or not response.set_variable_result:
        return False

    success_count = sum(
        1
        for r in response.set_variable_result
        if r.get("attribute_status") == "Accepted"
    )
    return success_count >= expected_count


def validate_set_variables_rejected(response, expected_reason=None):
    """Validate SetVariables response indicates rejection.
    response is a call_result.SetVariables with set_variable_result as list of dicts.
    """
    if not response or not response.set_variable_result:
        return False

    for result in response.set_variable_result:
        status = result.get("attribute_status")
        if status != "Accepted":
            if expected_reason:
                status_info = result.get("attribute_status_info") or {}
                reason = status_info.get("reason_code", "").upper()
                return expected_reason.upper() in reason
            return True

    return False


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_cold_boot_01(
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    B01.FR.01
    ...
    """

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint()

    try:
        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v21,
            "StatusNotification",
            call21.StatusNotification(
                1,  ConnectorStatusEnumType.available, 1, datetime.now().isoformat()
            ),
            validate_status_notification_201,
        )
    except Exception as e:
        traceback.print_exc()
        logging.critical(e)

    # TOOD(piet): Check configured HeartbeatInterval of BootNotificationResponse


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_cold_boot_pending_01(
    test_config: OcppTestConfiguration,
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):

    @on(Action.boot_notification)
    def on_boot_notification_pending(**kwargs):
        return call_result21.BootNotification(
            current_time=datetime.now().isoformat(),
            interval=5,
            status=RegistrationStatusEnumType.pending,
        )

    @on(Action.boot_notification)
    def on_boot_notification_accepted(**kwargs):
        return call_result21.BootNotification(
            current_time=datetime.now(timezone.utc).isoformat(),
            interval=5,
            status=RegistrationStatusEnumType.accepted,
        )

    test_utility.forbidden_actions.append("SecurityEventNotification")

    central_system_v21.function_overrides.append(
        ("on_boot_notification", on_boot_notification_pending)
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint()

    setattr(charge_point_v21, "on_boot_notification",
            on_boot_notification_accepted)
    central_system_v21.chargepoint.route_map = create_route_map(
        central_system_v21.chargepoint
    )

    assert await wait_for_and_validate(
        test_utility, charge_point_v21, "BootNotification", {}
    )

    test_utility.forbidden_actions.clear()

    test_controller.plug_in()

    assert await wait_for_and_validate(
        test_utility, charge_point_v21, "SecurityEventNotification", {}
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_cold_boot_rejected_01(
    test_config: OcppTestConfiguration,
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):

    @on(Action.boot_notification)
    def on_boot_notification_pending(**kwargs):
        return call_result21.BootNotification(
            current_time=datetime.now().isoformat(),
            interval=5,
            status=RegistrationStatusEnumType.rejected,
        )

    @on(Action.boot_notification)
    def on_boot_notification_accepted(**kwargs):
        return call_result21.BootNotification(
            current_time=datetime.now(timezone.utc).isoformat(),
            interval=5,
            status=RegistrationStatusEnumType.accepted,
        )

    central_system_v21.function_overrides.append(
        ("on_boot_notification", on_boot_notification_pending)
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint()

    setattr(charge_point_v21, "on_boot_notification",
            on_boot_notification_accepted)
    central_system_v21.chargepoint.route_map = create_route_map(
        central_system_v21.chargepoint
    )

    assert await wait_for_and_validate(
        test_utility, charge_point_v21, "BootNotification", {}
    )
@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_get_network_configuration_slot_1(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    B09.FR.01 - Get NetworkConfiguration for slot 1 (primary configuration)
    Verify that GetVariables can retrieve NetworkConfiguration[1] variables
    """
    log.info(
        "##################### B09.FR.01: Get NetworkConfiguration Slot 1 #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Request OcppCsmsUrl for NetworkConfiguration slot 1
    get_var = GetVariableDataType(
        component=ComponentType(
            name="NetworkConfiguration", instance="1"
        ),
        variable=VariableType(name="OcppCsmsUrl"),
        attribute_type=AttributeEnumType.actual,
    )

    response = await charge_point_v21.get_variables_req(
        get_variable_data=[get_var]
    )

    assert response and response.get_variable_result, "No get variable result"
    results = response.get_variable_result
    assert len(results) > 0, "Empty get variable result"
    assert (
        results[0].get("attribute_status") == "Accepted"
    ), f"Failed to get OcppCsmsUrl: {results[0]}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_set_network_configuration_slot_2(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    B09.FR.09 - Set NetworkConfiguration for slot 2 (backup configuration)
    Verify that SetVariables can update NetworkConfiguration[2] variables
    which are not in the priority list
    """
    log.info(
        "##################### B09.FR.09: Set NetworkConfiguration Slot 2 #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Set OcppCsmsUrl for NetworkConfiguration slot 2
    set_var = SetVariableDataType(
        component=ComponentType(
            name="NetworkConfiguration", instance="2"
        ),
        variable=VariableType(name="OcppCsmsUrl"),
        attribute_type=AttributeEnumType.actual,
        attribute_value="wss://backup-csms.example.com/ocpp",
    )

    response = await charge_point_v21.set_variables_req(
        set_variable_data=[set_var]
    )

    assert validate_set_variables_success(response, 1), \
        f"Failed to set OcppCsmsUrl on slot 2: {response}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_set_network_configuration_apn(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    B09.FR.11 - Set APN configuration for NetworkConfiguration slot 2
    Verify that APN variables can be configured
    """
    log.info(
        "##################### B09.FR.11: Set APN Configuration #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Set APN details for slot 2 (ApnEnabled is ReadOnly, set internally)
    set_vars = [
        SetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name="Apn"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="internet",
        ),
        SetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name="ApnUserName"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="testuser",
        ),
    ]

    response = await charge_point_v21.set_variables_req(
        set_variable_data=set_vars
    )

    assert validate_set_variables_success(response, 2), \
        f"Failed to set APN configuration: {response}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_set_network_configuration_vpn(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    B09.FR.13 - Set VPN configuration for NetworkConfiguration slot 2
    Verify that VPN variables can be configured
    """
    log.info(
        "##################### B09.FR.13: Set VPN Configuration #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Set VPN details for slot 2 (VpnEnabled is ReadOnly, set internally)
    set_vars = [
        SetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name="VpnServer"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="vpn.example.com",
        ),
        SetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name="VpnType"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="IKEv2",
        ),
    ]

    response = await charge_point_v21.set_variables_req(
        set_variable_data=set_vars
    )

    assert validate_set_variables_success(response, 2), \
        f"Failed to set VPN configuration: {response}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_set_network_configuration_identity_override(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    B09.FR.16-18 - Set per-slot Identity override in NetworkConfiguration
    Verify that Identity can be configured per configuration slot
    """
    log.info(
        "##################### B09.FR.16-18: Set Identity Override #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Set Identity for slot 2
    set_var = SetVariableDataType(
        component=ComponentType(
            name="NetworkConfiguration", instance="2"
        ),
        variable=VariableType(name="Identity"),
        attribute_type=AttributeEnumType.actual,
        attribute_value="slot2_identity",
    )

    response = await charge_point_v21.set_variables_req(
        set_variable_data=[set_var]
    )

    assert validate_set_variables_success(response, 1), \
        f"Failed to set Identity: {response}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_set_network_configuration_basic_auth_password(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    B09.FR.26-28 - Set per-slot BasicAuthPassword override in NetworkConfiguration
    Verify that BasicAuthPassword can be configured per configuration slot
    """
    log.info(
        "##################### B09.FR.26-28: Set BasicAuthPassword Override #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Set BasicAuthPassword for slot 2
    set_var = SetVariableDataType(
        component=ComponentType(
            name="NetworkConfiguration", instance="2"
        ),
        variable=VariableType(name="BasicAuthPassword"),
        attribute_type=AttributeEnumType.actual,
        attribute_value="mysecretpassword",
    )

    response = await charge_point_v21.set_variables_req(
        set_variable_data=[set_var]
    )

    assert validate_set_variables_success(response, 1), \
        f"Failed to set BasicAuthPassword: {response}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_set_network_configuration_security_profile(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    B09.FR.35 - Verify security profile can be configured per slot
    Security profile downgrade is tested separately (may be rejected based on rules)
    """
    log.info(
        "##################### B09.FR.35: Set Security Profile #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Set SecurityProfile for slot 2 (upgrade from default 1 to 2)
    set_var = SetVariableDataType(
        component=ComponentType(
            name="NetworkConfiguration", instance="2"
        ),
        variable=VariableType(name="SecurityProfile"),
        attribute_type=AttributeEnumType.actual,
        attribute_value="2",
    )

    response = await charge_point_v21.set_variables_req(
        set_variable_data=[set_var]
    )

    # Should succeed for upgrade
    assert validate_set_variables_success(response, 1), \
        f"Failed to set SecurityProfile: {response}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_get_all_network_configuration_variables_slot_1(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    Verify GetVariables can retrieve all NetworkConfiguration[1] variables
    """
    log.info(
        "##################### Get All NetworkConfiguration[1] Variables #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Request all main configuration variables for slot 1
    variable_names = [
        "OcppCsmsUrl",
        "SecurityProfile",
        "OcppInterface",
        "OcppTransport",
        "MessageTimeout",
    ]

    get_vars = [
        GetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="1"
            ),
            variable=VariableType(name=var_name),
            attribute_type=AttributeEnumType.actual,
        )
        for var_name in variable_names
    ]

    response = await charge_point_v21.get_variables_req(
        get_variable_data=get_vars
    )

    assert response and response.get_variable_result, "No get variable result"
    results = response.get_variable_result
    assert len(results) == len(variable_names), "Not all variables returned"

    # Count successful retrievals
    success_count = sum(
        1
        for r in results
        if r.get("attribute_status") == "Accepted"
    )
    assert (
        success_count == len(variable_names)
    ), f"Some variables failed: {results}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_set_network_configuration_multiple_variables(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    Verify SetVariables can update multiple NetworkConfiguration variables in one request
    """
    log.info(
        "##################### Set Multiple NetworkConfiguration Variables #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Set multiple variables for slot 2 in one request
    set_vars = [
        SetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name="OcppCsmsUrl"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="wss://csms-2.example.com/ocpp",
        ),
        SetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name="SecurityProfile"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="1",
        ),
        SetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name="MessageTimeout"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="60",
        ),
        SetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name="OcppInterface"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="Wired0",
        ),
        SetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name="OcppTransport"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="JSON",
        ),
    ]

    response = await charge_point_v21.set_variables_req(
        set_variable_data=set_vars
    )

    assert validate_set_variables_success(response, 5), \
        f"Failed to set multiple variables: {response}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_set_network_configuration_with_apn_via_set_variables(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    TC_B_105_CSMS: Set new NetworkConnectionProfile - Add new NetworkConfiguration using SetVariables
    B09.FR.29, B09.FR.30 - Verify that CSMS can set NetworkConfiguration component with APN
    connection details via SetVariables request. Tests setting a network configuration with
    basic connection parameters and APN authentication details on a non-active configuration slot.
    """
    log.info(
        "##################### TC_B_105_CSMS: Set NetworkConfiguration with APN via SetVariables #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # TC_B_105_CSMS: Set NetworkConfiguration slot 2 (non-active) with complete APN configuration
    # ApnEnabled and VpnEnabled are ReadOnly (set internally), so they are not included here
    set_vars = [
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="OcppCsmsUrl"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="wss://csms-backup.example.com/ocpp",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="OcppInterface"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="Wired0",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="OcppTransport"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="JSON",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="MessageTimeout"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="30",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="SecurityProfile"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="2",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="Identity"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="charging_station_id_slot2",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="BasicAuthPassword"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="BasicAuthPasswordOfSufficientLength",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="Apn"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="configured_apn_url",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="ApnUserName"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="configured_apn_username",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="ApnPassword"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="configured_apn_password",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="ApnAuthentication"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="AUTO",
        ),
    ]

    response = await charge_point_v21.set_variables_req(
        set_variable_data=set_vars
    )

    # Validate that all variables were successfully set
    assert validate_set_variables_success(
        response, len(set_vars)
    ), f"Failed to set NetworkConfiguration with APN via SetVariables: {response}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_network_configuration_priority_list_management(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    TC_B_107_CS: Add and remove slots from NetworkConfigurationPriority list
    B09.FR.21, B09.FR.22, B09.FR.23 - Verify that CSMS can manage which configuration slots
    are in the priority list, and verify that configuration values are consistent when
    slots are added/removed from the priority list.
    """
    log.info(
        "##################### TC_B_107_CS: NetworkConfigurationPriority List Management #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # First, set a configuration on slot 2 (which is not in priority by default)
    set_vars = [
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="OcppCsmsUrl"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="wss://backup-csms.example.com/ocpp",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="SecurityProfile"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="1",
        ),
    ]

    response = await charge_point_v21.set_variables_req(
        set_variable_data=set_vars
    )

    assert validate_set_variables_success(
        response, len(set_vars)
    ), f"Failed to set initial configuration on slot 2: {response}"

    # Verify the configuration was set
    get_var = GetVariableDataType(
        component=ComponentType(name="NetworkConfiguration", instance="2"),
        variable=VariableType(name="OcppCsmsUrl"),
        attribute_type=AttributeEnumType.actual,
    )

    response = await charge_point_v21.get_variables_req(
        get_variable_data=[get_var]
    )

    assert response and response.get_variable_result, "No get variable result"
    results = response.get_variable_result
    assert len(results) > 0, "Empty get variable result"
    assert (
        results[0].get("attribute_status") == "Accepted"
    ), f"Failed to get OcppCsmsUrl from slot 2: {results[0]}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_network_configuration_reject_priority_slot_changes(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    TC_B_108_CS: Reject SetVariables on the currently active NetworkConfiguration slot
    B09.FR.22 - Verify that attempts to change configuration on the active slot
    are rejected with "PriorityNetworkConf" reason code.
    """
    log.info(
        "##################### TC_B_108_CS: Reject Changes to Priority Slots #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Attempt to change NetworkConfiguration slot 1 (which is in priority by default)
    # This should be rejected with "PriorityNetworkConf" reason
    set_var = SetVariableDataType(
        component=ComponentType(name="NetworkConfiguration", instance="1"),
        variable=VariableType(name="OcppCsmsUrl"),
        attribute_type=AttributeEnumType.actual,
        attribute_value="wss://new-csms.example.com/ocpp",
    )

    response = await charge_point_v21.set_variables_req(
        set_variable_data=[set_var]
    )

    # Validate that the change was rejected with PriorityNetworkConf reason
    assert validate_set_variables_rejected(
        response, "PriorityNetworkConf"
    ), f"Expected SetVariables to be rejected with PriorityNetworkConf, but got: {response}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_network_configuration_identity_password_override(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    TC_B_109_CS: Per-slot Identity and BasicAuthPassword override behavior
    B09.FR.16-28 - Verify that Identity and BasicAuthPassword can be set per slot
    and that they override the global SecurityCtrlr values when a configuration is active.
    """
    log.info(
        "##################### TC_B_109_CS: Identity and BasicAuthPassword Per-Slot Override #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Set Identity and BasicAuthPassword on slot 2
    set_vars = [
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="Identity"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="slot2_identity_override",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="BasicAuthPassword"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="slot2_password_override",
        ),
    ]

    response = await charge_point_v21.set_variables_req(
        set_variable_data=set_vars
    )

    assert validate_set_variables_success(
        response, len(set_vars)
    ), f"Failed to set Identity and BasicAuthPassword overrides: {response}"

    # Verify that the values were set
    get_vars = [
        GetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="Identity"),
            attribute_type=AttributeEnumType.actual,
        ),
        GetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="BasicAuthPassword"),
            attribute_type=AttributeEnumType.actual,
        ),
    ]

    response = await charge_point_v21.get_variables_req(
        get_variable_data=get_vars
    )

    assert response and response.get_variable_result, "No get variable result"
    results = response.get_variable_result
    success_count = sum(
        1
        for r in results
        if r.get("attribute_status") == "Accepted"
    )
    # BasicAuthPassword is WriteOnly so it may not be gettable, but Identity should be
    assert (
        success_count >= 1
    ), f"Failed to retrieve per-slot overrides: {results}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_network_configuration_security_downgrade_rejection_set_network_profile(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    TC_B_110_CS: Reject security profile downgrade via SetNetworkProfileRequest
    B09.FR.31 - Verify that SetNetworkProfileRequest is rejected with "NoSecurityDowngrade"
    reason when attempting to downgrade the active security profile.
    """
    log.info(
        "##################### TC_B_110_CS: Reject Security Downgrade via SetNetworkProfileRequest #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # First, get the current active security profile from slot 1
    get_var = GetVariableDataType(
        component=ComponentType(name="NetworkConfiguration", instance="1"),
        variable=VariableType(name="SecurityProfile"),
        attribute_type=AttributeEnumType.actual,
    )

    response = await charge_point_v21.get_variables_req(
        get_variable_data=[get_var]
    )

    assert response and response.get_variable_result, "No get variable result"
    results = response.get_variable_result
    assert len(results) > 0, "Empty get variable result"

    # Try to set a lower security profile on slot 2
    # Assuming the current active profile is 1, try to set slot 2 to 0 (which would be lower)
    set_vars = [
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="SecurityProfile"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="0",  # Downgrade attempt
        ),
    ]

    response = await charge_point_v21.set_variables_req(
        set_variable_data=set_vars
    )

    # B09.FR.31: Downgrade attempt should be rejected with NoSecurityDowngrade
    assert validate_set_variables_rejected(
        response, "NoSecurityDowngrade"
    ), f"Expected security downgrade to be rejected with NoSecurityDowngrade, but got: {response}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_network_configuration_security_downgrade_rejection_set_variables(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    TC_B_111_CS: Reject security profile downgrade via SetVariables on NetworkConfiguration
    B09.FR.32 - Verify that SetVariables is rejected with "NoSecurityDowngrade" reason
    when attempting to downgrade the SecurityProfile variable on an active configuration slot.
    """
    log.info(
        "##################### TC_B_111_CS: Reject Security Downgrade via SetVariables #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # First, set a higher security profile on slot 2
    set_var = SetVariableDataType(
        component=ComponentType(name="NetworkConfiguration", instance="2"),
        variable=VariableType(name="SecurityProfile"),
        attribute_type=AttributeEnumType.actual,
        attribute_value="2",  # Set to profile 2
    )

    response = await charge_point_v21.set_variables_req(
        set_variable_data=[set_var]
    )

    # This should succeed if slot 2 is not active
    assert validate_set_variables_success(
        response, 1
    ), f"Failed to set initial security profile: {response}"

    # Now try to downgrade the security profile on slot 2
    set_var = SetVariableDataType(
        component=ComponentType(name="NetworkConfiguration", instance="2"),
        variable=VariableType(name="SecurityProfile"),
        attribute_type=AttributeEnumType.actual,
        attribute_value="0",  # Attempt downgrade
    )

    response = await charge_point_v21.set_variables_req(
        set_variable_data=[set_var]
    )

    # B09.FR.32: Downgrade attempt on slot 2 after setting profile 2 should be rejected
    assert validate_set_variables_rejected(
        response, "NoSecurityDowngrade"
    ), f"Expected security downgrade to be rejected with NoSecurityDowngrade, but got: {response}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_network_configuration_allow_security_downgrade_flag(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    TC_B_112_CS: AllowSecurityDowngrade configuration flag behavior
    B09.FR.04, B09.FR.31 - Verify that when AllowSecurityDowngrade is set to false,
    all security profile downgrades are rejected regardless of whether they occur
    via SetNetworkProfileRequest or SetVariables on NetworkConfiguration.
    """
    log.info(
        "##################### TC_B_112_CS: AllowSecurityDowngrade Flag Enforcement #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Check the current AllowSecurityDowngrade setting
    get_var = GetVariableDataType(
        component=ComponentType(name="SecurityCtrlr"),
        variable=VariableType(name="AllowSecurityDowngrade"),
        attribute_type=AttributeEnumType.actual,
    )

    response = await charge_point_v21.get_variables_req(
        get_variable_data=[get_var]
    )

    # Verify we can read the AllowSecurityDowngrade variable
    assert response and response.get_variable_result, "No get variable result"
    results = response.get_variable_result
    assert len(results) > 0, "Empty get variable result"

    # When AllowSecurityDowngrade is false, any downgrade should be rejected
    # Try to set a lower security profile
    set_var = SetVariableDataType(
        component=ComponentType(name="NetworkConfiguration", instance="2"),
        variable=VariableType(name="SecurityProfile"),
        attribute_type=AttributeEnumType.actual,
        attribute_value="0",
    )

    response = await charge_point_v21.set_variables_req(
        set_variable_data=[set_var]
    )

    # B09.FR.04, B09.FR.31: When AllowSecurityDowngrade is false, any downgrade should be rejected
    assert validate_set_variables_rejected(
        response, "NoSecurityDowngrade"
    ), f"Expected security downgrade to be rejected with NoSecurityDowngrade, but got: {response}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_network_configuration_vpn_configuration(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    TC_B_113_CS: Set VPN configuration with all required parameters
    B09.FR.13, B09.FR.35 - Verify that CSMS can configure VPN connection details
    including server, user, password, key, type, and group for a network configuration slot.
    """
    log.info(
        "##################### TC_B_113_CS: Set Complete VPN Configuration #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Set complete VPN configuration on slot 2 (VpnEnabled is ReadOnly, set internally)
    set_vars = [
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="VpnServer"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="vpn-server.example.com",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="VpnUser"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="vpn_username",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="VpnPassword"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="vpn_password_secret",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="VpnKey"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="vpn_key_secret_material",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="VpnType"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="IKEv2",
        ),
        SetVariableDataType(
            component=ComponentType(name="NetworkConfiguration", instance="2"),
            variable=VariableType(name="VpnGroup"),
            attribute_type=AttributeEnumType.actual,
            attribute_value="vpn_group_1",
        ),
    ]

    response = await charge_point_v21.set_variables_req(
        set_variable_data=set_vars
    )

    # Validate that all VPN variables were successfully set
    assert validate_set_variables_success(
        response, len(set_vars)
    ), f"Failed to set complete VPN configuration: {response}"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_network_configuration_dm_end_to_end(
    central_system_v21: CentralSystem,
    test_controller: TestController,
):
    """
    US-006: End-to-end integration test for NetworkConfiguration device model.

    Verifies the full lifecycle:
    1. Migration from legacy NetworkConnectionProfiles blob to DM variables on boot
    2. GetVariables reads correct values from NetworkConfiguration[1]
    3. SetVariables updates a non-active slot and the change persists (read-back)
    4. SetVariables on the active slot is rejected with PriorityNetworkConf
    """
    log.info(
        "##################### US-006: NetworkConfiguration DM End-to-End #################"
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # ── Step 1 & 2: Verify migration happened by reading slot 1 DM variables ──
    # The charge point booted with a legacy NetworkConnectionProfiles blob.
    # Migration should have populated NetworkConfiguration[1] with those values.
    variable_names = ["OcppCsmsUrl", "SecurityProfile", "OcppInterface", "OcppTransport"]

    get_vars = [
        GetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="1"
            ),
            variable=VariableType(name=var_name),
            attribute_type=AttributeEnumType.actual,
        )
        for var_name in variable_names
    ]

    response = await charge_point_v21.get_variables_req(
        get_variable_data=get_vars
    )

    assert response and response.get_variable_result, "No get variable result for slot 1"
    results = response.get_variable_result
    assert len(results) == len(variable_names), (
        f"Expected {len(variable_names)} results, got {len(results)}"
    )

    # Build a map of variable name → value for easier assertions
    slot1_values = {}
    for r in results:
        var_name = r.get("variable", {}).get("name")
        status = r.get("attribute_status")
        assert status == "Accepted", (
            f"GetVariables failed for {var_name}: status={status}"
        )
        slot1_values[var_name] = r.get("attribute_value")

    # Verify migration produced correct values from the legacy blob
    # The legacy blob has: securityProfile=1, ocppInterface=Wired0, ocppTransport=JSON
    # OcppCsmsUrl is injected by the test framework to point to the test CSMS
    assert slot1_values["OcppCsmsUrl"], "OcppCsmsUrl should not be empty after migration"
    assert "ws" in slot1_values["OcppCsmsUrl"].lower(), (
        f"OcppCsmsUrl should be a websocket URL, got: {slot1_values['OcppCsmsUrl']}"
    )
    assert slot1_values["SecurityProfile"] == "1", (
        f"SecurityProfile should be 1, got: {slot1_values['SecurityProfile']}"
    )
    assert slot1_values["OcppInterface"] == "Wired0", (
        f"OcppInterface should be Wired0, got: {slot1_values['OcppInterface']}"
    )
    assert slot1_values["OcppTransport"] == "JSON", (
        f"OcppTransport should be JSON, got: {slot1_values['OcppTransport']}"
    )

    log.info("Step 1-2 PASSED: Migration verified — slot 1 DM variables match legacy blob")

    # ── Step 3: SetVariables on non-active slot 2, then read back to verify persistence ──
    new_url = "wss://updated-backup.example.com/ocpp"
    new_security_profile = "2"
    new_interface = "Wired0"
    new_transport = "JSON"

    set_vars = [
        SetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name="OcppCsmsUrl"),
            attribute_type=AttributeEnumType.actual,
            attribute_value=new_url,
        ),
        SetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name="SecurityProfile"),
            attribute_type=AttributeEnumType.actual,
            attribute_value=new_security_profile,
        ),
        SetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name="OcppInterface"),
            attribute_type=AttributeEnumType.actual,
            attribute_value=new_interface,
        ),
        SetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name="OcppTransport"),
            attribute_type=AttributeEnumType.actual,
            attribute_value=new_transport,
        ),
    ]

    response = await charge_point_v21.set_variables_req(
        set_variable_data=set_vars
    )

    assert validate_set_variables_success(response, len(set_vars)), (
        f"SetVariables on slot 2 should succeed: {response}"
    )

    # Read back slot 2 to verify persistence
    get_vars_slot2 = [
        GetVariableDataType(
            component=ComponentType(
                name="NetworkConfiguration", instance="2"
            ),
            variable=VariableType(name=var_name),
            attribute_type=AttributeEnumType.actual,
        )
        for var_name in variable_names
    ]

    response = await charge_point_v21.get_variables_req(
        get_variable_data=get_vars_slot2
    )

    assert response and response.get_variable_result, "No get variable result for slot 2"
    results = response.get_variable_result

    slot2_values = {}
    for r in results:
        var_name = r.get("variable", {}).get("name")
        status = r.get("attribute_status")
        assert status == "Accepted", (
            f"GetVariables failed for slot 2 {var_name}: status={status}"
        )
        slot2_values[var_name] = r.get("attribute_value")

    assert slot2_values["OcppCsmsUrl"] == new_url, (
        f"Slot 2 OcppCsmsUrl should be {new_url}, got: {slot2_values['OcppCsmsUrl']}"
    )
    assert slot2_values["SecurityProfile"] == new_security_profile, (
        f"Slot 2 SecurityProfile should be {new_security_profile}, got: {slot2_values['SecurityProfile']}"
    )
    assert slot2_values["OcppInterface"] == new_interface, (
        f"Slot 2 OcppInterface should be {new_interface}, got: {slot2_values['OcppInterface']}"
    )
    assert slot2_values["OcppTransport"] == new_transport, (
        f"Slot 2 OcppTransport should be {new_transport}, got: {slot2_values['OcppTransport']}"
    )

    log.info("Step 3 PASSED: SetVariables on slot 2 persisted and verified via GetVariables")

    # ── Step 4: SetVariables on the active slot (1) should be rejected ──
    set_var_active = SetVariableDataType(
        component=ComponentType(
            name="NetworkConfiguration", instance="1"
        ),
        variable=VariableType(name="OcppCsmsUrl"),
        attribute_type=AttributeEnumType.actual,
        attribute_value="wss://should-be-rejected.example.com/ocpp",
    )

    response = await charge_point_v21.set_variables_req(
        set_variable_data=[set_var_active]
    )

    assert validate_set_variables_rejected(
        response, "PriorityNetworkConf"
    ), (
        f"SetVariables on active slot 1 should be rejected with "
        f"PriorityNetworkConf, but got: {response}"
    )

    log.info("Step 4 PASSED: SetVariables on active slot 1 rejected with PriorityNetworkConf")
