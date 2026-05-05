# EEBUS Tests

Tests for the EEBUS module (LPC — Limitation of Power Consumption use case).

## Mock-based unit tests

Use mock gRPC servers — no external binary required:

```bash
cd tests
pytest --everest-prefix ../build/dist eebus_tests/eebus_tests.py -v
```

## Reference control box end-to-end test

Starts the real [eebus-go](https://github.com/enbility/eebus-go) reference control box alongside EVerest and verifies that a power consumption limit is received over a real EEBUS/SHIP connection.

### Prerequisites

1. Build and install EVerest with `BUILD_TESTING=ON` (EEBUS certs must be installed at `<prefix>/etc/everest/certs/eebus/`)
2. Place the compiled `controlbox` binary at `tests/eebus_tests/controlbox` (or `~/go/bin/controlbox`)
3. Place the control box TLS cert and key at `tests/eebus_tests/eebus.crt` and `tests/eebus_tests/eebus.key`
4. Ensure the control box cert's SKI matches the `eebus_ems_ski_allowlist` value in `config-test-eebus-reference.yaml`

The test automatically reads EVerest's SKI from the installed certs and passes it to the control box so the SHIP trust handshake completes in both directions.

### Run

```bash
cd tests
pytest --everest-prefix ../build/dist eebus_tests/test_eebus_reference_simple.py -v
```

The test allows up to 30 seconds for mDNS discovery, SHIP handshake, and limit delivery.

## C++ unit tests (LPC state machine)

```bash
ninja -C build everest-core_EEBUS_lpc_state_machine_tests
./build/modules/EnergyManagement/EEBUS/tests/everest-core_EEBUS_lpc_state_machine_tests
```
