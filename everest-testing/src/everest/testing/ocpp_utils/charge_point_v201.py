# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

import asyncio
import logging
import time
from datetime import datetime
from websockets.exceptions import ConnectionClosedOK, ConnectionClosedError

from ocpp.messages import unpack
from ocpp.charge_point import camel_to_snake_case
from ocpp.v201 import ChargePoint as cp
from ocpp.v201 import call, call_result
from ocpp.v201.enums import (
    Action,
    RegistrationStatusType,
    AuthorizationStatusType,
    AttributeType,
    NotifyEVChargingNeedsStatusType,
    GenericStatusType
)
from ocpp.v201.datatypes import IdTokenInfoType, SetVariableDataType, GetVariableDataType, ComponentType, VariableType
from ocpp.routing import on

from everest.testing.ocpp_utils.charge_point_utils import MessageHistory

logging.basicConfig(level=logging.DEBUG)


class ChargePoint201(cp):

    """Wrapper for the OCPP2.0.1 chargepoint websocket client. Implementes the communication
     of messages sent from CSMS to chargepoint.
    """

    def __init__(self, cp_id, connection, response_timeout=30):
        super().__init__(cp_id, connection, response_timeout)
        self.pipeline = []
        self.pipe = False
        self.csr = None
        self.message_event = asyncio.Event()
        self.message_history = MessageHistory()

    async def start(self):
        """Start to receive, store and route incoming messages.
        """
        try:
            while True:
                message = await self._connection.recv()
                logging.debug(f"Chargepoint: \n{message}")
                self.message_history.add_received(message)

                if (self.pipe):
                    self.pipeline.append(message)
                    self.message_event.set()

                await self.route_message(message)
                self.message_event.clear()
        except ConnectionClosedOK:
            logging.debug("ConnectionClosedOK: Websocket is going down")
        except ConnectionClosedError:
            logging.debug("ConnectionClosedError: Websocket is going down")

    async def stop(self):
        self._connection.close()

    async def _send(self, message):
        logging.debug(f"CSMS: \n{message}")
        self.message_history.add_send(message)
        await self._connection.send(message)

    async def wait_for_message(self):
        """If no message is in the pipeline, this method waits for the next message.
        If there is one or more messages in the pipeline, it pops the latest message.
        """
        if not self.pipeline:
            await self.message_event.wait()
        return self.pipeline.pop(0)

    async def wait_for_specific_message(self, exp_action, exp_payload={}, timeout=60):
        msg_found = False
        t_timeout = time.time() + timeout
        while (not msg_found and time.time() < t_timeout):
            try:
                raw_message = await asyncio.wait_for(self.wait_for_message(), timeout=timeout)
                msg = unpack(raw_message)
                if (msg.action == exp_action):
                    payload_matches = True
                    # check if exp_payload matches
                    for k, v in exp_payload.items():
                        if k not in msg.payload or msg.payload[k] != v:
                            payload_matches = False
                    if payload_matches:
                        return camel_to_snake_case(msg.payload)
            except asyncio.TimeoutError:
                logging.debug("Timeout while waiting for new message")
        return None

    @on(Action.BootNotification)
    def on_boot_notification(self, **kwargs):
        logging.debug("Received a BootNotification")
        return call_result.BootNotificationPayload(current_time=datetime.now().isoformat(),
                                                   interval=300, status=RegistrationStatusType.accepted)

    @on(Action.StatusNotification)
    def on_status_notification(self, **kwargs):
        return call_result.StatusNotificationPayload()

    @on(Action.Heartbeat)
    def on_heartbeat(self, **kwargs):
        return call_result.HeartbeatPayload(current_time=datetime.utcnow().isoformat())

    @on(Action.Authorize)
    def on_authorize(self, **kwargs):
        return call_result.AuthorizePayload(
            id_token_info=IdTokenInfoType(
                status=AuthorizationStatusType.accepted
            )
        )

    @on(Action.NotifyReport)
    def on_notify_report(self, **kwargs):
        return call_result.NotifyReportPayload()

    @on(Action.LogStatusNotification)
    def on_log_status_notification(self, **kwargs):
        return call_result.LogStatusNotificationPayload()

    @on(Action.FirmwareStatusNotification)
    def on_firmware_status_notification(self, **kwargs):
        return call_result.FirmwareStatusNotificationPayload()

    @on(Action.TransactionEvent)
    def on_transaction_event(self, **kwargs):
        return call_result.TransactionEventPayload()

    @on(Action.NotifyChargingLimit)
    def on_notify_charging_limit(self, **kwargs):
        return call_result.NotifyChargingLimitPayload()

    @on(Action.NotifyCustomerInformation)
    def on_notify_customer_information(self, **kwargs):
        return call_result.NotifyCustomerInformationPayload()

    @on(Action.NotifyEVChargingNeeds)
    def on_notify_ev_charging_needs(self, **kwargs):
        return call_result.NotifyEVChargingNeedsPayload(status=NotifyEVChargingNeedsStatusType.accepted)

    @on(Action.NotifyEVChargingSchedule)
    def on_notify_ev_charging_schedule(self, **kwargs):
        return call_result.NotifyEVChargingSchedulePayload(status=GenericStatusType.accepted)

    @on(Action.NotifyEvent)
    def on_notify_event(self, **kwargs):
        return call_result.NotifyEventPayload()

    @on(Action.NotifyMonitoringReport)
    def on_notify_monitoring_report(self, **kwargs):
        return call_result.NotifyMonitoringReportPayload()

    @on(Action.PublishFirmwareStatusNotification)
    def on_publish_firmware_status_notification(self, **kwargs):
        return call_result.PublishFirmwareStatusNotificationPayload()

    @on(Action.ReportChargingProfiles)
    def on_report_charging_profiles(self, **kwargs):
        return call_result.ReportChargingProfilesPayload()

    @on(Action.ReservationStatusUpdate)
    def on_reservation_status_update(self, **kwargs):
        return call_result.ReservationStatusUpdatePayload()

    @on(Action.SecurityEventNotification)
    def on_security_event_notification(self, **kwargs):
        return call_result.SecurityEventNotificationPayload()

    @on(Action.SignCertificate)
    def on_sign_certificate(self, **kwargs):
        return call_result.SignCertificatePayload(status=GenericStatusType.accepted)

    async def set_variables_req(self, **kwargs):
        payload = call.SetVariablesPayload(**kwargs)
        return await self.call(payload)

    async def set_config_variables_req(self, component_name, variable_name, value):
        el = SetVariableDataType(
            attribute_value=value,
            attribute_type=AttributeType.actual,
            component=ComponentType(
                name=component_name
            ),
            variable=VariableType(
                name=variable_name
            )
        )
        payload = call.SetVariablesPayload([el])
        return await self.call(payload)

    async def get_variables_req(self, **kwargs):
        payload = call.GetVariablesPayload(**kwargs)
        return await self.call(payload)

    async def get_config_variables_req(self, component_name, variable_name):
        el = GetVariableDataType(
            component=ComponentType(
                name=component_name
            ),
            variable=VariableType(
                name=variable_name
            ),
            attribute_type=AttributeType.actual
        )
        payload = call.GetVariablesPayload([el])
        return await self.call(payload)

    async def get_base_report_req(self, **kwargs):
        payload = call.GetBaseReportPayload(**kwargs)
        return await self.call(payload)

    async def get_report_req(self, **kwargs):
        payload = call.GetReportPayload(**kwargs)
        return await self.call(payload)

    async def reset_req(self, **kwargs):
        payload = call.ResetPayload(**kwargs)
        return await self.call(payload)

    async def request_start_transaction_req(self, **kwargs):
        payload = call.RequestStartTransactionPayload(**kwargs)
        return await self.call(payload)

    async def request_stop_transaction_req(self, **kwargs):
        payload = call.RequestStopTransactionPayload(**kwargs)
        return await self.call(payload)

    async def change_availablility_req(self, **kwargs):
        payload = call.ChangeAvailabilityPayload(**kwargs)
        return await self.call(payload)

    async def clear_cache_req(self, **kwargs):
        payload = call.ClearCachePayload(**kwargs)
        return await self.call(payload)

    async def cancel_reservation_req(self, **kwargs):
        payload = call.CancelReservationPayload(**kwargs)
        return await self.call(payload)

    async def certificate_signed_req(self, **kwargs):
        payload = call.CertificateSignedPayload(**kwargs)
        return await self.call(payload)

    async def clear_charging_profile_req(self, **kwargs):
        payload = call.ClearChargingProfilePayload(**kwargs)
        return await self.call(payload)

    async def clear_display_message_req(self, **kwargs):
        payload = call.ClearDisplayMessagePayload(**kwargs)
        return await self.call(payload)

    async def clear_charging_limit_req(self, **kwargs):
        payload = call.ClearedChargingLimitPayload(**kwargs)
        return await self.call(payload)

    async def clear_variable_monitoring_req(self, **kwargs):
        payload = call.ClearVariableMonitoringPayloaddPayload(**kwargs)
        return await self.call(payload)

    async def cost_update_req(self, **kwargs):
        payload = call.CostUpdatedPayload(**kwargs)
        return await self.call(payload)

    async def customer_information_req(self, **kwargs):
        payload = call.CustomerInformationPayload(**kwargs)
        return await self.call(payload)

    async def data_transfer_req(self, **kwargs):
        payload = call.DataTransferPayload(**kwargs)
        return await self.call(payload)

    async def delete_certificate_req(self, **kwargs):
        payload = call.DeleteCertificatePayload(**kwargs)
        return await self.call(payload)

    async def get_charging_profiles_req(self, **kwargs):
        payload = call.GetChargingProfilesPayload(**kwargs)
        return await self.call(payload)

    async def get_composite_schedule_req(self, **kwargs):
        payload = call.GetCompositeSchedulePayload(**kwargs)
        return await self.call(payload)

    async def get_display_nessages_req(self, **kwargs):
        payload = call.GetDisplayMessagesPayload(**kwargs)
        return await self.call(payload)

    async def get_installed_certificate_ids_req(self, **kwargs):
        payload = call.GetInstalledCertificateIdsPayload(**kwargs)
        return await self.call(payload)

    async def get_local_list_version(self, **kwargs):
        payload = call.GetLocalListVersionPayload(**kwargs)
        return await self.call(payload)

    async def get_log_req(self, **kwargs):
        payload = call.GetLogPayload(**kwargs)
        return await self.call(payload)

    async def get_transaction_status_req(self, **kwargs):
        payload = call.GetTransactionStatusPayload(**kwargs)
        return await self.call(payload)

    async def install_certificate_req(self, **kwargs):
        payload = call.InstallCertificatePayload(**kwargs)
        return await self.call(payload)

    async def publish_firmware_req(self, **kwargs):
        payload = call.PublishFirmwarePayload(**kwargs)
        return await self.call(payload)

    async def reserve_now_req(self, **kwargs):
        payload = call.ReserveNowPayload(**kwargs)
        return await self.call(payload)

    async def send_local_list_req(self, **kwargs):
        payload = call.SendLocalListPayload(**kwargs)
        return await self.call(payload)

    async def set_charging_profile_req(self, **kwargs):
        payload = call.SetChargingProfilePayload(**kwargs)
        return await self.call(payload)

    async def set_display_message_req(self, **kwargs):
        payload = call.SetDisplayMessagePayload(**kwargs)
        return await self.call(payload)

    async def set_monitoring_base_req(self, **kwargs):
        payload = call.SetMonitoringBasePayload(**kwargs)
        return await self.call(payload)

    async def set_monitoring_level_req(self, **kwargs):
        payload = call.SetMonitoringLevelPayload(**kwargs)
        return await self.call(payload)

    async def set_network_profile_req(self, **kwargs):
        payload = call.SetNetworkProfilePayload(self, **kwargs)
        return await self.call(payload)

    async def set_variable_monitoring_req(self, **kwargs):
        payload = call.SetVariableMonitoringPayload(**kwargs)
        return await self.call(payload)

    async def trigger_message_req(self, **kwargs):
        payload = call.TriggerMessagePayload(**kwargs)
        return await self.call(payload)

    async def unlock_connector_req(self, **kwargs):
        payload = call.UnlockConnectorPayload(**kwargs)
        return await self.call(payload)

    async def unpublish_firmware_req(self, **kwargs):
        payload = call.UnpublishFirmwarePayload(**kwargs)
        return await self.call(payload)

    async def update_firmware(self, **kwargs):
        payload = call.UpdateFirmwarePayload(**kwargs)
        return await self.call(payload)

