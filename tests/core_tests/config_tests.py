
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

from everest.testing.core_utils.configuration.everest_configuration_visitors.everest_configuration_visitor import \
    EverestConfigAdjustmentVisitor


@pytest.mark.everest_core_config('config-example.yaml')
@pytest.mark.asyncio
async def test_start_config_example(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_example <<<<<<<<<")
    everest_core.start()


@pytest.mark.everest_core_config('config-sil-dc-sae-v2g.yaml')
@pytest.mark.asyncio
async def test_start_config_sil_dc_sae_v2g(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_sil_dc_sae_v2g <<<<<<<<<")
    everest_core.start()


@pytest.mark.everest_core_config('config-sil-dc-sae-v2h.yaml')
@pytest.mark.asyncio
async def test_start_config_sil_dc_sae_v2h(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_sil_dc_sae_v2h <<<<<<<<<")
    everest_core.start()


@pytest.mark.everest_core_config('config-sil-dc.yaml')
@pytest.mark.asyncio
async def test_start_config_sil_dc(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_sil_dc <<<<<<<<<")
    everest_core.start()


@pytest.mark.everest_core_config('config-sil-energy-management.yaml')
@pytest.mark.asyncio
async def test_start_config_sil_energy_management(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_config_sil_energy_management <<<<<<<<<")
    everest_core.start()


class EverestCoreConfigSilGenPmConfigurationVisitor(EverestConfigAdjustmentVisitor):
    def __init__(self):
        self.temporary_directory = mkdtemp()
        self.serial_port_0, self.serial_port_1 = pty.openpty()
        self.serial_port_0_name = os.ttyname(self.serial_port_0) # FIXME: cleanup socket after test

    def adjust_everest_configuration(self, everest_config: Dict):
        adjusted_config = deepcopy(everest_config)

        adjusted_config["active_modules"]["serial_comm_hub"]["config_implementation"]["main"]["serial_port"] = self.serial_port_0_name

        return adjusted_config


@pytest.mark.everest_core_config('config-sil-gen-pm.yaml')
@pytest.mark.everest_config_adaptions(EverestCoreConfigSilGenPmConfigurationVisitor())
@pytest.mark.asyncio
async def test_start_config_sil_gen_pm(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_sil_gen_pm <<<<<<<<<")

    everest_core.start()


@pytest.mark.everest_core_config('config-sil-ocpp-custom-extension.yaml')
@pytest.mark.asyncio
async def test_start_config_sil_ocpp_custom_extension(everest_core: EverestCore):
    logging.info(
        ">>>>>>>>> test_start_config_sil_ocpp_custom_extension <<<<<<<<<")
    everest_core.start()


@pytest.mark.everest_core_config('config-sil-ocpp-pnc.yaml')
@pytest.mark.asyncio
async def test_start_config_sil_ocpp_pnc(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_sil_ocpp_pnc <<<<<<<<<")
    everest_core.start()


@pytest.mark.everest_core_config('config-sil-ocpp.yaml')
@pytest.mark.asyncio
async def test_start_config_sil_ocpp(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_sil_ocpp <<<<<<<<<")
    everest_core.start()


@pytest.mark.everest_core_config('config-sil-ocpp201.yaml')
@pytest.mark.asyncio
async def test_start_config_sil_ocpp201(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_sil_ocpp201 <<<<<<<<<")
    everest_core.start()


@pytest.mark.everest_core_config('config-sil-two-evse-dc.yaml')
@pytest.mark.asyncio
async def test_start_config_sil_two_evse_dc(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_sil_two_evse_dc <<<<<<<<<")
    everest_core.start()


@pytest.mark.everest_core_config('config-sil-two-evse.yaml')
@pytest.mark.asyncio
async def test_start_config_sil_two_evse(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_sil_two_evse <<<<<<<<<")
    everest_core.start()


@pytest.mark.everest_core_config('config-sil.yaml')
@pytest.mark.asyncio
async def test_start_config_sil(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_sil <<<<<<<<<")
    everest_core.start()
