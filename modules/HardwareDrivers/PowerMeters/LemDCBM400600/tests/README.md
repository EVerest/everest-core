## Unit & Integration Tests with GTest

A series of unit tests test the implemented business logic of the controller. For the http client that wraps
libcurl, integration tests can be used to test succesful communication with the device.

### Requirements for unit/integration tests

The GTest unit tests require GTest. This can be installed via
```bash
apt install libgtest-dev
```

### Build

Build the module with the flag `LEMDCBM_BUILD_TESTS:BOOL=ON`, e.g. via

```bash
export CMAKE_PREFIX_PATH=<Path to workspace / required Everest repositories>
mkdir -p build
cd build
cmake -DLEMDCBM_BUILD_TESTS:BOOL=ON .. 
make -j 10 
```

### Run Unit tests

In the build directory, run
```bash
./modules/LemDCBM400600/tests/test_lem_dcbm_400600_controller
```

### Run HTTPClient Integration Tests

Note: The integration test require the configured backend (in form of an actual LEM DCBM oder the Mock) to be running
at the configured address and port.

To start the mocked API, run 
```bash
python3 <Projekt root directory>/modules/LemDCBM400600/utils/lem_dcbm_api_mock/main.py 
```

To then run the http client integration tests, run in the build directory
```bash
./modules/LemDCBM400600/tests/integration_test_http_client
```
## Integration / E2E Tests for LemDCBM400600 (Python wrapped)

The integration / E2E tests built on the integration test tools from  `everest-core/tests` allow to test
the module from inside EVerest both against the mock (integration test) and the actual device (e2e test).

### Requirements for E2E tests

-  Module built & installed into <Build dir>/dist

-  Everest testing utils installed; cf. everst-core/tests/Readme.md

-  Further, this requires the following installed packages in the used Python interpreter
    ```bash
    pip install fastapi uvicorn pyyaml
    ```

If not done before  set the Cmake install prefix, for example via
```bash
CMAKE_INSTALL_PREFIX=<Build dir>/dist
```
then build and install the tool again (`cmake build`, `make`, `make install`; in the end, the $CMAKE_INSTALL_PREFIX directory
should contain the installed binaries)

### Run E2E tests

In `modules/LemDCBM400600/tests`, run:
```bash
python3 -m pytest --everest-prefix=$CMAKE_INSTALL_PREFIX test_lem_dcbm_400_600_sil.py
python3 -m pytest --lem-dcbm-host 10.8.8.24 --lem-dcbm-port 5566 --everest-prefix=$CMAKE_INSTALL_PREFIX test_lem_dcbm_400_600_e2e.py
```
(here, for the e2e test substitute appropriate values of the actual test device)

*Note* Due to a behavior of the `EverestCore` testing class from everest-utils, it is not possible 
to quote escape strings in configuration yamls; this leads to an unexpected behavior since an unquoted
ip address (such as 127.0.0.1) will fail the EVerest type check. A local workaround is a
host entry in `/etc/hosts`, such as
```bash
10.8.8.24     lemdcbm
```
and then use `lemdcbm` instead of the IP address.
