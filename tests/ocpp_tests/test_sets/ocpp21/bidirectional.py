# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
from datetime import datetime, timezone

import traceback
# fmt: off
import logging

from everest.testing.core_utils.controller.test_controller_interface import TestController

from ocpp.v21 import call as call21
from ocpp.v21 import call_result as call_result21
from ocpp.v21.enums import *
from ocpp.v21.datatypes import *
from ocpp.routing import on, create_route_map
from everest.testing.ocpp_utils.fixtures import *
from everest_test_utils import * # Needs to be before the datatypes below since it overrides the v21 Action enum with the v16 one
from ocpp.v21.enums import (Action, ConnectorStatusEnumType, AuthorizationStatusEnumType, EnergyTransferModeEnumType, AttributeEnumType, GetVariableStatusEnumType, NotifyEVChargingNeedsStatusEnumType, NotifyAllowedEnergyTransferStatusEnumType)
from validations import validate_status_notification_201
from everest.testing.core_utils._configuration.libocpp_configuration_helper import GenericOCPP2XConfigAdjustment
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility
# fmt: on

log = logging.getLogger("bidirectionalTest")


def validate_notify_ev_charging_needs(meta_data, msg, expected):
    # Q01.FR.03
    return (
        msg.payload["evseId"] == expected["evseId"]
        and msg.payload["chargingNeeds"]["requestedEnergyTransfer"] == expected["requestedEnergyTransfer"]
        and msg.payload["chargingNeeds"]["v2xChargingParameters"]
        and msg.payload["chargingNeeds"]["controlMode"] == expected["controlMode"]
    )


def validate_tx_event_with_evccid(meta_data, msg, expected):
    return (
        msg.payload["eventType"] == expected["eventType"]
        and msg.payload["idToken"]["additionalInfo"][0]["type"] == "EVCCID"
    )


@pytest.mark.xdist_group(name="ISO15118")
@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config("everest-config-ocpp201-sil-dc-d20-eim.yaml")
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_q01(
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    Q01
    ...
    """

    log.info(
        "##################### Q01: V2X Authorization #################"
    )
    id_token = IdTokenType(id_token="8BADF00D",
                           type="ISO14443")

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(wait_for_bootnotification=True)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "StatusNotification",
        call21.StatusNotification(
            1,  ConnectorStatusEnumType.available, 1, datetime.now().isoformat()
        ),
        validate_status_notification_201,
    )

    iso_enabled = GetVariableDataType(component=ComponentType(name="ISO15118Ctrlr", evse=EVSEType(id=1)),
                                      variable=VariableType(name="Enabled"),
                                      attribute_type=AttributeEnumType.actual)
    v2x_enabled = GetVariableDataType(component=ComponentType(name="V2XChargingCtrlr", evse=EVSEType(id=1)),
                                      variable=VariableType(name="Enabled"),
                                      attribute_type=AttributeEnumType.actual)
    v2x_supported_op_modes = GetVariableDataType(component=ComponentType(name="V2XChargingCtrlr", evse=EVSEType(id=1)),
                                                 variable=VariableType(
                                                     name="SupportedOperationModes"),
                                                 attribute_type=AttributeEnumType.actual)
    v2x_supported_energy_transfers = GetVariableDataType(component=ComponentType(name="V2XChargingCtrlr", evse=EVSEType(id=1)),
                                                         variable=VariableType(
        name="SupportedEnergyTransferModes"),
        attribute_type=AttributeEnumType.actual)
    list_of_vars = [iso_enabled, v2x_enabled,
                    v2x_supported_op_modes, v2x_supported_energy_transfers]
    r: call_result21.GetVariables = await charge_point_v21.get_variables_req(get_variable_data=list_of_vars)

    for result in r.get_variable_result:
        assert result['attribute_status'] == GetVariableStatusEnumType.accepted
        if result['variable']['name'] == 'Enabled':
            # Q01.FR.01
            assert result['attribute_value'] == 'true'
        elif result['variable']['name'] == 'SupportedOperationModes':
            # Q01.FR.31
            # TODO(mlitre) update to check for the min requirements once they are supported
            # Notably we need: ChargingOnly, CentralSetpoint and CentralFrequency
            assert result['attribute_value']
        elif result['variable']['name'] == 'SupportedEnergyTransferModes':
            # Q01.FR.32, we just check that it is not empty
            assert result['attribute_value']

    test_controller.plug_in_dc_iso()
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "StatusNotification",
        call21.StatusNotification(
            1,  ConnectorStatusEnumType.occupied, 1, datetime.now().isoformat()
        ),
        validate_status_notification_201,
    )

    @on(Action.authorize)
    def on_authorize(**kwargs):
        return call_result21.Authorize(
            id_token_info=IdTokenInfoType(
                status=AuthorizationStatusEnumType.accepted,
                group_id_token=IdTokenType(
                    id_token="12345", type="Central"
                ),
            ),
            allowed_energy_transfer=[EnergyTransferModeEnumType.dc_bpt]
        )

    setattr(charge_point_v21, "on_authorize", on_authorize)
    central_system_v21.chargepoint.route_map = create_route_map(
        central_system_v21.chargepoint
    )
    test_controller.swipe(id_token.id_token)

    # Question: should we check other flows aka auth first then plug
    # Checking here Q01.FR.02 for idToken is not reliable, as we start tx before we get evcc id
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "TransactionEvent",
        {"eventType": "Started", "offline": False},
    )

    # Q01.FR.03: Check all the fields of NotifyEVChargingNeeds
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "NotifyEVChargingNeeds",
        {"evseId": 1, "requestedEnergyTransfer": "DC",
            "controlMode": "DynamicControl", "mobilityNeedsMode": "EVCC"},
        validate_notify_ev_charging_needs
    )

    # Check variables after NotifyEVChargingNeeds, so that we don't have to guess when the variables have been updated
    ev_available = GetVariableDataType(component=ComponentType(name="ConnectedEV", evse=EVSEType(id=1)),
                                       variable=VariableType(name="Available"),
                                       attribute_type=AttributeEnumType.actual)
    ev_vehicle_id = GetVariableDataType(component=ComponentType(name="ConnectedEV", evse=EVSEType(id=1)),
                                        variable=VariableType(
                                            name="VehicleId"),
                                        attribute_type=AttributeEnumType.actual)
    ev_protocol_agreed = GetVariableDataType(component=ComponentType(name="ConnectedEV", evse=EVSEType(id=1)),
                                             variable=VariableType(
        name="ProtocolAgreed"),
        attribute_type=AttributeEnumType.actual)
    ev_protocol_supported_by_ev1 = GetVariableDataType(component=ComponentType(name="ConnectedEV", evse=EVSEType(id=1)),
                                                       variable=VariableType(
        name="ProtocolSupportedByEV", instance="1"),
        attribute_type=AttributeEnumType.actual)
    list_of_vars = [ev_available, ev_vehicle_id,
                    ev_protocol_agreed, ev_protocol_supported_by_ev1]
    r: call_result21.GetVariables = await charge_point_v21.get_variables_req(get_variable_data=list_of_vars)
    # TODO(mlitre): Add check on VehicleCertificate when it is supported

    # Q01.FR.36: Validate ConnectedEV variables
    for result in r.get_variable_result:
        assert result['attribute_status'] == GetVariableStatusEnumType.accepted
        if result['variable']['name'] == 'Available':
            assert result['attribute_value'] == 'true'
        elif result['variable']['name'] == 'VehicleId':
            assert result['attribute_value']
        elif result['variable']['name'] == 'ProtocolAgreed':
            assert result['attribute_value'] == 'urn:iso:std:iso:15118:-20:DC,1,0'
        elif result['variable']['name'] == 'ProtocolSupportedByEV':
            # TODO(mlitre): How many should we check?
            assert result['attribute_value']

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "TransactionEvent",
        {"eventType": "Updated", "transactionInfo": {"chargingState": "Charging"}},
    )

    test_controller.swipe(id_token.id_token)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "TransactionEvent",
        {"eventType": "Ended"},
        validate_tx_event_with_evccid
    )


@pytest.mark.xdist_group(name="ISO15118")
@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config("everest-config-ocpp201-sil-dc-d20-eim.yaml")
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_rejected_q01(
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    Q01
    ...
    """

    log.info(
        "##################### Q01: V2X Authorization #################"
    )
    id_token = IdTokenType(id_token="8BADF00D",
                           type="ISO14443")

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(wait_for_bootnotification=True)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "StatusNotification",
        call21.StatusNotification(
            1,  ConnectorStatusEnumType.available, 1, datetime.now().isoformat()
        ),
        validate_status_notification_201,
    )

    iso_enabled = GetVariableDataType(component=ComponentType(name="ISO15118Ctrlr", evse=EVSEType(id=1)),
                                      variable=VariableType(name="Enabled"),
                                      attribute_type=AttributeEnumType.actual)
    v2x_enabled = GetVariableDataType(component=ComponentType(name="V2XChargingCtrlr", evse=EVSEType(id=1)),
                                      variable=VariableType(name="Enabled"),
                                      attribute_type=AttributeEnumType.actual)
    v2x_supported_op_modes = GetVariableDataType(component=ComponentType(name="V2XChargingCtrlr", evse=EVSEType(id=1)),
                                                 variable=VariableType(
                                                     name="SupportedOperationModes"),
                                                 attribute_type=AttributeEnumType.actual)
    v2x_supported_energy_transfers = GetVariableDataType(component=ComponentType(name="V2XChargingCtrlr", evse=EVSEType(id=1)),
                                                         variable=VariableType(
        name="SupportedEnergyTransferModes"),
        attribute_type=AttributeEnumType.actual)
    list_of_vars = [iso_enabled, v2x_enabled,
                    v2x_supported_op_modes, v2x_supported_energy_transfers]
    r: call_result21.GetVariables = await charge_point_v21.get_variables_req(get_variable_data=list_of_vars)

    for result in r.get_variable_result:
        assert result['attribute_status'] == GetVariableStatusEnumType.accepted
        if result['variable']['name'] == 'Enabled':
            # Q01.FR.01
            assert result['attribute_value'] == 'true'
        elif result['variable']['name'] == 'SupportedOperationModes':
            # Q01.FR.31
            # TODO(mlitre) update to check for the min requirements once they are supported: ChargingOnly, CentralSetpoint, CentralFrequency
            assert result['attribute_value']
        elif result['variable']['name'] == 'SupportedEnergyTransferModes':
            # Q01.FR.32, we just check that it is not empty
            assert result['attribute_value']

    test_controller.plug_in_dc_iso()
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "StatusNotification",
        call21.StatusNotification(
            1,  ConnectorStatusEnumType.occupied, 1, datetime.now().isoformat()
        ),
        validate_status_notification_201,
    )

    @on(Action.authorize)
    def on_authorize(**kwargs):
        return call_result21.Authorize(
            id_token_info=IdTokenInfoType(
                status=AuthorizationStatusEnumType.accepted,
                group_id_token=IdTokenType(
                    id_token="12345", type="Central"
                ),
            ),
            allowed_energy_transfer=[EnergyTransferModeEnumType.dc_bpt]
        )

    setattr(charge_point_v21, "on_authorize", on_authorize)
    central_system_v21.chargepoint.route_map = create_route_map(
        central_system_v21.chargepoint
    )
    test_controller.swipe(id_token.id_token)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "TransactionEvent",
        {"eventType": "Started", "offline": False},
    )

    @on(Action.notify_ev_charging_needs)
    def on_notify_ev_charging_needs(**kwargs):
        return call_result21.NotifyEVChargingNeeds(
            status=NotifyEVChargingNeedsStatusEnumType.rejected
        )

    setattr(charge_point_v21, "on_notify_ev_charging_needs",
            on_notify_ev_charging_needs)
    central_system_v21.chargepoint.route_map = create_route_map(
        central_system_v21.chargepoint
    )

    # Q01.FR.03: Check all the fields of NotifyEVChargingNeeds
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "NotifyEVChargingNeeds",
        {"evseId": 1, "requestedEnergyTransfer": "DC",
            "controlMode": "DynamicControl", "mobilityNeedsMode": "EVCC"},
        validate_notify_ev_charging_needs
    )
    # Check variables after NotifyEVChargingNeeds, so that we don't have to guess when the variables have been updated
    ev_available = GetVariableDataType(component=ComponentType(name="ConnectedEV", evse=EVSEType(id=1)),
                                       variable=VariableType(name="Available"),
                                       attribute_type=AttributeEnumType.actual)
    ev_vehicle_id = GetVariableDataType(component=ComponentType(name="ConnectedEV", evse=EVSEType(id=1)),
                                        variable=VariableType(
                                            name="VehicleId"),
                                        attribute_type=AttributeEnumType.actual)
    ev_protocol_agreed = GetVariableDataType(component=ComponentType(name="ConnectedEV", evse=EVSEType(id=1)),
                                             variable=VariableType(
        name="ProtocolAgreed"),
        attribute_type=AttributeEnumType.actual)
    ev_protocol_supported_by_ev = GetVariableDataType(component=ComponentType(name="ConnectedEV", evse=EVSEType(id=1)),
                                                      variable=VariableType(
        name="ProtocolSupportedByEV", instance="1"),
        attribute_type=AttributeEnumType.actual)
    list_of_vars = [ev_available, ev_vehicle_id,
                    ev_protocol_agreed, ev_protocol_supported_by_ev]
    r: call_result21.GetVariables = await charge_point_v21.get_variables_req(get_variable_data=list_of_vars)
    # TODO(mlitre): Add check on VehicleCertificate when it is supported

    # Q01.FR.36: Validate ConnectedEV variables
    for result in r.get_variable_result:
        assert result['attribute_status'] == GetVariableStatusEnumType.accepted
        if result['variable']['name'] == 'Available':
            assert result['attribute_value'] == 'true'
        elif result['variable']['name'] == 'VehicleId':
            # TODO(mlitre): Do we know the value before hand to check?
            assert result['attribute_value']
        elif result['variable']['name'] == 'ProtocolAgreed':
            assert result['attribute_value'] == 'urn:iso:std:iso:15118:-20:DC,1,0'
        elif result['variable']['name'] == 'ProtocolSupportedByEV':
            # TODO(mlitre): How many should we check?
            assert result['attribute_value']

    # Q01.FR.06
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "TransactionEvent",
        {"eventType": "Ended", "transactionInfo": {
            "stoppedReason": "ReqEnergyTransferRejected"}, "triggerReason": "AbnormalCondition"},
    )


@pytest.mark.asyncio
@pytest.mark.xdist_group(name="ISO15118")
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config("everest-config-ocpp201-sil-dc-d20-eim.yaml")
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_q02_no_service_renegotiation(
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    Q02
    ...
    """

    log.info(
        "##################### Q02: Starting in operationMode ChargingOnly before enabling V2X #################"
    )
    id_token = IdTokenType(id_token="8BADF00D",
                           type="ISO14443")

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(wait_for_bootnotification=True)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "StatusNotification",
        call21.StatusNotification(
            1,  ConnectorStatusEnumType.available, 1, datetime.now().isoformat()
        ),
        validate_status_notification_201,
    )
    test_controller.plug_in_dc_iso()

    # Make sure we don't start BPT
    @on(Action.authorize)
    def on_authorize(**kwargs):
        return call_result21.Authorize(
            id_token_info=IdTokenInfoType(
                status=AuthorizationStatusEnumType.accepted,
                group_id_token=IdTokenType(
                    id_token="12345", type="Central"
                ),
            ),
            allowed_energy_transfer=[EnergyTransferModeEnumType.dc]
        )

    setattr(charge_point_v21, "on_authorize", on_authorize)
    central_system_v21.chargepoint.route_map = create_route_map(
        central_system_v21.chargepoint
    )
    test_controller.swipe(id_token.id_token)
    r: call21.TransactionEvent = call21.TransactionEvent(
        **await wait_for_and_validate(
            test_utility,
            charge_point_v21,
            "TransactionEvent",
            {"eventType": "Started"},
        )
    )

    transaction = TransactionType(**r.transaction_info)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "NotifyEVChargingNeeds",
        {"evseId": 1, "requestedEnergyTransfer": "DC",
            "controlMode": "DynamicControl", "mobilityNeedsMode": "EVCC"},
        validate_notify_ev_charging_needs
    )
    r: call_result21.NotifyAllowedEnergyTransfer = await charge_point_v21.notify_allowed_energy_transfer_request(allowed_energy_transfer=[EnergyTransferModeEnumType.dc_bpt], transaction_id=transaction.transaction_id)
    # TODO(mlitre): Once service renegotiation is supported expect Accepted instead of rejected
    assert r.status == NotifyAllowedEnergyTransferStatusEnumType.rejected
