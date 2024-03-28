# API module documentation
This module is responsible for providing a simple MQTT based API to EVerest internals

## Periodically published variables for each connected EvseManager
This module periodically publishes the following variables for each connected EvseManager.

### everest_api/connectors
This variable is published every second and contains an array of the connectors for which the api is available:
```
["evse_manager"]
```

The following documentation assumes that the only connector available is called "evse_manager".

### everest_api/evse_manager/var/datetime
This variable is published every second and contains a string representation of the current UTC datetime in RFC3339 format:
```
2022-10-11T16:18:57.746Z
```

### everest_api/evse_manager/var/hardware_capabilities
This variable is published every second and contains the hardware capabilities in the following format:
```json
    {
        "max_current_A_export":16.0,
        "max_current_A_import":32.0,
        "max_phase_count_export":3,
        "max_phase_count_import":3,
        "min_current_A_export":0.0,
        "min_current_A_import":6.0,
        "min_phase_count_export":1,
        "min_phase_count_import":1,
        "supports_changing_phases_during_charging":true
    }
```

### everest_api/evse_manager/var/session_info
This variable is published every second and contains a json object with information relating to the current charging session in the following format:
```json
    {
        "charged_energy_wh": 0,
        "charging_duration_s": 84,
        "datetime": "2022-10-11T16:48:35.747Z",
        "discharged_energy_wh": 0,
        "latest_total_w": 0.0,
        "state": "Unplugged",
        "active_permanent_faults": [],
        "active_errors": [],
        "active_enable_disable_source": {
            "source": "Unspecified",
            "state": "Enable",
            "priority": 5000
        },
        "uk_random_delay": {
            "remaining_s": 34,
            "current_limit_after_delay_A": 16.0,
            "current_limit_during_delay_A": 0.0,
            "start_time": "2024-02-28T14:11:11.129Z"
        },
        "last_enable_disable_source": "Unspecified",
        "enable_disable_allow_others_to_modify": true
    }
```

Example with permanent faults being active:

```json
{
  "active_errors": [],
  "active_permanent_faults": [
    {
      "description": "The control pilot voltage is out of range.",
      "severity": "High",
      "type": "MREC14PilotFault"
    },
    {
      "description": "The vehicle is in an invalid mode for charging (Reported by IEC stack)",
      "severity": "High",
      "type": "MREC10InvalidVehicleMode"
    }
  ],
  "charged_energy_wh": 0,
  "charging_duration_s": 0,
  "datetime": "2024-01-15T14:58:15.172Z",
  "discharged_energy_wh": 0,
  "latest_total_w": 0,
  "state": "Preparing",
  "last_enable_disable_source": "Unspecified",
  "enable_disable_other_sources_may_modify": true
}
```

- **charged_energy_wh** contains the charged energy in Wh
- **charging_duration_s** contains the duration of the current charging session in seconds
- **datetime** contains a string representation of the current UTC datetime in RFC3339 format
- **discharged_energy_wh** contains the energy fed into the power grid by the EV in Wh
- **latest_total_w** contains the latest total power reading over all phases in Watt
- **uk_random_delay_remaining_s** Remaining time of a currently active random delay according to UK smart charging regulations. Not set if no delay is active.
- **state** contains the current state of the charging session, from a list of the following possible states:
    - Unplugged
    - Disabled
    - Preparing
    - Reserved
    - AuthRequired
    - WaitingForEnergy
    - Charging
    - ChargingPausedEV
    - ChargingPausedEVSE
    - Finished

- **active_permanent_faults** array of all active errors that are permanent faults (i.e. that block charging). If anything is set here it should be shown as an error to the user instead of showing the current state:
    - RCD_Selftest
    - RCD_DC
    - RCD_AC
    - VendorError
    - VendorWarning
    - ConnectorLockCapNotCharged
    - ConnectorLockUnexpectedOpen
    - ConnectorLockUnexpectedClose
    - ConnectorLockFailedLock
    - ConnectorLockFailedUnlock
    - MREC1ConnectorLockFailure
    - MREC2GroundFailure
    - MREC3HighTemperature
    - MREC4OverCurrentFailure
    - MREC5OverVoltage
    - MREC6UnderVoltage
    - MREC8EmergencyStop
    - MREC10InvalidVehicleMode
    - MREC14PilotFault
    - MREC15PowerLoss
    - MREC17EVSEContactorFault
    - MREC18CableOverTempDerate
    - MREC19CableOverTempStop
    - MREC20PartialInsertion
    - MREC23ProximityFault
    - MREC24ConnectorVoltageHigh
    - MREC25BrokenLatch
    - MREC26CutCable
    - DiodeFault
    - VentilationNotAvailable
    - BrownOut
    - EnergyManagement
    - PermanentFault
    - PowermeterTransactionStartFailed

- **active_errors** array of all active errors that do not block charging. 
This could be shown to the user but the current state should still be shown 
as it does not interfere with charging. The enum is the same as for active_permanent_faults.

### everest_api/evse_manager/var/limits
This variable is published every second and contains a json object with information 
relating to the current limits of this EVSE.
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

### everest_api/evse_manager/var/powermeter
This variable is published every second and contains powermeter information
of the EVSE.
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

## Periodically published variables for OCPP

### everest_api/ocpp/var/connection_status
This variable is published every second and contains the connection 
status of the OCPP module.
If the OCPP module has not yet published its "is_connected" status or 
no OCPP module is configured "unknown" is published. Otherwise "connected" 
or "disconnected" are published.


## Commands and variables published in response
### everest_api/evse_manager/cmd/enable_disable
Command to enable or disable a connector on the EVSE. The payload should be 
the following json:

```json
    {
        "connector_id": 0,
        "source": "LocalAPI",
        "state": "Enable",
        "priority": 42
    }
```
connector_id is a positive integer identifying the connector that should be 
enabled. If the connector_id is 0 the whole EVSE is enabled.

The source is an enum of the following source types :

    - Unspecified
    - LocalAPI
    - LocalKeyLock
    - ServiceTechnician
    - RemoteKeyLock
    - MobileApp
    - FirmwareUpdate
    - CSMS

The state can be either "enable", "disable", or "unassigned".

"enable" and "disable" enforce the state to be enable/disable, while unassigned means 
that the source does not care about the state and other sources may decide.

Each call to this command will update an internal table that looks like this:

| Source       | State      | Priority |
| ------------ | ---------- | -------- |
| Unspecified  | unassigned | 10000    |
| LocalAPI     | disable    | 42       |
| LocalKeyLock | enable     | 0        |

Evaluation will be done based on priorities. 0 is the highest priority, 
10000 the lowest, so in this example the connector will be enabled regardless 
of what other sources say.
Imagine LocalKeyLock sends a "unassigned, prio 0", the table will then look like this:

| Source       | State      | Priority |
| ------------ | ---------- | -------- |
| Unspecified  | unassigned | 10000    |
| LocalAPI     | disable    | 42       |
| LocalKeyLock | unassigned | 0        |

So now the connector will be disabled, because the second highest priority (42) sets it to disabled.

If all sources are unassigned, the connector is enabled.
If two sources have the same priority, "disabled" has priority over "enabled".

### everest_api/evse_manager/cmd/enable
Legacy command to enable a connector on the EVSE kept for compatibility reasons.
They payload should be a positive integer identifying the connector that should be enabled.
If the payload is 0 the whole EVSE is enabled.
It will actually call the following command on everest_api/evse_manager/cmd/enable_enable:
```json
    {
        "connector_id": 1,
        "source": "LocalAPI",
        "state": "enable",
        "priority": 0
    }
```

### everest_api/evse_manager/cmd/disable
Legacy command to enable a connector on the EVSE kept for compatibility reasons.
Command to disable a connector on the EVSE. They payload should be a positive integer 
identifying the connector that should be disabled. If the payload is 0 the whole EVSE is disabled.
It will actually call the following command on everest_api/evse_manager/cmd/enable_disable:
```json
    {
        "connector_id": 1,
        "source": "LocalAPI",
        "state": "disable",
        "priority": 0
    }
```

### everest_api/evse_manager/cmd/pause_charging
If any arbitrary payload is published to this topic charging will be paused by the EVSE.

### everest_api/evse_manager/cmd/resume_charging
If any arbitrary payload is published to this topic charging will be paused by the EVSE.

### everest_api/evse_manager/cmd/set_limit_amps
Command to set an amps limit for this EVSE that will be considered within the EnergyManager. This does not automatically imply that this limit will be set by the EVSE because the energymanagement might consider limitations from other sources, too. The payload can be a positive or negative number. 

### everest_api/evse_manager/cmd/set_limit_watts
Command to set a watt limit for this EVSE that will be considered within the EnergyManager. This does not automatically imply that this limit will be set by the EVSE because the energymanagement might consider limitations from other sources, too. The payload can be a positive or negative number.

### everest_api/evse_manager/cmd/force_unlock
Command to force unlock a connector on the EVSE. They payload should be a positive integer identifying the connector that should be unlocked. If the payload is empty or cannot be converted to an integer connector 1 is assumed.

### everest_api/evse_manager/cmd/uk_random_delay
Command to control the UK Smart Charging random delay feature. The payload can be the following enum: "enable" and "disable" to enable/disable the feature entirely or "cancel" to cancel an ongoing delay.

### everest_api/evse_manager/cmd/uk_random_delay_set_max_duration_s
Command to set the UK Smart Charging random delay maximum duration. Payload is an integer in seconds.
