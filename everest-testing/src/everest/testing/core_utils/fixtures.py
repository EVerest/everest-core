# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

import pytest
from pathlib import Path

from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.test_control_module import TestControlModule


@pytest.fixture
def everest_core(request) -> EverestCore:
    """Fixture that can be used to start and stop everest-core"""

    everest_prefix = Path(request.config.getoption("--everest-prefix"))
    everest_core = EverestCore(everest_prefix)
    yield everest_core

    # FIXME (aw): proper life time management, shouldn't the fixure start and stop?
    everest_core.stop()


@pytest.fixture
def test_control_module(request):
    """Fixture that references the module which interfaces with everest-core and that can be used for
    control events for the test cases for everest-core.
    """
    everest_prefix = Path(request.config.getoption("--everest-prefix"))
    test_control_module = TestControlModule(everest_prefix)
    yield test_control_module

    # FIXME (aw): proper life time management, shouldn't the fixure start and stop?
    test_control_module.stop()
