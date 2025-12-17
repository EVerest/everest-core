# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
import asyncio
from datetime import datetime

# fmt: off
import logging

from everest.testing.core_utils.controller.test_controller_interface import TestController

from ocpp.routing import on, create_route_map
from ocpp.v201 import call as call201
from ocpp.v201 import call_result as call_result201
from ocpp.v201.enums import *
from ocpp.v201.datatypes import *
from everest.testing.ocpp_utils.fixtures import *
from everest_test_utils import * # Needs to be before the datatypes below since it overrides the v201 Action enum with the v16 one
from ocpp.v201.enums import (Action, IdTokenEnumType as IdTokenTypeEnum, SetVariableStatusEnumType, ClearCacheStatusEnumType, ConnectorStatusEnumType)
from validations import validate_status_notification_201
from everest.testing.core_utils._configuration.libocpp_configuration_helper import GenericOCPP2XConfigAdjustment, OCPP2XConfigVariableIdentifier
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility, ValidationMode, validate_incoming_messages
# fmt: on

log = logging.getLogger("transactionsTest")


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_E04(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    E04.FR.01
    ...
    """
    # prepare data for the test
    evse_id1 = 1
    connector_id = 1

    evse_id2 = 2

    # make an unknown IdToken
    id_token = IdTokenType(id_token="8BADF00D", type=IdTokenTypeEnum.iso14443)

    log.info(
        "##################### E04: Transaction started while charging station is offline #################"
    )

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id=evse_id1,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id=evse_id2,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )

    # Enable AuthCacheCtrlr
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AuthCacheCtrlr", "Enabled", "true"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # Enable LocalPreAuthorize
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AuthCtrlr", "LocalPreAuthorize", "true"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # Set AuthCacheLifeTime
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AuthCacheCtrlr", "LifeTime", "86400"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # Clear cache
    r: call_result201.ClearCache = await charge_point_v201.clear_cache_req()
    assert r.status == ClearCacheStatusEnumType.accepted

    # E04.FR.03 the queued transaction messages must contain the flag 'offline' as TRUE

    # Enable offline authorization for unknown ID
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AuthCtrlr", "OfflineTxForUnknownIdEnabled", "true"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # Enable AlignedDataSignReadings (Not implemented yet)
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AlignedDataCtrlr", "SignReadings", "true"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    test_utility.messages.clear()

    # Disconnect CS
    log.debug(" Disconnect the CS from the CSMS")
    test_controller.disconnect_websocket()

    await asyncio.sleep(2)

    # swipe id tag to authorize
    test_controller.swipe(id_token.id_token)

    # start charging session
    test_controller.plug_in()

    # charge for 30 seconds
    await asyncio.sleep(30)

    # swipe id tag to de-authorize
    test_controller.swipe(id_token.id_token)

    # stop charging session
    test_controller.plug_out()

    await asyncio.sleep(10)

    # Connect CS
    log.debug(" Connect the CS to the CSMS")
    test_controller.connect_websocket()

    # wait for reconnect
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=False
    )

    # All offline generated transaction messaages must be marked offline = True

    # should send a  Transaction event C15.FR.02
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Started", "offline": True},
    )
    # should send a  Transaction event C15.FR.02
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Updated", "offline": True},
    )
    # should send a  Transaction event C15.FR.02
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Ended", "offline": True},
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.inject_csms_mock
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "OCPPCommCtrlr", "MessageTimeout", "Actual"
                ),
                "1",
            ),
            (
                OCPP2XConfigVariableIdentifier(
                    "OCPPCommCtrlr", "MessageAttemptInterval", "Actual"
                ),
                "1",
            ),
            (
                OCPP2XConfigVariableIdentifier(
                    "OCPPCommCtrlr", "MessageAttempts", "Actual"
                ),
                "3",
            ),
        ]
    )
)
async def test_cleanup_transaction_events_after_max_attempts_exhausted(
    central_system: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    Test if transaction events are properly cleaned up after the max message attempts
    ...
    """
    # prepare data for the test
    evse_id1 = 1
    connector_id = 1

    evse_id2 = 2
    connector_id2 = 1

    # make an unknown IdToken
    id_token = IdTokenType(id_token="8BADF00D", type=IdTokenTypeEnum.iso14443)

    test_controller.start()
    charge_point_v201 = await central_system.wait_for_chargepoint(
        wait_for_bootnotification=True
    )
    tx_event_attempt = SetVariableDataType(attribute_value="3", attribute_type=AttributeEnumType.actual,
                                           component=ComponentType(name="OCPPCommCtrlr"), variable=VariableType(name="MessageAttempts", instance="TransactionEvent"))
    tx_event_interval = SetVariableDataType(attribute_value="1", attribute_type=AttributeEnumType.actual,
                                            component=ComponentType(name="OCPPCommCtrlr"), variable=VariableType(name="MessageAttemptInterval", instance="TransactionEvent"))
    r: call_result201.SetVariables = (
        await charge_point_v201.set_variables_req(set_variable_data=[tx_event_attempt, tx_event_interval])
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[1]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id=evse_id1,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id=evse_id2,
            connector_id=connector_id2,
        ),
        validate_status_notification_201,
    )

    # return a CALLERROR for the transaction event
    central_system.mock.on_transaction_event.side_effect = [
        NotImplementedError()]

    # swipe id tag to authorize
    test_controller.swipe(id_token.id_token)

    # start charging session
    test_controller.plug_in()

    # should send a  Transaction event
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Started", "offline": False},
    )
    test_utility.validation_mode = ValidationMode.STRICT

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Started", "offline": False},
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Started", "offline": False},
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Updated", "offline": False},
    )

    central_system.mock.on_transaction_event.reset()

    # respond properly to transaction events again
    central_system.mock.on_transaction_event.side_effect = [
        call_result201.TransactionEvent()
    ]

    # stop charging session
    test_controller.plug_out()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Ended", "offline": False},
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        {"evseId": 1, "connectorId": 1, "connectorStatus": "Available"},
    )

    test_controller.stop()
    # add a sleep to allow time for reboot
    await asyncio.sleep(2)
    test_controller.start()

    # no attempts on delivering the transaction message should be made
    assert await validate_incoming_messages(test_utility, charge_point_v201, "TransactionEvent", {}, timeout=10) is False
    test_utility.validation_mode = ValidationMode.EASY


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.inject_csms_mock
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "AlignedDataCtrlr", "AlignedDataTxEndedInterval", "Actual"
                ),
                "5",
            )
        ]
    )
)
async def test_two_parallel_transactions(
    central_system: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    Test if two parallel transactions work
    ...
    """
    # prepare data for the test
    evse_id1 = 1
    connector_id = 1

    evse_id2 = 2
    connector_id2 = 1

    # make an unknown IdToken
    id_token = IdTokenType(id_token="8BADF00D", type=IdTokenTypeEnum.iso14443)
    id_token2 = IdTokenType(id_token="ABAD1DEA", type=IdTokenTypeEnum.iso14443)

    test_controller.start()
    charge_point_v201 = await central_system.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id=evse_id1,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id=evse_id2,
            connector_id=connector_id2,
        ),
        validate_status_notification_201,
    )

    # swipe id tag to authorize
    test_controller.swipe(id_token.id_token, connectors=[1])

    # start charging session
    test_controller.plug_in(evse_id1)

    # should send a  Transaction event
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Started", "offline": False},
    )

    # swipe id tag to authorize
    test_controller.swipe(id_token2.id_token, connectors=[2])

    # start charging session
    test_controller.plug_in(evse_id2)

    # should send a Transaction event
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Started", "offline": False},
    )
    # let transactions run for a bit
    await asyncio.sleep(10)
    # # stop charging session
    test_controller.plug_out(evse_id1)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Ended", "offline": False},
    )
    test_controller.plug_out(evse_id2)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Ended", "offline": False},
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_id_token_info_updated_in_tx_event(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    # prepare data for the test
    evse_id1 = 1
    connector_id = 1

    evse_id2 = 2

    # make an unknown IdToken
    id_token = IdTokenType(id_token="8BADF00D", type=IdTokenTypeEnum.iso14443)

    log.info(
        "##################### Transaction with token info updated in transaction event #################"
    )

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # expect StatusNotification with status available
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id=evse_id1,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        call201.StatusNotification(
            datetime.now().isoformat(),
            ConnectorStatusEnumType.available,
            evse_id=evse_id2,
            connector_id=connector_id,
        ),
        validate_status_notification_201,
    )

    @on(Action.transaction_event)
    def on_transaction_event(**kwargs):
        msg = call201.TransactionEvent(**kwargs)
        if msg.id_token != None:
            msg_token = IdTokenType(**msg.id_token)
            return call_result201.TransactionEvent(
                id_token_info=IdTokenInfoType(
                    status=AuthorizationStatusEnumType.accepted,
                    group_id_token=IdTokenType(
                        id_token="123", type=IdTokenTypeEnum.central
                    ),
                )
            )
        else:
            return call_result201.TransactionEvent()

    setattr(charge_point_v201, "on_transaction_event", on_transaction_event)
    central_system_v201.chargepoint.route_map = create_route_map(
        central_system_v201.chargepoint
    )
    # swipe id tag to authorize
    test_controller.swipe(id_token.id_token)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "Authorize",
        {},
    )
    # start charging session
    test_controller.plug_in()
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Started"},
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Updated"},
    )

    # charge for 10 seconds
    await asyncio.sleep(10)

    @on(Action.authorize)
    def on_authorize(**kwargs):
        msg = call201.Authorize(**kwargs)
        msg_token = IdTokenType(**msg.id_token)
        return call_result201.Authorize(
            id_token_info=IdTokenInfoType(
                status=AuthorizationStatusEnumType.accepted,
                group_id_token=IdTokenType(
                    id_token="123", type=IdTokenTypeEnum.central
                ),
            )
        )
    setattr(charge_point_v201, "on_authorize", on_authorize)
    central_system_v201.chargepoint.route_map = create_route_map(
        central_system_v201.chargepoint
    )
    # swipe id tag to de-authorize
    id_token = IdTokenType(id_token="8BADF00A", type=IdTokenTypeEnum.iso14443)
    test_controller.swipe(id_token.id_token)
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "Authorize",
        {},
    )
    # should send a  Transaction event C15.FR.02
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Ended"},
    )

    # stop charging session
    test_controller.plug_out()

@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.everest_core_config('everest-config-ocpp201-ps.yaml')
@pytest.mark.use_temporary_persistent_store
@pytest.mark.ocpp_config_adaptions(
        GenericOCPP2XConfigAdjustment(
            [
                (
                    OCPP2XConfigVariableIdentifier(
                        "InternalCtrlr",
                        "ResumeTransactionsOnBoot",
                        "Actual",
                    ),
                    "true",
                ),
            ]
        )
    )
async def test_stop_pending_transactions(
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    logging.info("######### test_stop_pending_transactions #########")

    # prepare data for the test
    evse_id = 1
    connector_id = 1
    remote_start_id = 1
    id_token = IdTokenType(id_token="DEADBEEF", type=IdTokenTypeEnum.iso14443)

    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )


    await charge_point_v201.request_start_transaction_req(
        id_token=id_token, remote_start_id=remote_start_id, evse_id=evse_id
    )

    test_controller.plug_in()

    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "TransactionEvent", {
            "eventType": "Started"}
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {"eventType": "Updated"},
    )

    # charge for some time...
    logging.debug("Charging for a while...")
    await asyncio.sleep(2)

    test_controller.stop()

    await asyncio.sleep(2)

    test_controller.start()

    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=False
    )

    await asyncio.sleep(2)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {
            "eventType": "Ended",
            "transactionInfo": {
                "stoppedReason": "PowerLoss"
            }
        },
    )