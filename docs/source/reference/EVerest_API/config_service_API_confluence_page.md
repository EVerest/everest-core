source: ConfigServiceAPI confluence page (2026-04-09)

# API Reference

## Types

### SlotId

is an int “numeric index”

### ConfigMetadata

- <SlotId> slot_id
- <tstamp-string> last_updated
- <bool> is_valid
- {<string> description} “optional human-readable description” 
- {<string> config_file_path}

### ListSlotIdsResult

- <array>
  - <ConfigMetadata>

### GetActiveSlotIdResult

- {<SlotId>} slot_id

### enum SetActiveSlotStatus “{Success|AlreadyActive|DoesNotExist|NotAllowed}”

### enum StopModulesResult “{Stopping|NoModulesToStop|NotAllowed}“

### enum RestartModulesResult “{Starting|Restarting|NoConfigToStart|NotAllowed}“

### enum DeleteSlotStatus “{Success|CannotDeleteActiveSlot|DoesNotExist|NotAllowed}”

### DuplicateSlotResult

- <bool> success
- {<SlotId>} slot_id “optional; included if status==Success”

### LoadFromYamlResult

- <bool> success
- {<SlotId>} slot_id “optional; included if status==Success”

### enum ApplyMode “{Immediate|OnRestart|Auto}”

### ConfigurationParameterIdentifier

- <string> module_id
- <string> parameter_name
- {<string>} implementation_id

### ConfigurationParameterUpdate

- <ConfigurationParameterIdentifier> cfg_param_id
- <string> value
- <ApplyMode> apply_mode

### enum SetConfigurationParameterResult “{Applied|WillApplyOnRestart|DoesNotExist|Rejected}”

### enum StartModulesResult “{Starting|Restarting|NoValidConfiguration}”

### enum ActiveSlotStatus “{Running|FailedToStart|RestartTriggered}”

### ActiveSlotUpdate

- <tstamp-string> tstamp
- <SlotId> slot_id
- <ActiveSlotStatus> status

### ConfigurationParameterUpdateNotice

- <ConfigurationParameterIdentifier> cfg_param_id
- <string> value
- <SetConfigurationParameterResult> result “only ‘Applied’ and ‘WillApplyOnRestart’ will be used”

### ConfigurationUpdate

- <tstamp-string> tstamp
- <SlotId> slot_id
- <array> updates
  - <ConfigurationParameterUpdateNotice>

### enum ExecutionStatus “{Running|NotRunning|ConfigServiceOnly}“

### Mapping

- <int> evse
- {<int>} connector

### ImplMapping

- <string> implementation_id
- <Mapping> mapping

### ModuleTierMappings

- <Mapping> module
- <array> implementations
  - <ImplMapping>

### ReqFulfillment

- <string> module_id
- <string> implementation_id
- <int> index of fulfilled requirement

### ModuleConnection

- <string> requirement_id
- <array> fulfillments
  - <ReqFulfillment>

### ModuleConnections

- <array> connections
  - <ModuleConnection>

### enum ConfigurationParameterDatatype “{String|Decimal|Integer|Boolean}”

### enum ConfigurationParameterMutability “{ReadOnly|ReadWrite|WriteOnly}”

### enum ConfigurationActivationPolicy “{Immediate|RequiresRestart}”

### ConfigurationParameterCharacteristics

- <ConfigurationParameterDatatype> datatype
- <ConfigurationParameterMutability> mutability
- <ConfigurationActivationPolicy> activation_policy
- {<string>} unit

### ConfigurationParameter

- <string> name
- <string> value
- <ConfigurationParameterCharacteristics> characteristics

### ImplementationConfigurationParameter

- <string> implementation_id
- <array> configuration_parameters
  - <ConfigurationParameter>

### ModuleConfiguration

- <string> module_id
- <string> module_name
- <ModuleConnections> connections
- {<ModuleTierMappings>} mapping
- <array> module_configuration_parameters
  - <ConfigurationParameter>
- <array> implementation_configuration_parameters
  - <ImplementationConfigurationParameter>

### enum GetConfigurationStatus “{Success|SlotDoesNotExist}”

### GetConfigurationResult

- <GetConfigurationStatus> status
- {<array>} module_configurations “optional; included if status==Success”
  - <ModuleConfiguration>

## Commands

### configuration slot selection

- list_all_slots() → <ListSlotIdsResult>
- get_active_slot() → <GetActiveSlotIdResult>
- mark_active_slot(<SlotId>) → <SetActiveSlotStatus>

### execution control

- stop_modules() → <StopModulesResult>
- restart_modules() → <RestartModulesResult>

### configuration slot creation/destruction

- delete_slot(<SlotId>) → <DeleteSlotStatus> “Returns Success if deleted; otherwise returns the specific error enum.”
- duplicate_slot(<SlotId>, {<string> new_description}) → <DuplicateSlotResult> “Returns Success if duplicated; otherwise returns the specific error enum.”
- load_from_yaml(<string> raw_yaml) → <LoadFromYamlResult> “Returns status and new slot_id on success“

### configuration internals

- set_config_parameters(<SlotId>, [<ConfigurationParameterUpdate>]) -> [<SetConfigurationParameterResult>] “Returns a list of results, mapping 1-1 to the list of updates given as argument“
- get_configuration(<SlotId>) →  <GetConfigurationResult> “Returns the complete module configuration tree, or an error status if the slot does not exist.”

## Variables

### active_slot

Published when

- EVerest succeeded in starting the modules using some slot_id
- EVerest failed to start the modules using some slot_id
- A switch to another slot_id is triggered (restart is triggered)

Type: <ActiveSlotUpdate>

### config_updates

Published when

- the configuration parameters of any slot_id change

Type: <ConfigurationUpdate>

### execution_status

Published when

- execution status changes between Running/NotRunning/OnlyConfigService

Type: <ExecutionStatus>

retained topic (uses LWT to set to NotRunning)

# Examples

Examples are given in YAML as in AsyncAPI definitions. Real serialized data will be JSON.

## Commands

### list_all_slots() [Query]

Retrieve a list of all configuration slots currently stored in the database.

Arguments: None

Returns: ListSlotIdsResult (Array of ConfigMetadata)

Example Return:

```yaml
- slot_id: 1
  last_updated: "2026-04-08T16:00:00Z"
  is_valid: true
  description: "Production_AC_v1"
  config_file_path: "/var/opt/everest/slots/slot_1.yaml"
- slot_id: 2
  last_updated: "2026-03-15T09:30:00Z"
  is_valid: false
  description: "Testing_DC_Fast"
```


### get_active_slot() [Query]

Retrieve the slot_id of the configuration slot currently marked as active.

Arguments: None

Returns: GetActiveSlotIdResult (optional of SlotId)

Example Return:

```yaml
slot_id: 1
```


### mark_active_slot(slot_id) [Mutation]

Flag the specified slot_id to be used as the active configuration upon the next restart.

Arguments: SlotId

Returns: SetActiveSlotStatus

Example Return:

  "DoesNotExist"



### stop_modules() [Execution]

Instruct EVerest to stop all currently running modules.

Arguments: None

Returns: StopModulesResult

Example Return

  "NoModulesToStop"



### restart_modules() [Execution]

Instruct EVerest to restart all modules, or start them if they are currently stopped.

Arguments: None

Returns: RestartModulesResult

Example Return:

  "Restarting"



### delete_slot(slot_id) [Mutation]

Delete the specified configuration slot. Fails if the slot is currently active.

Arguments: SlotId

Returns: DeleteSlotStatus

Example Return:

  "CannotDeleteActiveSlot"



### duplicate_slot(slot_id, new_description) [Mutation]

Duplicate an existing configuration slot, optionally applying a new description.

Arguments: SlotId, string

Returns: DuplicateSlotResult

Example Argument:

```yaml
slot_id: 1
new_description: "Backup of Config XYZ"
```

Example Return:

```yaml
success: true
slot_id: 5
```


### load_from_yaml(raw_yaml) [Mutation]

Create a new configuration slot populated with the configuration data provided in the YAML string.

Arguments: string

Returns: LoadFromYamlResult

Example Argument:

```yaml
active_modules:
  store:
    module: Store
  example:
    config_implementation:
      example:
        current: 42
        enum_test: one
        enum_test2: 2
    connections:
      kvs:
        - module_id: store
          implementation_id: main
    module: Example
  example_user:
    connections:
      example:
        - module_id: example
          implementation_id: example
    module: ExampleUser"
```

Example Return:

```yaml
success: true
slot_id: 5
```


### set_config_parameters(slot_id, parameter_updates) [Mutation]

Update one or more configuration parameters within the specified slot.

Arguments: SlotId, Array<ConfigurationParameterUpdate>

Returns: Array<SetConfigurationParameterResult>

Example Argument:

```yaml
slot_id: 1
parameter_updates:
  - cfg_param_id:
      module_id: "evse_manager_main"
      parameter_name: "max_current"
    value: "16"
    apply_mode: "OnRestart"
  - cfg_param_id:
      module_id: "ocpp"
      parameter_name: "ChargePointId"
      implementation_id: "main"
    value: "CP_001"
    apply_mode: "Immediate"
```

Example Return:

```yaml
- "WillApplyOnRestart"
- "Applied"
```


### get_configuration(slot_id) [Query]

Retrieve the complete module configuration tree for the specified slot.

Arguments: SlotId

Returns: GetConfigurationResult

Example Return:

```yaml
status: "Success"
module_configurations:
  - module_id: "evse_manager_main"
    module_name: "EvseManager"

    # <ModuleConnections>
    connections:
      - requirement_id: "board_support"
        fulfillments:
          - module_id: "yeti_board"
            implementation_id: "board_support"
            index: 0

    # {<ModuleTierMappings>} (Optional)
    mapping:
      module:
        evse: 1
        connector: 1
      implementations:
        - implementation_id: "board_support"
          mapping:
            evse: 1
            connector: 1

    # <array> module_configuration_parameters
    module_configuration_parameters:
      - name: "session_logging"
        value: "true"
        characteristics:
          datatype: "Boolean"
          mutability: "ReadWrite"
          activation_policy: "RequiresRestart"

    # <array> implementation_configuration_parameters
    implementation_configuration_parameters:
      - implementation_id: "board_support"
        configuration_parameters:
          - name: "max_current"
            value: "32"
            characteristics:
              datatype: "Integer"
              mutability: "ReadWrite"
              activation_policy: "Immediate"
              unit: "A"
  - module_id: "yeti_board"
    module_name: ...
    ...
```


## Variables

### active_slot

```yaml
tstamp: "2026-03-30T17:45:00Z"
slot_id: 1
status: "RestartTriggered"
```

### config_updates

```yaml
tstamp: "2026-03-30T17:45:00Z"
updates:
  - cfg_param_id:
      module_id: "evse_1"
      parameter_name: "max_current"
    value: "16"
    result: "WillApplyOnRestart"
  - cfg_param_id:
      ...
slot_id: 1
```

### execution_status

retained topic; LWT set to publish “NotRunning”

```yaml
"ConfigServiceOnly"
```

