# Everest Testing

This python package provides utility for testing EVerest with pytest.

The utilities are seperated into
* core_utils: Providing classes and fixtures to test everest-core
* ocpp_utils (under development): Providing class and fixtures to test against an OCPP1.6J or OCPP2.0.1J central system 

## Core Utils

The core_utils basically provide two fixtures that you can require in your test cases: 
* everest_core
* test_control_module

The fixture `everest_core` can be used to start and stop the everest-core application.

The fixture `test_control_module` is a wrapper around the everest module `PyTestControlModule`. It contains a direct reference to this python module and is used to interface with everest-core. It also contains a data structure in which events that occur within EVerest are stored. Those events can be used to validate against the expectations of your test case. You can access controlling the simulation and events for the test cases.

## OCPP utils
The ocpp utils provide fixture which you can require in your test cases in order to start a central system and initiate operations.
These utilities are still under early development.

## Install

In order to use the provided fixtures within your test cases, a successful build of everest-core is required. Refer to https://github.com/EVerest/everest-core for this.

Install this package using 

```bash
python3 -m pip install .
```

## Usage

Simply require the provided fixtures in your test case.

```python

import logging
import time
import pytest

from everest.testing.core_utils.fixtures import *
from everest.testing.core_utils.test_control_module import TestControlModule
from everest.testing.core_utils.everest_core import EverestCore

@ pytest.mark.asyncio
async def test_01(everest_core: EverestCore, test_control_module: TestControlModule):
    logging.info(">>>>>>>>> test_01 <<<<<<<<<")

    # start everest-core
    everest_core.start(standalone_module='test_control_module')
    logging.info("everest-core ready, waiting for test control module")

    # start standalone module
    test_control_module_handle = test_control_module.start()
    logging.info("test_control_module started")

    # start a charging session using the test control module
    test_control_module_handle.start_charging(mode = 'raw', charge_sim_string = "sleep 1;iec_wait_pwr_ready;sleep 1;draw_power_regulated 16,3;sleep 20;unplug")
    
    # do validations
    # ... 
    # ...


    # stop everest-core
    everest_core.stop()
```
