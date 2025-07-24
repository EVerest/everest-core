# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from datetime import datetime, timezone
import pytest
# fmt: off
import sys
import os

from everest.testing.core_utils.controller.test_controller_interface import TestController

sys.path.append(os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../..")))
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility, ValidationMode
from everest.testing.ocpp_utils.fixtures import *
from ocpp.routing import on, after, create_route_map
from ocpp.v201.enums import (IdTokenEnumType as IdTokenTypeEnum, TriggerMessageStatusEnumType)
from ocpp.v201.enums import *
from ocpp.v201.datatypes import *
from ocpp.v201 import call as call201
from validations import validate_status_notification_201, validate_measurands_match
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
from everest_test_utils import *
from validations import wait_for_callerror_and_validate
# fmt: on


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_F01_F02_F03(
    charge_point_v201: ChargePoint201,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    F01.FR.01
    F01.FR.02
    F01.FR.03
    F01.FR.05
    F01.FR.07
    F01.FR.14
    F01.FR.19
    F01.FR.23
    """

    # prepare data for the test
    evse_id = 1
    connector_id = 1
    remote_start_id = 1
    id_token = IdTokenType(id_token="DEADBEEF", type=IdTokenTypeEnum.iso14443)
    evse = EVSEType(id=evse_id, connector_id=connector_id)

    # Disable AuthCacheCtrlr
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AuthCacheCtrlr", "Enabled", "false"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # get configured measurands in order to compare them to the measurands used in TransactionEvent(eventType=Started) for testing F01.FR.14
    r: call_result201.GetVariables = (
        await charge_point_v201.get_config_variables_req(
            "SampledDataCtrlr", "TxStartedMeasurands"
        )
    )
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0]
    )
    assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted
    expected_started_measurands = get_variables_result.attribute_value.split(
        ",")

    # get configured measurands in order to compare them to the measurands used in TransactionEvent(eventType=Started) for testing F01.FR.14
    r: call_result201.GetVariables = (
        await charge_point_v201.get_config_variables_req(
            "SampledDataCtrlr", "TxUpdatedMeasurands"
        )
    )
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0]
    )
    assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted

    # get configured measurands in order to compare them to the measurands used in TransactionEvent(eventType=Started) for testing F01.FR.14
    r: call_result201.GetVariables = (
        await charge_point_v201.get_config_variables_req(
            "SampledDataCtrlr", "TxEndedMeasurands"
        )
    )
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0]
    )
    assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted
    expected_ended_measurands = get_variables_result.attribute_value.split(",")

    # set AuthorizeRemoteStart to true
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AuthCtrlr", "AuthorizeRemoteStart", "true"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # put EVSE to unavailable
    await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusEnumType.inoperative, evse=evse
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.unavailable,
            evse_id,
            connector_id,
        ),
        validate_status_notification_201,
    )

    # send RequestStartTransaction while EVSE in unavailable and expect rejected
    await charge_point_v201.request_start_transaction_req(
        id_token=id_token, remote_start_id=remote_start_id
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "RequestStartTransaction",
        call_result201.RequestStartTransaction(
            status=RequestStartStopStatusEnumType.rejected
        ),
    )

    # put EVSE to available
    await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusEnumType.operative, evse=evse
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id,
            connector_id,
        ),
        validate_status_notification_201,
    )

    await asyncio.sleep(2)

    # send RequestStartTransaction without evse_id and expect Rejected
    await charge_point_v201.request_start_transaction_req(
        id_token=id_token, remote_start_id=remote_start_id
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "RequestStartTransaction",
        call_result201.RequestStartTransaction(
            status=RequestStartStopStatusEnumType.rejected
        ),
    )

    # send RequestStartTransaction and expect Accepted
    await charge_point_v201.request_start_transaction_req(
        id_token=id_token, remote_start_id=remote_start_id, evse_id=evse_id
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "RequestStartTransaction",
        call_result201.RequestStartTransaction(
            status=RequestStartStopStatusEnumType.accepted
        ),
    )

    # because AuthorizeRemoteStart is true we expect an Authorize here
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "Authorize",
        call201.Authorize(id_token=id_token),
    )

    test_controller.plug_in()
    # eventType=Started
    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "TransactionEvent", {
            "eventType": "Started"}
    )
    test_utility.messages.clear()
    test_controller.plug_out()
    # eventType=Ended
    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "TransactionEvent", {
            "eventType": "Ended"}
    )

    test_utility.messages.clear()

    # set AuthorizeRemoteStart to false
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AuthCtrlr", "AuthorizeRemoteStart", "false"
        )
    )
    test_utility.forbidden_actions.append("Authorize")

    test_controller.plug_in()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.occupied,
            evse_id,
            connector_id,
        ),
        validate_status_notification_201,
    )

    # send RequestStartTransaction and expect Accepted
    await charge_point_v201.request_start_transaction_req(
        id_token=id_token, remote_start_id=remote_start_id, evse_id=evse_id
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "RequestStartTransaction",
        call_result201.RequestStartTransaction(
            status=RequestStartStopStatusEnumType.accepted
        ),
    )

    # because AuthorizeRemoteStart is false we directly expect a TransactionEvent(eventType=Started)
    r: call201.TransactionEvent = call201.TransactionEvent(
        **await wait_for_and_validate(
            test_utility,
            charge_point_v201,
            "TransactionEvent",
            {"eventType": "Started"},
        )
    )

    transaction = TransactionType(**r.transaction_info)

    # do some basic checks on TransactionEvent
    assert r.trigger_reason == TriggerReasonEnumType.remote_start
    assert r.event_type == TransactionEventEnumType.started
    assert EVSEType(**r.evse) == evse
    assert IdTokenType(**r.id_token) == id_token

    # check if the configured measurands are part of the MeterValue of the TransactionEvent
    assert validate_measurands_match(
        MeterValueType(**r.meter_value[0]), expected_started_measurands
    )

    await asyncio.sleep(2)

    # send RequestStartTransaction and expect Rejected because transaction_id is wrong
    await charge_point_v201.request_stop_transaction_req(
        transaction_id="wrong_transaction_id"
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "RequestStopTransaction",
        call_result201.RequestStartTransaction(
            status=RequestStartStopStatusEnumType.rejected
        ),
    )

    # send RequestStartTransaction and expect Accepted
    await charge_point_v201.request_stop_transaction_req(
        transaction_id=transaction.transaction_id
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "RequestStopTransaction",
        call_result201.RequestStartTransaction(
            status=RequestStartStopStatusEnumType.accepted
        ),
    )

    r: call201.TransactionEvent = call201.TransactionEvent(
        **await wait_for_and_validate(
            test_utility, charge_point_v201, "TransactionEvent", {
                "eventType": "Ended"}
        )
    )

    transaction = TransactionType(**r.transaction_info)

    assert r.trigger_reason == TriggerReasonEnumType.remote_stop
    assert transaction.stopped_reason == ReasonEnumType.remote
    assert transaction.remote_start_id == remote_start_id

    assert validate_measurands_match(
        MeterValueType(**r.meter_value[0]), expected_ended_measurands
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_F06(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    F06.FR.03
    F06.FR.04
    F06.FR.05
    F06.FR.05
    F06.FR.06
    F06.FR.07
    F06.FR.08
    F06.FR.09
    F06.FR.10
    F06.FR.11
    F06.FR.12
    F06.FR.17
    """

    # Skipped for now (Do test NotImplemented):
    # LogStatusNotification
    # FirmwareStatusNotification
    # PublishFirmwareStatusNotification
    # SignChargingStationCertificate
    # SignV2GCertificate
    # SignCombinedCertificate

    # Test BootNotification

    @on(Action.boot_notification)
    def on_boot_notification_pending(**kwargs):
        return call_result201.BootNotification(
            current_time=datetime.now().isoformat(),
            interval=5,
            status=RegistrationStatusEnumType.rejected,
        )

    @on(Action.boot_notification)
    def on_boot_notification_accepted(**kwargs):
        return call_result201.BootNotification(
            current_time=datetime.now(timezone.utc).isoformat(),
            interval=300,
            status=RegistrationStatusEnumType.accepted,
        )

    @after(Action.boot_notification)
    async def after_boot_notification(reason, charging_station, **kwargs):
        # F06.FR.17: Reject trigger messages when boot_notification_state is Accepted
        r: call_result201.TriggerMessage = await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.boot_notification
        )
        assert TriggerMessageStatusEnumType(
            r.status) == TriggerMessageStatusEnumType.rejected

    central_system_v201.function_overrides.append(
        ("on_boot_notification", on_boot_notification_pending)
    )

    test_controller.start()
    charge_point_v201: ChargePoint201 = await central_system_v201.wait_for_chargepoint()

    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.boot_notification
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.accepted

    test_utility.validation_mode = ValidationMode.STRICT
    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "BootNotification", {
            "reason": "Triggered"}
    )

    central_system_v201.function_overrides.append(
        ("on_boot_notification", on_boot_notification_accepted)
    )
    setattr(charge_point_v201, "after_boot_notification",
            on_boot_notification_accepted)
    central_system_v201.chargepoint.route_map = create_route_map(
        central_system_v201.chargepoint
    )

    # Trigger again so we respond with accepted
    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.boot_notification
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.accepted

    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "BootNotification", {
            "reason": "Triggered"}
    )
    test_utility.validation_mode = ValidationMode.EASY

    # Limit the amount of data in metervalues and transactions

    # add a sleep to ensure that the csms sends boot notification response before triggering SetVariables
    await asyncio.sleep(2)

    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AlignedDataCtrlr", "Measurands", MeasurandEnumType.current_import
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "SampledDataCtrlr", "TxStartedMeasurands", MeasurandEnumType.power_active_import
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "SampledDataCtrlr", "TxUpdatedMeasurands", MeasurandEnumType.power_active_import
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "SampledDataCtrlr", "TxEndedMeasurands", MeasurandEnumType.power_active_import
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # Test Heartbeat

    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.heartbeat
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.accepted

    test_utility.validation_mode = ValidationMode.STRICT
    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "Heartbeat", {}, timeout=2
    )
    test_utility.validation_mode = ValidationMode.EASY

    # Test Metervalues

    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.meter_values, evse=EVSEType(
                id=1)
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.accepted

    def check_meter_value(response):
        for meter_value in response.meter_value:
            value = MeterValueType(**meter_value)
            for sampled_value in value.sampled_value:
                value = SampledValueType(**sampled_value)
                assert value.measurand == MeasurandEnumType.current_import
                assert value.context == ReadingContextEnumType.trigger

    r: call201.MeterValues = call201.MeterValues(
        **await wait_for_and_validate(
            test_utility, charge_point_v201, "MeterValues", {"evseId": 1}
        )
    )
    check_meter_value(r)

    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.meter_values
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.accepted

    r: call201.MeterValues = call201.MeterValues(
        **await wait_for_and_validate(
            test_utility, charge_point_v201, "MeterValues", {"evseId": 1}
        )
    )
    check_meter_value(r)
    r: call201.MeterValues = call201.MeterValues(
        **await wait_for_and_validate(
            test_utility, charge_point_v201, "MeterValues", {"evseId": 2}
        )
    )
    check_meter_value(r)

    r = await charge_point_v201.trigger_message_req(
        requested_message=MessageTriggerEnumType.meter_values, evse=EVSEType(
            id=3)
    )
    assert await wait_for_callerror_and_validate(
        test_utility, charge_point_v201, "OccurrenceConstraintViolation"
    )

    # Test StatusNotification

    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.status_notification,
            evse=EVSEType(id=1, connector_id=1),
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.accepted

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        {"evseId": 1, "connectorId": 1, "connectorStatus": "Available"},
    )

    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.status_notification,
            evse=EVSEType(id=1, connector_id=2),
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.rejected

    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.status_notification,
            evse=EVSEType(id=3, connector_id=1),
        )
    )
    assert await wait_for_callerror_and_validate(
        test_utility, charge_point_v201, "OccurrenceConstraintViolation"
    )

    # F06.FR.12
    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.status_notification
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.rejected

    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.status_notification,
            evse=EVSEType(id=1),
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.rejected

    # Test TransactionEvent

    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.transaction_event, evse=EVSEType(
                id=1)
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.rejected

    test_controller.swipe("001", connectors=[1, 2])
    test_controller.plug_in()

    r: call201.TransactionEvent = call201.TransactionEvent(
        **await wait_for_and_validate(
            test_utility,
            charge_point_v201,
            "TransactionEvent",
            {"eventType": "Started", "evse": {"id": 1}},
        )
    )
    transaction_1: TransactionType = TransactionType(**r.transaction_info)

    test_utility.validation_mode = ValidationMode.STRICT
    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.transaction_event, evse=EVSEType(
                id=1)
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.accepted

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {
            "eventType": "Updated",
            "triggerReason": "Trigger",
            "transactionInfo": {"transactionId": transaction_1.transaction_id},
        },
    )

    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.transaction_event
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.accepted

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {
            "eventType": "Updated",
            "triggerReason": "Trigger",
            "transactionInfo": {"transactionId": transaction_1.transaction_id},
        },
    )
    test_utility.validation_mode = ValidationMode.EASY

    test_controller.swipe("002", connectors=[1, 2])
    test_controller.plug_in(connector_id=2)

    r: call201.TransactionEvent = call201.TransactionEvent(
        **await wait_for_and_validate(
            test_utility,
            charge_point_v201,
            "TransactionEvent",
            {"eventType": "Started", "evse": {"id": 2}},
        )
    )
    transaction_2: TransactionType = TransactionType(**r.transaction_info)

    r: call201.TransactionEvent = call201.TransactionEvent(
        **await wait_for_and_validate(
            test_utility,
            charge_point_v201,
            "TransactionEvent",
            {
                "eventType": "Updated",
                "triggerReason": "ChargingStateChanged",
                "transactionInfo": {"transactionId": transaction_2.transaction_id},
            },
        )
    )

    test_utility.validation_mode = ValidationMode.STRICT
    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.transaction_event, evse=EVSEType(
                id=2)
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.accepted

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {
            "eventType": "Updated",
            "triggerReason": "Trigger",
            "transactionInfo": {"transactionId": transaction_2.transaction_id},
        },
    )

    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.transaction_event
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.accepted

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {
            "eventType": "Updated",
            "triggerReason": "Trigger",
            "transactionInfo": {"transactionId": transaction_1.transaction_id},
        },
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {
            "eventType": "Updated",
            "triggerReason": "Trigger",
            "transactionInfo": {"transactionId": transaction_2.transaction_id},
        },
    )
    test_utility.validation_mode = ValidationMode.EASY

    test_controller.swipe("001", connectors=[1, 2])
    test_controller.swipe("002", connectors=[1, 2])
    test_controller.plug_out()
    test_controller.plug_out(connector_id=2)

    # Test LogStatusNotificaiton
    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.log_status_notification
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.accepted

    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "LogStatusNotification", {
            "status": "Idle"}
    )

    # Waiting for the log callback to be implemented in the everest core
    # log_param = LogParametersType(
    #     remote_location="ftp://user:12345@localhost:2121",
    #     oldest_timestamp=(datetime.now(timezone.utc) - timedelta(days=1)).isoformat(),
    #     latest_timestamp=datetime.now(timezone.utc).isoformat()
    # )

    # r: call_result201.TriggerMessage = await charge_point_v201.get_log_req(log=log_param, log_type=LogType.diagnostics_log, request_id=10)
    # assert await wait_for_and_validate(test_utility, charge_point_v201, "LogStatusNotification", {"status": "Uploading"})
    # assert await wait_for_and_validate(test_utility, charge_point_v201, "LogStatusNotification", {"status": "UploadFailed"})

    # r: call_result201.TriggerMessage = await charge_point_v201.trigger_message_req(requested_message=MessageTriggerEnumType.log_status_notification)
    # assert TriggerMessageStatusEnumType(r.status) == TriggerMessageStatusEnumType.accepted
    # test_utility.validation_mode = ValidationMode.STRICT
    # assert await wait_for_and_validate(test_utility, charge_point_v201, "LogStatusNotification", {"status": "Idle"})
    # test_utility.validation_mode = ValidationMode.EASY

    # Test FirmwareStatusNotification
    r: call_result201.TriggerMessage = (
        await charge_point_v201.trigger_message_req(
            requested_message=MessageTriggerEnumType.firmware_status_notification
        )
    )
    assert TriggerMessageStatusEnumType(
        r.status) == TriggerMessageStatusEnumType.accepted

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "FirmwareStatusNotification",
        {"status": "Idle"},
    )

    # Waiting for the update firmware callback to be implemented in the everest core
    # firmware_type = FirmwareType(
    #     location="ftp://user:12345@localhost:2121",
    #     retrieve_date_time=(datetime.now(timezone.utc) + timedelta(seconds=10)).isoformat()
    # )

    # test_utility.validation_mode = ValidationMode.STRICT

    # r: call_result201.UpdateFirmware = await charge_point_v201.update_firmware(firmware=firmware_type, request_id=10)

    # assert await wait_for_and_validate(test_utility, charge_point_v201, "FirmwareStatusNotification", {"status": "DownloadScheduled", "requestId": 10})
    # r: call_result201.TriggerMessage = await charge_point_v201.trigger_message_req(requested_message=MessageTriggerEnumType.firmware_status_notification)
    # assert TriggerMessageStatusEnumType(r.status) == TriggerMessageStatusEnumType.accepted
    # assert await wait_for_and_validate(test_utility, charge_point_v201, "FirmwareStatusNotification", {"status": "DownloadScheduled", "requestId": 10})

    # assert await wait_for_and_validate(test_utility, charge_point_v201, "FirmwareStatusNotification", {"status": "Downloading", "requestId": 10})

    # assert await wait_for_and_validate(test_utility, charge_point_v201, "FirmwareStatusNotification", {"status": "Downloading", "requestId": 10})

    # assert await wait_for_and_validate(test_utility, charge_point_v201, "FirmwareStatusNotification", {"status": "DownloadFailed", "requestId": 10})

    # r: call_result201.TriggerMessage = await charge_point_v201.trigger_message_req(requested_message=MessageTriggerEnumType.firmware_status_notification)
    # assert TriggerMessageStatusEnumType(r.status) == TriggerMessageStatusEnumType.accepted
    # assert await wait_for_and_validate(test_utility, charge_point_v201, "FirmwareStatusNotification", {"status": "Idle"})

    # test_utility.validation_mode = ValidationMode.EASY
