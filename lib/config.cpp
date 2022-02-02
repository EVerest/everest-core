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

        if (!config_entry.contains("default") and !config_map.contains(config_entry_name)) {
            throw ConfigParseException(ConfigParseException::MISSING_ENTRY, config_entry_name);
        }

        // default value if not existing is None (should validate to
        // default value or error)
        json config_entry_value = json();
        if (config_map.contains(config_entry_name)) {
            config_entry_value = config_map[config_entry_name];

            json_validator validator(Config::loader, Config::format_checker);
            validator.set_root_schema(config_entry);

            try {
                validator.validate(config_entry_value);
            } catch (const std::exception& err) {
                throw ConfigParseException(ConfigParseException::SCHEMA, config_entry_name, err.what());
            }
        } else {
            config_entry_value = config_entry["default"];
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
        validator.validate(this->main);
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
            validator.validate(this->manifests[module_name]);
        } catch (const std::exception& e) {
            EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Failed to load and parse manifest file: ", e.what()));
        }

        // validate user-defined default values for the config meta-schemas
        if (this->manifests[module_name].contains("config")) {
            try {
                validate_config_schema(this->manifests[module_name]["config"]);
            } catch (const std::exception& e) {
                EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError,
                                            "Failed to validate the module configuration meta-schema for module '",
                                            module_name, "'. Reason:\n", e.what()));
            }
        }

        for (auto& impl : this->manifests[module_name]["provides"].items()) {
            if (impl.value().contains("config")) {
                try {
                    validate_config_schema(impl.value()["config"]);
                } catch (const std::exception& e) {
                    EVLOG_AND_THROW(EVEXCEPTION(
                        EverestConfigError,
                        "Failed to validate the implementation configuration meta-schema for implementation '",
                        impl.key(), "' in module '", module_name, "'. Reason:\n", e.what()));
                }
            }
        }

        std::set<std::string> provided_impls = Config::keys(this->manifests[module_name]["provides"]);

        this->interfaces[module_name] = json({});
        this->base_interfaces[module_name] = json({});

        for (const auto& impl_id : provided_impls) {
            EVLOG(info) << "Loading interface for implementation: " << impl_id;
            auto intf_name = this->manifests[module_name]["provides"][impl_id]["interface"].get<std::string>();
            // the std::set is only needed for internal loop detection
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

            // default value for missing implementation configs is an empty dict
            json config_map = json({});
            if (module_config.contains("config_implementation") &&
                module_config["config_implementation"].contains(impl_id)) {
                config_map = module_config["config_implementation"][impl_id];
            }

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
                                                "!  Reason:\n"
                                                    << err.what));
                } else {
                    throw err;
                }
            }
        }

        // validate config for !module
        {
            // default value for missing !module config is an empty dict
            json config_map = module_config.value("config_module", json({}));
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

        json_validator validator(Config::loader, Config::format_checker);
        validator.set_root_schema(this->_schemas.interface);
        validator.validate(interface_json);

        return interface_json;
    } catch (const std::exception& e) {
        EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Failed to load and parse interface file: ", e.what()));
    }
}

json Config::resolve_requirement(const std::string& module_id, const std::string& requirement_id) {
    BOOST_LOG_FUNCTION();

    bool optional_requirement = false;
    if (std::regex_match(requirement_id, this->optional_requirement_regex)) {
        optional_requirement = true;
    }
    if (!this->main.contains(module_id)) {
        if (!optional_requirement) {
            EVLOG_AND_THROW(EVEXCEPTION(EverestApiError, "Requested requirement id '", requirement_id, "' of module ",
                                        printable_identifier(module_id), " not found in manifest!"));
        }
    }
    json module_config = this->main[module_id];
    if (!module_config["connections"].contains(requirement_id)) {
        if (!optional_requirement) {
            EVLOG_AND_THROW(EVEXCEPTION(EverestApiError, "Requested requirement id '", requirement_id, "' of module ",
                                        printable_identifier(module_id), " not found in manifest!"));
        }

        return json(nullptr); // optional requirement not found
    }

    json connection = module_config["connections"][requirement_id];
    return connection;
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
    for (auto& author: module_metadata["authors"]) {
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

            bool optional_requirement = false;
            if (std::regex_match(requirement_id, this->optional_requirement_regex)) {
                optional_requirement = true;
            }

            if (!module_config["connections"].contains(requirement_id)) {
                if (optional_requirement) {
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
            json connection = module_config["connections"][requirement_id];
            std::string connection_module_id = connection["module_id"];
            if (!this->main.contains(connection["module_id"])) {
                // TODO(kai): is this really a good idea it can leave dangling connections hanging around and throwing
                // errors when adding new modules to the config that happen to have the same module id as the dangling
                // connection points to
                /*
                if (optional_requirement) {
                    EVLOG(info) << "Config of " << printable_identifier(module_id) << " lists OPTIONAL requirement '"
                                << requirement_id
                                << "' which could not be fulfilled and will be "
                                   "ignored: required module id '"
                                << connection["module_id"] << "' not listed in config...";
                    // remove not resolved optional connection
                    module_config["connections"].erase(requirement_id);
                    continue; // stop here, there is nothing we can do
                }
                */
                EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Requirement '", requirement_id, "' of module ",
                                            printable_identifier(module_id), " not fulfilled: module id '",
                                            connection_module_id,
                                            "' (configured in connection) not loaded in config!"));
            }

            std::string connection_module_name = this->main[connection_module_id]["module"];
            std::string connection_impl_id = connection["implementation_id"];
            json connection_manifest = this->manifests[connection_module_name];
            if (!connection_manifest["provides"].contains(connection_impl_id)) {
                EVLOG_AND_THROW(EVEXCEPTION(EverestConfigError, "Requirement '", requirement_id, "' of module ",
                                            printable_identifier(module_id), " not fulfilled: required module ",
                                            printable_identifier(connection["module_id"]),
                                            " does not provide a implementation for '", connection_impl_id, "'!"));
            }

            json connection_provides = connection_manifest["provides"][connection_impl_id];
            json connection_provides_intfs = this->base_interfaces[connection_module_name][connection_impl_id];
            for (auto& req : requirement.items()) {
                const auto& requirement_type = req.key();
                auto& requirement_value = req.value();

                if (connection_provides.contains(requirement_type) && requirement_type == "interface") {
                    std::set<std::string> provided_intfs = connection_provides_intfs;
                    if (provided_intfs.count(requirement_value) < 1) {
                        provided_intfs.erase(connection_provides[requirement_type].get<std::string>());
                        EVLOG_AND_THROW(EVEXCEPTION(
                            EverestConfigError, "Requirement '", requirement_id, "' of module ",
                            printable_identifier(module_id), " not fulfilled by connection to module ",
                            printable_identifier(connection["module_id"], connection_impl_id), ": required interface '",
                            std::string(requirement_value), "' is not provided by this implementation!",
                            " Connected implementation provides interface '",
                            connection_provides[requirement_type].get<std::string>(),
                            "' with parent interfaces: ", json(provided_intfs)));
                    }
                    // other properties (simple comparison)
                    // FIXME (aw): this behaviour of comparing all sub elements should be dropped
                } else if (!(connection_provides.contains(requirement_type) &&
                             connection_provides[requirement_type] == requirement_value)) {
                    std::string provided_propery = "<unset>";
                    if (connection_provides.contains(requirement_type)) {
                        provided_propery = connection_provides[requirement_type];
                    }
                    EVLOG_AND_THROW(
                        EVEXCEPTION(EverestConfigError, "Requirement '", requirement_id, "' of module ",
                                    printable_identifier(module_id), " not fulfilled by connection to module ",
                                    printable_identifier(connection["module_id"], connection["implementation_id"]),
                                    ": provided property '", requirement_type, "' is not ", requirement_value, " but ",
                                    provided_propery, "!"));
                }
            }

            connection["provides"] = connection_provides;
            EVLOG(info) << "Manifest of" << printable_identifier(module_id) << " lists requirement '" << requirement_id
                        << "' which will be fulfilled by "
                        << printable_identifier(connection["module_id"], connection["implementation_id"]) << "...";
        }
    }
    EVLOG(info) << "All module requirements resolved successfully...";
}
} // namespace Everest
