# Carlo Gavazzi EM580 Power Meter Driver

Driver module for the **Carlo Gavazzi EM580** power meter using Modbus via EVerest's `serial_communication_hub` interface.
It implements the standardized EVerest `powermeter` interface and supports **OCMF/Eichrecht** transaction flows.

## Overview

This is an **EVerest Hardware Driver** module that:

- **Implements**: `powermeter` interface
- **Communicates**: Modbus RTU (through `SerialCommHub`)
- **Provides**: Live meter values, OCMF transaction start/stop handling, public key publishing

## Features

- **Live measurements**: Publishes `powermeter` readings periodically (`live_measurement_interval_ms`)
- **OCMF transactions**:
  - `start_transaction`: writes OCMF identification fields + tariff text (TT) + start command
  - `stop_transaction`: ends transaction, waits for READY, reads OCMF file, confirms file read
- **Modbus protocol compliance**: transport splits writes into chunks (max 123 registers per request)
- **Resilience / retries**:
  - Separate initial connection retry settings vs. normal operation retry settings
  - Communication-fault raise/clear hooks
- **Device state monitoring**: periodic read of device state bitfield (`device_state_read_interval_ms`)
- **Signature key readout**: reads signature type and public keys; publishes public key (hex) via `public_key_ocmf`

## Hardware requirements & compatibility

### Supported devices

- **Carlo Gavazzi EM580** with **Modbus RTU** enabled/available.
- **OCMF/Eichrecht flow**: requires a meter variant/firmware that supports the OCMF register set used by this driver.

If you are unsure which EM580 variant you have, check the device documentation/ordering code and confirm:
- Modbus RTU via RS-485 is supported and enabled
- The meter is configured for a known Modbus **unit id** (device address)

### Bus / physical layer

- **RS-485 (2-wire, half duplex)**: correct A/B wiring is essential.
- **Termination**: enable 120Ω termination at the ends of the RS-485 bus (and only at the ends).
- **Biasing**: ensure the bus has proper bias resistors (often provided by the adapter/master or by dedicated biasing).

### Host requirements

- A Linux host running EVerest with access to a serial device (e.g. `/dev/ttyUSB0`).
- A **USB-to-RS485** adapter (or equivalent RS-485 interface) supported by the OS.
- Permissions to access the serial device node (group membership / udev rules).

## Configuration

### Required connections

The module requires a `serial_communication_hub` implementation (typically `SerialCommHub`) via its `modbus` requirement.
`SerialCommHub` encapsulates the serial port settings (port, baudrate, parity, timeouts). The EM580 module only needs the
hub connection plus its Modbus unit id (`powermeter_device_id`).

### Example configuration (bringup)

See `config/bringup/config-bringup-CGEM580.yaml`:

```yaml
active_modules:
  cgem580:
    module: CarloGavazzi_EM580
    config_implementation:
      main:
        powermeter_device_id: 1
        communication_retry_count: 3
        communication_retry_delay_ms: 500
        communication_error_pause_delay_s: 10
        initial_connection_retry_count: 10
        initial_connection_retry_delay_ms: 2000
        timezone_offset_minutes: 60
        live_measurement_interval_ms: 1000
        device_state_read_interval_ms: 10000
    connections:
      modbus:
        - module_id: comm_hub
          implementation_id: main
```

### Multiple EM580 devices on one RS-485 bus

You can run multiple EM580 devices on the same RS-485 line by:
- Creating multiple `CarloGavazzi_EM580` module instances
- Pointing them all to the same `SerialCommHub`
- Giving each instance a unique `powermeter_device_id` (Modbus unit id)

### Configuration parameters

All parameters are defined in `modules/HardwareDrivers/PowerMeters/CarloGavazzi_EM580/manifest.yaml`:

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `powermeter_device_id` | integer | `1` | Modbus device ID on the bus |
| `communication_retry_count` | integer | `3` | Retries for regular Modbus operations |
| `communication_retry_delay_ms` | integer | `500` | Delay between regular retries |
| `communication_error_pause_delay_s` | integer | `10` | Pause after a communication failure in the live measurement thread before retrying (also applies to initial communication) |
| `initial_connection_retry_count` | integer | `10` (0 = infinite) | Retries during initial device setup/signature config reads |
| `initial_connection_retry_delay_ms` | integer | `2000` | Delay between initialization retries |
| `timezone_offset_minutes` | integer | `0` | Timezone offset from UTC (minutes) |
| `live_measurement_interval_ms` | integer | `1000` | Interval for reading/publishing live measurements |
| `device_state_read_interval_ms` | integer | `10000` | Interval for reading device-state bitfield (VendorError reporting) |

### Parameter tuning notes

- **`initial_connection_*`**:
  - Used during module startup for device setup / signature config reads.
  - `initial_connection_retry_count: 0` means **retry forever**.
  - `initial_connection_retry_delay_ms` has a **minimum of 100ms** (see `manifest.yaml`).
- **`communication_*`**:
  - Used for regular Modbus operations during runtime.
  - `communication_retry_delay_ms` has a **minimum of 10ms** (see `manifest.yaml`).
- **`communication_error_pause_delay_s`**:
  - After a communication exception in the live thread, the module waits this long before retrying.
  - If the line is physically broken (wrong wiring / adapter unplugged), increasing this value reduces log spam.
- **`live_measurement_interval_ms` / `device_state_read_interval_ms`**:
  - Keep live measurements reasonable for your bus speed and number of devices.
  - For multi-drop RS-485, consider increasing intervals if you see bus contention/timeouts.

## Interfaces

### Provides

- `main`: `powermeter`

### Requires

- `modbus`: `serial_communication_hub`

## Transaction flow (OCMF)

### `start_transaction`

High-level flow:

1. Read OCMF state register and ensure it is `NOT_READY` before starting.
2. Write OCMF transaction registers:
   - Identification status/level/flags/type
   - Identification data (ID)
   - Charging point identifier type + value (EVSE ID)
   - Tariff text (TT) as `tariff_text + "<=>" + transaction_id`
     - Written as **0-terminated** and only the **used** portion (no full padding).
3. Write session modality (charging vehicle).
4. Write the start command (`'B'`).

### `stop_transaction`

High-level flow:

1. Write end command (`'E'`) if stopping the currently tracked transaction.
2. Wait for OCMF state `READY`.
3. Read OCMF file (size + content).
4. Confirm file read by writing `NOT_READY` to OCMF state.
5. Return OCMF report in `signed_meter_value` (with `public_key` attached).

## Signature validation (recommended)

For troubleshooting and integration testing, this module ships a small signature validation tool under:
`modules/HardwareDrivers/PowerMeters/CarloGavazzi_EM580/ocmf_validation/`.

It can validate EM580 OCMF signatures (`ECDSA-brainpoolP384r1-SHA256`) against the public key read from the meter.
See `ocmf_validation/README.md` for usage details.

## Troubleshooting

### No communication / timeouts

Common causes and checks:

- **Wrong unit id**:
  - Verify `powermeter_device_id` matches the meter’s Modbus address.
  - If you have multiple meters, ensure each has a unique id (1..247 are typical).
- **Wrong serial settings**:
  - Ensure `SerialCommHub` settings match the meter configuration (baudrate, parity).
  - If you are unsure, start with conservative settings and increase once stable.
- **RS-485 wiring / termination**:
  - Swap A/B if you see only timeouts.
  - Ensure termination is correct (only at bus ends).
  - Keep cables short / twisted pair; avoid star topologies where possible.
- **Adapter / permissions**:
  - Confirm the serial device path exists and is stable (`/dev/ttyUSB0` can change between boots).
  - Ensure the EVerest process has permissions to open the device node.

### Repeated `CommunicationFault` raises

The module raises a communication fault when Modbus operations fail and clears it once communication is restored.
If you see frequent toggling:

- Reduce bus load (increase `live_measurement_interval_ms`, especially with multiple meters)
- Increase `communication_retry_count` modestly (and keep `communication_retry_delay_ms` ≥ 10ms)
- Consider increasing `communication_error_pause_delay_s` to reduce retry storms on hard faults

### OCMF transaction does not complete / stuck waiting for READY

- Check that the meter is in the expected OCMF state and is not holding a previous transaction open.
- Ensure the transaction ids passed to `start_transaction` / `stop_transaction` match your intended flow.
- Inspect logs around OCMF state transitions and file readout to see which step fails.

### Tariff text (TT) is truncated

The EM580 TT field is limited to `CHAR[252]`. The driver logs a warning and truncates overlong strings.
Shorten `tariff_text` and/or the appended data (e.g. transaction id formatting).

### Signature verification fails

Use the validation tool in `ocmf_validation/` to validate the produced OCMF string against the published public key.
If it fails:
- Ensure public key and OCMF data come from the **same device** and **same transaction**
- Ensure the OCMF data is not modified (OCMF signatures are over compact JSON)

## Notes / Limitations

- **Write-multiple-registers limit**: the Modbus transport enforces the protocol limit by chunking into max 123 registers.
- **Tariff text length**: TT is a `CHAR[252]` field (126 words). The driver logs a warning and truncates if needed.

## References

- Module docs: `modules/HardwareDrivers/PowerMeters/CarloGavazzi_EM580/docs/index.rst`
- OCMF spec: see [SAFE-eV OCMF specification](https://github.com/SAFE-eV/OCMF-Open-Charge-Metering-Format)

## Unit tests

Unit tests live under `modules/HardwareDrivers/PowerMeters/CarloGavazzi_EM580/tests/` and include:

- Helper-level tests (`helper.hpp`)
- `powermeterImpl` behavior tests using a fake Modbus transport and small test hooks

Build/run example (target name may vary by build system settings):

```bash
ninja -C build everest-core_carlo_gavazzi_em580_helper_tests
./build/modules/HardwareDrivers/PowerMeters/CarloGavazzi_EM580/tests/everest-core_carlo_gavazzi_em580_helper_tests
```


