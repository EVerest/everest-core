// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef UTILS_CONFIG_HPP
#define UTILS_CONFIG_HPP

#include <filesystem>
#include <list>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>

#include <nlohmann/json-schema.hpp>

#include <utils/config_cache.hpp>
#include <utils/error.hpp>
#include <utils/error/error_type_map.hpp>
#include <utils/module_config.hpp>
#include <utils/mqtt_settings.hpp>
#include <utils/types.hpp>

namespace Everest {
namespace fs = std::filesystem;

struct ManagerSettings;
struct RuntimeSettings;

///
/// \brief A structure that contains all available schemas
///
struct Schemas {
    nlohmann::json config;                 ///< The config schema
    nlohmann::json manifest;               ///< The manifest scheme
    nlohmann::json interface;              ///< The interface schema
    nlohmann::json type;                   ///< The type schema
    nlohmann::json error_declaration_list; ///< The error-declaration-list schema
};

struct Validators {
    nlohmann::json_schema::json_validator config;
    nlohmann::json_schema::json_validator manifest;
    nlohmann::json_schema::json_validator type;
    nlohmann::json_schema::json_validator interface;
    nlohmann::json_schema::json_validator error_declaration_list;
};

struct SchemaValidation {
    Schemas schemas;
    Validators validators;
};

///
/// \brief Allowed format of a type URI, which are of a format like this /type_file_name#/TypeName
///
const static std::regex type_uri_regex{R"(^((?:\/[a-zA-Z0-9\-\_]+)+#\/[a-zA-Z0-9\-\_]+)$)"};

struct ImplementationInfo {
    std::string module_id;
    std::string module_name;
    std::string impl_id;
    std::string impl_intf;
};

///
/// \brief A simple json schema loader that uses the builtin draft7 schema of
/// the json schema validator when it encounters it, throws an exception
/// otherwise
void loader(const nlohmann::json_uri& uri, nlohmann::json& schema);

///
/// \brief An extension to the default format checker of the json schema
/// validator supporting uris
void format_checker(const std::string& format, const std::string& value);

///
/// \brief loads and validates a json schema at the provided \p path
///
/// \returns the loaded json schema as a json object as well as a related schema validator
std::tuple<nlohmann::json, nlohmann::json_schema::json_validator> load_schema(const fs::path& path);

///
/// \brief loads the config.json and manifest.json in the schemes subfolder of
/// the provided \p schemas_dir
///
/// \returns the loaded configs and related validators
SchemaValidation load_schemas(const fs::path& schemas_dir);

///
/// \brief Base class for configs
///
class ConfigBase {
protected:
    nlohmann::json main;
    nlohmann::json settings;

    nlohmann::json manifests;
    nlohmann::json interfaces;
    nlohmann::json interface_definitions;
    nlohmann::json types;
    Schemas schemas;

    std::unordered_map<std::string, ModuleTierMappings> tier_mappings;
    // experimental caches
    std::unordered_map<std::string, std::string> module_names;
    std::unordered_map<std::string, ConfigCache> module_config_cache;

    error::ErrorTypeMap error_map;

    const MQTTSettings mqtt_settings;

public:
    ///
    /// \brief Create a ConfigBase with the provided \p mqtt_settings
    explicit ConfigBase(const MQTTSettings& mqtt_settings) : mqtt_settings(mqtt_settings){};

    ///
    /// \brief turns then given \p module_id into a printable identifier
    ///
    /// \returns a string with the printable identifier
    std::string printable_identifier(const std::string& module_id) const;

    ///
    /// \brief turns then given \p module_id and \p impl_id into a printable identifier
    ///
    /// \returns a string with the printable identifier
    std::string printable_identifier(const std::string& module_id, const std::string& impl_id) const;

    ///
    /// \returns the module name matching the provided \p module_id
    std::string get_module_name(const std::string& module_id) const;

    ///
    /// \brief turns the given \p module_id and \p impl_id into a mqtt prefix
    ///
    std::string mqtt_prefix(const std::string& module_id, const std::string& impl_id);

    ///
    /// \brief turns the given \p module_id into a mqtt prefix
    ///
    std::string mqtt_module_prefix(const std::string& module_id);

    ///
    /// \returns a json object that contains the main config
    const nlohmann::json& get_main_config() const;

    ///
    /// \brief checks if the config contains the given \p module_id
    bool contains(const std::string& module_id) const;

    ///
    /// \returns a json object that contains the manifests
    const nlohmann::json& get_manifests() const;

    ///
    /// \returns a json object that contains the interface definitions
    const nlohmann::json& get_interface_definitions() const;

    ///
    /// \returns a json object that contains the available interfaces
    const nlohmann::json& get_interfaces() const;

    ///
    /// \returns a json object that contains the settings
    const nlohmann::json& get_settings() const;

    ///
    /// \returns a json object that contains the schemas
    const nlohmann::json get_schemas() const;

    ///
    /// \returns a json object that contains the schemas
    nlohmann::json get_error_types();

    ///
    /// \returns a json object that contains the types
    const nlohmann::json& get_types() const;

    ///
    /// \returns the module config cache
    std::unordered_map<std::string, ConfigCache> get_module_config_cache();

    ///
    /// \return the cached mapping of module ids to module names
    std::unordered_map<std::string, std::string> get_module_names();

    ///
    /// \brief checks if the given \p module_id provides the requirement given in \p requirement_id
    ///
    /// \returns a json object that contains the requirement
    nlohmann::json resolve_requirement(const std::string& module_id, const std::string& requirement_id) const;

    ///
    /// \brief resolves all Requirements of the given \p module_id to their Fulfillments
    ///
    /// \returns a map indexed by Requirements
    std::map<Requirement, Fulfillment> resolve_requirements(const std::string& module_id) const;

    ///
    /// \returns a list of Requirements for \p module_id
    std::list<Requirement> get_requirements(const std::string& module_id) const;

    ///
    /// \brief A Fulfillment is a combination of a Requirement and the module and implementation ids where this is
    /// implemented
    /// \returns a map of Fulfillments for \p module_id
    std::map<std::string, std::vector<Fulfillment>> get_fulfillments(const std::string& module_id) const;

    /// \returns the 3 tier model mappings
    std::unordered_map<std::string, ModuleTierMappings> get_3_tier_model_mappings();

    //
    /// \returns the 3 tier model mappings for the given \p module_id
    std::optional<ModuleTierMappings> get_module_3_tier_model_mappings(const std::string& module_id) const;

    //
    /// \returns the 3 tier model mapping for the given \p module_id and \p impl_id
    std::optional<Mapping> get_3_tier_model_mapping(const std::string& module_id, const std::string& impl_id) const;
};

///
/// \brief Config intended to be created by the manager for validation and serialization. Contains config and manifest
/// parsing
///
class ManagerConfig : public ConfigBase {
private:
    const ManagerSettings& ms;
    std::unordered_map<std::string, std::optional<TelemetryConfig>> telemetry_configs;
    Validators validators;
    std::unique_ptr<nlohmann::json_schema::json_validator> draft7_validator;

    ///
    /// \brief loads and validates the manifest of the module \p module_id using the provided \p module config
    void load_and_validate_manifest(const std::string& module_id, const nlohmann::json& module_config);

    ///
    /// \brief loads and validates the given file \p file_path with the schema \p schema
    ///
    /// \returns the loaded json and how long the validation took in ms
    std::tuple<nlohmann::json, int64_t> load_and_validate_with_schema(const fs::path& file_path,
                                                                      const nlohmann::json& schema);

    ///
    /// \brief resolves inheritance tree of json interface \p intf_name, throws an exception if variables or commands
    /// would be overwritten
    ///
    /// \returns the resulting interface definition
    nlohmann::json resolve_interface(const std::string& intf_name);

    ///
    /// \brief loads the contents of the interface file referenced by the give \p intf_name from disk and validates its
    /// contents
    ///
    /// \returns a json object containing the interface definition
    nlohmann::json load_interface_file(const std::string& intf_name);

    ///
    /// \brief loads the contents of an error or an error list referenced by the given \p reference.
    ///
    /// \returns a list of json objects containing the error definitions
    std::list<nlohmann::json> resolve_error_ref(const std::string& reference);

    ///
    /// \brief replaces all error references in the given \p interface_json with the actual error definitions
    ///
    /// \returns the interface_json with replaced error references
    nlohmann::json replace_error_refs(nlohmann::json& interface_json);

    ///
    /// \brief resolves all requirements (connections) of the modules in the main config
    void resolve_all_requirements();

    ///
    /// \brief parses the provided \p config resolving types, errors, manifests, requirements and 3 tier module mappings
    void parse(nlohmann::json config);

    ///
    /// \brief Parses the 3 tier model mappings in the config
    /// A "mapping" can be specified in the following way:
    /// You can set a EVSE id called "evse" and Connector id called "connector" for the whole module.
    /// Alternatively you can set individual mappings for implementations.
    /// mapping:
    ///   module:
    ///     evse: 1
    ///     connector: 1
    ///   implementations:
    ///     implementation_id:
    ///       evse: 1
    ///       connector: 1
    /// If no mappings are found it will be assumed that the module is mapped to the charging station.
    /// If only a module mapping is defined alle implementations are mapped to this module mapping.
    /// Implementations can have overwritten mappings.
    void parse_3_tier_model_mapping();

public:
    ///
    /// \brief Create a ManagerConfig from the provided ManagerSettings \p ms
    explicit ManagerConfig(const ManagerSettings& ms);

    ///
    /// \returns a TelemetryConfig if this has been configured for the given \p module_id
    std::optional<TelemetryConfig> get_telemetry_config(const std::string& module_id);
};

///
/// \brief Contains intended to be used by modules using a pre-parsed and validated config json serialized from
/// ManagerConfig
///
class Config : public ConfigBase {
private:
    std::optional<TelemetryConfig> telemetry_config;

public:
    ///
    /// \brief creates a new Config object form the given \p mqtt_settings and \p config
    explicit Config(const MQTTSettings& mqtt_settings, nlohmann::json config);

    ///
    /// \returns the ErrorTypeMap
    error::ErrorTypeMap get_error_map() const;

    ///
    /// \returns true if the module \p module_name provides the implementation \p impl_id
    bool module_provides(const std::string& module_name, const std::string& impl_id);

    ///
    /// \returns the commands that the modules \p module_name implements from the given implementation \p impl_id
    const nlohmann::json& get_module_cmds(const std::string& module_name, const std::string& impl_id);

    ///
    /// \brief A RequirementInitialization contains everything needed to initialize a requirement in user code. This
    /// includes the Requirement, its Fulfillment and an optional Mapping
    /// \returns a RequirementInitialization
    RequirementInitialization get_requirement_initialization(const std::string& module_id) const;

    ///
    /// \returns a map of module config options
    ModuleConfigs get_module_configs(const std::string& module_id) const;

    ///
    /// \returns a json object that contains the module config options
    const nlohmann::json& get_module_json_config(const std::string& module_id);

    ///
    /// \brief assemble basic information about the module (id, name,
    /// authors, license)
    ///
    /// \returns a ModuleInfo object
    ModuleInfo get_module_info(const std::string& module_id) const;

    ///
    /// \returns a TelemetryConfig if this has been configured
    std::optional<TelemetryConfig> get_telemetry_config();

    ///
    /// \returns a json object that contains the interface definition
    nlohmann::json get_interface_definition(const std::string& interface_name) const;

    ///
    /// \brief A json schema loader that can handle type refs and otherwise uses the builtin draft7 schema of
    /// the json schema validator when it encounters it. Throws an exception
    /// otherwise
    void ref_loader(const nlohmann::json_uri& uri, nlohmann::json& schema);

    ///
    /// \brief loads all module manifests relative to the \p main_dir
    ///
    /// \returns all module manifests as a json object
    static nlohmann::json load_all_manifests(const std::string& modules_dir, const std::string& schemas_dir);

    ///
    /// \brief Extracts the keys of the provided json \p object
    ///
    /// \returns a set of object keys
    static std::set<std::string> keys(const nlohmann::json& object);
};
} // namespace Everest

NLOHMANN_JSON_NAMESPACE_BEGIN
template <> struct adl_serializer<Everest::Schemas> {
    static void to_json(nlohmann::json& j, const Everest::Schemas& s);

    static void from_json(const nlohmann::json& j, Everest::Schemas& s);
};
NLOHMANN_JSON_NAMESPACE_END

#endif // UTILS_CONFIG_HPP
