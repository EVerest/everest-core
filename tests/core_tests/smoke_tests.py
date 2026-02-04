#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
import asyncio
from datetime import datetime, timezone
from unittest.mock import Mock
from copy import deepcopy
from typing import Dict

from everest.testing.core_utils.common import Requirement
from everest.testing.core_utils.fixtures import *
from everest.testing.core_utils.controller.test_controller_interface import (
    TestController,
)
from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.probe_module import ProbeModule


class NetworkInterfaceConfigAdjustmentStrategy(EverestConfigAdjustmentStrategy):
    """
    Adjustment strategy to set network interface configuration
    """

    def __init__(self, interface_name: str):
        self.interface_name = interface_name

    def adjust_everest_configuration(self, everest_config: Dict):
        adjusted_config = deepcopy(everest_config)
        adjusted_config["active_modules"]["iso15118_charger"]["config_module"]["device"] = self.interface_name
        adjusted_config["active_modules"]["iso15118_car"]["config_module"]["device"] = self.interface_name
        return adjusted_config

class DcConfigAdjustmentStrategy(EverestConfigAdjustmentStrategy):
    """
    Adjustment strategy to disable DIN SPEC 70121 module
    """

    def __init__(self, zero_power_ignore_pause: bool = False):
        self.zero_power_ignore_pause = zero_power_ignore_pause

    def adjust_everest_configuration(self, everest_config: Dict):
        adjusted_config = deepcopy(everest_config)
        adjusted_config["active_modules"]["iso15118_car"]["config_module"]["supported_DIN70121"] = False
        adjusted_config["active_modules"]["evse_manager"]["config_module"]["hack_allow_bpt_with_iso2"] = False
        adjusted_config["active_modules"]["powersupply_dc"]["config_implementation"] = {"main": {"min_current": 0}}
        adjusted_config["active_modules"]["evse_manager"]["config_module"]["zero_power_ignore_pause"] = self.zero_power_ignore_pause
        return adjusted_config

async def wait_for_session_events(mock, expected_events, timeout=30):
    """Wait for specific events to appear in the mock's call list in the exact order.
    
    After successfully matching events, the mock is automatically reset so subsequent
    calls will only see new events. This allows checking for the same event multiple times.
    """
    start_time = asyncio.get_event_loop().time()

    while asyncio.get_event_loop().time() - start_time < timeout:
        events = []
        for call in mock.call_args_list:
            event_data = call[0][0]
            event_type = event_data.get("event")
            events.append(event_type)

        # Check if expected_events appear in order (as a subsequence)
        expected_idx = 0
        for event in events:
            if expected_idx < len(expected_events) and event == expected_events[expected_idx]:
                expected_idx += 1
        
        if expected_idx == len(expected_events):
            mock.reset_mock()
            return

        await asyncio.sleep(0.1)

    raise TimeoutError(f"Timeout waiting for events {expected_events} in order. Got: {events}")

class EnergyVerificationMode:
    STAY_BELOW = 1
    MUST_EXCEED = 2

async def assert_energy_delivery(mock, energy_threshold_wh, verify_mode: EnergyVerificationMode = EnergyVerificationMode.MUST_EXCEED, timeout=30):
    """Assert that the energy delivered (in Wh) is within the specified bounds within the timeout period.
       If verify_mode is EnergyVerificationMode.STAY_BELOW, the energy must stay below energy_threshold_wh for the entire timeout.
       If verify_mode is EnergyVerificationMode.MUST_EXCEED, the energy must exceed energy_threshold_wh within the timeout and 
       the function returns immediately when that happens.
    """

    start_time = asyncio.get_event_loop().time()

    while asyncio.get_event_loop().time() - start_time < timeout:
        energy_wh = None
        for call in mock.call_args_list:
            powermeter_data = call[0][0]
            energy_wh = powermeter_data.get("energy_Wh_import").get("total")

        if energy_wh is not None:
            if verify_mode == EnergyVerificationMode.STAY_BELOW:
                if energy_wh > energy_threshold_wh:
                    raise AssertionError(
                        f"Energy delivered {energy_wh}Wh exceeded threshold of {energy_threshold_wh}Wh."
                    )
            else:
                if energy_wh >= energy_threshold_wh:
                    return  # Success

        await asyncio.sleep(0.1)

    if verify_mode == EnergyVerificationMode.MUST_EXCEED:
        raise TimeoutError(
            f"Timeout waiting for energy delivered to exceed {energy_threshold_wh}Wh. Last known: {energy_wh}Wh."
        )

async def assert_no_events(mock, excluded_events, wait_time=2):
    """Wait for a period and verify that certain events do NOT occur.
    
    Args:
        mock: The mock object tracking events
        excluded_events: List of event types that should NOT appear
        wait_time: Time to wait in seconds before checking (default 2)
    """
    await asyncio.sleep(wait_time)
    
    events = []
    for call in mock.call_args_list:
        event_data = call[0][0]
        event_type = event_data.get("event")
        events.append(event_type)
    
    # Check if any excluded events appeared
    found_excluded = [event for event in events if event in excluded_events]
    
    if found_excluded:
        raise AssertionError(
            f"Events {found_excluded} should not have occurred. All events: {events}"
        )
    
    # Reset mock after checking so subsequent calls start fresh
    mock.reset_mock()


async def wait_for_ready(mock, timeout=5):
    """Wait until the ready mock has been called."""
    start_time = asyncio.get_event_loop().time()

    while asyncio.get_event_loop().time() - start_time < timeout:
        if mock.call_count > 0:
            return
        await asyncio.sleep(0.1)

    raise TimeoutError("Timeout waiting for ready signal.")


async def wait_for_error(mock, timeout=5):
    """Wait until the error mock has been called."""
    start_time = asyncio.get_event_loop().time()

    while asyncio.get_event_loop().time() - start_time < timeout:
        if mock.call_count > 0:
            return
        await asyncio.sleep(0.1)

    raise TimeoutError("Timeout waiting for error signal.")


async def setup_probe_module(
    test_controller: TestController, everest_core: EverestCore
):
    """Initialize test controller and probe module, wait for ready. Returns probe_module."""
    test_controller.start()
    probe_module = ProbeModule(everest_core.get_runtime_session())

    ready_mock = Mock()
    probe_module.subscribe_variable("evse_manager", "ready", ready_mock)

    probe_module.start()
    await probe_module.wait_to_be_ready()
    await wait_for_ready(ready_mock, timeout=5)

    return probe_module


async def setup_session_mocks(
    test_controller: TestController,
    everest_core: EverestCore,
    connection_id: str = "evse_manager",
):
    """Setup probe module and session monitoring. Returns (probe_module, session_event_mock, powermeter_mock)."""
    probe_module = await setup_probe_module(test_controller, everest_core)
    session_event_mock, powermeter_mock = setup_evse_manager_monitoring(
        probe_module, connection_id
    )
    return probe_module, session_event_mock, powermeter_mock


def setup_evse_manager_monitoring(probe_module: ProbeModule, connection_id: str):
    """Subscribe to session events from a connection. Returns session_event_mock."""
    session_event_mock = Mock()
    powermeter_mock = Mock()
    probe_module.subscribe_variable(connection_id, "session_event", session_event_mock)
    probe_module.subscribe_variable(connection_id, "powermeter", powermeter_mock)
    return session_event_mock, powermeter_mock


def setup_error_monitoring(probe_module: ProbeModule, connection_id: str):
    """Subscribe to error events from a connection. Returns error_raised_mock and error_cleared_mock."""
    error_raised_mock = Mock()
    error_cleared_mock = Mock()
    probe_module.subscribe_all_errors(
        connection_id, error_raised_mock, error_cleared_mock
    )
    return error_raised_mock, error_cleared_mock

async def set_external_limits(probe_module: ProbeModule, module_id: int, import_limit: int, export_limit: int):
    await probe_module.call_command(
        module_id,
        "set_external_limits",
        {
            "value": {
                "schedule_import": [
                    {
                        "timestamp": datetime.now(timezone.utc).isoformat(),
                        "limits_to_leaves": {
                            "total_power_W": {"value": import_limit, "source": "test"},
                            "ac_max_current_A": {"value": import_limit / 230 / 3, "source": "test"},
                        },
                        "limits_to_root": {
                            "total_power_W": {"value": export_limit, "source": "test"},
                            "ac_max_current_A": {"value": export_limit / 230 / 3, "source": "test"},
                        },
                    }
                ],
                "schedule_export": [],
                "schedule_setpoints": []
            }
        },
    )


async def assert_energy_exceeds(powermeter_mock, energy_threshold_wh: int, timeout: int = 30):
    await assert_energy_delivery(
        powermeter_mock,
        energy_threshold_wh=energy_threshold_wh,
        verify_mode=EnergyVerificationMode.MUST_EXCEED,
        timeout=timeout,
    )


async def assert_energy_below(powermeter_mock, energy_threshold_wh: int, timeout: int = 30):
    await assert_energy_delivery(
        powermeter_mock,
        energy_threshold_wh=energy_threshold_wh,
        verify_mode=EnergyVerificationMode.STAY_BELOW,
        timeout=timeout,
    )


async def start_session(
    test_controller: TestController,
    session_event_mock,
    plug_in_callable,
    start_sequence=None,
):
    if start_sequence is None:
        start_sequence = BASIC_SESSION_START_SEQUENCE
    plug_in_callable()
    await wait_for_session_events(session_event_mock, start_sequence)


async def end_session(test_controller: TestController, session_event_mock):
    test_controller.plug_out()
    await wait_for_session_events(session_event_mock, SESSION_END_EVENTS)


# Common event sequences
BASIC_SESSION_START_SEQUENCE = [
    "SessionStarted",
    "AuthRequired",
    "Authorized",
    "TransactionStarted",
    "PrepareCharging",
    "ChargingStarted",
]

NO_ENERGY_SESSION_START_SEQUENCE = [
    "SessionStarted",
    "AuthRequired",
    "Authorized",
    "TransactionStarted",
    "PrepareCharging"
]

SESSION_END_EVENTS = ["TransactionFinished", "SessionFinished"]


async def run_basic_session(test_controller: TestController, session_event_mock, powermeter_mock, plug_in_method: str):
    """Run a complete basic charging session and verify events."""
    getattr(test_controller, plug_in_method)()
    await wait_for_session_events(session_event_mock, BASIC_SESSION_START_SEQUENCE)

    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=5, timeout=30)
    
    await end_session(test_controller, session_event_mock)

@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={"evse_manager": [Requirement("connector_1", "evse")]}
)
@pytest.mark.everest_core_config("config-sil.yaml")
async def test_pwm_ac_session(
    test_controller: TestController, everest_core: EverestCore
):
    """Test session events of a basic PWM AC charging session."""
    _, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )
    
    await run_basic_session(test_controller, session_event_mock, powermeter_mock, "plug_in")


@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={"evse_manager": [Requirement("connector_1", "evse")]}
)
@pytest.mark.everest_core_config("config-sil.yaml")
async def test_iso15118_ac_session(
    test_controller: TestController, everest_core: EverestCore
):
    """Test session events of an ISO 15118 AC charging session."""
    _, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )
    
    await run_basic_session(test_controller, session_event_mock, powermeter_mock, "plug_in_ac_iso")


@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={"evse_manager": [Requirement("evse_manager", "evse")]}
)
@pytest.mark.everest_core_config("config-sil-dc.yaml")
@pytest.mark.everest_config_adaptions(DcConfigAdjustmentStrategy())
async def test_iso15118_dc_session(
    test_controller: TestController, everest_core: EverestCore
):
    """Test session events of an ISO 15118 DC charging session."""
    _, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )
    
    await run_basic_session(test_controller, session_event_mock, powermeter_mock, "plug_in_dc_iso")

@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={"evse_manager": [Requirement("evse_manager", "evse")]}
)
@pytest.mark.everest_core_config("config-sil-dc.yaml")
@pytest.mark.everest_config_adaptions(DcConfigAdjustmentStrategy())
async def test_iso15118_dc_session_error_before_session(
    test_controller: TestController, everest_core: EverestCore
):
    """
    Test session events of an ISO 15118 DC charging session with an error before the session.
    """

    probe_module = await setup_probe_module(test_controller, everest_core)
    session_event_mock, _ = setup_evse_manager_monitoring(
        probe_module, "evse_manager"
    )
    error_raised_mock, _ = setup_error_monitoring(
        probe_module, "evse_manager"
    )

    test_controller.raise_error("MREC2GroundFailure")

    # Verify that error was raised
    await wait_for_error(error_raised_mock)
    assert error_raised_mock.called, "Error should have been raised"

    test_controller.plug_in_dc_iso()

    assert_no_events(session_event_mock, ["SessionStarted"], wait_time=10)
    assert (
        session_event_mock.call_count == 0
    ), "No session events should occur while error is active"

###########################################################
################ Pause and No Energy Tests ################
###########################################################

@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={
        "evse_manager": [Requirement("connector_1", "evse")],
        "gcp": [Requirement("grid_connection_point", "external_limits")],
    }
)
@pytest.mark.everest_core_config("config-sil.yaml")
async def test_pwm_ac_session_no_energy_before_session(
    test_controller: TestController, everest_core: EverestCore
):
    """Test PWM AC charging session with no energy at the start."""
    probe_module, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )
    
    await set_external_limits(probe_module, "gcp", 0, 0)
    
    await start_session(
        test_controller,
        session_event_mock,
        test_controller.plug_in,
        start_sequence=NO_ENERGY_SESSION_START_SEQUENCE,
    )

    await assert_energy_below(powermeter_mock, energy_threshold_wh=0, timeout=10)
    
    await set_external_limits(probe_module, "gcp", 10000, 10000)
    await wait_for_session_events(session_event_mock, ["ChargingStarted"])

    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=5, timeout=30)
    
    await end_session(test_controller, session_event_mock)


@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={
        "evse_manager": [Requirement("connector_1", "evse")],
        "gcp": [Requirement("grid_connection_point", "external_limits")],
    }
)
@pytest.mark.everest_core_config("config-sil.yaml")
async def test_iso15118_ac_session_no_energy_before_session(
    test_controller: TestController, everest_core: EverestCore
):
    """Test ISO 15118 AC charging session with no energy at the start."""
    probe_module, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )

    await set_external_limits(probe_module, "gcp", 0, 0)
    # even with no energy we go should go to Chargeloop
    await start_session(test_controller, session_event_mock, test_controller.plug_in_ac_iso)
    await assert_energy_below(powermeter_mock, energy_threshold_wh=0, timeout=10)
    await set_external_limits(probe_module, "gcp", 10000, 10000)
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=5, timeout=30)
    await end_session(test_controller, session_event_mock)


@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={
        "evse_manager": [Requirement("evse_manager", "evse")],
        "gcp": [Requirement("grid_connection_point", "external_limits")]
        # FIXME: "iso15118": [Requirement("iso15118_charger", "evse")]
    }
)
@pytest.mark.everest_core_config("config-sil-dc.yaml")
@pytest.mark.everest_config_adaptions(DcConfigAdjustmentStrategy())
async def test_iso15118_dc_session_no_energy_before_session(
    test_controller: TestController, everest_core: EverestCore
):
    """Test ISO 15118 DC charging session with no energy at the start."""
    probe_module, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )

    await set_external_limits(probe_module, "gcp", 0, 0)
    await start_session(test_controller, session_event_mock, test_controller.plug_in_dc_iso, start_sequence=NO_ENERGY_SESSION_START_SEQUENCE)
    
    # current_demand_started
    # start_pre_charge
    # start_cable_check
    # v2g_setup_finish (after PowerDelivery)
    
    # verify current demand has not started!

    await assert_energy_below(powermeter_mock, energy_threshold_wh=5, timeout=15)
    await set_external_limits(probe_module, "gcp", 10000, 10000)
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=20, timeout=30)
    await end_session(test_controller, session_event_mock)




@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={
        "evse_manager": [Requirement("evse_manager", "evse")],
        "gcp": [Requirement("grid_connection_point", "external_limits")],
    }
)
@pytest.mark.everest_core_config("config-sil-dc.yaml")
@pytest.mark.everest_config_adaptions(DcConfigAdjustmentStrategy(zero_power_ignore_pause=True))
async def test_iso15118_dc_session_no_energy_before_session_no_pause(
    test_controller: TestController, everest_core: EverestCore
):
    """
    Test ISO 15118 DC charging session with no energy at the start. zero_power_ignore_pause=True
    so the session should start normally and go into CurrentDemand
    """
    probe_module, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )

    await set_external_limits(probe_module, "gcp", 0, 0)
    # even with no energy we go should go to Chargeloop
    await start_session(test_controller, session_event_mock, test_controller.plug_in_dc_iso)
    # Allow 5Wh for cable check
    await assert_energy_below(powermeter_mock, energy_threshold_wh=5, timeout=15)
    await set_external_limits(probe_module, "gcp", 10000, 10000)
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=20, timeout=30)
    await end_session(test_controller, session_event_mock)

@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={
        "evse_manager": [Requirement("connector_1", "evse")],
        "gcp": [Requirement("grid_connection_point", "external_limits")],
    }
)
@pytest.mark.everest_core_config("config-sil.yaml")
async def test_pwm_ac_session_no_energy_during_session(
    test_controller: TestController, everest_core: EverestCore
):
    """Test PWM AC charging session where energy is removed and restored during charging."""
    probe_module, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )
    
    await start_session(test_controller, session_event_mock, test_controller.plug_in)
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=10, timeout=15)
    await set_external_limits(probe_module, "gcp", 0, 0)
    await wait_for_session_events(session_event_mock, ["ChargingPausedEVSE"])
    await assert_no_events(session_event_mock, ["ChargingStarted", "ChargingResumed"], wait_time=5)
    await assert_energy_below(powermeter_mock, energy_threshold_wh=15, timeout=5)
    await set_external_limits(probe_module, "gcp", 10000, 10000)
    await wait_for_session_events(session_event_mock, ["PrepareCharging","ChargingStarted"])
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=15, timeout=15)
    await end_session(test_controller, session_event_mock)


@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={
        "evse_manager": [Requirement("connector_1", "evse")],
        "gcp": [Requirement("grid_connection_point", "external_limits")],
    }
)
@pytest.mark.everest_core_config("config-sil.yaml")
# FIXME: Fails because CarSim stays in C and we simply turn off PWM
async def test_iso15118_ac_session_no_energy_during_session(
    test_controller: TestController, everest_core: EverestCore
):
    """Test ISO 15118 AC charging session where energy is removed and restored during charging."""
    probe_module, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )
    
    await start_session(test_controller, session_event_mock, test_controller.plug_in_ac_iso)
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=10, timeout=15)
    await set_external_limits(probe_module, "gcp", 0, 0)
    await assert_no_events(session_event_mock, ["ChargingStarted", "ChargingResumed"], wait_time=5)
    await assert_energy_below(powermeter_mock, energy_threshold_wh=15, timeout=5)
    await set_external_limits(probe_module, "gcp", 10000, 10000)
    await wait_for_session_events(session_event_mock, ["ChargingStarted"])
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=15, timeout=15)
    await end_session(test_controller, session_event_mock)

@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={
        "evse_manager": [Requirement("evse_manager", "evse")],
        "gcp": [Requirement("grid_connection_point", "external_limits")],
    }
)
@pytest.mark.everest_core_config("config-sil-dc.yaml")
@pytest.mark.everest_config_adaptions(DcConfigAdjustmentStrategy())
# FIXME: EV Simulator doesnt apply the ISO15118 limits from the EVSE
async def test_iso15118_dc_session_no_energy_during_session(
    test_controller: TestController, everest_core: EverestCore
):
    """Test ISO 15118 DC charging session where energy is removed and restored during charging."""
    probe_module, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )
    
    await start_session(test_controller, session_event_mock, test_controller.plug_in_dc_iso)
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=10, timeout=15)
    await set_external_limits(probe_module, "gcp", 0, 0)
    await assert_energy_below(powermeter_mock, energy_threshold_wh=15, timeout=5)
    await set_external_limits(probe_module, "gcp", 10000, 10000)
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=15, timeout=15)
    
    await end_session(test_controller, session_event_mock)

@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={
        "evse_manager": [Requirement("connector_1", "evse")],
        "gcp": [Requirement("grid_connection_point", "external_limits")],
    }
)
@pytest.mark.everest_core_config("config-sil.yaml")
async def test_pwm_ac_session_paused_by_ev(
    test_controller: TestController, everest_core: EverestCore
):
    """Test PWM AC charging session paused by EV."""
    _, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )
    
    await start_session(test_controller, session_event_mock, test_controller.plug_in)
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=10, timeout=15)
    test_controller.pause_session()
    await wait_for_session_events(session_event_mock, ["ChargingPausedEV"])
    await assert_energy_below(powermeter_mock, energy_threshold_wh=15, timeout=5)
    await assert_no_events(session_event_mock, ["ChargingStarted", "ChargingResumed"], wait_time=5)
    test_controller.resume_session()
    await wait_for_session_events(session_event_mock, ["ChargingResumed"])
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=15, timeout=15)
    await end_session(test_controller, session_event_mock)

@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={
        "evse_manager": [Requirement("connector_1", "evse")],
        "gcp": [Requirement("grid_connection_point", "external_limits")],
    }
)
@pytest.mark.everest_core_config("config-sil.yaml")
async def test_iso15118_ac_session_paused_by_ev(
    test_controller: TestController, everest_core: EverestCore
):
    """
    Test session events of a basic ISO 15118 AC charging session with session paused by EV.
    """

    _, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )

    await start_session(test_controller, session_event_mock, test_controller.plug_in_ac_iso)

    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=10, timeout=15)

    test_controller.pause_iso_session()
    await wait_for_session_events(session_event_mock, ["ChargingPausedEV"])

    await assert_energy_below(powermeter_mock, energy_threshold_wh=20, timeout=5)

    await assert_no_events(session_event_mock, ["ChargingStarted", "ChargingResumed"], wait_time=5)

    test_controller.resume_iso_session_ac()
    await wait_for_session_events(session_event_mock, ["ChargingStarted"])

    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=20, timeout=15)

    await end_session(test_controller, session_event_mock)

@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={
        "evse_manager": [Requirement("evse_manager", "evse")],
        "gcp": [Requirement("grid_connection_point", "external_limits")],
    }
)
@pytest.mark.everest_core_config("config-sil-dc.yaml")
@pytest.mark.everest_config_adaptions(DcConfigAdjustmentStrategy())
async def test_iso15118_dc_session_paused_by_ev(
    test_controller: TestController, everest_core: EverestCore
):
    """
    Test session events of a basic ISO 15118 DC charging session with session paused by EV.
    """

    _, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )

    await start_session(test_controller, session_event_mock, test_controller.plug_in_dc_iso)

    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=10, timeout=15)

    test_controller.pause_iso_session()
    await wait_for_session_events(session_event_mock, ["ChargingPausedEV"])

    await assert_energy_below(powermeter_mock, energy_threshold_wh=20, timeout=5)

    await assert_no_events(session_event_mock, ["ChargingStarted", "ChargingResumed"], wait_time=5)

    test_controller.resume_iso_session_dc()

    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=20, timeout=45)

    await wait_for_session_events(session_event_mock, ["ChargingStarted"])

    await end_session(test_controller, session_event_mock)

@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={
        "evse_manager": [Requirement("connector_1", "evse")],
        "gcp": [Requirement("grid_connection_point", "external_limits")],
    }
)
@pytest.mark.everest_core_config("config-sil.yaml")
async def test_pwm_ac_session_paused_by_evse(
    test_controller: TestController, everest_core: EverestCore
):
    """
    Test session events of a basic PWM AC charging session with session paused by EVSE.
    """

    probe_module, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )

    await start_session(test_controller, session_event_mock, test_controller.plug_in)
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=10, timeout=15)

    await probe_module.call_command(
        "evse_manager",
        "pause_charging",
        {},
    )

    await wait_for_session_events(session_event_mock, ["ChargingPausedEVSE"])
    await assert_energy_below(powermeter_mock, energy_threshold_wh=15, timeout=5)

    await assert_no_events(session_event_mock, ["ChargingStarted", "ChargingResumed"], wait_time=5)

    await probe_module.call_command(
        "evse_manager",
        "resume_charging",
        {},
    )

    await wait_for_session_events(session_event_mock, ["ChargingStarted"])
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=15, timeout=15)

    await end_session(test_controller, session_event_mock)

@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={
        "evse_manager": [Requirement("connector_1", "evse")],
        "gcp": [Requirement("grid_connection_point", "external_limits")],
    }
)
@pytest.mark.everest_core_config("config-sil.yaml")
async def test_iso15118_ac_session_paused_by_evse(
    test_controller: TestController, everest_core: EverestCore
):
    """
    Test session events of a basic ISO 15118 AC charging session with session paused by EVSE.
    """
    #FIXME: Fails with warning: Pause initialized by the charger is not supported in DIN70121 and ISO15118-2

    probe_module, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )

    await start_session(test_controller, session_event_mock, test_controller.plug_in_ac_iso)
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=10, timeout=15)

    await probe_module.call_command(
        "evse_manager",
        "pause_charging",
        {},
    )

    await wait_for_session_events(session_event_mock, ["ChargingPausedEVSE"])
    await assert_energy_below(powermeter_mock, energy_threshold_wh=15, timeout=5)

    await assert_no_events(session_event_mock, ["ChargingStarted", "ChargingResumed"], wait_time=5)

    await probe_module.call_command(
        "evse_manager",
        "resume_charging",
        {},
    )

    await wait_for_session_events(session_event_mock, ["ChargingStarted"])
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=15, timeout=15)

    await end_session(test_controller, session_event_mock)

@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={
        "evse_manager": [Requirement("evse_manager", "evse")],
        "gcp": [Requirement("grid_connection_point", "external_limits")],
    }
)
@pytest.mark.everest_core_config("config-sil-dc.yaml")
@pytest.mark.everest_config_adaptions(DcConfigAdjustmentStrategy())
async def test_iso15118_dc_session_paused_by_evse(
    test_controller: TestController, everest_core: EverestCore
):
    """
    Test session events of a basic ISO 15118 DC charging session paused by EVSE.
    """
    #FIXME: Fails with warning: Pause initialized by the charger is not supported in DIN70121 and ISO15118-2

    probe_module, session_event_mock, powermeter_mock = await setup_session_mocks(
        test_controller, everest_core
    )

    await start_session(test_controller, session_event_mock, test_controller.plug_in_dc_iso)
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=10, timeout=15)

    await probe_module.call_command(
        "evse_manager",
        "pause_charging",
        {},
    )

    await wait_for_session_events(session_event_mock, ["ChargingPausedEVSE"])
    await assert_energy_below(powermeter_mock, energy_threshold_wh=15, timeout=5)

    await assert_no_events(session_event_mock, ["ChargingStarted", "ChargingResumed"], wait_time=5)

    await probe_module.call_command(
        "evse_manager",
        "resume_charging",
        {},
    )

    await wait_for_session_events(session_event_mock, ["ChargingStarted"])
    await assert_energy_exceeds(powermeter_mock, energy_threshold_wh=15, timeout=15)

    await end_session(test_controller, session_event_mock)

