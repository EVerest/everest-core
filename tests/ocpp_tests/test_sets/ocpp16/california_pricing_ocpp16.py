# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import asyncio
import logging
import json

from datetime import datetime, timedelta, timezone

from unittest.mock import Mock, ANY


# fmt: off

from validations import (
    validate_standard_start_transaction,
    validate_standard_stop_transaction
)

from everest.testing.ocpp_utils.fixtures import *
from everest.testing.ocpp_utils.charge_point_v16 import ChargePoint16
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility, ValidationMode
from everest.testing.core_utils.controller.test_controller_interface import TestController

from everest_test_utils import *

from everest_test_utils_probe_modules import (probe_module,
                                              ProbeModuleCostAndPriceMetervaluesConfigurationAdjustment,
                                              ProbeModuleCostAndPriceDisplayMessageConfigurationAdjustment,
                                              ProbeModuleCostAndPriceSessionCostConfigurationAdjustment)

# fmt: on

from ocpp.v16.enums import *
from ocpp.v16 import call, call_result


@pytest.mark.asyncio
@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp16-costandprice.yaml'))
class TestOcpp16CostAndPrice:
    """
    Tests for OCPP 1.6 California Pricing Requirements
    """

    # Running cost request data, to be used in tests
    running_cost_data = {
        "transactionId": 1,
        "timestamp": datetime.now(timezone.utc).isoformat(), "meterValue": 1234000,
        "cost": 1.345,
        "state": "Charging",
        "chargingPrice": {
            "kWhPrice": 0.123, "hourPrice": 2.00, "flatFee": 42.42},
        "idlePrice": {"graceMinutes": 30, "hourPrice": 1.00},
        "nextPeriod": {
            "atTime": (datetime.now(timezone.utc) + timedelta(hours=2)).isoformat(),
            "chargingPrice": {
                "kWhPrice": 0.100, "hourPrice": 4.00, "flatFee": 84.84},
            "idlePrice": {"hourPrice": 0.50}
        },
        "triggerMeterValue": {
            "atTime": datetime.now(timezone.utc).isoformat(),
            "atEnergykWh": 5.0,
            "atPowerkW": 8.0,
            "atCPStatus": [ChargePointStatus.finishing, ChargePointStatus.suspended_ev]
        }
    }

    # Final cost request data, to be used in tests.
    final_cost_data = {
        "transactionId": 1,
        "cost": 3.31,
        "priceText": "GBP 2.81 @ 0.12/kWh, GBP 0.50 @ 1/h, TOTAL KWH: 23.4 TIME: 03.50 COST: GBP 3.31. Visit www.cpo.com/invoices/13546 for an invoice of your session.",
        "priceTextExtra": [
            {"format": "UTF8", "language": "nl", "content": "€2.81 @ €0.12/kWh, €0.50 @ €1/h, TOTAL KWH: 23.4 "
                                                            "TIME: 03.50 COST: €3.31. Bezoek www.cpo.com/invoices/13546 "
                                                            "voor een factuur van uw laadsessie."},
            {"format": "UTF8", "language": "de", "content": "€2,81 @ €0,12/kWh, €0,50 @ €1/h, GESAMT-KWH: 23,4 "
                                                            "ZEIT: 03:50 KOSTEN: €3,31. Besuchen Sie "
                                                            "www.cpo.com/invoices/13546 um eine Rechnung für Ihren "
                                                            "Ladevorgang zu erhalten."}],
        "qrCodeText": "https://www.cpo.com/invoices/13546"
    }

    @staticmethod
    async def start_transaction(test_controller: TestController, test_utility: TestUtility, charge_point: ChargePoint16,
                          test_config: OcppTestConfiguration):
        """
        Function to start a transaction during tests.
        """
        # Start transaction
        await charge_point.change_configuration_req(key="MeterValueSampleInterval", value="300")

        # start charging session
        test_controller.plug_in()

        # expect StatusNotification with status preparing
        assert await wait_for_and_validate(test_utility, charge_point, "StatusNotification",
                                           call.StatusNotificationPayload(1, ChargePointErrorCode.no_error,
                                                                          ChargePointStatus.preparing))

        test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

        test_utility.validation_mode = ValidationMode.STRICT

        # expect authorize.req
        assert await wait_for_and_validate(test_utility, charge_point,
                                           "Authorize",
                                           call.AuthorizePayload(test_config.authorization_info.valid_id_tag_1))

        test_utility.validation_mode = ValidationMode.EASY

        # expect StartTransaction.req
        assert await wait_for_and_validate(test_utility, charge_point, "StartTransaction",
                                           call.StartTransactionPayload(
                                               1, test_config.authorization_info.valid_id_tag_1, 0, ""),
                                           validate_standard_start_transaction)

        # expect StatusNotification with status charging
        assert await wait_for_and_validate(test_utility, charge_point, "StatusNotification",
                                           call.StatusNotificationPayload(1, ChargePointErrorCode.no_error,
                                                                          ChargePointStatus.charging))

        test_utility.messages.clear()

    @staticmethod
    async def await_mock_called(mock):
        while not mock.call_count:
            await asyncio.sleep(0.1)

    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceDisplayMessageConfigurationAdjustment())
    @pytest.mark.asyncio
    async def test_cost_and_price_set_user_price_no_transaction(self, test_config: OcppTestConfiguration,
                                                                      test_utility: TestUtility,
                                                                      test_controller: TestController, probe_module,
                                                                      central_system: CentralSystem):
        """
        Test if the datatransfer call returns 'rejected' when session cost is sent while there is no transaction
        running.
        """

        logging.info("######### test_cost_and_price_set_user_price_no_transaction #########")

        data = {
            "idToken": test_config.authorization_info.valid_id_tag_1,
            "priceText": "GBP 0.12/kWh, no idle fee",
            "priceTextExtra": [{"format": "UTF8", "language": "nl",
                                "content": "€0.12/kWh, geen idle fee"},
                               {"format": "UTF8", "language": "de",
                                "content": "€0,12/kWh, keine Leerlaufgebühr"}
                               ]
        }

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

        # wait for libocpp to go online
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        await chargepoint_with_pm.data_transfer_req(vendor_id="org.openchargealliance.costmsg",
                                                    message_id="SetUserPrice",
                                                    data=json.dumps(data))

        # No session running, datatransfer should return 'accepted' and id token is added to the message
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "DataTransfer",
                                           call_result.DataTransferPayload(DataTransferStatus.accepted), timeout=5)

        # Display message should have received a message with the current price information
        data_received = {
            'request': [{'identifier_id': test_config.authorization_info.valid_id_tag_1, 'identifier_type': 'IdToken',
                         'message': {'content': 'GBP 0.12/kWh, no idle fee', 'language': 'en'}},
                        {'identifier_id': test_config.authorization_info.valid_id_tag_1, 'identifier_type': 'IdToken',
                         'message': {'content': '€0.12/kWh, geen idle fee', 'format': 'UTF8', 'language': 'nl'}},
                        {'identifier_id': test_config.authorization_info.valid_id_tag_1, 'identifier_type': 'IdToken',
                         'message': {'content': '€0,12/kWh, keine Leerlaufgebühr', 'format': 'UTF8', 'language': 'de'}}]
        }

        probe_module_mock_fn.assert_called_once_with(data_received)

    @pytest.mark.asyncio
    async def test_cost_and_price_set_user_price_no_transaction_no_id_token(self, test_config: OcppTestConfiguration,
                                                                      charge_point_v16: ChargePoint16,
                                                                      test_utility: TestUtility):
        """
        Test if the datatransfer call returns 'rejected' when session cost is sent while there is no transaction
        running.
        """

        logging.info("######### test_cost_and_price_set_user_price_no_transaction_no_id_token #########")

        data = {
            "priceText": "GBP 0.12/kWh, no idle fee",
            "priceTextExtra": [{"format": "UTF8", "language": "nl",
                                "content": "€0.12/kWh, geen idle fee"},
                               {"format": "UTF8", "language": "de",
                                "content": "€0,12/kWh, keine Leerlaufgebühr"}
                               ]
        }

        await charge_point_v16.data_transfer_req(vendor_id="org.openchargealliance.costmsg", message_id="SetUserPrice",
                                                 data=json.dumps(data))

        # No session running, and no id token, datatransfer should return 'rejected'
        assert await wait_for_and_validate(test_utility, charge_point_v16, "DataTransfer",
                                           call_result.DataTransferPayload(DataTransferStatus.rejected), timeout=5)

    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceDisplayMessageConfigurationAdjustment())
    @pytest.mark.asyncio
    async def test_cost_and_price_set_user_price_with_transaction(self, test_config: OcppTestConfiguration,
                                                                        test_utility: TestUtility,
                                                                        test_controller: TestController, probe_module,
                                                                        central_system: CentralSystem):
        """
        Test if user price is sent correctly when there is a transaction.
        """

        logging.info("######### test_cost_and_price_set_user_price_with_transaction #########")

        data = {
            "idToken": test_config.authorization_info.valid_id_tag_1,
            "priceText": "GBP 0.12/kWh, no idle fee",
            "priceTextExtra": [{"format": "UTF8", "language": "nl",
                                "content": "€0.12/kWh, geen idle fee"},
                               {"format": "UTF8", "language": "de",
                                "content": "€0,12/kWh, keine Leerlaufgebühr"}
                               ]
        }

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

        # wait for libocpp to go online
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Start transaction
        await self.start_transaction(test_controller, test_utility, chargepoint_with_pm, test_config)

        # Send 'set user price', which is tight to a transaction.
        await chargepoint_with_pm.data_transfer_req(vendor_id="org.openchargealliance.costmsg",
                                                    message_id="SetUserPrice",
                                                    data=json.dumps(data))

        # Datatransfer should be successful.
        success = await wait_for_and_validate(test_utility, chargepoint_with_pm, "DataTransfer",
                                              call_result.DataTransferPayload(DataTransferStatus.accepted), timeout=5)

        # Display message should have received a message with the current price information
        data_received = {
            'request': [{'identifier_id': ANY, 'identifier_type': 'SessionId',
                         'message': {'content': 'GBP 0.12/kWh, no idle fee', 'language': 'en'}},
                        {'identifier_id': ANY, 'identifier_type': 'SessionId',
                         'message': {'content': '€0.12/kWh, geen idle fee', 'format': 'UTF8', 'language': 'nl'}},
                        {'identifier_id': ANY, 'identifier_type': 'SessionId',
                         'message': {'content': '€0,12/kWh, keine Leerlaufgebühr', 'format': 'UTF8', 'language': 'de'}}]
        }

        probe_module_mock_fn.assert_called_once_with(data_received)

        assert success

    @pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp16-costandprice.yaml'))
    @pytest.mark.asyncio
    async def test_cost_and_price_final_cost_no_transaction(self, test_config: OcppTestConfiguration,
                                                                  charge_point_v16: ChargePoint16,
                                                                  test_utility: TestUtility):
        """
        Test sending of final price when there is no transaction: DataTransfer should return rejected.
        """
        logging.info("######### test_cost_and_price_final_cost_no_transaction #########")

        await charge_point_v16.data_transfer_req(vendor_id="org.openchargealliance.costmsg", message_id="FinalCost",
                                                 data=json.dumps(self.final_cost_data))

        # Since there is no transaction, datatransfer should return 'rejected' here.
        success = await wait_for_and_validate(test_utility, charge_point_v16, "DataTransfer",
                                              call_result.DataTransferPayload(DataTransferStatus.rejected), timeout=5)
        assert success

    @pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp16-costandprice.yaml'))
    @pytest.mark.asyncio
    async def test_cost_and_price_final_cost_with_transaction_not_found(self, test_config: OcppTestConfiguration,
                                                                              charge_point_v16: ChargePoint16,
                                                                              test_utility: TestUtility,
                                                                              test_controller: TestController):
        """
        A transaction is running when a final cost message is sent, but the transaction is not found. This should
        return a 'rejected' response on the DataTransfer message.
        """
        logging.info("######### test_cost_and_price_final_cost_with_transaction_not_found #########")

        # Start transaction
        await self.start_transaction(test_controller, test_utility, charge_point_v16, test_config)

        data = self.final_cost_data.copy()
        # Set a non existing transaction id
        data["transactionId"] = 98765

        await charge_point_v16.data_transfer_req(vendor_id="org.openchargealliance.costmsg", message_id="FinalCost",
                                                 data=json.dumps(data))

        #Transaction does not exist: 'rejected' must be returned.
        success = await wait_for_and_validate(test_utility, charge_point_v16, "DataTransfer",
                                              call_result.DataTransferPayload(DataTransferStatus.rejected), timeout=5)
        assert success

    @pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp16-costandprice.yaml'))
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceSessionCostConfigurationAdjustment())
    @pytest.mark.asyncio
    async def test_cost_and_price_final_cost_with_transaction(self, test_config: OcppTestConfiguration,
                                                                    test_utility: TestUtility,
                                                                    test_controller: TestController, probe_module,
                                                                    central_system: CentralSystem):
        """
        A transaction is running whan a final cost message for that transaction is sent. A session cost message
        should be sent now.
        """
        logging.info("######### test_cost_and_price_final_cost_with_transaction #########")

        session_cost_mock = Mock()

        probe_module.subscribe_variable("session_cost", "session_cost", session_cost_mock)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        # wait for libocpp to go online
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Start transaction
        await self.start_transaction(test_controller, test_utility, chargepoint_with_pm, test_config)

        # Send final cost message.
        await chargepoint_with_pm.data_transfer_req(vendor_id="org.openchargealliance.costmsg", message_id="FinalCost",
                                                 data=json.dumps(self.final_cost_data))

        # Which is accepted
        success = await wait_for_and_validate(test_utility, chargepoint_with_pm, "DataTransfer",
                                              call_result.DataTransferPayload(DataTransferStatus.accepted), timeout=5)

        received_data = {'cost_chunks': [{'cost': {'value': 33100}}], 'currency': {'decimals': 4}, 'message': [{
            'content': 'GBP 2.81 @ 0.12/kWh, GBP 0.50 @ 1/h, TOTAL KWH: 23.4 TIME: 03.50 COST: GBP 3.31. '
                       'Visit www.cpo.com/invoices/13546 for an invoice of your session.'},
            {
                'content': '€2.81 @ €0.12/kWh, €0.50 @ €1/h, TOTAL KWH: 23.4 TIME: 03.50 COST: €3.31. '
                           'Bezoek www.cpo.com/invoices/13546 voor een factuur van uw laadsessie.',
                'format': 'UTF8',
                'language': 'nl'},
            {
                'content': '€2,81 @ €0,12/kWh, €0,50 @ €1/h, GESAMT-KWH: 23,4 ZEIT: 03:50 KOSTEN: €3,31. '
                           'Besuchen Sie www.cpo.com/invoices/13546 um eine Rechnung für Ihren Ladevorgang zu erhalten.',
                'format': 'UTF8',
                'language': 'de'}],
                         'qr_code': 'https://www.cpo.com/invoices/13546', 'session_id': ANY, 'status': 'Finished'}

        # A session cost message should have been received
        await self.await_mock_called(session_cost_mock)

        assert session_cost_mock.call_count == 1

        # And it should contain the correct data
        session_cost_mock.assert_called_once_with(received_data)

        assert success

    @pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp16-costandprice.yaml'))
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceSessionCostConfigurationAdjustment())
    @pytest.mark.asyncio
    async def test_cost_and_price_running_cost(self, test_config: OcppTestConfiguration,
                                                     test_controller: TestController,
                                                     test_utility: TestUtility, probe_module,
                                                     central_system: CentralSystem):
        """
        A transaction is started and a 'running cost' message with the transaction id is sent. This should send a
        session cost message over the interface.
        """
        logging.info("######### test_cost_and_price_running_cost #########")

        session_cost_mock = Mock()
        probe_module.subscribe_variable("session_cost", "session_cost", session_cost_mock)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        # wait for libocpp to go online
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Start transaction
        await self.start_transaction(test_controller, test_utility, chargepoint_with_pm, test_config)

        test_utility.messages.clear()

        # Send running cost message.
        assert await chargepoint_with_pm.data_transfer_req(vendor_id="org.openchargealliance.costmsg",
                                                           message_id="RunningCost",
                                                           data=json.dumps(self.running_cost_data))

        # Since there is a transaction running and the correct transaction id is sent in the running cost request,
        # the datatransfer message is accepted.
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "DataTransfer",
                                           call_result.DataTransferPayload(DataTransferStatus.accepted))

        # A session cost call should have been sent now with the correct data.
        received_data = {
            'charging_price': [{'category': 'Time', 'price': {'currency': {'decimals': 4}, 'value': {'value': 20000}}},
                               {'category': 'Energy', 'price': {'currency': {'decimals': 4}, 'value': {'value': 1230}}},
                               {'category': 'FlatFee',
                                'price': {'currency': {'decimals': 4}, 'value': {'value': 424200}}}],
            'cost_chunks': [
                {'cost': {'value': 13450}, 'metervalue_to': 1234000, 'timestamp_to': ANY}],
            'currency': {'decimals': 4},
            'idle_price': {'grace_minutes': 30, 'hour_price': {'currency': {'decimals': 4}, 'value': {'value': 10000}}},
            'next_period': {
                'charging_price': [{'category': 'Time',
                                    'price': {'currency': {'decimals': 4}, 'value': {'value': 40000}}},
                                   {'category': 'Energy',
                                    'price': {'currency': {'decimals': 4}, 'value': {'value': 1000}}},
                                   {'category': 'FlatFee',
                                    'price': {'currency': {'decimals': 4}, 'value': {'value': 848400}}}],
                'idle_price': {'hour_price': {'currency': {'decimals': 4}, 'value': {'value': 5000}}},
                'timestamp_from': ANY},
            'session_id': ANY, 'status': 'Running'}

        await self.await_mock_called(session_cost_mock)

        assert session_cost_mock.call_count == 1

        session_cost_mock.assert_called_once_with(received_data)

    @pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp16-costandprice.yaml'))
    @pytest.mark.asyncio
    async def test_cost_and_price_running_cost_wrong_transaction(self, test_config: OcppTestConfiguration,
                                                                       test_controller: TestController,
                                                                       test_utility: TestUtility,
                                                                       charge_point_v16: ChargePoint16):
        """
        A transaction is started and a running cost message is sent, but the transaction id is not known so the message
        is rejected.
        """
        logging.info("######### test_cost_and_price_running_cost_wrong_transaction #########")

        # Start transaction
        await self.start_transaction(test_controller, test_utility, charge_point_v16, test_config)

        data = self.running_cost_data.copy()
        # Set non existing transaction id.
        data["transactionId"] = 42

        # Send running cost message with incorrect transaction id.
        assert await charge_point_v16.data_transfer_req(vendor_id="org.openchargealliance.costmsg",
                                                           message_id="RunningCost",
                                                           data=json.dumps(data))

        # DataTransfer should return 'rejected' because the transaction is not found.
        assert await wait_for_and_validate(test_utility, charge_point_v16, "DataTransfer",
                                           call_result.DataTransferPayload(DataTransferStatus.rejected), timeout=15)

    @pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp16-costandprice.yaml'))
    @pytest.mark.asyncio
    async def test_cost_and_price_running_cost_no_transaction(self, test_config: OcppTestConfiguration,
                                                                    test_utility: TestUtility,
                                                                    charge_point_v16: ChargePoint16):
        """
        There is no transaction but there is a running cost message sent. This should return a 'rejected' on the
        DataTransfer request.
        """
        logging.info("######### test_cost_and_price_running_cost_no_transaction #########")

        test_utility.messages.clear()

        data = {
            "transactionId": 1,
            "timestamp": datetime.now(timezone.utc).isoformat(), "meterValue": 1234000,
            "cost": 1.345,
            "state": "Charging",
            "chargingPrice": {
                "kWhPrice": 0.123, "hourPrice": 0.00, "flatFee": 0.00},
            "idlePrice": {"graceMinutes": 30, "hourPrice": 1.00},
            "nextPeriod": {
                "atTime": (datetime.now(timezone.utc) + timedelta(hours=2)).isoformat(),
                "chargingPrice": {
                    "kWhPrice": 0.100, "hourPrice": 0.00, "flatFee": 0.00},
                "idlePrice": {"hourPrice": 0.00}
            },
            "triggerMeterValue": {
                "atTime": (datetime.now(timezone.utc) + timedelta(seconds=3)).isoformat(),
                "atEnergykWh": 5.0,
                "atPowerkW": 8.0,
                "atCPStatus": [ChargePointStatus.finishing, ChargePointStatus.available]
            }
        }

        # Send RunningCost message while there is no transaction.
        assert await charge_point_v16.data_transfer_req(vendor_id="org.openchargealliance.costmsg",
                                                        message_id="RunningCost",
                                                        data=json.dumps(data))

        # This should return 'Rejected'
        assert await wait_for_and_validate(test_utility, charge_point_v16, "DataTransfer",
                                           call_result.DataTransferPayload(DataTransferStatus.rejected), timeout=15)

    @pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp16-costandprice.yaml'))
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceMetervaluesConfigurationAdjustment(
        evse_manager_ids=["evse_manager"]))
    @pytest.mark.asyncio
    async def test_cost_and_price_running_cost_trigger_time(self, test_config: OcppTestConfiguration,
                                                                  test_controller: TestController,
                                                                  test_utility: TestUtility, probe_module,
                                                                  central_system: CentralSystem):
        """
        Send running cost with a trigger time to return meter values.
        """
        logging.info("######### test_cost_and_price_running_cost_trigger_time #########")

        probe_module_mock_fn = Mock()
        probe_module_mock_fn.return_value = {
            "status": "OK"
        }

        probe_module.implement_command("ProbeModulePowerMeter", "start_transaction", probe_module_mock_fn)
        probe_module.implement_command("ProbeModulePowerMeter", "stop_transaction", probe_module_mock_fn)

        power_meter_value = {
            "timestamp": datetime.now(timezone.utc).isoformat(),
            "energy_Wh_import": {
                "total": 1.0
            },
            "power_W": {
                "total": 1000.0
            }
        }

        probe_module.start()
        await probe_module.wait_to_be_ready()

        # wait for libocpp to go online
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Start transaction
        await self.start_transaction(test_controller, test_utility, chargepoint_with_pm, test_config)

        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        test_utility.messages.clear()

        # Metervalues should be sent at below trigger time.
        data = self.running_cost_data.copy()
        data["triggerMeterValue"]["atTime"] = (datetime.now(timezone.utc) + timedelta(seconds=3)).isoformat()

        # While the transaction is started, send a 'RunningCost' message.
        assert await chargepoint_with_pm.data_transfer_req(vendor_id="org.openchargealliance.costmsg",
                                                           message_id="RunningCost",
                                                           data=json.dumps(data))

        # Which is accepted.
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "DataTransfer",
                                           call_result.DataTransferPayload(DataTransferStatus.accepted))

        # At the given time, metervalues must have been sent.
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "MeterValues",
                                           call.MeterValuesPayload(1, meter_value=[{'sampledValue': [
                                               {'context': 'Other', 'format': 'Raw', 'location': 'Outlet',
                                                'measurand': 'Energy.Active.Import.Register', 'unit': 'Wh',
                                                'value': '1.00'}], 'timestamp': timestamp[:-9] + 'Z'}],
                                                                   transaction_id=1), timeout=15)

    @pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp16-costandprice.yaml'))
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceMetervaluesConfigurationAdjustment(
        evse_manager_ids=["evse_manager"]
    ))
    @pytest.mark.asyncio
    async def test_cost_and_price_running_cost_trigger_energy(self, test_config: OcppTestConfiguration,
                                                                     test_controller: TestController,
                                                                     test_utility: TestUtility, probe_module,
                                                                     central_system: CentralSystem):
        """
        Send running cost with a trigger kwh value to return meter values.
        """
        logging.info("######### test_cost_and_price_running_cost_trigger_energy #########")

        probe_module_mock_fn = Mock()
        probe_module_mock_fn.return_value = {
            "status": "OK"
        }

        probe_module.implement_command("ProbeModulePowerMeter", "start_transaction", probe_module_mock_fn)
        probe_module.implement_command("ProbeModulePowerMeter", "stop_transaction", probe_module_mock_fn)

        power_meter_value = {
            "timestamp": datetime.now(timezone.utc).isoformat(),
            "energy_Wh_import": {
                "total": 1.0
            }
        }

        probe_module.start()
        await probe_module.wait_to_be_ready()

        # wait for libocpp to go online
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Start transaction
        await self.start_transaction(test_controller, test_utility, chargepoint_with_pm, test_config)

        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        test_utility.messages.clear()

        # Send running cost, which has a trigger specified on atEnergykWh = 5.0
        assert await chargepoint_with_pm.data_transfer_req(vendor_id="org.openchargealliance.costmsg",
                                                           message_id="RunningCost",
                                                           data=json.dumps(self.running_cost_data))

        # Datatransfer is valid and should be accepted.
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "DataTransfer",
                                           call_result.DataTransferPayload(DataTransferStatus.accepted))

        # Now increase power meter value so it is above the specified trigger and publish the powermeter value
        power_meter_value["energy_Wh_import"]["total"] = 6000.0
        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        # Metervalues should now be sent
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "MeterValues",
                                           call.MeterValuesPayload(1, meter_value=[{'sampledValue': [
                                               {'context': 'Other', 'format': 'Raw', 'location': 'Outlet',
                                                'measurand': 'Energy.Active.Import.Register', 'unit': 'Wh',
                                                'value': '6000.00'}], 'timestamp': timestamp[:-9] + 'Z'}],
                                                                   transaction_id=1))

    @pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp16-costandprice.yaml'))
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceMetervaluesConfigurationAdjustment(
        evse_manager_ids=["evse_manager"]
    ))
    @pytest.mark.asyncio
    async def test_cost_and_price_running_cost_trigger_power(self, test_config: OcppTestConfiguration,
                                                                    test_controller: TestController,
                                                                    test_utility: TestUtility, probe_module,
                                                                    central_system: CentralSystem):
        """
        Send running cost with a trigger kw value to return meter values.
        """
        logging.info("######### test_cost_and_price_running_cost_trigger_power #########")

        probe_module_mock_fn = Mock()
        probe_module_mock_fn.return_value = {
            "status": "OK"
        }

        probe_module.implement_command("ProbeModulePowerMeter", "start_transaction", probe_module_mock_fn)
        probe_module.implement_command("ProbeModulePowerMeter", "stop_transaction", probe_module_mock_fn)

        power_meter_value = {
            "timestamp": datetime.now(timezone.utc).isoformat(),
            "energy_Wh_import": {
                "total": 1.0
            },
            "power_W": {
                "total": 1000.0
            }
        }

        probe_module.start()
        await probe_module.wait_to_be_ready()

        # wait for libocpp to go online
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Start transaction
        await self.start_transaction(test_controller, test_utility, chargepoint_with_pm, test_config)

        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        test_utility.messages.clear()

        # Send running cost data with a trigger specified of 8 kW
        assert await chargepoint_with_pm.data_transfer_req(vendor_id="org.openchargealliance.costmsg",
                                                           message_id="RunningCost",
                                                           data=json.dumps(self.running_cost_data))

        # DataTransfer message is valid, expect it's accepted.
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "DataTransfer",
                                           call_result.DataTransferPayload(DataTransferStatus.accepted))

        # Set W above the trigger value and publish a new powermeter value.
        power_meter_value["energy_Wh_import"]["total"] = 1.0
        power_meter_value["power_W"]["total"] = 10000.0
        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        # Powermeter value should be sent because of the trigger.
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "MeterValues",
                                           call.MeterValuesPayload(1, meter_value=[{'sampledValue': [
                                               {'context': 'Other', 'format': 'Raw', 'location': 'Outlet',
                                                'measurand': 'Energy.Active.Import.Register', 'unit': 'Wh',
                                                'value': '1.00'},
                                               {'context': 'Other', 'format': 'Raw', 'location': 'Outlet',
                                                'measurand': 'Power.Active.Import', 'unit': 'W',
                                                'value': '10000.00'}], 'timestamp': timestamp[:-9] + 'Z'}],
                                                                   transaction_id=1))

        # W value is below trigger, but hysteresis prevents sending the metervalue.
        power_meter_value["energy_Wh_import"]["total"] = 8000.0
        power_meter_value["power_W"]["total"] = 7990.0
        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        # So no metervalue is sent.
        assert not await wait_for_and_validate(test_utility, chargepoint_with_pm, "MeterValues",
                                               call.MeterValuesPayload(1, meter_value=[{'sampledValue': [
                                                   {'context': 'Other', 'format': 'Raw', 'location': 'Outlet',
                                                    'measurand': 'Energy.Active.Import.Register', 'unit': 'Wh',
                                                    'value': '8000.00'},
                                                   {'context': 'Other', 'format': 'Raw', 'location': 'Outlet',
                                                    'measurand': 'Power.Active.Import', 'unit': 'W',
                                                    'value': '7990.00'}], 'timestamp': timestamp[:-9] + 'Z'}],
                                                                       transaction_id=1))

        # Only when trigger is high ( / low) enough, metervalue will be sent.
        power_meter_value["energy_Wh_import"]["total"] = 9500.0
        power_meter_value["power_W"]["total"] = 7200.0
        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "MeterValues",
                                           call.MeterValuesPayload(1, meter_value=[{'sampledValue': [
                                               {'context': 'Other', 'format': 'Raw', 'location': 'Outlet',
                                                'measurand': 'Energy.Active.Import.Register', 'unit': 'Wh',
                                                'value': '9500.00'},
                                               {'context': 'Other', 'format': 'Raw', 'location': 'Outlet',
                                                'measurand': 'Power.Active.Import', 'unit': 'W',
                                                'value': '7200.00'}], 'timestamp': timestamp[:-9] + 'Z'}],
                                                                   transaction_id=1))

    @pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp16-costandprice.yaml'))
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceMetervaluesConfigurationAdjustment(
        evse_manager_ids=["evse_manager"]
    ))
    @pytest.mark.asyncio
    async def test_cost_and_price_running_cost_trigger_cp_status(self, test_config: OcppTestConfiguration,
                                                                        test_controller: TestController,
                                                                        test_utility: TestUtility, probe_module,
                                                                        central_system: CentralSystem):
        """
        Send running cost with a trigger chargepoint status to return meter values.
        """
        logging.info("######### test_cost_and_price_running_cost_trigger_cp_status #########")

        probe_module_mock_fn = Mock()
        probe_module_mock_fn.return_value = {
            "status": "OK"
        }

        probe_module.implement_command("ProbeModulePowerMeter", "start_transaction", probe_module_mock_fn)
        probe_module.implement_command("ProbeModulePowerMeter", "stop_transaction", probe_module_mock_fn)

        power_meter_value = {
            "timestamp": datetime.now(timezone.utc).isoformat(),
            "energy_Wh_import": {
                "total": 1.0
            },
            "power_W": {
                "total": 1000.0
            }
        }

        probe_module.start()
        await probe_module.wait_to_be_ready()

        # wait for libocpp to go online
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Start transaction
        await self.start_transaction(test_controller, test_utility, chargepoint_with_pm, test_config)

        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        test_utility.messages.clear()

        # Send data with cp status finishing and suspended ev as triggers.
        data = {
            "transactionId": 1,
            "timestamp": datetime.now(timezone.utc).isoformat(), "meterValue": 1234000,
            "cost": 1.345,
            "state": "Charging",
            "chargingPrice": {
                "kWhPrice": 0.123, "hourPrice": 0.00, "flatFee": 0.00},
            "idlePrice": {"graceMinutes": 30, "hourPrice": 1.00},
            "nextPeriod": {
                "atTime": (datetime.now(timezone.utc) + timedelta(hours=2)).isoformat(),
                "chargingPrice": {
                    "kWhPrice": 0.100, "hourPrice": 0.00, "flatFee": 0.00},
                "idlePrice": {"hourPrice": 0.00}
            },
            "triggerMeterValue": {
                "atCPStatus": [ChargePointStatus.finishing, ChargePointStatus.suspended_ev]
            }
        }

        assert await chargepoint_with_pm.data_transfer_req(vendor_id="org.openchargealliance.costmsg",
                                                           message_id="RunningCost",
                                                           data=json.dumps(data))

        # And wait for the datatransfer to be accepted.
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "DataTransfer",
                                           call_result.DataTransferPayload(DataTransferStatus.accepted), timeout=15)

        # swipe id tag to finish transaction
        test_controller.swipe(test_config.authorization_info.valid_id_tag_1)

        # expect StopTransaction.req
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "StopTransaction",
                                           call.StopTransactionPayload(0, "", 1, Reason.local),
                                           validate_standard_stop_transaction)

        # expect StatusNotification with status finishing
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "StatusNotification",
                                           call.StatusNotificationPayload(1, ChargePointErrorCode.no_error,
                                                                          ChargePointStatus.finishing))

        # As the chargepoint status is now 'finishing' new metervalues should be sent.
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "MeterValues",
                                           call.MeterValuesPayload(1, meter_value=[{'sampledValue': [
                                               {'context': 'Other', 'format': 'Raw', 'location': 'Outlet',
                                                'measurand': 'Energy.Active.Import.Register', 'unit': 'Wh',
                                                'value': '1.00'}], 'timestamp': timestamp[:-9] + 'Z'}]))

        test_controller.plug_out()

        # # expect StatusNotification.req with status available
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "StatusNotification",
                                           call.StatusNotificationPayload(1, ChargePointErrorCode.no_error,
                                                                          ChargePointStatus.available))

    @pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp16-costandprice.yaml'))
    @pytest.mark.asyncio
    async def test_cost_and_price_set_price_text(self, test_config: OcppTestConfiguration,
                                                        charge_point_v16: ChargePoint16,
                                                        test_utility: TestUtility):
        """
        Test 'DefaultPriceText' configuration setting.
        """
        logging.info("######### test_cost_and_price_set_price_text #########")

        test_utility.validation_mode = ValidationMode.STRICT

        # First get price text for specific language.
        response = await charge_point_v16.get_configuration_req(key=['DefaultPriceText,de'])
        assert response.configuration_key[0]['key'] == 'DefaultPriceText,de'
        assert response.configuration_key[0]['value'] == ''

        # Set price text for specific language.
        price_text = {
            "priceText": "€0.15 / kWh, Leerlaufgebühr nach dem Aufladen: 1 $/hr",
            "priceTextOffline": "Die Station ist offline. Laden ist für €0,15/kWh möglich"
        }

        response = await charge_point_v16.change_configuration_req(key="DefaultPriceText,de", value=json.dumps(price_text))
        assert response.status == "Accepted"

        # Get price text for specific language to check if it is set.
        response = await charge_point_v16.get_configuration_req(key=['DefaultPriceText,de'])

        assert response.configuration_key[0]['key'] == 'DefaultPriceText,de'
        assert json.loads(response.configuration_key[0]['value']) == price_text

        # Set price text for not supported language.
        price_text = {
            "priceText": "0,15 € / kWh, frais d'inactivité après recharge : 1 $/h"
        }
        response = await charge_point_v16.change_configuration_req(key="DefaultPriceText,fr", value=json.dumps(price_text))
        assert response.status == "Rejected"

        # Get price text for specific language to check if it is set.
        response = await charge_point_v16.get_configuration_req(key=['DefaultPriceText,fr'])

        assert response.configuration_key[0]['key'] == 'DefaultPriceText,fr'
        assert response.configuration_key[0]['value'] == ''

    @pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp16-costandprice.yaml'))
    @pytest.mark.asyncio
    async def test_cost_and_price_set_charging_price(self, test_config: OcppTestConfiguration,
                                                            charge_point_v16: ChargePoint16,
                                                            test_utility: TestUtility):
        """
        Test 'DefaultPrice' configuration setting.
        """
        logging.info("######### test_cost_and_price_set_charging_price #########")

        test_utility.validation_mode = ValidationMode.STRICT

        # First get price text for specific language.
        response = await charge_point_v16.get_configuration_req(key=['DefaultPrice'])
        assert response.configuration_key[0]['key'] == 'DefaultPrice'
        assert response.configuration_key[0]['value']

        # Set price text for specific language.
        default_price = {
            "priceText": "0.15 $/kWh, idle fee after charging: 1 $/hr",
            "priceTextOffline": "The station is offline. Charging is possible for 0.15 $/kWh.",
            "chargingPrice": {"kWhPrice": 0.15, "hourPrice": 0.00, "flatFee": 0.00}
        }

        await charge_point_v16.change_configuration_req(key="DefaultPrice", value=json.dumps(default_price))

        # Get price text for specific language to check if it is set.
        response = await charge_point_v16.get_configuration_req(key=['DefaultPrice'])

        assert response.configuration_key[0]['key'] == 'DefaultPrice'
        assert json.loads(response.configuration_key[0]['value']) == default_price
