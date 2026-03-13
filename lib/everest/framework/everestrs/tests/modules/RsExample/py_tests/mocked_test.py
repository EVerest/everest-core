#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
"""
Smoke test for RsExample using the Python testing framework.

Launches EVerest with RsExample (example_0) and a ProbeModule (example_1),
then verifies that RsExample publishes max_current(123.0) on ready and
responds to the uses_something command.
"""

import asyncio
import sys
from unittest.mock import Mock

import pytest

from everest.testing.core_utils.common import Requirement
from everest.testing.core_utils.fixtures import *
from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.probe_module import ProbeModule


async def wait_for_mock(mock, timeout=10):
    """Poll until mock has been called or timeout."""
    for _ in range(int(timeout * 10)):
        if mock.called:
            return
        await asyncio.sleep(0.1)
    raise TimeoutError(f"Mock not called within {timeout}s")


@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={"a_friend": [Requirement("example_0", "foobar")]},
    module_id="example_1",
)
@pytest.mark.everest_core_config("config_probe.yaml")
async def test_rs_example_publishes_max_current(everest_core: EverestCore):
    """Verify that RsExample publishes max_current(123.0) on ready."""
    everest_core.start()

    probe = ProbeModule(everest_core.get_runtime_session(), module_id="example_1")

    max_current_mock = Mock()
    probe.subscribe_variable("a_friend", "max_current", max_current_mock)

    probe.implement_command("foobar", "uses_something", lambda args: True)

    probe.start()
    await probe.wait_to_be_ready(timeout=10)

    await wait_for_mock(max_current_mock, timeout=10)
    value = max_current_mock.call_args[0][0]
    assert value == 123.0, f"Expected max_current=123.0, got {value}"


@pytest.mark.asyncio
@pytest.mark.probe_module(
    connections={"a_friend": [Requirement("example_0", "foobar")]},
    module_id="example_1",
)
@pytest.mark.everest_core_config("config_probe.yaml")
async def test_rs_example_uses_something(everest_core: EverestCore):
    """Verify that we can call uses_something on RsExample."""
    everest_core.start()

    probe = ProbeModule(everest_core.get_runtime_session(), module_id="example_1")

    probe.implement_command("foobar", "uses_something", lambda args: True)

    probe.start()
    await probe.wait_to_be_ready(timeout=10)

    result = await probe.call_command("a_friend", "uses_something", {"key": "hello"})
    # The C++ binding may return None for boolean results; verify at least no exception.
    assert result is None or result == True, f"Unexpected result: {result!r}"


if __name__ == "__main__":
    sys.exit(pytest.main([__file__, "-v"] + sys.argv[1:]))
