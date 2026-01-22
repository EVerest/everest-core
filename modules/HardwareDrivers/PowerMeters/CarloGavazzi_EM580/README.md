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

## Configuration

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

### Configuration parameters

All parameters are defined in `modules/HardwareDrivers/PowerMeters/CarloGavazzi_EM580/manifest.yaml`:

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `powermeter_device_id` | integer | `1` | Modbus device ID on the bus |
| `communication_retry_count` | integer | `3` | Retries for regular Modbus operations |
| `communication_retry_delay_ms` | integer | `500` | Delay between regular retries |
| `communication_error_pause_delay_s` | integer | `10` | Pause after a communication failure in live thread before retrying |
| `initial_connection_retry_count` | integer | `10` (0 = infinite) | Retries during initial device setup/signature config reads |
| `initial_connection_retry_delay_ms` | integer | `2000` | Delay between initialization retries |
| `timezone_offset_minutes` | integer | `0` | Timezone offset from UTC (minutes) |
| `live_measurement_interval_ms` | integer | `1000` | Interval for reading/publishing live measurements |
| `device_state_read_interval_ms` | integer | `10000` | Interval for reading device-state bitfield (VendorError reporting) |

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

## Notes / Limitations

- **Write-multiple-registers limit**: the Modbus transport enforces the protocol limit by chunking into max 123 registers.
- **Tariff text length**: TT is a `CHAR[252]` field (126 words). The driver logs a warning and truncates if needed.

## Unit tests

Unit tests live under `modules/HardwareDrivers/PowerMeters/CarloGavazzi_EM580/tests/` and include:

- Helper-level tests (`helper.hpp`)
- `powermeterImpl` behavior tests using a fake Modbus transport and small test hooks

Build/run example (target name may vary by build system settings):

```bash
ninja -C build everest-core_carlo_gavazzi_em580_helper_tests
./build/modules/HardwareDrivers/PowerMeters/CarloGavazzi_EM580/tests/everest-core_carlo_gavazzi_em580_helper_tests
```


