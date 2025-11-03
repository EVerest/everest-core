# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from datetime import timezone
from unittest.mock import Mock

import pytest

import logging

from everest.testing.ocpp_utils.central_system import CentralSystem

# Needs to be before the datatypes below since it overrides the v201 Action enum with the v16 one
from everest_test_utils import *
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
from everest.testing.core_utils.controller.test_controller_interface import TestController

from everest_test_utils_probe_modules import (probe_module,
                                              ProbeModuleCostAndPriceDisplayMessageConfigurationAdjustment)

from ocpp.v201 import call as call201
from ocpp.v201 import call_result as call_result201
from ocpp.v201.enums import (
    IdTokenEnumType as IdTokenTypeEnum, ConnectorStatusEnumType)
from ocpp.v201.datatypes import *

from everest.testing.core_utils._configuration.libocpp_configuration_helper import (
    GenericOCPP2XConfigAdjustment,
    OCPP2XConfigVariableIdentifier,
)
from validations import validate_status_notification_201

log = logging.getLogger("ocpp201DisplayMessageTest")


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.inject_csms_mock
@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp201-costandprice.yaml'))
@pytest.mark.ocpp_config_adaptions(GenericOCPP2XConfigAdjustment([
    (OCPP2XConfigVariableIdentifier("DisplayMessageCtrlr", "DisplayMessageCtrlrAvailable", "Actual"),
     "true"),
    (OCPP2XConfigVariableIdentifier("DisplayMessageCtrlr", "QRCodeDisplayCapable",
                                    "Actual"), "true"),
    (OCPP2XConfigVariableIdentifier("DisplayMessageCtrlr", "DisplayMessageLanguage", "Actual"),
     "en"),
    (OCPP2XConfigVariableIdentifier("TariffCostCtrlr", "TariffCostCtrlrAvailableTariff", "Actual"),
     "true"),
    (OCPP2XConfigVariableIdentifier("TariffCostCtrlr", "TariffCostCtrlrAvailableCost", "Actual"),
     "true"),
    (OCPP2XConfigVariableIdentifier("TariffCostCtrlr",
     "TariffCostCtrlrEnabledTariff", "Actual"), "true"),
    (OCPP2XConfigVariableIdentifier("TariffCostCtrlr",
     "TariffCostCtrlrEnabledCost", "Actual"), "true"),
    (OCPP2XConfigVariableIdentifier("TariffCostCtrlr", "NumberOfDecimalsForCostValues", "Actual"),
     "5"),
    (OCPP2XConfigVariableIdentifier("OCPPCommCtrlr", "MessageTimeout", "Actual"),
     "1"),
    (OCPP2XConfigVariableIdentifier("OCPPCommCtrlr", "MessageAttemptInterval",
                                    "Actual"), "1"),
    (OCPP2XConfigVariableIdentifier("OCPPCommCtrlr", "MessageAttempts", "Actual"),
     "3"),
    (OCPP2XConfigVariableIdentifier("AuthCacheCtrlr", "AuthCacheCtrlrEnabled", "Actual"),
     "true"),
    (OCPP2XConfigVariableIdentifier("AuthCtrlr", "LocalPreAuthorize",
                                    "Actual"), "true"),
    (OCPP2XConfigVariableIdentifier("AuthCacheCtrlr", "AuthCacheLifeTime", "Actual"),
     "86400"),
    (OCPP2XConfigVariableIdentifier("DisplayMessageCtrlr", "DisplayMessageSupportedPriorities",
                                    "Actual"), "AlwaysFront,NormalCycle"),
    (OCPP2XConfigVariableIdentifier("DisplayMessageCtrlr", "DisplayMessageSupportedFormats",
                                    "Actual"), "ASCII,URI,UTF8"),
    (OCPP2XConfigVariableIdentifier("DisplayMessageCtrlr", "DisplayMessageSupportedStates", "Actual"),
     "Charging,Faulted,Unavailable")
]))
class TestOcpp201DisplayMessage:
    """
    Tests for OCPP 2.0.1 Display Message
    """

    @staticmethod
    async def start_transaction(test_controller: TestController, test_utility: TestUtility,
                                charge_point: ChargePoint201):
        # prepare data for the test
        evse_id1 = 1
        connector_id = 1

        # make an unknown IdToken
        id_token = IdTokenType(
            id_token="DEADBEEF",
            type=IdTokenTypeEnum.iso14443
        )

        assert await wait_for_and_validate(test_utility, charge_point, "StatusNotification",
                                           call201.StatusNotification(datetime.now().isoformat(),
                                                                      ConnectorStatusEnumType.available,
                                                                      evse_id=evse_id1,
                                                                      connector_id=connector_id),
                                           validate_status_notification_201)

        # Charging station is now available, start charging session.
        # swipe id tag to authorize
        test_controller.swipe(id_token.id_token)
        assert await wait_for_and_validate(test_utility, charge_point, "Authorize",
                                           call201.Authorize(id_token
                                                             ))

        # start charging session
        test_controller.plug_in()

        # should send a Transaction event
        transaction_event = await wait_for_and_validate(test_utility, charge_point, "TransactionEvent",
                                                        {"eventType": "Started"})
        transaction_id = transaction_event['transaction_info']['transaction_id']

        assert await wait_for_and_validate(test_utility, charge_point, "TransactionEvent",
                                           {"eventType": "Updated"})

        return transaction_id

    @staticmethod
    async def await_mock_called(mock):
        while not mock.call_count:
            await asyncio.sleep(0.1)

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceDisplayMessageConfigurationAdjustment())
    async def test_set_display_message(self, central_system: CentralSystem, test_controller: TestController,
                                       test_utility: TestUtility, test_config: OcppTestConfiguration, probe_module):
        probe_module_mock_fn = Mock()
        probe_module_mock_fn.return_value = {
            "status": "Accepted"
        }

        probe_module.implement_command("ProbeModuleDisplayMessage", "set_display_message",
                                       probe_module_mock_fn)
        probe_module.implement_command("ProbeModuleDisplayMessage", "get_display_messages",
                                       probe_module_mock_fn)
        probe_module.implement_command("ProbeModuleDisplayMessage", "clear_display_message",
                                       probe_module_mock_fn)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        start_time = datetime.now(timezone.utc).isoformat()
        end_time = (datetime.now(timezone.utc) +
                    timedelta(minutes=1)).isoformat()

        message = {'id': 1, 'priority': 'NormalCycle',
                   'message': {'format': 'UTF8', 'language': 'en',
                               'content': 'This is a display message'},
                   'startDateTime': start_time,
                   'endDateTime': end_time}

        await chargepoint_with_pm.set_display_message_req(message=message, custom_data=None)

        # Display message should have received a message with the current price information
        data_received = {
            'request': [{'id': 1, 'identifier_type': 'TransactionId',
                         'message': {'content': 'This is a display message', 'format': 'UTF8', 'language': 'en'},
                         'priority': 'NormalCycle', 'timestamp_from': start_time[:-9] + 'Z',
                         'timestamp_to': end_time[:-9] + 'Z'}]
        }

        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "SetDisplayMessage",
                                           call_result201.SetDisplayMessage(
                                               status='Accepted'),
                                           timeout=5)
        probe_module_mock_fn.assert_called_once_with(data_received)

        # Test rejected return value
        probe_module_mock_fn.return_value = {
            "status": "Rejected"
        }

        await chargepoint_with_pm.set_display_message_req(message=message, custom_data=None)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "SetDisplayMessage",
                                           call_result201.SetDisplayMessage(
                                               status='Rejected'),
                                           timeout=5)

        probe_module_mock_fn.return_value = {
            "status": "Accepted"
        }

        # Test unsupported priority
        message['priority'] = 'InFront'

        await chargepoint_with_pm.set_display_message_req(message=message, custom_data=None)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "SetDisplayMessage",
                                           call_result201.SetDisplayMessage(
                                               status='NotSupportedPriority'),
                                           timeout=5)
        message['priority'] = 'NormalCycle'

        # Test unsupported message format
        message['message']['format'] = 'HTML'

        await chargepoint_with_pm.set_display_message_req(message=message, custom_data=None)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "SetDisplayMessage",
                                           call_result201.SetDisplayMessage(
                                               status='NotSupportedMessageFormat'),
                                           timeout=5)
        message['message']['format'] = 'UTF8'

        # Test unsupported state
        message['state'] = 'Idle'

        await chargepoint_with_pm.set_display_message_req(message=message, custom_data=None)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "SetDisplayMessage",
                                           call_result201.SetDisplayMessage(
                                               status='NotSupportedState'),
                                           timeout=5)

        message['state'] = 'Charging'

        # Test unknown transaction
        message['transactionId'] = '12345'

        await chargepoint_with_pm.set_display_message_req(message=message, custom_data=None)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "SetDisplayMessage",
                                           call_result201.SetDisplayMessage(
                                               status='UnknownTransaction'),
                                           timeout=5)

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceDisplayMessageConfigurationAdjustment())
    async def test_set_display_message_with_transaction(self, central_system: CentralSystem,
                                                        test_controller: TestController,
                                                        test_utility: TestUtility, test_config: OcppTestConfiguration,
                                                        probe_module):
        probe_module_mock_fn = Mock()
        probe_module_mock_fn.return_value = {
            "status": "Accepted"
        }

        probe_module.implement_command("ProbeModuleDisplayMessage", "set_display_message",
                                       probe_module_mock_fn)
        probe_module.implement_command("ProbeModuleDisplayMessage", "get_display_messages",
                                       probe_module_mock_fn)
        probe_module.implement_command("ProbeModuleDisplayMessage", "clear_display_message",
                                       probe_module_mock_fn)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        transaction_id = await self.start_transaction(test_controller, test_utility, chargepoint_with_pm)

        message = {'transactionId': transaction_id, 'id': 1, 'priority': 'NormalCycle',
                   'message': {'format': 'UTF8', 'language': 'en',
                               'content': 'This is a display message'}}

        await chargepoint_with_pm.set_display_message_req(message=message, custom_data=None)

        # Display message should have received a message with the current price information
        data_received = {
            'request': [{'id': 1, 'identifier_id': transaction_id, 'identifier_type': 'TransactionId',
                         'message': {'content': 'This is a display message', 'format': 'UTF8', 'language': 'en'},
                         'priority': 'NormalCycle'}]
        }

        probe_module_mock_fn.assert_called_once_with(data_received)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "SetDisplayMessage",
                                           call_result201.SetDisplayMessage(
                                               status='Accepted'),
                                           timeout=5)

        # Test unknown transaction
        message['transactionId'] = '12345'

        await chargepoint_with_pm.set_display_message_req(message=message, custom_data=None)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "SetDisplayMessage",
                                           call_result201.SetDisplayMessage(
                                               status='UnknownTransaction'),
                                           timeout=5)

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceDisplayMessageConfigurationAdjustment())
    async def test_get_display_messages(self, central_system: CentralSystem, test_controller: TestController,
                                        test_utility: TestUtility, test_config: OcppTestConfiguration, probe_module):
        probe_module_mock_fn = Mock()

        probe_module.implement_command("ProbeModuleDisplayMessage", "set_display_message",
                                       probe_module_mock_fn)
        probe_module.implement_command("ProbeModuleDisplayMessage", "get_display_messages",
                                       probe_module_mock_fn)
        probe_module.implement_command("ProbeModuleDisplayMessage", "clear_display_message",
                                       probe_module_mock_fn)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # No messages should return 'unknown'
        probe_module_mock_fn.return_value = {
            "messages": []
        }

        await chargepoint_with_pm.get_display_nessages_req(request_id=1)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "GetDisplayMessage",
                                           call_result201.GetDisplayMessages(
                                               status='Unknown'),
                                           timeout=5)

        # At least one message should return 'accepted'
        probe_module_mock_fn.return_value = {
            "messages": [
                {'id': 1, 'message': {'content': 'This is a display message', 'format': 'UTF8', 'language': 'en'},
                 'priority': 'InFront'}
            ]
        }

        await chargepoint_with_pm.get_display_nessages_req(id=[1], request_id=1)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "GetDisplayMessage",
                                           call_result201.GetDisplayMessages(
                                               status='Accepted'),
                                           timeout=5)

        assert await \
            wait_for_and_validate(test_utility, chargepoint_with_pm, "NotifyDisplayMessages",
                                  call201.NotifyDisplayMessages(request_id=1,
                                                                message_info=[{"id": 1,
                                                                               "message": {
                                                                                   "content": "This is a "
                                                                                   "display message",
                                                                                   "format": "UTF8",
                                                                                   "language": "en"},
                                                                               "priority": "InFront"}]))

        # Return multiple messages
        probe_module_mock_fn.return_value = {
            "messages": [
                {'id': 1, 'message': {'content': 'This is a display message', 'format': 'UTF8', 'language': 'en'},
                 'priority': 'InFront'},
                {'id': 2, 'message': {'content': 'This is a display message 2', 'format': 'UTF8', 'language': 'en'},
                 'priority': 'NormalCycle'}
            ]
        }

        await chargepoint_with_pm.get_display_nessages_req(request_id=1)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "GetDisplayMessage",
                                           call_result201.GetDisplayMessages(
                                               status='Accepted'),
                                           timeout=5)

        assert await \
            wait_for_and_validate(test_utility, chargepoint_with_pm, "NotifyDisplayMessages",
                                  call201.NotifyDisplayMessages(request_id=1,
                                                                message_info=[{"id": 1,
                                                                               "message": {
                                                                                   "content": "This is a "
                                                                                   "display message",
                                                                                   "format": "UTF8",
                                                                                   "language": "en"},
                                                                               "priority": "InFront"}, {"id": 2,
                                                                                                        "message": {
                                                                                                            "content": "This is a "
                                                                                                            "display message 2",
                                                                                                            "format": "UTF8",
                                                                                                            "language": "en"},
                                                                                                        "priority": "NormalCycle"}
                                                                              ]))

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceDisplayMessageConfigurationAdjustment())
    async def test_clear_display_messages(self, central_system: CentralSystem, test_controller: TestController,
                                          test_utility: TestUtility, test_config: OcppTestConfiguration, probe_module):
        probe_module_mock_fn = Mock()

        probe_module.implement_command("ProbeModuleDisplayMessage", "set_display_message",
                                       probe_module_mock_fn)
        probe_module.implement_command("ProbeModuleDisplayMessage", "get_display_messages",
                                       probe_module_mock_fn)
        probe_module.implement_command("ProbeModuleDisplayMessage", "clear_display_message",
                                       probe_module_mock_fn)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Clear display message is accepted
        probe_module_mock_fn.return_value = {
            "status": "Accepted"
        }

        await chargepoint_with_pm.clear_display_message_req(id=1)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "ClearDisplayMessage",
                                           call_result201.ClearDisplayMessage(
                                               status='Accepted'),
                                           timeout=5)

        # Clear display message returns unknown
        probe_module_mock_fn.return_value = {
            "status": "Unknown"
        }

        await chargepoint_with_pm.clear_display_message_req(id=1)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "ClearDisplayMessage",
                                           call_result201.ClearDisplayMessage(
                                               status='Unknown'),
                                           timeout=5)
