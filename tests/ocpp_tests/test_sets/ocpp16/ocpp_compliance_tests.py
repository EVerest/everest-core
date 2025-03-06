# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from datetime import datetime, timedelta
import logging
import asyncio

from everest.testing.core_utils.controller.test_controller_interface import (
    TestController,
)

# fmt: off

from validations import (
    dont_validate_meter_values,
    dont_validate_sign_certificate,
    validate_composite_schedule, validate_get_log,
    validate_meter_values,
    validate_remote_start_stop_transaction,
    validate_standard_start_transaction,
    validate_standard_stop_transaction,
    validate_boot_notification
)

from everest.testing.ocpp_utils.fixtures import *
from everest.testing.ocpp_utils.charge_point_v16 import ChargePoint16
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility, ValidationMode
from everest.testing.core_utils._configuration.libocpp_configuration_helper import GenericOCPP16ConfigAdjustment
from everest_test_utils import *
# fmt: on

from ocpp.v16.enums import *
from ocpp.v16.datatypes import *
from ocpp.v16 import call, call_result
from ocpp.routing import create_route_map, on
from ocpp.messages import unpack


@pytest.mark.asyncio
async def test_reset_to_idle(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_reset_to_idle #########")
    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value="0"
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )
    await charge_point_v16.change_configuration_req(
        key="ClockAlignedDataInterval", value="0"
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )
    await charge_point_v16.change_configuration_req(
        key="LocalPreAuthorize", value="false"
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )

    await charge_point_v16.change_availability_req(
        connector_id=0, type=AvailabilityType.operative
    )

    await charge_point_v16.get_configuration_req(key=["AuthorizationCacheEnabled"])
    await charge_point_v16.get_configuration_req(key=["LocalAuthListEnabled"])

    await charge_point_v16.send_local_list_req(
        list_version=0, update_type=UpdateType.full
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SendLocalList",
        call_result.SendLocalListPayload(UpdateStatus.accepted),
    )

    await charge_point_v16.get_configuration_req(key=["MaxChargingProfilesInstalled"])

    await charge_point_v16.clear_charging_profile_req(
        id=1, connector_id=1, charging_profile_purpose="TxDefaultProfile", stack_level=0
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ClearChargingProfile",
        call_result.ClearChargingProfilePayload(ClearChargingProfileStatus.accepted),
    )


@pytest.mark.asyncio
async def test_stop_tx(test_controller: TestController, test_utility: TestUtility):
    logging.info("######### test_stop_tx #########")
    pass


@pytest.mark.asyncio
async def test_cold_boot(
    central_system_v16: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_cold_boot #########")

    test_controller.start()
    charge_point_v16 = await central_system_v16.wait_for_chargepoint()

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )

    await charge_point_v16.change_configuration_req(key="HeartbeatInterval", value="2")
    assert await wait_for_and_validate(
        test_utility, charge_point_v16, "Heartbeat", call.HeartbeatPayload()
    )


@pytest.mark.asyncio
async def test_cold_boot_pending(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_cold_boot_pending #########")

    @on(Action.BootNotification)
    def on_boot_notification_pending(**kwargs):
        return call_result.BootNotificationPayload(
            current_time=datetime.utcnow().isoformat(),
            interval=10,
            status=RegistrationStatus.pending,
        )

    @on(Action.BootNotification)
    def on_boot_notification_accepted(**kwargs):
        return call_result.BootNotificationPayload(
            current_time=datetime.utcnow().isoformat(),
            interval=5,
            status=RegistrationStatus.accepted,
        )

    central_system_v16.function_overrides.append(
        ("on_boot_notification", on_boot_notification_pending)
    )

    test_controller.start()
    charge_point_v16 = await central_system_v16.wait_for_chargepoint()
    charge_point_v16.pipe = True

    response = await charge_point_v16.get_configuration_req()
    assert len(response.configuration_key) > 20

    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value="10"
    )

    setattr(charge_point_v16, "on_boot_notification", on_boot_notification_accepted)
    central_system_v16.chargepoint.route_map = create_route_map(
        central_system_v16.chargepoint
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "BootNotification",
        call.BootNotificationPayload(
            test_config.charge_point_info.charge_point_model,
            charge_box_serial_number=test_config.charge_point_info.charge_point_id,
            charge_point_vendor=test_config.charge_point_info.charge_point_vendor,
            firmware_version=test_config.charge_point_info.firmware_version,
        ),
        validate_boot_notification,
    )

    # expect StatusNotification.req with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )

    assert await wait_for_and_validate(
        test_utility, charge_point_v16, "Heartbeat", call.HeartbeatPayload()
    )


@pytest.mark.asyncio
async def test_regular_charging_session_plugin(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_regular_charging_session_plugin #########")

    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value="10"
    )

    # start charging session
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

    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    test_utility.validation_mode = ValidationMode.STRICT

    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_1),
    )

    test_utility.validation_mode = ValidationMode.EASY

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


@pytest.mark.asyncio
async def test_regular_charging_session_identification(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_regular_charging_session_identification #########")

    await charge_point_v16.clear_cache_req()
    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value="10"
    )

    # swipe id tag to authorize
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    test_utility.validation_mode = ValidationMode.STRICT

    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_1),
    )

    test_utility.validation_mode = ValidationMode.EASY

    # expect StatusNotification with status preparing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.preparing
        ),
    )

    # start charging session
    test_controller.plug_in()

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
async def test_regular_charging_session_identification_conn_timeout(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info(
        "######### test_regular_charging_session_identification_conn_timeout #########"
    )

    await charge_point_v16.clear_cache_req()
    await charge_point_v16.change_configuration_req(key="ConnectionTimeout", value="5")

    # swipe id tag to authorize
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    test_utility.validation_mode = ValidationMode.STRICT

    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_1),
    )

    # expect StatusNotification with status preparing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.preparing
        ),
    )

    # expect StatusNotification with status available because of connection timeout
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_stop_transaction_match_tag(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_stop_transaction_match_tag #########")

    # start charging session
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

    # swipe id tag to authorize
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
    # swipe wrong id tag
    test_controller.swipe(test_config.authorization_info.invalid_id_tag)

    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

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
        call.StopTransactionPayload(
            0, "", 1, Reason.local, id_tag=test_config.authorization_info.valid_id_tag_1
        ),
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
async def test_stop_transaction_parent_id_tag(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):

    logging.info("######### test_stop_transaction_parent_id_tag #########")

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

    # start charging session
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

    # swipe id tag to authorize
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

    # swipe other id tag to authorize (same parent id)
    test_controller.swipe(test_config.authorization_info.valid_id_tag_2)

    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_2),
    )

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


@pytest.mark.asyncio
async def test_005_1_ev_side_disconnect(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_005_1_ev_side_disconnect #########")

    await charge_point_v16.change_configuration_req(
        key="AuthorizeRemoteTxRequests", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="StopTransactionOnEVSideDisconnect", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="UnlockConnectorOnEVSideDisconnect", value="true"
    )

    # start charging session
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

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
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

    test_controller.plug_out()

    test_utility.messages.clear()

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.ev_disconnected),
        validate_standard_stop_transaction,
    )

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
async def test_ev_side_disconnect(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_ev_side_disconnect #########")

    await charge_point_v16.change_configuration_req(
        key="AuthorizeRemoteTxRequests", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="StopTransactionOnEVSideDisconnect", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="UnlockConnectorOnEVSideDisconnect", value="true"
    )

    # start charging session
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

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
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

    test_controller.plug_out()

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.ev_disconnected),
        validate_standard_stop_transaction,
    )

    await charge_point_v16.unlock_connector_req(connector_id=1)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "UnlockConnector",
        call_result.UnlockConnectorPayload(UnlockStatus.unlocked),
    )

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
async def test_ev_side_disconnect_tx_active(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_ev_side_disconnect_tx_active #########")

    await charge_point_v16.change_configuration_req(
        key="MinimumStatusDuration", value="false"
    )
    await charge_point_v16.clear_cache_req()
    await charge_point_v16.change_configuration_req(
        key="UnlockConnectorOnEVSideDisconnect", value="false"
    )
    await charge_point_v16.change_configuration_req(
        key="StopTransactionOnEVSideDisconnect", value="false"
    )

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # start charging session
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

    await charge_point_v16.remote_stop_transaction_req(transaction_id=1)

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
        call.StopTransactionPayload(0, "", 1, Reason.remote),
        validate_standard_stop_transaction,
    )


@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-two-connectors.yaml")
)
@pytest.mark.asyncio
async def test_one_reader_for_multiple_connectors(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_one_reader_for_multiple_connectors #########")

    await charge_point_v16.clear_cache_req()

    test_controller.swipe(
        test_config.authorization_info.valid_id_tag_1, connectors=[1, 2]
    )

    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_1),
    )

    test_controller.plug_in(connector_id=1)

    # expect StatusNotification with status preparing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.preparing
        ),
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

    test_controller.swipe(
        test_config.authorization_info.valid_id_tag_2, connectors=[1, 2]
    )

    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_2),
    )

    test_controller.plug_in(connector_id=2)

    # expect StatusNotification with status preparing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            2, ChargePointErrorCode.no_error, ChargePointStatus.preparing
        ),
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

    test_controller.swipe(
        test_config.authorization_info.valid_id_tag_1, connectors=[1, 2]
    )

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

    test_controller.plug_out(connector_id=1)

    # expect StatusNotification.req with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )

    test_controller.swipe(
        test_config.authorization_info.valid_id_tag_2, connectors=[1, 2]
    )

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            2, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 2, Reason.local),
        validate_standard_stop_transaction,
    )

    test_controller.plug_out(connector_id=2)

    # expect StatusNotification.req with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            2, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_regular_charge_session_cached_id(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_regular_charge_session_cached_id #########")

    await charge_point_v16.change_configuration_req(
        key="AuthorizeRemoteTxRequests", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="AuthorizationCacheEnabled", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="LocalPreAuthorize", value="true"
    )
    await charge_point_v16.clear_cache_req()

    # start charging session
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

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
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

    # charge for some time...
    logging.debug("Charging for a while...")
    await asyncio.sleep(2)

    await charge_point_v16.remote_stop_transaction_req(transaction_id=1)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStopTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.remote),
        validate_standard_stop_transaction,
    )

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )

    logging.debug("executing unplug")
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

    await asyncio.sleep(2)

    # start charging session
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

    # no authorize.req should be sent because id tag should be authorized locally
    test_utility.forbidden_actions.append("Authorize")

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    # charge for some time...
    logging.debug("Charging for a while...")
    await asyncio.sleep(2)

    await charge_point_v16.remote_stop_transaction_req(transaction_id=1)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStopTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.remote),
        validate_standard_stop_transaction,
    )

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )


@pytest.mark.asyncio
async def test_clear_authorization_data_cache(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_clear_authorization_data_cache #########")

    await charge_point_v16.change_configuration_req(
        key="AuthorizationCacheEnabled", value="true"
    )
    # expect ChangeConfiguration.conf with status Accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )

    await charge_point_v16.change_configuration_req(key="ConnectionTimeout", value="2")
    # expect ChangeConfiguration.conf with status Accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )

    # swipe valid id tag
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_1),
    )

    # expect StatusNotification with status preparing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.preparing
        ),
    )

    test_utility.messages.clear()

    # expect StatusNotification with status available after connectionTimeout
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )

    await charge_point_v16.clear_cache_req()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ClearCache",
        call_result.ClearCachePayload(ClearCacheStatus.accepted),
    )

    # swipe valid id tag
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_1),
    )


@pytest.mark.asyncio
async def test_remote_charge_start_stop_cable_first(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_remote_charge_start_stop_cable_first #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

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
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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

    # expect StatusNotification with status preparing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.preparing
        ),
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

    await charge_point_v16.remote_stop_transaction_req(transaction_id=1)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStopTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.remote),
        validate_standard_stop_transaction,
    )

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
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
async def test_remote_charge_start_stop_remote_first(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_remote_charge_start_stop_remote_first #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])
    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value="10"
    )

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # start charging session
    test_controller.plug_in()

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

    # expect StatusNotification with status preparing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.preparing
        ),
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

    await charge_point_v16.remote_stop_transaction_req(transaction_id=1)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStopTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.remote),
        validate_standard_stop_transaction,
    )

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
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
async def test_remote_charge_start_timeout(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_remote_charge_start_timeout #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])
    await charge_point_v16.change_configuration_req(key="ConnectionTimeout", value="10")

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # expect StatusNotification with status preparing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.preparing
        ),
    )
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
async def test_remote_charge_stop(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_remote_charge_stop #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])
    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value="10"
    )

    # start charging session
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

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    await charge_point_v16.remote_stop_transaction_req(transaction_id=1)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStopTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.remote),
        validate_standard_stop_transaction,
    )

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
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
async def test_hard_reset_no_tx(
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_hard_reset_no_tx #########")

    await charge_point_v16.change_availability_req(
        connector_id=1, type=AvailabilityType.inoperative
    )

    # expect StatusNotification with status unavailable
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.unavailable
        ),
    )

    await charge_point_v16.reset_req(type=ResetType.hard)

    test_controller.stop()
    await asyncio.sleep(2)

    test_controller.start()

    charge_point_v16 = await central_system_v16.wait_for_chargepoint()

    # expect StatusNotification with status unavailable
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.unavailable
        ),
    )

    await charge_point_v16.change_availability_req(
        connector_id=1, type=AvailabilityType.operative
    )

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_soft_reset_without_tx(
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_soft_reset_without_tx #########")

    await charge_point_v16.change_availability_req(
        connector_id=1, type=AvailabilityType.inoperative
    )

    # expect StatusNotification with status unavailable
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.unavailable
        ),
    )

    await charge_point_v16.reset_req(type=ResetType.soft)

    test_controller.stop()
    await asyncio.sleep(2)

    test_controller.start()

    charge_point_v16 = await central_system_v16.wait_for_chargepoint()

    # expect StatusNotification with status unavailable
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.unavailable
        ),
    )

    await charge_point_v16.change_availability_req(
        connector_id=1, type=AvailabilityType.operative
    )

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_hard_reset_with_transaction(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_hard_reset_with_transaction #########")

    await charge_point_v16.change_configuration_req(
        key="AuthorizeRemoteTxRequests", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value="10"
    )

    # start charging session
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

    # send RemoteStartTransaction.req
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    await charge_point_v16.reset_req(type=ResetType.hard)

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.hard_reset),
        validate_standard_stop_transaction,
    )

    test_controller.stop()

    await asyncio.sleep(2)

    test_controller.start()

    charge_point_v16 = await central_system_v16.wait_for_chargepoint()

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


@pytest.mark.asyncio
async def test_soft_reset_with_transaction(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_soft_reset_with_transaction #########")

    await charge_point_v16.change_configuration_req(
        key="AuthorizeRemoteTxRequests", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value="10"
    )

    # start charging session
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

    # send RemoteStartTransaction.req
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    await charge_point_v16.reset_req(type=ResetType.soft)

    test_utility.validation_mode = ValidationMode.STRICT

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.soft_reset),
        validate_standard_stop_transaction,
    )


@pytest.mark.asyncio
async def test_unlock_connector_no_charging_no_fixed_cable(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_unlock_connector_no_charging #########")

    test_utility.validation_mode = ValidationMode.STRICT
    await charge_point_v16.unlock_connector_req(connector_id=1)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "UnlockConnector",
        call_result.UnlockConnectorPayload(UnlockStatus.unlocked),
    )

@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-two-connectors.yaml") # this config has no connector_lock configured
)
@pytest.mark.asyncio
async def test_unlock_connector_no_charging_fixed_cable(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_unlock_connector_no_charging_fixed_cable #########")

    test_utility.validation_mode = ValidationMode.STRICT
    await charge_point_v16.unlock_connector_req(connector_id=1)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "UnlockConnector",
        call_result.UnlockConnectorPayload(UnlockStatus.not_supported),
        timeout=10,
    )


@pytest.mark.asyncio
async def test_unlock_connector_with_charging_session_no_fixed_cable(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info(
        "######### test_unlock_connector_with_charging_session_no_fixed_cable #########"
    )

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

    # start charging session
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

    # send RemoteStartTransaction.req
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

    await charge_point_v16.unlock_connector_req(connector_id=1)

    test_utility.validation_mode = ValidationMode.STRICT
    await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "UnlockConnector",
        call_result.UnlockConnectorPayload(UnlockStatus.unlocked),
    )

    test_utility.validation_mode = ValidationMode.EASY

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
        call.StopTransactionPayload(0, "", 1, Reason.unlock_command),
        validate_standard_stop_transaction,
    )


@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-two-connectors.yaml") # this config has no connector_lock configured
)
@pytest.mark.asyncio
async def test_unlock_connector_with_charging_session_fixed_cable(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info(
        "######### test_unlock_connector_with_charging_session_fixed_cable #########"
    )

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

    # start charging session
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

    # send RemoteStartTransaction.req
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    await charge_point_v16.unlock_connector_req(connector_id=1)
    test_utility.validation_mode = ValidationMode.STRICT

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "UnlockConnector",
        call_result.UnlockConnectorPayload(UnlockStatus.not_supported),
    )

    await charge_point_v16.remote_stop_transaction_req(transaction_id=1)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStopTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )
    test_utility.validation_mode = ValidationMode.EASY

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
        call.StopTransactionPayload(0, "", 1, Reason.remote),
        validate_standard_stop_transaction,
    )

    test_controller.plug_out()

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_get_configuration(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_get_configuration #########")

    test_utility.validation_mode = ValidationMode.STRICT
    response = await charge_point_v16.get_configuration_req()

    assert len(response.configuration_key) > 20


@pytest.mark.asyncio
async def test_set_configuration(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_set_configuration #########")

    test_utility.validation_mode = ValidationMode.STRICT

    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value="15"
    )
    response = await charge_point_v16.get_configuration_req(
        key=["MeterValueSampleInterval"]
    )

    assert response.configuration_key[0]["key"] == "MeterValueSampleInterval"
    assert response.configuration_key[0]["value"] == "15"


@pytest.mark.asyncio
async def test_sampled_meter_values(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_sampled_meter_values #########")

    meter_value_sample_interval = "2"

    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value=meter_value_sample_interval
    )
    await charge_point_v16.change_configuration_req(
        key="ClockAlignedDataInterval", value="0"
    )

    meter_values_sampled_data_response = await charge_point_v16.get_configuration_req(
        key=["MeterValuesSampledData"]
    )
    periodic_measurands = meter_values_sampled_data_response.configuration_key[0][
        "value"
    ].split(",")

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        call_result.GetConfigurationPayload(
            [
                {
                    "key": "MeterValuesSampledData",
                    "readonly": False,
                    "value": "Energy.Active.Import.Register",
                }
            ]
        ),
    )
    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        call_result.GetConfigurationPayload(
            [{"key": "AuthorizeRemoteTxRequests", "readonly": False, "value": "true"}]
        ),
    )

    # start charging session
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

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )

    test_utility.validation_mode = ValidationMode.STRICT
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

    test_utility.validation_mode = ValidationMode.EASY
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

    meter_values_messages = []

    logging.debug("Collecting meter values...")
    while len(meter_values_messages) < 4:
        raw_message = await asyncio.wait_for(
            charge_point_v16.wait_for_message(), timeout=30
        )
        charge_point_v16.message_event.clear()
        msg = unpack(raw_message)
        if msg.action == "MeterValues":
            meter_values_messages.append(msg)
            logging.debug(f"Got {len(meter_values_messages)}...")
    logging.debug("Collected meter values...")
    assert validate_meter_values(
        meter_values_messages,
        periodic_measurands,
        [],
        int(meter_value_sample_interval),
        0,
    )

    test_utility.validation_mode = ValidationMode.EASY

    await charge_point_v16.remote_stop_transaction_req(transaction_id=1)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStopTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

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
        call.StopTransactionPayload(0, "", 1, Reason.remote, transaction_data=[]),
        validate_standard_stop_transaction,
    )


@pytest.mark.asyncio
async def test_clock_aligned_meter_values(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_clock_aligned_meter_values #########")

    await charge_point_v16.change_configuration_req(
        key="ClockAlignedDataInterval", value="2"
    )
    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value="2"
    )

    meter_values_sampled_data_response = await charge_point_v16.get_configuration_req(
        key=["MeterValuesSampledData"]
    )
    periodic_measurands = meter_values_sampled_data_response.configuration_key[0][
        "value"
    ].split(",")

    meter_values_aligned_data_response = await charge_point_v16.get_configuration_req(
        key=["MeterValuesAlignedData"]
    )
    clock_aligned_measurands = meter_values_aligned_data_response.configuration_key[0][
        "value"
    ].split(",")

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        call_result.GetConfigurationPayload(
            [
                {
                    "key": "MeterValuesSampledData",
                    "readonly": False,
                    "value": "Energy.Active.Import.Register",
                }
            ]
        ),
    )
    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        call_result.GetConfigurationPayload(
            [{"key": "AuthorizeRemoteTxRequests", "readonly": False, "value": "true"}]
        ),
    )

    # start charging session
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

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )

    test_utility.validation_mode = ValidationMode.STRICT
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

    test_utility.validation_mode = ValidationMode.EASY
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

    meter_values_messages = []

    logging.debug("Collecting meter values...")
    while len(meter_values_messages) < 6:
        raw_message = await asyncio.wait_for(
            charge_point_v16.wait_for_message(), timeout=30
        )
        charge_point_v16.message_event.clear()
        msg = unpack(raw_message)
        if msg.action == "MeterValues":
            meter_values_messages.append(msg)
            logging.debug(f"Got {len(meter_values_messages)}...")
    logging.debug("Collected meter values...")
    assert validate_meter_values(
        meter_values_messages, periodic_measurands, clock_aligned_measurands, 2, 0
    )

    test_utility.validation_mode = ValidationMode.EASY

    await charge_point_v16.remote_stop_transaction_req(transaction_id=1)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStopTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

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
        call.StopTransactionPayload(0, "", 1, Reason.remote, transaction_data=[]),
        validate_standard_stop_transaction,
    )

    # unplug to finish simulation
    test_controller.plug_out()

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_authorize_invalid_blocked_expired(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_authorize_invalid_blocked_expired #########")

    await charge_point_v16.change_configuration_req(
        key="MinimumStatusDuration", value="0"
    )
    await charge_point_v16.change_configuration_req(
        key="LocalPreAuthorize", value="true"
    )

    # start charging session
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

    # swipe id tag to authorize
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    test_utility.validation_mode = ValidationMode.STRICT

    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.AuthorizePayload(test_config.authorization_info.valid_id_tag_1),
    )


@pytest.mark.asyncio
@pytest.mark.skip(reason="EVerest SIL cant simulate connector lock failure")
async def test_start_charging_session_lock_failure(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_start_charging_session_lock_failure #########")

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )

    # expect StatusNotification with status preparing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.preparing
        ),
    )

    # TODO(piet): Simulate connector lock failure...

    # expect StatusNotification with status preparing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.connector_lock_failure, ChargePointStatus.faulted
        ),
    )


@pytest.mark.asyncio
async def test_remote_start_charging_session_rejected(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_remote_start_charging_session_rejected #########")

    await charge_point_v16.change_configuration_req(
        key="MinimumStatusDuration", value="0"
    )
    await charge_point_v16.change_configuration_req(
        key="LocalPreAuthorize", value="false"
    )

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

    # start charging session
    test_controller.plug_in()

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )

    test_utility.validation_mode = ValidationMode.STRICT
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

    test_utility.validation_mode = ValidationMode.EASY
    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.rejected),
        validate_remote_start_stop_transaction,
    )


@pytest.mark.asyncio
async def test_remote_start_tx_connector_id_zero(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
):
    logging.info("######### test_remote_start_connector_id_zero #########")

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=0
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.rejected),
        validate_remote_start_stop_transaction,
    )


@pytest.mark.asyncio
async def test_remote_stop_tx_rejected(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_remote_stop_rejected #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

    await asyncio.sleep(2)

    charge_point_v16.pipeline = []
    test_utility.validation_mode = ValidationMode.STRICT
    # start charging session
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

    test_utility.validation_mode = ValidationMode.EASY

    # send RemoteStartTransaction.req
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

    # send RemoteStopTransaction.req with invalid transaction_id
    await charge_point_v16.remote_stop_transaction_req(transaction_id=3)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStopTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.rejected),
        validate_remote_start_stop_transaction,
    )


@pytest.mark.asyncio
@pytest.mark.skip(reason="EVerest SIL cant simulate connector lock failure")
async def test_unlock_connector_failure(
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_unlock_connector_failure #########")
    # TODO(piet): Put chargepoint in a state where unlock connector fails
    await charge_point_v16.unlock_connector_req(connector_id=1)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "UnlockConnector",
        call_result.UnlockConnectorPayload(UnlockStatus.unlock_failed),
    )


@pytest.mark.asyncio
async def test_unlock_unknown_connector(
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_unlock_unknown_connector #########")

    # send UnlockConnector.req with invalid connector id
    await charge_point_v16.unlock_connector_req(connector_id=2)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "UnlockConnector",
        call_result.UnlockConnectorPayload(UnlockStatus.not_supported),
    )


@pytest.mark.asyncio
@pytest.mark.skip(reason="EVerest SIL cant simulate powerloss with USV")
async def test_power_failure_going_down_charge_point_stop_tx(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info(
        "######### test_power_failure_going_down_charge_point_stop_tx #########"
    )
    # start charging session
    test_controller.plug_in()

    # send RemoteStartTransaction.req
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

    # TOOD(piet): Simulate power loss with USV

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.power_loss, transaction_data=[]),
        validate_standard_stop_transaction,
    )


@pytest.mark.asyncio
@pytest.mark.skip(reason="EVerest SIL cant simulate powerloss with USV")
async def test_power_failure_boot_charge_point_stop_tx(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_power_failure_boot_charge_point_stop_tx #########")
    # start charging session
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

    # send RemoteStartTransaction.req
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

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    test_controller.stop()

    await asyncio.sleep(2)

    test_controller.start()
    charge_point_v16 = await central_system_v16.wait_for_chargepoint()

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
        call.StopTransactionPayload(0, "", 1, Reason.power_loss, transaction_data=[]),
        validate_standard_stop_transaction,
    )


@pytest.mark.asyncio
async def test_power_failure_with_unavailable_status(
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_power_failure_with_unavailable_status #########")

    await charge_point_v16.change_availability_req(
        connector_id=1, type=AvailabilityType.inoperative
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeAvailability",
        call_result.ChangeAvailabilityPayload(AvailabilityStatus.accepted),
    )

    # expect StatusNotification with status unavailable
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.unavailable
        ),
    )

    test_controller.stop()

    await asyncio.sleep(2)

    test_controller.start()

    charge_point_v16 = await central_system_v16.wait_for_chargepoint()
    charge_point_v16.pipe = True

    await asyncio.sleep(2)

    # expect StatusNotification with status unavailable
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.unavailable
        ),
    )

    await charge_point_v16.change_availability_req(
        connector_id=1, type=AvailabilityType.operative
    )

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_idle_charge_point(
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_idle_charge_point #########")

    await charge_point_v16.change_configuration_req(key="HeartbeatInterval", value="10")

    assert await wait_for_and_validate(
        test_utility, charge_point_v16, "Heartbeat", call.HeartbeatPayload()
    )

    logging.debug("disconnect the ws connection...")
    test_controller.disconnect_websocket()

    await asyncio.sleep(1)

    logging.debug("connecting the ws connection")
    test_controller.connect_websocket()

    # wait for reconnect
    await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)

    charge_point_v16 = central_system_v16.chargepoint

    assert await wait_for_and_validate(
        test_utility, charge_point_v16, "Heartbeat", call.HeartbeatPayload()
    )


@pytest.mark.asyncio
async def test_connection_loss_during_transaction(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_connection_loss_during_transaction #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])
    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value="10"
    )

    meter_values_sampled_data_response = await charge_point_v16.get_configuration_req(
        key=["MeterValuesSampledData"]
    )
    periodic_measurands = meter_values_sampled_data_response.configuration_key[0][
        "value"
    ].split(",")

    # start charging session
    test_controller.plug_in()

    # send RemoteStartTransaction.req
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

    logging.debug("disconnect the ws connection...")
    test_controller.disconnect_websocket()

    await asyncio.sleep(60)

    logging.debug("connecting the ws connection")
    test_controller.connect_websocket()

    # wait for reconnect
    await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)

    charge_point_v16 = central_system_v16.chargepoint
    test_utility = TestUtility()

    meter_values_messages = []

    logging.debug("Collecting meter values...")
    while len(meter_values_messages) < 6:
        raw_message = await asyncio.wait_for(
            charge_point_v16.wait_for_message(), timeout=30
        )
        charge_point_v16.message_event.clear()
        msg = unpack(raw_message)
        if msg.action == "MeterValues":
            meter_values_messages.append(msg)
            logging.debug(f"Got {len(meter_values_messages)}...")
    logging.debug("Collected meter values...")
    assert validate_meter_values(meter_values_messages, periodic_measurands, [], 10, 0)

    await charge_point_v16.remote_stop_transaction_req(transaction_id=1)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStopTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.remote),
        validate_standard_stop_transaction,
    )

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )


@pytest.mark.asyncio
async def test_offline_start_transaction(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_offline_start_transaction #########")

    await charge_point_v16.change_configuration_req(
        key="AllowOfflineTxForUnknownId", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="LocalAuthorizeOffline", value="true"
    )

    await asyncio.sleep(1)

    logging.debug("disconnect the ws connection...")
    test_controller.disconnect_websocket()

    await asyncio.sleep(1)

    # start charging session
    test_controller.plug_in()

    # swipe id tag to authorize
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    await asyncio.sleep(2)

    logging.debug("connecting the ws connection")
    test_controller.connect_websocket()

    # wait for reconnect
    await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)

    charge_point_v16 = central_system_v16.chargepoint
    test_utility = TestUtility()

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

    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

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


@pytest.mark.asyncio
async def test_offline_start_transaction_restore(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_offline_start_transaction_restore #########")

    # StartTransaction.conf with invalid id
    @on(Action.StartTransaction)
    def on_start_transaction(**kwargs):
        id_tag_info = IdTagInfo(
            status=AuthorizationStatus.invalid,
            parent_id_tag=test_config.authorization_info.parent_id_tag,
        )
        return call_result.StartTransactionPayload(
            transaction_id=1, id_tag_info=id_tag_info
        )

    await charge_point_v16.change_configuration_req(
        key="AllowOfflineTxForUnknownId", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="StopTransactionOnInvalidId", value="false"
    )
    await charge_point_v16.change_configuration_req(
        key="LocalAuthorizeOffline", value="true"
    )
    await charge_point_v16.clear_cache_req()

    logging.debug("disconnect the ws connection...")
    test_controller.disconnect_websocket()

    await asyncio.sleep(2)

    # start charging session
    test_controller.plug_in()

    # swipe id tag to authorize
    test_controller.swipe(test_config.authorization_info.invalid_id_tag)

    await asyncio.sleep(5)

    central_system_v16.function_overrides.append(
        ("on_start_transaction", on_start_transaction)
    )

    logging.debug("connecting the ws connection")
    test_controller.connect_websocket()

    # wait for reconnect
    await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)

    charge_point_v16 = central_system_v16.chargepoint
    test_utility = TestUtility()

    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.invalid_id_tag, 0, ""
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

    # expect StatusNotification with status suspended_evse
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.suspended_evse
        ),
    )

    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.invalid_id_tag)

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

    # unplug to finish simulation
    test_controller.plug_out()

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_offline_start_transaction_restore_flow(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_offline_start_transaction_restore_flow #########")

    # StartTransaction.conf with invalid id
    @on(Action.StartTransaction)
    def on_start_transaction(**kwargs):
        id_tag_info = IdTagInfo(
            status=AuthorizationStatus.invalid,
            parent_id_tag=test_config.authorization_info.parent_id_tag,
        )
        return call_result.StartTransactionPayload(
            transaction_id=1, id_tag_info=id_tag_info
        )

    await charge_point_v16.change_configuration_req(
        key="AllowOfflineTxForUnknownId", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="LocalAuthorizeOffline", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="StopTransactionOnInvalidId", value="true"
    )

    logging.debug("disconnect the ws connection...")
    test_controller.disconnect_websocket()

    await asyncio.sleep(2)

    # start charging session
    test_controller.plug_in()

    # swipe id tag to authorize
    test_controller.swipe(test_config.authorization_info.invalid_id_tag)

    await asyncio.sleep(10)

    central_system_v16.function_overrides.append(
        ("on_start_transaction", on_start_transaction)
    )

    logging.debug("connecting the ws connection")
    test_controller.connect_websocket()

    # wait for reconnect
    await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)

    charge_point_v16 = central_system_v16.chargepoint
    test_utility = TestUtility()

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransactionPayload(
            1, test_config.authorization_info.invalid_id_tag, 0, ""
        ),
        validate_standard_start_transaction,
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.de_authorized),
        validate_standard_stop_transaction,
    )

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )

    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.invalid_id_tag)

    # unplug to finish simulation
    test_controller.plug_out()

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_offline_stop_transaction(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_offline_stop_transaction #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])
    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value="10"
    )

    # start charging session
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

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    logging.debug("disconnect the ws connection...")
    test_controller.disconnect_websocket()

    await asyncio.sleep(2)

    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    await asyncio.sleep(3)

    logging.debug("connecting the ws connection")
    test_controller.connect_websocket()

    # wait for reconnect
    await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)

    charge_point_v16 = central_system_v16.chargepoint
    test_utility = TestUtility()

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.local),
        validate_standard_stop_transaction,
    )

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )

    # unplug to finish simulation
    test_controller.plug_out()

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_offline_transaction(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_offline_transaction #########")

    await charge_point_v16.change_configuration_req(
        key="LocalAuthorizeOffline", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="AllowOfflineTxForUnknownId", value="true"
    )

    await asyncio.sleep(2)

    logging.debug("disconnect the ws connection...")
    test_controller.disconnect_websocket()

    # start charging session
    test_controller.plug_in()

    # swipe id tag to start transaction
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    # charge for some time...
    logging.debug("Charging for a while...")
    await asyncio.sleep(45)

    # swipe id tag to finish transaction
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    await asyncio.sleep(2)

    logging.debug("connecting the ws connection")
    test_controller.connect_websocket()

    # wait for reconnect
    await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)

    charge_point_v16 = central_system_v16.chargepoint
    test_utility = TestUtility()

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

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.local),
        validate_standard_stop_transaction,
    )

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )

    # unplug to finish simulation
    test_controller.plug_out()

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )


@pytest.mark.asyncio
async def test_configuration_keys(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_configuration_keys #########")

    await charge_point_v16.change_configuration_req(key="NotSupportedKey", value="true")

    # expect ChangeConfiguration.conf with status NotSupported
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.not_supported),
    )


@pytest.mark.asyncio
async def test_configuration_keys_incorrect(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_configuration_keys_incorrect #########")

    await charge_point_v16.change_configuration_req(
        key="MeterValueSampleInterval", value="-1"
    )

    # expect ChangeConfiguration.conf with status NotSupported
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.rejected),
    )


@pytest.mark.asyncio
@pytest.mark.skip(reason="EVerest SIL currently does not support faulted state")
async def test_fault_behavior(
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_fault_behavior #########")

    # Set Diode fault
    test_controller.diode_fail()
    # expect StatusNotification with status faulted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.ground_failure, ChargePointStatus.faulted
        ),
    )


@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-042_1.yaml")
)
@pytest.mark.asyncio
async def test_get_local_list_version(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_get_local_list_version #########")

    await charge_point_v16.change_configuration_req(
        key="LocalAuthListEnabled", value="false"
    )

    # expect ChangeConfiguration.conf with status NotSupported
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.not_supported),
    )

    await charge_point_v16.get_local_list_version_req()

    # expect GetLocalListVersion.conf with status listVersion -1
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetLocalListVersion",
        call_result.GetLocalListVersionPayload(list_version=-1),
    )


@pytest.mark.asyncio
async def test_get_local_list_version_flow(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_get_local_list_version_flow #########")

    await charge_point_v16.change_configuration_req(
        key="LocalAuthListEnabled", value="true"
    )

    # expect ChangeConfiguration.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )

    await charge_point_v16.send_local_list_req(
        list_version=1, update_type=UpdateType.full
    )

    # expect SendLocallist.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SendLocalList",
        call_result.SendLocalListPayload(UpdateStatus.accepted),
    )

    await charge_point_v16.get_local_list_version_req()

    # expect GetLocalListVersion.conf with status listVersion 0
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetLocalListVersion",
        call_result.GetLocalListVersionPayload(list_version=0),
    )


@pytest.mark.asyncio
async def test_send_local_list(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_send_local_list #########")

    await charge_point_v16.change_configuration_req(
        key="LocalAuthListEnabled", value="true"
    )

    # expect ChangeConfiguration.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )

    await charge_point_v16.send_local_list_req(
        list_version=1,
        update_type=UpdateType.differential,
        local_authorization_list=[
            {
                "idTag": "RFID1",
                "idTagInfo": {
                    "status": "Accepted",
                    "expiryDate": "2342-06-19T09:10:00.000Z",
                    "parentIdTag": "PTAG",
                },
            }
        ],
    )

    # expect SendLocallist.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SendLocalList",
        call_result.SendLocalListPayload(UpdateStatus.accepted),
    )

    await charge_point_v16.send_local_list_req(
        list_version=1,
        update_type=UpdateType.full,
        local_authorization_list=[
            {
                "idTag": "RFID1",
                "idTagInfo": {
                    "status": "Accepted",
                    "expiryDate": "2342-06-19T09:10:00.000Z",
                    "parentIdTag": "PTAG",
                },
            }
        ],
    )

    # expect SendLocallist.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SendLocalList",
        call_result.SendLocalListPayload(UpdateStatus.accepted),
    )


@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-042_1.yaml")
)
@pytest.mark.asyncio
async def test_send_local_list_not_supported(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_send_local_list_not_supported #########")

    await charge_point_v16.send_local_list_req(
        list_version=1,
        update_type=UpdateType.full,
        local_authorization_list=[
            {
                "idTag": "RFID1",
                "idTagInfo": {
                    "status": "Accepted",
                    "expiryDate": "2342-06-19T09:10:00.000Z",
                    "parentIdTag": "PTAG",
                },
            }
        ],
    )

    # expect SendLocallist.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SendLocalList",
        call_result.SendLocalListPayload(UpdateStatus.not_supported),
    )


@pytest.mark.asyncio
async def test_send_local_list_ver_mismatch(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_send_local_list_ver_mismatch #########")

    await charge_point_v16.change_configuration_req(
        key="LocalAuthListEnabled", value="true"
    )

    # expect ChangeConfiguration.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )

    await charge_point_v16.send_local_list_req(
        list_version=2,
        update_type=UpdateType.full,
        local_authorization_list=[
            {
                "idTag": "RFID1",
                "idTagInfo": {
                    "status": "Accepted",
                    "expiryDate": "2342-06-19T09:10:00.000Z",
                    "parentIdTag": "PTAG",
                },
            }
        ],
    )

    # expect SendLocallist.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SendLocalList",
        call_result.SendLocalListPayload(UpdateStatus.accepted),
    )

    await charge_point_v16.get_local_list_version_req()

    # expect GetLocalListVersion.conf with status listVersion 2
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetLocalListVersion",
        call_result.GetLocalListVersionPayload(list_version=2),
    )

    await charge_point_v16.send_local_list_req(
        list_version=5,
        update_type=UpdateType.full,
        local_authorization_list=[
            {
                "idTag": "RFID1",
                "idTagInfo": {
                    "status": "Accepted",
                    "expiryDate": "2342-06-19T09:10:00.000Z",
                    "parentIdTag": "PTAG",
                },
            }
        ],
    )

    # expect SendLocallist.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SendLocalList",
        call_result.SendLocalListPayload(UpdateStatus.accepted),
    )

    await charge_point_v16.get_local_list_version_req()

    # expect GetLocalListVersion.conf with status listVersion 5
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetLocalListVersion",
        call_result.GetLocalListVersionPayload(list_version=5),
    )

    await charge_point_v16.send_local_list_req(
        list_version=4,
        update_type=UpdateType.differential,
        local_authorization_list=[
            {
                "idTag": "RFID1",
                "idTagInfo": {
                    "status": "Accepted",
                    "expiryDate": "2342-06-19T09:10:00.000Z",
                    "parentIdTag": "PTAG",
                },
            }
        ],
    )

    # expect SendLocallist.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SendLocalList",
        call_result.SendLocalListPayload(UpdateStatus.version_mismatch),
    )

    await charge_point_v16.get_local_list_version_req()

    # expect GetLocalListVersion.conf with status listVersion -1
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetLocalListVersion",
        call_result.GetLocalListVersionPayload(list_version=5),
    )


@pytest.mark.asyncio
@pytest.mark.skip(
    reason="EVerest SIL cannot put CP in a state where Send Local List fails"
)
async def test_send_local_list_failed(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_send_local_list_failed #########")

    # TODO(piet): Put cp in state where this fails

    await charge_point_v16.send_local_list_req(
        list_version=0, update_type=UpdateType.full
    )

    # expect SendLocallist.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SendLocalList",
        call_result.SendLocalListPayload(UpdateStatus.failed),
    )


@pytest.mark.asyncio
async def test_start_charging_id_in_authorization_list(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_start_charging_id_in_authorization_list #########")

    await charge_point_v16.change_configuration_req(
        key="LocalPreAuthorize", value="true"
    )
    await charge_point_v16.change_configuration_req(
        key="AuthorizationCacheEnabled", value="false"
    )
    await charge_point_v16.change_configuration_req(
        key="LocalAuthListEnabled", value="true"
    )

    response = await charge_point_v16.get_local_list_version_req()
    await charge_point_v16.send_local_list_req(
        list_version=response.list_version,
        update_type=UpdateType.full,
        local_authorization_list=[
            {
                "idTag": "RFID1",
                "idTagInfo": {
                    "status": "Accepted",
                    "expiryDate": "2342-06-19T09:10:00.000Z",
                    "parentIdTag": "PTAG",
                },
            }
        ],
    )

    # start charging session
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

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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

    await charge_point_v16.remote_stop_transaction_req(transaction_id=1)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStopTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.remote),
        validate_standard_stop_transaction,
    )

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )


@pytest.mark.asyncio
@pytest.mark.xdist_group(name="FTP")
async def test_firwmare_update_donwload_install(
    charge_point_v16: ChargePoint16, test_utility: TestUtility, ftp_server, test_config
):
    # not supported when implemented security extensions
    logging.info("######### test_firwmare_update_donwload_install #########")

    retrieve_date = datetime.utcnow()
    location = f"ftp://{getpass.getuser()}:12345@localhost:{ftp_server.port}/firmware_update.pnx"

    await charge_point_v16.update_firmware_req(
        location=location, retrieve_date=retrieve_date.isoformat()
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "FirmwareStatusNotification",
        call.DiagnosticsStatusNotificationPayload(FirmwareStatus.downloading),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "FirmwareStatusNotification",
        call.DiagnosticsStatusNotificationPayload(FirmwareStatus.downloaded),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "FirmwareStatusNotification",
        call.DiagnosticsStatusNotificationPayload(FirmwareStatus.installing),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "FirmwareStatusNotification",
        call.DiagnosticsStatusNotificationPayload(FirmwareStatus.installed),
    )


@pytest.mark.asyncio
async def test_firwmare_update_download_fail(
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    # not supported when implemented security extensions
    logging.info("######### test_firwmare_update_download_fail #########")
    pass


@pytest.mark.asyncio
async def test_firwmare_update_install_fail(
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    # not supported when implemented security extensions
    logging.info("######### test_firwmare_update_install_fail #########")
    pass


@pytest.mark.asyncio
@pytest.mark.xdist_group(name="FTP")
async def test_get_diagnostics(
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
    ftp_server,
):
    logging.info("######### test_get_diagnostics #########")

    await asyncio.sleep(1)

    location = f"ftp://{getpass.getuser()}:12345@localhost:{ftp_server.port}"
    start_time = datetime.utcnow()
    stop_time = start_time + timedelta(days=3)

    await charge_point_v16.get_diagnostics_req(
        location=location,
        start_time=start_time.isoformat(),
        stop_time=stop_time.isoformat(),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DiagnosticsStatusNotification",
        call.DiagnosticsStatusNotificationPayload(DiagnosticsStatus.uploading),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DiagnosticsStatusNotification",
        call.DiagnosticsStatusNotificationPayload(DiagnosticsStatus.uploaded),
    )


@pytest.mark.asyncio
async def test_get_diagnostics_upload_fail(
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_get_diagnostics_upload_fail #########")

    location = "ftp://pionix:12345@notavalidftpserver:21"
    start_time = datetime.utcnow()
    stop_time = start_time + timedelta(days=3)
    retries = 0

    await charge_point_v16.get_diagnostics_req(
        location=location,
        start_time=start_time.isoformat(),
        stop_time=stop_time.isoformat(),
        retries=retries,
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DiagnosticsStatusNotification",
        call.DiagnosticsStatusNotificationPayload(DiagnosticsStatus.uploading),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DiagnosticsStatusNotification",
        call.DiagnosticsStatusNotificationPayload(DiagnosticsStatus.upload_failed),
    )


@pytest.mark.asyncio
async def test_reservation_local_start_tx(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_reservation_local_start_tx #########")

    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date="2030-06-19T09:10:00.000Z",
        id_tag=test_config.authorization_info.valid_id_tag_1,
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

    # swipe invalid id tag
    test_controller.swipe(test_config.authorization_info.invalid_id_tag)

    # swipe valid id tag to authorize
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    # expect StatusNotification with status preparing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.preparing
        ),
    )

    # start charging session
    test_controller.plug_in()

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
async def test_reservation_remote_start_tx(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_reservation_remote_start_tx #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])
    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date="2030-06-19T09:10:00.000Z",
        id_tag=test_config.authorization_info.valid_id_tag_1,
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

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # start charging session
    test_controller.plug_in()

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
async def test_reservation_connector_expire(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_reservation_connector_expire #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

    t = datetime.utcnow() + timedelta(seconds=10)

    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date=t.isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
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

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # start charging session
    test_controller.plug_in()

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
async def test_reservation_connector_faulted(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_reservation_connector_faulted #########")

    # Set diode fault
    test_controller.diode_fail()
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1
    )

    await asyncio.sleep(10)

    t = datetime.utcnow() + timedelta(seconds=10)

    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date=t.isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
        reservation_id=0,
    )

    # expect ReserveNow.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ReserveNow",
        call_result.ReserveNowPayload(ReservationStatus.faulted),
    )


@pytest.mark.asyncio
async def test_reservation_connector_occupied(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_reservation_connector_occupied #########")

    # start charging session
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

    t = datetime.utcnow() + timedelta(seconds=10)

    await asyncio.sleep(2)

    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date=t.isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
        reservation_id=0,
    )

    # expect ReserveNow.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ReserveNow",
        call_result.ReserveNowPayload(ReservationStatus.occupied),
    )


@pytest.mark.asyncio
async def test_reservation_connector_unavailable(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
):
    logging.info("######### test_reservation_connector_unavailable #########")

    await charge_point_v16.change_availability_req(
        connector_id=1, type=AvailabilityType.inoperative
    )

    t = datetime.utcnow() + timedelta(seconds=10)

    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date=t.isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
        reservation_id=0,
    )

    # expect ReserveNow.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ReserveNow",
        call_result.ReserveNowPayload(ReservationStatus.unavailable),
    )


@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-042_1.yaml")
)
@pytest.mark.asyncio
async def test_reservation_connector_rejected(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
):
    logging.info("######### test_reservation_connector_rejected #########")

    t = datetime.utcnow() + timedelta(seconds=10)

    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date=t.isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
        reservation_id=0,
    )

    # expect ReserveNow.conf with status rejected
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ReserveNow",
        call_result.ReserveNowPayload(ReservationStatus.rejected),
    )


@pytest.mark.ocpp_config_adaptions(
    GenericOCPP16ConfigAdjustment([("Reservation", "ReserveConnectorZeroSupported", False)])
)
@pytest.mark.asyncio
async def test_reservation_connector_zero_not_supported(
    charge_point_v16: ChargePoint16, test_utility: TestUtility, test_config: OcppTestConfiguration
):
    logging.info("######### test_reservation_connector_zero_not_supported #########")

    await charge_point_v16.reserve_now_req(
        connector_id=0,
        expiry_date=(datetime.utcnow() + timedelta(minutes=10)).isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
        reservation_id=0,
    )

    # expect ReserveNow.conf with status rejected
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ReserveNow",
        call_result.ReserveNowPayload(ReservationStatus.rejected),
    )


@pytest.mark.ocpp_config_adaptions(
    GenericOCPP16ConfigAdjustment([("Reservation", "ReserveConnectorZeroSupported", True)])
)
@pytest.mark.asyncio
async def test_reservation_connector_zero_supported(
        charge_point_v16: ChargePoint16,
        test_utility: TestUtility,
        test_config: OcppTestConfiguration,
        test_controller: TestController,
):
    logging.info("######### test_reservation_connector_zero_supported #########")

    await charge_point_v16.reserve_now_req(
        connector_id=0,
        expiry_date=(datetime.utcnow() + timedelta(minutes=10)).isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
        reservation_id=0,
    )

    # expect ReserveNow.conf with status rejected
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ReserveNow",
        call_result.ReserveNowPayload(ReservationStatus.accepted),
    )

    # expect StatusNotification with status reserved
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.reserved
        ),
    )

    # swipe valid id tag to authorize
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    # expect StatusNotification with status preparing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.preparing
        ),
    )

    # start charging session
    test_controller.plug_in()

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
async def test_reservation_faulted_state(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):
    logging.info("######### test_reservation_faulted_state #########")

    test_controller.raise_error("MREC6UnderVoltage", 1)

    await asyncio.sleep(1)

    # expect StatusNotification with status faulted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.other_error, ChargePointStatus.faulted
        ),
    )
    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date=datetime.utcnow().isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
        reservation_id=0,
    )

    # expect ReserveNow.conf with status rejected
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ReserveNow",
        call_result.ReserveNowPayload(ReservationStatus.faulted),
    )

    test_controller.clear_error("MREC6UnderVoltage", 1)


@pytest.mark.asyncio
async def test_reservation_occupied_state(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):
    logging.info("######### test_reservation_occupied_state #########")

    test_controller.plug_in()
    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )
    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date=(datetime.utcnow() + timedelta(minutes=10)).isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
        reservation_id=0,
    )

    # expect ReserveNow.conf with status rejected
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ReserveNow",
        call_result.ReserveNowPayload(ReservationStatus.occupied),
    )


@pytest.mark.asyncio
async def test_reservation_cancel(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_reservation_cancel #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

    t = datetime.utcnow() + timedelta(minutes=10)

    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date=t.isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
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

    await charge_point_v16.cancel_reservation_req(reservation_id=0)

    # expect CancelReservation.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "CancelReservation",
        call_result.CancelReservationPayload(CancelReservationStatus.accepted),
    )

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )

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
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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
async def test_reservation_cancel_rejected(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
):
    logging.info("######### test_reservation_cancel_rejected #########")

    t = datetime.utcnow() + timedelta(minutes=10)

    await charge_point_v16.reserve_now_req(
        connector_id=1,
        expiry_date=t.isoformat(),
        id_tag=test_config.authorization_info.valid_id_tag_1,
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

    await charge_point_v16.cancel_reservation_req(reservation_id=2)

    # expect CancelReservation.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "CancelReservation",
        call_result.CancelReservationPayload(CancelReservationStatus.rejected),
    )


@pytest.mark.asyncio
async def test_reservation_with_parentid(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):
    logging.info("######### test_reservation_with_parentid #########")

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

    # start charging session
    test_controller.plug_in()

    # send RemoteStartTransaction.req
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

    await charge_point_v16.remote_stop_transaction_req(transaction_id=1)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStopTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
        validate_remote_start_stop_transaction,
    )

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StopTransaction",
        call.StopTransactionPayload(0, "", 1, Reason.remote),
        validate_standard_stop_transaction,
    )

    # expect StatusNotification with status finishing
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.finishing
        ),
    )


@pytest.mark.asyncio
async def test_trigger_message(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):
    logging.info("######### test_trigger_message #########")

    await charge_point_v16.trigger_message_req(
        requested_message=MessageTrigger.meter_values, connector_id=1
    )
    # expect TriggerMessage.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "TriggerMessage",
        call_result.TriggerMessagePayload(TriggerMessageStatus.accepted),
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "MeterValues",
        call.MeterValuesPayload(0, []),
        dont_validate_meter_values,
    )

    await charge_point_v16.trigger_message_req(
        requested_message=MessageTrigger.heartbeat
    )
    # expect TriggerMessage.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "TriggerMessage",
        call_result.TriggerMessagePayload(TriggerMessageStatus.accepted),
    )
    assert await wait_for_and_validate(
        test_utility, charge_point_v16, "Heartbeat", call.HeartbeatPayload()
    )

    await charge_point_v16.trigger_message_req(
        requested_message=MessageTrigger.status_notification
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
            1, ChargePointErrorCode.no_error, ChargePointStatus.available
        ),
    )

    await charge_point_v16.trigger_message_req(
        requested_message=MessageTrigger.diagnostics_status_notification
    )
    # expect TriggerMessage.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "TriggerMessage",
        call_result.TriggerMessagePayload(TriggerMessageStatus.accepted),
    )
    # expect DiagnosticsStatusNotificationPayload.req with status idle
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DiagnosticsStatusNotification",
        call.DiagnosticsStatusNotificationPayload(DiagnosticsStatus.idle),
    )

    await charge_point_v16.trigger_message_req(
        requested_message=MessageTrigger.boot_notification
    )
    # expect TriggerMessage.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "TriggerMessage",
        call_result.TriggerMessagePayload(TriggerMessageStatus.accepted),
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "BootNotification",
        call.BootNotificationPayload(
            test_config.charge_point_info.charge_point_model,
            charge_box_serial_number=test_config.charge_point_info.charge_point_id,
            charge_point_vendor=test_config.charge_point_info.charge_point_vendor,
            firmware_version=test_config.charge_point_info.firmware_version,
        ),
        validate_boot_notification,
    )


@pytest.mark.asyncio
async def test_trigger_message_rejected(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_trigger_message_rejected #########")

    await charge_point_v16.trigger_message_req(
        requested_message=MessageTrigger.meter_values, connector_id=2
    )
    # expect TriggerMessage.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "TriggerMessage",
        call_result.TriggerMessagePayload(TriggerMessageStatus.rejected),
    )


@pytest.mark.asyncio
async def test_central_charging_tx_default_profile(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_central_charging_tx_default_profile #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

    valid_from = datetime.utcnow()
    valid_to = valid_from + timedelta(days=3)

    set_charging_profile_req = call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=valid_from.isoformat(),
            valid_to=valid_to.isoformat(),
            charging_schedule=ChargingSchedule(
                duration=300,
                start_schedule=valid_from.isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=6),
                    ChargingSchedulePeriod(start_period=60, limit=10),
                    ChargingSchedulePeriod(start_period=120, limit=8),
                ],
            ),
        ),
    )

    await charge_point_v16.set_charging_profile_req(set_charging_profile_req)

    # expect SetChargingProfile.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SetChargingProfile",
        call_result.SetChargingProfilePayload(ChargingProfileStatus.accepted),
    )

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
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    cs = await charge_point_v16.get_composite_schedule_req(connector_id=1, duration=300)

    passed_seconds = int((datetime.utcnow() - valid_from).total_seconds())

    exp_get_composite_schedule_response = call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        connector_id=1,
        schedule_start=valid_from.isoformat(),
        charging_schedule=ChargingSchedule(
            duration=300,
            start_schedule=valid_from.isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(start_period=0, limit=6),
                ChargingSchedulePeriod(start_period=60 - passed_seconds, limit=10),
                ChargingSchedulePeriod(start_period=120 - passed_seconds, limit=8),
                ChargingSchedulePeriod(start_period=300 - passed_seconds, limit=48),
            ],
        ),
    )

    # expect correct GetCompositeSchedule.conf
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp_get_composite_schedule_response,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_central_charging_tx_profile(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_central_charging_tx_profile #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

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
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    valid_from = datetime.utcnow()
    valid_to = valid_from + timedelta(days=3)

    set_charging_profile_req = call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=3,
            transaction_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=valid_from.isoformat(),
            valid_to=valid_to.isoformat(),
            charging_schedule=ChargingSchedule(
                duration=260,
                start_schedule=valid_from.isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=6),
                    ChargingSchedulePeriod(start_period=60, limit=10),
                    ChargingSchedulePeriod(start_period=120, limit=8),
                ],
            ),
        ),
    )

    await charge_point_v16.set_charging_profile_req(set_charging_profile_req)

    # expect SetChargingProfile.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SetChargingProfile",
        call_result.SetChargingProfilePayload(ChargingProfileStatus.accepted),
    )

    await charge_point_v16.get_composite_schedule_req(connector_id=1, duration=300)

    passed_seconds = int((datetime.utcnow() - valid_from).total_seconds())

    exp_get_composite_schedule_response = call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=valid_from.isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=300,
            start_schedule=valid_from.isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(start_period=0, limit=6),
                ChargingSchedulePeriod(start_period=60 - passed_seconds, limit=10),
                ChargingSchedulePeriod(start_period=120 - passed_seconds, limit=8),
                ChargingSchedulePeriod(start_period=260 - passed_seconds, limit=48),
            ],
        ),
    )

    # expect correct GetCompositeSchedule.conf
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp_get_composite_schedule_response,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_central_charging_no_transaction(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_central_charging_no_transaction #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

    valid_from = datetime.utcnow()
    valid_to = valid_from + timedelta(days=3)

    set_charging_profile_req = call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=3,
            transaction_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=valid_from.isoformat(),
            valid_to=valid_to.isoformat(),
            charging_schedule=ChargingSchedule(
                duration=260,
                start_schedule=valid_from.isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=6),
                    ChargingSchedulePeriod(start_period=60, limit=10),
                    ChargingSchedulePeriod(start_period=120, limit=8),
                ],
            ),
        ),
    )

    await charge_point_v16.set_charging_profile_req(set_charging_profile_req)

    # expect SetChargingProfile.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SetChargingProfile",
        call_result.SetChargingProfilePayload(ChargingProfileStatus.rejected),
    )


@pytest.mark.asyncio
async def test_central_charging_wrong_tx_id(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_central_charging_wrong_tx_id #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

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
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    valid_from = datetime.utcnow()
    valid_to = valid_from + timedelta(days=3)

    set_charging_profile_req = call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=3,
            transaction_id=3,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=valid_from.isoformat(),
            valid_to=valid_to.isoformat(),
            charging_schedule=ChargingSchedule(
                duration=260,
                start_schedule=valid_from.isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=6),
                    ChargingSchedulePeriod(start_period=60, limit=10),
                    ChargingSchedulePeriod(start_period=120, limit=8),
                ],
            ),
        ),
    )

    await charge_point_v16.set_charging_profile_req(set_charging_profile_req)

    # expect SetChargingProfile.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SetChargingProfile",
        call_result.SetChargingProfilePayload(ChargingProfileStatus.rejected),
    )


@pytest.mark.asyncio
async def test_central_charging_tx_default_profile_ongoing_transaction(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info(
        "######### test_central_charging_tx_default_profile_ongoing_transaction #########"
    )

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

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
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    valid_from = datetime.utcnow()
    valid_to = valid_from + timedelta(days=3)

    set_charging_profile_req = call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=3,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=valid_from.isoformat(),
            valid_to=valid_to.isoformat(),
            charging_schedule=ChargingSchedule(
                duration=300,
                start_schedule=valid_from.isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=6),
                    ChargingSchedulePeriod(start_period=60, limit=10),
                    ChargingSchedulePeriod(start_period=120, limit=8),
                ],
            ),
        ),
    )

    await charge_point_v16.set_charging_profile_req(set_charging_profile_req)

    # expect SetChargingProfile.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SetChargingProfile",
        call_result.SetChargingProfilePayload(ChargingProfileStatus.accepted),
    )

    await charge_point_v16.get_composite_schedule_req(connector_id=1, duration=300)

    exp_get_composite_schedule_response = call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        connector_id=1,
        schedule_start=valid_from.isoformat(),
        charging_schedule=ChargingSchedule(
            duration=300,
            start_schedule=valid_from.isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(start_period=0, limit=6),
                ChargingSchedulePeriod(start_period=60, limit=10),
                ChargingSchedulePeriod(start_period=120, limit=8),
            ],
        ),
    )

    # expect correct GetCompositeSchedule.conf
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp_get_composite_schedule_response,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_get_composite_schedule(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_get_composite_schedule #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

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
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    valid_from = datetime.utcnow()
    valid_to = valid_from + timedelta(days=3)

    set_charging_profile_req_1 = call.SetChargingProfilePayload(
        connector_id=0,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.charge_point_max_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=valid_from.isoformat(),
            valid_to=valid_to.isoformat(),
            charging_schedule=ChargingSchedule(
                duration=86400,
                start_schedule=valid_from.isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=10)
                ],
            ),
        ),
    )

    set_charging_profile_req_2 = call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=2,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=valid_from.isoformat(),
            valid_to=valid_to.isoformat(),
            charging_schedule=ChargingSchedule(
                duration=300,
                start_schedule=valid_from.isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=6),
                    ChargingSchedulePeriod(start_period=60, limit=10),
                    ChargingSchedulePeriod(start_period=120, limit=8),
                    ChargingSchedulePeriod(start_period=180, limit=25),
                    ChargingSchedulePeriod(start_period=260, limit=8),
                ],
            ),
        ),
    )

    set_charging_profile_req_3 = call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=3,
            transaction_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=valid_from.isoformat(),
            valid_to=valid_to.isoformat(),
            charging_schedule=ChargingSchedule(
                duration=260,
                start_schedule=valid_from.isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=8),
                    ChargingSchedulePeriod(start_period=50, limit=11),
                    ChargingSchedulePeriod(start_period=140, limit=16),
                    ChargingSchedulePeriod(start_period=200, limit=6),
                    ChargingSchedulePeriod(start_period=240, limit=12),
                ],
            ),
        ),
    )

    await charge_point_v16.set_charging_profile_req(set_charging_profile_req_1)
    # expect SetChargingProfile.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SetChargingProfile",
        call_result.SetChargingProfilePayload(ChargingProfileStatus.accepted),
    )

    # await asyncio.sleep(2)

    await charge_point_v16.set_charging_profile_req(set_charging_profile_req_2)
    # expect SetChargingProfile.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SetChargingProfile",
        call_result.SetChargingProfilePayload(ChargingProfileStatus.accepted),
    )

    # await asyncio.sleep(2)

    await charge_point_v16.set_charging_profile_req(set_charging_profile_req_3)
    # expect SetChargingProfile.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SetChargingProfile",
        call_result.SetChargingProfilePayload(ChargingProfileStatus.accepted),
    )

    cs = await charge_point_v16.get_composite_schedule_req(connector_id=1, duration=400)

    exp_get_composite_schedule_response = call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        connector_id=1,
        schedule_start=valid_from.isoformat(),
        charging_schedule=ChargingSchedule(
            duration=400,
            start_schedule=valid_from.isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(start_period=0, limit=8),
                ChargingSchedulePeriod(start_period=50, limit=10),
                ChargingSchedulePeriod(start_period=200, limit=6),
                ChargingSchedulePeriod(start_period=240, limit=10),
                ChargingSchedulePeriod(start_period=260, limit=8),
                ChargingSchedulePeriod(start_period=300, limit=10),
            ],
        ),
    )

    # expect correct GetCompositeSchedule.conf
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp_get_composite_schedule_response,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_clear_charging_profile(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_clear_charging_profile #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

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
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StatusNotification",
        call.StatusNotificationPayload(
            1, ChargePointErrorCode.no_error, ChargePointStatus.charging
        ),
    )

    valid_from = datetime.utcnow()
    valid_to = valid_from + timedelta(days=3)

    set_charging_profile_req = call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=valid_from.isoformat(),
            valid_to=valid_to.isoformat(),
            charging_schedule=ChargingSchedule(
                duration=300,
                start_schedule=valid_from.isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=6),
                    ChargingSchedulePeriod(start_period=60, limit=10),
                    ChargingSchedulePeriod(start_period=120, limit=8),
                ],
            ),
        ),
    )

    await charge_point_v16.set_charging_profile_req(set_charging_profile_req)
    # expect SetChargingProfile.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SetChargingProfile",
        call_result.SetChargingProfilePayload(ChargingProfileStatus.accepted),
    )

    await charge_point_v16.clear_charging_profile_req(
        id=1, connector_id=1, charging_profile_purpose="TxDefaultProfile", stack_level=0
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ClearChargingProfile",
        call_result.ClearChargingProfilePayload(ClearChargingProfileStatus.accepted),
    )

    exp_get_composite_schedule_response = call_result.GetCompositeSchedulePayload(
        status="Accepted",
        connector_id=1,
        charging_schedule=ChargingSchedule(
            charging_schedule_period=[],
            duration=400,
            charging_rate_unit=ChargingRateUnitType.amps,
        ),
    )

    await charge_point_v16.get_composite_schedule_req(connector_id=1, duration=400)

    # expect correct GetCompositeSchedule.conf
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp_get_composite_schedule_response,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_stacking_charging_profiles(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_stacking_charging_profiles #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])
    await charge_point_v16.get_configuration_req(key=["MaxChargingProfilesInstalled"])
    await charge_point_v16.get_configuration_req(key=["ChargeProfileMaxStackLevel"])

    valid_from = datetime.utcnow()
    valid_to = valid_from + timedelta(days=3)

    set_charging_profile_req_1 = call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=valid_from.isoformat(),
            valid_to=valid_to.isoformat(),
            charging_schedule=ChargingSchedule(
                duration=400,
                start_schedule=valid_from.isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=6),
                    ChargingSchedulePeriod(start_period=50, limit=5),
                    ChargingSchedulePeriod(start_period=100, limit=8),
                    ChargingSchedulePeriod(start_period=200, limit=10),
                ],
            ),
        ),
    )

    set_charging_profile_req_2 = call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=2,
            stack_level=1,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=valid_from.isoformat(),
            valid_to=valid_to.isoformat(),
            charging_schedule=ChargingSchedule(
                duration=150,
                start_schedule=valid_from.isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=7),
                    ChargingSchedulePeriod(start_period=100, limit=9),
                ],
            ),
        ),
    )

    await charge_point_v16.set_charging_profile_req(set_charging_profile_req_1)
    # expect SetChargingProfile.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SetChargingProfile",
        call_result.SetChargingProfilePayload(ChargingProfileStatus.accepted),
    )

    await asyncio.sleep(2)

    await charge_point_v16.set_charging_profile_req(set_charging_profile_req_2)
    # expect SetChargingProfile.conf with status accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SetChargingProfile",
        call_result.SetChargingProfilePayload(ChargingProfileStatus.accepted),
    )

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
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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

    cs = await charge_point_v16.get_composite_schedule_req(connector_id=1, duration=350)

    passed_seconds = int((datetime.utcnow() - valid_from).total_seconds())

    exp_get_composite_schedule_response = call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        connector_id=1,
        schedule_start=valid_from.isoformat(),
        charging_schedule=ChargingSchedule(
            duration=350,
            start_schedule=valid_from.isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(start_period=0, limit=7),
                ChargingSchedulePeriod(start_period=100 - passed_seconds, limit=9),
                ChargingSchedulePeriod(start_period=150 - passed_seconds, limit=8),
                ChargingSchedulePeriod(start_period=200 - passed_seconds, limit=10),
            ],
        ),
    )

    # expect correct GetCompositeSchedule.conf
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetCompositeSchedule",
        exp_get_composite_schedule_response,
        validate_composite_schedule,
    )


@pytest.mark.asyncio
async def test_remote_start_tx_with_profile(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_remote_start_tx_with_profile #########")

    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])

    # start charging session
    test_controller.plug_in()

    valid_from = datetime.utcnow()
    valid_to = valid_from + timedelta(days=3)

    cs_charging_profiles = ChargingProfile(
        charging_profile_id=1,
        stack_level=2,
        charging_profile_purpose=ChargingProfilePurposeType.tx_profile,
        charging_profile_kind=ChargingProfileKindType.absolute,
        valid_from=valid_from.isoformat(),
        valid_to=valid_to.isoformat(),
        charging_schedule=ChargingSchedule(
            duration=30,
            start_schedule=valid_from.isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[ChargingSchedulePeriod(start_period=0, limit=6)],
        ),
    )

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1,
        connector_id=1,
        charging_profile=cs_charging_profiles,
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.accepted),
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
async def test_remote_start_tx_with_profile_rejected(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
):
    logging.info("######### test_remote_start_tx_with_profile_rejected #########")

    valid_from = datetime.utcnow()
    valid_to = valid_from + timedelta(days=3)

    cs_charging_profiles = ChargingProfile(
        charging_profile_id=3,
        transaction_id=1,
        stack_level=0,
        charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
        charging_profile_kind=ChargingProfileKindType.absolute,
        valid_from=valid_from.isoformat(),
        valid_to=valid_to.isoformat(),
        charging_schedule=ChargingSchedule(
            duration=260,
            start_schedule=valid_from.isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(start_period=0, limit=6),
                ChargingSchedulePeriod(start_period=60, limit=10),
                ChargingSchedulePeriod(start_period=120, limit=8),
            ],
        ),
    )

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1,
        connector_id=1,
        charging_profile=cs_charging_profiles,
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "RemoteStartTransaction",
        call_result.RemoteStartTransactionPayload(RemoteStartStopStatus.rejected),
        validate_remote_start_stop_transaction,
    )


@pytest.mark.asyncio
async def test_data_transfer_to_chargepoint(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    logging.info("######### test_data_transfer_to_chargepoint #########")

    await charge_point_v16.data_transfer_req(
        vendor_id="VENID", message_id="MSGID", data="Data1"
    )

    success = await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DataTransfer",
        call_result.DataTransferPayload(DataTransferStatus.rejected),
        timeout=5,
    )

    if not success:
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "DataTransfer",
            call_result.DataTransferPayload(DataTransferStatus.unknown_vendor_id),
            timeout=5,
        )
    else:
        assert success


@pytest.mark.asyncio
async def test_data_transfer_to_csms(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    # TODO(piet): Need to trigger DataTransfer.req from cp->cs for that via mqtt
    # (somehow similiar to websocket control)
    logging.info("######### test_data_transfer_to_csms #########")
    pass


@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-security-profile-1.yaml")
)
@pytest.mark.asyncio
async def test_chargepoint_update_http_auth_key(
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
):
    logging.info("######### test_chargepoint_update_http_auth_key #########")

    await charge_point_v16.change_configuration_req(
        key="AuthorizationKey", value="4f43415f4f4354545f61646d696e5f74657374"
    )
    # expect ChangeConfiguration.conf with status Accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )

    # wait for reconnect
    await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)

    charge_point_v16 = central_system_v16.chargepoint
    test_utility = TestUtility()

    response = await charge_point_v16.get_configuration_req()
    assert len(response.configuration_key) > 20


@pytest.mark.asyncio
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP16ConfigAdjustment([("Security", "SecurityProfile", 0)])
)
async def test_chargepoint_update_security_profile(
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
):
    logging.info("######### test_chargepoint_update_security_profile #########")

    await charge_point_v16.change_configuration_req(key="SecurityProfile", value="1")
    # expect ChangeConfiguration.conf with status Accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )

    # wait for reconnect
    await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)

    charge_point_v16 = central_system_v16.chargepoint
    test_utility = TestUtility()

    response = await charge_point_v16.get_configuration_req(key=["SecurityProfile"])

    assert response.configuration_key[0]["key"] == "SecurityProfile"
    assert response.configuration_key[0]["value"] == "1"


@pytest.mark.asyncio
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP16ConfigAdjustment(
        [
            ("Internal", "RetryBackoffRandomRange", 1),
            ("Internal", "RetryBackoffWaitMinimum", 2),
        ]
    )
)
async def test_chargepoint_update_security_profile_fallback(
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
):
    logging.info(
        "######### test_chargepoint_update_security_profile_fallback #########"
    )

    await charge_point_v16.change_configuration_req(key="SecurityProfile", value="2")
    # expect ChangeConfiguration.conf with status Accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )

    # wait for reconnect
    await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)

    charge_point_v16 = central_system_v16.chargepoint
    test_utility = TestUtility()

    response = await charge_point_v16.get_configuration_req(key=["SecurityProfile"])

    assert response.configuration_key[0]["key"] == "SecurityProfile"
    assert response.configuration_key[0]["value"] == "0"


@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-security-profile-2.yaml")
)
@pytest.mark.source_certs_dir(Path(__file__).parent / "../everest-aux/certs")
@pytest.mark.asyncio
@pytest.mark.csms_tls
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP16ConfigAdjustment([("Internal", "VerifyCsmsCommonName", False)])
)
async def test_chargepoint_update_certificate(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
):
    logging.info("######### test_chargepoint_update_certificate #########")

    await charge_point_v16.change_configuration_req(key="CpoName", value="VENID")
    # expect ChangeConfiguration.conf with status Accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        call_result.ChangeConfigurationPayload(ConfigurationStatus.accepted),
    )

    await charge_point_v16.extended_trigger_message_req(
        requested_message=MessageTrigger.sign_charge_point_certificate
    )
    # expect ExtendedTriggerMessage.conf with status Accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ExtendedTriggerMessage",
        call_result.ExtendedTriggerMessagePayload(TriggerMessageStatus.accepted),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignCertificate",
        call.SignCertificatePayload(csr=""),
        dont_validate_sign_certificate,
    )

    await charge_point_v16.certificate_signed_req(
        csms_root_ca=test_config.certificate_info.csms_root_ca,
        csms_root_ca_key=test_config.certificate_info.csms_root_ca_key,
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "CertificateSigned",
        call_result.CertificateSignedPayload(CertificateSignedStatus.accepted),
    )


@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-security-profile-2.yaml")
)
@pytest.mark.asyncio
@pytest.mark.source_certs_dir(Path(__file__).parent / "../everest-aux/certs")
@pytest.mark.csms_tls
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP16ConfigAdjustment([("Internal", "VerifyCsmsCommonName", False)])
)
async def test_chargepoint_install_certificate(
    test_config: OcppTestConfiguration,
    central_system_v16: CentralSystem,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
):
    logging.info("######### test_chargepoint_install_certificate #########")

    certificate = open(
        Path(__file__).parent.parent / test_config.certificate_info.csms_cert
    ).read()

    await charge_point_v16.install_certificate_req(
        certificate_type=CertificateUse.central_system_root_certificate,
        certificate=certificate,
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "InstallCertificate",
        call_result.InstallCertificatePayload(CertificateStatus.accepted),
    )

    r = await charge_point_v16.get_installed_certificate_ids_req(
        certificate_type=CertificateUse.central_system_root_certificate
    )

    exp_cert_hash_data = {
        "hash_algorithm": "SHA256",
        "issuer_key_hash": "7569e411948ceda3d815f04caab0d8548035c624116e1be688344ef095aea53b",
        "issuer_name_hash": "d3768132ad54b8162680d1ba88966d189e5719d226524f6cd893c5f1c506f068",
        "serial_number": "59d2755839892c132eaa9a49ad6c71638ce8012b",
    }
    assert exp_cert_hash_data in r.certificate_hash_data

    await charge_point_v16.delete_certificate_req(
        certificate_hash_data=exp_cert_hash_data
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DeleteCertificate",
        call_result.DeleteCertificatePayload(DeleteCertificateStatus.accepted),
    )


@pytest.mark.asyncio
async def test_chargepoint_delete_certificate(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
):
    logging.info("######### test_chargepoint_delete_certificate #########")

    r = await charge_point_v16.get_installed_certificate_ids_req(
        certificate_type=CertificateUse.central_system_root_certificate
    )

    if r.status == GetInstalledCertificateStatus.not_found:
        certificate = open(test_config.certificate_info.csms_root_ca).read()
        await charge_point_v16.install_certificate_req(
            certificate_type=CertificateUse.central_system_root_certificate,
            certificate=certificate,
        )
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v16,
            "InstallCertificate",
            call_result.InstallCertificatePayload(CertificateStatus.accepted),
        )

    certificate_hash_data = {
        "hashAlgorithm": "SHA256",
        "issuerKeyHash": "89ea6977e786fcbaeb4f04e4ccdbfaa6a6088e8ba8f7404033ac1b3a62bc36a1",
        "issuerNameHash": "e60bd843bf2279339127ca19ab6967081dd6f95e745dc8b8632fa56031debe5b",
        "serialNumber": "1",
    }

    test_utility.validation_mode = ValidationMode.STRICT

    await charge_point_v16.delete_certificate_req(
        certificate_hash_data=certificate_hash_data
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DeleteCertificate",
        call_result.DeleteCertificatePayload(DeleteCertificateStatus.accepted),
    )

    await charge_point_v16.delete_certificate_req(
        certificate_hash_data=certificate_hash_data
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "DeleteCertificate",
        call_result.DeleteCertificatePayload(DeleteCertificateStatus.not_found),
    )


@pytest.mark.asyncio
async def test_chargepoint_invalid_certificate_security_event(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
):
    logging.info(
        "######### test_chargepoint_invalid_certificate_security_event #########"
    )

    await charge_point_v16.extended_trigger_message_req(
        requested_message=MessageTrigger.sign_charge_point_certificate
    )
    # expect ExtendedTriggerMessage.conf with status Accepted
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ExtendedTriggerMessage",
        call_result.ExtendedTriggerMessagePayload(TriggerMessageStatus.accepted),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignCertificate",
        call.SignCertificatePayload(csr=""),
        dont_validate_sign_certificate,
    )

    # this is an invalid certificate chain
    await charge_point_v16.certificate_signed_req(
        csms_root_ca=test_config.certificate_info.csms_root_ca,
        csms_root_ca_key=test_config.certificate_info.csms_root_ca_key,
        certificate_chain="-----BEGIN CERTIFICATE-----\nMIHgMIGaAgEBMA0GCSqG-----END CERTIFICATE-----",
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "CertificateSigned",
        call_result.CertificateSignedPayload(CertificateSignedStatus.rejected),
    )

    # InvalidChargePointCertificate is defined as critical in OCPP1.6
    # assert await wait_for_and_validate(test_utility, charge_point_v16, "SecurityEventNotification", {"type": "InvalidChargePointCertificate"})


@pytest.mark.asyncio
async def test_chargepoint_invalid_central_system_security_event(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    central_system_v16: CentralSystem,
    test_utility: TestUtility,
):
    logging.info(
        "######### test_chargepoint_invalid_central_system_security_event #########"
    )

    await charge_point_v16.install_certificate_req(
        certificate_type=CertificateUse.central_system_root_certificate,
        certificate="-----BEGIN CERTIFICATE-----\nMIHgMIGaAgEBMA0GCSqG-----END CERTIFICATE-----",
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "InstallCertificate",
        call_result.InstallCertificatePayload(CertificateStatus.rejected),
    )
    # InvalidCentralSystemCertificate is defined as critical in OCPP1.6
    # assert await wait_for_and_validate(test_utility, charge_point_v16, "SecurityEventNotification", {"type": "InvalidCentralSystemCertificate"})


@pytest.mark.asyncio
@pytest.mark.xdist_group(name="FTP")
async def test_get_security_log(
    charge_point_v16: ChargePoint16, test_utility: TestUtility, ftp_server
):
    logging.info("######### test_get_security_log #########")

    oldest_timestamp = datetime.utcnow()
    latest_timestamp = oldest_timestamp + timedelta(days=3)

    log = {
        "remoteLocation": f"ftp://{getpass.getuser()}:12345@localhost:{ftp_server.port}",
        "oldestTimestamp": oldest_timestamp.isoformat(),
        "latestTimestamp": latest_timestamp.isoformat(),
    }

    await charge_point_v16.get_log_req(log=log, log_type=Log.security_log, request_id=1)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetLog",
        call_result.GetLogPayload(LogStatus.accepted),
        validate_get_log,
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "LogStatusNotification",
        call.LogStatusNotificationPayload(UploadLogStatus.uploading, 1),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "LogStatusNotification",
        call.LogStatusNotificationPayload(UploadLogStatus.uploaded, 1),
    )


@pytest.mark.asyncio
@pytest.mark.xdist_group(name="FTP")
async def test_signed_update_firmware(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    ftp_server,
):
    logging.info("######### test_signed_update_firmware #########")

    certificate = open(test_config.certificate_info.mf_root_ca).read()

    await charge_point_v16.install_certificate_req(
        certificate_type=CertificateUse.manufacturer_root_certificate,
        certificate=certificate,
    )

    os.system(
        f"curl -T {Path(__file__).parent.parent / test_config.firmware_info.update_file} ftp://{getpass.getuser()}:12345@localhost:{ftp_server.port}"
    )

    location = f"ftp://{getpass.getuser()}:12345@localhost:{ftp_server.port}/firmware_update.pnx"
    retrieve_date_time = datetime.utcnow()
    mf_root_ca = open(test_config.certificate_info.mf_root_ca).read()
    fw_signature = open(test_config.firmware_info.update_file_signature).read()

    firmware = {
        "location": location,
        "retrieveDateTime": retrieve_date_time.isoformat(),
        "signingCertificate": mf_root_ca,
        "signature": fw_signature,
    }

    await charge_point_v16.signed_update_firmware_req(request_id=1, firmware=firmware)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignedUpdateFirmware",
        call_result.SignedUpdateFirmwarePayload(UpdateFirmwareStatus.accepted),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignedFirmwareStatusNotification",
        call.SignedFirmwareStatusNotificationPayload(FirmwareStatus.downloading, 1),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignedFirmwareStatusNotification",
        call.SignedFirmwareStatusNotificationPayload(FirmwareStatus.downloaded, 1),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignedFirmwareStatusNotification",
        call.SignedFirmwareStatusNotificationPayload(
            FirmwareStatus.signature_verified, 1
        ),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignedFirmwareStatusNotification",
        call.SignedFirmwareStatusNotificationPayload(FirmwareStatus.installing, 1),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "SignedFirmwareStatusNotification",
        call.SignedFirmwareStatusNotificationPayload(FirmwareStatus.installed, 1),
    )
