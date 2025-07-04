# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import asyncio
import logging
from datetime import datetime, timezone

from ocpp.routing import on
from ocpp.v21 import ChargePoint as cp
from ocpp.v21 import call, call_result
from ocpp.v21.datatypes import IdTokenInfoType, SetVariableDataType, GetVariableDataType, ComponentType, VariableType
from ocpp.v21.enums import (
    Action,
    RegistrationStatusEnumType,
    AuthorizationStatusEnumType,
    AttributeEnumType,
    NotifyEVChargingNeedsStatusEnumType,
    GenericStatusEnumType
)
from websockets.exceptions import ConnectionClosedOK, ConnectionClosedError

from everest.testing.ocpp_utils.charge_point_utils import MessageHistory

logging.basicConfig(level=logging.DEBUG)


class ChargePoint21(cp):
    """Wrapper for the OCPP2.1 chargepoint websocket client. Implementes the communication
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
        await self._connection.close()

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

    @on(Action.boot_notification)
    def on_boot_notification(self, **kwargs):
        logging.debug("Received a BootNotification")
        return call_result.BootNotification(current_time=datetime.now().isoformat(),
                                            interval=300, status=RegistrationStatusEnumType.accepted)

    @on(Action.status_notification)
    def on_status_notification(self, **kwargs):
        return call_result.StatusNotification()

    @on(Action.heartbeat)
    def on_heartbeat(self, **kwargs):
        return call_result.Heartbeat(current_time=datetime.now(timezone.utc).isoformat())

    @on(Action.authorize)
    def on_authorize(self, **kwargs):
        return call_result.Authorize(
            id_token_info=IdTokenInfoType(
                status=AuthorizationStatusEnumType.accepted
            )
        )

    @on(Action.notify_report)
    def on_notify_report(self, **kwargs):
        return call_result.NotifyReport()

    @on(Action.log_status_notification)
    def on_log_status_notification(self, **kwargs):
        return call_result.LogStatusNotification()

    @on(Action.firmware_status_notification)
    def on_firmware_status_notification(self, **kwargs):
        return call_result.FirmwareStatusNotification()

    @on(Action.transaction_event)
    def on_transaction_event(self, **kwargs):
        return call_result.TransactionEvent()

    @on(Action.meter_values)
    def on_meter_values(self, **kwargs):
        return call_result.MeterValues()

    @on(Action.notify_charging_limit)
    def on_notify_charging_limit(self, **kwargs):
        return call_result.NotifyChargingLimit()

    @on(Action.notify_customer_information)
    def on_notify_customer_information(self, **kwargs):
        return call_result.NotifyCustomerInformation()

    @on(Action.notify_ev_charging_needs)
    def on_notify_ev_charging_needs(self, **kwargs):
        return call_result.NotifyEVChargingNeeds(status=NotifyEVChargingNeedsStatusEnumType.accepted)

    @on(Action.notify_ev_charging_schedule)
    def on_notify_ev_charging_schedule(self, **kwargs):
        return call_result.NotifyEVChargingSchedule(status=GenericStatusEnumType.accepted)

    @on(Action.notify_event)
    def on_notify_event(self, **kwargs):
        return call_result.NotifyEvent()

    @on(Action.notify_monitoring_report)
    def on_notify_monitoring_report(self, **kwargs):
        return call_result.NotifyMonitoringReport()

    @on(Action.publish_firmware_status_notification)
    def on_publish_firmware_status_notification(self, **kwargs):
        return call_result.PublishFirmwareStatusNotification()

    @on(Action.report_charging_profiles)
    def on_report_charging_profiles(self, **kwargs):
        return call_result.ReportChargingProfiles()

    @on(Action.reservation_status_update)
    def on_reservation_status_update(self, **kwargs):
        return call_result.ReservationStatusUpdate()

    @on(Action.security_event_notification)
    def on_security_event_notification(self, **kwargs):
        return call_result.SecurityEventNotification()

    @on(Action.sign_certificate)
    def on_sign_certificate(self, **kwargs):
        return call_result.SignCertificate(status=GenericStatusEnumType.accepted)

    @on(Action.get15118_ev_certificate)
    def on_get_15118_ev_certificate(self, **kwargs):
        return call_result.Get15118EVCertificate(status=GenericStatusEnumType.accepted,
                                                 exi_response="")

    @on(Action.get_certificate_status)
    def on_get_certificate_status(self, **kwargs):
        return call_result.GetCertificateStatus(status=GenericStatusEnumType.accepted,
                                                ocsp_result="")

    @on(Action.data_transfer)
    def on_data_transfer(self, **kwargs):
        return call_result.DataTransfer(status=GenericStatusEnumType.accepted, data="")

    async def set_variables_req(self, **kwargs):
        payload = call.SetVariables(**kwargs)
        return await self.call(payload)

    async def set_config_variables_req(self, component_name, variable_name, value):
        el = SetVariableDataType(
            attribute_value=value,
            attribute_type=AttributeEnumType.actual,
            component=ComponentType(
                name=component_name
            ),
            variable=VariableType(
                name=variable_name
            )
        )
        payload = call.SetVariables([el])
        return await self.call(payload)

    async def get_variables_req(self, **kwargs):
        payload = call.GetVariables(**kwargs)
        return await self.call(payload)

    async def get_config_variables_req(self, component_name, variable_name):
        el = GetVariableDataType(
            component=ComponentType(
                name=component_name
            ),
            variable=VariableType(
                name=variable_name
            ),
            attribute_type=AttributeEnumType.actual
        )
        payload = call.GetVariables([el])
        return await self.call(payload)

    async def get_base_report_req(self, **kwargs):
        payload = call.GetBaseReport(**kwargs)
        return await self.call(payload)

    async def get_report_req(self, **kwargs):
        payload = call.GetReport(**kwargs)
        return await self.call(payload)

    async def reset_req(self, **kwargs):
        payload = call.Reset(**kwargs)
        return await self.call(payload)

    async def request_start_transaction_req(self, **kwargs):
        payload = call.RequestStartTransaction(**kwargs)
        return await self.call(payload)

    async def request_stop_transaction_req(self, **kwargs):
        payload = call.RequestStopTransaction(**kwargs)
        return await self.call(payload)

    async def change_availablility_req(self, **kwargs):
        payload = call.ChangeAvailability(**kwargs)
        return await self.call(payload)

    async def clear_cache_req(self, **kwargs):
        payload = call.ClearCache(**kwargs)
        return await self.call(payload)

    async def cancel_reservation_req(self, **kwargs):
        payload = call.CancelReservation(**kwargs)
        return await self.call(payload)

    async def certificate_signed_req(self, **kwargs):
        payload = call.CertificateSigned(**kwargs)
        return await self.call(payload)

    async def clear_charging_profile_req(self, **kwargs):
        payload = call.ClearChargingProfile(**kwargs)
        return await self.call(payload)

    async def clear_display_message_req(self, **kwargs):
        payload = call.ClearDisplayMessage(**kwargs)
        return await self.call(payload)

    async def clear_charging_limit_req(self, **kwargs):
        payload = call.ClearedChargingLimit(**kwargs)
        return await self.call(payload)

    async def clear_variable_monitoring_req(self, **kwargs):
        payload = call.ClearVariableMonitoringd(**kwargs)
        return await self.call(payload)

    async def cost_update_req(self, **kwargs):
        payload = call.CostUpdated(**kwargs)
        return await self.call(payload)

    async def customer_information_req(self, **kwargs):
        payload = call.CustomerInformation(**kwargs)
        return await self.call(payload)

    async def data_transfer_req(self, **kwargs):
        payload = call.DataTransfer(**kwargs)
        return await self.call(payload)

    async def delete_certificate_req(self, **kwargs):
        payload = call.DeleteCertificate(**kwargs)
        return await self.call(payload)

    async def get_charging_profiles_req(self, **kwargs):
        payload = call.GetChargingProfiles(**kwargs)
        return await self.call(payload)

    async def get_composite_schedule_req(self, **kwargs):
        payload = call.GetCompositeSchedule(**kwargs)
        return await self.call(payload)

    async def get_display_nessages_req(self, **kwargs):
        payload = call.GetDisplayMessages(**kwargs)
        return await self.call(payload)

    async def get_installed_certificate_ids_req(self, **kwargs):
        payload = call.GetInstalledCertificateIds(**kwargs)
        return await self.call(payload)

    async def get_local_list_version(self, **kwargs):
        payload = call.GetLocalListVersion(**kwargs)
        return await self.call(payload)

    async def get_log_req(self, **kwargs):
        payload = call.GetLog(**kwargs)
        return await self.call(payload)

    async def get_transaction_status_req(self, **kwargs):
        payload = call.GetTransactionStatus(**kwargs)
        return await self.call(payload)

    async def install_certificate_req(self, **kwargs):
        payload = call.InstallCertificate(**kwargs)
        return await self.call(payload)

    async def publish_firmware_req(self, **kwargs):
        payload = call.PublishFirmware(**kwargs)
        return await self.call(payload)

    async def reserve_now_req(self, **kwargs):
        payload = call.ReserveNow(**kwargs)
        return await self.call(payload)

    async def send_local_list_req(self, **kwargs):
        payload = call.SendLocalList(**kwargs)
        return await self.call(payload)

    async def set_charging_profile_req(self, **kwargs):
        payload = call.SetChargingProfile(**kwargs)
        return await self.call(payload)

    async def set_display_message_req(self, **kwargs):
        payload = call.SetDisplayMessage(**kwargs)
        return await self.call(payload)

    async def set_monitoring_base_req(self, **kwargs):
        payload = call.SetMonitoringBase(**kwargs)
        return await self.call(payload)

    async def set_monitoring_level_req(self, **kwargs):
        payload = call.SetMonitoringLevel(**kwargs)
        return await self.call(payload)

    async def set_network_profile_req(self, **kwargs):
        payload = call.SetNetworkProfile(**kwargs)
        return await self.call(payload)

    async def set_variable_monitoring_req(self, **kwargs):
        payload = call.SetVariableMonitoring(**kwargs)
        return await self.call(payload)

    async def trigger_message_req(self, **kwargs):
        payload = call.TriggerMessage(**kwargs)
        return await self.call(payload)

    async def unlock_connector_req(self, **kwargs):
        payload = call.UnlockConnector(**kwargs)
        return await self.call(payload)

    async def unpublish_firmware_req(self, **kwargs):
        payload = call.UnpublishFirmware(**kwargs)
        return await self.call(payload)

    async def update_firmware(self, **kwargs):
        payload = call.UpdateFirmware(**kwargs)
        return await self.call(payload)
