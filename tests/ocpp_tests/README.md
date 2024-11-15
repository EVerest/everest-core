# OCPP Integration Tests

This directory contains some test tooling and integration tests
for OCPP1.6 and OCPP2.0.1.

## Requirements

In order to run the integration tests, you need to have everest-core compiled
and installed on your system.

Make sure to also install the python requirements
for running the test cases using

```bash
python3 -m pip install -r requirements.txt
```

## Run the tests

You can run the integration tests using the convenience scripts
provided in this directory e.g.

```bash
./run-testing.sh
```

This command runs all test cases in parallel.
The time for running the test cases depends on your system.
It usually takes a couple of minutes.
You can check out the test results by opening the generated `results.html`.

You can choose to run the tests sequentially and/or only run subsets
for OCPP1.6 or OCPP2.0.1 using any of the other run scripts.

Alternatively, you can run individual test sets or test cases using

```bash
python3 -m pytest test_sets/ocpp201/remote_control.py \
  --everest-prefix <path-to-everest-core-installation-directory> \
  -k 'test_F01_F02_F03'
```

e.g.

```bash
python3 -m pytest test_sets/ocpp201/remote_control.py \
  --everest-prefix ~/checkout/everest-core/build/dist \
  -k 'test_F01_F02_F03'
```

This runs test case `test_F01_F02_F03`
specified in `test_sets/ocpp201/remote_control.py`.

If you run the test cases individually,
make sure to have all required certificates and configs
for the test cases installed using the
convenience scripts inside [test_sets/everest/aux](test_sets/everest-aux/)

```bash
./install_certs <path-to-everest-core-installation-directory>
./install_configs <path-to-everest-core-installation-directory>
