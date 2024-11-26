import pytest_asyncio
import pytest
import logging
from unittest.mock import ANY

from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility
from everest.testing.ocpp_utils.fixtures import *
from everest.testing.core_utils.controller.test_controller_interface import TestController
from everest.testing.core_utils._configuration.libocpp_configuration_helper import GenericOCPP201ConfigAdjustment
from everest_test_utils import *

from ocpp.v201.enums import (IdTokenType as IdTokenTypeEnum, ReserveNowStatusType, ConnectorStatusType,
                             OperationalStatusType, CancelReservationStatusType, SetVariableStatusType,
                             RequestStartStopStatusType, AttributeType)
from ocpp.v201.datatypes import *
from ocpp.v201 import call as call_201
from ocpp.v201 import call_result as call_result201
from validations import (validate_standard_start_transaction,
                         validate_standard_stop_transaction,
                         validate_remote_start_stop_transaction,
                         validate_status_notification_201)
from ocpp.routing import on, create_route_map


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_reservation_local_start_tx(
        test_config: OcppTestConfiguration,
        charge_point_v201: ChargePoint201,
        test_controller: TestController,
        test_utility: TestUtility,
):
    """
    Test making a reservation and start a transaction on the reserved evse id with the correct id token.
    """
    logging.info("######### test_reservation_local_start_tx #########")

    t = datetime.utcnow() + timedelta(minutes=10)

    await charge_point_v201.reserve_now_req(
        id=0,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat(),
        evse_id=1
    )

    # expect ReserveNow response with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.accepted),
    )

    # expect StatusNotification with status reserved
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, ConnectorStatusType.reserved, 1, 1
        ),
    )

    # swipe invalid id tag
    test_controller.swipe(test_config.authorization_info.invalid_id_tag)

    # swipe valid id tag to authorize
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    # expect StatusNotification with status available (reservation is now used)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, ConnectorStatusType.available, 1, 1
        ),
    )

    # start charging session
    test_controller.plug_in()

    # expect TransactionEvent with event type Started and the reservation id.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Started", "reservationId": 0}
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Updated"}
    )

    # expect StatusNotification with status occupied
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, 'Occupied', 1, 1
        ),
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_reservation_remote_start_tx(
    test_config: OcppTestConfiguration,
    charge_point_v201: ChargePoint201,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    Test making a reservation and start a remote transaction on the reserved evse id with the correct id token.
    """
    logging.info("######### test_reservation_remote_start_tx #########")

    t = datetime.utcnow() + timedelta(minutes=10)

    # Make reservation for evse id 1.
    await charge_point_v201.reserve_now_req(
        id=0,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat(),
        evse_id=1
    )

    # expect ReserveNow response with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.accepted),
    )

    # expect StatusNotification with status reserved
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, ConnectorStatusType.reserved, 1, 1
        ),
    )

    # send start transaction request
    await charge_point_v201.request_start_transaction_req(
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443), remote_start_id=1, evse_id=1
    )

    # Which should be accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "RequestStartTransaction",
        call_result201.RequestStartTransactionPayload(status="Accepted"),
        validate_remote_start_stop_transaction,
    )

    # start charging session
    test_controller.plug_in()

    # expect StatusNotification with status available (because reservation is 'used')
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, ConnectorStatusType.available, 1, 1
        ),
    )

    # expect StatusNotification with status occupied
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, 'Occupied', 1, 1
        ),
    )

    # expect StartTransaction with the given reservation id
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Started", "reservationId": 0}
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Updated"}
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_reservation_connector_expire(
    test_config: OcppTestConfiguration,
    charge_point_v201: ChargePoint201,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    Test that a reservation can expire.
    """
    logging.info("######### test_reservation_connector_expire #########")

    # Make a reservation with an expiry time ten seconds from now.
    t = datetime.utcnow() + timedelta(seconds=10)

    await charge_point_v201.reserve_now_req(
        id=5,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat(),
        evse_id=1
    )

    # Reservation is accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.accepted),
    )

    # expect StatusNotification with status reserved
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, ConnectorStatusType.reserved, 1, 1
        ),
    )

    # Request to start transaction for the reserved evse but with another id token.
    await charge_point_v201.request_start_transaction_req(
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_2,
                             type=IdTokenTypeEnum.iso14443), remote_start_id=1, evse_id=1
    )

    # This will not succeed.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "RequestStartTransaction",
        call_result201.RequestStartTransactionPayload(status="Rejected"),
        validate_remote_start_stop_transaction,
    )

    # So we wait until the reservation is expired.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReservationStatusUpdate",
        call_201.ReservationStatusUpdatePayload(
            5, "Expired"
        ),
    )

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, ConnectorStatusType.available, 1, 1
        ),
    )

    # Try to start a transaction now on the previously reserved evse (which reservation is now expired).
    await charge_point_v201.request_start_transaction_req(
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_2,
                             type=IdTokenTypeEnum.iso14443), remote_start_id=1, evse_id=1
    )

    # Now it succeeds.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "RequestStartTransaction",
        call_result201.RequestStartTransactionPayload(status="Accepted"),
        validate_remote_start_stop_transaction,
    )

    # Start charging session
    test_controller.plug_in()

    # expect StatusNotification with status occupied
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, 'Occupied', 1, 1
        ),
    )

    # expect TransactionEvent with event type 'Started'
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Started"}
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Updated"}
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_reservation_connector_faulted(
    test_config: OcppTestConfiguration,
    charge_point_v201: ChargePoint201,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    Test if an evse can be reserved when the evse status is 'Faulted'
    """
    logging.info("######### test_reservation_connector_faulted #########")

    # Set evse in state 'faulted'
    test_controller.raise_error()

    await asyncio.sleep(10)

    t = datetime.utcnow() + timedelta(minutes=10)

    # Send a reserve new request
    await charge_point_v201.reserve_now_req(
        id=0,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat(),
        evse_id=1
    )

    # which should return 'faulted'
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.faulted),
    )

    test_controller.clear_error()


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_reservation_connector_faulted_after_reservation(
    test_config: OcppTestConfiguration,
    charge_point_v201: ChargePoint201,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    Test if a reservation is cancelled after the evse status is 'Faulted'
    """
    logging.info("######### test_reservation_connector_faulted_after_reservation #########")

    t = datetime.utcnow() + timedelta(minutes=10)
    await charge_point_v201.reserve_now_req(
        id=42,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat(),
        evse_id=1
    )

    # Set evse in state 'faulted'
    test_controller.raise_error(1)

    # This should result in the reservation being cancelled.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReservationStatusUpdate",
        call_201.ReservationStatusUpdatePayload(
            42, "Removed"
        ),
    )

    test_controller.clear_error()


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_reservation_connector_occupied(
    test_config: OcppTestConfiguration,
    charge_point_v201: ChargePoint201,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    Try to make a reservation while a evse is occupied.
    """
    logging.info("######### test_reservation_connector_occupied #########")

    # start charging session
    test_controller.plug_in()

    # TODO mz fix comments everywhere in this file!!!
    # expect StatusNotification with status occupied
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, 'Occupied', 1, 1
        ),
    )

    t = datetime.utcnow() + timedelta(minutes=10)

    await asyncio.sleep(2)

    # Request reservation
    await charge_point_v201.reserve_now_req(
        id=0,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat(),
        evse_id=1
    )

    # expect ReserveNow response with status occupied
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.occupied),
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_reservation_connector_unavailable(
    test_config: OcppTestConfiguration,
    charge_point_v201: ChargePoint201,
    test_utility: TestUtility,
):
    """
    Test making a reservation with an unavailable connector, this should return 'unavailable' on a reserve now request.
    """
    logging.info("######### test_reservation_connector_unavailable #########")

    # Set evse id 1 to inoperative.
    await charge_point_v201.change_availablility_req(
        evse={'id': 1}, operational_status=OperationalStatusType.inoperative
    )

    t = datetime.utcnow() + timedelta(seconds=30)

    # Make a reservation for evse id 1.
    await charge_point_v201.reserve_now_req(
        id=0,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat(),
        evse_id=1
    )

    # Which should fail (ReserveNow response 'Unavailable').
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.unavailable),
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP201ConfigAdjustment(
        [
            (
                OCPP201ConfigVariableIdentifier(
                    "ReservationCtrlr", "ReservationCtrlrAvailable", "Actual"
                ),
                "false",
            )
        ]
    )
)
async def test_reservation_connector_rejected(
    test_config: OcppTestConfiguration,
    charge_point_v201: ChargePoint201,
    test_utility: TestUtility,
    test_controller: TestController,
):
    """
    Test if reservation is rejected with the reservation ctrlr is not available.
    """
    logging.info("######### test_reservation_connector_rejected #########")

    t = datetime.utcnow() + timedelta(seconds=10)

    # Try to make reservation.
    await charge_point_v201.reserve_now_req(
        id=5,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat(),
        evse_id=1
    )

    # expect ReserveNow response with status rejected
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.rejected),
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP201ConfigAdjustment(
        [
            (
                OCPP201ConfigVariableIdentifier(
                    "ReservationCtrlr", "ReservationCtrlrNonEvseSpecific", "Actual"
                ),
                "false",
            )
        ]
    )
)
async def test_reservation_non_evse_specific_rejected(
        test_config: OcppTestConfiguration,
        charge_point_v201: ChargePoint201,
        test_utility: TestUtility,
        test_controller: TestController,
):
    """
    Try to make a non-evse specific reservation, while that is not allowed according to the settings. That should fail.
    """
    logging.info("######### test_reservation_non_evse_specific_rejected #########")

    t = datetime.utcnow() + timedelta(seconds=30)

    # Try to make a reservation without evse id.
    await charge_point_v201.reserve_now_req(
        id=5,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat()
    )

    # expect ReserveNow respone with status rejected
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.rejected),
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP201ConfigAdjustment(
        [
            (
                OCPP201ConfigVariableIdentifier(
                    "ReservationCtrlr", "ReservationCtrlrNonEvseSpecific", "Actual"
                ),
                "true",
            )
        ]
    )
)
async def test_reservation_non_evse_specific_accepted(
        test_config: OcppTestConfiguration,
        charge_point_v201: ChargePoint201,
        test_utility: TestUtility,
        test_controller: TestController,
):
    """
    Try to make a non evse specific reservation. This should succeed, according to the settings (devicemodel).
    """
    logging.info("######### test_reservation_non_evse_specific_accepted #########")

    t = datetime.utcnow() + timedelta(seconds=30)

    # Make reservation without evse id.
    await charge_point_v201.reserve_now_req(
        id=5,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat()
    )

    # This should be accepted.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.accepted),
    )

    # swipe valid id tag to authorize
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    # start charging session
    test_controller.plug_in()

    # expect TransactionEvent with event type 'Started' and the correct reservation id.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Started", "reservationId": 5}
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Updated"}
    )

    # expect StatusNotification with status occupied
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, 'Occupied', 1, 1
        ),
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP201ConfigAdjustment(
        [
            (
                OCPP201ConfigVariableIdentifier(
                    "ReservationCtrlr", "ReservationCtrlrNonEvseSpecific", "Actual"
                ),
                "true",
            )
        ]
    )
)
async def test_reservation_non_evse_specific_accepted_multiple(
        test_config: OcppTestConfiguration,
        charge_point_v201: ChargePoint201,
        test_utility: TestUtility,
        test_controller: TestController,
):
    """
    Try to make a non evse specific reservation. This should succeed, according to the settings (devicemodel).
    When making multiple reservations, as soon as there are as many reservations as evse's available, the evse's should
    go to occupied.
    """
    logging.info("######### test_reservation_non_evse_specific_accepted_multiple #########")

    t = datetime.utcnow() + timedelta(seconds=30)

    # Make reservation
    await charge_point_v201.reserve_now_req(
        id=5,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat()
    )

    # Expect it to be accepted.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.accepted),
    )

    # Make another reservation with another reservation id.
    await charge_point_v201.reserve_now_req(
        id=6,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat()
    )

    # expect This should be accepted as well.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.accepted),
    )

    # There are now as many reservations as evse's, so all evse's go to 'reserved'.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, ConnectorStatusType.reserved, 1, 1
        ),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, ConnectorStatusType.reserved, 2, 1
        ),
    )

    # There are now as many reservations as evse's, so another reservation is not possible.
    await charge_point_v201.reserve_now_req(
        id=7,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_2,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat()
    )

    # expect ReserveNow response with status occupied
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.occupied),
    )

    # swipe valid id tag to authorize
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    # start charging session
    test_controller.plug_in()

    # expect TransactionEvent 'Started' with the correct reservation id.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Started", "reservationId": 5}
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Updated"}
    )

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, 'Occupied', 1, 1
        ),
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_reservation_faulted_state(
    test_config: OcppTestConfiguration,
    charge_point_v201: ChargePoint201,
    test_utility: TestUtility,
    test_controller: TestController,
):
    """
    Test if making a reservation is possible if the evse is in faulted state.
    """
    logging.info("######### test_reservation_faulted_state #########")

    test_controller.raise_error(1)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, 'Faulted', 1, 1
        ),
    )

    t = datetime.utcnow() + timedelta(seconds=30)

    # Try to make the reservation.
    await charge_point_v201.reserve_now_req(
        id=0,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat(),
        evse_id=1
    )

    # expect ReserveNow response with status 'Faulted'
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.faulted),
    )

    test_controller.clear_error(1)


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_reservation_cancel(
    test_config: OcppTestConfiguration,
    charge_point_v201: ChargePoint201,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    Test if a reservation can be cancelled.
    """
    logging.info("######### test_reservation_cancel #########")

    t = datetime.utcnow() + timedelta(minutes=10)

    # Make the reservation
    await charge_point_v201.reserve_now_req(
        id=5,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat(),
        evse_id=1
    )

    # Expect it to be accepted.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.accepted),
    )

    # expect StatusNotification request with status reserved
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, ConnectorStatusType.reserved, 1, 1
        ),
    )

    # Cancel the reservation.
    await charge_point_v201.cancel_reservation_req(reservation_id=5)

    # expect CancelReservation response with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "CancelReservation",
        call_result201.CancelReservationPayload(status=CancelReservationStatusType.accepted),
    )

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, ConnectorStatusType.available, 1, 1
        ),
    )

    # start charging session
    test_controller.plug_in()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, 'Occupied', 1, 1
        ),
    )

    # send request start transaction
    await charge_point_v201.request_start_transaction_req(
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443), remote_start_id=1, evse_id=1
    )

    # Which should be accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "RequestStartTransaction",
        call_result201.RequestStartTransactionPayload(status="Accepted"),
        validate_remote_start_stop_transaction,
    )

    # expect TransactionEvent with eventType 'Started'
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Started"}
    )

    # expect TransactionEvent with status 'Updated'
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Updated"}
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_reservation_cancel_rejected(
    test_config: OcppTestConfiguration,
    charge_point_v201: ChargePoint201,
    test_utility: TestUtility,
):
    """
    Try to cancel a non existing reservation
    """
    logging.info("######### test_reservation_cancel_rejected #########")

    t = datetime.utcnow() + timedelta(minutes=10)

    # Make a reservation with reservation id 5
    await charge_point_v201.reserve_now_req(
        id=5,
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        expiry_date_time=t.isoformat(),
        evse_id=1
    )

    # expect ReserveNow response with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.accepted),
    )

    # expect StatusNotification with status reserved
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, ConnectorStatusType.reserved, 1, 1
        ),
    )

    # Try to cancel reservation with reservation id 2, which does not exist.
    await charge_point_v201.cancel_reservation_req(reservation_id=2)

    # expect CancelReservation response with status rejected
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "CancelReservation",
        call_result201.CancelReservationPayload(status=CancelReservationStatusType.rejected),
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_reservation_with_parentid(
    test_config: OcppTestConfiguration,
    charge_point_v201: ChargePoint201,
    test_utility: TestUtility,
    test_controller: TestController,
    central_system_v201: CentralSystem
):
    """
    Test reservation with parent id.
    """
    logging.info("######### test_reservation_with_parentid #########")

    # authorize.conf with parent id tag
    @on(Action.Authorize)
    def on_authorize(**kwargs):
        id_tag_info = IdTokenInfoType(
            status=AuthorizationStatusType.accepted,
            group_id_token=IdTokenType(
                id_token=test_config.authorization_info.parent_id_tag, type=IdTokenTypeEnum.iso14443
            ),
        )
        return call_result201.AuthorizePayload(id_token_info=id_tag_info)

    setattr(charge_point_v201, "on_authorize", on_authorize)
    central_system_v201.chargepoint.route_map = create_route_map(
        central_system_v201.chargepoint
    )
    charge_point_v201.route_map = create_route_map(charge_point_v201)

    r: call_result201.SetVariablesPayload = (
        await charge_point_v201.set_config_variables_req(
            "AuthCtrlr", "AuthorizeRemoteStart", "true"
        )
    )

    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # Disable remote authorization so an 'Authorize' request is sent when starting remotely.
    r: call_result201.SetVariablesPayload = (
        await charge_point_v201.set_config_variables_req(
            "AuthCtrlr", "DisableRemoteAuthorization", "false"
        )
    )

    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    t = datetime.utcnow() + timedelta(minutes=10)

    # Make a new reservation.
    await charge_point_v201.reserve_now_req(
        evse_id=1,
        expiry_date_time=t.isoformat(),
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_1,
                             type=IdTokenTypeEnum.iso14443),
        group_id_token=IdTokenType(id_token=test_config.authorization_info.parent_id_tag,
                             type=IdTokenTypeEnum.iso14443),
        id=0,
    )

    # expect ReserveNow response with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "ReserveNow",
        call_result201.ReserveNowPayload(ReserveNowStatusType.accepted),
    )

    # expect StatusNotification request with status reserved
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call_201.StatusNotificationPayload(
            ANY, ConnectorStatusType.reserved, 1, 1
        ),
    )

    # start charging session
    test_controller.plug_in()

    # send request start transaction for another id tag than the one from the reservation.
    await charge_point_v201.request_start_transaction_req(
        id_token=IdTokenType(id_token=test_config.authorization_info.valid_id_tag_2,
                             type=IdTokenTypeEnum.iso14443), remote_start_id=1, evse_id=1
    )

    # This is accepted because the reservation has a group id token.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "RequestStartTransaction",
        call_result201.RequestStartTransactionPayload(status="Accepted"),
        validate_remote_start_stop_transaction,
    )

    # expect Authorize request.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "Authorize",
        call_201.AuthorizePayload(IdTokenType(id_token=test_config.authorization_info.valid_id_tag_2,
                                              type=IdTokenTypeEnum.iso14443)),
    )

    # Authorize was accepted because of the correct group id token, transaction is started.
    r: call_201.TransactionEventPayload = call_201.TransactionEventPayload(
        **await wait_for_and_validate(
            test_utility,
            charge_point_v201,
            "TransactionEvent",
            {"eventType": "Started"}
        )
    )

    transaction = TransactionType(**r.transaction_info)

    # expect TransactionEvent with eventType 'Updated'
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Updated"}
    )

    # send request stop transaction
    await charge_point_v201.request_stop_transaction_req(
        transaction_id=transaction.transaction_id
    )

    # Which should be accepted.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "RequestStopTransaction",
        call_result201.RequestStartTransactionPayload(
            status=RequestStartStopStatusType.accepted
        ),
    )

    # And the session should end.
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Ended"}
    )
