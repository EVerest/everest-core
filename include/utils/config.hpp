// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
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
#include <utils/types.hpp>

namespace Everest {
using json = nlohmann::json;
using json_uri = nlohmann::json_uri;
using json_validator = nlohmann::json_schema::json_validator;
namespace fs = std::filesystem;

struct RuntimeSettings;
///
/// \brief A structure that contains all available schemas
///
struct schemas {
    json config;                 ///< The config schema
    json manifest;               ///< The manifest scheme
    json interface;              ///< The interface schema
    json type;                   ///< The type schema
    json error_declaration_list; ///< The error-declaration-list schema
};

///
/// \brief Allowed format of a type URI, which are of a format like this /type_file_name#/TypeName
///
const static std::regex type_uri_regex{R"(^((?:\/[a-zA-Z0-9\-\_]+)+#\/[a-zA-Z0-9\-\_]+)$)"};

///
/// \brief Contains config and manifest parsing
///
class Config {
private:
    std::shared_ptr<RuntimeSettings> rs;
    bool manager;

    json main;

    json manifests;
    json interfaces;
    json interface_definitions;
    json types;
    json errors;
    schemas _schemas;

    std::unordered_map<std::string, std::optional<TelemetryConfig>> telemetry_configs;

    ///
    /// \brief loads the contents of an error or an error list referenced by the given \p reference.
    ///
    /// \returns a list of json objects containing the error definitions
    std::list<json> resolve_error_ref(const std::string& reference);

    ///
    /// \brief replaces all error references in the given \p interface_json with the actual error definitions
    ///
    /// \returns the interface_json with replaced error references
    json replace_error_refs(json& interface_json);

    ///
    /// \brief loads the contents of the interface file referenced by the give \p intf_name from disk and validates its
    /// contents
    ///
    /// \returns a json object containing the interface definition
    json load_interface_file(const std::string& intf_name);

    ///
    /// \brief resolves inheritance tree of json interface \p intf_name, throws an exception if variables or commands
    /// would be overwritten
    ///
    /// \returns the resulting interface definiion
    json resolve_interface(const std::string& intf_name);

    ///
    /// \brief extracts information about the provided module given via \p module_id from the config and manifest
    ///
    /// \returns a json object containing module_id, module_name, impl_id and impl_intf
    json extract_implementation_info(const std::string& module_id);

    ///
    /// \brief extracts information about the provided implementation given via \p module_id and \p impl_id from the
    /// config and
    /// manifest
    ///
    /// \returns a json object containing module_id, module_name, impl_id and impl_intf
    json extract_implementation_info(const std::string& module_id, const std::string& impl_id);
    void resolve_all_requirements();

    // experimental caches
    std::unordered_map<std::string, std::string> module_names;
    std::unordered_map<std::string, ConfigCache> module_config_cache;

    void load_and_validate_manifest(const std::string& module_id, const json& module_config);

    error::ErrorTypeMap error_map;

    ///
    /// \brief loads and validates the given file \p file_path with the schema \p schema
    ///
    /// \returns the loaded json and how long the validation took in ms
    std::tuple<json, int> load_and_validate_with_schema(const fs::path& file_path, const json& schema);

public:
    error::ErrorTypeMap get_error_map() const;
    std::string get_module_name(const std::string& module_id);
    bool module_provides(const std::string& module_name, const std::string& impl_id);
    json get_module_cmds(const std::string& module_name, const std::string& impl_id);
    ///
    /// \brief creates a new Config object
    explicit Config(std::shared_ptr<RuntimeSettings> rs);
    explicit Config(std::shared_ptr<RuntimeSettings> rs, bool manager);

    ///
    /// \brief checks if the given \p module_id provides the requirement given in \p requirement_id
    ///
    /// \returns a json object that contains the requirement
    json resolve_requirement(const std::string& module_id, const std::string& requirement_id);

    ///
    /// \brief checks if the config contains the given \p module_id
    ///
    bool contains(const std::string& module_id) const;

    ///
    /// \returns a json object that contains the main config
    // FIXME (aw): this should be const and return the config by const ref!
    json get_main_config();

    ///
    /// \returns a map of module config options
    ModuleConfigs get_module_configs(const std::string& module_id) const;

    ///
    /// \returns a json object that contains the module config options
    json get_module_json_config(const std::string& module_id);

    ///
    /// \brief assemble basic information about the module (id, name,
    /// authors, license)
    ///
    /// \returns a ModuleInfo object
    ModuleInfo get_module_info(const std::string& module_id);

    ///
    /// \returns a TelemetryConfig if this has been configured
    std::optional<TelemetryConfig> get_telemetry_config(const std::string& module_id);

    ///
    /// \returns a json object that contains the manifests
    const json& get_manifests();

    ///
    /// \returns a json object that contains the available interfaces
    json get_interfaces();

    ///
    /// \returns a json object that contains the interface definition
    json get_interface_definition(const std::string& interface_name);

    ///
    /// \brief turns then given \p module_id into a printable identifier
    ///
    /// \returns a string with the printable identifier
    std::string printable_identifier(const std::string& module_id);

    ///
    /// \brief turns then given \p module_id and \p impl_id into a printable identifier
    ///
    /// \returns a string with the printable identifier
    std::string printable_identifier(const std::string& module_id, const std::string& impl_id);

    ///
    /// \brief turns the given \p module_id and \p impl_id into a mqtt prefix
    ///
    std::string mqtt_prefix(const std::string& module_id, const std::string& impl_id);

    ///
    /// \brief turns the given \p module_id into a mqtt prefix
    ///
    std::string mqtt_module_prefix(const std::string& module_id);

    ///
    /// \brief A json schema loader that can handle type refs and otherwise uses the builtin draft7 schema of
    /// the json schema validator when it encounters it. Throws an exception
    /// otherwise
    ///
    void ref_loader(const json_uri& uri, json& schema);

    ///
    /// \brief loads the config.json and manifest.json in the schemes subfolder of
    /// the provided \p schemas_dir
    ///
    /// \returns the config and manifest schemas
    static schemas load_schemas(const fs::path& schemas_dir);

    ///
    /// \brief loads and validates a json schema at the provided \p path
    ///
    /// \returns the loaded json schema as a json object
    static json load_schema(const fs::path& path);

    ///
    /// \brief loads all module manifests relative to the \p main_dir
    ///
    /// \returns all module manifests as a json object
    static json load_all_manifests(const std::string& modules_dir, const std::string& schemas_dir);

    ///
    /// \brief Extracts the keys of the provided json \p object
    ///
    /// \returns a set of object keys
    static std::set<std::string> keys(json object);

    ///
    /// \brief A simple json schema loader that uses the builtin draft7 schema of
    /// the json schema validator when it encounters it, throws an exception
    /// otherwise
    ///
    static void loader(const json_uri& uri, json& schema);

    ///
    /// \brief An extension to the default format checker of the json schema
    /// validator supporting uris
    ///
    static void format_checker(const std::string& format, const std::string& value);
};
} // namespace Everest

#endif // UTILS_CONFIG_HPP
