# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from pathlib import Path
import pytest


def pytest_addoption(parser):
    parser.addoption("--everest-prefix", action="store", default="../build/dist",
                     help="everest prefix path; default = '../build/dist'")


def pytest_configure(config):
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
        if config_id == 'config-sil-gen-pm' or config_id.startswith('config-test-'):
            # skip
            continue
        pytest.everest_configs['params'].append(config_path)
        pytest.everest_configs['ids'].append(config_id)
