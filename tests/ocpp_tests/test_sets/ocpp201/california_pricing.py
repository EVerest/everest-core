# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from datetime import timezone
from unittest.mock import Mock, ANY

import logging
from copy import deepcopy

# Needs to be before the datatypes below since it overrides the v201 Action enum with the v16 one
from everest_test_utils import *
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
from everest.testing.core_utils.controller.test_controller_interface import TestController

from ocpp.v201 import call as call201
from ocpp.v201 import call_result as call_result201
from ocpp.v201.enums import (IdTokenEnumType as IdTokenTypeEnum, ConnectorStatusEnumType,
                             ClearCacheStatusEnumType, SetVariableStatusEnumType,
                             AttributeEnumType)
from ocpp.v201.datatypes import *
from everest.testing.ocpp_utils.fixtures import *
from everest_test_utils_probe_modules import (probe_module,
                                              ProbeModuleCostAndPriceMetervaluesConfigurationAdjustment,
                                              ProbeModuleCostAndPriceSessionCostConfigurationAdjustment)

from everest.testing.core_utils._configuration.libocpp_configuration_helper import (
    GenericOCPP2XConfigAdjustment,
    OCPP2XConfigVariableIdentifier,
)

from validations import validate_status_notification_201

log = logging.getLogger("ocpp201CaliforniaPricingTest")


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
    (OCPP2XConfigVariableIdentifier("CustomizationCtrlr", "CustomImplementationCaliforniaPricingEnabled",
                                    "Actual"), "true"),
    (OCPP2XConfigVariableIdentifier("CustomizationCtrlr", "CustomImplementationMultiLanguageEnabled",
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
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceSessionCostConfigurationAdjustment())
    async def test_set_running_cost(self, central_system: CentralSystem, test_controller: TestController,
                                    test_utility: TestUtility, test_config: OcppTestConfiguration, probe_module):
        """
        Test running and final cost, that is 'embedded' in the TransactionEventResponse.
        """
        # prepare data for the test
        transaction_event_response_started = call_result201.TransactionEvent()

        transaction_event_response = call_result201.TransactionEvent()
        # According to the OCPP spec this should be a floating point number but the test framework does not allow that.
        transaction_event_response.total_cost = 3.13
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

        probe_module.subscribe_variable(
            "session_cost", "session_cost", probe_module_mock_fn)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Clear cache
        r: call_result201.ClearCache = await chargepoint_with_pm.clear_cache_req()
        assert r.status == ClearCacheStatusEnumType.accepted

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
                                           call201.StatusNotification(datetime.now().isoformat(),
                                                                      ConnectorStatusEnumType.available,
                                                                      evse_id=evse_id1,
                                                                      connector_id=connector_id),
                                           validate_status_notification_201)

        # Charging station is now available, start charging session.
        # swipe id tag to authorize
        test_controller.swipe(id_token.id_token)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "Authorize",
                                           call201.Authorize(id_token
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
        received_data['cost_chunks'][0] = {
            'cost': {'value': 5510000}, 'metervalue_to': 0, 'timestamp_to': ANY}
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
                {'category': 'Time', 'price': {'currency': {
                    'code': 'EUR', 'decimals': 5}, 'value': {'value': 200000}}},
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
        probe_module.subscribe_variable(
            "session_cost", "session_cost", session_cost_mock)

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
                                           call201.StatusNotification(datetime.now().isoformat(),
                                                                      ConnectorStatusEnumType.available,
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
                                           call201.Authorize(id_token
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
        r: call_result201.ClearCache = await chargepoint_with_pm.clear_cache_req()
        assert r.status == ClearCacheStatusEnumType.accepted
        session_cost_mock.call_count = 0

        await chargepoint_with_pm.cost_update_req(total_cost=1.345, transaction_id=transaction_id,
                                                  custom_data=self.cost_updated_custom_data)

        # A session cost message should have been received
        await self.await_mock_called(session_cost_mock)
        session_cost_mock.assert_called_once_with(received_data)

        # Clear cache
        r: call_result201.ClearCache = await chargepoint_with_pm.clear_cache_req()
        assert r.status == ClearCacheStatusEnumType.accepted
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

        probe_module.implement_command(
            "ProbeModulePowerMeter", "start_transaction", probe_module_mock_fn)
        probe_module.implement_command(
            "ProbeModulePowerMeter", "stop_transaction", probe_module_mock_fn)

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
        probe_module.publish_variable(
            "ProbeModulePowerMeter", "powermeter", power_meter_value)

        test_utility.messages.clear()

        # Metervalues should be sent at below trigger time.
        data = self.cost_updated_custom_data.copy()
        data["triggerMeterValue"]["atTime"] = (datetime.now(
            timezone.utc) + timedelta(seconds=3)).isoformat()

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

        probe_module.implement_command(
            "ProbeModulePowerMeter", "start_transaction", probe_module_mock_fn)
        probe_module.implement_command(
            "ProbeModulePowerMeter", "stop_transaction", probe_module_mock_fn)

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
        probe_module.publish_variable(
            "ProbeModulePowerMeter", "powermeter", power_meter_value)

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
        probe_module.publish_variable(
            "ProbeModulePowerMeter", "powermeter", power_meter_value)

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

        probe_module.implement_command(
            "ProbeModulePowerMeter", "start_transaction", probe_module_mock_fn)
        probe_module.implement_command(
            "ProbeModulePowerMeter", "stop_transaction", probe_module_mock_fn)

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
        probe_module.publish_variable(
            "ProbeModulePowerMeter", "powermeter", power_meter_value)

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
        probe_module.publish_variable(
            "ProbeModulePowerMeter", "powermeter", power_meter_value)

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
        probe_module.publish_variable(
            "ProbeModulePowerMeter", "powermeter", power_meter_value)

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
        probe_module.publish_variable(
            "ProbeModulePowerMeter", "powermeter", power_meter_value)

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

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceSessionCostConfigurationAdjustment())
    async def test_tariff_fallback_message_on_authorize(self, central_system: CentralSystem,
                                                         test_controller: TestController,
                                                         test_utility: TestUtility,
                                                         test_config: OcppTestConfiguration,
                                                         probe_module):
        """
        I04.FR.01 integration: When TariffFallbackMessage is configured and the CSMS returns no
        personalMessage in AuthorizeResponse, test if personal message is still set to the configured fallback.
        The injected personalMessage is then converted and published via session_cost.tariff_message
        so the display can show the price to the EV Driver.
        """
        tariff_message_mock = Mock()
        probe_module.subscribe_variable("session_cost", "tariff_message", tariff_message_mock)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Clear auth cache so the CSMS is consulted for every authorization.
        r: call_result201.ClearCache = await chargepoint_with_pm.clear_cache_req()
        assert r.status == ClearCacheStatusEnumType.accepted

        # Configure TariffFallbackMessage on the CS via OCPP SetVariables.
        r = await chargepoint_with_pm.set_config_variables_req(
            "TariffCostCtrlr", "TariffFallbackMessage", "Tariff: 0.30 EUR/kWh"
        )
        assert SetVariableResultType(**r.set_variable_result[0]).attribute_status == SetVariableStatusEnumType.accepted

        evse_id1 = 1
        connector_id = 1
        id_token = IdTokenType(id_token="DEADBEEF", type=IdTokenTypeEnum.iso14443)

        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "StatusNotification",
                                           call201.StatusNotification(datetime.now().isoformat(),
                                                                      ConnectorStatusEnumType.available,
                                                                      evse_id=evse_id1,
                                                                      connector_id=connector_id),
                                           validate_status_notification_201)

        # Swipe to trigger authorization. The CSMS mock returns Accepted without a personalMessage
        # (default behavior), so ensure_personal_message must inject the configured fallback.
        test_controller.swipe(id_token.id_token)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "Authorize",
                                           call201.Authorize(id_token))

        # The fallback tariff message must arrive on session_cost.tariff_message.
        await self.await_mock_called(tariff_message_mock)

        call_data = tariff_message_mock.call_args[0][0]
        assert call_data["identifier_id"] == "DEADBEEF"
        assert call_data["identifier_type"] == "IdToken"
        assert len(call_data["messages"]) == 1
        assert call_data["messages"][0]["content"] == "Tariff: 0.30 EUR/kWh"

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceSessionCostConfigurationAdjustment())
    async def test_tariff_fallback_message_multilanguage_on_authorize(self, central_system: CentralSystem,
                                                                       test_controller: TestController,
                                                                       test_utility: TestUtility,
                                                                       test_config: OcppTestConfiguration,
                                                                       probe_module):
        """
        California Pricing 4.3.4 integration: When TariffFallbackMessage is configured for multiple languages, the
        base (no-instance) message becomes personalMessage and language-specific instances go into
        customData.personalMessageExtra. Both are forwarded as entries in session_cost.tariff_message
        so multi-language displays can show the correct price text.
        """
        tariff_message_mock = Mock()
        probe_module.subscribe_variable("session_cost", "tariff_message", tariff_message_mock)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Clear auth cache so the CSMS is consulted for every authorization.
        r: call_result201.ClearCache = await chargepoint_with_pm.clear_cache_req()
        assert r.status == ClearCacheStatusEnumType.accepted

        # Set the base TariffFallbackMessage (no language instance).
        r = await chargepoint_with_pm.set_config_variables_req(
            "TariffCostCtrlr", "TariffFallbackMessage", "Tariff: 0.30 EUR/kWh"
        )
        assert SetVariableResultType(**r.set_variable_result[0]).attribute_status == SetVariableStatusEnumType.accepted

        # Set the German-language instance ("de" is in DisplayMessageCtrlr.Language.valuesList).
        r = await chargepoint_with_pm.set_variables_req(set_variable_data=[
            SetVariableDataType(
                attribute_value="Tarif: 0,30 EUR/kWh",
                attribute_type=AttributeEnumType.actual,
                component=ComponentType(name="TariffCostCtrlr"),
                variable=VariableType(name="TariffFallbackMessage", instance="de")
            )
        ])
        assert SetVariableResultType(**r.set_variable_result[0]).attribute_status == SetVariableStatusEnumType.accepted

        evse_id1 = 1
        connector_id = 1
        id_token = IdTokenType(id_token="DEADBEEF", type=IdTokenTypeEnum.iso14443)

        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "StatusNotification",
                                           call201.StatusNotification(datetime.now().isoformat(),
                                                                      ConnectorStatusEnumType.available,
                                                                      evse_id=evse_id1,
                                                                      connector_id=connector_id),
                                           validate_status_notification_201)

        test_controller.swipe(id_token.id_token)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "Authorize",
                                           call201.Authorize(id_token))

        await self.await_mock_called(tariff_message_mock)

        call_data = tariff_message_mock.call_args[0][0]
        assert call_data["identifier_id"] == "DEADBEEF"
        assert call_data["identifier_type"] == "IdToken"
        # Base message + German entry from personalMessageExtra.
        assert len(call_data["messages"]) == 2
        assert call_data["messages"][0]["content"] == "Tariff: 0.30 EUR/kWh"
        assert call_data["messages"][1]["content"] == "Tarif: 0,30 EUR/kWh"
        assert call_data["messages"][1]["language"] == "de"

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceSessionCostConfigurationAdjustment())
    async def test_total_cost_fallback_message_on_offline_transaction_end(self, central_system: CentralSystem,
                                                                           test_controller: TestController,
                                                                           test_utility: TestUtility,
                                                                           test_config: OcppTestConfiguration,
                                                                           probe_module):
        """
        I05.FR.02 integration: When the CS is offline when a transaction ends and TotalCostFallbackMessage
        is configured, a default total cost shall still be published via session_cost.tariff_message,
        even if the CSMS response containing totalCost will never arrive.
        """
        tariff_message_mock = Mock()
        probe_module.subscribe_variable("session_cost", "tariff_message", tariff_message_mock)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        # Configure TotalCostFallbackMessage. TariffFallbackMessage is intentionally left empty so
        # the auth step does not produce a tariff_message event.
        r = await chargepoint_with_pm.set_config_variables_req(
            "TariffCostCtrlr", "TotalCostFallbackMessage", "Total cost unavailable (offline)"
        )
        assert SetVariableResultType(**r.set_variable_result[0]).attribute_status == SetVariableStatusEnumType.accepted

        evse_id1 = 1
        connector_id = 1
        id_token = IdTokenType(id_token="DEADBEEF", type=IdTokenTypeEnum.iso14443)

        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "StatusNotification",
                                           call201.StatusNotification(datetime.now().isoformat(),
                                                                      ConnectorStatusEnumType.available,
                                                                      evse_id=evse_id1,
                                                                      connector_id=connector_id),
                                           validate_status_notification_201)

        # Start a transaction while online.
        test_controller.swipe(id_token.id_token)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "Authorize",
                                           call201.Authorize(id_token))

        test_controller.plug_in()
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "TransactionEvent",
                                           {"eventType": "Started"})
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "TransactionEvent",
                                           {"eventType": "Updated"})

        # Go offline before ending the transaction so that the CS cannot retrieve totalCost.
        test_controller.disconnect_websocket()
        await asyncio.sleep(2)

        # End the transaction while offline.
        test_controller.plug_out()

        # The TotalCostFallbackMessage must be published via session_cost.tariff_message.
        await self.await_mock_called(tariff_message_mock)

        call_data = tariff_message_mock.call_args[0][0]
        assert call_data["identifier_type"] == "TransactionId"
        assert len(call_data["messages"]) == 1
        assert call_data["messages"][0]["content"] == "Total cost unavailable (offline)"

        # Reconnect so queued TransactionEvent(Ended) can be sent and test teardown completes cleanly.
        test_controller.connect_websocket()
        chargepoint_with_pm = await central_system.wait_for_chargepoint(wait_for_bootnotification=False)
        assert await wait_for_and_validate(test_utility, chargepoint_with_pm, "TransactionEvent",
                                           {"eventType": "Ended"})

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceSessionCostConfigurationAdjustment())
    async def test_default_price_published_on_disconnect(self, central_system: CentralSystem,
                                                         test_controller: TestController,
                                                         test_utility: TestUtility,
                                                         test_config: OcppTestConfiguration,
                                                         probe_module):
        """
        When TariffFallbackMessage and OfflineTariffFallbackMessage are configured and the CS goes
        offline, default_price shall be published via session_cost.default_price with the configured
        offline price text.
        """
        default_price_mock = Mock()
        probe_module.subscribe_variable("session_cost", "default_price", default_price_mock)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        r = await chargepoint_with_pm.set_config_variables_req(
            "TariffCostCtrlr", "TariffFallbackMessage", "0.30 EUR/kWh"
        )
        assert SetVariableResultType(**r.set_variable_result[0]).attribute_status == SetVariableStatusEnumType.accepted

        r = await chargepoint_with_pm.set_config_variables_req(
            "TariffCostCtrlr", "OfflineTariffFallbackMessage", "Station is offline"
        )
        assert SetVariableResultType(**r.set_variable_result[0]).attribute_status == SetVariableStatusEnumType.accepted

        # Reset any publications from the startup phase before triggering a controlled disconnect.
        default_price_mock.reset_mock()

        test_controller.disconnect_websocket()

        # Multiple publications can arrive around disconnect; wait for the explicit offline text.
        offline_message_received = False
        for _ in range(30):
            for call_args in default_price_mock.call_args_list:
                call_data = call_args[0][0]
                if (
                    len(call_data.get("messages", [])) == 1
                    and call_data["messages"][0].get("content") == "Station is offline"
                ):
                    offline_message_received = True
                    break
            if offline_message_received:
                break
            await asyncio.sleep(0.1)

        assert offline_message_received

        # Reconnect for clean teardown.
        test_controller.connect_websocket()
        await central_system.wait_for_chargepoint(wait_for_bootnotification=False)

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceSessionCostConfigurationAdjustment())
    async def test_default_price_published_on_reconnect(self, central_system: CentralSystem,
                                                        test_controller: TestController,
                                                        test_utility: TestUtility,
                                                        test_config: OcppTestConfiguration,
                                                        probe_module):
        """
        When TariffFallbackMessage is configured and the CS reconnects to the CSMS after being
        offline, default_price shall be re-published with the configured online price text.
        """
        default_price_mock = Mock()
        probe_module.subscribe_variable("session_cost", "default_price", default_price_mock)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        r = await chargepoint_with_pm.set_config_variables_req(
            "TariffCostCtrlr", "TariffFallbackMessage", "0.30 EUR/kWh"
        )
        assert SetVariableResultType(**r.set_variable_result[0]).attribute_status == SetVariableStatusEnumType.accepted

        r = await chargepoint_with_pm.set_config_variables_req(
            "TariffCostCtrlr", "OfflineTariffFallbackMessage", "Station is offline"
        )
        assert SetVariableResultType(**r.set_variable_result[0]).attribute_status == SetVariableStatusEnumType.accepted

        # Go offline and wait for the offline publication, then clear the mock.
        default_price_mock.reset_mock()
        test_controller.disconnect_websocket()
        await self.await_mock_called(default_price_mock)
        default_price_mock.reset_mock()

        # Reconnect and wait for the online publication.
        test_controller.connect_websocket()
        chargepoint_with_pm = await central_system.wait_for_chargepoint(wait_for_bootnotification=False)

        await self.await_mock_called(default_price_mock)

        call_data = default_price_mock.call_args[0][0]
        assert len(call_data["messages"]) == 1
        assert call_data["messages"][0]["content"] == "0.30 EUR/kWh"

    @pytest.mark.asyncio
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleCostAndPriceSessionCostConfigurationAdjustment())
    async def test_default_price_published_on_change(self, central_system: CentralSystem,
                                                     test_controller: TestController,
                                                     test_utility: TestUtility,
                                                     test_config: OcppTestConfiguration,
                                                     probe_module):
        """
        When TariffFallbackMessage is changed via SetVariables while connected, default_price
        shall be immediately re-published with the new price text.
        """
        default_price_mock = Mock()
        probe_module.subscribe_variable("session_cost", "default_price", default_price_mock)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        chargepoint_with_pm = await central_system.wait_for_chargepoint()
        default_price_mock.reset_mock()

        r = await chargepoint_with_pm.set_config_variables_req(
            "TariffCostCtrlr", "TariffFallbackMessage", "0.30 EUR/kWh"
        )
        assert SetVariableResultType(**r.set_variable_result[0]).attribute_status == SetVariableStatusEnumType.accepted

        await self.await_mock_called(default_price_mock)

        call_data = default_price_mock.call_args[0][0]
        assert len(call_data["messages"]) == 1
        assert call_data["messages"][0]["content"] == "0.30 EUR/kWh"
