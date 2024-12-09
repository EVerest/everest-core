from datetime import timezone
from unittest.mock import Mock, ANY

import logging
from copy import deepcopy

from everest_test_utils import *  # Needs to be before the datatypes below since it overrides the v201 Action enum with the v16 one
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
from everest.testing.core_utils.controller.test_controller_interface import TestController

from ocpp.v201 import call as call201
from ocpp.v201 import call_result as call_result201
from ocpp.v201.enums import (IdTokenType as IdTokenTypeEnum, ConnectorStatusType,
                             ClearCacheStatusType)
from ocpp.v201.datatypes import *
from everest.testing.ocpp_utils.fixtures import *
from everest_test_utils_probe_modules import (probe_module, chargepoint_with_pm,
                                              ProbeModuleCostAndPriceMetervaluesConfigurationAdjustment,
                                              ProbeModuleCostAndPriceDisplayMessageConfigurationAdjustment,
                                              ProbeModuleCostAndPriceSessionCostConfigurationAdjustment)

from everest.testing.core_utils._configuration.libocpp_configuration_helper import (
    GenericOCPP201ConfigAdjustment,
    OCPP201ConfigVariableIdentifier,
)

from validations import validate_status_notification_201

log = logging.getLogger("ocpp201CaliforniaPricingTest")


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.inject_csms_mock
@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp201-costandprice.yaml'))
@pytest.mark.ocpp_config_adaptions(GenericOCPP201ConfigAdjustment([
    (OCPP201ConfigVariableIdentifier("DisplayMessageCtrlr", "DisplayMessageCtrlrAvailable", "Actual"),
     "true"),
    (OCPP201ConfigVariableIdentifier("DisplayMessageCtrlr", "QRCodeDisplayCapable",
                                                                    "Actual"), "true"),
    (OCPP201ConfigVariableIdentifier("DisplayMessageCtrlr", "DisplayMessageLanguage", "Actual"),
     "en"),
    (OCPP201ConfigVariableIdentifier("TariffCostCtrlr", "TariffCostCtrlrAvailableTariff", "Actual"),
     "true"),
    (OCPP201ConfigVariableIdentifier("TariffCostCtrlr", "TariffCostCtrlrAvailableCost", "Actual"),
     "true"),
    (OCPP201ConfigVariableIdentifier("TariffCostCtrlr", "TariffCostCtrlrEnabledTariff", "Actual"), "true"),
    (OCPP201ConfigVariableIdentifier("TariffCostCtrlr", "TariffCostCtrlrEnabledCost", "Actual"), "true"),
    (OCPP201ConfigVariableIdentifier("TariffCostCtrlr", "NumberOfDecimalsForCostValues", "Actual"),
     "5"),
    (OCPP201ConfigVariableIdentifier("OCPPCommCtrlr", "MessageTimeout", "Actual"),
     "1"),
    (OCPP201ConfigVariableIdentifier("OCPPCommCtrlr", "MessageAttemptInterval",
                                                                    "Actual"), "1"),
    (OCPP201ConfigVariableIdentifier("OCPPCommCtrlr", "MessageAttempts", "Actual"),
     "3"),
    (OCPP201ConfigVariableIdentifier("AuthCacheCtrlr", "AuthCacheCtrlrEnabled", "Actual"),
     "true"),
    (OCPP201ConfigVariableIdentifier("AuthCtrlr", "LocalPreAuthorize",
                                                                    "Actual"), "true"),
    (OCPP201ConfigVariableIdentifier("AuthCacheCtrlr", "AuthCacheLifeTime", "Actual"),
     "86400"),
    (OCPP201ConfigVariableIdentifier("CustomizationCtrlr", "CustomImplementationCaliforniaPricingEnabled",
                                                                    "Actual"), "true"),
    (OCPP201ConfigVariableIdentifier("CustomizationCtrlr", "CustomImplementationMultiLanguageEnabled",
                                         "Actual"), "true")
]))
class TestOcpp201CostAndPrice:
    """
    Tests for OCPP 2.0.1 California Pricing Requirements
    """

    cost_updated_custom_data = {
        "vendorId": "org.openchargealliance.costmsg",
        "timestamp": datetime.now(timezone.utc).isoformat(), "meterValue": 1234000,
        "state": "Charging",
        "chargingPrice": {"kWhPrice": 0.123, "hourPrice": 2.00, "flatFee": 42.42},
        "idlePrice": {"graceMinutes": 30, "hourPrice": 1.00},
        "nextPeriod": {
            "atTime": (datetime.now(timezone.utc) + timedelta(hours=2)).isoformat(),
            "chargingPrice": {"kWhPrice": 0.100, "hourPrice": 4.00, "flatFee": 84.84},
            "idlePrice": {"hourPrice": 0.50}
        },
        "triggerMeterValue": {
            "atTime": datetime.now(timezone.utc).isoformat(),
            "atEnergykWh": 5.0,
            "atPowerkW": 8.0
        }
    }

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
                                           call201.StatusNotificationPayload(datetime.now().isoformat(),
                                                                             ConnectorStatusType.available,
                                                                             evse_id=evse_id1,
                                                                             connector_id=connector_id),
                                           validate_status_notification_201)

        # Charging station is now available, start charging session.
        # swipe id tag to authorize
        test_controller.swipe(id_token.id_token)
        assert await wait_for_and_validate(test_utility, charge_point, "Authorize",
                                           call201.AuthorizePayload(id_token
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
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceSessionCostConfigurationAdjustment())
    async def test_set_running_cost(self, central_system: CentralSystem, test_controller: TestController,
                                    test_utility: TestUtility, test_config: OcppTestConfiguration, probe_module):
        """
        Test running and final cost, that is 'embedded' in the TransactionEventResponse.
        """
        # prepare data for the test
        transaction_event_response_started = call_result201.TransactionEventPayload()

        transaction_event_response = call_result201.TransactionEventPayload()
        transaction_event_response.total_cost = 3.13  # According to the OCPP spec this should be a floating point number but the test framework does not allow that.
        transaction_event_response.updated_personal_message = {"format": "UTF8", "language": "en",
                                                               "content": "$2.81 @ $0.12/kWh, $0.50 @ $1/h, TOTAL KWH: 23.4 TIME: 03.50 COST: $3.31. Visit www.cpo.com/invoices/13546 for an invoice of your session."}
        transaction_event_response.custom_data = {"vendorId": "org.openchargealliance.org.qrcode",
                                                  "qrCodeText": "https://www.cpo.com/invoices/13546"}

        transaction_event_response_ended = deepcopy(transaction_event_response)
        transaction_event_response_ended.total_cost = 55.1

        received_data = {'cost_chunks': [{'cost': {'value': 313000}, 'timestamp_to': ANY}],
                         'currency': {'code': 'EUR', 'decimals': 5}, 'message': [{
                'content': '$2.81 @ $0.12/kWh, $0.50 @ $1/h, TOTAL KWH: 23.4 TIME: 03.50 COST: $3.31. '
                           'Visit www.cpo.com/invoices/13546 for an invoice of your session.',
                'format': 'UTF8', 'language': 'en'},
            ],
                         'qr_code': 'https://www.cpo.com/invoices/13546', 'session_id': ANY, 'status': 'Running'}

        evse_id1 = 1
        connector_id = 1

        probe_module_mock_fn = Mock()

        probe_module.subscribe_variable("session_cost", "session_cost", probe_module_mock_fn)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Clear cache
        r: call_result201.ClearCachePayload = await chargepoint_with_pm.clear_cache_req()
        assert r.status == ClearCacheStatusType.accepted

        # make an unknown IdToken
        id_token = IdTokenType(
            id_token="DEADBEEF",
            type=IdTokenTypeEnum.iso14443
        )

        # Three TransactionEvents will be sent: started, updated and ended. The last two have the pricing information.
        central_system.mock.on_transaction_event.side_effect = [transaction_event_response_started,  # Started
                                                                transaction_event_response,  # Updated
                                                                transaction_event_response_ended]  # Ended

        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "StatusNotification",
                                           call201.StatusNotificationPayload(datetime.now().isoformat(),
                                                                             ConnectorStatusType.available,
                                                                             evse_id=evse_id1,
                                                                             connector_id=connector_id),
                                           validate_status_notification_201)

        # Charging station is now available, start charging session.
        # swipe id tag to authorize
        test_controller.swipe(id_token.id_token)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "Authorize",
                                           call201.AuthorizePayload(id_token
                                                                    ))

        # start charging session
        test_controller.plug_in()

        # should send a Transaction event
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "TransactionEvent",
                                           {"eventType": "Started"})

        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "TransactionEvent",
                                           {"eventType": "Updated"})

        # A session cost message should have been received
        await self.await_mock_called(probe_module_mock_fn)
        probe_module_mock_fn.assert_called_once_with(received_data)

        # Now stop the transaction, this should also send a TransactionEvent (Ended)
        test_controller.plug_out()

        # 'Final' costs are a bit different than the 'Running' costs.
        received_data['cost_chunks'][0] = {'cost': {'value': 5510000}, 'metervalue_to': 0, 'timestamp_to': ANY}
        received_data['status'] = 'Finished'
        probe_module_mock_fn.call_count = 0

        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "TransactionEvent",
                                           {"eventType": "Ended"})

        await self.await_mock_called(probe_module_mock_fn)
        probe_module_mock_fn.assert_called_once_with(received_data)

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceSessionCostConfigurationAdjustment())
    async def test_cost_updated_request(self, central_system: CentralSystem,
                                        test_controller: TestController, test_utility: TestUtility,
                                        test_config: OcppTestConfiguration, probe_module):
        """
        Test the 'cost updated request' with california pricing information.
        """
        received_data = {
            'charging_price': [
                {'category': 'Time', 'price': {'currency': {'code': 'EUR', 'decimals': 5}, 'value': {'value': 200000}}},
                {'category': 'Energy',
                 'price': {'currency': {'code': 'EUR', 'decimals': 5}, 'value': {'value': 12300}}},
                {'category': 'FlatFee',
                 'price': {'currency': {'code': 'EUR', 'decimals': 5}, 'value': {'value': 4242000}}}],
            'cost_chunks': [
                {'cost': {'value': 134500}, 'metervalue_to': 1234000, 'timestamp_to': ANY}],
            'currency': {'code': 'EUR', 'decimals': 5},
            'idle_price': {'grace_minutes': 30,
                           'hour_price': {'currency': {'code': 'EUR', 'decimals': 5}, 'value': {'value': 100000}}},
            'next_period': {
                'charging_price': [{'category': 'Time',
                                    'price': {'currency': {'code': 'EUR', 'decimals': 5}, 'value': {'value': 400000}}},
                                   {'category': 'Energy',
                                    'price': {'currency': {'code': 'EUR', 'decimals': 5}, 'value': {'value': 10000}}},
                                   {'category': 'FlatFee',
                                    'price': {'currency': {'code': 'EUR', 'decimals': 5},
                                              'value': {'value': 8484000}}}],
                'idle_price': {'hour_price': {'currency': {'code': 'EUR', 'decimals': 5}, 'value': {'value': 50000}}},
                'timestamp_from': ANY},
            'session_id': ANY, 'status': 'Running'}

        session_cost_mock = Mock()
        probe_module.subscribe_variable("session_cost", "session_cost", session_cost_mock)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # prepare data for the test
        evse_id1 = 1
        connector_id = 1

        # make an unknown IdToken
        id_token = IdTokenType(
            id_token="DEADBEEF",
            type=IdTokenTypeEnum.iso14443
        )

        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "StatusNotification",
                                           call201.StatusNotificationPayload(datetime.now().isoformat(),
                                                                             ConnectorStatusType.available,
                                                                             evse_id=evse_id1,
                                                                             connector_id=connector_id),
                                           validate_status_notification_201)

        # Send cost updated request while there is no transaction: This should just forward the request There is nothing
        # in the spec that sais what to do here and you can't send a 'rejected'.
        await chargepoint_with_pm.cost_update_req(total_cost=1.345, transaction_id="1",
                                                  custom_data=self.cost_updated_custom_data)

        # A session cost message should have been received
        await self.await_mock_called(session_cost_mock)
        session_cost_mock.assert_called_once_with(received_data)

        # swipe id tag to authorize
        test_controller.swipe(id_token.id_token)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "Authorize",
                                           call201.AuthorizePayload(id_token
                                                                    ))

        # start charging session
        test_controller.plug_in()

        # should send a Transaction event
        transaction_event = await wait_for_and_validate(test_utility, chargepoint_with_pm, "TransactionEvent",
                                                        {"eventType": "Started"})
        transaction_id = transaction_event['transaction_info']['transaction_id']

        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "TransactionEvent",
                                           {"eventType": "Updated"})

        # Clear cache
        r: call_result201.ClearCachePayload = await chargepoint_with_pm.clear_cache_req()
        assert r.status == ClearCacheStatusType.accepted
        session_cost_mock.call_count = 0

        await chargepoint_with_pm.cost_update_req(total_cost=1.345, transaction_id=transaction_id,
                                                  custom_data=self.cost_updated_custom_data)

        # A session cost message should have been received
        await self.await_mock_called(session_cost_mock)
        session_cost_mock.assert_called_once_with(received_data)

        # Clear cache
        r: call_result201.ClearCachePayload = await chargepoint_with_pm.clear_cache_req()
        assert r.status == ClearCacheStatusType.accepted
        session_cost_mock.call_count = 0

        # Set transaction id to a not existing transaction id.
        await chargepoint_with_pm.cost_update_req(total_cost=1.345, transaction_id="12345",
                                                  custom_data=self.cost_updated_custom_data)

        # A session cost message should still have been received
        await self.await_mock_called(session_cost_mock)
        session_cost_mock.assert_called_once_with(received_data)

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceMetervaluesConfigurationAdjustment(
        evse_manager_ids=["connector_1", "connector_2"]))
    async def test_running_cost_trigger_time(self, central_system: CentralSystem,
                                             test_controller: TestController, test_utility: TestUtility,
                                             test_config: OcppTestConfiguration, probe_module):
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
        transaction_id = await self.start_transaction(test_controller, test_utility, chargepoint_with_pm)

        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        test_utility.messages.clear()

        # Metervalues should be sent at below trigger time.
        data = self.cost_updated_custom_data.copy()
        data["triggerMeterValue"]["atTime"] = (datetime.now(timezone.utc) + timedelta(seconds=3)).isoformat()

        # Once the transaction is started, send a 'RunningCost' message.
        await chargepoint_with_pm.cost_update_req(total_cost=1.345, transaction_id=transaction_id,
                                                  custom_data=data)

        # At the given time, metervalues must have been sent.
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "MeterValues",
                                           {"evseId": 1, "meterValue": [{"sampledValue": [
                                               {"context": "Other", "location": "Outlet",
                                                "measurand": "Energy.Active.Import.Register",
                                                "unitOfMeasure": {"unit": "Wh"}, "value": 1.0}],
                                               'timestamp': timestamp[:-9] + 'Z'}]}
                                           )

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceMetervaluesConfigurationAdjustment(
        evse_manager_ids=["connector_1", "connector_2"]
    ))
    async def test_running_cost_trigger_energy(self, central_system: CentralSystem,
                                               test_controller: TestController, test_utility: TestUtility,
                                               test_config: OcppTestConfiguration, probe_module):
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
        transaction_id = await self.start_transaction(test_controller, test_utility, chargepoint_with_pm)

        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        test_utility.messages.clear()

        # Metervalues should be sent at below trigger time.
        data = self.cost_updated_custom_data.copy()

        # Send running cost, which has a trigger specified on atEnergykWh = 5.0
        await chargepoint_with_pm.cost_update_req(total_cost=1.345, transaction_id=transaction_id,
                                                  custom_data=data)

        # Now increase power meter value so it is above the specified trigger and publish the powermeter value
        power_meter_value["energy_Wh_import"]["total"] = 6000.0
        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        # Powermeter value should be sent because of the trigger.
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "MeterValues",
                                           {"evseId": 1, "meterValue": [{"sampledValue": [
                                               {"context": "Other", "location": "Outlet",
                                                "measurand": "Energy.Active.Import.Register",
                                                "unitOfMeasure": {"unit": "Wh"}, "value": 6000.0}],
                                               'timestamp': timestamp[:-9] + 'Z'}]}
                                           )

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceMetervaluesConfigurationAdjustment(
        evse_manager_ids=["connector_1", "connector_2"]
    ))
    async def test_running_cost_trigger_power(self, central_system: CentralSystem,
                                              test_controller: TestController, test_utility: TestUtility,
                                              test_config: OcppTestConfiguration, probe_module):
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
        transaction_id = await self.start_transaction(test_controller, test_utility, chargepoint_with_pm)

        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        test_utility.messages.clear()

        # Metervalues should be sent at below trigger time.
        data = self.cost_updated_custom_data.copy()

        # Send running cost, which has a trigger specified on atEnergykWh = 5.0
        await chargepoint_with_pm.cost_update_req(total_cost=1.345, transaction_id=transaction_id,
                                                  custom_data=data)

        # Set W above the trigger value and publish a new powermeter value.
        power_meter_value["energy_Wh_import"]["total"] = 1.0
        power_meter_value["power_W"]["total"] = 10000.0
        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        # Powermeter value should be sent because of the trigger.
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "MeterValues",
                                           {"evseId": 1, "meterValue": [{"sampledValue": [
                                               {"context": "Other", "location": "Outlet",
                                                "measurand": "Energy.Active.Import.Register",
                                                "unitOfMeasure": {"unit": "Wh"}, "value": 1.0},
                                               {'context': 'Other', 'location': 'Outlet',
                                                'measurand': 'Power.Active.Import', "unitOfMeasure": {"unit": "W"},
                                                'value': 10000.00}
                                           ],
                                               'timestamp': timestamp[:-9] + 'Z'}]}
                                           )

        # W value is below trigger, but hysteresis prevents sending the metervalue.
        power_meter_value["power_W"]["total"] = 7990.0
        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        # So no metervalue is sent
        assert not await wait_for_and_validate(test_utility, chargepoint_with_pm, "MeterValues",
                                               {"evseId": 1, "meterValue": [{"sampledValue": [
                                                   {"context": "Other", "location": "Outlet",
                                                    "measurand": "Energy.Active.Import.Register",
                                                    "unitOfMeasure": {"unit": "Wh"}, "value": 1.0},
                                                   {'context': 'Other', 'location': 'Outlet',
                                                    'measurand': 'Power.Active.Import', "unitOfMeasure": {"unit": "W"},
                                                    'value': 7990.0}
                                               ],
                                                   'timestamp': timestamp[:-9] + 'Z'}]}
                                               )

        # Only when trigger is high ( / low) enough, metervalue will be sent.
        power_meter_value["power_W"]["total"] = 7200.0
        timestamp = datetime.now(timezone.utc).isoformat()
        power_meter_value["timestamp"] = timestamp
        probe_module.publish_variable("ProbeModulePowerMeter", "powermeter", power_meter_value)

        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "MeterValues",
                                           {"evseId": 1, "meterValue": [{"sampledValue": [
                                               {"context": "Other", "location": "Outlet",
                                                "measurand": "Energy.Active.Import.Register",
                                                "unitOfMeasure": {"unit": "Wh"}, "value": 1.0},
                                               {'context': 'Other', 'location': 'Outlet',
                                                'measurand': 'Power.Active.Import', "unitOfMeasure": {"unit": "W"},
                                                'value': 7200.00}
                                           ],
                                               'timestamp': timestamp[:-9] + 'Z'}]}
                                           )
