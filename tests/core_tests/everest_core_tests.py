
#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

import logging
import pytest

from everest.testing.core_utils.fixtures import *
from validations.validations import wait_for_and_validate_event
from everest.testing.core_utils.test_control_module import TestControlModule
from everest.testing.core_utils.everest_core import EverestCore
from validations.validation_functions import *

@ pytest.mark.asyncio
async def test_000_startup_check(everest_core: EverestCore):
    logging.info(">>>>>>>>> test_000_startup_check <<<<<<<<<")
    everest_core.start()
    everest_core.stop()

@ pytest.mark.asyncio
async def test_001_start_test_module(everest_core: EverestCore, test_control_module: TestControlModule):
    logging.info(">>>>>>>>> test_001_start_test_module <<<<<<<<<")

    everest_core.start(standalone_module='test_control_module')
    logging.info("everest-core ready, waiting for test control module")

    test_control_module_handle = test_control_module.start()
    logging.info("test_control_module started")

    test_control_module_handle.start_charging(mode = 'raw', charge_sim_string = "sleep 1;iec_wait_pwr_ready;sleep 1;draw_power_regulated 16,3;sleep 5;unplug")

    # assert only for events to have happened (ignore contents)
    assert await wait_for_and_validate_event(test_control_module, exp_event='transaction_started', exp_data=None, validation_function=None, timeout=10)
    assert await wait_for_and_validate_event(test_control_module, exp_event='transaction_finished', exp_data=None, validation_function=None, timeout=10)

    everest_core.stop()


# @ pytest.mark.asyncio
# async def test_002_charge_defined_ammount(everest_core: EverestCore, test_control_module: TestControlModule):
#     logging.info(">>>>>>>>> test_002_charge_defined_ammount <<<<<<<<<")

#     everest_core.start(standalone_module='test_control_module')
#     logging.info("everest-core ready, waiting for test control module")

#     test_control_module_handle = test_control_module.start()
#     logging.info("test_control_module started")

#     test_control_module_handle.start_charging(mode = 'raw', charge_sim_string = "sleep 1;iec_wait_pwr_ready;sleep 1;draw_power_regulated 16,3;sleep 20;unplug")

#     # assert that charged energy is > target
#     assert await wait_for_and_validate_event(test_control_module, exp_event='transaction_started', exp_data=None, validation_function=set_test_memory, timeout=20)
#     assert await wait_for_and_validate_event(test_control_module, exp_event='transaction_finished', exp_data=float(56.0), validation_function=validate_transaction_charged_amount, timeout=30)

#     everest_core.stop()
