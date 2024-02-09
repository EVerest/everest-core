# everest-core tests

This folder contains the basic test functions for everest-core's integration testing. You can either extend the '*\*_tests.py*' files in the "everest-core/**tests/core_tests**" folder or write your own!

## Prerequisites

For running the tests you need to install the everest-testing framework from everest-utils:

```bash
cd ~/checkout/everest-workspace/everest-utils/everest-testing
python3 -m pip install .
```

You can also use the following cmake target to install everest-testing and its dependencies:
```bash
cd everest-core
cmake --build build --target install_everest_testing
```

## Execute locally

To start you should try executing the available basic tests:

-   **startup_tests.py** (checks if all test functionality can be started correctly, but does not yet do any value-based integration testing)
-   **basic_charging_tests.py** (tests a basic charging situation: enable charging -> wait 20 seconds -> check if a certain minimum amount of kWhs have been charged)

Go to your "everest-core/**tests**" folder and execute *pytest*:

```bash
cd ~/checkout/everest-workspace/everest-core/tests
pytest --everest-prefix ../build/dist core_tests/*.py framework_tests/*.py
```

After execution a "results.xml" file should be available in the "everest-core/**tests**" folder, which details the current test results.

(*Note: Because the everest-core tests are used in CI testing and because an upstream issue in the way everestpy handles external modules, it is currently not possible to receive a test report on stdout at the end of a pytest run in everest-core tests, as otherwise CI testing would not always work. We are currently working on resolving the issue, please be patient. Thank you!*)

## Add own test sets

To create own test sets, you need to write a new python file in the "everest-core/**tests/core_tests/**" folder. A basic file template would look like this:

```python
import logging
import pytest

from everest.testing.core_utils.fixtures import *
from validations.base_functions import wait_for_and_validate_event
from everest.testing.core_utils.test_control_module import TestControlModule
from everest.testing.core_utils.everest_core import EverestCore
from validations.user_functions import *

@ pytest.mark.asyncio
async def test_001_my_first_test(everest_core: EverestCore, 
                                 test_control_module: TestControlModule):
    logging.info(">>>>>>>>> test_001_my_first_test <<<<<<<<<")
    everest_core.start()

    assert await wait_for_and_validate_event(test_control_module, 
                                             exp_event='my_event', 
                                             exp_data={"my_data":"test"},
                                             validation_function=my_validation_function,
                                             timeout=30) 
```

\*Note: All test functions' names NEED to start with "test" (see pytest documentation: https://docs.pytest.org/)

In the above example you then would also have to write a (user-)validation function by the name of "my_validation_function()" in the "everest-core/**tests/core_tests/validations/user_functions.py**" file.

For this, you can utilize helper functions from the "everest-core/**tests/core_tests/validations/base_functions.py**" file (e.g. "get_key_if_exists()" to traverse and retrieve event data from an event-object).

**Attention**: When you change something in the test- or validation files, please do not forget to run "*make install*" on everest-core before starting the tests, as otherwise the changes are not reflected in the test run!

### Controlling more functionality with PyTestControlModule

If you would like to receive other everest-internal data, send commands to other everest-core modules or would like to change the received events for everest-testing, you can modify the **PyTestControlModule** in the "everest-core/**modules/PyTestControlModule**" folder.

**Attention:** Please be aware, though, that the use of this module is preliminary and changes in the *everest-framework* may render this module **obsolete** in the future!

## Troubleshooting

**1. Problem:** Multiple concatenated tests that use the *PyTestControlModule* (in standalone configuration) result in an exception and subsequent crash of everest-testing.

**Cause:** There is currently no way to unload a module in *everest-core*. But because pytest runs out of the same context for all tests in a testset, a once loaded *PyTestControlModule* stays active for the rest of the pytest/everest-testing run. When a new test tries to load a second instance of the *PyTestControlModule* it collides with the already running instance and causes everest-testing to crash.

**Solution:** Unfortunately, this requires a change in the everest-framework. This change is currently under development, but not yet merged into main. 
For the moment, there is a *workaround available:* Use a new test-set (another python file containing a new set of tests) for every test that requires control via the *PyTestControlModule*. As soon as a fix for the underlying issue is merged, this will be updated!

**2. Problem:** On an unmodified run of "basic_charging_tests.py" the test *test_001_charge_defined_ammount* fails and reports an amount of 0.0kWh charged.

**Cause:** Currently there is a bug in the EVSE manager implementation in everest-core, causing a race-condition between resetting the "amount charged" parameter and the event reporting for a "*transaction_finished*" event. Sometimes it reports correctly, other times it reports a value of 0.0kWh charged.

**Solution:** This problem is known and a fix is under way. For the moment, just re-run the test. Usually, this should then produce a passing result.
