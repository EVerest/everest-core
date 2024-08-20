# OCPP 2.0.1: Monitors

Monitors are a mechanism for reporting based on certain criteria the internal state of the variables present on the charger. The monitors can be configured in different ways, with custom monitors being sent from the CSMS and HardWired and Preconfigured monitors set up in the config of the database.


## Basic Configuration

The monitors are evaluated from time to time in the case of periodic monitors and after a variable has been modified in the case of monitors that are triggered. Periodic monitors will be handled from time to time, the default being 1 second.

### Variables:
- Enabling monitors: set the `MonitoringCtrlrEnabled` variable to true
- Periodic monitor process time: set the `MonitorsProcessingInterval` to the desired interval (default 1 second)
- To activate monitor processing: set the `ActiveMonitoringBase` variable to `All`
- To filter the verbosity level: set the `ActiveMonitoringLevel` variable to a value of 0-9 with 9 being the most verbose
- To filter the verbosity level when the charging station is offline: set the `OfflineQueuingSeverity` value to 0-9, with 9 keeping all monitor generated event while being offline

Note: There is a small overhead for the monitoring process interval. The periodic monitors that are triggered will require a database value query. However, based on the count and config of monitors it is unlikely that many of them will trigger at the same time, therefore, the database queries will be limited.

## Hardwired/Preconfigured Monitors

In order to set up pre-existing monitors that are not set up by the CSMS, for the variables that allow monitoring the configuration json file can be extended in the following way:

```
"EVSEPower": {
  "variable_name": "Power",
  "characteristics": {
    "unit": "W",
    "maxLimit": 22000,
    "supportsMonitoring": true,
    "dataType": "decimal"
  },
  "attributes": [
    {
      "type": "Actual",
      "mutability": "ReadOnly"
    },
    {
      "type": "MaxSet",
      "mutability": "ReadOnly"
    }
  ],
  "monitors": [
    {
      "value": 21950,
      "severity": 1,
      "transaction": false,
      "type": "UpperThreshold",
      "config_type": "HardWiredMonitor"
    },
    {
      "value": 100,
      "severity": 1,
      "transaction": false,
      "type": "LowerThreshold",
      "config_type": "HardWiredMonitor"
    },
    {
      "value": 100,
      "severity": 1,
      "transaction": false,
      "type": "Delta",
      "reference_value": "10700",
      "config_type": "PreconfiguredMonitor"
    }
  ],
  "description": "",
  "type": "number",
  "default": "0"
}
```

In the example for the 'EVSEPower' variable that supports monitoring there were attached three hardwired and preconfigured monitors. The monitors will report (based on the setup, see the `Basic Configuration` section) to the CSMS when the power will exceed '21950' W, when the power will fall below '100' W and when there will be a delta difference of more than `100` W from the `reference_value` in the case of the delta monitor. When the delta is exceeded the `reference_value` will be updated internally, and a new delta will be calculated based on that.

For more information related to the monitor functionality, please refer to the OCPP201 specification.

Note: for a delta monitor, an initial `reference_value` must be provided or the library will fail to initialize.