# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
from everest.testing.core_utils.controller.test_controller_interface import TestController

from ocpp.v201.enums import OperationalStatusType, ChangeAvailabilityStatusType
from ocpp.v201.datatypes import EVSEType
from ocpp.v201 import call_result as call_result

# fmt: off
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility
from everest_test_utils import *
from everest.testing.ocpp_utils.fixtures import *
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
# fmt: on


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp201.yaml'))
async def test_g03(central_system_v201: CentralSystem, charge_point_v201: ChargePoint201, test_controller: TestController, test_utility: TestUtility):
    evse_1 = EVSEType(id=1)
    evse_1_1 = EVSEType(id=1, connector_id=1)
    evse_2 = EVSEType(id=2)
    evse_2_1 = EVSEType(id=2, connector_id=1)

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.inoperative, evse=evse_1_1)
    assert r.status == ChangeAvailabilityStatusType.accepted
    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 1, "connectorId": 1, "connectorStatus": "Unavailable"})

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.inoperative, evse=evse_2_1)
    assert r.status == ChangeAvailabilityStatusType.accepted
    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 2, "connectorId": 1, "connectorStatus": "Unavailable"})

    test_utility.forbidden_actions.append("StatusNotification")

    test_utility.messages.clear()

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.inoperative, evse=evse_1)
    assert r.status == ChangeAvailabilityStatusType.accepted

    test_utility.messages.clear()

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.inoperative, evse=evse_2)
    assert r.status == ChangeAvailabilityStatusType.accepted

    test_utility.messages.clear()

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.operative, evse=evse_1)
    assert r.status == ChangeAvailabilityStatusType.accepted

    test_utility.messages.clear()

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.operative, evse=evse_2)
    assert r.status == ChangeAvailabilityStatusType.accepted

    test_utility.messages.clear()

    test_utility.forbidden_actions.clear()

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.operative, evse=evse_1_1)
    assert r.status == ChangeAvailabilityStatusType.accepted
    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 1, "connectorId": 1, "connectorStatus": "Available"})

    test_utility.messages.clear()

    test_controller.stop()
    await asyncio.sleep(1)
    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(wait_for_bootnotification=False)

    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 1, "connectorId": 1, "connectorStatus": "Available"})
    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 2, "connectorId": 1, "connectorStatus": "Unavailable"})

    test_utility.messages.clear()

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.operative, evse=evse_2_1)
    assert r.status == ChangeAvailabilityStatusType.accepted
    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 2, "connectorId": 1, "connectorStatus": "Available"})

    test_controller.swipe("001", connectors=[1])
    test_controller.plug_in()

    assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent", {"eventType": "Started", "evse": {"id": 1}})

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.inoperative, evse=evse_1_1)
    assert r.status == ChangeAvailabilityStatusType.scheduled

    test_utility.messages.clear()

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.inoperative, evse=evse_2_1)
    assert r.status == ChangeAvailabilityStatusType.accepted

    test_utility.messages.clear()

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.inoperative, evse=evse_1)
    assert r.status == ChangeAvailabilityStatusType.scheduled

    test_utility.messages.clear()

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.inoperative, evse=evse_2)
    assert r.status == ChangeAvailabilityStatusType.accepted

    test_utility.messages.clear()

    test_controller.plug_out()

    assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent", {"eventType": "Ended"})

    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 1, "connectorId": 1, "connectorStatus": "Unavailable"})
    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 2, "connectorId": 1, "connectorStatus": "Unavailable"})

    test_utility.messages.clear()

    # try state that EVSE is already in
    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.inoperative, evse=evse_1_1)
    assert r.status == ChangeAvailabilityStatusType.accepted

    test_utility.messages.clear()

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.operative, evse=evse_1)
    assert r.status == ChangeAvailabilityStatusType.accepted

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.operative, evse=evse_1_1)
    assert r.status == ChangeAvailabilityStatusType.accepted

    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 1, "connectorId": 1, "connectorStatus": "Available"})


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.everest_core_config(get_everest_config_path_str('everest-config-ocpp201.yaml'))
async def test_g04(central_system_v201: CentralSystem, charge_point_v201: ChargePoint201, test_controller: TestController, test_utility: TestUtility):
    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.operative)
    assert r.status == ChangeAvailabilityStatusType.accepted

    test_utility.messages.clear()

    test_controller.swipe("001", connectors=[1])
    test_controller.plug_in()

    assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent", {"eventType": "Started", "evse": {"id": 1}})

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.operative)
    assert r.status == ChangeAvailabilityStatusType.accepted


    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.inoperative)
    assert r.status == ChangeAvailabilityStatusType.scheduled

    test_controller.plug_out()
    test_utility.messages.clear()

    assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent", {"eventType": "Ended"})

    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 1, "connectorId": 1, "connectorStatus": "Unavailable"})
    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 2, "connectorId": 1, "connectorStatus": "Unavailable"})

    test_utility.messages.clear()

    test_controller.stop()
    await asyncio.sleep(1)
    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(wait_for_bootnotification=False)

    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 1, "connectorId": 1, "connectorStatus": "Unavailable"})
    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 2, "connectorId": 1, "connectorStatus": "Unavailable"})

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.operative)
    assert r.status == ChangeAvailabilityStatusType.accepted

    test_utility.messages.clear()

    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 1, "connectorId": 1, "connectorStatus": "Available"})
    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 2, "connectorId": 1, "connectorStatus": "Available"})

    await asyncio.sleep(2)

    test_utility.messages.clear()

    test_controller.swipe("001", connectors=[1])

    await asyncio.sleep(2)

    test_controller.plug_in()

    assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent", {"eventType": "Started", "evse": {"id": 1}})

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.inoperative)
    assert r.status == ChangeAvailabilityStatusType.scheduled

    test_utility.messages.clear()

    r: call_result.ChangeAvailabilityPayload = await charge_point_v201.change_availablility_req(
        operational_status=OperationalStatusType.operative)
    assert r.status == ChangeAvailabilityStatusType.accepted

    await asyncio.sleep(2)

    test_controller.plug_out()
    test_utility.messages.clear()

    assert await wait_for_and_validate(test_utility, charge_point_v201, "TransactionEvent", {"eventType": "Ended"})

    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification", {"evseId": 1, "connectorId": 1, "connectorStatus": "Available"})
