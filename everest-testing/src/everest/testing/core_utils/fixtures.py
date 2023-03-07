# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

import pytest
from pathlib import Path

from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.test_control_module import TestControlModule

@ pytest.fixture
def everest_core(request) -> EverestCore:
    """Fixture that can be used to start and stop everest-core
    """
    everest_core_path = Path(request.config.getoption("--path"))
    everest_core = EverestCore(everest_core_path, everest_core_path / "config/config-sil.yaml")
    yield everest_core
    everest_core.stop()

@ pytest.fixture
def test_control_module(request):
    """Fixture that references the module which interfaces with everest-core and that can be used for
    control events for the test cases for everest-core.
    """
    everest_core_path = Path(request.config.getoption("--path"))
    test_control_module = TestControlModule(everest_core_path)
    return test_control_module
