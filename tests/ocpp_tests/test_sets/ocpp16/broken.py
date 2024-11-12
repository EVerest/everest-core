# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
import logging
from datetime import datetime

from everest.testing.core_utils.controller.test_controller_interface import TestController
from everest.testing.core_utils.everest_core import EverestCore, Requirement
from everest.testing.core_utils.probe_module import ProbeModule
from ocpp.v16 import call, call_result
from ocpp.v16.enums import *
from ocpp.v16.datatypes import IdTagInfo
from ocpp.messages import Call, _DecimalEncoder
from ocpp.charge_point import snake_to_camel_case
from ocpp.routing import on, create_route_map

# fmt: off
from validations import wait_for_callerror_and_validate, validate_boot_notification
from everest.testing.ocpp_utils.fixtures import *
from everest.testing.ocpp_utils.charge_point_v16 import ChargePoint16
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate
from everest_test_utils import *
# fmt: on


async def send_message_without_validation(charge_point_v16, call_msg):
    json_data = json.dumps([
        call_msg.message_type_id,
        call_msg.unique_id,
        call_msg.action,
        call_msg.payload,
    ],
        # By default json.dumps() adds a white space after every separator.
        # By setting the separator manually that can be avoided.
        separators=(',', ':'),
        cls=_DecimalEncoder
    )

    async with charge_point_v16._call_lock:
        await charge_point_v16._send(json_data)


@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-sil-ocpp.yaml'))
@ pytest.mark.asyncio
async def test_missing_payload_field(test_config, charge_point_v16: ChargePoint16, test_controller: TestController, test_utility: TestUtility):
    logging.info("######### test_missing_payload_field #########")

    payload = call.ChangeConfigurationPayload(
        key="WebSocketPingInterval", value="0")
    camel_case_payload = snake_to_camel_case(asdict(payload))

    call_msg = Call(
        unique_id=str(charge_point_v16._unique_id_generator()),
        action=payload.__class__.__name__[:-7],
        payload=remove_nones(camel_case_payload)
    )

    # remove a required payload field
    del call_msg.payload["value"]

    await send_message_without_validation(charge_point_v16, call_msg)

    assert await wait_for_callerror_and_validate(test_utility, charge_point_v16, "FormationViolation")


@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-sil-ocpp.yaml'))
@pytest.mark.skip(reason="libocpp currently does not support this")
@ pytest.mark.asyncio
async def test_additional_payload_field(test_config, charge_point_v16: ChargePoint16, test_controller: TestController, test_utility: TestUtility):
    logging.info("######### test_additional_payload_field #########")

    payload = call.ChangeConfigurationPayload(
        key="WebSocketPingInterval", value="0")
    camel_case_payload = snake_to_camel_case(asdict(payload))

    call_msg = Call(
        unique_id=str(charge_point_v16._unique_id_generator()),
        action=payload.__class__.__name__[:-7],
        payload=remove_nones(camel_case_payload)
    )

    # add a payload field
    call_msg.payload["additional"] = "123"

    await send_message_without_validation(charge_point_v16, call_msg)

    # FIXME: this message seems to be accepted, should be rejected according to spec...
    assert await wait_for_callerror_and_validate(test_utility, charge_point_v16, "FormationViolation")


@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-sil-ocpp.yaml'))
@ pytest.mark.asyncio
async def test_wrong_payload_type(test_config, charge_point_v16: ChargePoint16, test_controller: TestController, test_utility: TestUtility):
    logging.info("######### test_wrong_payload_type #########")

    # key should just be string, but here we set it to array of string
    payload = call.ChangeConfigurationPayload(
        key=["WebSocketPingInterval"], value="0")
    camel_case_payload = snake_to_camel_case(asdict(payload))

    call_msg = Call(
        unique_id=str(charge_point_v16._unique_id_generator()),
        action=payload.__class__.__name__[:-7],
        payload=remove_nones(camel_case_payload)
    )

    await send_message_without_validation(charge_point_v16, call_msg)

    assert await wait_for_callerror_and_validate(test_utility, charge_point_v16, "FormationViolation")


@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-sil-ocpp.yaml'))
@ pytest.mark.asyncio
async def test_wrong_auth_payload(test_config, charge_point_v16: ChargePoint16, test_controller: TestController, test_utility: TestUtility):
    logging.info("######### test_wrong_auth_payload #########")

    @ on(Action.Authorize)
    def on_authorize(**kwargs):
        # send an empty id_tag_info, this should not crash EVerest
        id_tag_info = {}
        res = call_result.AuthorizePayload(id_tag_info=id_tag_info)
        return res

    setattr(charge_point_v16, 'on_authorize', on_authorize)
    charge_point_v16.route_map = create_route_map(charge_point_v16)
    charge_point_v16.route_map[Action.Authorize]['_skip_schema_validation'] = True

    await charge_point_v16.change_configuration_req(key="AuthorizeRemoteTxRequests", value="true")

    test_controller.plug_in()

    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)
    # expect authorize.req
    assert await wait_for_and_validate(test_utility, charge_point_v16,
                                       "Authorize", call.AuthorizePayload(test_config.authorization_info.valid_id_tag_1))

    # this only works if we don't crash from the broken response
    test_controller.swipe(test_config.authorization_info.valid_id_tag_2)
    # expect authorize.req
    assert await wait_for_and_validate(test_utility, charge_point_v16,
                                       "Authorize", call.AuthorizePayload(test_config.authorization_info.valid_id_tag_2))


@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-sil-ocpp.yaml'))
@pytest.mark.probe_module(connections = {
        'ocpp_data_transfer': [Requirement('ocpp', 'data_transfer')]
    })
@pytest.mark.inject_csms_mock
@pytest.mark.asyncio
async def test_data_transfer_with_probe_module(central_system_v16_standalone: CentralSystem, everest_core: EverestCore):
    logging.info("######### test_data_transfer_with_probe_module #########")

    @ on(Action.DataTransfer)
    def on_data_transfer(**kwargs):
        logging.info(f"Received a data transfer message {datetime.now()}")
        req = call.DataTransferPayload(**kwargs)
        if req.vendor_id == 'PIONIX' and req.message_id == 'test_message':
            return call_result.DataTransferPayload(
                status=DataTransferStatus.accepted,
                data="Hello there"
            )
        elif req.vendor_id == 'PIONIX' and req.message_id == 'test_message_broken':
            # purposefully return a wrong payload
            return call_result.AuthorizePayload(
                id_tag_info={}
            )
        return call_result.DataTransferPayload(
            status=DataTransferStatus.unknown_message_id,
            data="Please implement me"
        )

    cs = central_system_v16_standalone.mock
    cs.on_data_transfer.side_effect = on_data_transfer

    probe_module = ProbeModule(everest_core.get_runtime_session())
    probe_module.start()

    await probe_module.wait_to_be_ready()

    charge_point_v16 = await central_system_v16_standalone.wait_for_chargepoint()
    charge_point_v16.route_map[Action.DataTransfer]['_skip_schema_validation'] = True

    result = await probe_module.call_command('ocpp_data_transfer', 'data_transfer', {"request": {
                                             'vendor_id': 'PIONIX', 'message_id': 'test_message', 'data': 'test'}})
    assert 'data' in result and 'status' in result and result['status'] == 'Accepted'

    result = await probe_module.call_command('ocpp_data_transfer', 'data_transfer', {"request": {
                                             'vendor_id': 'PIONIX', 'message_id': 'test_message_unknown', 'data': 'test'}})
    assert 'status' in result and result['status'] == 'UnknownMessageId'

    result = await probe_module.call_command('ocpp_data_transfer', 'data_transfer', {"request": {
                                             'vendor_id': 'PIONIX', 'message_id': 'test_message_broken', 'data': 'test'}})
    assert 'status' in result and result['status'] == 'Rejected'


@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-sil-ocpp.yaml'))
@ pytest.mark.asyncio
async def test_boot_notification_call_error(test_config, central_system_v16: CentralSystem, test_controller: TestController, test_utility: TestUtility):
    logging.info("######### test_boot_notification_call_error #########")

    test_controller.start()

    @ on(Action.BootNotification)
    def on_boot_notification_error(**kwargs):
        raise InternalError()

    @ on(Action.BootNotification)
    def on_boot_notification_accepted(**kwargs):
        return call_result.BootNotificationPayload(
            current_time=datetime.utcnow().isoformat(),
            interval=5,
            status=RegistrationStatus.accepted,
        )

    central_system_v16.function_overrides.append(('on_boot_notification', on_boot_notification_error))
    charge_point_v16 = await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)
    # charge_point_v16.route_map[Action.Authorize]['_skip_schema_validation'] = True

    assert await wait_for_and_validate(test_utility, charge_point_v16, "BootNotification",
                                       call.BootNotificationPayload(charge_box_serial_number="cp001",
                                                                    charge_point_model="Yeti",
                                                                    charge_point_vendor="Pionix",
                                                                    firmware_version="0.1"), validate_boot_notification)

    central_system_v16.function_overrides.append(('on_boot_notification', on_boot_notification_accepted))

    logging.info("disconnect the ws connection...")
    test_controller.disconnect_websocket()

    await asyncio.sleep(1)

    logging.info("connecting the ws connection")
    test_controller.connect_websocket()

    # wait for reconnect
    charge_point_v16 = await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)
    # charge_point_v16.route_map[Action.Authorize]['_skip_schema_validation'] = True

    assert await wait_for_and_validate(test_utility, charge_point_v16, "BootNotification",
                                       call.BootNotificationPayload(charge_box_serial_number="cp001",
                                                                    charge_point_model="Yeti",
                                                                    charge_point_vendor="Pionix",
                                                                    firmware_version="0.1"), validate_boot_notification, timeout=70)

@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-sil-ocpp.yaml'))
@pytest.mark.asyncio
@pytest.mark.inject_csms_mock
async def test_start_transaction_call_error_or_timeout(test_config, central_system_v16: CentralSystem, test_controller: TestController, test_utility: TestUtility):
    logging.info("######### test_start_transaction_call_error_or_timeout #########")

    test_controller.start()

    central_system_v16.mock.on_start_transaction.side_effect = [
            NotImplementedError(),
            NotImplementedError(),
            NotImplementedError(),
            NotImplementedError(),
            call_result.StartTransactionPayload(transaction_id=1, id_tag_info=IdTagInfo(status=AuthorizationStatus.accepted))
        ]

    charge_point_v16 = await central_system_v16.wait_for_chargepoint(wait_for_bootnotification=False)

    test_controller.swipe("DEADBEEF")
    test_controller.plug_in()

    # expect StartTransaction.req
    assert await wait_for_and_validate(test_utility, charge_point_v16, "StartTransaction", {})

    await asyncio.sleep(2)

    test_controller.plug_out()

    assert await wait_for_and_validate(test_utility, charge_point_v16, "StopTransaction", {"transactionId": 1})
