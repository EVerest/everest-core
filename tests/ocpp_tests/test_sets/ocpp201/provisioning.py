# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
import asyncio
from datetime import datetime, timezone

import traceback
# fmt: off
import logging

from everest.testing.core_utils.controller.test_controller_interface import TestController

from ocpp.v201 import call as call201
from ocpp.v201 import call_result as call_result201
from ocpp.v201.enums import *
from ocpp.v201.datatypes import *
from ocpp.routing import on, create_route_map
from everest.testing.ocpp_utils.fixtures import *
from everest_test_utils import * # Needs to be before the datatypes below since it overrides the v201 Action enum with the v16 one
from ocpp.v201.enums import (Action, SetVariableStatusEnumType, ConnectorStatusEnumType,GetVariableStatusEnumType)
from validations import validate_status_notification_201, validate_notify_report_data_201, wait_for_callerror_and_validate
from everest.testing.core_utils._configuration.libocpp_configuration_helper import GenericOCPP2XConfigAdjustment
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility, OcppTestConfiguration, ValidationMode
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
# fmt: on

log = logging.getLogger("provisioningTest")


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_B08_FR_07(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    B08.FR.07
    ComponentCriteria contains: Active The Charging Station SHALL report every component that has
    the variable Active set to true, or does not have the Active variable in a NotifyReportRequest
    """

    log.info(
        " ############################# Test case B08: Get custom report ###############################"
    )

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # set a component variable to true
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "ChargingStatusIndicator", "Active", "true"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    r = await charge_point_v201.get_report_req(
        request_id=567, component_criteria=[ComponentCriterionEnumType.active]
    )

    exp_single_report_data_active = ReportDataType(
        component=ComponentType(name="ChargingStatusIndicator"),
        variable=VariableType(name="Active"),
        variable_attribute=VariableAttributeType(
            type=AttributeEnumType.actual, value="true"
        ),
    )

    # get the value of component criteria
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "NotifyReport",
        call201.NotifyReport(
            request_id=567,
            generated_at=datetime.now().isoformat(),
            seq_no=0,
            report_data=[exp_single_report_data_active],
        ),
        validate_notify_report_data_201,
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_B08_FR_08(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    B08.FR.08
    ComponentCriteria contains: Available The Charging Station SHALL report every component that has
    the variable Available set to true, or does not have the Available variable in a NotifyReportRequest
    """

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    await charge_point_v201.get_report_req(
        request_id=777, component_criteria=[ComponentCriterionEnumType.available]
    )

    exp_single_report_data_avail = ReportDataType(
        component=ComponentType(name="AuthCacheCtrlr"),
        variable=VariableType(name="Available"),
        variable_attribute=VariableAttributeType(
            type=AttributeEnumType.actual,
            value="true",
            # mutability=MutabilityEnumType.read_write
        ),
    )

    # get the value of component criteria
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "NotifyReport",
        call201.NotifyReport(
            request_id=777,
            generated_at=datetime.now().isoformat(),
            seq_no=0,
            report_data=[exp_single_report_data_avail],
        ),
        validate_notify_report_data_201,
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_B08_FR_09(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """B08.FR.09 ComponentCriteria contains: EnabledThe Charging Station SHALL report every component that
    has the variable Enabled set to true, or does not have the Enabled variable, in a NotifyReportRequest.
    """

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Enable some variables with enable
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AuthCacheCtrlr", "Enabled", "true"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "SampledDataCtrlr", "Enabled", "false"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    r = await charge_point_v201.get_report_req(
        request_id=1,
        component_criteria=[ComponentCriterionEnumType.enabled],
        component_variable=[
            ComponentVariableType(component=ComponentType(name="TxCtrlr")),
            ComponentVariableType(
                component=ComponentType(name="DeviceDataCtrlr")),
            ComponentVariableType(
                component=ComponentType(name="AuthCacheCtrlr")),
        ],
    )

    exp_single_report_data = ReportDataType(
        component=ComponentType(name="AuthCacheCtrlr"),
        variable=VariableType(name="Enabled"),
        variable_attribute=VariableAttributeType(
            type=AttributeEnumType.actual,
            value="true",
            # mutability=MutabilityEnumType.read_write
        ),
        variable_characteristics=VariableCharacteristicsType(
            data_type=DataEnumType.boolean, supports_monitoring=True
        ),
    )

    # get the value of component criteria
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "NotifyReport",
        call201.NotifyReport(
            request_id=1,
            generated_at=datetime.now().isoformat(),
            seq_no=0,
            report_data=[exp_single_report_data],
        ),
        validate_notify_report_data_201,
    )

    # await asyncio.sleep(3)


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_B08_FR_10(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    B08.FR.10
    ComponentCriteria contains: ProblemThe Charging Station SHALL report every component that has
    the variable Problem set to true in a NotifyReportRequest.
    """

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # set a component variable to true
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "ChargingStation", "Problem", "true"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    r = await charge_point_v201.get_report_req(
        request_id=45, component_criteria=[ComponentCriterionEnumType.problem]
    )

    exp_single_report_data2 = ReportDataType(
        component=ComponentType(name="ChargingStation"),
        variable=VariableType(name="Problem"),
        variable_attribute=VariableAttributeType(
            type=AttributeEnumType.actual,
            value="true",
            # mutability=MutabilityEnumType.read_write
        ),
    )

    # get the value of component criteria
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "NotifyReport",
        call201.NotifyReport(
            request_id=45,
            generated_at=datetime.now().isoformat(),
            seq_no=0,
            report_data=[exp_single_report_data2],
        ),
        validate_notify_report_data_201,
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "DeviceDataCtrlr", "BytesPerMessageGetReport", "Actual"
                ),
                "42",
            )
        ]
    )
)
async def test_B08_FR_18(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    B08.FR.18
    Charging Station receives a GetReportRequest with a length of more bytes than allowed by BytesPerMessageGetReport
    The Charging Station MAY respond with a CALLERROR(FormatViolation)

    Setup: Set BytesPerMessage to 42 for this test
    """

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # get the value of BytesPerMessage
    r: call_result201.GetVariables = await charge_point_v201.get_variables_req(
        get_variable_data=[
            GetVariableDataType(
                component=ComponentType(name="DeviceDataCtrlr"),
                variable=VariableType(
                    name="BytesPerMessage",
                    instance="GetReport",
                ),
                attribute_type=AttributeEnumType.actual,
            )
        ]
    )
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0]
    )
    assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted
    bytes_per_message = json.loads(get_variables_result.attribute_value)
    log.debug(" max bytes per get report request %d" % bytes_per_message)

    r = await charge_point_v201.get_report_req(
        request_id=777, component_criteria=[ComponentCriterionEnumType.available]
    )
    assert await wait_for_callerror_and_validate(
        test_utility, charge_point_v201, "FormatViolation"
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "DeviceDataCtrlr", "ItemsPerMessageGetReport"
                ),
                "2",
            )
        ]
    )
)
async def test_B08_FR_17(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    B08.FR.17
    Charging Station receives a GetReportRequest with more ComponentVariableType elements than allowed by ItemsPerMessageGetReport
    The Charging Station MAY respond with a CALLERROR(OccurenceConstraintViolation)

    Setup set ItemsPerMessageGetReport to 2
    """

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # get the value of ItemsPerMessage
    r: call_result201.GetVariables = await charge_point_v201.get_variables_req(
        get_variable_data=[
            GetVariableDataType(
                component=ComponentType(name="DeviceDataCtrlr"),
                variable=VariableType(
                    name="ItemsPerMessage",
                    instance="GetReport",
                ),
                attribute_type=AttributeEnumType.actual,
            )
        ]
    )
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0]
    )
    assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted
    items_per_message = json.loads(get_variables_result.attribute_value)
    log.debug(" max items per get report request %d" % items_per_message)

    r = await charge_point_v201.get_report_req(
        request_id=777,
        component_variable=[
            ComponentVariableType(
                component=ComponentType(name="TxCtrlr"),
                variable=VariableType(name="StopTxOnInvalidId"),
            ),
            ComponentVariableType(
                component=ComponentType(name="DeviceDataCtrlr"),
                variable=VariableType(
                    name="ItemsPerMessage",
                    instance="GetVariables",
                ),
            ),
            ComponentVariableType(
                component=ComponentType(name="AlignedDataCtrlr"),
                variable=VariableType(name="Measurands"),
            ),
        ],
    )
    assert await wait_for_callerror_and_validate(
        test_utility, charge_point_v201, "OccurenceConstraintViolation"
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_TC_B_18_CS(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    TC_B_18_CS
    Get Custom Report - with component criteria and component/variable
    """
    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    r = await charge_point_v201.get_report_req(
        request_id=2534,
        component_criteria=[ComponentCriterionEnumType.available],
        component_variable=[
            ComponentVariableType(
                component=ComponentType(name="EVSE", evse=EVSEType(id=1)),
                variable=VariableType(name="AvailabilityState"),
            )
        ],
    )

    exp_single_report_data = ReportDataType(
        component=ComponentType(name="EVSE", evse=EVSEType(id=1)),
        variable=VariableType(name="AvailabilityState"),
        variable_attribute=VariableAttributeType(
            type=AttributeEnumType.actual, value="Available"
        ),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "NotifyReport",
        call201.NotifyReport(
            request_id=2534,
            generated_at=datetime.now().isoformat(),
            seq_no=0,
            report_data=[exp_single_report_data],
        ),
        validate_notify_report_data_201,
    )

    r: call_result201.GetReport = await charge_point_v201.get_report_req(
        request_id=2535,
        component_criteria=[ComponentCriterionEnumType.problem],
        component_variable=[
            ComponentVariableType(
                component=ComponentType(name="EVSE", evse=EVSEType(id=1)),
                variable=VariableType(name="AvailabilityState"),
            )
        ],
    )

    # should return an empty set
    assert r.status == "EmptyResultSet"


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "DeviceDataCtrlr", "ItemsPerMessageGetReport"
                ),
                "4",
            ),
            (
                OCPP2XConfigVariableIdentifier(
                    "DeviceDataCtrlr", "ItemsPerMessageGetVariables"
                ),
                "2",
            ),
        ]
    )
)
async def test_TC_B_54_CS(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    TC_B_54_CS
    Get Custom Report - with component/variable, but no instance
    """
    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    await charge_point_v201.get_report_req(
        request_id=2538,
        component_variable=[
            ComponentVariableType(
                component=ComponentType(
                    name="DeviceDataCtrlr",
                ),
                variable=VariableType(name="ItemsPerMessage"),
            )
        ],
    )

    b_54_1 = ReportDataType(
        component=ComponentType(
            name="DeviceDataCtrlr",
        ),
        variable=VariableType(name="ItemsPerMessage", instance="GetReport"),
        variable_attribute=VariableAttributeType(
            type=AttributeEnumType.actual, value="4"),
    )

    b_54_2 = ReportDataType(
        component=ComponentType(
            name="DeviceDataCtrlr",
        ),
        variable=VariableType(name="ItemsPerMessage", instance="GetVariables"),
        variable_attribute=VariableAttributeType(
            type=AttributeEnumType.actual, value="2"),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "NotifyReport",
        call201.NotifyReport(
            request_id=2538,
            generated_at=datetime.now().isoformat(),
            seq_no=0,
            report_data=[b_54_1, b_54_2],
        ),
        validate_notify_report_data_201,
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "DeviceDataCtrlr", "ItemsPerMessageGetReport"
                ),
                "4",
            )
        ]
    )
)
async def test_TC_B_55_CS(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    TC_B_55_CS
    Get Custom Report - with component/variable/instance
    """

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    await charge_point_v201.get_report_req(
        request_id=2539,
        component_variable=[
            ComponentVariableType(
                component=ComponentType(
                    name="DeviceDataCtrlr",
                ),
                variable=VariableType(
                    name="ItemsPerMessage", instance="GetReport"),
            )
        ],
    )

    b_55_1 = ReportDataType(
        component=ComponentType(
            name="DeviceDataCtrlr",
        ),
        variable=VariableType(name="ItemsPerMessage", instance="GetReport"),
        variable_attribute=VariableAttributeType(
            type=AttributeEnumType.actual, value="4"),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "NotifyReport",
        call201.NotifyReport(
            request_id=2539,
            generated_at=datetime.now().isoformat(),
            seq_no=0,
            report_data=[b_55_1],
        ),
        validate_notify_report_data_201,
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_TC_B_56_CS(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    TC_B_56_CS
    Get Custom Report - with component/variable, but no evseId
    """

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    await charge_point_v201.get_report_req(
        request_id=2544,
        component_variable=[
            ComponentVariableType(
                component=ComponentType(name="EVSE"),
                variable=VariableType(name="AvailabilityState"),
            )
        ],
    )

    b_56_1 = ReportDataType(
        component=ComponentType(name="EVSE", evse=EVSEType(id=1)),
        variable=VariableType(name="AvailabilityState"),
        variable_attribute=VariableAttributeType(
            type=AttributeEnumType.actual, value="Available"
        ),
    )

    b_56_2 = ReportDataType(
        component=ComponentType(name="EVSE", evse=EVSEType(id=2)),
        variable=VariableType(name="AvailabilityState"),
        variable_attribute=VariableAttributeType(
            type=AttributeEnumType.actual, value="Available"
        ),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "NotifyReport",
        call201.NotifyReport(
            request_id=2544,
            generated_at=datetime.now().isoformat(),
            seq_no=0,
            report_data=[b_56_1, b_56_2],
        ),
        validate_notify_report_data_201,
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_cold_boot_01(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    B01.FR.01
    ...
    """

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint()

    try:
        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v201,
            "StatusNotification",
            call201.StatusNotification(
                datetime.now().isoformat(), ConnectorStatusEnumType.available, 1, 1
            ),
            validate_status_notification_201,
        )
    except Exception as e:
        traceback.print_exc()
        logging.critical(e)

    # TOOD(piet): Check configured HeartbeatInterval of BootNotificationResponse


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_cold_boot_pending_01(
    test_config: OcppTestConfiguration,
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):

    @on(Action.boot_notification)
    def on_boot_notification_pending(**kwargs):
        return call_result201.BootNotification(
            current_time=datetime.now().isoformat(),
            interval=5,
            status=RegistrationStatusEnumType.pending,
        )

    @on(Action.boot_notification)
    def on_boot_notification_accepted(**kwargs):
        return call_result201.BootNotification(
            current_time=datetime.now(timezone.utc).isoformat(),
            interval=5,
            status=RegistrationStatusEnumType.accepted,
        )

    test_utility.forbidden_actions.append("SecurityEventNotification")

    central_system_v201.function_overrides.append(
        ("on_boot_notification", on_boot_notification_pending)
    )

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint()

    setattr(charge_point_v201, "on_boot_notification",
            on_boot_notification_accepted)
    central_system_v201.chargepoint.route_map = create_route_map(
        central_system_v201.chargepoint
    )

    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "BootNotification", {}
    )

    test_utility.forbidden_actions.clear()

    test_controller.plug_in()

    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "SecurityEventNotification", {}
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_cold_boot_rejected_01(
    test_config: OcppTestConfiguration,
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):

    @on(Action.boot_notification)
    def on_boot_notification_pending(**kwargs):
        return call_result201.BootNotification(
            current_time=datetime.now().isoformat(),
            interval=5,
            status=RegistrationStatusEnumType.rejected,
        )

    @on(Action.boot_notification)
    def on_boot_notification_accepted(**kwargs):
        return call_result201.BootNotification(
            current_time=datetime.now(timezone.utc).isoformat(),
            interval=5,
            status=RegistrationStatusEnumType.accepted,
        )

    central_system_v201.function_overrides.append(
        ("on_boot_notification", on_boot_notification_pending)
    )

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint()

    setattr(charge_point_v201, "on_boot_notification",
            on_boot_notification_accepted)
    central_system_v201.chargepoint.route_map = create_route_map(
        central_system_v201.chargepoint
    )

    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "BootNotification", {}
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_set_get_variables_01(
    charge_point_v201: ChargePoint201, test_utility: TestUtility
):

    await charge_point_v201.get_variables_req(
        get_variable_data=[
            GetVariableDataType(
                component=ComponentType(name="TxCtrlr"),
                variable=VariableType(name="StopTxOnInvalidId"),
                attribute_type=AttributeEnumType.actual,
            )
        ]
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "GetVariables",
        call_result201.GetVariables(
            get_variable_result=[
                GetVariableResultType(
                    component=ComponentType(name="TxCtrlr"),
                    variable=VariableType(name="StopTxOnInvalidId"),
                    attribute_status=GetVariableStatusEnumType.accepted,
                    attribute_type=AttributeEnumType.actual,
                    attribute_value="true",
                )
            ]
        ),
    )

    await charge_point_v201.set_variables_req(
        set_variable_data=[
            SetVariableDataType(
                attribute_value="false",
                attribute_type=AttributeEnumType.actual,
                component=ComponentType(name="TxCtrlr"),
                variable=VariableType(name="StopTxOnInvalidId"),
            )
        ]
    )

    await charge_point_v201.get_variables_req(
        get_variable_data=[
            GetVariableDataType(
                component=ComponentType(name="TxCtrlr"),
                variable=VariableType(name="StopTxOnInvalidId"),
                attribute_type=AttributeEnumType.actual,
            )
        ]
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "GetVariables",
        call_result201.GetVariables(
            get_variable_result=[
                GetVariableResultType(
                    component=ComponentType(name="TxCtrlr"),
                    variable=VariableType(name="StopTxOnInvalidId"),
                    attribute_status=GetVariableStatusEnumType.accepted,
                    attribute_type=AttributeEnumType.actual,
                    attribute_value="false",
                )
            ]
        ),
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_set_get_variables_02(
    charge_point_v201: ChargePoint201, test_utility: TestUtility
):
    await charge_point_v201.get_variables_req(
        get_variable_data=[
            GetVariableDataType(
                component=ComponentType(name="TxCtrlr"),
                variable=VariableType(name="StopTxOnInvalidId"),
                attribute_type=AttributeEnumType.actual,
            ),
            GetVariableDataType(
                component=ComponentType(name="InternalCtrlr"),
                variable=VariableType(name="ChargePointVendor"),
                attribute_type=AttributeEnumType.actual,
            ),
            GetVariableDataType(
                component=ComponentType(name="OCPPCommCtrlr"),
                variable=VariableType(name="UnknownVariable"),
                attribute_type=AttributeEnumType.actual,
            ),
            GetVariableDataType(
                component=ComponentType(name="UnknownComponent"),
                variable=VariableType(name="UnknownVariable"),
                attribute_type=AttributeEnumType.actual,
            ),
        ]
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "GetVariables",
        call_result201.GetVariables(
            get_variable_result=[
                GetVariableResultType(
                    component=ComponentType(name="TxCtrlr"),
                    variable=VariableType(name="StopTxOnInvalidId"),
                    attribute_status=GetVariableStatusEnumType.accepted,
                    attribute_type=AttributeEnumType.actual,
                    attribute_value="true",
                ),
                GetVariableResultType(
                    component=ComponentType(name="InternalCtrlr"),
                    variable=VariableType(name="ChargePointVendor"),
                    attribute_status=GetVariableStatusEnumType.accepted,
                    attribute_type=AttributeEnumType.actual,
                    attribute_value="EVerestVendor",
                ),
                GetVariableResultType(
                    component=ComponentType(name="OCPPCommCtrlr"),
                    variable=VariableType(name="UnknownVariable"),
                    attribute_status=GetVariableStatusEnumType.unknown_variable,
                    attribute_type=AttributeEnumType.actual,
                ),
                GetVariableResultType(
                    component=ComponentType(name="UnknownComponent"),
                    variable=VariableType(name="UnknownVariable"),
                    attribute_status=GetVariableStatusEnumType.unknown_component,
                    attribute_type=AttributeEnumType.actual,
                ),
            ]
        ),
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_get_base_report_01(
    charge_point_v201: ChargePoint201, test_utility: TestUtility
):
    await charge_point_v201.get_base_report_req(
        request_id=1, report_base=ReportBaseEnumType.full_inventory
    )

    await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "GetBaseReport",
        call_result201.GetBaseReport(
            status=GenericDeviceModelStatusEnumType.accepted
        ),
    )

    exp_single_report_data = ReportDataType(
        component=ComponentType(name="TxCtrlr"),
        variable=VariableType(name="StopTxOnInvalidId"),
        variable_attribute=VariableAttributeType(
            type=AttributeEnumType.actual,
            value="true",
            mutability=MutabilityEnumType.read_write,
        ),
        variable_characteristics=VariableCharacteristicsType(
            data_type=DataEnumType.boolean, supports_monitoring=True
        ),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "NotifyReport",
        call201.NotifyReport(
            request_id=1,
            generated_at=datetime.now().isoformat(),
            seq_no=0,
            report_data=[exp_single_report_data],
        ),
        validate_notify_report_data_201,
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_get_custom_report_01(charge_point_v201: ChargePoint201):
    await charge_point_v201.get_report_req(
        request_id=1,
        component_variable=[
            ComponentVariableType(
                component=ComponentType(name="TxCtrlr"),
                variable=VariableType(name="NotAValidVariable"),
            ),
        ],
        component_criteria=[ComponentCriterionEnumType.enabled],
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_b09_b10(
    charge_point_v201: ChargePoint201,
    test_controller: TestController,
    central_system_v201: CentralSystem,
):

    # TODO(This discovers a bug in the connectivity_manager of libocpp. this->network_connection_profiles are not updated when a new profile is set)

    r: call_result201.GetVariables = (
        await charge_point_v201.get_config_variables_req(
            "InternalCtrlr", "NetworkConnectionProfiles"
        )
    )
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0]
    )
    assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted

    profiles = json.loads(get_variables_result.attribute_value)
    assert len(profiles) == 1

    # invalid security profile
    r: call_result201.SetNetworkProfile = (
        await charge_point_v201.set_network_profile_req(
            configuration_slot=1,
            connection_data=NetworkConnectionProfileType(
                ocpp_version=OCPPVersionEnumType.ocpp20,
                ocpp_transport=OCPPTransportEnumType.json,
                ocpp_csms_url="ws://localhost:9000/cp001",
                message_timeout=30,
                security_profile=0,
                ocpp_interface=OCPPInterfaceEnumType.wired0,
            ),
        )
    )

    assert r.status == "Rejected"

    # invalid configuration slot
    r: call_result201.SetNetworkProfile = (
        await charge_point_v201.set_network_profile_req(
            configuration_slot=100,
            connection_data=NetworkConnectionProfileType(
                ocpp_version=OCPPVersionEnumType.ocpp20,
                ocpp_transport=OCPPTransportEnumType.json,
                ocpp_csms_url="ws://localhost:9000/cp001",
                message_timeout=30,
                security_profile=0,
                ocpp_interface=OCPPInterfaceEnumType.wired0,
            ),
        )
    )

    assert r.status == "Rejected"

    # valid
    r: call_result201.SetNetworkProfile = (
        await charge_point_v201.set_network_profile_req(
            configuration_slot=2,
            connection_data=NetworkConnectionProfileType(
                ocpp_version=OCPPVersionEnumType.ocpp20,
                ocpp_transport=OCPPTransportEnumType.json,
                ocpp_csms_url="wss://localhost:9000/cp001",
                message_timeout=30,
                security_profile=2,
                ocpp_interface=OCPPInterfaceEnumType.wired0,
            ),
        )
    )

    assert r.status == "Accepted"

    r: call_result201.GetVariables = (
        await charge_point_v201.get_config_variables_req(
            "InternalCtrlr", "NetworkConnectionProfiles"
        )
    )
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0]
    )
    assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted

    profiles = json.loads(get_variables_result.attribute_value)
    assert len(profiles) == 2

    # Set valid NetworkConfigurationPriority
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "OCPPCommCtrlr", "NetworkConfigurationPriority", "2,1"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "DeviceDataCtrlr", "ItemsPerMessageGetVariables"
                ),
                "2",
            )
        ]
    )
)
async def test_B06_09_16(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    B06.FR.09
    B06.FR.16
    """
    log.info(
        " ############################# Test case B06: Get variables Request ###############################"
    )

    # When the Charging Station receives a GetVariablesRequest for a Variable in the GetVariableData that is WriteOnly,
    # The Charging Station SHALL set the attributeStatus field in the
    # corresponding GetVariableResult to: Rejected.

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # Write into Basic Auth Password
    r: call_result.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "SecurityCtrlr", "BasicAuthPassword", "8BADF00D8BADF00D"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # wait for reconnect
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=False
    )

    r: call_result201.GetVariables = (
        await charge_point_v201.get_config_variables_req(
            "SecurityCtrlr", "BasicAuthPassword"
        )
    )
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0]
    )
    assert get_variables_result.attribute_status == GetVariableStatusEnumType.rejected

    # Charging Station receives a GetVariablesRequest with more GetVariableData elements than allowed by ItemsPerMessageGetVariables
    # The Charging Station MAY respond with a CALLERROR(OccurenceConstraintViolation)

    # get the value of ItemsPerMessage
    r: call_result201.GetVariables = await charge_point_v201.get_variables_req(
        get_variable_data=[
            GetVariableDataType(
                component=ComponentType(name="DeviceDataCtrlr"),
                variable=VariableType(
                    name="ItemsPerMessage",
                    instance="GetVariables",
                ),
                attribute_type=AttributeEnumType.actual,
            )
        ]
    )
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0]
    )
    assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted
    items_per_message = json.loads(get_variables_result.attribute_value)
    log.debug(" max items per get variables request %d" % items_per_message)

    # request more than max items per message variables
    # get the value of ItemsPerMessage
    r: call_result201.GetVariables = await charge_point_v201.get_variables_req(
        get_variable_data=[
            GetVariableDataType(
                component=ComponentType(name="DeviceDataCtrlr"),
                variable=VariableType(
                    name="ItemsPerMessage",
                    instance="GetVariables",
                ),
                attribute_type=AttributeEnumType.actual,
            ),
            GetVariableDataType(
                component=ComponentType(name="TxCtrlr"),
                variable=VariableType(name="StopTxOnInvalidId"),
                attribute_type=AttributeEnumType.actual,
            ),
            GetVariableDataType(
                component=ComponentType(name="AlignedDataCtrlr"),
                variable=VariableType(name="Measurands"),
                attribute_type=AttributeEnumType.actual,
            ),
            GetVariableDataType(
                component=ComponentType(name="AuthCacheCtrlr"),
                variable=VariableType(name="Enabled"),
                attribute_type=AttributeEnumType.actual,
            ),
        ]
    )
    assert await wait_for_callerror_and_validate(
        test_utility, charge_point_v201, "OccurenceConstraintViolation"
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_B04(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    B04.FR.01
    B04.FR.02
    ...
    """

    # prepare data for the test
    evse_id1 = 1
    connector_id = 1

    evse_id2 = 2

    @on(Action.boot_notification)
    def on_boot_notification_accepted(**kwargs):
        return call_result201.BootNotification(
            current_time=datetime.now(timezone.utc).isoformat(),
            interval=3,
            status=RegistrationStatusEnumType.accepted,
        )

    central_system_v201.function_overrides.append(
        ("on_boot_notification", on_boot_notification_accepted)
    )

    test_utility.validation_mode = ValidationMode.STRICT

    log.info(
        " ############################# Test case B04: Offline Idle Behaviour ###############################"
    )

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=False
    )

    # setattr(charge_point_v201, 'on_boot_notification_accepted',on_boot_notification_accepted)
    central_system_v201.chargepoint.route_map = create_route_map(
        central_system_v201.chargepoint
    )

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id=evse_id1,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id=evse_id2,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )

    # Set valid OfflineThreshold to 15s
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "OCPPCommCtrlr", "OfflineThreshold", "10"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    test_utility.messages.clear()

    log.debug("========================B04.FR.01=========================")
    # Simulate connection loss
    test_controller.disconnect_websocket()

    # Wait 11 seconds
    await asyncio.sleep(11)

    # Connect CS
    log.debug(" Connect the CS to the CSMS")
    test_controller.connect_websocket()

    # wait for reconnect
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=False
    )

    # expect StatusNotification with status available as the disconnect duration was > than offline throeshold
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id=evse_id1,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id=evse_id2,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )

    test_utility.messages.clear()

    # Wait 5 seconds
    await asyncio.sleep(5)

    log.debug("========================B04.FR.02=========================")

    # start charging session
    test_controller.plug_in(connector_id=2)

    # expect StatusNotification with status occupied
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.occupied,
            evse_id=evse_id2,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )

    # Simulate connection loss
    test_controller.disconnect_websocket()

    # Wait 2 seconds
    await asyncio.sleep(2)

    # stop charging session
    test_controller.plug_out(connector_id=2)

    # Wait 3 seconds
    await asyncio.sleep(3)

    # Connect CS
    log.debug(" Connect the CS to the CSMS")
    test_controller.connect_websocket()

    # wait for reconnect
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=False
    )

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id=evse_id2,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )

    test_utility.messages.clear()

    for _ in range(3):
        # send HeartBeat request when idle
        assert await wait_for_and_validate(
            test_utility, charge_point_v201, "Heartbeat", call.Heartbeat()
        )
