# Everest Testing

This python package provides utility for testing EVerest with pytest.

The utilities are seperated into

- core_utils: Providing classes and fixtures to test everest-core
- ocpp_utils (under development): Providing class and fixtures to test against an OCPP1.6J or OCPP2.0.1J central system

## Core Utils

The core_utils basically provide two fixtures that you can require in your test cases:

- everest_core

The fixture `everest_core` can be used to start and stop the everest-core application.

## OCPP utils

The ocpp utils provide fixture which you can require in your test cases in order to start a central system and initiate operations.
These utilities are still under development.

- **test_controller**: Fixture that references the the test_controller that can be used for control events for the test cases. This includes control over simulations that trigger events like an EV plug in, EV plug out, swipe RFID and more.
- **central_system_v16**: Fixture that starts up an OCPP1.6 central system. Can be started as TLS or plain websocket depending on the request parameter.
- **central_system_v201**: Fixture that starts up an OCPP2.0.1 central system. Can be started as TLS or plain websocket depending on the request parameter.
- **charge_point_v16**: Fixture starts up an OCPP1.6 central system and provides access to the connection of the charge point that connects to it. This reference can be used to send OCPP messages initiated by the central system and to receive and validate messages from the charge point. It requires the fixtures central_system_v16 and test_controller and starts the test_controller immediately.
- **charge_point_v201**: Fixture starts up an OCPP2.0.1 central system and provides access to the connection of the charge point that connects to it. This reference can be used to send OCPP messages initiated by the central system and to receive and validate messages from the charge point. It requires the fixtures central_system_v16 and test_controller and starts the test_controller immediately.
- **test_utility**: Utility fixture that contains the OCPP message history, the validation mode (STRICT, EASY) and it can keep track of forbidden OCPP messages (Actions) (ones that cause a test case to fail if they are received)
- **ftp_server**: This fixture creates a temporary directory and starts a local ftp server connected to that directory. The temporary directory is deleted after the test. It is used for Diagnostics and Logfiles
- **test_config**: This fixture is of type OcppTestConfiguration and it specifies some data that are required or can be configured for testing OCPP. If you dont override this fixture, it initiializes to some default information that is required to set up other fixtures (e.g. ChargePointId, CSMS Port). You can implement this fixture yourself in order to be able to include this information in your test cases.

An important function that you will frequently use when writing test cases is the **wait_for_and_validate** function inside [charge_point_utils.py](src/everest/testing/ocpp_utils/charge_point_utils.py). This method waits for an expected message specified by the message_type, the action and the payload to be received. It also considers the test case meta_data that contains the message history, the validation mode and forbidden actions.

## pytest markers

Some OCPP fixtures will parse pytest markers of test cases. The following markers can be used:

- **ocpp_version**: Can be "ocpp1.6" or "ocpp2.0.1" and is used to setup EVerest and the central system for the specific OCPP version
- **everest_core_config**: Can be used to specify the everest configuration file to be used in this test case

## Add a conftest.py

The test_controller fixture and inherently also the charge_point_v16 and charge_point_v201 require information about the directory of the everest-core application and libocpp. Those can be specified within a conftest.py. Within the conftest.py you could also override the test_config fixture for your specific setup.

## Install

In order to use the provided fixtures within your test cases, a successful build of everest-core is required. Refer to https://github.com/EVerest/everest-core for this.

Install this package using

```bash
python3 -m pip install .
```

## Examples

Have a look at [example_tests.py](examples/tests.py). In this file you can find and run one OCPP1.6 and one OCPP2.0.1 test case. These test cases will help you to get familiar with the fixtures provided in this package. You need a successful of [everest-core](https://github.com/EVerest/everest-core) on your development machine in order to run the tests.

You can run these tests using

```bash
cd examples
python3 -m pytest tests.py --everest-prefix <path-to-everest-core>/build/dist/ --libocpp <path-to-libocpp> --log-cli-level=DEBUG
```
