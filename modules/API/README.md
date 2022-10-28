# API module documentation
This module is responsible for providing a simple MQTT based API to EVerest internals

## Periodically published variables
This module periodically publishes the following variables for each connected EvseManager.

### everest_api/evse_manager/var/datetime
This variable is published every second and contains a string representation of the current UTC datetime in RFC3339 format:
```
2022-10-11T16:18:57.746Z
```

### everest_api/evse_manager/var/hardware_capabilities
This variable is published every second and contains the hardware capabilities in the following format:
```json
    {
        "max_current_A": 32.0,
        "max_phase_count": 3,
        "min_current_A": 6.0,
        "min_phase_count": 1,
        "supports_changing_phases_during_charging": true
    }
```

### everest_api/evse_manager/var/session_info
This variable is published every second and contains a json object with information relating to the current charging session in the following format:
```json
    {
        "charged_energy_wh": 0,
        "charging_duration_s": 84,
        "datetime": "2022-10-11T16:48:35.747Z",
        "latest_total_w": 0.0,
        "state": "Unplugged",
        "state_info": ""
    }
```

- **charged_energy_wh** contains the charged energy in Wh
- **charging_duration_s** contains the duration of the current charging session in seconds
- **datetime** contains a string representation of the current UTC datetime in RFC3339 format
- **latest_total_w** contains the latest total power reading over all phases in Watt
- **state** contains the current state of the charging session, from a list of the following possible states:
    - Disabled
    - Unplugged
    - Preparing
    - Reserved
    - AuthRequired
    - Charging
    - ChargingPausedEV
    - ChargingPausedEVSE
    - Finished
    - Error
    - PermanentFault
- **state_info** contains additional information for the current state, at the moment this is only set to a meaningful value in the Error state. Here it can have the following values:
    - Car
    - CarDiodeFault
    - Relais
    - RCD
    - VentilationNotAvailable
    - OverCurrent
    - Internal
    - SLAC
    - HLC

### everest_api/evse_manager/var/limits
This variable is published every second and contains a json object with information relating to the current limits of this EVSE.
```json
    {
        "max_current": 16.0,
        "nr_of_phases_available": 1,
        "uuid": "evse_manager"
    }
```

### everest_api/evse_manager/var/telemetry
This variable is published every second and contains telemetry of the EVSE.
```json
    {
        "fan_rpm": 0.0,
        "rcd_current": 0.0991784930229187,
        "relais_on": false,
        "supply_voltage_12V": 11.950915336608887,
        "supply_voltage_minus_12V": -11.94166374206543,
        "temperature": 30.729248046875
    }
```

### everest_api/evse_manager/var/telemetry
This variable is published every second and contains powermeter information of the EVSE.
```json
    {
        "current_A": {
            "L1": 16.113445281982422,
            "L2": 16.113445281982422,
            "L3": 16.113445281982422,
            "N": 0.20141807198524475
        },
        "energy_Wh_import": {
            "L1": 1537.3179931640625,
            "L2": 1537.3179931640625,
            "L3": 1537.3179931640625,
            "total": 4611.9541015625
        },
        "frequency_Hz": {
            "L1": 50.03734588623047,
            "L2": 50.03734588623047,
            "L3": 50.03734588623047
        },
        "meter_id": "YETI_POWERMETER",
        "phase_seq_error": false,
        "power_W": {
            "L1": 3602.54833984375,
            "L2": 3602.54833984375,
            "L3": 3602.54833984375,
            "total": 10807.64453125
        },
        "timestamp": 1665509120.0,
        "voltage_V": {
            "L1": 223.5740509033203,
            "L2": 223.5740509033203,
            "L3": 223.5740509033203
        }
    }
```

## Commands and variables published in response
### everest_api/evse_manager/cmd/pause_charging
If any arbitrary payload is published to this topic charging will be paused by the EVSE.

### everest_api/evse_manager/cmd/resume_charging
If any arbitrary payload is published to this topic charging will be paused by the EVSE.
