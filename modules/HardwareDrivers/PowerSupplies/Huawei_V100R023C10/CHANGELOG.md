# Changelog

## December 2025

### Module

- The module can now publish telemetry data on a specified mqtt base topic, set via the config option `telemetry_topic_prefix`. The concrete telemetry data is published only when the data changes to reduce mqtt traffic. The telemetry data is published as json objects per dispenser and per connector. See the module documentation for details.
- The module now supports setting some specific BSP errors to the PSU as dispenser and connector alarms:
  - Per dispenser:
    - `evse_board_support/EnclosureOpen` set as Door status alarm to the PSU
    - `evse_board_support/WaterIngressDetected` set as Water alarm
    - `evse_board_support/MREC8EmergencyStop` set as EPO alarm
    - `evse_board_support/TiltDetected` set as Tilt alarm
  - Per connector:
    - `evse_board_support/MREC17EVSEContactorFault` set as DC output contactor fault

## June 2025

- Module
  - The module now verifies the HMAC of received goose messages by default (this was not the case before). This can be disabled with the module config `verify_secure_goose: false`
  - The modules' goose security options are now finer grained. `secure_goose` has been split into `send_secure_goose`, which controls the security of outgoing messages, `allow_insecure_goose` and `verify_secure_goose`, which control the security of incoming messages. See manifest.yaml for more details
  - Module allocation failure (including module allocation response timeout) is now treated as a warning instead of an error
  - Some info messages have been changed to debug messages to reduce log noise
  - Capabilities are now used from the Powersupply instead of hardcoded values. An error is raised as long as the capabilities are not set by the Powersupply. When the powersupply communication fails, the stored capabilities are cleared and the error is raised again.
  - The Ethernet socket now filters the received packages on kernel level to improve performance
  - Adds a hack to use voltage readings from a over voltage monitor during cable check. For this enough voltage monitors must be configured and the config option `HACK_use_ovm_while_cable_check` must be enabled.
  - Adds `upstream_voltage_source` config option to select which upstream voltage source to use.
- Mock
  - The mock can now accept multiple connections to simulate multiple dispensers
  - The mock has new options to enable or disable the security of incoming and outgoing goose messages. These are accessible via the environment variables `FUSION_CHARGER_MOCK_DISABLE_SEND_HMAC` and `FUSION_CHARGER_MOCK_DISABLE_VERIFY_HMAC`
  - The mock has a new option to change the ethernet interface used for goose messages. This is accessible via the environment variable `FUSION_CHARGER_MOCK_ETH`
  - A bug that caused the mock to use the wrong hmac key has been fixed
  - The mock now waits 5 seconds before sending the capabilities to reflect the real hardware behavior better
  - The mock can now send received power requirements to a mqtt broker. For this, set hte environment variables `FUSION_CHARGER_MOCK_MQTT_HOST` and `FUSION_CHARGER_MOCK_MQTT_PORT` and optionally `FUSION_CHARGER_MOCK_MQTT_BASE_TOPIC` (defaults to `fusion_charger_mock/`). The mock then publishes under `{base_topic}/{global connector number}/power_request` a json object with `{"voltage": <voltage>, "current": <current>}`
