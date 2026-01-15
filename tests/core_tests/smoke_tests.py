#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
import asyncio
from datetime import datetime, timezone
from unittest.mock import Mock

from everest.testing.core_utils.common import Requirement
from everest.testing.core_utils.fixtures import *
from everest.testing.core_utils.controller.test_controller_interface import (
    TestController,
)
from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.probe_module import ProbeModule


async def wait_for_session_events(mock, expected_events, timeout=30):
    """Wait for specific events to appear in the mock's call list in the exact order."""
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
            return events

        await asyncio.sleep(0.1)

    raise TimeoutError(f"Timeout waiting for events {expected_events} in order. Got: {events}")


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


def setup_session_event_monitoring(probe_module: ProbeModule, connection_id: str):
    """Subscribe to session events from a connection. Returns session_event_mock."""
    session_event_mock = Mock()
    probe_module.subscribe_variable(connection_id, "session_event", session_event_mock)
    return session_event_mock


def setup_error_monitoring(probe_module: ProbeModule, connection_id: str):
    """Subscribe to error events from a connection. Returns error_raised_mock and error_cleared_mock."""
    error_raised_mock = Mock()
    error_cleared_mock = Mock()
    probe_module.subscribe_all_errors(
        connection_id, error_raised_mock, error_cleared_mock
    )
    return error_raised_mock, error_cleared_mock


@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={"evse_manager": [Requirement("connector_1", "evse")]}
)
@pytest.mark.everest_core_config("config-sil.yaml")
async def test_pwm_ac_session(
    test_controller: TestController, everest_core: EverestCore
):
    """
    Test session events of a basic PWM AC charging session.
    """
    probe_module = await setup_probe_module(test_controller, everest_core)
    session_event_mock = setup_session_event_monitoring(probe_module, "evse_manager")

    expected_events = [
        "SessionStarted",
        "AuthRequired",
        "Authorized",
        "TransactionStarted",
        "PrepareCharging",
        "ChargingStarted",
    ]

    test_controller.plug_in()
    await wait_for_session_events(session_event_mock, expected_events)

    test_controller.plug_out()
    await wait_for_session_events(
        session_event_mock, ["TransactionFinished", "SessionFinished"]
    )


@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={"evse_manager": [Requirement("connector_1", "evse")]}
)
@pytest.mark.everest_core_config("config-sil.yaml")
async def test_iso15118_ac_session(
    test_controller: TestController, everest_core: EverestCore
):
    """
    Test session events of an ISO 15118 AC charging session.
    """
    probe_module = await setup_probe_module(test_controller, everest_core)
    session_event_mock = setup_session_event_monitoring(probe_module, "evse_manager")

    expected_events = [
        "SessionStarted",
        "AuthRequired",
        "Authorized",
        "TransactionStarted",
        "PrepareCharging",
        "ChargingStarted",
    ]

    test_controller.plug_in_ac_iso()
    await wait_for_session_events(session_event_mock, expected_events)

    test_controller.plug_out()
    await wait_for_session_events(
        session_event_mock, ["TransactionFinished", "SessionFinished"]
    )


@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={"evse_manager": [Requirement("evse_manager", "evse")]}
)
@pytest.mark.everest_core_config("config-sil-dc.yaml")
async def test_iso15118_dc_session(
    test_controller: TestController, everest_core: EverestCore
):
    """
    Test session events of an ISO 15118 DC charging session.
    """

    probe_module = await setup_probe_module(test_controller, everest_core)

    session_event_mock = setup_session_event_monitoring(probe_module, "evse_manager")

    expected_events = [
        "SessionStarted",
        "AuthRequired",
        "Authorized",
        "TransactionStarted",
        "PrepareCharging",
        "ChargingStarted",
    ]

    test_controller.plug_in_dc_iso()
    await wait_for_session_events(session_event_mock, expected_events)

    test_controller.plug_out()
    await wait_for_session_events(
        session_event_mock, ["TransactionFinished", "SessionFinished"]
    )


@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={"evse_manager": [Requirement("evse_manager", "evse")]}
)
@pytest.mark.everest_core_config("config-sil-dc.yaml")
async def test_iso15118_dc_session_error_before_session(
    test_controller: TestController, everest_core: EverestCore
):
    """
    Test session events of an ISO 15118 DC charging session with an error before the session.
    """

    probe_module = await setup_probe_module(test_controller, everest_core)
    session_event_mock = setup_session_event_monitoring(probe_module, "evse_manager")
    error_raised_mock, error_cleared_mock = setup_error_monitoring(
        probe_module, "evse_manager"
    )

    test_controller.raise_error("MREC2GroundFailure")

    # Verify that error was raised
    await wait_for_error(error_raised_mock)
    assert error_raised_mock.called, "Error should have been raised"

    test_controller.plug_in_dc_iso()

    # wait for any session events for a short time
    await asyncio.sleep(10)

    assert (
        session_event_mock.call_count == 0
    ), "No session events should occur while error is active"
