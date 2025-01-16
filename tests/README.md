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


# Code coverage

To activate code coverage, you need to configure the project with
`BUILD_TESTING` enabled.  Furthermore, `gcovr` needs to be installed.

## Generating coverage reports

Before generating a coverage report you will typically need to run some
executables or tests, which contain or link to code, that generates coverage
statistics.

After that, just run:
```bash
cmake --build build --target everest-core_create_coverage
# or ninja -C build everest-core_create_coverage
```

The generated `index.html` will be available at
`build/everest-core_create_coverage/index.html`.

**Note**: the code coverage for a specific compilation unit (cpp file) will
accumulate if you run multiple executables that use this compilation unit.  The
code coverage is recorded in `gcda` files.  If you want to get separate coverage
reports for different executables, these `gcda` files need to be deleted for
each new *run*.


## Adding code/targets to code coverage

By default, all *cpp* modules are added automatically to a list of targets that
will be compiled with coverage flags.  If additional libraries or tests need to
be added to code coverage, you should use the following functions in your
*cmake* code:
```cmake
ev_register_library_target(LIBRARY_TARGET_NAME)
ev_register_test_target(TEST_TARGET_NAME)
```

## Conventions

For the `ev_register_*_target` functions, call these function close to their
scope of detail - meaning near `add_library` for libraries like:

```cmake
add_library(evse_security_conversions STATIC)
add_library(everest::evse_security_conversions ALIAS evse_security_conversions)
ev_register_library_target(evse_security_conversions)
```

and near `add_test` for tests like:

```cmake
add_test(${TLS_GTEST_NAME} ${TLS_GTEST_NAME})
ev_register_test_target(${TLS_GTEST_NAME})
```

and not scattered around.

---

By default, all directories ending with `tests` will be excluded from the code
coverage report (because we're not interested in the code coverage of the unit
test code itself).  So when adding unit tests for code coverage, place them into
a `tests` folder.

## Troubleshooting

`no_working_dir_found`: while developing code (e.g. an `example.cpp`), it might
happen, that the created object file `example.cpp.o` will get out of sync with
its corresponding `example.cpp.gcno` (`gcov` note file) and/or
`example.cpp.gcda` (`gcov` data file).  When creating a report, `gcovr` will
complain, because it can't find the corresponding `example.cpp.o` file for a
stale `example.cpp.gcno` file.  It might also not be able to find the
`example.cpp.gcno` file for a stale `example.cpp.gcda` file or an older `gcda`
file might got out of sync due to a newer `gcno` file and so forth.  Do not
simply ignore these warnings/errors, as the report might not be accurate
anymore.

The current sledgehammer approach is to manually delete the files in question or
just do (**be careful** with `rm` commands and wildcards!):
```bash
find ./build -name "*.gcno" | xargs rm
find ./build -name "*.gcda" | xargs rm
ninja -C build clean
ninja -C build
# run code
# create coverage
```

We're currently trying to write some tooling around this to make this more
usable.
