# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

"""
Integration smoke tests for OCPP 2.1 R04 - Configure DER control settings at Charging Station.

Covers the routing/availability behaviour end-to-end over a real websocket connection:
  * CS returns NotImplemented (CALLERROR) when DCDERCtrlr/ACDERCtrlr is not available
  * CS returns Accepted when DCDERCtrlr.Available=true and controlType is in ModesSupported
  * CS returns NotSupported when the controlType is not in ModesSupported
  * GetDERControl with no stored controls returns NotFound
  * ClearDERControl with no controls returns NotFound

These tests exercise the wiring done in libocpp: the DERControl functional block
is conditionally instantiated based on the DERCtrlr.Available variable, and
SetDERControl/GetDERControl/ClearDERControl messages are routed to it.
"""

import pytest
import logging

# fmt: off
from everest.testing.core_utils.controller.test_controller_interface import TestController

from ocpp.v21 import call as call21
from ocpp.v21 import call_result as call_result21
from ocpp.v21.enums import *
from ocpp.v21.datatypes import *
from ocpp.routing import on, create_route_map

from everest.testing.ocpp_utils.fixtures import *
from everest_test_utils import *  # must come before v21 datatype re-imports
from ocpp.v21.enums import Action, DERControlEnumType, DERControlStatusEnumType, DERUnitEnumType

from everest.testing.core_utils._configuration.libocpp_configuration_helper import GenericOCPP2XConfigAdjustment
from everest.testing.ocpp_utils.charge_point_utils import TestUtility, OcppTestConfiguration
# fmt: on

log = logging.getLogger("derControlTest")


# -----------------------------------------------------------------------------
# R04 - DER not available: SetDERControl is rejected as NotImplemented
# -----------------------------------------------------------------------------

@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            ),
        ]
    )
)
async def test_set_der_control_der_not_available_not_implemented(
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """R04: Without DCDERCtrlr/ACDERCtrlr Available, SetDERControl returns CALLERROR NotImplemented.

    The Python ocpp library suppresses CallErrors by default (returning None)
    rather than raising, so we assert that the response is None. The underlying
    CALLError is logged as a warning by the library.
    """
    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint()

    freq_droop = FreqDroopType(
        priority=0,
        over_freq=61.0,
        under_freq=59.0,
        over_droop=5.0,
        under_droop=5.0,
        response_time=3.0,
    )

    r = await charge_point_v21.set_der_control_req(
        is_default=True,
        control_id="ctrl-1",
        control_type=DERControlEnumType.freq_droop,
        freq_droop=freq_droop,
    )
    # CALLError NotImplemented is suppressed to None by the library
    assert r is None


# -----------------------------------------------------------------------------
# R04.FR.02 - DER available + supported controlType: Accepted
# -----------------------------------------------------------------------------

@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            ),
            (
                OCPP2XConfigVariableIdentifier(
                    "DCDERCtrlr_1", "DCDERCtrlrAvailable", "Actual"
                ),
                "true",
            ),
            # ModesSupported is ReadOnly; test relies on the default list in
            # DCDERCtrlr_1.json: FreqDroop,VoltWatt,LimitMaxDischarge,VoltVar,
            # FixedVar,EnterService,Gradients
        ]
    )
)
async def test_set_der_control_supported_type_accepted(
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """R04.FR.02: With DCDERCtrlr Available and FreqDroop in ModesSupported, set default FreqDroop returns Accepted."""
    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint()

    freq_droop = FreqDroopType(
        priority=0,
        over_freq=61.0,
        under_freq=59.0,
        over_droop=5.0,
        under_droop=5.0,
        response_time=3.0,
    )

    r: call_result21.SetDERControl = await charge_point_v21.set_der_control_req(
        is_default=True,
        control_id="ctrl-default-fd",
        control_type=DERControlEnumType.freq_droop,
        freq_droop=freq_droop,
    )
    assert r.status == DERControlStatusEnumType.accepted


# -----------------------------------------------------------------------------
# R04.FR.01 - DER available but controlType not in ModesSupported: NotSupported
# -----------------------------------------------------------------------------

@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            ),
            (
                OCPP2XConfigVariableIdentifier(
                    "DCDERCtrlr_1", "DCDERCtrlrAvailable", "Actual"
                ),
                "true",
            ),
            (
                OCPP2XConfigVariableIdentifier(
                    "DCDERCtrlr_1", "DCDERCtrlrModesSupported", "Actual"
                ),
                # HFMustTrip is intentionally NOT in this list
                "FreqDroop,VoltWatt",
            ),
        ]
    )
)
async def test_set_der_control_unsupported_type_not_supported(
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """R04.FR.01: With DCDERCtrlr Available but controlType not in ModesSupported, returns NotSupported.

    HFMustTrip is intentionally NOT in the default ModesSupported list
    (DCDERCtrlr_1.json), so this should return NotSupported.
    """
    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint()

    # HFMustTrip curve with yUnit=Not_Applicable (valid shape, unsupported type).
    # NOTE: StrEnum auto-converts "Not_Applicable" -> Python attr `not__applicable` (double underscore).
    curve = DERCurveType(
        curve_data=[DERCurvePointsType(x=62.0, y=1.0)],
        priority=0,
        y_unit=DERUnitEnumType.not__applicable,
    )

    r: call_result21.SetDERControl = await charge_point_v21.set_der_control_req(
        is_default=True,
        control_id="ctrl-hf-trip",
        control_type=DERControlEnumType.hf_must_trip,
        curve=curve,
    )
    assert r.status == DERControlStatusEnumType.not_supported


# -----------------------------------------------------------------------------
# R04.FR.30 - GetDERControl with no stored controls: NotFound
# -----------------------------------------------------------------------------

@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            ),
            (
                OCPP2XConfigVariableIdentifier(
                    "DCDERCtrlr_1", "DCDERCtrlrAvailable", "Actual"
                ),
                "true",
            ),
            # ModesSupported is ReadOnly; test relies on the default list in
            # DCDERCtrlr_1.json: FreqDroop,VoltWatt,LimitMaxDischarge,VoltVar,
            # FixedVar,EnterService,Gradients
        ]
    )
)
async def test_get_der_control_no_controls_not_found(
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """R04.FR.30: GetDERControl when no matching controls exist returns NotFound."""
    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint()

    r: call_result21.GetDERControl = await charge_point_v21.get_der_control_req(
        request_id=42,
        control_type=DERControlEnumType.freq_droop,
    )
    assert r.status == DERControlStatusEnumType.not_found


# -----------------------------------------------------------------------------
# R04.FR.41 - ClearDERControl with no matching controls: NotFound
# -----------------------------------------------------------------------------

@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            ),
            (
                OCPP2XConfigVariableIdentifier(
                    "DCDERCtrlr_1", "DCDERCtrlrAvailable", "Actual"
                ),
                "true",
            ),
            (
                OCPP2XConfigVariableIdentifier(
                    "DCDERCtrlr_1", "DCDERCtrlrModesSupported", "Actual"
                ),
                "FreqDroop,VoltWatt",
            ),
        ]
    )
)
async def test_clear_der_control_no_match_not_found(
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """R04.FR.41: ClearDERControl when no matching controls exist returns NotFound."""
    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint()

    r: call_result21.ClearDERControl = await charge_point_v21.clear_der_control_req(
        is_default=True,
        control_type=DERControlEnumType.freq_droop,
    )
    assert r.status == DERControlStatusEnumType.not_found


# -----------------------------------------------------------------------------
# TC_R_102 end-to-end - Clearing controlTypes
# (R04.FR.02, FR.30, FR.33, FR.41, FR.44, FR.45)
#
# Exercises the cross-message state evolution that the unit tests can't:
# Set defaults of two distinct types, Clear-by-type one of them, verify the
# other survives, then Clear-all and verify the table is empty. Multi-row
# Report payload ordering itself is unit-tested in
# DERControlTest.GetDERControl_ReportOrdersMultiRowByIsSuperseded; this test
# locks in that the persisted state evolves correctly across messages.
# -----------------------------------------------------------------------------

@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            ),
            (
                OCPP2XConfigVariableIdentifier(
                    "DCDERCtrlr_1", "DCDERCtrlrAvailable", "Actual"
                ),
                "true",
            ),
            # ModesSupported is ReadOnly; relies on the default list in
            # DCDERCtrlr_1.json which includes both FreqDroop and VoltWatt.
        ]
    )
)
async def test_tc_r_102_clear_by_type_then_clear_all(
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """TC_R_102: Set two defaults of distinct types, clear one by type, verify
    the other survives, then clear all and verify the table is empty.
    """
    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint()

    # Step 1: SetDERControl default FreqDroop (R04.FR.02)
    freq_droop = FreqDroopType(
        priority=0,
        over_freq=61.0,
        under_freq=59.0,
        over_droop=5.0,
        under_droop=5.0,
        response_time=3.0,
    )
    r: call_result21.SetDERControl = await charge_point_v21.set_der_control_req(
        is_default=True,
        control_id="ctrl-default-fd",
        control_type=DERControlEnumType.freq_droop,
        freq_droop=freq_droop,
    )
    assert r.status == DERControlStatusEnumType.accepted

    # Step 2: SetDERControl default VoltWatt
    volt_watt_curve = DERCurveType(
        curve_data=[DERCurvePointsType(x=240.0, y=100.0)],
        priority=1,
        y_unit=DERUnitEnumType.pct_max_w,
    )
    r = await charge_point_v21.set_der_control_req(
        is_default=True,
        control_id="ctrl-default-vw",
        control_type=DERControlEnumType.volt_watt,
        curve=volt_watt_curve,
    )
    assert r.status == DERControlStatusEnumType.accepted

    # Step 3: GetDERControl FreqDroop -> Accepted (the row exists)
    r_get: call_result21.GetDERControl = await charge_point_v21.get_der_control_req(
        request_id=1,
        is_default=True,
        control_type=DERControlEnumType.freq_droop,
    )
    assert r_get.status == DERControlStatusEnumType.accepted

    # Step 4: GetDERControl VoltWatt -> Accepted (the row exists)
    r_get = await charge_point_v21.get_der_control_req(
        request_id=2,
        is_default=True,
        control_type=DERControlEnumType.volt_watt,
    )
    assert r_get.status == DERControlStatusEnumType.accepted

    # Step 5: ClearDERControl by type=FreqDroop (R04.FR.45)
    r_clear: call_result21.ClearDERControl = (
        await charge_point_v21.clear_der_control_req(
            is_default=True,
            control_type=DERControlEnumType.freq_droop,
        )
    )
    assert r_clear.status == DERControlStatusEnumType.accepted

    # Step 6: GetDERControl FreqDroop -> NotFound (the row was cleared, R04.FR.30)
    r_get = await charge_point_v21.get_der_control_req(
        request_id=3,
        is_default=True,
        control_type=DERControlEnumType.freq_droop,
    )
    assert r_get.status == DERControlStatusEnumType.not_found

    # Step 7: GetDERControl VoltWatt -> Accepted (untouched by the targeted clear)
    r_get = await charge_point_v21.get_der_control_req(
        request_id=4,
        is_default=True,
        control_type=DERControlEnumType.volt_watt,
    )
    assert r_get.status == DERControlStatusEnumType.accepted

    # Step 8: ClearDERControl with no controlType / controlId,
    # isDefault=true -> clear ALL matching defaults (R04.FR.44)
    r_clear = await charge_point_v21.clear_der_control_req(is_default=True)
    assert r_clear.status == DERControlStatusEnumType.accepted

    # Step 9: GetDERControl any default -> NotFound (table is empty)
    r_get = await charge_point_v21.get_der_control_req(
        request_id=5,
        is_default=True,
        control_type=DERControlEnumType.volt_watt,
    )
    assert r_get.status == DERControlStatusEnumType.not_found
