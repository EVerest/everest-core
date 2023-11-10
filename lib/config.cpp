// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <algorithm>
#include <fstream>
#include <list>
#include <set>
#include <sstream>

#include <fmt/format.h>

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

#include <framework/runtime.hpp>
#include <utils/config.hpp>
#include <utils/formatter.hpp>
#include <utils/yaml_loader.hpp>

namespace Everest {

class ConfigParseException : public std::exception {
public:
    enum ParseErrorType {
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
            throw std::runtime_error(fmt::format("Config item '{}' has issues:\n{}", config_item.key(), e.what()));
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

        json config_entry_value;
        if (config_map.contains(config_entry_name)) {
            config_entry_value = config_map[config_entry_name];
        } else if (config_entry.contains("default")) {
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

static auto get_provides_for_probe_module(const std::string& probe_module_id, const json& config,
                                          const json& manifests) {
    auto probe_module_config = config.at(probe_module_id);

    auto provides = json::object();

    for (const auto& item : config.items()) {
        if (item.key() == probe_module_id) {
            // do not parse ourself
            continue;
        }

        const auto& module_config = item.value();

        const auto& connections = module_config.value("connections", json::object());

        for (const auto& connection : connections.items()) {
            const std::string req_id = connection.key();
            const std::string module_name = module_config.at("module");
            const auto& module_manifest = manifests.at(module_name);

            // FIXME (aw): in principle we would need to check here again, the listed connections are indeed specified
            // in the modules manifest
            const std::string requirement_interface = module_manifest.at("requires").at(req_id).at("interface");

            for (const auto& req_item : connection.value().items()) {
                const std::string impl_mod_id = req_item.value().at("module_id");
                const std::string impl_id = req_item.value().at("implementation_id");

                if (impl_mod_id != probe_module_id) {
                    continue;
                }

                if (provides.contains(impl_id) && (provides[impl_id].at("interface") != requirement_interface)) {
                    EVLOG_AND_THROW(EverestConfigError(
                        "ProbeModule can not fulfill multiple requirements for the same implementation id '" + impl_id +
                        "', but with different interfaces"));
                } else {
                    provides[impl_id] = {{"interface", requirement_interface}, {"description", "none"}};
                }
            }
        }
    }

    if (provides.empty()) {
        provides["none"] = {{"interface", "empty"}, {"description", "none"}};
    }

    return provides;
}

static auto get_requirements_for_probe_module(const std::string& probe_module_id, const json& config,
                                              const json& manifests) {
    auto probe_module_config = config.at(probe_module_id);

    auto requirements = json::object();

    const auto connections_it = probe_module_config.find("connections");
    if (connections_it == probe_module_config.end()) {
        return requirements;
    }

    for (const auto& item : connections_it->items()) {
        const auto& req_id = item.key();

        for (const auto& ffs : item.value()) {
            const std::string module_id = ffs.at("module_id");
            const std::string impl_id = ffs.at("implementation_id");

            const auto& module_config_it = config.find(module_id);

            if (module_config_it == config.end()) {
                EVLOG_AND_THROW(
                    EverestConfigError("ProbeModule refers to a non-existent module id '" + module_id + "'"));
            }

            const auto& module_manifest = manifests.at(module_config_it->at("module"));

            const auto& module_provides_it = module_manifest.find("provides");

            if (module_provides_it == module_manifest.end()) {
                EVLOG_AND_THROW(EverestConfigError("ProbeModule requires something from module id' " + module_id +
                                                   "', but it does not provide anything"));
            }

            const auto& provide_it = module_provides_it->find(impl_id);
            if (provide_it == module_provides_it->end()) {
                EVLOG_AND_THROW(EverestConfigError("ProbeModule requires something from module id '" + module_id +
                                                   "', but it does not provide '" + impl_id + "'"));
            }

            const std::string interface = provide_it->at("interface");

            if (requirements.contains(req_id) && (requirements[req_id].at("interface") != interface)) {
                // FIXME (aw): we might need to adujst the min/max values here for possible implementations
                EVLOG_AND_THROW(EverestConfigError("ProbeModule interface mismatch -- FIXME (aw)"));
            } else {
                requirements[req_id] = {{"interface", interface}};
            }
        }
    }

    return requirements;
}

static void setup_probe_module_manifest(const std::string& probe_module_id, const json& config, json& manifests) {
    // setup basic information
    auto& manifest = manifests["ProbeModule"];
    manifest = {
        {"description", "ProbeModule (generated)"},
        {
            "metadata",
            {
                {"license", "https://opensource.org/licenses/Apache-2.0"},
                {"authors", {"everest"}},
            },
        },
    };

    manifest["provides"] = get_provides_for_probe_module(probe_module_id, config, manifests);

    auto requirements = get_requirements_for_probe_module(probe_module_id, config, manifests);
    if (not requirements.empty()) {
        manifest["requires"] = requirements;
    }
}

void Config::load_and_validate_manifest(const std::string& module_id, const json& module_config) {
    std::string module_name = module_config["module"];

    this->module_config_cache[module_id] = ConfigCache();
    this->module_names[module_id] = module_name;
    EVLOG_debug << fmt::format("Found module {}, loading and verifying manifest...", printable_identifier(module_id));

    // load and validate module manifest.json
    fs::path manifest_path = this->rs->modules_dir / module_name / "manifest.yaml";
    try {

        if (module_name != "ProbeModule") {
            // FIXME (aw): this is implicit logic, because we know, that the ProbeModule manifest had been set up
            // manually already
            EVLOG_debug << fmt::format("Loading module manifest file at: {}", fs::canonical(manifest_path).string());
            this->manifests[module_name] = load_yaml(manifest_path);
        }

        json_validator validator(Config::loader, Config::format_checker);
        validator.set_root_schema(this->_schemas.manifest);
        auto patch = validator.validate(this->manifests[module_name]);
        if (!patch.is_null()) {
            // extend manifest with default values
            this->manifests[module_name] = this->manifests[module_name].patch(patch);
        }
    } catch (const std::exception& e) {
        EVLOG_AND_THROW(EverestConfigError(fmt::format("Failed to load and parse manifest file {}: {}",
                                                       fs::weakly_canonical(manifest_path).string(), e.what())));
    }

    // validate user-defined default values for the config meta-schemas
    try {
        validate_config_schema(this->manifests[module_name]["config"]);
    } catch (const std::exception& e) {
        EVLOG_AND_THROW(EverestConfigError(
            fmt::format("Failed to validate the module configuration meta-schema for module '{}'. Reason:\n{}",
                        module_name, e.what())));
    }

    for (auto& impl : this->manifests[module_name]["provides"].items()) {
        try {
            validate_config_schema(impl.value()["config"]);
        } catch (const std::exception& e) {
            EVLOG_AND_THROW(
                EverestConfigError(fmt::format("Failed to validate the implementation configuration meta-schema "
                                               "for implementation '{}' in module '{}'. Reason:\n{}",
                                               impl.key(), module_name, e.what())));
        }
    }

    std::set<std::string> provided_impls = Config::keys(this->manifests[module_name]["provides"]);

    this->interfaces[module_name] = json({});
    this->module_config_cache[module_name].provides_impl = provided_impls;

    for (const auto& impl_id : provided_impls) {
        EVLOG_debug << fmt::format("Loading interface for implementation: {}", impl_id);
        auto intf_name = this->manifests[module_name]["provides"][impl_id]["interface"].get<std::string>();
        auto seen_interfaces = std::set<std::string>();
        this->interfaces[module_name][impl_id] = resolve_interface(intf_name);
        this->module_config_cache[module_name].cmds[impl_id] = this->interfaces.at(module_name).at(impl_id).at("cmds");
    }

    // check if config only contains impl_ids listed in manifest file
    std::set<std::string> unknown_impls_in_config;
    std::set<std::string> configured_impls = Config::keys(this->main[module_id]["config_implementation"]);

    std::set_difference(configured_impls.begin(), configured_impls.end(), provided_impls.begin(), provided_impls.end(),
                        std::inserter(unknown_impls_in_config, unknown_impls_in_config.end()));

    if (!unknown_impls_in_config.empty()) {
        EVLOG_AND_THROW(EverestApiError(
            fmt::format("Implementation id(s)[{}] mentioned in config, but not defined in manifest of module '{}'!",
                        fmt::join(unknown_impls_in_config, " "), module_config["module"])));
    }

    // validate config entries against manifest file
    for (const auto& impl_id : provided_impls) {
        EVLOG_verbose << fmt::format(
            "Validating implementation config of {} against json schemas defined in module mainfest...",
            printable_identifier(module_id, impl_id));

        json config_map = module_config.value("config_implementation", json::object()).value(impl_id, json::object());
        json config_map_schema =
            this->manifests[module_config["module"].get<std::string>()]["provides"][impl_id]["config"];

        try {
            this->main[module_id]["config_maps"][impl_id] = parse_config_map(config_map_schema, config_map);
        } catch (const ConfigParseException& err) {
            if (err.err_t == ConfigParseException::NOT_DEFINED) {
                EVLOG_AND_THROW(EverestConfigError(
                    fmt::format("Config entry '{}' of {} not defined in manifest of module '{}'!", err.entry,
                                printable_identifier(module_id, impl_id), module_config["module"])));
            } else if (err.err_t == ConfigParseException::MISSING_ENTRY) {
                EVLOG_AND_THROW(EverestConfigError(fmt::format("Missing mandatory config entry '{}' in {}!", err.entry,
                                                               printable_identifier(module_id, impl_id))));
            } else if (err.err_t == ConfigParseException::SCHEMA) {
                EVLOG_AND_THROW(
                    EverestConfigError(fmt::format("Schema validation for config entry '{}' failed in {}! Reason:\n{}",
                                                   err.entry, printable_identifier(module_id, impl_id), err.what)));
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
                EVLOG_AND_THROW(EverestConfigError(
                    fmt::format("Config entry '{}' for module config not defined in manifest of module '{}'!",
                                err.entry, module_config["module"])));
            } else if (err.err_t == ConfigParseException::MISSING_ENTRY) {
                EVLOG_AND_THROW(
                    EverestConfigError(fmt::format("Missing mandatory config entry '{}' for module config in module {}",
                                                   err.entry, module_config["module"])));
            } else if (err.err_t == ConfigParseException::SCHEMA) {
                EVLOG_AND_THROW(EverestConfigError(fmt::format(
                    "Schema validation for config entry '{}' failed for module config in module {}! Reason:\n{}",
                    err.entry, module_config["module"], err.what)));
            } else {
                throw err;
            }
        }
    }
}

Config::Config(std::shared_ptr<RuntimeSettings> rs) : Config(rs, false) {
}

Config::Config(std::shared_ptr<RuntimeSettings> rs, bool manager) : rs(rs), manager(manager) {
    BOOST_LOG_FUNCTION();

    this->manifests = json({});
    this->interfaces = json({});
    this->interface_definitions = json({});
    this->types = json({});
    this->errors = json({});
    this->_schemas = Config::load_schemas(this->rs->schemas_dir);
    this->error_map = error::ErrorTypeMap(this->rs->errors_dir);

    // load and process config file
    fs::path config_path = rs->config_file;

    try {
        if (manager) {
            EVLOG_info << fmt::format("Loading config file at: {}", fs::canonical(config_path).string());
        }
        auto complete_config = load_yaml(config_path);
        // try to load user config from a directory "user-config" that might be in the same parent directory as the
        // config_file. The config is supposed to have the same name as the parent config.
        // TODO(kai): introduce a parameter that can overwrite the location of the user config?
        // TODO(kai): or should we introduce a "meta-config" that references all configs that should be merged here?
        auto user_config_path = config_path.parent_path() / "user-config" / config_path.filename();
        if (fs::exists(user_config_path)) {
            if (manager) {
                EVLOG_info << fmt::format("Loading user-config file at: {}", fs::canonical(user_config_path).string());
            }
            auto user_config = load_yaml(user_config_path);
            EVLOG_debug << "Augmenting main config with user-config entries";
            complete_config.merge_patch(user_config);
        } else {
            EVLOG_verbose << "No user-config provided.";
        }

        json_validator validator(Config::loader, Config::format_checker);
        validator.set_root_schema(this->_schemas.config);
        auto patch = validator.validate(complete_config);
        if (!patch.is_null()) {
            // extend config with default values
            complete_config = complete_config.patch(patch);
        }

        this->main = complete_config.at("active_modules");

    } catch (const std::exception& e) {
        EVLOG_AND_THROW(EverestConfigError(fmt::format("Failed to load and parse config file: {}", e.what())));
    }

    // load type files
    if (manager or rs->validate_schema) {
        int total_time_validation_ms = 0, total_time_parsing_ms = 0;
        for (auto const& types_entry : fs::recursive_directory_iterator(this->rs->types_dir)) {
            auto start_time = std::chrono::system_clock::now();
            auto const& type_file_path = types_entry.path();
            if (fs::is_regular_file(type_file_path) && type_file_path.extension() == ".yaml") {
                auto type_path = std::string("/") + fs::relative(type_file_path, this->rs->types_dir).stem().string();

                try {
                    // load and validate type file, store validated result in this->types
                    EVLOG_verbose << fmt::format("Loading type file at: {}", fs::canonical(type_file_path).c_str());
                    json type_json = load_yaml(type_file_path);
                    auto start_time_validate = std::chrono::system_clock::now();
                    json_validator validator(Config::loader, Config::format_checker);
                    validator.set_root_schema(this->_schemas.type);
                    validator.validate(type_json);
                    auto end_time_validate = std::chrono::system_clock::now();
                    EVLOG_debug << "YAML validation of " << types_entry.path().string() << " took: "
                                << std::chrono::duration_cast<std::chrono::milliseconds>(end_time_validate -
                                                                                         start_time_validate)
                                       .count()
                                << "ms";
                    total_time_validation_ms +=
                        std::chrono::duration_cast<std::chrono::milliseconds>(end_time_validate - start_time_validate)
                            .count();

                    this->types[type_path] = type_json["types"];
                } catch (const std::exception& e) {
                    EVLOG_AND_THROW(EverestConfigError(fmt::format(
                        "Failed to load and parse type file '{}', reason: {}", type_file_path.string(), e.what())));
                }
            }
            auto end_time = std::chrono::system_clock::now();
            total_time_parsing_ms +=
                std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            EVLOG_debug << "Parsing of type " << types_entry.path().string() << " took: "
                        << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms";
        }
        EVLOG_info << "- Types loaded in [" << total_time_parsing_ms - total_time_validation_ms << "ms]";
        EVLOG_info << "- Types validated [" << total_time_validation_ms << "ms]";
    }

    // load error files
    if (manager or rs->validate_schema) {
        int total_time_validation_ms = 0, total_time_parsing_ms = 0;
        for (auto const& errors_entry : fs::recursive_directory_iterator(this->rs->errors_dir)) {
            auto start_time = std::chrono::system_clock::now();
            auto const& error_file_path = errors_entry.path();
            if (fs::is_regular_file(error_file_path) && error_file_path.extension() == ".yaml") {
                auto error_path =
                    std::string("/") + fs::relative(error_file_path, this->rs->errors_dir).stem().string();

                try {
                    // load and validate error file, store validated result in this->errors
                    EVLOG_verbose << fmt::format("Loading error file at: {}", fs::canonical(error_file_path).c_str());
                    json error_json = load_yaml(error_file_path);
                    auto start_time_validate = std::chrono::system_clock::now();
                    json_validator validator(Config::loader, Config::format_checker);
                    validator.set_root_schema(this->_schemas.error_declaration_list);
                    validator.validate(error_json);
                    auto end_time_validate = std::chrono::system_clock::now();
                    EVLOG_debug << "YAML validation of " << errors_entry.path().string() << " took: "
                                << std::chrono::duration_cast<std::chrono::milliseconds>(end_time_validate -
                                                                                         start_time_validate)
                                       .count()
                                << "ms";
                    total_time_validation_ms +=
                        std::chrono::duration_cast<std::chrono::milliseconds>(end_time_validate - start_time_validate)
                            .count();

                    this->errors[error_path] = error_json["errors"];
                } catch (const std::exception& e) {
                    EVLOG_AND_THROW(EverestConfigError(fmt::format(
                        "Failed to load and parse error file '{}', reason: {}", error_file_path.string(), e.what())));
                }
            }
            auto end_time = std::chrono::system_clock::now();
            total_time_parsing_ms +=
                std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            EVLOG_debug << "Parsing of error " << errors_entry.path().string() << " took: "
                        << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms";
        }
        EVLOG_info << "- Errors loaded in [" << total_time_parsing_ms - total_time_validation_ms << "ms]";
        EVLOG_info << "- Errors validated [" << total_time_validation_ms << "ms]";
    }
    std::optional<std::string> probe_module_id;

    // load manifest files of configured modules
    for (auto& element : this->main.items()) {
        const auto& module_id = element.key();
        const auto& module_config = element.value();

        if (module_config.at("module") == "ProbeModule") {
            if (probe_module_id) {
                EVLOG_AND_THROW(EverestConfigError("Multiple instance of module type ProbeModule not supported yet"));
            }
            probe_module_id = module_id;
            continue;
        }

        load_and_validate_manifest(module_id, module_config);
    }

    if (probe_module_id) {
        auto& manifest = this->manifests["ProbeModule"];

        setup_probe_module_manifest(*probe_module_id, this->main, this->manifests);

        load_and_validate_manifest(*probe_module_id, this->main.at(*probe_module_id));
    }

    // load telemetry configs
    for (auto& element : this->main.items()) {
        const auto& module_id = element.key();
        auto& module_config = element.value();
        std::string module_name = module_config.at("module");
        if (module_config.contains("telemetry")) {
            this->telemetry_configs[module_id].emplace(TelemetryConfig{module_config.at("telemetry").at("id")});
        }
    }

    resolve_all_requirements();
}

error::ErrorTypeMap Config::get_error_map() const {
    return this->error_map;
}

std::string Config::get_module_name(const std::string& module_id) {
    return this->module_names.at(module_id);
}

bool Config::module_provides(const std::string& module_name, const std::string& impl_id) {
    auto& provides = this->module_config_cache.at(module_name).provides_impl;
    return (provides.find(impl_id) != provides.end());
}

json Config::get_module_cmds(const std::string& module_name, const std::string& impl_id) {
    return this->module_config_cache.at(module_name).cmds.at(impl_id);
}

json Config::resolve_interface(const std::string& intf_name) {
    // load and validate interface.json and mark interface as seen
    auto intf_definition = load_interface_file(intf_name);

    this->interface_definitions[intf_name] = intf_definition;
    return intf_definition;
}

std::list<json> Config::resolve_error_ref(const std::string& reference) {
    BOOST_LOG_FUNCTION();
    std::string ref_prefix = "/errors/";
    std::string err_ref = reference.substr(ref_prefix.length());
    auto result = err_ref.find("#/");
    std::string err_namespace;
    std::string err_name;
    bool is_error_list;
    if (result == std::string::npos) {
        err_namespace = err_ref;
        err_name = "";
        is_error_list = true;
    } else {
        err_namespace = err_ref.substr(0, result);
        err_name = err_ref.substr(result + 2);
        is_error_list = false;
    }
    fs::path path = this->rs->errors_dir / (err_namespace + ".yaml");
    json error_json = load_yaml(path);
    std::list<json> errors;
    if (is_error_list) {
        for (auto& error : error_json.at("errors")) {
            error["namespace"] = err_namespace;
            errors.push_back(error);
        }
    } else {
        for (auto& error : error_json.at("errors")) {
            if (error.at("name") == err_name) {
                error["namespace"] = err_namespace;
                errors.push_back(error);
                break;
            }
        }
    }
    return errors;
}

json Config::replace_error_refs(json& interface_json) {
    BOOST_LOG_FUNCTION();
    if (!interface_json.contains("errors")) {
        return interface_json;
    }
    json errors_new = json::object();
    for (auto& error_entry : interface_json.at("errors")) {
        std::list<json> errors = resolve_error_ref(error_entry.at("reference"));
        for (auto& error : errors) {
            if (!errors_new.contains(error.at("namespace"))) {
                errors_new[error.at("namespace")] = json::object();
            }
            if (errors_new.at(error.at("namespace")).contains(error.at("name"))) {
                EVLOG_AND_THROW(EverestConfigError(fmt::format("Error name '{}' in namespace '{}' already referenced!",
                                                               error.at("name"), error.at("namespace"))));
            }
            errors_new[error.at("namespace")][error.at("name")] = error;
        }
    }
    interface_json["errors"] = errors_new;
    return interface_json;
}

json Config::load_interface_file(const std::string& intf_name) {
    BOOST_LOG_FUNCTION();
    fs::path intf_path = this->rs->interfaces_dir / (intf_name + ".yaml");
    try {
        EVLOG_debug << fmt::format("Loading interface file at: {}", fs::canonical(intf_path).string());

        json interface_json = load_yaml(intf_path);

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
        interface_json = Config::replace_error_refs(interface_json);

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
        EVLOG_AND_THROW(EverestConfigError(fmt::format("Failed to load and parse interface file {}: {}",
                                                       fs::weakly_canonical(intf_path).string(), e.what())));
    }
}

json Config::resolve_requirement(const std::string& module_id, const std::string& requirement_id) {
    BOOST_LOG_FUNCTION();

    // FIXME (aw): this function should throw, if the requirement id
    //             isn't even listed in the module manifest
    // FIXME (aw): the following if doesn't check for the requirement id
    //             at all
    auto module_name_it = this->module_names.find(module_id);
    if (module_name_it == this->module_names.end()) {
        EVLOG_AND_THROW(EverestApiError(fmt::format("Requested requirement id '{}' of module {} not found in config!",
                                                    requirement_id, printable_identifier(module_id))));
    }

    // check for connections for this requirement
    auto& module_config = this->main[module_id];
    std::string module_name = module_name_it->second;
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

bool Config::contains(const std::string& module_id) const {
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

ModuleConfigs Config::get_module_configs(const std::string& module_id) const {
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
                        value = data.get<double>();
                    } else {
                        value = data.get<int>();
                    }
                } else if (data.is_boolean()) {
                    value = data.get<bool>();
                } else {
                    EVLOG_AND_THROW(EverestInternalError(
                        fmt::format("Found a config entry: '{}' for module type '{}', which has a data type, that is "
                                    "different from (bool, integer, number, string)",
                                    entry.key(), module_type)));
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
            EverestInternalError(fmt::format("Schema file does not exist at: {}", fs::absolute(path).string())));
    }

    EVLOG_debug << fmt::format("Loading schema file at: {}", fs::canonical(path).string());

    json schema = load_yaml(path);

    json_validator validator(Config::loader, Config::format_checker);

    try {
        validator.set_root_schema(schema);
    } catch (const std::exception& e) {
        EVLOG_AND_THROW(EverestInternalError(
            fmt::format("Validation of schema '{}' failed, here is why: {}", path.string(), e.what())));
    }

    return schema;
}

schemas Config::load_schemas(const fs::path& schemas_dir) {
    BOOST_LOG_FUNCTION();
    schemas schemas;

    EVLOG_debug << fmt::format("Loading base schema files for config and manifests... from: {}", schemas_dir.string());
    schemas.config = Config::load_schema(schemas_dir / "config.yaml");
    schemas.manifest = Config::load_schema(schemas_dir / "manifest.yaml");
    schemas.interface = Config::load_schema(schemas_dir / "interface.yaml");
    schemas.type = Config::load_schema(schemas_dir / "type.yaml");
    schemas.error_declaration_list = Config::load_schema(schemas_dir / "error-declaration-list.yaml");

    return schemas;
}

json Config::load_all_manifests(const std::string& modules_dir, const std::string& schemas_dir) {
    BOOST_LOG_FUNCTION();

    json manifests = json({});

    schemas schemas = Config::load_schemas(schemas_dir);

    fs::path modules_path = fs::path(modules_dir);

    for (auto&& module_path : fs::directory_iterator(modules_path)) {
        fs::path manifest_path = module_path.path() / "manifest.yaml";
        if (!fs::exists(manifest_path)) {
            continue;
        }

        std::string module_name = module_path.path().filename().string();
        EVLOG_debug << fmt::format("Found module {}, loading and verifying manifest...", module_name);

        try {
            manifests[module_name] = load_yaml(manifest_path);

            json_validator validator(Config::loader, Config::format_checker);
            validator.set_root_schema(schemas.manifest);
            validator.validate(manifests[module_name]);
        } catch (const std::exception& e) {
            EVLOG_AND_THROW(EverestConfigError(
                fmt::format("Failed to load and parse module manifest file of module {}: {}", module_name, e.what())));
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
            EverestInternalError(fmt::format("Provided value is not an object. It is a: {}", object.type_name())));
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
    EVTHROW(EverestInternalError(fmt::format("{} is not supported for schema loading at the moment\n", uri.url())));
}

void Config::ref_loader(const json_uri& uri, json& schema) {
    BOOST_LOG_FUNCTION();

    if (uri.location() == "http://json-schema.org/draft-07/schema") {
        schema = nlohmann::json_schema::draft7_schema_builtin;
        return;
    } else {
        auto path = uri.path();
        if (this->types.contains(path)) {
            schema = this->types[path];
            EVLOG_verbose << fmt::format("ref path \"{}\" schema has been found.", path);
            return;
        } else {
            EVLOG_verbose << fmt::format("ref path \"{}\" schema has not been found.", path);
        }
    }

    // TODO(kai): think about supporting more urls here
    EVTHROW(EverestInternalError(fmt::format("{} is not supported for schema loading at the moment\n", uri.url())));
}

void Config::format_checker(const std::string& format, const std::string& value) {
    BOOST_LOG_FUNCTION();

    if (format == "uri") {
        if (value.find("://") == std::string::npos) {
            EVTHROW(std::invalid_argument("URI does not contain :// - invalid"));
        }
    } else if (format == "uri-reference") {
        if (!std::regex_match(value, type_uri_regex)) {
            EVTHROW(std::invalid_argument("Type URI is malformed."));
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
    const auto module_string =
        fmt::format("{}:{}", info["module_id"].get<std::string>(), info["module_name"].get<std::string>());
    if (impl_id.empty()) {
        return module_string;
    }
    return fmt::format("{}->{}:{}", module_string, info["impl_id"].get<std::string>(),
                       info["impl_intf"].get<std::string>());
}

ModuleInfo Config::get_module_info(const std::string& module_id) {
    BOOST_LOG_FUNCTION();
    // FIXME (aw): the following if block is used so often, it should be
    //             refactored into a helper function
    if (!this->main.contains(module_id)) {
        EVTHROW(EverestApiError(fmt::format("Module id '{}' not found in config!", module_id)));
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

std::optional<TelemetryConfig> Config::get_telemetry_config(const std::string& module_id) {
    BOOST_LOG_FUNCTION();

    if (this->telemetry_configs.find(module_id) == this->telemetry_configs.end()) {
        return {};
    }

    return this->telemetry_configs.at(module_id);
}

std::string Config::mqtt_prefix(const std::string& module_id, const std::string& impl_id) {
    BOOST_LOG_FUNCTION();

    return fmt::format("{}{}/{}", this->rs->mqtt_everest_prefix, module_id, impl_id);
}

std::string Config::mqtt_module_prefix(const std::string& module_id) {
    BOOST_LOG_FUNCTION();

    return fmt::format("{}{}", this->rs->mqtt_everest_prefix, module_id);
}

json Config::extract_implementation_info(const std::string& module_id, const std::string& impl_id) {
    BOOST_LOG_FUNCTION();

    if (!this->main.contains(module_id)) {
        EVTHROW(EverestApiError(fmt::format("Module id '{}' not found in config!", module_id)));
    }

    json info;
    info["module_id"] = module_id;
    info["module_name"] = get_module_name(module_id);
    info["impl_id"] = impl_id;
    info["impl_intf"] = "";
    if (!impl_id.empty()) {
        if (!this->manifests.contains(info["module_name"])) {
            EVTHROW(EverestApiError(fmt::format("No known manifest for module name '{}'!", info["module_name"])));
        }

        if (!this->manifests[info["module_name"].get<std::string>()]["provides"].contains(impl_id)) {
            EVTHROW(EverestApiError(fmt::format("Implementation id '{}' not defined in manifest of module '{}'!",
                                                impl_id, info["module_name"])));
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

    EVLOG_debug << "Resolving module reguirements...";
    // this whole code will not check existence of keys defined by config or
    // manifest metaschemas these have already been checked by schema validation
    for (auto& element : this->main.items()) {
        const auto& module_id = element.key();
        EVLOG_verbose << fmt::format("Resolving requirements of module {}...", printable_identifier(module_id));

        auto& module_config = element.value();

        std::set<std::string> unknown_requirement_entries;
        std::set<std::string> module_config_connections_set = Config::keys(module_config["connections"]);
        std::set<std::string> manifest_module_requires_set =
            Config::keys(this->manifests[module_config["module"].get<std::string>()]["requires"]);

        std::set_difference(module_config_connections_set.begin(), module_config_connections_set.end(),
                            manifest_module_requires_set.begin(), manifest_module_requires_set.end(),
                            std::inserter(unknown_requirement_entries, unknown_requirement_entries.end()));

        if (!unknown_requirement_entries.empty()) {
            EVLOG_AND_THROW(EverestApiError(fmt::format("Configured connection for requirement id(s) [{}] of {} not "
                                                        "defined as requirement in manifest of module '{}'!",
                                                        fmt::join(unknown_requirement_entries, " "),
                                                        printable_identifier(module_id), module_config["module"])));
        }

        for (auto& element : this->manifests[module_config["module"].get<std::string>()]["requires"].items()) {
            const auto& requirement_id = element.key();
            auto& requirement = element.value();

            if (!module_config["connections"].contains(requirement_id)) {
                if (requirement["min_connections"] < 1) {
                    EVLOG_debug << fmt::format("Manifest of {} lists OPTIONAL requirement '{}' which could not be "
                                               "fulfilled and will be ignored...",
                                               printable_identifier(module_id), requirement_id);
                    continue; // stop here, there is nothing we can do
                }
                EVLOG_AND_THROW(EverestConfigError(fmt::format(
                    "Requirement '{}' of module {} not fulfilled: requirement id '{}' not listed in connections!",
                    requirement_id, printable_identifier(module_id), requirement_id)));
            }
            json connections = module_config["connections"][requirement_id];

            // check if min_connections and max_connections are fulfilled
            if (connections.size() < requirement["min_connections"] ||
                connections.size() > requirement["max_connections"]) {
                EVLOG_AND_THROW(
                    EverestConfigError(fmt::format("Requirement '{}' of module {} not fulfilled: requirement list does "
                                                   "not have an entry count between {} and {}!",
                                                   requirement_id, printable_identifier(module_id),
                                                   requirement["min_connections"], requirement["max_connections"])));
            }

            for (uint64_t connection_num = 0; connection_num < connections.size(); connection_num++) {
                auto& connection = connections[connection_num];
                const std::string connection_module_id = connection["module_id"];
                if (!this->main.contains(connection_module_id)) {
                    EVLOG_AND_THROW(EverestConfigError(fmt::format(
                        "Requirement '{}' of module {} not fulfilled: module id '{}' (configured in "
                        "connection {}) not loaded in config!",
                        requirement_id, printable_identifier(module_id), connection_module_id, connection_num)));
                }

                std::string connection_module_name = this->main[connection_module_id]["module"];
                std::string connection_impl_id = connection["implementation_id"];
                auto& connection_manifest = this->manifests[connection_module_name];
                if (!connection_manifest["provides"].contains(connection_impl_id)) {
                    EVLOG_AND_THROW(EverestConfigError(
                        fmt::format("Requirement '{}' of module {} not fulfilled: required module {} does not provide "
                                    "an implementation for '{}'!",
                                    requirement_id, printable_identifier(module_id),
                                    printable_identifier(connection["module_id"]), connection_impl_id)));
                }

                auto& connection_provides = connection_manifest["provides"][connection_impl_id];
                std::string requirement_interface = requirement["interface"];

                // check interface requirement
                if (requirement_interface != connection_provides["interface"]) {
                    EVLOG_AND_THROW(EverestConfigError(fmt::format(
                        "Requirement '{}' of module {} not fulfilled by connection to module {}: required "
                        "interface "
                        "'{}' is not provided by this implementation! Connected implementation provides interface "
                        "'{}'.",
                        requirement_id, printable_identifier(module_id),
                        printable_identifier(connection["module_id"], connection_impl_id), requirement_interface,
                        connection_provides["interface"].get<std::string>())));
                }

                module_config["connections"][requirement_id][connection_num]["provides"] = connection_provides;
                module_config["connections"][requirement_id][connection_num]["required_interface"] =
                    requirement_interface;
                EVLOG_debug << fmt::format(
                    "Manifest of {} lists requirement '{}' which will be fulfilled by {}...",
                    printable_identifier(module_id), requirement_id,
                    printable_identifier(connection["module_id"], connection["implementation_id"]));
            }
        }
    }
    EVLOG_debug << "All module requirements resolved successfully...";
}
} // namespace Everest
