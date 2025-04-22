# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
import asyncio
from datetime import datetime

from everest.testing.core_utils.controller.test_controller_interface import (
    TestController,
)
from ocpp.v16.datatypes import (
    ChargingRateUnitType,
)

from ocpp.v16.enums import ChargingProfileStatus, RemoteStartStopStatus, ClearChargingProfileStatus

from ocpp.v16 import call, call_result

# fmt: off
from validations import (validate_composite_schedule,
                               validate_remote_start_stop_transaction,
                               validate_standard_start_transaction)

from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility
from everest.testing.ocpp_utils.fixtures import *
from everest.testing.ocpp_utils.charge_point_v16 import ChargePoint16

from smart_charging_profiles.absolute_profiles import *
from smart_charging_profiles.relative_profiles import *
from smart_charging_profiles.recurring_profiles import *
from smart_charging_profiles.combined_profiles import *
from everest_test_utils import *
# fmt: on


@pytest.mark.asyncio
async def test_absolute_1(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    test_controller.plug_in(connector_id=1)
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    req1 = abs_req1_test1()
    req2 = abs_req2_test1()
    req3 = abs_req3_test1()

    assert await charge_point_v16.set_charging_profile_req(req1) == exp_scp_result
    assert await charge_point_v16.set_charging_profile_req(req2) == exp_scp_result
    assert await charge_point_v16.set_charging_profile_req(req3) == exp_scp_result

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=400)
    )

    passed_seconds = int(
        (
            datetime.utcnow()
            - datetime.fromisoformat(req1.cs_charging_profiles.valid_from)
        ).total_seconds()
    )
    exp = abs_exp_test1(passed_seconds)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_absolute_2(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    req = abs_req1_test2()

    assert await charge_point_v16.set_charging_profile_req(req) == exp_scp_result

    test_controller.plug_in(connector_id=1)
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=300)
    )

    passed_seconds = int(
        (
            datetime.utcnow()
            - datetime.fromisoformat(req.cs_charging_profiles.valid_from)
        ).total_seconds()
    )
    exp = abs_exp_test2(passed_seconds)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_absolute_3(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    test_controller.plug_in(connector_id=1)
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )
    req = abs_req1_test3()

    assert await charge_point_v16.set_charging_profile_req(req) == exp_scp_result

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=300)
    )

    passed_seconds = int(
        (
            datetime.utcnow()
            - datetime.fromisoformat(req.cs_charging_profiles.valid_from)
        ).total_seconds()
    )
    exp = abs_exp_test3(passed_seconds)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )

    test_controller.plug_out()

    await asyncio.sleep(2)


@pytest.mark.asyncio
async def test_absolute_4(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    test_controller.plug_in(connector_id=1)
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    req = abs_req1_test2()

    assert await charge_point_v16.set_charging_profile_req(req) == exp_scp_result

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=300)
    )

    passed_seconds = int(
        (
            datetime.utcnow()
            - datetime.fromisoformat(req.cs_charging_profiles.valid_from)
        ).total_seconds()
    )
    exp = abs_exp_test2(passed_seconds)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_absolute_5(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    req1 = abs_req1_test5()
    req2 = abs_req2_test5()

    assert await charge_point_v16.set_charging_profile_req(req1) == exp_scp_result
    assert await charge_point_v16.set_charging_profile_req(req2) == exp_scp_result

    test_controller.plug_in(connector_id=1)
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=350)
    )

    passed_seconds = int(
        (
            datetime.utcnow()
            - datetime.fromisoformat(req1.cs_charging_profiles.valid_from)
        ).total_seconds()
    )
    exp = abs_exp_test5_1(passed_seconds)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )

    test_utility.messages.clear()

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=550)
    )

    passed_seconds = int(
        (
            datetime.utcnow()
            - datetime.fromisoformat(req1.cs_charging_profiles.valid_from)
        ).total_seconds()
    )
    exp = abs_exp_test5_2(passed_seconds)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )


@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-two-connectors.yaml")
)
@pytest.mark.asyncio
async def test_absolute_6(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):

    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    req1 = abs_req1_test6()
    req2 = abs_req2_test6()
    req3 = abs_req3_test6()

    assert await charge_point_v16.set_charging_profile_req(req1) == exp_scp_result
    assert await charge_point_v16.set_charging_profile_req(req2) == exp_scp_result
    assert await charge_point_v16.set_charging_profile_req(req3) == exp_scp_result

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=0, duration=700)
    )

    exp_con0 = abs_exp_test6_con0()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp_con0,
        validate_composite_schedule,
    )

    test_utility.messages.clear()

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=0, duration=700)
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp_con0,
        validate_composite_schedule,
    )

    test_utility.messages.clear()

    test_controller.plug_in(connector_id=1)
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=900)
    )

    passed_seconds = int(
        (
            datetime.utcnow()
            - datetime.fromisoformat(req1.cs_charging_profiles.valid_from)
        ).total_seconds()
    )

    exp_con1 = abs_exp_test6_con1(passed_seconds)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp_con1,
        validate_composite_schedule,
    )

    test_utility.messages.clear()

    test_controller.plug_in(connector_id=2)
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_2, connector_id=2
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            2, test_config.authorization_info.valid_id_tag_2, 0, ""
        ),
        validate_standard_start_transaction,
    )

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=2, duration=400)
    )

    passed_seconds = int(
        (
            datetime.utcnow()
            - datetime.fromisoformat(req2.cs_charging_profiles.valid_from)
        ).total_seconds()
    )

    exp_con2 = abs_exp_test6_con2(passed_seconds)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp_con2,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
@pytest.mark.skip(
    "Expected behavior when schedules are sent in other unit than composite schedules are requested needs to be discussed."
)
async def test_combined_1(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):

    test_controller.plug_in()
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    req1 = comb_req1_test1()
    req2 = comb_req2_test1()
    exp1 = comb_exp1_test1()
    exp2 = comb_exp2_test1()

    assert await charge_point_v16.set_charging_profile_req(req1) == exp_scp_result
    assert await charge_point_v16.set_charging_profile_req(req2) == exp_scp_result

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(
            connector_id=1, duration=400, charging_rate_unit=ChargingRateUnitType.watts
        )
    )

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(
            connector_id=1, duration=400, charging_rate_unit=ChargingRateUnitType.amps
        )
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp1,
        validate_composite_schedule,
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp2,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_combined_2(charge_point_v16: ChargePoint16, test_utility: TestUtility):

    req = comb_req1_test2()

    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    assert await charge_point_v16.set_charging_profile_req(req) == exp_scp_result

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=0, duration=400)
    )

    exp = comb_exp_test2()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_recurring_1(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):

    test_controller.plug_in()
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    req = rec_req1_test1()
    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.rejected
    )

    assert await charge_point_v16.set_charging_profile_req(req) == exp_scp_result


@pytest.mark.asyncio
async def test_recurring_2(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):

    test_controller.plug_in()
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    req1 = rec_req1_test2()
    req2 = rec_req2_test2()
    exp = rec_exp_test2()

    assert await charge_point_v16.set_charging_profile_req(req1) == exp_scp_result
    assert await charge_point_v16.set_charging_profile_req(req2) == exp_scp_result

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=172800)
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_recurring_3(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):

    test_controller.plug_in()
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    req1 = rec_req1_test3()
    req2 = rec_req2_test3()
    exp = rec_exp_test3()

    assert await charge_point_v16.set_charging_profile_req(req1) == exp_scp_result
    assert await charge_point_v16.set_charging_profile_req(req2) == exp_scp_result

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=172800)
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_relative_1(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):

    req1 = rel_req1_test1()

    test_controller.plug_in()
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    assert await charge_point_v16.set_charging_profile_req(req1) == exp_scp_result

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=400)
    )

    exp = rel_exp_test1()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_relative_2(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):

    req1 = rel_req1_test2()
    req2 = rel_req2_test2()

    test_controller.plug_in()
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    assert await charge_point_v16.set_charging_profile_req(req1) == exp_scp_result
    assert await charge_point_v16.set_charging_profile_req(req2) == exp_scp_result

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=300)
    )

    exp = rel_exp_test2()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_relative_3(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):

    req1 = rel_req1_test3()
    req2 = rel_req2_test3()

    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    assert await charge_point_v16.set_charging_profile_req(req1) == exp_scp_result
    assert await charge_point_v16.set_charging_profile_req(req2) == exp_scp_result

    # start charging session
    test_controller.plug_in()

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(
            RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=90)
    )

    exp = rel_exp_test3()

    # expect correct GetCompositeSchedule.conf
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_clear_charging_profiles_for_connector_0(
    test_config,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
    central_system_v16: CentralSystem,
):

    exp_scp_result = call_result.SetChargingProfilePayload(
        ChargingProfileStatus.accepted
    )

    test_controller.plug_in(connector_id=1)
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    req1 = abs_req1_test1()

    assert await charge_point_v16.set_charging_profile_req(req1) == exp_scp_result

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=400)
    )

    passed_seconds = int(
        (
            datetime.utcnow()
            - datetime.fromisoformat(req1.cs_charging_profiles.valid_from)
        ).total_seconds()
    )
    exp = abs_exp_test1_con0(passed_seconds)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )

    req2 = abs_req2_test_con0()

    assert await charge_point_v16.set_charging_profile_req(req2) == exp_scp_result

    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=400)
    )

    passed_seconds = int(
        (
            datetime.utcnow()
            - datetime.fromisoformat(req2.cs_charging_profiles.valid_from)
        ).total_seconds()
    )
    exp = abs_exp_test2_con0(passed_seconds)
    # expect correct GetCompositeSchedule.conf
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )

    await charge_point_v16.clear_charging_profile_req(id=2)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ClearChargingProfile",
        call_result.ClearChargingProfilePayload(
            ClearChargingProfileStatus.accepted),
    )

    test_controller.stop()
    await asyncio.sleep(2)
    test_controller.start()

    charge_point_v16 = await central_system_v16.wait_for_chargepoint()
    await charge_point_v16.get_composite_schedule(
        call.GetCompositeSchedulePayload(connector_id=1, duration=90)
    )

    passed_seconds = int(
        (
            datetime.utcnow()
            - datetime.fromisoformat(req1.cs_charging_profiles.valid_from)
        ).total_seconds()
    )
    exp = abs_exp_test3_con0(passed_seconds)

    # expect correct GetCompositeSchedule.conf
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp,
        validate_composite_schedule,
    )
