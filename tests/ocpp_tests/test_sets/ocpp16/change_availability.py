import pytest

from ocpp.v16.enums import AvailabilityType, AvailabilityStatus, ReservationStatus, CancelReservationStatus
from ocpp.v16.call_result import ChangeAvailabilityPayload, ReserveNowPayload, CancelReservationPayload

from everest.testing.ocpp_utils.charge_point_utils import (
    wait_for_and_validate,
    TestUtility,
)
from everest.testing.ocpp_utils.fixtures import *
from everest_test_utils import get_everest_config_path_str
from everest.testing.ocpp_utils.charge_point_v16 import ChargePoint16
from everest.testing.core_utils.controller.test_controller_interface import (
    TestController,
)


@pytest.mark.asyncio
async def test_change_availability(
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility
):

    r: ChangeAvailabilityPayload = await charge_point_v16.change_availability_req(
        type=AvailabilityType.inoperative, connector_id=1
    )

    assert r.status == AvailabilityStatus.accepted

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 1, "status": "Unavailable", "errorCode": "NoError"},
    )

    # verify the same request is accepted and no further StatusNotification.req is sent
    r: ChangeAvailabilityPayload = await charge_point_v16.change_availability_req(
        type=AvailabilityType.inoperative, connector_id=1
    )

    assert r.status == AvailabilityStatus.accepted

    test_utility.messages.clear()
    test_utility.forbidden_actions.append("StatusNotification")

    # verify connector can be set back to operational
    r: ChangeAvailabilityPayload = await charge_point_v16.change_availability_req(
        type=AvailabilityType.operative, connector_id=1
    )

    assert r.status == AvailabilityStatus.accepted

    test_utility.forbidden_actions.clear()
    test_utility.messages.clear()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 1, "status": "Available", "errorCode": "NoError"},
    )


@pytest.mark.asyncio
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-two-connectors.yaml")
)
async def test_change_availability_connector_zero(
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility
):

    r: ChangeAvailabilityPayload = await charge_point_v16.change_availability_req(
        type=AvailabilityType.inoperative, connector_id=0
    )

    assert r.status == AvailabilityStatus.accepted

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 0, "status": "Unavailable", "errorCode": "NoError"},
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 1, "status": "Unavailable", "errorCode": "NoError"},
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 2, "status": "Unavailable", "errorCode": "NoError"},
    )

    # verify connector can be set back to operational
    r: ChangeAvailabilityPayload = await charge_point_v16.change_availability_req(
        type=AvailabilityType.operative, connector_id=0
    )

    assert r.status == AvailabilityStatus.accepted

    test_utility.messages.clear()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 0, "status": "Available", "errorCode": "NoError"},
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 1, "status": "Available", "errorCode": "NoError"},
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 2, "status": "Available", "errorCode": "NoError"},
    )


@pytest.mark.asyncio
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-two-connectors.yaml")
)
async def test_change_availability_scheduled_in_preparing(
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):

    test_controller.plug_in()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 1, "status": "Preparing", "errorCode": "NoError"},
    )

    r: ChangeAvailabilityPayload = await charge_point_v16.change_availability_req(
        type=AvailabilityType.inoperative, connector_id=0
    )

    assert r.status == AvailabilityStatus.scheduled

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 0, "status": "Unavailable", "errorCode": "NoError"},
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 2, "status": "Unavailable", "errorCode": "NoError"},
    )

    test_utility.messages.clear()
    test_controller.plug_out()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 1, "status": "Unavailable", "errorCode": "NoError"},
    )


@pytest.mark.asyncio
async def test_change_availability_scheduled_in_transaction(
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):

    test_controller.plug_in()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 1, "status": "Preparing", "errorCode": "NoError"},
    )

    test_controller.swipe("DEADBEEF")

    assert await wait_for_and_validate(
        test_utility, charge_point_v16, "StartTransaction", {"connectorId": 1}
    )

    test_utility.messages.clear()

    r: ChangeAvailabilityPayload = await charge_point_v16.change_availability_req(
        type=AvailabilityType.inoperative, connector_id=1
    )

    assert r.status == AvailabilityStatus.scheduled

    test_controller.plug_out()

    assert await wait_for_and_validate(
        test_utility, charge_point_v16, "StopTransaction", {"reason": "EVDisconnected"}
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 1, "status": "Unavailable", "errorCode": "NoError"},
    )

@pytest.mark.asyncio
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-two-connectors.yaml")
)
async def test_change_availability_scheduled_in_reserved(
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility
):
    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date="2090-06-19T09:10:00.000Z",
        id_tag="RFID1",
        reservation_id=0,
    )

    # expect ReserveNow.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ReserveNow",
        ReserveNowPayload(ReservationStatus.accepted),
    )
    
    ## end

    r: ChangeAvailabilityPayload = await charge_point_v16.change_availability_req(
        type=AvailabilityType.inoperative, connector_id=0
    )

    assert r.status == AvailabilityStatus.scheduled

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 0, "status": "Unavailable", "errorCode": "NoError"},
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 2, "status": "Unavailable", "errorCode": "NoError"},
    )

    test_utility.messages.clear()
    await charge_point_v16.cancel_reservation_req(reservation_id=0)

    # expect CancelReservation.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "CancelReservation",
        CancelReservationPayload(CancelReservationStatus.accepted),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        {"connectorId": 1, "status": "Unavailable", "errorCode": "NoError"},
    )