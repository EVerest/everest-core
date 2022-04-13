// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_CONFIG_HPP
#define UTILS_CONFIG_HPP

#include <regex>
#include <set>
#include <string>

#include <boost/filesystem.hpp>
#include <nlohmann/json-schema.hpp>

#include <utils/types.hpp>

namespace Everest {
using json = nlohmann::json;
using json_uri = nlohmann::json_uri;
using json_validator = nlohmann::json_schema::json_validator;
namespace fs = boost::filesystem;

///
/// \brief A structure that contains all available schemas
///
struct schemas {
    json config;    ///< The config schema
    json manifest;  ///< The manifest scheme
    json interface; ///< The interface schema
};

///
/// \brief Contains config and manifest parsing
///
class Config {
private:
    std::string schemas_dir;
    std::string modules_dir;
    std::string interfaces_dir;

    json main;

    json manifests;
    json interfaces;
    json interface_definitions;
    schemas _schemas;

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

public:
    ///
    /// \brief creates a new Config object, looking for the config.json and schemes folder relative to the provided \p
    /// main_dir
    explicit Config(std::string schemas_dir, std::string config_file, std::string modules_dir,
                    std::string interfaces_dir);

    ///
    /// \brief checks if the given \p module_id provides the requirement given in \p requirement_id
    ///
    /// \returns a json object that contains the requirement
    json resolve_requirement(const std::string& module_id, const std::string& requirement_id);

    ///
    /// \brief checks if the config contains the given \p module_id
    ///
    bool contains(const std::string& module_id);

    ///
    /// \returns a json object that contains the main config
    json get_main_config();

    ///
    /// \returns a map of module config options
    ModuleConfigs get_module_configs(const std::string& module_id);

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
    /// \brief loads the config.json and manifest.json in the schemes subfolder of
    /// the provided \p schemas_dir
    ///
    /// \returns the config and manifest schemas
    static schemas load_schemas(std::string schemas_dir);

    ///
    /// \brief loads and validates a json schema at the provided \p path
    ///
    /// \returns the loaded json schema as a json object
    static json load_schema(const fs::path& path);

    ///
    /// \brief loads all module manifests relative to the \p main_dir
    ///
    /// \returns all module manifests as a json object
    static json load_all_manifests(std::string modules_dir, std::string schemas_dir);

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
