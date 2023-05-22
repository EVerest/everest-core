# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

from pathlib import Path
import pytest


from everest.testing.core_utils.everest_core import EverestCore


@pytest.fixture
def everest_core(request) -> EverestCore:
    """Fixture that can be used to start and stop everest-core"""

    everest_prefix = Path(request.config.getoption("--everest-prefix"))
    everest_core = EverestCore(everest_prefix)
    yield everest_core

    # FIXME (aw): proper life time management, shouldn't the fixure start and stop?
    everest_core.stop()
