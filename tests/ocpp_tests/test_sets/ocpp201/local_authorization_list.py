# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

# fmt: off
import pytest
import logging

from everest.testing.core_utils.controller.test_controller_interface import TestController

from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility, ValidationMode
from everest.testing.ocpp_utils.fixtures import *

from everest_test_utils import *
from ocpp.v201.enums import (IdTokenType as IdTokenTypeEnum)
from ocpp.v201.enums import *
from ocpp.v201.datatypes import *
from ocpp.v201 import call as call201
from ocpp.v201 import call_result as call_result201
from ocpp.routing import on, create_route_map
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
# fmt: on


@ pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_D01_D02(charge_point_v201: ChargePoint201, central_system_v201: CentralSystem, test_controller: TestController, test_utility: TestUtility):

    id_token_123 = IdTokenType(id_token='123', type=IdTokenTypeEnum.iso14443)
    id_token_124 = IdTokenType(id_token='124', type=IdTokenTypeEnum.iso14443)
    id_token_125 = IdTokenType(id_token='125', type=IdTokenTypeEnum.iso14443)

    id_token_accepted = IdTokenInfoType(status=AuthorizationStatusType.accepted)
    id_token_blocked = IdTokenInfoType(status=AuthorizationStatusType.blocked)

    # D02.FR.01
    async def check_list_version(expected_version: int):
        r: call_result201.GetLocalListVersionPayload = await charge_point_v201.get_local_list_version()
        assert r.version_number == expected_version

    # D01.FR.12
    async def check_list_size(expected_size: int):
        r: call_result201.GetVariablesPayload = await charge_point_v201.get_config_variables_req("LocalAuthListCtrlr", "Entries")
        get_variables_result: GetVariableResultType = GetVariableResultType(
            **r.get_variable_result[0])
        assert get_variables_result.attribute_status == GetVariableStatusType.accepted
        assert get_variables_result.attribute_value == str(expected_size)

    # LocalAuthListCtrlr needs to be avaialable
    r: call_result201.GetVariablesPayload = await charge_point_v201.get_config_variables_req("LocalAuthListCtrlr", "Available")
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0])
    assert get_variables_result.attribute_status == GetVariableStatusType.accepted
    assert get_variables_result.attribute_value == "true"

    # Enable local list
    r: call_result201.SetVariablesPayload = await charge_point_v201.set_config_variables_req("LocalAuthListCtrlr", "Enabled", "true")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # D02.FR.02 LocalAuthListEnabled is true amd CSMS has not sent any update
    await check_list_version(0)
    await check_list_size(0)

    # D01.FR.18 VersionNumber shall be greater than 0 (we fail otherwise)
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=0,
        update_type=UpdateType.full
    )
    assert r.status == SendLocalListStatusType.failed

    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=0,
        update_type=UpdateType.differential
    )
    assert r.status == SendLocalListStatusType.failed

    await check_list_version(0)
    await check_list_size(0)

    # D01.FR.01
    # D01.FR.02
    # Add first list version
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=10,
        update_type=UpdateType.full,
        local_authorization_list=[
            AuthorizationData(id_token=id_token_123, id_token_info=id_token_accepted),
            AuthorizationData(id_token=id_token_124, id_token_info=id_token_accepted)
        ]
    )
    assert r.status == SendLocalListStatusType.accepted

    await check_list_version(10)
    await check_list_size(2)

    # D01.FR.04
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=20,
        update_type=UpdateType.full
    )
    assert r.status == SendLocalListStatusType.accepted

    await check_list_version(20)
    await check_list_size(0)

    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=12,
        update_type=UpdateType.full,
        local_authorization_list=[
            AuthorizationData(id_token=id_token_123, id_token_info=id_token_accepted),
            AuthorizationData(id_token=id_token_124, id_token_info=id_token_accepted),
            AuthorizationData(id_token=id_token_125, id_token_info=id_token_blocked)
        ]
    )
    assert r.status == SendLocalListStatusType.accepted

    await check_list_version(12)
    await check_list_size(3)

    # D01.FR.05
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=15,
        update_type=UpdateType.differential
    )
    assert r.status == SendLocalListStatusType.accepted

    await check_list_version(15)
    await check_list_size(3)

    # D01.FR.06
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=25,
        update_type=UpdateType.full,
        local_authorization_list=[
            AuthorizationData(id_token=id_token_123, id_token_info=id_token_accepted),
            AuthorizationData(id_token=id_token_124, id_token_info=id_token_accepted),
            AuthorizationData(id_token=id_token_124, id_token_info=id_token_accepted)
        ]
    )
    assert r.status == SendLocalListStatusType.failed

    await check_list_version(15)
    await check_list_size(3)

    # idTokenInfo is required when UpdateType is full
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=3,
        update_type=UpdateType.full,
        local_authorization_list=[
            AuthorizationData(id_token=id_token_123, id_token_info=id_token_accepted),
            AuthorizationData(id_token=id_token_124)
        ]
    )
    assert r.status == SendLocalListStatusType.failed

    await check_list_version(15)
    await check_list_size(3)


    # D01.FR.15
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=25,
        update_type=UpdateType.full,
        local_authorization_list=[
            AuthorizationData(id_token=id_token_123, id_token_info=id_token_accepted),
            AuthorizationData(id_token=id_token_124, id_token_info=id_token_accepted)
        ]
    )
    assert r.status == SendLocalListStatusType.accepted

    await check_list_version(25)
    await check_list_size(2)


    # D01.FR.16 Update
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=26,
        update_type=UpdateType.differential,
        local_authorization_list=[
            AuthorizationData(id_token=id_token_123, id_token_info=id_token_blocked)
        ]
    )
    assert r.status == SendLocalListStatusType.accepted

    await check_list_version(26)
    await check_list_size(2)

    # D01.FR.16 Add
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=27,
        update_type=UpdateType.differential,
        local_authorization_list=[
            AuthorizationData(id_token=id_token_125, id_token_info=id_token_accepted)
        ]
    )
    assert r.status == SendLocalListStatusType.accepted

    await check_list_version(27)
    await check_list_size(3)

    # D01.FR.17 Remove if empty idTokenInfo
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=28,
        update_type=UpdateType.differential,
        local_authorization_list=[
            AuthorizationData(id_token=id_token_123)
        ]
    )
    assert r.status == SendLocalListStatusType.accepted

    await check_list_version(28)
    await check_list_size(2)

    # D01.FR.19 Smaller or equal version_number should be ignored with status set to VersionMismatch
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=27,
        update_type=UpdateType.differential,
        local_authorization_list=[
            AuthorizationData(id_token=id_token_125)
        ]
    )
    assert r.status == SendLocalListStatusType.version_mismatch

    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=28,
        update_type=UpdateType.differential,
        local_authorization_list=[
            AuthorizationData(id_token=id_token_125)
        ]
    )
    assert r.status == SendLocalListStatusType.version_mismatch

    await check_list_version(28)
    await check_list_size(2)


    # D01.FR.13
    # Disable auth list again to check if version returns to 0
    r: call_result201.SetVariablesPayload = await charge_point_v201.set_config_variables_req("LocalAuthListCtrlr", "Enabled", "false")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted


    # D02.FR.03: Always return 0 when LocalAuthListEnabled is false
    await check_list_version(0)

    # Disabled so should not be able to send list
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=1,
        update_type=UpdateType.full,
        local_authorization_list=[
            AuthorizationData(id_token=id_token_123, id_token_info=id_token_accepted),
            AuthorizationData(id_token=id_token_124, id_token_info=id_token_accepted)
        ]
    )
    assert r.status == SendLocalListStatusType.failed


async def prepare_auth_cache(charge_point_v201: ChargePoint201, central_system_v201: CentralSystem, test_controller: TestController, test_utility: TestUtility, accepted_tags: [], rejected_tags: []):
    # Prepare the cache with valid and invalid tags

    def get_token_info(token: str):
        if token in accepted_tags:
            return IdTokenInfoType(status=AuthorizationStatusType.accepted)
        else:
            return IdTokenInfoType(status=AuthorizationStatusType.blocked)

    @ on(Action.Authorize)
    def on_authorize(**kwargs):
        msg = call201.AuthorizePayload(**kwargs)
        msg_token = IdTokenType(**msg.id_token)
        return call_result201.AuthorizePayload(id_token_info=get_token_info(msg_token.id_token))

    setattr(charge_point_v201, 'on_authorize', on_authorize)
    central_system_v201.chargepoint.route_map = create_route_map(central_system_v201.chargepoint)

    test_utility.validation_mode = ValidationMode.STRICT
    for tag in accepted_tags:
        test_controller.swipe(tag)
        test_controller.plug_in()
        assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent", {"eventType": "Started"})
        test_controller.swipe(tag)
        test_controller.plug_out()
        assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"connectorStatus": "Available", "evseId": 1})

    for tag in rejected_tags:
        test_controller.swipe(tag)
        assert await wait_for_and_validate(test_utility, charge_point_v201, "Authorize", {"idToken": {"idToken": tag }})

    test_utility.validation_mode = ValidationMode.EASY
    test_utility.messages.clear()


@ pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_C13(charge_point_v201: ChargePoint201, central_system_v201: CentralSystem, test_controller: TestController, test_utility: TestUtility):

    # LocalAuthListCtrlr needs to be avaialable
    r: call_result201.GetVariablesPayload = await charge_point_v201.get_config_variables_req("LocalAuthListCtrlr", "Available")
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0])
    assert get_variables_result.attribute_status == GetVariableStatusType.accepted
    assert get_variables_result.attribute_value == "true"

    # Enable local list
    r: call_result201.SetVariablesPayload = await charge_point_v201.set_config_variables_req("LocalAuthListCtrlr", "Enabled", "true")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # Set OfflineThreshold
    r: call_result201.SetVariablesPayload = await charge_point_v201.set_config_variables_req("OCPPCommCtrlr", "OfflineThreshold", "2")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # Disable offline tx for unknown id
    r: call_result201.SetVariablesPayload = await charge_point_v201.set_config_variables_req("AuthCtrlr", "OfflineTxForUnknownIdEnabled", "false")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted


    id_token_123 = IdTokenType(id_token='123', type=IdTokenTypeEnum.iso14443)
    id_token_124 = IdTokenType(id_token='124', type=IdTokenTypeEnum.iso14443)
    id_token_125 = IdTokenType(id_token='125', type=IdTokenTypeEnum.iso14443)

    id_token_accepted = IdTokenInfoType(status=AuthorizationStatusType.accepted)
    id_token_blocked = IdTokenInfoType(status=AuthorizationStatusType.blocked)

    async def check_list_version(expected_version: int):
        r: call_result201.GetLocalListVersionPayload = await charge_point_v201.get_local_list_version()
        assert r.version_number == expected_version

    async def check_list_size(expected_size: int):
        r: call_result201.GetVariablesPayload = await charge_point_v201.get_config_variables_req("LocalAuthListCtrlr", "Entries")
        get_variables_result: GetVariableResultType = GetVariableResultType(
            **r.get_variable_result[0])
        assert get_variables_result.attribute_status == GetVariableStatusType.accepted
        assert get_variables_result.attribute_value == str(expected_size)

    await prepare_auth_cache(charge_point_v201=charge_point_v201,
                             central_system_v201=central_system_v201,
                             test_controller=test_controller,
                             test_utility=test_utility,
                             accepted_tags=[id_token_123.id_token, id_token_125.id_token],
                             rejected_tags=[id_token_124.id_token])


    # Add first list version
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=10,
        update_type=UpdateType.full,
        local_authorization_list=[
            AuthorizationData(id_token=id_token_123, id_token_info=id_token_accepted),
            AuthorizationData(id_token=id_token_124, id_token_info=id_token_accepted),
            AuthorizationData(id_token=id_token_125, id_token_info=id_token_blocked)
        ]
    )
    assert r.status == SendLocalListStatusType.accepted

    await check_list_version(10)
    await check_list_size(3)

    test_utility.forbidden_actions.append("Authorize")


    # C13.FR.02
    # C13.FR.03
    # Valid token in the local list may be authorized offline
    # Check AuthList: Valid, Cache: Invalid
    # Expected result: Start session
    logging.info("disconnect the ws connection...")
    test_controller.disconnect_websocket()

    await asyncio.sleep(2)

    test_controller.swipe(id_token_123.id_token)
    test_controller.plug_in()

    await asyncio.sleep(2)

    logging.info("connecting the ws connection")
    test_controller.connect_websocket()

    # wait for reconnect
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(wait_for_bootnotification=False)

    assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent", {
        "eventType": "Started",
        "idToken": {
            "idToken": id_token_123.id_token,
            "type": "ISO14443"
        }
    })

    test_controller.swipe(id_token_123.id_token)
    test_controller.plug_out()

    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"connectorStatus": "Available", "evseId": 1})



    # C13.FR.01
    # Invalid token in local list may not be authorized
    # Check AuthList: Invalid, Cache: Valid
    # Expected result: No session started
    logging.info("disconnect the ws connection...")
    test_controller.disconnect_websocket()

    await asyncio.sleep(2)

    test_controller.swipe(id_token_125.id_token)
    test_controller.plug_in()

    await asyncio.sleep(5)

    logging.info("connecting the ws connection")
    test_controller.connect_websocket()

    test_utility.messages.clear()
    test_utility.forbidden_actions.append("TransactionEvent")

    # wait for reconnect
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(wait_for_bootnotification=False)

    test_controller.plug_out()
    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"connectorStatus": "Available", "evseId": 1})

    test_utility.forbidden_actions.remove("TransactionEvent")


    # C13.FR.04
    # With OfflineTxForUnknownIdEnabled == true
    # Invalid token in local list may not be authorized
    # Unkown token may be authorized
    # See errata for case C13.FR.04

    # Enable offline tx for unknown id
    r: call_result201.SetVariablesPayload = await charge_point_v201.set_config_variables_req("AuthCtrlr", "OfflineTxForUnknownIdEnabled", "true")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted


    logging.info("disconnect the ws connection...")
    test_controller.disconnect_websocket()

    await asyncio.sleep(2)

    test_controller.swipe(id_token_125.id_token)
    test_controller.swipe("unknown")
    test_controller.plug_in()

    await asyncio.sleep(5)

    logging.info("connecting the ws connection")
    test_controller.connect_websocket()

    # wait for reconnect
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(wait_for_bootnotification=False)

    assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent", {
        "eventType": "Started",
        "idToken": {
            "idToken": "unknown",
            "type": "ISO14443"
        }
    })

    test_controller.plug_out()
    test_controller.swipe("unknown")

    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"connectorStatus": "Available", "evseId": 1})



@ pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_C14(charge_point_v201: ChargePoint201, central_system_v201: CentralSystem, test_controller: TestController, test_utility: TestUtility):

    # LocalAuthListCtrlr needs to be avaialable
    r: call_result201.GetVariablesPayload = await charge_point_v201.get_config_variables_req("LocalAuthListCtrlr", "Available")
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0])
    assert get_variables_result.attribute_status == GetVariableStatusType.accepted
    assert get_variables_result.attribute_value == "true"

    # AuthCacheCtrlr needs to be avaialable
    r: call_result201.GetVariablesPayload = await charge_point_v201.get_config_variables_req("AuthCacheCtrlr", "Available")
    get_variables_result: GetVariableResultType = GetVariableResultType(
        **r.get_variable_result[0])
    assert get_variables_result.attribute_status == GetVariableStatusType.accepted
    assert get_variables_result.attribute_value == "true"

    # Enable local list
    r: call_result201.SetVariablesPayload = await charge_point_v201.set_config_variables_req("LocalAuthListCtrlr", "Enabled", "true")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # Enable AuthCacheCtrlr
    r: call_result201.SetVariablesPayload = await charge_point_v201.set_config_variables_req("AuthCacheCtrlr", "Enabled", "true")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # Disable offline tx for unknown id
    r: call_result201.SetVariablesPayload = await charge_point_v201.set_config_variables_req("AuthCtrlr", "OfflineTxForUnknownIdEnabled", "false")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    id_token_123 = IdTokenType(id_token='123', type=IdTokenTypeEnum.iso14443)
    id_token_124 = IdTokenType(id_token='124', type=IdTokenTypeEnum.iso14443)

    id_token_accepted = IdTokenInfoType(status=AuthorizationStatusType.accepted)
    id_token_blocked = IdTokenInfoType(status=AuthorizationStatusType.blocked)

    async def check_list_version(expected_version: int):
        r: call_result201.GetLocalListVersionPayload = await charge_point_v201.get_local_list_version()
        assert r.version_number == expected_version

    async def check_list_size(expected_size: int):
        r: call_result201.GetVariablesPayload = await charge_point_v201.get_config_variables_req("LocalAuthListCtrlr", "Entries")
        get_variables_result: GetVariableResultType = GetVariableResultType(
            **r.get_variable_result[0])
        assert get_variables_result.attribute_status == GetVariableStatusType.accepted
        assert get_variables_result.attribute_value == str(expected_size)

    await prepare_auth_cache(charge_point_v201=charge_point_v201,
                             central_system_v201=central_system_v201,
                             test_controller=test_controller,
                             test_utility=test_utility,
                             accepted_tags=[id_token_123.id_token],
                             rejected_tags=[id_token_124.id_token])


    # Add first list version
    r: call_result201.SendLocalListPayload = await charge_point_v201.send_local_list_req(
        version_number=10,
        update_type=UpdateType.full,
        local_authorization_list=[
            AuthorizationData(id_token=id_token_124, id_token_info=id_token_accepted),
            AuthorizationData(id_token=id_token_123, id_token_info=id_token_blocked)
        ]
    )
    assert r.status == SendLocalListStatusType.accepted

    await check_list_version(10)
    await check_list_size(2)

    await asyncio.sleep(1)


    # C14.FR.02
    # Check AuthList: Valid, Cache: Invalid
    # Expected result: Start session without authorizeReq
    test_utility.messages.clear()
    test_utility.forbidden_actions.append("Authorize")

    test_controller.swipe(id_token_124.id_token)
    test_controller.plug_in()

    test_utility.validation_mode = ValidationMode.STRICT

    assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent", {
        "eventType": "Started",
        "idToken": {
            "idToken": id_token_124.id_token,
            "type": "ISO14443"
        }
    })

    test_utility.validation_mode = ValidationMode.EASY

    await asyncio.sleep(1)

    test_controller.swipe(id_token_124.id_token)
    test_controller.plug_out()

    test_utility.validation_mode = ValidationMode.STRICT
    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"connectorStatus": "Available", "evseId": 1})
    test_utility.validation_mode = ValidationMode.EASY

    test_utility.forbidden_actions.remove("Authorize")

    # C14.FR.01
    # C14.FR.03
    # Check AuthList: Invalid, Cache: Valid
    # Expected result: Send autorize request

    await asyncio.sleep(1)
    test_utility.messages.clear()

    test_controller.swipe(id_token_123.id_token)
    test_controller.plug_in()

    test_utility.validation_mode = ValidationMode.STRICT
    assert await wait_for_and_validate(test_utility, charge_point_v201, "Authorize", call201.AuthorizePayload(id_token=id_token_123))

    assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent", {
        "eventType": "Started",
        "idToken": {
            "idToken": id_token_123.id_token,
            "type": "ISO14443"
        }
    })

    test_utility.validation_mode = ValidationMode.EASY

    test_controller.swipe(id_token_123.id_token)
    test_controller.plug_out()

    test_utility.validation_mode = ValidationMode.STRICT
    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"connectorStatus": "Available", "evseId": 1})
    test_utility.validation_mode = ValidationMode.EASY
