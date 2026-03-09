# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from pathlib import Path
import pytest

from everest.testing.core_utils.network_isolation import NetworkIsolationPlugin


def pytest_addoption(parser):
    parser.addoption("--everest-prefix", action="store", default="../build/dist",
                     help="everest prefix path; default = '../build/dist'")
    parser.addoption(
        "--network-isolation",
        action="store_true",
        default=False,
        help="Enable network isolation for parallel ISO 15118 tests. "
             "Requires veth pairs created by setup-network-isolation.sh.",
    )


def pytest_configure(config):
    NetworkIsolationPlugin.register(config)

    everest_prefix = config.getoption('--everest-prefix')
    everest_config_path = Path(everest_prefix) / 'etc/everest'
    if not everest_config_path.exists():
        return
    everest_configs = [path for path in everest_config_path.iterdir(
    ) if path.name.startswith('config-') and path.name.endswith('.yaml')]
    pytest.everest_configs = {}
    pytest.everest_configs['params'] = []
    pytest.everest_configs['ids'] = []
    for config_path in everest_configs:
        config_id = config_path.stem
        if config_id == 'config-sil-gen-pm' or config_id.startswith('config-test-') or config_id.startswith('config-bringup-') or config_id.startswith('config-CB'):
            # skip
            continue
        pytest.everest_configs['params'].append(config_path)
        pytest.everest_configs['ids'].append(config_id)

@pytest.fixture
def started_test_controller(test_controller):
    test_controller.start()
    yield test_controller
    test_controller.stop()
