# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

import asyncio
import json
import OpenSSL.crypto as crypto
import logging
import time
from datetime import datetime, timezone
from websockets.exceptions import ConnectionClosedOK, ConnectionClosedError

from ocpp.messages import unpack
from ocpp.charge_point import camel_to_snake_case, snake_to_camel_case, asdict, remove_nones
from ocpp.v16.datatypes import (
    IdTagInfo,
)
from ocpp.v16 import call, call_result
from ocpp.v16.enums import (
    Action,
    RegistrationStatus,
    AuthorizationStatus,
    GenericStatus,
    DataTransferStatus
)
from ocpp.v16 import ChargePoint as cp
from ocpp.routing import on

# for OCPP1.6 PnC whitepaper:
from ocpp.v201 import call_result as call_result201
from ocpp.v201.datatypes import IdTokenInfoType
from ocpp.v201.enums import (AuthorizationStatusType, GenericStatusType, GetCertificateStatusType)

from everest.testing.ocpp_utils.charge_point_utils import MessageHistory, create_cert

logging.basicConfig(level=logging.DEBUG)


class ChargePoint16(cp):

    """Wrapper for the OCPP1.6 chargepoint websocket client. Implementes the communication
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
        """Drops the websocket connection
        """
        await self._connection.close()

    async def _send(self, message):
        """Saves the given message to the MessageHistory and sends the message over the ws connection

        Args:
            message (str): message
        """
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

    @on(Action.BootNotification)
    def on_boot_notification(
        self, charge_point_vendor: str, charge_point_model: str, **kwargs
    ):
        logging.debug("Received a BootNotification")
        # connecting to mqtt server
        return call_result.BootNotificationPayload(
            current_time=datetime.now(timezone.utc).isoformat(),
            interval=1440,
            status=RegistrationStatus.accepted,
        )

    @on(Action.Heartbeat)
    def on_heartbeat(self, **kwargs):
        return call_result.HeartbeatPayload(current_time=datetime.now(timezone.utc).isoformat())

    @on(Action.Authorize)
    def on_authorize(self, **kwargs):
        id_tag_info = IdTagInfo(status=AuthorizationStatus.accepted)
        return call_result.AuthorizePayload(id_tag_info=id_tag_info)

    @on(Action.MeterValues)
    def on_meter_values(self, **kwargs):
        return call_result.MeterValuesPayload()

    @on(Action.StatusNotification)
    def on_status_notification(self, **kwargs):
        return call_result.StatusNotificationPayload()

    @on(Action.StartTransaction)
    def on_start_transaction(self, **kwargs):
        id_tag_info = IdTagInfo(status=AuthorizationStatus.accepted)
        return call_result.StartTransactionPayload(transaction_id=1, id_tag_info=id_tag_info)

    @on(Action.StopTransaction)
    def on_stop_transaction(self, **kwargs):
        return call_result.StopTransactionPayload()

    @on(Action.DiagnosticsStatusNotification)
    def on_diagnostics_status_notification(self, **kwargs):
        return call_result.DiagnosticsStatusNotificationPayload()

    @on(Action.SignCertificate)
    def on_sign_certificate(self, **kwargs):
        self.csr = kwargs['csr']
        return call_result.SignCertificatePayload(GenericStatus.accepted)

    @on(Action.SecurityEventNotification)
    def on_security_event_notification(self, **kwargs):
        return call_result.SecurityEventNotificationPayload()

    @on(Action.SignedFirmwareStatusNotification)
    def on_signed_update_firmware_status_notificaion(self, **kwargs):
        return call_result.SignedFirmwareStatusNotificationPayload()

    @on(Action.LogStatusNotification)
    def on_log_status_notification(self, **kwargs):
        return call_result.LogStatusNotificationPayload()

    @on(Action.FirmwareStatusNotification)
    def on_firmware_status_notification(self, **kwargs):
        return call_result.FirmwareStatusNotificationPayload()

    @on(Action.DataTransfer)
    def on_data_transfer(self, **kwargs):
        req = call.DataTransferPayload(**kwargs)
        if req.vendor_id == 'org.openchargealliance.iso15118pnc':
            if (req.message_id == "Authorize"):
                response = call_result201.AuthorizePayload(
                    id_token_info=IdTokenInfoType(
                        status=AuthorizationStatusType.accepted
                    )
                )
                return call_result.DataTransferPayload(
                    status=DataTransferStatus.accepted,
                    data=json.dumps(remove_nones(
                        snake_to_camel_case(asdict(response))))
                )
            # Should not be part of DataTransfer.req from CP->CSMS
            elif (req.message_id == "CertificateSigned"):
                return call_result.DataTransferPayload(
                    status=DataTransferStatus.unknown_message_id,
                    data="Please implement me"
                )
            # Should not be part of DataTransfer.req from CP->CSMS
            elif req.message_id == "DeleteCertificate":
                return call_result.DataTransferPayload(
                    status=DataTransferStatus.unknown_message_id,
                    data="Please implement me"
                )
            elif req.message_id == "Get15118EVCertificate":
                return call_result.DataTransferPayload(
                    status=DataTransferStatus.unknown_message_id,
                    data="Please implement me"
                )
            elif req.message_id == "GetCertificateStatus":
                return call_result.DataTransferPayload(
                    status=DataTransferStatus.accepted,
                    data=json.dumps(remove_nones(snake_to_camel_case(asdict(
                        call_result201.GetCertificateStatusPayload(
                            status=GetCertificateStatusType.accepted,
                            ocsp_result="anwfdiefnwenfinfinef"
                        )
                    ))))
                )
            # Should not be part of DataTransfer.req from CP->CSMS
            elif req.message_id == "InstallCertificate":
                return call_result.DataTransferPayload(
                    status=DataTransferStatus.unknown_message_id,
                    data="Please implement me"
                )
            elif req.message_id == "SignCertificate":
                return call_result.DataTransferPayload(
                    status=DataTransferStatus.accepted,
                    data=json.dumps(asdict(
                        call_result201.SignCertificatePayload(
                            status=GenericStatusType.accepted
                        )
                    ))
                )
            # Should not be part of DataTransfer.req from CP->CSMS
            elif req.message_id == "TriggerMessage":
                return call_result.DataTransferPayload(
                    status=DataTransferStatus.unknown_message_id,
                    data="Please implement me"
                )
            else:
                return call_result.DataTransferPayload(
                    status=DataTransferStatus.unknown_message_id,
                    data="Please implement me"
                )
        else:
            return call_result.DataTransferPayload(
                status=DataTransferStatus.unknown_vendor_id,
                data="Please implement me"
            )

    async def get_configuration_req(self, **kwargs):
        payload = call.GetConfigurationPayload(**kwargs)
        return await self.call(payload)

    async def change_configuration_req(self, **kwargs):
        payload = call.ChangeConfigurationPayload(**kwargs)
        return await self.call(payload)

    async def clear_cache_req(self, **kwargs):
        payload = call.ClearCachePayload()
        return await self.call(payload)

    async def remote_start_transaction_req(self, **kwargs):
        payload = call.RemoteStartTransactionPayload(**kwargs)
        return await self.call(payload)

    async def remote_stop_transaction_req(self, **kwargs):
        payload = call.RemoteStopTransactionPayload(**kwargs)
        return await self.call(payload)

    async def unlock_connector_req(self, **kwargs):
        payload = call.UnlockConnectorPayload(**kwargs)
        return await self.call(payload)

    async def change_availability_req(self, **kwargs):
        payload = call.ChangeAvailabilityPayload(**kwargs)
        return await self.call(payload)

    async def reset_req(self, **kwargs):
        payload = call.ResetPayload(**kwargs)
        return await self.call(payload)

    async def get_local_list_version_req(self, **kwargs):
        payload = call.GetLocalListVersionPayload()
        return await self.call(payload)

    async def send_local_list_req(self, **kwargs):
        payload = call.SendLocalListPayload(**kwargs)
        return await self.call(payload)

    async def reserve_now_req(self, **kwargs):
        payload = call.ReserveNowPayload(**kwargs)
        return await self.call(payload)

    async def cancel_reservation_req(self, **kwargs):
        payload = call.CancelReservationPayload(**kwargs)
        return await self.call(payload)

    async def trigger_message_req(self, **kwargs):
        payload = call.TriggerMessagePayload(**kwargs)
        return await self.call(payload)

    async def set_charging_profile_req(self, payload: call.SetChargingProfilePayload):
        logging.info(payload)
        return await self.call(payload)

    async def get_composite_schedule(self, payload: call.GetCompositeSchedulePayload) -> call_result.GetCompositeSchedulePayload:
        return await self.call(payload)

    async def get_composite_schedule_req(self, **kwargs) -> call_result.GetCompositeSchedulePayload:
        payload = call.GetCompositeSchedulePayload(**kwargs)
        return await self.call(payload)

    async def clear_charging_profile_req(self, **kwargs):
        payload = call.ClearChargingProfilePayload(**kwargs)
        return await self.call(payload)

    async def data_transfer_req(self, **kwargs):
        payload = call.DataTransferPayload(**kwargs)
        return await self.call(payload)

    async def extended_trigger_message_req(self, **kwargs):
        payload = call.ExtendedTriggerMessagePayload(**kwargs)
        return await self.call(payload)

    async def certificate_signed_req(self, **kwargs):
        if 'certificate_chain' not in kwargs:
            serial_no = 1
            not_before = -86400
            not_after = 86400*365

            ca_cert = crypto.load_certificate(crypto.FILETYPE_PEM, open(
                kwargs['csms_root_ca']).read())
            csr = crypto.load_certificate_request(
                crypto.FILETYPE_PEM, self.csr)
            ca_private_key = crypto.load_privatekey(
                crypto.FILETYPE_PEM, open(kwargs['csms_root_ca_key']).read(), str.encode('ocatool'))

            cert = create_cert(serial_no, not_before,
                               not_after, ca_cert, csr, ca_private_key)

            payload = call.CertificateSignedPayload(
                certificate_chain=cert.decode())
            return await self.call(payload)
        else:
            payload = call.CertificateSignedPayload(
                certificate_chain=kwargs['certificate_chain'])
            return await self.call(payload)

    async def install_certificate_req(self, **kwargs):
        payload = call.InstallCertificatePayload(**kwargs)
        return await self.call(payload)

    async def get_installed_certificate_ids_req(self, **kwargs):
        payload = call.GetInstalledCertificateIdsPayload(**kwargs)
        return await self.call(payload)

    async def delete_certificate_req(self, **kwargs):
        payload = call.DeleteCertificatePayload(**kwargs)
        return await self.call(payload)

    async def get_log_req(self, **kwargs):
        payload = call.GetLogPayload(**kwargs)
        return await self.call(payload)

    async def signed_update_firmware_req(self, **kwargs):
        payload = call.SignedUpdateFirmwarePayload(**kwargs)
        return await self.call(payload)

    async def get_diagnostics_req(self, **kwargs):
        payload = call.GetDiagnosticsPayload(**kwargs)
        return await self.call(payload)

    async def update_firmware_req(self, **kwargs):
        payload = call.UpdateFirmwarePayload(**kwargs)
        return await self.call(payload)
