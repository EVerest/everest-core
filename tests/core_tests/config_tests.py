
#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from copy import deepcopy
import logging
import os
from pathlib import Path
import pty
import pytest
from tempfile import mkdtemp
from typing import Dict

from everest.testing.core_utils.fixtures import *
from everest.testing.core_utils.everest_core import EverestCore

from everest.testing.core_utils import EverestConfigAdjustmentStrategy


class EverestCoreConfigSilGenPmConfigurationAdjustment(EverestConfigAdjustmentStrategy):
    def __init__(self):
        self.temporary_directory = mkdtemp()
        self.serial_port_0, self.serial_port_1 = pty.openpty()
        # FIXME: cleanup socket after test
        self.serial_port_0_name = os.ttyname(self.serial_port_0)

    def adjust_everest_configuration(self, everest_config: Dict):
        adjusted_config = deepcopy(everest_config)

        adjusted_config["active_modules"]["serial_comm_hub"]["config_implementation"]["main"]["serial_port"] = self.serial_port_0_name

        return adjusted_config


@pytest.mark.everest_core_config('config-sil-gen-pm.yaml')
@pytest.mark.everest_config_adaptions(EverestCoreConfigSilGenPmConfigurationAdjustment())
@pytest.mark.asyncio
async def test_start_config_sil_gen_pm(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_sil_gen_pm <<<<<<<<<")

    everest_core.start()


class TestConfigsInDirectory:
    @pytest.fixture(params=pytest.everest_configs['params'], ids=pytest.everest_configs['ids'])
    def core_config(self, request) -> EverestEnvironmentCoreConfiguration:
        everest_prefix = Path(request.config.getoption("--everest-prefix"))

        everest_config_path = request.param

        return EverestEnvironmentCoreConfiguration(
            everest_core_path=everest_prefix,
            template_everest_config_path=everest_config_path,
        )

    @pytest.mark.asyncio
    async def test_config(self, everest_core: EverestCore):
        logging.info(">>>>>>>>> test_config <<<<<<<<<")
        everest_core.start()
