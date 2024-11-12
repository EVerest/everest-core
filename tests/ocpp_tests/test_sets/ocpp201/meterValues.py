# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

# fmt: off
import pytest
from datetime import datetime
import logging

from everest.testing.core_utils.controller.test_controller_interface import TestController

from validations import *
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility
from everest.testing.ocpp_utils.fixtures import *

from everest_test_utils import * # Needs to be before the datatypes below since it overrides the v201 Action enum with the v16 one
from ocpp.v201.enums import (IdTokenType as IdTokenTypeEnum, SetVariableStatusType, ConnectorStatusType,GetVariableStatusType)
from ocpp.v201.datatypes import *
from ocpp.v201 import call as call201
from ocpp.v201 import call_result as call_result201

# fmt: on

log = logging.getLogger("meterValues")


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_J01_19(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    J01.FR.19
    ...
    """
    # prepare data for the test
    evse_id1 = 1
    connector_id = 1

    evse_id2 = 2

    # make an unknown IdToken
    id_tokenJ01 = IdTokenType(id_token="8BADF00D", type=IdTokenTypeEnum.iso14443)

    log.info(
        "##################### J01.FR.19: Sending Meter Values not related to a transaction #################"
    )
    test_utility.messages.clear()

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotificationPayload(
            datetime.now().isoformat(),
            ConnectorStatusType.available,
            evse_id=evse_id1,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotificationPayload(
            datetime.now().isoformat(),
            ConnectorStatusType.available,
            evse_id=evse_id2,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )

    # Configure AlignedDataInterval
    r: call_result201.SetVariablesPayload = (
        await charge_point_v201.set_config_variables_req(
            "AlignedDataCtrlr", "Interval", "3"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # Configure SampledDataInterval
    r: call_result201.SetVariablesPayload = (
        await charge_point_v201.set_config_variables_req(
            "SampledDataCtrlr", "TxUpdatedInterval", "3"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # Configure AlignedDataInterval
    r: call_result201.SetVariablesPayload = (
        await charge_point_v201.set_config_variables_req(
            "AlignedDataCtrlr", "SendDuringIdle", "true"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # Configure PhaseRotation
    r: call_result201.SetVariablesPayload = (
        await charge_point_v201.set_config_variables_req(
            "ChargingStation", "PhaseRotation", "TRS"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # Get the value of PhaseRotation
    r: call_result201.GetVariablesPayload = (
        await charge_point_v201.get_config_variables_req(
            "ChargingStation", "PhaseRotation"
        )
    )
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0]
    )
    if get_variables_result.attribute_status == GetVariableStatusType.accepted:
        log.info("Phase Rotation %s " % get_variables_result.attribute_value)

    # send meter values periodically when not charging
    logging.debug("Collecting meter values...")
    for _ in range(3):
        # send MeterValues
        assert await wait_for_and_validate(
            test_utility, charge_point_v201, "MeterValues", {"evseId": 1}
        )
        assert await wait_for_and_validate(
            test_utility, charge_point_v201, "MeterValues", {"evseId": 2}
        )

    # swipe id tag to authorize
    test_controller.swipe(id_tokenJ01.id_token)

    # start charging session
    test_controller.plug_in()

    test_utility.messages.clear()

    # when in a middle of a transaction do not send meter values
    test_utility.forbidden_actions.append("MeterValues")

    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "TransactionEvent", {"eventType": "Started"}
    )

    for _ in range(3):
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v201,
            "TransactionEvent",
            {"eventType": "Updated"},
        )

    # swipe id tag to de-authorize
    test_controller.swipe(id_tokenJ01.id_token)

    # stop charging session
    test_controller.plug_out()

    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "TransactionEvent", {"eventType": "Ended"}
    )
