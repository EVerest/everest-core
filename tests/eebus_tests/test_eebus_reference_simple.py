# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

"""
EEBUS Reference Control Box Integration Test

End-to-end test that starts the eebus-go reference control box alongside EVerest
and verifies that the EEBUS module successfully connects and receives a power
consumption limit from the control box via the EEBUS/SHIP protocol.
"""

import asyncio
import time

import pytest

from everest.testing.core_utils.fixtures import *
from everest.testing.core_utils.everest_core import EverestCore

from fixtures.eebus_module_test import EebusTestProbeModule
from helpers.async_helpers import async_get

from conftest import ReferenceControlBox

# The EEBUS module config uses failsafe_control_limit_W = 4200 (default).
# Any limit with a different value must have come from the control box.
FAILSAFE_LIMIT_W = 4200.0


@pytest.mark.everest_core_config("config-test-eebus-reference.yaml")
@pytest.mark.probe_module
class TestEEBUSReference:
    """End-to-end tests against the real eebus-go reference control box."""

    @pytest.mark.asyncio
    async def test_receives_limit_from_control_box(
        self,
        everest_core: EverestCore,
        test_controller,
        reference_control_box: ReferenceControlBox,
    ):
        """
        Start EVerest and the reference control box, then assert that the
        probe module receives a consumption limit from the control box that
        differs from the default failsafe limit, proving a real EEBUS/SHIP
        connection was established.

        The control box sends a WriteConsumptionLimit ~5 s after
        UseCaseSupportUpdate, so we allow up to 30 s total for mDNS
        discovery + SHIP handshake + limit delivery.
        """
        test_controller.start()
        assert reference_control_box.is_running()

        probe = EebusTestProbeModule(everest_core.get_runtime_session())
        probe.start()

        received_power_values = []
        control_box_limit = None
        deadline = time.monotonic() + 30

        while time.monotonic() < deadline:
            try:
                limits = await async_get(probe.external_limits_queue, timeout=2)
            except asyncio.TimeoutError:
                continue

            if not limits or not limits.schedule_import:
                continue

            power_w = limits.schedule_import[0].limits_to_leaves.total_power_W.value
            received_power_values.append(power_w)

            # Any value that is not the failsafe limit came from the control box.
            if abs(power_w - FAILSAFE_LIMIT_W) > 1.0:
                control_box_limit = power_w
                break

        assert reference_control_box.is_running(), "Control box died during the test"

        assert control_box_limit is not None, (
            f"Never received a limit from the eebus-go control box. "
            f"Only saw failsafe values: {received_power_values}"
        )

        test_controller.stop()
