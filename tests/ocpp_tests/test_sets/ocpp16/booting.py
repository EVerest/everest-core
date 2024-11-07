# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from everest.testing.core_utils.controller.test_controller_interface import TestController
# fmt: off
from ocpp.routing import create_route_map, on
from ocpp.v16.enums import *
from ocpp.v16 import call
from datetime import datetime
import asyncio
import logging
import pytest
from validations import (validate_standard_start_transaction,
                               validate_standard_stop_transaction,
                               validate_boot_notification
                               )
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility, OcppTestConfiguration
from everest.testing.ocpp_utils.fixtures import *
from everest.testing.ocpp_utils.charge_point_v16 import ChargePoint16
from everest.testing.ocpp_utils.central_system import CentralSystem
from everest.testing.core_utils._configuration.libocpp_configuration_helper import GenericOCPP16ConfigAdjustment
from everest_test_utils import *
# fmt: on


@pytest.mark.asyncio
async def test_stop_pending_transactions(test_config: OcppTestConfiguration, charge_point_v16: ChargePoint16, test_utility: TestUtility, test_controller: TestController, central_system_v16: CentralSystem):
    logging.info("######### test_stop_pending_transactions #########")

    # start charging session
    test_controller.plug_in()

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1)

    # expect StartTransaction.req
    assert await wait_for_and_validate(test_utility, charge_point_v16, "StartTransaction",
                                       call.StartTransactionPayload(
                                           1, test_config.authorization_info.valid_id_tag_1, 0, ""),
                                       validate_standard_start_transaction)

    # expect StatusNotification with status charging
    assert await wait_for_and_validate(test_utility, charge_point_v16, "StatusNotification",
                                       call.StatusNotificationPayload(1, ChargePointErrorCode.no_error,
                                                                      ChargePointStatus.charging))

    # charge for some time...
    logging.debug("Charging for a while...")
    await asyncio.sleep(2)

    test_controller.stop()

    await asyncio.sleep(2)

    test_controller.start()

    charge_point_v16 = await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)

    await asyncio.sleep(2)

    # expect StopTransaction.req
    assert await wait_for_and_validate(test_utility, charge_point_v16, "StopTransaction",
                                       call.StopTransactionPayload(
                                           0, "", 1, Reason.power_loss),
                                       validate_standard_stop_transaction)


@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-security-profile-1.yaml'))
@pytest.mark.asyncio
async def test_change_authorization_key_in_pending(test_config: OcppTestConfiguration, central_system_v16: CentralSystem, test_controller: TestController, test_utility: TestUtility):
    logging.info(
        "######### test_change_authorization_key_in_pending #########")

    @ on(Action.BootNotification)
    def on_boot_notification_pending(**kwargs):
        return call_result.BootNotificationPayload(
            current_time=datetime.utcnow().isoformat(),
            interval=10,
            status=RegistrationStatus.pending,
        )

    @ on(Action.BootNotification)
    def on_boot_notification_accepted(**kwargs):
        return call_result.BootNotificationPayload(
            current_time=datetime.utcnow().isoformat(),
            interval=5,
            status=RegistrationStatus.accepted,
        )

    central_system_v16.function_overrides.append(
        ('on_boot_notification', on_boot_notification_pending))

    test_controller.start()
    charge_point_v16 = await central_system_v16.wait_for_chargepoint()
    charge_point_v16.pipe = True

    response = await charge_point_v16.get_configuration_req()
    assert len(response.configuration_key) > 20

    await charge_point_v16.change_configuration_req(key="MeterValueSampleInterval", value="10")
    await charge_point_v16.change_configuration_req(key="AuthorizationKey", value="DEADBEEFDEADBEEF")

    # wait for reconnect
    await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)
    charge_point_v16 = central_system_v16.chargepoint

    setattr(charge_point_v16, 'on_boot_notification',
            on_boot_notification_accepted)
    central_system_v16.chargepoint.route_map = create_route_map(
        central_system_v16.chargepoint)

    assert await wait_for_and_validate(test_utility, charge_point_v16, "BootNotification",
                                       call.BootNotificationPayload(test_config.charge_point_info.charge_point_model,
                                                                    charge_box_serial_number=test_config.charge_point_info.charge_point_id,
                                                                    charge_point_vendor=test_config.charge_point_info.charge_point_vendor,
                                                                    firmware_version=test_config.charge_point_info.firmware_version), validate_boot_notification)

    # expect StatusNotification.req with status available
    assert await wait_for_and_validate(test_utility, charge_point_v16, "StatusNotification",
                                       call.StatusNotificationPayload(1, ChargePointErrorCode.no_error,
                                                                      ChargePointStatus.available))

    assert await wait_for_and_validate(test_utility, charge_point_v16, "Heartbeat", call.HeartbeatPayload())

@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-security-profile-1.yaml'))
@pytest.mark.asyncio
async def test_remote_start_stop_in_pending(test_config: OcppTestConfiguration, central_system_v16: CentralSystem, test_controller: TestController, test_utility: TestUtility):
    logging.info(
        "######### test_change_authorization_key_in_pending #########")

    @ on(Action.BootNotification)
    def on_boot_notification_pending(**kwargs):
        return call_result.BootNotificationPayload(
            current_time=datetime.utcnow().isoformat(),
            interval=10,
            status=RegistrationStatus.pending,
        )

    central_system_v16.function_overrides.append(
        ('on_boot_notification', on_boot_notification_pending))

    test_controller.start()
    charge_point_v16 = await central_system_v16.wait_for_chargepoint()
    charge_point_v16.pipe = True

    await charge_point_v16.remote_start_transaction_req(id_tag="DEADBEEF")
    assert await wait_for_and_validate(test_utility, charge_point_v16, "RemoteStartTransaction", {"status": "Rejected"})

    await charge_point_v16.remote_stop_transaction_req(transaction_id=20)
    assert await wait_for_and_validate(test_utility, charge_point_v16, "RemoteStopTransaction", {"status": "Rejected"})

@pytest.mark.asyncio
async def test_boot_notification_rejected(test_config: OcppTestConfiguration, central_system_v16: CentralSystem, test_controller: TestController, test_utility: TestUtility):
    logging.info(
        "######### test_boot_notification_rejected #########")

    @ on(Action.BootNotification)
    def on_boot_notification_rejected(**kwargs):
        return call_result.BootNotificationPayload(
            current_time=datetime.utcnow().isoformat(),
            interval=10,
            status=RegistrationStatus.rejected,
        )

    @ on(Action.BootNotification)
    def on_boot_notification_accepted(**kwargs):
        return call_result.BootNotificationPayload(
            current_time=datetime.utcnow().isoformat(),
            interval=5,
            status=RegistrationStatus.accepted,
        )

    central_system_v16.function_overrides.append(
        ('on_boot_notification', on_boot_notification_rejected))

    test_controller.start()
    charge_point_v16: ChargePoint16 = await central_system_v16.wait_for_chargepoint()
    charge_point_v16.pipe = True

    setattr(charge_point_v16, 'on_boot_notification',
            on_boot_notification_accepted)
    central_system_v16.chargepoint.route_map = create_route_map(
        central_system_v16.chargepoint)

    assert await wait_for_and_validate(test_utility, charge_point_v16, "BootNotification",
                                       call.BootNotificationPayload(test_config.charge_point_info.charge_point_model,
                                                                    charge_box_serial_number=test_config.charge_point_info.charge_point_id,
                                                                    charge_point_vendor=test_config.charge_point_info.charge_point_vendor,
                                                                    firmware_version=test_config.charge_point_info.firmware_version), validate_boot_notification)

    # expect StatusNotification.req with status available
    assert await wait_for_and_validate(test_utility, charge_point_v16, "StatusNotification",
                                       call.StatusNotificationPayload(1, ChargePointErrorCode.no_error,
                                                                      ChargePointStatus.available))

    assert await wait_for_and_validate(test_utility, charge_point_v16, "Heartbeat", call.HeartbeatPayload())


@pytest.mark.asyncio
async def test_boot_notification_callerror(test_config: OcppTestConfiguration, central_system_v16: CentralSystem, test_controller: TestController, test_utility: TestUtility):
    logging.info(
        "######### test_boot_notification_callerror #########")

    @ on(Action.BootNotification)
    def on_boot_notification_accepted(**kwargs):
        return call_result.BootNotificationPayload(
            current_time=datetime.utcnow().isoformat(),
            interval=5,
            status=RegistrationStatus.accepted,
        )

    # Provoke a CALLERROR as a response to a BootNotification.req
    central_system_v16.function_overrides.append(
        ('on_boot_notification', None))

    test_controller.start()
    charge_point_v16: ChargePoint16 = await central_system_v16.wait_for_chargepoint()
    charge_point_v16.pipe = True

    setattr(charge_point_v16, 'on_boot_notification',
            on_boot_notification_accepted)
    central_system_v16.chargepoint.route_map = create_route_map(
        central_system_v16.chargepoint)

    assert await wait_for_and_validate(test_utility, charge_point_v16, "BootNotification",
                                       call.BootNotificationPayload(test_config.charge_point_info.charge_point_model,
                                                                    charge_box_serial_number=test_config.charge_point_info.charge_point_id,
                                                                    charge_point_vendor=test_config.charge_point_info.charge_point_vendor,
                                                                    firmware_version=test_config.charge_point_info.firmware_version), validate_boot_notification, timeout=100)

    # expect StatusNotification.req with status available
    assert await wait_for_and_validate(test_utility, charge_point_v16, "StatusNotification",
                                       call.StatusNotificationPayload(1, ChargePointErrorCode.no_error,
                                                                      ChargePointStatus.available))

    assert await wait_for_and_validate(test_utility, charge_point_v16, "Heartbeat", call.HeartbeatPayload())


@pytest.mark.asyncio
async def test_boot_notification_no_response(test_config: OcppTestConfiguration, central_system_v16: CentralSystem, test_controller: TestController, test_utility: TestUtility):
    logging.info(
        "######### test_boot_notification_no_response #########")

    async def route_message(msg):
        return

    # do not respond at all
    central_system_v16.function_overrides.append(
        ('route_message', route_message))

    test_controller.start()
    charge_point_v16: ChargePoint16 = await central_system_v16.wait_for_chargepoint()
    charge_point_v16.pipe = True

    # this is the second BootNotification.req
    assert await wait_for_and_validate(test_utility, charge_point_v16, "BootNotification",
                                       call.BootNotificationPayload(test_config.charge_point_info.charge_point_model,
                                                                    charge_box_serial_number=test_config.charge_point_info.charge_point_id,
                                                                    charge_point_vendor=test_config.charge_point_info.charge_point_vendor,
                                                                    firmware_version=test_config.charge_point_info.firmware_version), validate_boot_notification, timeout=100)

@pytest.mark.asyncio
@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-security-profile-2.yaml'))
@pytest.mark.source_certs_dir(Path(__file__).parent / "../everest-aux/certs")
@ pytest.mark.asyncio
@pytest.mark.csms_tls
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP16ConfigAdjustment([("Internal", "VerifyCsmsCommonName", False)]))
async def test_initiate_message_in_pending(test_config: OcppTestConfiguration, central_system_v16: CentralSystem, test_controller: TestController, test_utility: TestUtility):
    logging.info(
        "######### test_initiate_message_in_pending #########")
    
    @ on(Action.BootNotification)
    def on_boot_notification_pending(**kwargs):
        return call_result.BootNotificationPayload(
            current_time=datetime.utcnow().isoformat(),
            interval=10,
            status=RegistrationStatus.pending,
        )

    @ on(Action.BootNotification)
    def on_boot_notification_accepted(**kwargs):
        return call_result.BootNotificationPayload(
            current_time=datetime.utcnow().isoformat(),
            interval=5,
            status=RegistrationStatus.accepted,
        )

    central_system_v16.function_overrides.append(
        ('on_boot_notification', on_boot_notification_pending))
    
    test_utility.forbidden_actions.append("SecurityEventNotification")

    test_controller.start()
    charge_point_v16: ChargePoint16 = await central_system_v16.wait_for_chargepoint()
    charge_point_v16.pipe = True

    await charge_point_v16.change_configuration_req(key="CpoName", value="VENID")


    await charge_point_v16.extended_trigger_message_req(requested_message=MessageTrigger.status_notification)
    assert await wait_for_and_validate(test_utility, charge_point_v16, "StatusNotification",
                                       call.StatusNotificationPayload(1, ChargePointErrorCode.no_error,
                                                                      ChargePointStatus.available))

    test_utility.messages.clear()
    await charge_point_v16.extended_trigger_message_req(requested_message=MessageTrigger.boot_notification)
    assert await wait_for_and_validate(test_utility, charge_point_v16, "BootNotification",
                                       call.BootNotificationPayload(test_config.charge_point_info.charge_point_model,
                                                                    charge_box_serial_number=test_config.charge_point_info.charge_point_id,
                                                                    charge_point_vendor=test_config.charge_point_info.charge_point_vendor,
                                                                    firmware_version=test_config.charge_point_info.firmware_version), validate_boot_notification)

    test_utility.messages.clear()
    await charge_point_v16.extended_trigger_message_req(requested_message=MessageTrigger.heartbeat)
    assert await wait_for_and_validate(test_utility, charge_point_v16, "Heartbeat", call.HeartbeatPayload())

    test_utility.messages.clear()
    await charge_point_v16.trigger_message_req(requested_message=MessageTrigger.diagnostics_status_notification)
    assert await wait_for_and_validate(test_utility, charge_point_v16, "DiagnosticsStatusNotification",
                                       call.DiagnosticsStatusNotificationPayload(DiagnosticsStatus.idle))

    test_utility.messages.clear()
    await charge_point_v16.trigger_message_req(requested_message=MessageTrigger.firmware_status_notification)
    assert await wait_for_and_validate(test_utility, charge_point_v16, "FirmwareStatusNotification", call.FirmwareStatusNotificationPayload(FirmwareStatus.idle))

    test_utility.messages.clear()
    await charge_point_v16.trigger_message_req(requested_message=MessageTrigger.status_notification)
    assert await wait_for_and_validate(test_utility, charge_point_v16, "StatusNotification",
                                       call.StatusNotificationPayload(1, ChargePointErrorCode.no_error,
                                                                      ChargePointStatus.available))

    await charge_point_v16.extended_trigger_message_req(requested_message=MessageTrigger.sign_charge_point_certificate)
    # expect ExtendedTriggerMessage.conf with status Accepted
    assert await wait_for_and_validate(test_utility, charge_point_v16, "ExtendedTriggerMessage",
                                       call_result.ExtendedTriggerMessagePayload(TriggerMessageStatus.accepted))

    assert await wait_for_and_validate(test_utility, charge_point_v16, "SignCertificate", {})

    setattr(charge_point_v16, 'on_boot_notification',
            on_boot_notification_accepted)
    central_system_v16.chargepoint.route_map = create_route_map(
        central_system_v16.chargepoint)

    assert await wait_for_and_validate(test_utility, charge_point_v16, "BootNotification",
                                       call.BootNotificationPayload(test_config.charge_point_info.charge_point_model,
                                                                    charge_box_serial_number=test_config.charge_point_info.charge_point_id,
                                                                    charge_point_vendor=test_config.charge_point_info.charge_point_vendor,
                                                                    firmware_version=test_config.charge_point_info.firmware_version), validate_boot_notification)

    test_utility.forbidden_actions.clear()
    assert await wait_for_and_validate(
        test_utility, charge_point_v16, "SecurityEventNotification", {"type": "StartupOfTheDevice"})

    # expect StatusNotification.req with status available
    assert await wait_for_and_validate(test_utility, charge_point_v16, "StatusNotification",
                                       call.StatusNotificationPayload(1, ChargePointErrorCode.no_error,
                                                                      ChargePointStatus.available))

    assert await wait_for_and_validate(test_utility, charge_point_v16, "Heartbeat", call.HeartbeatPayload())


@pytest.mark.asyncio
async def test_boot_notification_rejected_and_call_by_csms(test_config: OcppTestConfiguration, central_system_v16: CentralSystem, test_controller: TestController, test_utility: TestUtility):
    @ on(Action.BootNotification)
    def on_boot_notification_rejected(**kwargs):
        return call_result.BootNotificationPayload(
            current_time=datetime.utcnow().isoformat(),
            interval=10,
            status=RegistrationStatus.rejected,
        )

    central_system_v16.function_overrides.append(
        ('on_boot_notification', on_boot_notification_rejected))

    test_controller.start()
    charge_point_v16: ChargePoint16 = await central_system_v16.wait_for_chargepoint()
    charge_point_v16.pipe = True

    # Response to this message is not allowed
    test_utility.forbidden_actions.append("RemoteStartTransaction")

    t = threading.Thread(target=asyncio.run, args=(charge_point_v16.remote_start_transaction_req(
        id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1),))
    t.start()

    assert await wait_for_and_validate(test_utility, charge_point_v16, "BootNotification", {})

