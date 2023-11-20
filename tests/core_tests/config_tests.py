
#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

import logging
import pytest

from everest.testing.core_utils.fixtures import *
from everest.testing.core_utils.everest_core import EverestCore


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


@pytest.mark.everest_core_config('config-sil-gen-pm.yaml')
@pytest.mark.asyncio
async def test_start_config_sil_gen_pm(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_sil_gen_pm <<<<<<<<<")
    everest_core.start()


@pytest.mark.everest_core_config('config-sil-ocpp-custom-extension.yaml')
@pytest.mark.asyncio
async def test_start_config_sil_ocpp_custom_extension(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_start_config_sil_ocpp_custom_extension <<<<<<<<<")
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
