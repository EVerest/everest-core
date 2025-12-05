# Everest Testing

This python package provides utility for testing EVerest with pytest.

The utilities are seperated into

- core_utils: Providing classes and fixtures to get a running and properly configured EVerest instance
- ocpp_utils (under development): Providing class and fixtures to test against an OCPP1.6J or OCPP2.0.1J central system

## Core Utils

### everest_core
The EverestCore class wraps the running EVerest instance and takes care of providing a proper set up environment (including 
the generation of temporary directories and adjusting the configuration accordingly. Note that in order to do so, EverestCore generates
 a temporary configuration file.)

### test_controller

Controller that can be used to start/stop the Everest instance and send events to control/simulate the stack's behavior.

### Fixtures

The core_utils basically provide two fixtures that you can require in your test cases:

- **everest_core** The main fixture `everest_core` can be used to start and stop the everest-core application.
- **test_controller**: Fixture that references the test_controller that can be used for control events for the test cases. This includes control over simulations that trigger events like an EV plug in, EV plug out, swipe RFID and more. 

#### Configuration Fixtures:

- **core_config** Core configuration, which is the everest_core path and the configuration path (utilizes the `everest_core_config` marker.)
- **probe_module_config** Used to provide the probe module configuration. In particular can be overriden if the probe module should require module connections.
- **ocpp_config**  Used to provide the configuration, i.e. the JSON device model,  to set up the OCPP (1.6 or 2.0.1) module.
- **evse_security_config** Used to provide the configuration to set up the EvseSecurity module.
- **persistent_storage_config** Used to provide the configuration to set up the PersistentStorage module.
- **everest_config_strategies**: Provides a list of additional `EverestConfigAdjustmentStrategy` instances that are called to manipulate the resulting Everest configuration.

### pytest markers

Some OCPP fixtures will parse pytest markers of test cases. The following markers can be used:
- **everest_core_config**: Can be used to specify the everest configuration file to be used in this test case
- **standalone_module**: Define one or several modules as standalone (multiple modules via `@pytest.mark.standalone_module("mod1","mod2")`)
- **probe_module**: If set, the ProbeModule will be injected into the config (used by the `probe_module_config` fixture). This marker accepts optional keyword arguments `connections` and `module_id` to configure the probe module. 
- **source_certs_dir**: If set and the  default `evse_security_config` fixture is used, this will cause the  `EvseSecurity` module configuration to use a  temporary certificates folder into which the source certificate folder trees are copied.
- **use_temporary_persistent_store**: If set and the  default `persistent_storage_config` fixture is used, this will cause the  `PersistentStore` module configuration to use a  temporary database.
- **everest_config_adaptions**: Can be given instances of `EverestConfigAdjustmentStrategy` as positional arguments which will be applied to the resulting Everest configuration.

## OCPP utils

The ocpp utils provide fixture which you can require in your test cases in order to start a central system and initiate operations.
These utilities are still under development.

- **central_system_v16**: Fixture that starts up an OCPP1.6 central system. Can be started as TLS or plain websocket depending on the request parameter.
- **central_system_v201**: Fixture that starts up an OCPP2.0.1 central system. Can be started as TLS or plain websocket depending on the request parameter.
- **charge_point_v16**: Fixture starts up an OCPP1.6 central system and provides access to the connection of the charge point that connects to it. This reference can be used to send OCPP messages initiated by the central system and to receive and validate messages from the charge point. It requires the fixtures central_system_v16 and test_controller and starts the test_controller immediately.
- **charge_point_v201**: Fixture starts up an OCPP2.0.1 central system and provides access to the connection of the charge point that connects to it. This reference can be used to send OCPP messages initiated by the central system and to receive and validate messages from the charge point. It requires the fixtures central_system_v16 and test_controller and starts the test_controller immediately.
- **test_utility**: Utility fixture that contains the OCPP message history, the validation mode (STRICT, EASY) and it can keep track of forbidden OCPP messages (Actions) (ones that cause a test case to fail if they are received)
- **ftp_server**: This fixture creates a temporary directory and starts a local ftp server connected to that directory. The temporary directory is deleted after the test. It is used for Diagnostics and Logfiles

#### Configuration Fixtures:
- **test_config**: This fixture is of type OcppTestConfiguration and it specifies some data that are required or can be configured for testing OCPP. If you don't override this fixture, it initiializes to some default information that is required to set up other fixtures (e.g. ChargePointId, CSMS Port). You can implement this fixture yourself in order to be able to include this information in your test cases.
- **ocpp_config** _Overrides_ the core_util's `ocpp_config` fixture.  Requires the `test_config` fixture to  extract required OCPP configuration.

An important function that you will frequently use when writing test cases is the **wait_for_and_validate** function inside [charge_point_utils.py](src/everest/testing/ocpp_utils/charge_point_utils.py). This method waits for an expected message specified by the message_type, the action and the payload to be received. It also considers the test case meta_data that contains the message history, the validation mode and forbidden actions.


### pytest markers

- **ocpp_version**: Can be "ocpp1.6" or "ocpp2.0.1" and is used to setup EVerest and the central system for the specific OCPP version
- **ocpp_config**: Specification of the .json OCPP config file. Used in `ocpp_config` fixture and used as template configuration (if not specified, the OCPP config as specified in the EVerest configuration is used) 
- **ocpp_config_adaptions**: Specification of the .json OCPP config file. Used in `ocpp_config` fixture and used as template configuration (if not specified, the OCPP config as specified in the EVerest configuration is used) 
- **inject_csms_mock**: (currently only OCPP 2.0.1) If set, the `central_system_v201` will wrap any csms handler method into an unittest mock. In particular, this allows changing the CSMS behavior even after the chargepoint is started by setting side effects of the mock. See `everest.testing.ocpp_utils.charge_point_v201.inject_csms_v201_mock` docstring for an example.
- **csms_tls**: Enable/disable TLS for the CSMS websocket server. If given without arguments, enables TLS. First argument can be `False` to explicitly disable TLS. Further optional keyword arguments  `certificate`, `private_key`,`passphrase`, `root_ca` , and `verify_client_certificate` allow to overwrite SSL context options.
- **ocpp_config_adaptions**: Can be given instances of `OCPPConfigAdjustmentStrategy` as positional arguments which will be applied to the resulting OCPP configuration.
- **custom_central_system**: Can be given a instance of `CentralSystem` as the first positional argument which will use that instance as a central system for all fixtures that require a `central_system`.


## Add a conftest.py

The test_controller fixture and inherently also the charge_point_v16 and charge_point_v201 require information about the directory of the everest-core application and libocpp. Those can be specified within a conftest.py. Within the conftest.py you could also override the test_config fixture for your specific setup.

## Set markers and override fixture to configure instances

The `everest_core` fixture utilizes the several configuration fixtures to configure the running instances. In order to adjust
the configuration you can
- set respective pytest markers to adjust the configuration for a single test / test class
- override the specific fixtures to adjust the configuration for a whole test suite

Note: When overriding a fixture, be careful which pytest markers might be ignored as used by the overridden fixture!

### Example

```python

from everest.testing.core_utils.fixtures import *
from everest.testing.ocpp_utils.fixtures import ocpp_config

@pytest.fixture
def test_config(request) -> OcppTestConfiguration:
    # some code generating a customized OCPP test config
    ...
    return custom_test_config

@pytest.fixture
def core_config(request) -> EverestEnvironmentCoreConfiguration:
    #  some code generating a customized EveresetCore test config
    # e.g. useful to point to local Everest configuration files
    ...
    return custom_core_config


@pytest.mark.probe_module
class TestMyEverestModule:
    # ... pytest test suite that uses for all test the probe module

    def test_a(self, test_controller):
        ...

```

_Note_: The "*" import from `core_utils.fixtures` may ensure backwards compatibility to automatically load new default fixtures in the future!


## Install

In order to use the provided fixtures within your test cases, a successful build of everest-core is required. Refer to https://github.com/EVerest/everest-core for this.

An MQTT broker needs to run on your system in order to start the test cases including everest-core. Docker can be used for this. Refer to https://everest.github.io/nightly/tutorials/docker_setup.html in order to set this up.

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




