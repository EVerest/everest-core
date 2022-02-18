// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <algorithm>
#include <fstream>
#include <set>
#include <sstream>

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

#include <utils/config.hpp>

namespace Everest {

class ConfigParseException : public std::exception {
public:
    enum ParseErrorType
    {
        NOT_DEFINED,
        MISSING_ENTRY,
        SCHEMA
    };
    ConfigParseException(ParseErrorType err_t, const std::string& entry, const std::string& what = "") :
        err_t(err_t), entry(entry), what(what){};

    const ParseErrorType err_t;
    const std::string entry;
    const std::string what;
};

static json draft07 = R"(
{
    "$ref": "http://json-schema.org/draft-07/schema#"
}

)"_json;

static void validate_config_schema(const json& config_map_schema) {
    // iterate over every config entry
    json_validator validator(Config::loader, Config::format_checker);
    for (auto& config_item : config_map_schema.items()) {
        if (!config_item.value().contains("default")) {
            continue;
        }

        try {
            validator.set_root_schema(config_item.value());
            validator.validate(config_item.value()["default"]);
        } catch (const std::exception& e) {
            throw std::runtime_error("Config item '" + config_item.key() + "' has issues:\n" + e.what());
        }
    }
}

static json parse_config_map(const json& config_map_schema, const json& config_map) {
    json parsed_config_map{};

    std::set<std::string> unknown_config_entries;
    std::set<std::string> config_map_keys = Config::keys(config_map);
    std::set<std::string> config_map_schema_keys = Config::keys(config_map_schema);

    std::set_difference(config_map_keys.begin(), config_map_keys.end(), config_map_schema_keys.begin(),
                        config_map_schema_keys.end(),
                        std::inserter(unknown_config_entries, unknown_config_entries.end()));

    if (unknown_config_entries.size()) {
        throw ConfigParseException(ConfigParseException::NOT_DEFINED, *unknown_config_entries.begin());
    }

    // validate each config entry
    for (auto& config_entry_el : config_map_schema.items()) {
        std::string config_entry_name = config_entry_el.key();
        json config_entry = config_entry_el.value();

        // only convenience exception, would be catched by schema validation below if not thrown here
        if (!config_entry.contains("default") and !config_map.contains(config_entry_name)) {
            throw ConfigParseException(ConfigParseException::MISSING_ENTRY, config_entry_name);
        }

        json config_entry_value = config_map[config_entry_name]; // will be null if not present in json
        if (!config_map.contains(config_entry_name) && config_entry.contains("default")) {
            config_entry_value = config_entry["default"]; // use default value defined in manifest
        }
        json_validator validator(Config::loader, Config::format_checker);
        validator.set_root_schema(config_entry);
        try {
            auto patch = validator.validate(config_entry_value);
            if (!patch.is_null()) {
                // extend config entry with default values
                config_entry_value = config_entry_value.patch(patch);
            }
        } catch (const std::exception& err) {
            throw ConfigParseException(ConfigParseException::SCHEMA, config_entry_name, err.what());
        }

        parsed_config_map[config_entry_name] = config_entry_value;
    }

    return parsed_config_map;
}

Config::Config(std::string schemas_dir, std::string config_file, std::string modules_dir, std::string interfaces_dir) {
    BOOST_LOG_FUNCTION();

    this->schemas_dir = schemas_dir;
    this->modules_dir = modules_dir;
    this->interfaces_dir = interfaces_dir;

    this->manifests = json({});
    this->interfaces = json({});
    this->interface_definitions = json({});
    this->base_interfaces = json({});

    this->_schemas = Config::load_schemas(this->schemas_dir);

    // load and process config file
    fs::path config_path = config_file;

    try {
        EVLOG(info) << "Loading config file at: " << fs::canonical(config_path).c_str();

        std::ifstream ifs(config_path.c_str());
        std::string config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

        this->main = json::parse(config_file);

        // try to load user config from a directory "user-config" that might be in the same parent directory as the
        // config_file. The config is supposed to have the same name as the parent config.
        // TODO(kai): introduce a parameter that can overwrite the location of the user config?
        // TODO(kai): or should we introduce a "meta-config" that references all configs that should be merged here?
        auto user_config_path = config_path.parent_path() / "user-config" / config_path.filename();
        if (fs::exists(user_config_path)) {
            EVLOG(info) << "Loading user-config file at: " << fs::canonical(user_config_path).c_str();

            std::ifstream ifs(user_config_path.c_str());
            std::string user_config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

            auto user_config = json::parse(user_config_file);

            EVLOG(info) << "Augmenting main config with user-config entries";
            this->main.merge_patch(user_config);
        } else {
            EVLOG(debug) << "No user-config provided.";
        }

        json_validator validator(Config::loader);
        validator.set_root_schema(this->_schemas.config);
        auto patch = validator.validate(this->main);
        if (!patch.is_null()) {
            // extend config with default values
            this->main = this->main.patch(patch);
        }
    } catch (const std::exception& e) {
        EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Failed to load and parse config file: ", e.what()));
    }

    // load manifest files of configured modules
    for (auto& element : this->main.items()) {
        const auto& module_id = element.key();
        auto& module_config = element.value();
        std::string module_name = module_config["module"];

        EVLOG(info) << "Found module " << printable_identifier(module_id) << ", loading and verifying manifest...";

        // load and validate module manifest.json
        fs::path manifest_path = fs::path(modules_dir) / module_name / "manifest.json";
        try {
            EVLOG(info) << "Loading module manifest file at: " << fs::canonical(manifest_path).c_str();

            std::ifstream ifs(manifest_path.c_str());
            std::string manifest_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

            this->manifests[module_name] = json::parse(manifest_file);

            json_validator validator(Config::loader, Config::format_checker);
            validator.set_root_schema(this->_schemas.manifest);
            auto patch = validator.validate(this->manifests[module_name]);
            if (!patch.is_null()) {
                // extend manifest with default values
                this->manifests[module_name] = this->manifests[module_name].patch(patch);
            }
        } catch (const std::exception& e) {
            EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Failed to load and parse manifest file: ", e.what()));
        }

        // validate user-defined default values for the config meta-schemas
        try {
            validate_config_schema(this->manifests[module_name]["config"]);
        } catch (const std::exception& e) {
            EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError,
                                        "Failed to validate the module configuration meta-schema for module '",
                                        module_name, "'. Reason:\n", e.what()));
        }

        for (auto& impl : this->manifests[module_name]["provides"].items()) {
            try {
                validate_config_schema(impl.value()["config"]);
            } catch (const std::exception& e) {
                EVLOG_AND_THROW(
                    EVEXCEPTION(EverestConfigError,
                                "Failed to validate the implementation configuration meta-schema for implementation '",
                                impl.key(), "' in module '", module_name, "'. Reason:\n", e.what()));
            }
        }

        std::set<std::string> provided_impls = Config::keys(this->manifests[module_name]["provides"]);

        this->interfaces[module_name] = json({});
        this->base_interfaces[module_name] = json({});

        for (const auto& impl_id : provided_impls) {
            EVLOG(info) << "Loading interface for implementation: " << impl_id;
            auto intf_name = this->manifests[module_name]["provides"][impl_id]["interface"].get<std::string>();
            auto seen_interfaces = std::set<std::string>();
            auto interface_definition = resolve_interface(intf_name, seen_interfaces);
            this->interfaces[module_name][impl_id] = interface_definition;
            this->base_interfaces[module_name][impl_id] = seen_interfaces;
        }

        // check if config only contains impl_ids listed in manifest file
        std::set<std::string> unknown_impls_in_config;
        std::set<std::string> configured_impls = Config::keys(this->main[module_id]["config_implementation"]);

        std::set_difference(configured_impls.begin(), configured_impls.end(), provided_impls.begin(),
                            provided_impls.end(),
                            std::inserter(unknown_impls_in_config, unknown_impls_in_config.end()));

        if (!unknown_impls_in_config.empty()) {
            std::ostringstream oss;
            oss << "Implemenation id";
            for (const auto& impl_id : unknown_impls_in_config) {
                oss << " '" << impl_id << "'";
            }
            oss << "mentioned in config but not defined in manifest of module '" << module_config["module"] << "'!";
            EVLOG(error) << oss.str();
            EVTHROW(EverestApiError(oss.str()));
        }

        // validate config entries against manifest file
        for (const auto& impl_id : provided_impls) {
            EVLOG(debug) << "Validating implementation config of " << printable_identifier(module_id, impl_id)
                         << " against json schemas defined in module mainfest...";

            json config_map = module_config["config_implementation"][impl_id];
            json config_map_schema =
                this->manifests[module_config["module"].get<std::string>()]["provides"][impl_id]["config"];

            try {
                this->main[module_id]["config_maps"][impl_id] = parse_config_map(config_map_schema, config_map);
            } catch (const ConfigParseException& err) {
                if (err.err_t == ConfigParseException::NOT_DEFINED) {
                    EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Config entry '", err.entry, "' of ",
                                                printable_identifier(module_id, impl_id),
                                                " not defined in manifest of module '", module_config["module"], "'!"));
                } else if (err.err_t == ConfigParseException::MISSING_ENTRY) {
                    EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Missing mandatory config entry '", err.entry,
                                                "' in ", printable_identifier(module_id, impl_id), "!"));
                } else if (err.err_t == ConfigParseException::SCHEMA) {
                    EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Schema validation for config entry '", err.entry,
                                                "' failed in ", printable_identifier(module_id, impl_id),
                                                "! Reason:\n"
                                                    << err.what));
                } else {
                    throw err;
                }
            }
        }

        // validate config for !module
        {
            json config_map = module_config["config_module"];
            json config_map_schema = this->manifests[module_config["module"].get<std::string>()]["config"];

            try {
                this->main[module_id]["config_maps"]["!module"] = parse_config_map(config_map_schema, config_map);
            } catch (const ConfigParseException& err) {
                if (err.err_t == ConfigParseException::NOT_DEFINED) {
                    EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Config entry '", err.entry,
                                                "' for module config not defined in manifest of module '",
                                                module_config["module"], "'!"));
                } else if (err.err_t == ConfigParseException::MISSING_ENTRY) {
                    EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Missing mandatory config entry '", err.entry,
                                                "' for module config in module ", module_config["module"], "!"));
                } else if (err.err_t == ConfigParseException::SCHEMA) {
                    EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Schema validation for config entry '", err.entry,
                                                "' failed for module config in module ", module_config["module"],
                                                "!  Reason:\n"
                                                    << err.what));
                } else {
                    throw err;
                }
            }
        }
    }

    resolve_all_requirements();
}

json Config::resolve_interface(const std::string& intf_name, std::set<std::string>& seen_interfaces) {
    // load and validate interface.json and mark interface as seen
    auto intf_definition = load_interface_file(intf_name);
    seen_interfaces.insert(intf_name);

    if (intf_definition["parent"].is_string() && intf_definition["parent"] != "") {
        EVLOG(debug) << "interface '" << intf_name << "' has parent '" << intf_definition["parent"] << "'";
        auto parent_name = intf_definition["parent"].get<std::string>();
        if (seen_interfaces.count(parent_name) != 0) {
            EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Found unallowed inheritance loop: interface '", intf_name,
                                        "' tries to load already loaded interface '", parent_name, "'!"));
        }
        auto parent_definition = resolve_interface(parent_name, seen_interfaces);

        // merge interface definitions of parent into child
        if (parent_definition.contains("vars")) {
            if (intf_definition.contains("vars")) {
                EVLOG(debug)
                    << "there are vars in the base and child interface definitions, extending child interface '"
                    << intf_name << "' with base interface '" << parent_name << "'...";
                for (auto& var : intf_definition["vars"].items()) {
                    if (parent_definition["vars"].contains(var.key())) {
                        EVLOG_AND_THROW(EVEXCEPTION(
                            EverestConfigError, "parent interface contains var '"
                                                    << var.key() << "' that would be overwritten by child interface!"));
                    }
                }
                intf_definition["vars"].update(parent_definition["vars"]);
            } else {
                EVLOG(debug) << "there are only vars in the base interface definition, copying over base interface '"
                             << parent_name << "' to child interface '" << intf_name << "'...";
                intf_definition["vars"] = parent_definition["vars"];
            }
        }

        if (parent_definition.contains("cmds")) {
            if (intf_definition.contains("cmds")) {
                EVLOG(debug)
                    << "there are cmds in the base and child interface definitions, extending child interface '"
                    << intf_name << "' with base interface '" << parent_name << "'...";
                for (auto& cmd : intf_definition["cmds"].items()) {
                    if (parent_definition["cmds"].contains(cmd.key())) {
                        EVLOG_AND_THROW(EVEXCEPTION(
                            EverestConfigError, "parent interface contains cmd '"
                                                    << cmd.key() << "' that would be overwritten by child interface!"));
                        // TODO(kai): maybe only throw with identical function signature, otherwise overload
                        // --> no, overloading can't be implemented nicely in javascript and imposes all sorts of corner
                        // cases we don't want to handle
                    }
                }
                intf_definition["cmds"].update(parent_definition["cmds"]);
            } else {
                EVLOG(debug) << "there are only cmds in the base interface definition, copying over base interface '"
                             << parent_name << "' to child interface '" << intf_name << "'...";
                intf_definition["cmds"] = parent_definition["cmds"];
            }
        }
    }

    this->interface_definitions[intf_name] = intf_definition;
    return intf_definition;
}

json Config::load_interface_file(const std::string& intf_name) {
    BOOST_LOG_FUNCTION();
    fs::path intf_path = fs::path(this->interfaces_dir) / (intf_name + ".json");
    try {
        EVLOG(info) << "Loading interface file at: " << fs::canonical(intf_path).c_str();

        std::ifstream ifs(intf_path.c_str());
        std::string intf_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

        json interface_json = json::parse(intf_file);

        // this subschema can not use allOf with the draft-07 schema because that will cause our validator to
        // add all draft-07 default values which never validate (the {"not": true} default contradicts everything)
        // --> validating against draft-07 will be done in an extra step below
        json_validator validator(Config::loader, Config::format_checker);
        validator.set_root_schema(this->_schemas.interface);
        auto patch = validator.validate(interface_json);
        if (!patch.is_null()) {
            // extend config entry with default values
            interface_json = interface_json.patch(patch);
        }

        // validate every cmd arg/result and var definition against draft-07 schema
        validator.set_root_schema(draft07);
        for (auto& var_entry : interface_json["vars"].items()) {
            validator.validate(var_entry.value());
        }
        for (auto& cmd_entry : interface_json["cmds"].items()) {
            for (auto& arguments_entry : interface_json["cmds"][cmd_entry.key()]["arguments"].items()) {
                validator.validate(arguments_entry.value());
            }
            validator.validate(interface_json["cmds"][cmd_entry.key()]["result"]);
        }

        return interface_json;
    } catch (const std::exception& e) {
        EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Failed to load and parse interface file: ", e.what()));
    }
}

json Config::resolve_requirement(const std::string& module_id, const std::string& requirement_id) {
    BOOST_LOG_FUNCTION();

    // FIXME (aw): this function should throw, if the requirement id
    //             isn't even listed in the module manifest
    // FIXME (aw): the following if doesn't check for the requirement id
    //             at all
    if (!this->main.contains(module_id)) {
        EVLOG_AND_THROW(EVEXCEPTION(EverestApiError, "Requested requirement id '", requirement_id, "' of module ",
                                    printable_identifier(module_id), " not found in config!"));
    }

    // check for connections for this requirement
    json module_config = this->main[module_id];
    std::string module_name = module_config["module"].get<std::string>();
    auto& requirement = this->manifests[module_name]["requires"][requirement_id];
    if (!module_config["connections"].contains(requirement_id)) {
        return json::array(); // return an empty array if our config does not contain any connections for this
                              // requirement id
    }

    // if only one single connection entry was required, return only this one
    // callers can check with is_array() if this is a single connection (legacy) or a connection list
    if (requirement["min_connections"] == 1 && requirement["max_connections"] == 1) {
        return module_config["connections"][requirement_id].at(0);
    }
    return module_config["connections"][requirement_id];
}

bool Config::contains(const std::string& module_id) {
    BOOST_LOG_FUNCTION();
    return this->main.contains(module_id);
}

json Config::get_main_config() {
    BOOST_LOG_FUNCTION();
    return this->main;
}

// FIXME (aw): check if module_id does not exist
json Config::get_module_json_config(const std::string& module_id) {
    BOOST_LOG_FUNCTION();
    return this->main[module_id]["config_maps"];
}

ModuleConfigs Config::get_module_configs(const std::string& module_id) {
    BOOST_LOG_FUNCTION();
    ModuleConfigs module_configs;

    // FIXME (aw): throw exception if module_id does not exist
    if (contains(module_id)) {
        const auto module_type = this->main[module_id]["module"].get<std::string>();
        json config_maps = this->main[module_id]["config_maps"];
        json manifest = this->manifests[module_type];

        for (auto& conf_map : config_maps.items()) {
            json config_schema =
                (conf_map.key() == "!module") ? manifest["config"] : manifest["provides"][conf_map.key()]["config"];
            ConfigMap processed_conf_map;

            for (auto& entry : conf_map.value().items()) {
                json entry_type = config_schema[entry.key()]["type"];
                ConfigEntry value;
                json data = entry.value();

                if (data.is_string()) {
                    value = data.get<std::string>();
                } else if (data.is_number_float()) {
                    value = data.get<double>();
                } else if (data.is_number_integer()) {
                    if (entry_type == "number") {
                        EVLOG(debug) << "      (this is provided as an integer but converted to double";
                        value = data.get<double>();
                    } else {
                        value = data.get<int>();
                    }
                } else if (data.is_boolean()) {
                    value = data.get<bool>();
                } else {
                    EVLOG_AND_THROW(EVEXCEPTION(EverestInternalError, "Found a config entry: '", entry.key(),
                                                "' for module type '", module_type,
                                                "', which has a data type, that is different from ",
                                                "(bool, integer, number, string)"));
                }
                processed_conf_map[entry.key()] = value;
            }
            module_configs[conf_map.key()] = processed_conf_map;
        }
    }

    return module_configs;
}

const json& Config::get_manifests() {
    BOOST_LOG_FUNCTION();
    return this->manifests;
}

json Config::get_interfaces() {
    BOOST_LOG_FUNCTION();
    return this->interfaces;
}

json Config::get_interface_definition(const std::string& interface_name) {
    BOOST_LOG_FUNCTION();
    return this->interface_definitions.value(interface_name, json());
}

json Config::load_schema(const fs::path& path) {
    BOOST_LOG_FUNCTION();

    if (!fs::exists(path)) {
        EVLOG_AND_THROW(
            EVEXCEPTION(EverestInternalError, "Schema file does not exist at: ", fs::absolute(path).c_str()));
    }

    EVLOG(info) << "Loading schema file at: " << fs::canonical(path).c_str();

    std::ifstream ifs(path.c_str());
    std::string schema_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    json schema = json::parse(schema_file, nullptr, true, true);

    json_validator validator(Config::loader);

    try {
        validator.set_root_schema(schema);
    } catch (const std::exception& e) {
        EVLOG_AND_THROW(EVEXCEPTION(EverestInternalError, "Validation of schema failed, here is why: ", e.what()));
    }

    return schema;
}

schemas Config::load_schemas(std::string schemas_dir) {
    BOOST_LOG_FUNCTION();
    schemas schemas;

    EVLOG(info) << "Loading base schema files for config and manifests... from:" << schemas_dir;
    schemas.config = Config::load_schema(fs::path(schemas_dir) / "config.json");
    schemas.manifest = Config::load_schema(fs::path(schemas_dir) / "manifest.json");
    schemas.interface = Config::load_schema(fs::path(schemas_dir) / "interface.json");

    return schemas;
}

json Config::load_all_manifests(std::string modules_dir, std::string schemas_dir) {
    BOOST_LOG_FUNCTION();

    json manifests = json({});

    schemas schemas = Config::load_schemas(schemas_dir);

    fs::path modules_path = fs::path(modules_dir);

    for (auto&& module_path : fs::directory_iterator(modules_path)) {
        fs::path manifest_path = module_path.path() / "manifest.json";
        if (!fs::exists(manifest_path)) {
            continue;
        }

        std::string module_name = module_path.path().filename().c_str();
        EVLOG(info) << "Found module " << module_name << ", loading and verifying manifest...";

        try {
            std::ifstream ifs(manifest_path.c_str());
            std::string manifest_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

            manifests[module_name] = json::parse(manifest_file);

            json_validator validator(Config::loader, Config::format_checker);
            validator.set_root_schema(schemas.manifest);
            validator.validate(manifests[module_name]);
        } catch (const std::exception& e) {
            EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Failed to load and parse module manifest file of module ",
                                        module_name, ": ", e.what()));
        }
    }

    return manifests;
}

std::set<std::string> Config::keys(json object) {
    BOOST_LOG_FUNCTION();

    std::set<std::string> keys;
    if (!object.is_object()) {
        if (object.is_null() || object.empty()) {
            // if the object is null we should return an empty set
            return keys;
        }
        EVLOG_AND_THROW(
            EVEXCEPTION(EverestInternalError, "Provided value is not an object. It is a: ", object.type_name()));
    }

    for (auto& element : object.items()) {
        keys.insert(element.key());
    }

    return keys;
}

void Config::loader(const json_uri& uri, json& schema) {
    BOOST_LOG_FUNCTION();

    if (uri.location() == "http://json-schema.org/draft-07/schema") {
        schema = nlohmann::json_schema::draft7_schema_builtin;
        return;
    }

    // TODO(kai): think about supporting more urls here
    EVTHROW(EverestInternalError(uri.url() + " is not supported for schema loading at the moment\n"));
}

void Config::format_checker(const std::string& format, const std::string& value) {
    BOOST_LOG_FUNCTION();

    if (format == "uri") {
        if (value.find("://") == std::string::npos) {
            EVTHROW(std::invalid_argument("URI does not contain :// - invalid"));
        }
    } else {
        nlohmann::json_schema::default_string_format_check(format, value);
    }
}

std::string Config::printable_identifier(const std::string& module_id) {
    BOOST_LOG_FUNCTION();

    return printable_identifier(module_id, "");
}

std::string Config::printable_identifier(const std::string& module_id, const std::string& impl_id) {
    BOOST_LOG_FUNCTION();

    json info = extract_implementation_info(module_id, impl_id);
    // no implementation id yet so only return this kind of string:
    std::ostringstream oss;
    oss << info["module_id"].get<std::string>() << ":" << info["module_name"].get<std::string>();
    if (impl_id.empty()) {
        return oss.str();
    }
    oss << "->" << info["impl_id"].get<std::string>() << ":" << info["impl_intf"].get<std::string>();

    return oss.str();
}

ModuleInfo Config::get_module_info(const std::string& module_id) {
    BOOST_LOG_FUNCTION();
    // FIXME (aw): the following if block is used so often, it should be
    //             refactored into a helper function
    if (!this->main.contains(module_id)) {
        EVTHROW(EverestApiError("Module id '" + module_id + "' not found in config!"));
    }

    ModuleInfo module_info;
    module_info.id = module_id;
    module_info.name = this->main[module_id]["module"].get<std::string>();
    auto& module_metadata = this->manifests[module_info.name]["metadata"];
    for (auto& author : module_metadata["authors"]) {
        module_info.authors.emplace_back(author.get<std::string>());
    }
    module_info.license = module_metadata["license"].get<std::string>();

    return module_info;
}

std::string Config::mqtt_prefix(const std::string& module_id, const std::string& impl_id) {
    BOOST_LOG_FUNCTION();

    json info = extract_implementation_info(module_id, impl_id);

    std::ostringstream oss;
    oss << "everest/" << info["module_id"].get<std::string>() << ":" << info["module_name"].get<std::string>() << "/"
        << info["impl_id"].get<std::string>() << ":" << info["impl_intf"].get<std::string>();

    return oss.str();
}

std::string Config::mqtt_module_prefix(const std::string& module_id) {
    BOOST_LOG_FUNCTION();

    json info = extract_implementation_info(module_id);

    std::ostringstream oss;
    oss << "everest/" << info["module_id"].get<std::string>() << ":" << info["module_name"].get<std::string>();

    return oss.str();
}

json Config::extract_implementation_info(const std::string& module_id, const std::string& impl_id) {
    BOOST_LOG_FUNCTION();

    if (!this->main.contains(module_id)) {
        std::ostringstream oss;
        oss << "Module id '" << module_id << "' not found in config!";
        EVTHROW(EverestApiError(oss.str()));
    }

    json info;
    info["module_id"] = module_id;
    info["module_name"] = this->main[module_id]["module"];
    info["impl_id"] = impl_id;
    info["impl_intf"] = "";
    if (!impl_id.empty()) {
        if (!this->manifests.contains(info["module_name"])) {
            std::ostringstream oss;
            oss << "No known manifest for module name '" << info["module_name"] << "'!";
            EVTHROW(EverestApiError(oss.str()));
        }

        if (!this->manifests[info["module_name"].get<std::string>()]["provides"].contains(impl_id)) {
            std::ostringstream oss;
            oss << "Implementaiton id '" << impl_id << "' not defined in manifest of module '" << info["module_name"]
                << "'!";
            EVTHROW(EverestApiError(oss.str()));
        }

        info["impl_intf"] = this->manifests[info["module_name"].get<std::string>()]["provides"][impl_id]["interface"];
    }

    return info;
}

json Config::extract_implementation_info(const std::string& module_id) {
    BOOST_LOG_FUNCTION();

    return extract_implementation_info(module_id, "");
}

void Config::resolve_all_requirements() {
    BOOST_LOG_FUNCTION();

    EVLOG(info) << "Resolving module reguirements...";
    // this whole code will not check existence of keys defined by config or
    // manifest metaschemas these have already been checked by schema validation
    for (auto& element : this->main.items()) {
        const auto& module_id = element.key();
        EVLOG(debug) << "Resolving requirements of module " << printable_identifier(module_id) << "...";

        auto& module_config = element.value();

        std::set<std::string> unknown_requirement_entries;
        std::set<std::string> module_config_connections_set = Config::keys(module_config["connections"]);
        std::set<std::string> manifest_module_requires_set =
            Config::keys(this->manifests[module_config["module"].get<std::string>()]["requires"]);

        std::set_difference(module_config_connections_set.begin(), module_config_connections_set.end(),
                            manifest_module_requires_set.begin(), manifest_module_requires_set.end(),
                            std::inserter(unknown_requirement_entries, unknown_requirement_entries.end()));

        if (!unknown_requirement_entries.empty()) {
            std::ostringstream oss;
            oss << "Configured connection for requirement id";
            for (const auto& requirement_id : unknown_requirement_entries) {
                oss << " '" << requirement_id << "'";
            }
            oss << "of " << printable_identifier(module_id) << " not defined as requirement in manifest of module '"
                << module_config["module"] << "'!";
            EVLOG(error) << oss.str();
            EVTHROW(EverestApiError(oss.str()));
        }

        for (auto& element : this->manifests[module_config["module"].get<std::string>()]["requires"].items()) {
            const auto& requirement_id = element.key();
            auto& requirement = element.value();

            if (!module_config["connections"].contains(requirement_id)) {
                if (requirement["min_connections"] < 1) {
                    EVLOG(info) << "Manifest of " << printable_identifier(module_id) << " lists OPTIONAL requirement '"
                                << requirement_id
                                << "' which could not be fulfilled and will be "
                                   "ignored...";
                    continue; // stop here, there is nothing we can do
                }
                EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Requirement '", requirement_id, "' of module ",
                                            printable_identifier(module_id), " not fulfilled: requirement id '",
                                            requirement_id, "' not listed in connections!"));
            }
            json connections = module_config["connections"][requirement_id];

            // check if min_connections and max_connections are fulfilled
            if (connections.size() < requirement["min_connections"] ||
                connections.size() > requirement["max_connections"]) {
                EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Requirement '", requirement_id, "' of module ",
                                            printable_identifier(module_id), " not fulfilled: requirement list does",
                                            "not have an entry count between ", requirement["min_connections"], " and ",
                                            requirement["max_connections"], "!"));
            }

            for (uint64_t connection_num = 0; connection_num < connections.size(); connection_num++) {
                auto& connection = connections[connection_num];
                std::string connection_module_id = connection["module_id"];
                if (!this->main.contains(connection["module_id"])) {
                    EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Requirement '", requirement_id, "' of module ",
                                                printable_identifier(module_id), " not fulfilled: module id '",
                                                connection_module_id, "' (configured in connection ", connection_num,
                                                ") not loaded in config!"));
                }

                std::string connection_module_name = this->main[connection_module_id]["module"];
                std::string connection_impl_id = connection["implementation_id"];
                auto& connection_manifest = this->manifests[connection_module_name];
                if (!connection_manifest["provides"].contains(connection_impl_id)) {
                    EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Requirement '", requirement_id, "' of module ",
                                                printable_identifier(module_id), " not fulfilled: required module ",
                                                printable_identifier(connection["module_id"]),
                                                " does not provide a implementation for '", connection_impl_id, "'!"));
                }

                auto& connection_provides = connection_manifest["provides"][connection_impl_id];
                std::set<std::string> provided_intfs =
                    this->base_interfaces[connection_module_name][connection_impl_id];
                std::string requirement_interface = requirement["interface"];

                // check interface requirement
                if (provided_intfs.count(requirement_interface) < 1) {
                    EVLOG_AND_THROW(EVEXCEPTION(
                        EverestConfigError, "Requirement '", requirement_id, "' of module ",
                        printable_identifier(module_id), " not fulfilled by connection to module ",
                        printable_identifier(connection["module_id"], connection_impl_id), ": required interface '",
                        requirement_interface, "' is not provided by this implementation!",
                        " Connected implementation provides interface '",
                        connection_provides["interface"].get<std::string>(),
                        "' with parent interfaces: ", json(provided_intfs)));
                }

                module_config["connections"][requirement_id][connection_num]["provides"] = connection_provides;
                module_config["connections"][requirement_id][connection_num]["required_interface"] =
                    requirement_interface;
                EVLOG(info) << "Manifest of" << printable_identifier(module_id) << " lists requirement '"
                            << requirement_id << "' which will be fulfilled by "
                            << printable_identifier(connection["module_id"], connection["implementation_id"]) << "...";
            }
        }
    }
    EVLOG(info) << "All module requirements resolved successfully...";
}
} // namespace Everest
