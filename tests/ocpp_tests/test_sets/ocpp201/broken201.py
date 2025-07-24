# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import json

import pytest
from everest.testing.core_utils.controller.test_controller_interface import (
    TestController,
)

from ocpp.v201.datatypes import SetVariableDataType, VariableType, ComponentType
from ocpp.v201 import call as call201
from ocpp.messages import Call, _DecimalEncoder
from ocpp.charge_point import asdict, remove_nones, snake_to_camel_case

# fmt: off
from everest.testing.ocpp_utils.charge_point_utils import TestUtility
from everest_test_utils import get_everest_config_path_str, send_message_without_validation
from everest.testing.ocpp_utils.central_system import CentralSystem
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
from validations import wait_for_callerror_and_validate
# fmt: on


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
async def test_invalid_payloads(
    central_system_v201: CentralSystem,
    charge_point_v201: ChargePoint201,
    test_controller: TestController,
    test_utility: TestUtility,
):

    # a payload field that is too long should trigger a FormationViolation CALLERROR
    too_long_component_type_name_payload = call201.SetVariables(
        set_variable_data=[SetVariableDataType(attribute_value="abc", component=ComponentType(
            name="ThisIsMuchLongerThan50charactersThisIsMuchLongerThan50charactersThisIsMuchLongerThan50charactersThisIsMuchLongerThan50charactersThisIsMuchLongerThan50characters"), variable=VariableType(name="VariableName"))]
    )
    camel_case_payload = snake_to_camel_case(
        asdict(too_long_component_type_name_payload))

    call_msg = Call(
        unique_id=str(charge_point_v201._unique_id_generator()),
        action=too_long_component_type_name_payload.__class__.__name__,
        payload=remove_nones(camel_case_payload),
    )

    await send_message_without_validation(charge_point_v201, call_msg)

    assert await wait_for_callerror_and_validate(
        test_utility, charge_point_v201, "FormationViolation"
    )

    # a unknown action should trigger a FormationViolation CALLERROR
    call_msg = Call(
        unique_id=str(charge_point_v201._unique_id_generator()),
        action="ThisIsAnUnknownAction",
        payload="Invalid",
    )

    await send_message_without_validation(charge_point_v201, call_msg)

    assert await wait_for_callerror_and_validate(
        test_utility, charge_point_v201, "FormationViolation"
    )

    # a malformed CALL should trigger a RpcFrameworkError CALLERROR
    call_msg = "{Malformed"

    async with charge_point_v201._call_lock:
        await charge_point_v201._send(call_msg)

    assert await wait_for_callerror_and_validate(
        test_utility, charge_point_v201, "RpcFrameworkError"
    )

    # a malformed CALL should trigger a RpcFrameworkError CALLERROR
    call_msg = b"\xd8\x00\x00\x00"

    async with charge_point_v201._call_lock:
        await charge_point_v201._send(call_msg)

    assert await wait_for_callerror_and_validate(
        test_utility, charge_point_v201, "RpcFrameworkError"
    )

    # a invalid payload should trigger a FormationViolation CALLERROR
    call_msg = Call(
        unique_id=str(charge_point_v201._unique_id_generator()),
        action="SetVariables",
        payload=None,
    )

    await send_message_without_validation(charge_point_v201, call_msg)

    assert await wait_for_callerror_and_validate(
        test_utility, charge_point_v201, "FormationViolation"
    )

    # a CALL message without UUID, action and payload should trigger a RpcFrameworkError CALLERROR
    json_data = json.dumps(
        [
            2,  # CALL
        ],
        # By default json.dumps() adds a white space after every separator.
        # By setting the separator manually that can be avoided.
        separators=(",", ":"),
        cls=_DecimalEncoder,
    )

    async with charge_point_v201._call_lock:
        await charge_point_v201._send(json_data)

        assert await wait_for_callerror_and_validate(
            test_utility, charge_point_v201, "RpcFrameworkError"
        )

    # a completely empty message should trigger a RpcFrameworkError CALLERROR
    json_data = json.dumps(
        [
        ],
        # By default json.dumps() adds a white space after every separator.
        # By setting the separator manually that can be avoided.
        separators=(",", ":"),
        cls=_DecimalEncoder,
    )

    async with charge_point_v201._call_lock:
        await charge_point_v201._send(json_data)

        assert await wait_for_callerror_and_validate(
            test_utility, charge_point_v201, "RpcFrameworkError"
        )

    # a unknown RPC message should trigger a MessageTypeNotSupported CALLERROR
    json_data = json.dumps(
        [
            99,  # should be unknown
            "MessageId",
        ],
        # By default json.dumps() adds a white space after every separator.
        # By setting the separator manually that can be avoided.
        separators=(",", ":"),
        cls=_DecimalEncoder,
    )

    async with charge_point_v201._call_lock:
        await charge_point_v201._send(json_data)

        assert await wait_for_callerror_and_validate(
            test_utility, charge_point_v201, "MessageTypeNotSupported"
        )
