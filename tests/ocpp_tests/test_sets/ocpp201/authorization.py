# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

# fmt: off
import pytest
from datetime import datetime
import logging

from everest.testing.core_utils.controller.test_controller_interface import TestController

from validations import validate_status_notification_201
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility
from everest.testing.ocpp_utils.fixtures import *

from everest_test_utils import * # Needs to be before the datatypes below since it overrides the v201 Action enum with the v16 one
from ocpp.v201.enums import (Action, IdTokenEnumType as IdTokenTypeEnum, SetVariableStatusEnumType, ClearCacheStatusEnumType, ConnectorStatusEnumType,GetVariableStatusEnumType)
from ocpp.v201.datatypes import *
from ocpp.v201 import call as call201
from ocpp.v201 import call_result as call_result201
from ocpp.routing import on, create_route_map
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201

# fmt: on

log = logging.getLogger("authorizationTest")


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_authorize_01(
    charge_point_v201: ChargePoint201,
    test_controller: TestController,
    test_utility: TestUtility,
):
    test_controller.swipe("DEADBEEF")
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "Authorize",
        call201.Authorize(
            id_token=IdTokenType(id_token="DEADBEEF",
                                 type=IdTokenTypeEnum.iso14443)
        ),
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_c09(
    charge_point_v201: ChargePoint201,
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    C09.FR.03
    C09.FR.04
    C09.FR.05
    C09.FR.07
    C09.FR.09
    C09.FR.10
    C09.FR.11
    C09.FR.12
    """

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

    # Set MasterPassGroupId
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AuthCtrlr", "MasterPassGroupId", "00000000"
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

    accepted_tags = ["001", "002"]

    def get_token_info(token: str):
        if token in accepted_tags:
            return IdTokenInfoType(
                status=AuthorizationStatusEnumType.accepted,
                group_id_token=IdTokenType(
                    id_token="123", type=IdTokenTypeEnum.central
                ),
            )
        else:
            return IdTokenInfoType(
                status=AuthorizationStatusEnumType.blocked,
                group_id_token=IdTokenType(
                    id_token="123", type=IdTokenTypeEnum.central
                ),
            )

    @on(Action.authorize)
    def on_authorize(**kwargs):
        msg = call201.Authorize(**kwargs)
        msg_token = IdTokenType(**msg.id_token)
        return call_result201.Authorize(
            id_token_info=get_token_info(msg_token.id_token)
        )

    @on(Action.transaction_event)
    def on_transaction_event(**kwargs):
        msg = call201.TransactionEvent(**kwargs)
        if msg.id_token != None:
            msg_token = IdTokenType(**msg.id_token)
            return call_result201.TransactionEvent(
                id_token_info=get_token_info(msg_token.id_token)
            )
        else:
            return call_result201.TransactionEvent()

    setattr(charge_point_v201, "on_authorize", on_authorize)
    setattr(charge_point_v201, "on_transaction_event", on_transaction_event)
    central_system_v201.chargepoint.route_map = create_route_map(
        central_system_v201.chargepoint
    )

    # Wait for ready and make sure all messages are read into the test_utility
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        {"connectorStatus": "Available", "evseId": 2},
    )
    test_utility.messages.clear()

    test_controller.swipe("001")
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "Authorize",
        call201.Authorize(
            id_token=IdTokenType(id_token="001", type=IdTokenTypeEnum.iso14443)
        ),
    )

    test_controller.plug_in()
    # eventType=Started
    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "TransactionEvent", {
            "eventType": "Started"}
    )

    test_controller.swipe("002")
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "Authorize",
        call201.Authorize(
            id_token=IdTokenType(id_token="002", type=IdTokenTypeEnum.iso14443)
        ),
    )

    # eventType=Ended
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {
            "eventType": "Ended",
            "triggerReason": "StopAuthorized",
            "transactionInfo": {"stoppedReason": "Local"},
        },
    )

    test_controller.plug_out()
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        {"connectorStatus": "Available", "evseId": 1},
    )

    test_utility.messages.clear()

    test_controller.swipe("001")
    test_controller.plug_in()
    # eventType=Started
    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "TransactionEvent", {
            "eventType": "Started"}
    )

    # C09.FR.07: With a valid token in cache with the same groupId the CS shall end
    #            the autorization of the transaction without first sending an AuthorizeRequest
    test_utility.forbidden_actions.append("Authorize")

    test_controller.swipe("002")
    # eventType=Ended
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {
            "eventType": "Ended",
            "triggerReason": "StopAuthorized",
            "transactionInfo": {"stoppedReason": "Local"},
        },
    )

    test_controller.plug_out()
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "StatusNotification",
        {"connectorStatus": "Available", "evseId": 1},
    )

    test_utility.messages.clear()

    # Allow Authorize message again
    test_utility.forbidden_actions.remove("Authorize")

    test_controller.swipe("001")
    test_controller.plug_in()
    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "TransactionEvent", {
            "eventType": "Started"}
    )

    # C09.FR.11: Swipe card with groupIdToken the same as transacton but status blocked SHALL NOT stop the transaction
    #            Instead the plug out should stop the transaction. The transactionEvent will tell us which one it was.
    test_controller.swipe("003")
    test_controller.plug_out()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {
            "eventType": "Ended",
            "triggerReason": "EVCommunicationLost",
            "transactionInfo": {"stoppedReason": "EVDisconnected"},
        },
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_c10_c11_c12(
    charge_point_v201: ChargePoint201,
    test_controller: TestController,
    test_utility: TestUtility,
):

    # prepare data for the test
    evse_id = 1
    connector_id = 1

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

    test_controller.swipe("DEADBEEF")
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "Authorize",
        call201.Authorize(
            id_token=IdTokenType(id_token="DEADBEEF",
                                 type=IdTokenTypeEnum.iso14443)
        ),
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

    await asyncio.sleep(2)

    # because LocalPreAuthorize is true we dont expect an Authorize.req this time
    test_utility.forbidden_actions.append("Authorize")

    test_controller.swipe("DEADBEEF")
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

    # because LocalPreAuthorize is true we dont expect an authorize here
    r: call201.TransactionEvent = call201.TransactionEvent(
        **await wait_for_and_validate(
            test_utility,
            charge_point_v201,
            "TransactionEvent",
            {"eventType": "Started"},
        )
    )

    # Disable LocalPreAuthorize
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AuthCtrlr", "LocalPreAuthorize", "false"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # Set AuthCacheLifeTime to 1s
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AuthCacheCtrlr", "LifeTime", "1"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    test_utility.messages.clear()
    test_controller.plug_out()

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

    # eventType=Ended
    await wait_for_and_validate(
        test_utility, charge_point_v201, "TransactionEvent", {
            "eventType": "Ended"}
    )

    test_utility.forbidden_actions.clear()

    test_controller.swipe("DEADBEEF")

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "Authorize",
        call201.Authorize(
            id_token=IdTokenType(id_token="DEADBEEF",
                                 type=IdTokenTypeEnum.iso14443)
        ),
    )

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


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_c15(
    charge_point_v201: ChargePoint201,
    central_system_v201: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    C15.FR.01
    C15.FR.02
    C15.FR.03
    C15.FR.04
    C15.FR.05
    C15.FR.06
    C15.FR.07
    C15.FR.08
    """
    log.info(
        " ########### Test case C15: Offline Authorization of unknown Id ###########"
    )

    # prepare data for the test
    evse_id = 1
    connector_id = 1

    # make an unknown IdToken
    id_tokenC15 = IdTokenType(
        id_token="8BADF00D", type=IdTokenTypeEnum.iso14443)

    # Generate a transaction response
    # TODO: This needs to be adapted for C15.FR.03-07 use cases
    @on(Action.transaction_event)
    def on_transaction_event(**kwargs):
        msg = call201.TransactionEvent(**kwargs)
        if msg.id_token != None:
            if stop_tx_on_invalid_id != None:
                return call_result201.TransactionEvent(
                    id_token_info=IdTokenInfoType(
                        status=AuthorizationStatusEnumType.unknown
                    )
                )
            else:
                return call_result201.TransactionEvent(
                    id_token_info=IdTokenInfoType(
                        status=AuthorizationStatusEnumType.accepted
                    )
                )
        else:
            return call_result201.TransactionEvent()

    central_system_v201.chargepoint.route_map = create_route_map(
        central_system_v201.chargepoint
    )
    setattr(charge_point_v201, "on_transaction_event", on_transaction_event)

    central_system_v201.function_overrides.append(
        ("on_transaction_event", on_transaction_event)
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

    # set AuthorizeRemoteStart to false
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AuthCtrlr", "AuthorizeRemoteStart", "false"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # accept all transaction requests for now
    stop_tx_on_invalid_id = None

    # Get the value of MaxEnergyOnInvalidId
    r: call_result201.GetVariables = (
        await charge_point_v201.get_config_variables_req(
            "TxCtrlr", "MaxEnergyOnInvalidId"
        )
    )
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0]
    )
    if (
        get_variables_result.attribute_status
        == GetVariableStatusEnumType.not_supported_attribute_type
    ):
        pass
    else:
        # Enable LocalPreAuthorize
        r: call_result201.SetVariables = (
            await charge_point_v201.set_config_variables_req(
                "TxCtrlr", "MaxEnergyOnInvalidId", "0"
            )
        )
        set_variable_result: SetVariableResultType = SetVariableResultType(
            **r.set_variable_result[0]
        )
        assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    log.debug(
        "==============================================C15.FR.08 ===================================="
    )
    log.debug(
        "The Charging Station rejects the unknown IdToken if OfflineTxForUnknownIdEnabled is set False "
    )
    # Disable offline authorization for unknown ID
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "AuthCtrlr", "OfflineTxForUnknownIdEnabled", "false"
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

    # because offline authorization for unknown id is false, it shouldn't allow a transaction
    test_utility.forbidden_actions.append("TransactionEvent")

    # start charging session
    test_controller.plug_in()

    # swipe id tag to authorize
    log.debug("Attempt to Authorize")
    test_controller.swipe(id_tokenC15.id_token)

    await asyncio.sleep(3)

    # Connect CS
    log.debug(" Connect the CS to the CSMS")
    test_controller.connect_websocket()

    # wait for reconnect
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=False
    )

    test_controller.plug_out()

    # TODO: Currently fails here because WS doesnt recognize its disconnected and still sends the Authorize.req

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

    test_utility.messages.clear()
    test_utility.forbidden_actions.clear()

    # C15.FR.08
    log.debug(
        "==============================================C15.FR.08 ===================================="
    )
    log.debug(
        "The Charging Station accepts the unknown IdToken if OfflineTxForUnknownIdEnabled is set True "
    )
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

    # Disconnect CS
    log.debug(" Disconnect the CS from the CSMS")
    test_controller.disconnect_websocket()

    await asyncio.sleep(2)

    # swipe id tag to authorize
    test_controller.swipe(id_tokenC15.id_token)

    # start charging session
    test_controller.plug_in()

    await asyncio.sleep(2)

    # Connect CS
    log.debug(" Connect the CS to the CSMS")
    test_controller.connect_websocket()

    # wait for reconnect
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=False
    )

    log.debug(
        "==============================================C15.FR.02 ===================================="
    )
    # should send a  Transaction event C15.FR.02
    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "TransactionEvent", {
            "eventType": "Started"}
    )

    # swipe id tag to finish transaction
    test_controller.swipe(id_tokenC15.id_token)

    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "TransactionEvent", {
            "eventType": "Ended"}
    )

    # unplug
    test_controller.plug_out()

    test_utility.messages.clear()
    test_utility.forbidden_actions.clear()

    # # C15.FR.03. Commented because preconditions are unmet
    # The transaction is still ongoing AND StopTxOnInvalidId is true AND TxStopPoint does NOT contain: (Authorized OR PowerPathClosed OR EnergyTransfer)
    # log.debug("=================================================C15.FR.03 ======================================================")
    #  # Enable stop Tx on invalid Id
    # r: call_result201.SetVariables = await charge_point_v201.set_config_variables_req("TxCtrlr","StopTxOnInvalidId","true")
    # set_variable_result: SetVariableResultType = SetVariableResultType(**r.set_variable_result[0])
    # assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # # Get the value of StopTxOnInvalidId
    # r: call_result201.GetVariables = await charge_point_v201.get_config_variables_req("TxCtrlr","StopTxOnInvalidId")
    # get_variables_result: GetVariableResultType = GetVariableResultType(**r.get_variable_result[0])
    # assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted
    # stop_tx_on_invalid_id = json.loads(get_variables_result.attribute_value)

    # # Get the value of TxStopPoint
    # r: call_result201.GetVariables = await charge_point_v201.get_config_variables_req("TxCtrlr","TxStopPoint")
    # get_variables_result: GetVariableResultType = GetVariableResultType(**r.get_variable_result[0])
    # assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted

    # # tx_stop_point = json.loads(get_variables_result.attribute_value)
    # # log.debug(" TxStop Point: %s " %(tx_stop_point))

    # # Disconnect CS
    # log.debug(" Disconnect the CS from the CSMS")
    # test_controller.disconnect_websocket()

    # await asyncio.sleep(2)

    # # swipe id tag to authorize
    # test_controller.swipe(id_tokenC15.id_token)

    # # start charging session
    # test_controller.plug_in()

    # await asyncio.sleep(2)

    # # Connect CS
    # log.debug(" Connect the CS to the CSMS")
    # test_controller.connect_websocket()

    # #wait for reconnect
    # charge_point_v201 = await central_system_v201.wait_for_chargepoint(wait_for_bootnotification=False)

    # # should send a  Transaction event C15.FR.02
    # assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent", {"eventType": "Started"})

    # # should send a  Transaction event C15.FR.04 with ended
    # assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent",{
    #                                     "eventType": "Updated",
    #                                     "triggerReason": "Deauthorized",
    #                                     "transactionInfo": {
    #                                     "chargingState": "SuspendedEVSE"}})

    #  # unplug
    # test_controller.plug_out()

    # test_utility.messages.clear()
    # test_utility.forbidden_actions.clear()

    # C15.FR.04 if Transaction event response is not accepted and transaction is ongoing
    log.debug(
        "=================================================C15.FR.04 ======================================================"
    )
    # Enable stop Tx on invalid Id
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "TxCtrlr", "StopTxOnInvalidId", "true"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # Get the value of StopTxOnInvalidId
    r: call_result201.GetVariables = (
        await charge_point_v201.get_config_variables_req("TxCtrlr", "StopTxOnInvalidId")
    )
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0]
    )
    assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted
    stop_tx_on_invalid_id = json.loads(get_variables_result.attribute_value)

    # # Get the value of TxStopPoint
    # r: call_result201.GetVariables = await charge_point_v201.get_config_variables_req("TxCtrlr","TxStopPoint")
    # get_variables_result: GetVariableResultType = GetVariableResultType(**r.get_variable_result[0])
    # assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted

    # tx_stop_point = json.loads(get_variables_result.attribute_value)
    # log.debug(" TxStop Point: %s " %(tx_stop_point))

    # Disconnect CS
    log.debug(" Disconnect the CS from the CSMS")
    test_controller.disconnect_websocket()

    await asyncio.sleep(2)

    # swipe id tag to authorize
    test_controller.swipe(id_tokenC15.id_token)

    # start charging session
    test_controller.plug_in()

    await asyncio.sleep(2)

    # Connect CS
    log.debug(" Connect the CS to the CSMS")
    test_controller.connect_websocket()

    # wait for reconnect
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=False
    )

    # should send a  Transaction event C15.FR.02
    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "TransactionEvent", {
            "eventType": "Started"}
    )

    # should send a  Transaction event C15.FR.04 with ended
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {
            "eventType": "Ended",
            "triggerReason": "Deauthorized",
            "transactionInfo": {"stoppedReason": "DeAuthorized"},
        },
    )

    # #C15.FR.05 The cable should be locked until the user presents the token.
    # Commented beacuse cable currently cannot be locked in place
    # log.debug("==============================C15.FR.05=====================================")
    # test_controller.plug_out()

    # #connector status should still be occupied
    # assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification",
    #                                    call201.StatusNotification(datetime.now().isoformat(),
    #                                                                      ConnectorStatusEnumType.occupied, evse_id, connector_id),
    #                                    validate_status_notification_201)

    #  # swipe id tag to authorize
    # test_controller.swipe(id_tokenC15.id_token)

    #  #connector status should still be available
    # assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification",
    #                                    call201.StatusNotification(datetime.now().isoformat(),
    #                                                                      ConnectorStatusEnumType.available, evse_id, connector_id),
    #                                    validate_status_notification_201)

    # C15.FR.05 The cable should be locked until the user presents the token
    test_controller.plug_out()

    test_utility.messages.clear()
    test_utility.forbidden_actions.clear()

    # C15.FR.06
    log.debug(
        "==============================================C15.FR.06 ===================================="
    )

    # Disable stop Tx on invalid Id
    r: call_result201.SetVariables = (
        await charge_point_v201.set_config_variables_req(
            "TxCtrlr", "StopTxOnInvalidId", "false"
        )
    )
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0]
    )
    assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # Get the value of StopTxOnInvalidId
    r: call_result201.GetVariables = (
        await charge_point_v201.get_config_variables_req("TxCtrlr", "StopTxOnInvalidId")
    )
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0]
    )
    assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted
    stop_tx_on_invalid_id = json.loads(get_variables_result.attribute_value)

    # Disconnect CS
    log.debug("Disconnect the CS from the CSMS")
    test_controller.disconnect_websocket()

    await asyncio.sleep(2)

    # swipe id tag to authorize
    test_controller.swipe(id_tokenC15.id_token)

    # start charging session
    test_controller.plug_in()

    # TODO: This should work with smaller values too. Currently there is an issue when stopped in PrepareCharging state.
    await asyncio.sleep(10)

    # Connect CS
    log.debug("Connect the CS to the CSMS")
    test_controller.connect_websocket()

    # wait for reconnect
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=False
    )

    # should send a  Transaction event C15.FR.02
    assert await wait_for_and_validate(
        test_utility, charge_point_v201, "TransactionEvent", {
            "eventType": "Started"}
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "TransactionEvent",
        {
            "eventType": "Updated",
            "triggerReason": "ChargingStateChanged",
            "transactionInfo": {"chargingState": "SuspendedEVSE"},
        },
    )

    # swipe id tag to finish transaction
    test_controller.swipe(id_tokenC15.id_token)

    # unplug
    test_controller.plug_out()

    test_utility.messages.clear()
    test_utility.forbidden_actions.clear()

    #  #C15.FR.06
    # Commented because MaxEnergyOnInvalidId isn't implemented
    # log.debug("==============================================C15.FR.07 ====================================")

    # #Disable stop Tx on invalid Id
    # r: call_result201.SetVariables = await charge_point_v201.set_config_variables_req("TxCtrlr","StopTxOnInvalidId","false")
    # set_variable_result: SetVariableResultType = SetVariableResultType(**r.set_variable_result[0])
    # assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # # Get the value of StopTxOnInvalidId
    # r: call_result201.GetVariables = await charge_point_v201.get_config_variables_req("TxCtrlr","StopTxOnInvalidId")
    # get_variables_result: GetVariableResultType = GetVariableResultType(**r.get_variable_result[0])
    # assert get_variables_result.attribute_status == GetVariableStatusEnumType.accepted
    # stop_tx_on_invalid_id = json.loads(get_variables_result.attribute_value)

    #  #Set a value for MaxEnergyOnInvalidId
    # r: call_result201.SetVariables = await charge_point_v201.set_config_variables_req("TxCtrlr","MaxEnergyOnInvalidId","16")
    # set_variable_result: SetVariableResultType = SetVariableResultType(**r.set_variable_result[0])
    # assert set_variable_result.attribute_status == SetVariableStatusEnumType.accepted

    # # Get the value of MaxEnergyOnInvalidId
    # r: call_result201.GetVariables = await charge_point_v201.get_config_variables_req("TxCtrlr","MaxEnergyOnInvalidId")
    # get_variables_result: GetVariableResultType = GetVariableResultType(**r.get_variable_result[0])
    # if get_variables_result.attribute_status == GetVariableStatusEnumType.accepted:
    #     max_energy_on_invalid_id = json.loads(get_variables_result.attribute_value)
    #     log.debug("max energy on invalid Id %s " %max_energy_on_invalid_id)
    # else:
    #     max_energy_on_invalid_id = None

    # # Disconnect CS
    # log.debug(" Disconnect the CS from the CSMS")
    # test_controller.disconnect_websocket()

    # await asyncio.sleep(2)

    # # swipe id tag to authorize
    # test_controller.swipe(id_tokenC15.id_token)

    # # start charging session
    # test_controller.plug_in()

    # await asyncio.sleep(2)

    # # Connect CS
    # log.debug(" Connect the CS to the CSMS")
    # test_controller.connect_websocket()

    # #wait for reconnect
    # charge_point_v201 = await central_system_v201.wait_for_chargepoint(wait_for_bootnotification=False)

    # # should send a  Transaction event C15.FR.02
    # assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent", {"eventType": "Started"})

    # swipe id tag to finish transaction
    test_controller.swipe(id_tokenC15.id_token)

    # unplug
    test_controller.plug_out()
