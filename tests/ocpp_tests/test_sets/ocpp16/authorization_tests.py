# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
from datetime import datetime, timedelta
import logging
import asyncio

from everest.testing.core_utils.controller.test_controller_interface import (
    TestController,
)
from ocpp.routing import on, create_route_map
from ocpp.v16.datatypes import (
    IdTagInfo,
)
from ocpp.v16 import call, call_result
from ocpp.v16.enums import *

# fmt: off
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility
from everest.testing.ocpp_utils.fixtures import *
from everest.testing.ocpp_utils.charge_point_v16 import ChargePoint16
from everest_test_utils import *
from validations import (validate_standard_start_transaction,
                               validate_standard_stop_transaction,
                               validate_remote_start_stop_transaction,
                               wait_for_callerror_and_validate
                               )
# fmt: on


@pytest.mark.asyncio
async def test_authorize_parent_id_1(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):
    # authorize.conf with parent id tag

    @on(Action.Authorize)
    def on_authorize(**kwargs):
        id_tag_info = IdTagInfo(
            status=AuthorizationStatus.accepted,
            parent_id_tag=test_config.authorization_info.parent_id_tag,
        )
        return call_result.AuthorizePayload(id_tag_info=id_tag_info)

    setattr(charge_point_v16, "on_authorize", on_authorize)
    charge_point_v16.route_map = create_route_map(charge_point_v16)

    await charge_point_v16.change_configuration_req(
        key="AuthorizeRemoteTxRequests", value="true"
    )

    test_controller.plug_in()

    test_controller.swipe(test_config.authorization_info.valid_id_tag_2)
    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_2),
    )

    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_2, 0, ""
        ),
        validate_standard_start_transaction,
    )

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    test_utility.messages.clear()
    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    await asyncio.sleep(1)

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.local),
        validate_standard_stop_transaction,
    )

    test_controller.plug_out()

    # expect StatusNotification.req with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_authorize_invalid(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):
    # authorize.conf with parent id tag
    @on(Action.Authorize)
    def on_authorize(**kwargs):
        id_tag_info = IdTagInfo(status=AuthorizationStatus.invalid)
        return call_result.AuthorizePayload(id_tag_info=id_tag_info)

    setattr(charge_point_v16, "on_authorize", on_authorize)
    charge_point_v16.route_map = create_route_map(charge_point_v16)

    await charge_point_v16.change_configuration_req(
        key="AuthorizeRemoteTxRequests", value="true"
    )
    await charge_point_v16.change_configuration_req(key="HeartbeatInterval", value="5")

    test_controller.plug_in()

    # expect StatusNotification with status preparing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.preparing
        ),
    )

    test_controller.swipe(test_config.authorization_info.invalid_id_tag)
    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.invalid_id_tag),
    )

    test_utility.messages.clear()

    test_utility.forbidden_actions.append("StatusNotification")
    test_utility.forbidden_actions.append("StartTransaction")

    assert await wait_for_and_validate(
        test_utility, charge_point_v16, "Heartbeat", call.HeartbeatPayload()
    )


@pytest.mark.asyncio
async def test_parent_id_tag_reservation_1(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):

    # authorize.conf with parent id tag
    @on(Action.Authorize)
    def on_authorize(**kwargs):
        id_tag_info = IdTagInfo(
            status=AuthorizationStatus.accepted,
            parent_id_tag=test_config.authorization_info.parent_id_tag,
        )
        return call_result.AuthorizePayload(id_tag_info=id_tag_info)

    setattr(charge_point_v16, "on_authorize", on_authorize)
    charge_point_v16.route_map = create_route_map(charge_point_v16)

    await charge_point_v16.change_configuration_req(
        key="AuthorizeRemoteTxRequests", value="true"
    )

    t = datetime.utcnow() + timedelta(minutes=10)

    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date=t.isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
        parent_id_tag=test_config.authorization_info.parent_id_tag,
        reservation_id=0,
    )

    # expect ReserveNow.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ReserveNow",
        call_result.ReserveNowPayload(ReservationStatus.accepted),
    )

    # expect StatusNotification.req with status reserved
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.reserved
        ),
    )

    test_controller.plug_in()

    test_controller.swipe(test_config.authorization_info.valid_id_tag_2)
    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_2),
    )

    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_2, 0, ""
        ),
        validate_standard_start_transaction,
    )

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    test_utility.messages.clear()
    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    await asyncio.sleep(1)

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.local),
        validate_standard_stop_transaction,
    )

    test_controller.plug_out()

    # expect StatusNotification.req with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_parent_id_tag_reservation_2(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):
    # authorize.conf with parent id tag
    @on(Action.Authorize)
    def on_authorize(**kwargs):
        id_tag_info = IdTagInfo(
            status=AuthorizationStatus.accepted,
            parent_id_tag=test_config.authorization_info.parent_id_tag,
        )
        return call_result.AuthorizePayload(id_tag_info=id_tag_info)

    setattr(charge_point_v16, "on_authorize", on_authorize)
    charge_point_v16.route_map = create_route_map(charge_point_v16)

    await charge_point_v16.change_configuration_req(
        key="AuthorizeRemoteTxRequests", value="true"
    )

    t = datetime.utcnow() + timedelta(minutes=10)

    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date=t.isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
        parent_id_tag=test_config.authorization_info.parent_id_tag,
        reservation_id=0,
    )

    # expect ReserveNow.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ReserveNow",
        call_result.ReserveNowPayload(ReservationStatus.accepted),
    )

    # expect StatusNotification.req with status reserved
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.reserved
        ),
    )

    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_2, connector_id=1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_2),
    )

    test_controller.plug_in()

    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_2, 0, ""
        ),
        validate_standard_start_transaction,
    )

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    test_utility.messages.clear()
    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    await asyncio.sleep(1)

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.local),
        validate_standard_stop_transaction,
    )

    test_controller.plug_out()

    # expect StatusNotification.req with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_parent_id_tag_reservation_3(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):

    await charge_point_v16.change_configuration_req(
        key="AuthorizeRemoteTxRequests", value="true"
    )

    t = datetime.utcnow() + timedelta(minutes=10)

    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date=t.isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
        parent_id_tag=test_config.authorization_info.parent_id_tag,
        reservation_id=0,
    )

    # expect ReserveNow.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ReserveNow",
        call_result.ReserveNowPayload(ReservationStatus.accepted),
    )

    # expect StatusNotification.req with status reserved
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.reserved
        ),
    )

    test_controller.plug_in()

    await asyncio.sleep(1)

    logging.info("disconnect the ws connection...")
    test_controller.disconnect_websocket()

    await asyncio.sleep(1)

    test_controller.swipe(test_config.authorization_info.valid_id_tag_2)

    await asyncio.sleep(1)

    logging.info("connecting the ws connection")
    test_controller.connect_websocket()

    # wait for reconnect
    charge_point_v16 = await central_system_v16.wait_for_chargepoint(
        wait_for_bootnotification=False
    )

    await charge_point_v16.trigger_message_req(
        requested_message=MessageTrigger.status_notification, connector_id=1
    )
    # expect TriggerMessage.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "TriggerMessage",
        call_result.TriggerMessagePayload(TriggerMessageStatus.accepted),
    )
    # expect StatusNotification.req with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.reserved
        ),
    )


@pytest.mark.asyncio
async def test_authorization_cache_entry_1(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):
    @on(Action.Authorize)
    def on_authorize(**kwargs):
        id_tag_info = IdTagInfo(
            status=AuthorizationStatus.accepted,
            parent_id_tag=test_config.authorization_info.parent_id_tag,
        )
        return call_result.AuthorizePayload(id_tag_info=id_tag_info)

    @on(Action.StartTransaction)
    def on_start_transaction(**kwargs):
        id_tag_info = IdTagInfo(
            status=AuthorizationStatus.accepted,
            parent_id_tag=test_config.authorization_info.valid_id_tag_1,
        )
        return call_result.StartTransactionPayload(
            transaction_id=1, id_tag_info=id_tag_info
        )

    setattr(charge_point_v16, "on_authorize", on_authorize)
    setattr(charge_point_v16, "on_start_transaction", on_start_transaction)
    charge_point_v16.route_map = create_route_map(charge_point_v16)

    await charge_point_v16.change_configuration_req(
        key="AllowOfflineTxForUnknownId", value="false"
    )
    await charge_point_v16.change_configuration_req(
        key="LocalAuthorizeOffline", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="LocalPreAuthorize", value="true"
    )
    await charge_point_v16.get_configuration_req(key=["LocalAuthListEnabled"])
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        call_result.GetConfigurationPayload(
            [{"key": "LocalAuthListEnabled", "readonly": False, "value": "true"}]
        ),
    )

    test_controller.plug_in()

    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)
    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_1),
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    test_utility.messages.clear()
    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    await asyncio.sleep(1)

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.local),
        validate_standard_stop_transaction,
    )

    test_utility.messages.clear()
    test_controller.plug_out()

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )

    test_controller.plug_in()

    test_controller.swipe(test_config.authorization_info.valid_id_tag_2)
    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_2),
    )

    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.valid_id_tag_2, 0, ""
        ),
        validate_standard_start_transaction,
    )

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    test_utility.messages.clear()
    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.valid_id_tag_2)

    await asyncio.sleep(1)

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.local),
        validate_standard_stop_transaction,
    )

    test_controller.plug_out()

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )

    test_utility.messages.clear()

    logging.info("disconnect the ws connection...")
    test_controller.disconnect_websocket()

    await asyncio.sleep(2)

    # swipe card and authorize this by cache
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)
    test_controller.plug_in()

    await asyncio.sleep(5)

    logging.info("connecting the ws connection")
    test_controller.connect_websocket()

    await asyncio.sleep(2)

    # wait for reconnect
    charge_point_v16 = await central_system_v16.wait_for_chargepoint(
        wait_for_bootnotification=False
    )

    await charge_point_v16.trigger_message_req(
        requested_message=MessageTrigger.status_notification, connector_id=1
    )
    # expect TriggerMessage.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "TriggerMessage",
        call_result.TriggerMessagePayload(TriggerMessageStatus.accepted),
    )
    # expect StatusNotification.req with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    test_utility.messages.clear()
    # swipe card
    test_controller.swipe(test_config.authorization_info.valid_id_tag_2)

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.local),
        validate_standard_stop_transaction,
    )


@pytest.mark.asyncio
async def test_authorization_cache_entry_2(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):
    @on(Action.Authorize)
    def on_authorize(**kwargs):
        id_tag_info = IdTagInfo(status=AuthorizationStatus.accepted)
        return call_result.AuthorizePayload(id_tag_info=id_tag_info)

    @on(Action.StartTransaction)
    def on_start_transaction(**kwargs):
        id_tag_info = IdTagInfo(status=AuthorizationStatus.accepted)
        return call_result.StartTransactionPayload(
            transaction_id=1, id_tag_info=id_tag_info
        )

    setattr(charge_point_v16, "on_authorize", on_authorize)
    setattr(charge_point_v16, "on_start_transaction", on_start_transaction)
    charge_point_v16.route_map = create_route_map(charge_point_v16)

    await charge_point_v16.change_configuration_req(
        key="AllowOfflineTxForUnknownId", value="false"
    )
    await charge_point_v16.change_configuration_req(
        key="LocalAuthorizeOffline", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="LocalPreAuthorize", value="true"
    )
    await charge_point_v16.get_configuration_req(key=["LocalAuthListEnabled"])
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        call_result.GetConfigurationPayload(
            [{"key": "LocalAuthListEnabled", "readonly": False, "value": "true"}]
        ),
    )

    test_controller.plug_in()

    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)
    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_1),
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    test_utility.messages.clear()
    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    await asyncio.sleep(1)

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.local),
        validate_standard_stop_transaction,
    )

    test_controller.plug_out()

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )

    test_utility.messages.clear()
    test_utility.forbidden_actions.append("Authorize")

    test_controller.plug_in()

    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )


@pytest.mark.asyncio
async def test_authorization_cache_entry_3(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):
    @on(Action.Authorize)
    def on_authorize(**kwargs):
        # accepted but expires just now
        id_tag_info = IdTagInfo(
            status=AuthorizationStatus.accepted,
            expiry_date=datetime.utcnow().isoformat(),
        )
        return call_result.AuthorizePayload(id_tag_info=id_tag_info)

    @on(Action.StartTransaction)
    def on_start_transaction(**kwargs):
        id_tag_info = IdTagInfo(
            status=AuthorizationStatus.accepted,
            expiry_date=datetime.utcnow().isoformat(),
        )
        return call_result.StartTransactionPayload(
            transaction_id=1, id_tag_info=id_tag_info
        )

    setattr(charge_point_v16, "on_authorize", on_authorize)
    setattr(charge_point_v16, "on_start_transaction", on_start_transaction)
    charge_point_v16.route_map = create_route_map(charge_point_v16)

    await charge_point_v16.change_configuration_req(
        key="AllowOfflineTxForUnknownId", value="false"
    )
    await charge_point_v16.change_configuration_req(
        key="LocalAuthorizeOffline", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="LocalPreAuthorize", value="true"
    )
    await charge_point_v16.get_configuration_req(key=["LocalAuthListEnabled"])
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        call_result.GetConfigurationPayload(
            [{"key": "LocalAuthListEnabled", "readonly": False, "value": "true"}]
        ),
    )

    test_controller.plug_in()

    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)
    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_1),
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    test_utility.messages.clear()
    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    await asyncio.sleep(1)

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.local),
        validate_standard_stop_transaction,
    )

    test_controller.plug_out()

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )

    test_utility.messages.clear()

    test_controller.plug_in()

    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_1),
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )


@pytest.mark.asyncio
async def test_swipe_on_finishing(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):

    test_controller.plug_in()
    await asyncio.sleep(1)
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)
    await asyncio.sleep(1)

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
    await asyncio.sleep(2)

    test_utility.messages.clear()
    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    await asyncio.sleep(1)

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.local),
        validate_standard_stop_transaction,
    )

    test_utility.messages.clear()
    test_utility.forbidden_actions.append("StopTransaction")

    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    test_controller.plug_out()

    # expect StatusNotification.req with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-two-connectors.yaml")
)
@pytest.mark.asyncio
async def test_remote_start_transaction_no_connector(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):

    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    test_controller.plug_in(connector_id=1)

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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    test_utility.messages.clear()

    test_utility.forbidden_actions.append("StopTransaction")

    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_2
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    test_controller.plug_in(connector_id=2)

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


@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-two-connectors.yaml")
)
@pytest.mark.asyncio
async def test_remote_start_transaction_single_connector(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):

    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=2
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    test_controller.plug_in(connector_id=1)

    await asyncio.sleep(1)

    test_controller.plug_in(connector_id=2)

    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            2, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            2, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )


@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-two-connectors.yaml")
)
@pytest.mark.asyncio
async def test_double_remote_start_transaction(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):

    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    test_controller.plug_in(connector_id=1)

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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    test_utility.messages.clear()

    test_utility.forbidden_actions.append("StopTransaction")

    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    test_controller.plug_in(connector_id=2)

    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            2, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )


@pytest.mark.asyncio
async def test_send_local_lost_large_token(
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController
):

    from ocpp.messages import Call, _DecimalEncoder

    async def send_message_without_validation(charge_point_v16, call_msg):
        json_data = json.dumps(
            [
                call_msg.message_type_id,
                call_msg.unique_id,
                call_msg.action,
                call_msg.payload,
            ],
            # By default json.dumps() adds a white space after every separator.
            # By setting the separator manually that can be avoided.
            separators=(",", ":"),
            cls=_DecimalEncoder,
        )

        async with charge_point_v16._call_lock:
            await charge_point_v16._send(json_data)

    payload = call.SendLocalListPayload(
        list_version=1,
        update_type=UpdateType.differential,
        local_authorization_list=[{
            "idTag": "RFID111111111111111111111111111",
            "idTagInfo": {
                "status": "Accepted",
                "expiryDate": "2342-06-19T09:10:00.000Z",
                "parentIdTag": "PTAG",
            },
        }],
    )
    camel_case_payload = snake_to_camel_case(asdict(payload))

    call_msg = Call(
        unique_id=str(charge_point_v16._unique_id_generator()),
        action=payload.__class__.__name__[:-7],
        payload=remove_nones(camel_case_payload),
    )

    await send_message_without_validation(charge_point_v16, call_msg)

    assert await wait_for_callerror_and_validate(
        test_utility, charge_point_v16, "FormationViolation"
    )
