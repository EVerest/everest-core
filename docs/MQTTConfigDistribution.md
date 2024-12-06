# Module configuration distributed via MQTT

Since everest-framework 0.19.0 the module configuration is parsed once
by the manager and then distributed to the modules via MQTT.
This is achieved by parsing the MQTT settings from the config,
spawning the modules and passing these MQTT settings to them.
The modules themselves then ask for their module config via MQTT,
which is in turn provided to them from the manager.
After the modules have received their config, their init() function is called.
Afterwards they signal ready to the manager.
The manager sends out the global ready signal
once it has received all Module ready signals.

The following sequence diagram illustrates this startup process

```mermaid
sequenceDiagram
    create participant manager
    create participant ManagerSettings
    manager-)ManagerSettings: ManagerSettings(prefix, config_path)
    ManagerSettings-->>manager: return ms
    create participant ManagerConfig
    manager-)ManagerConfig: ManagerConfig(ms)
    create participant MQTTAbstraction
    manager-)MQTTAbstraction: MQTTAbstraction(ms.mqtt_settings)
    MQTTAbstraction-->>manager: return mqtt_abstraction
    activate manager
    manager->>manager: start_modules()
    manager->>ManagerConfig: serialize()
    ManagerConfig-->>manager: serialized_config
    manager->>MQTTAbstraction: publish(interfaces, types, schemas, manifests, settings, retain=true)
    loop For every module
        manager->>manager: spawn_modules(Module)
        create participant Module
        manager->>Module: spawn Module
        Module->>MQTTAbstraction: get(Config)
        MQTTAbstraction->>manager: get(Config of Module)
        manager-->>MQTTAbstraction: publish(module configs, mappings)
        MQTTAbstraction-->>Module: publish(module configs, mappings)
        Module->>Module: init
        Module->>MQTTAbstraction: publish(ready)
        MQTTAbstraction->>manager: publish(ready of Module)
    end
    manager->>MQTTAbstraction: publish global ready
```

Class diagram

```mermaid
classDiagram
ConfigBase <|-- ManagerConfig
ConfigBase <|-- Config
MQTTSettings *-- ConfigBase
ManagerSettings *-- ManagerConfig

note for ConfigBase "
Baseclass containing json config, manifests, interfaces,
types and functions to access this information which
needs to be available in all derived classes
"

class ManagerSettings{
    +fs::path configs_dir
    +fs::path schemas_dir
    +fs::path interfaces_dir
    +fs::path types_dir
    +fs::path errors_dir
    +fs::path config_file
    +fs::path www_dir
    +int controller_port
    +int controller_rpc_timeout_ms
    +std::string run_as_user
    +std::string version_information
    +nlohmann::json config
    +MQTTSettings mqtt_settings
    +std::unique_ptr<RuntimeSettings> runtime_settings
    +ManagerSettings(const std::string& prefix, const std::string& config)
    +const RuntimeSettings& get_runtime_settings()
}

class MQTTSettings{
    +std::string broker_socket_path
    +std::string broker_host
    +int broker_port
    +std::string everest_prefix
    +std::string external_prefix
    +bool uses_socket()
}

class ConfigBase{
    #const MQTTSettings mqtt_settings
    +ConfigBase(const MQTTSettings& mqtt_settings)
}

class ManagerConfig{
    -const ManagerSettings& ms
    +ManagerConfig(const ManagerSettings& ms)
    +nlohmann::json serialize()
    -load_and_validate_manifest(const std::string& module_id, const nlohmann::json& module_config)
    -std::tuple~nlohmann::json, int64_t~ load_and_validate_with_schema(const fs::path& file_path, const nlohmann::json& schema)
    -nlohmann::json resolve_interface(const std::string& intf_name)
    -nlohmann::json load_interface_file(const std::string& intf_name)
    -resolve_all_requirements()
    -parse(nlohmann::json config)
}

class Config{
    +Config(const MQTTSettings& mqtt_settings, nlohmann::json config)
    +bool module_provides(const std::string& module_name, const std::string& impl_id);
    +nlohmann::json get_module_cmds(const std::string& module_name, const std::string& impl_id)
    +nlohmann::json resolve_requirement(const std::string& module_id, const std::string& requirement_id)
    +std::list~Requirement~ get_requirements(const std::string& module_id)
    +RequirementInitialization get_requirement_initialization(const std::string& module_id)
    +ModuleConfigs get_module_configs(const std::string& module_id)
    +nlohmann::json get_module_json_config(const std::string& module_id)
    +ModuleInfo get_module_info(const std::string& module_id)
    +std::optional~<~TelemetryConfig~ get_telemetry_config()
    +nlohmann::json get_interface_definition(const std::string& interface_name) const;
}
```
