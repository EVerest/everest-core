# OCPP Integration Tests

This directory contains some test tooling and integration tests
for OCPP1.6 and OCPP2.0.1.

## Run the tests

All tests are run via the unified `tests/run-tests.sh` script from the
repository root. The script handles parallel execution, network-isolation
setup/teardown and certificate/config installation automatically.

```bash
# From the repository root:
tests/run-tests.sh ocpp          # all OCPP tests (1.6, 2.0.1, 2.1)
tests/run-tests.sh ocpp16        # OCPP 1.6 only
tests/run-tests.sh ocpp201       # OCPP 2.0.1 only
tests/run-tests.sh ocpp21        # OCPP 2.1 only
tests/run-tests.sh --serial ocpp # run serially
tests/run-tests.sh -j4 ocpp     # limit to 4 parallel workers
tests/run-tests.sh --help        # show all options
```

Tests run in parallel by default. The time depends on your system;
it usually takes a couple of minutes.
Check the generated `report.html` for detailed results.

You can also run individual test sets or test cases using

```bash
python3 -m pytest test_sets/ocpp201/remote_control.py::test_F01_F02_F03
```

This runs test case `test_F01_F02_F03`
specified in `test_sets/ocpp201/remote_control.py`.

If you run the test cases individually,
make sure to have all required certificates and configs
for the test cases installed using the
convenience scripts inside [test_sets/everest-aux](test_sets/everest-aux/)

```bash
./install_certs <path-to-everest-core-installation-directory>
./install_configs <path-to-everest-core-installation-directory>
