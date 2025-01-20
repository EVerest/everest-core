// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <algorithm>
#include <cstddef>
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
using json = nlohmann::json;
using json_uri = nlohmann::json_uri;
using json_validator = nlohmann::json_schema::json_validator;

class ConfigParseException : public std::exception {
public:
    enum ParseErrorType {
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

struct ParsedConfigMap {
    json parsed_config_map{};
    std::set<std::string> unknown_config_entries;
};

void loader(const json_uri& uri, json& schema) {
    BOOST_LOG_FUNCTION();

    if (uri.location() == "http://json-schema.org/draft-07/schema") {
        schema = nlohmann::json_schema::draft7_schema_builtin;
        return;
    }

    // TODO(kai): think about supporting more urls here
    EVTHROW(EverestInternalError(fmt::format("{} is not supported for schema loading at the moment\n", uri.url())));
}

void format_checker(const std::string& format, const std::string& value) {
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

std::tuple<nlohmann::json, nlohmann::json_schema::json_validator> load_schema(const fs::path& path) {
    BOOST_LOG_FUNCTION();

    if (!fs::exists(path)) {
        EVLOG_AND_THROW(
            EverestInternalError(fmt::format("Schema file does not exist at: {}", fs::absolute(path).string())));
    }

    EVLOG_debug << fmt::format("Loading schema file at: {}", fs::canonical(path).string());

    json schema = load_yaml(path);

    auto validator = nlohmann::json_schema::json_validator(loader, format_checker);

    try {
        validator.set_root_schema(schema);
    } catch (const std::exception& e) {
        EVLOG_AND_THROW(EverestInternalError(
            fmt::format("Validation of schema '{}' failed, here is why: {}", path.string(), e.what())));
    }

    return std::make_tuple<nlohmann::json, nlohmann::json_schema::json_validator>(std::move(schema),
                                                                                  std::move(validator));
}

SchemaValidation load_schemas(const fs::path& schemas_dir) {
    BOOST_LOG_FUNCTION();
    SchemaValidation schema_validation;

    EVLOG_debug << fmt::format("Loading base schema files for config and manifests... from: {}", schemas_dir.string());
    auto [config_schema, config_val] = load_schema(schemas_dir / "config.yaml");
    schema_validation.schemas.config = config_schema;
    schema_validation.validators.config = std::move(config_val);
    auto [manifest_schema, manifest_val] = load_schema(schemas_dir / "manifest.yaml");
    schema_validation.schemas.manifest = manifest_schema;
    schema_validation.validators.manifest = std::move(manifest_val);
    auto [interface_schema, interface_val] = load_schema(schemas_dir / "interface.yaml");
    schema_validation.schemas.interface = interface_schema;
    schema_validation.validators.interface = std::move(interface_val);
    auto [type_schema, type_val] = load_schema(schemas_dir / "type.yaml");
    schema_validation.schemas.type = type_schema;
    schema_validation.validators.type = std::move(type_val);
    auto [error_declaration_list_schema, error_declaration_list_val] =
        load_schema(schemas_dir / "error-declaration-list.yaml");
    schema_validation.schemas.error_declaration_list = error_declaration_list_schema;
    schema_validation.validators.error_declaration_list = std::move(error_declaration_list_val);

    return schema_validation;
}

static void validate_config_schema(const json& config_map_schema) {
    // iterate over every config entry
    json_validator validator(loader, format_checker);
    for (const auto& config_item : config_map_schema.items()) {
        if (!config_item.value().contains("default")) {
            continue;
        }

        try {
            validator.set_root_schema(config_item.value());
            validator.validate(config_item.value().at("default"));
        } catch (const std::exception& e) {
            throw std::runtime_error(fmt::format("Config item '{}' has issues:\n{}", config_item.key(), e.what()));
        }
    }
}

static ParsedConfigMap parse_config_map(const json& config_map_schema, const json& config_map) {
    json parsed_config_map{};

    std::set<std::string> unknown_config_entries;
    const std::set<std::string> config_map_keys = Config::keys(config_map);
    const std::set<std::string> config_map_schema_keys = Config::keys(config_map_schema);

    std::set_difference(config_map_keys.begin(), config_map_keys.end(), config_map_schema_keys.begin(),
                        config_map_schema_keys.end(),
                        std::inserter(unknown_config_entries, unknown_config_entries.end()));

    // validate each config entry
    for (const auto& config_entry_el : config_map_schema.items()) {
        const std::string& config_entry_name = config_entry_el.key();
        const json& config_entry = config_entry_el.value();

        // only convenience exception, would be catched by schema validation below if not thrown here
        if (!config_entry.contains("default") and !config_map.contains(config_entry_name)) {
            throw ConfigParseException(ConfigParseException::MISSING_ENTRY, config_entry_name);
        }

        json config_entry_value;
        if (config_map.contains(config_entry_name)) {
            config_entry_value = config_map[config_entry_name];
        } else if (config_entry.contains("default")) {
            config_entry_value = config_entry.at("default"); // use default value defined in manifest
        }
        json_validator validator(loader, format_checker);
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

    return {parsed_config_map, unknown_config_entries};
}

static auto get_provides_for_probe_module(const std::string& probe_module_id, const json& config,
                                          const json& manifests) {
    auto provides = json::object();

    for (const auto& item : config.items()) {
        if (item.key() == probe_module_id) {
            // do not parse ourself
            continue;
        }

        const auto& module_config = item.value();

        const auto& connections = module_config.value("connections", json::object());

        for (const auto& connection : connections.items()) {
            const std::string& req_id = connection.key();
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
    const auto& probe_module_config = config.at(probe_module_id);

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
                    EverestConfigError(fmt::format("ProbeModule refers to a non-existent module id '{}'", module_id)));
            }

            const auto& module_manifest = manifests.at(module_config_it->at("module"));

            const auto& module_provides_it = module_manifest.find("provides");

            if (module_provides_it == module_manifest.end()) {
                EVLOG_AND_THROW(EverestConfigError(fmt::format(
                    "ProbeModule requires something from module id '{}' but it does not provide anything", module_id)));
            }

            const auto& provide_it = module_provides_it->find(impl_id);
            if (provide_it == module_provides_it->end()) {
                EVLOG_AND_THROW(EverestConfigError(
                    fmt::format("ProbeModule requires something from module id '{}', but it does not provide '{}'",
                                module_id, impl_id)));
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

ImplementationInfo extract_implementation_info(const std::unordered_map<std::string, std::string>& module_names,
                                               const json& manifests, const std::string& module_id,
                                               const std::string& impl_id) {
    BOOST_LOG_FUNCTION();

    if (module_names.find(module_id) == module_names.end()) {
        EVTHROW(EverestApiError(fmt::format("Module id '{}' not found in config!", module_id)));
    }
    ImplementationInfo info;
    info.module_id = module_id;
    info.module_name = module_names.at(module_id);
    info.impl_id = impl_id;

    if (!impl_id.empty()) {
        if (not manifests.contains(info.module_name)) {
            EVTHROW(EverestApiError(fmt::format("No known manifest for module name '{}'!", info.module_name)));
        }

        if (not manifests.at(info.module_name).at("provides").contains(impl_id)) {
            EVTHROW(EverestApiError(fmt::format("Implementation id '{}' not defined in manifest of module '{}'!",
                                                impl_id, info.module_name)));
        }

        info.impl_intf = manifests.at(info.module_name).at("provides").at(impl_id).at("interface");
    }

    return info;
}

std::string create_printable_identifier(const ImplementationInfo& info, const std::string& module_id,
                                        const std::string& impl_id) {
    BOOST_LOG_FUNCTION();

    // no implementation id yet so only return this kind of string:
    auto module_string = fmt::format("{}:{}", info.module_id, info.module_name);
    if (impl_id.empty()) {
        return module_string;
    }
    return fmt::format("{}->{}:{}", module_string, info.impl_id, info.impl_intf);
}

// ConfigBase

std::string ConfigBase::printable_identifier(const std::string& module_id) const {
    BOOST_LOG_FUNCTION();

    return printable_identifier(module_id, "");
}

std::string ConfigBase::printable_identifier(const std::string& module_id, const std::string& impl_id) const {
    BOOST_LOG_FUNCTION();

    const auto info = extract_implementation_info(this->module_names, this->manifests, module_id, impl_id);
    return create_printable_identifier(info, module_id, impl_id);
}

std::string ConfigBase::get_module_name(const std::string& module_id) const {
    return this->module_names.at(module_id);
}

std::string ConfigBase::mqtt_prefix(const std::string& module_id, const std::string& impl_id) {
    BOOST_LOG_FUNCTION();

    return fmt::format("{}modules/{}/impl/{}", this->mqtt_settings.everest_prefix, module_id, impl_id);
}

std::string ConfigBase::mqtt_module_prefix(const std::string& module_id) {
    BOOST_LOG_FUNCTION();

    return fmt::format("{}modules/{}", this->mqtt_settings.everest_prefix, module_id);
}

const json& ConfigBase::get_main_config() const {
    BOOST_LOG_FUNCTION();
    return this->main;
}

bool ConfigBase::contains(const std::string& module_id) const {
    BOOST_LOG_FUNCTION();
    return this->main.contains(module_id);
}

const json& ConfigBase::get_manifests() const {
    BOOST_LOG_FUNCTION();
    return this->manifests;
}

const json& ConfigBase::get_interface_definitions() const {
    BOOST_LOG_FUNCTION();
    return this->interface_definitions;
}

const json& ConfigBase::get_interfaces() const {
    BOOST_LOG_FUNCTION();
    return this->interfaces;
}

const json& ConfigBase::get_settings() const {
    BOOST_LOG_FUNCTION();
    return this->settings;
}

const json ConfigBase::get_schemas() const {
    BOOST_LOG_FUNCTION();
    return this->schemas;
}

json ConfigBase::get_error_types() {
    BOOST_LOG_FUNCTION();
    return this->error_map.get_error_types();
}

const json& ConfigBase::get_types() const {
    BOOST_LOG_FUNCTION();
    return this->types;
}

std::unordered_map<std::string, ConfigCache> ConfigBase::get_module_config_cache() {
    BOOST_LOG_FUNCTION();
    return this->module_config_cache;
}

std::unordered_map<std::string, std::string> ConfigBase::get_module_names() {
    return this->module_names;
}

json ConfigBase::resolve_requirement(const std::string& module_id, const std::string& requirement_id) const {
    BOOST_LOG_FUNCTION();

    // FIXME (aw): this function should throw, if the requirement id
    //             isn't even listed in the module manifest
    // FIXME (aw): the following if doesn't check for the requirement id
    //             at all
    const auto module_name_it = this->module_names.find(module_id);
    if (module_name_it == this->module_names.end()) {
        EVLOG_AND_THROW(EverestApiError(fmt::format("Requested requirement id '{}' of module {} not found in config!",
                                                    requirement_id, printable_identifier(module_id))));
    }

    // check for connections for this requirement
    const auto& module_config = this->main.at(module_id);
    const std::string module_name = module_name_it->second;
    const auto& requirement = this->manifests.at(module_name).at("requires").at(requirement_id);
    if (!module_config.at("connections").contains(requirement_id)) {
        return json::array(); // return an empty array if our config does not contain any connections for this
                              // requirement id
    }

    // if only one single connection entry was required, return only this one
    // callers can check with is_array() if this is a single connection (legacy) or a connection list
    if (requirement.at("min_connections") == 1 && requirement.at("max_connections") == 1) {
        return module_config.at("connections").at(requirement_id).at(0);
    }
    return module_config.at("connections").at(requirement_id);
}

std::map<Requirement, Fulfillment> ConfigBase::resolve_requirements(const std::string& module_id) const {
    std::map<Requirement, Fulfillment> requirements;

    const auto& module_name = get_module_name(module_id);
    for (const auto& req_id : Config::keys(this->manifests.at(module_name).at("requires"))) {
        const auto& resolved_req = this->resolve_requirement(module_id, req_id);
        if (!resolved_req.is_array()) {
            const auto& resolved_module_id = resolved_req.at("module_id");
            const auto& resolved_impl_id = resolved_req.at("implementation_id");
            const auto req = Requirement{req_id, 0};
            requirements[req] = {resolved_module_id, resolved_impl_id, req};
        } else {
            for (std::size_t i = 0; i < resolved_req.size(); i++) {
                const auto& resolved_module_id = resolved_req.at(i).at("module_id");
                const auto& resolved_impl_id = resolved_req.at(i).at("implementation_id");
                const auto req = Requirement{req_id, i};
                requirements[req] = {resolved_module_id, resolved_impl_id, req};
            }
        }
    }

    return requirements;
}

std::list<Requirement> ConfigBase::get_requirements(const std::string& module_id) const {
    BOOST_LOG_FUNCTION();

    std::list<Requirement> res;

    for (const auto& [requirement, fulfillment] : this->resolve_requirements(module_id)) {
        res.push_back(requirement);
    }

    return res;
}

std::map<std::string, std::vector<Fulfillment>> ConfigBase::get_fulfillments(const std::string& module_id) const {
    BOOST_LOG_FUNCTION();

    std::map<std::string, std::vector<Fulfillment>> res;

    for (const auto& [requirement, fulfillment] : this->resolve_requirements(module_id)) {
        res[requirement.id].push_back(fulfillment);
    }

    return res;
}

std::unordered_map<std::string, ModuleTierMappings> ConfigBase::get_3_tier_model_mappings() {
    return this->tier_mappings;
}

std::optional<ModuleTierMappings> ConfigBase::get_module_3_tier_model_mappings(const std::string& module_id) const {
    if (this->tier_mappings.find(module_id) == this->tier_mappings.end()) {
        return std::nullopt;
    }
    return this->tier_mappings.at(module_id);
}

std::optional<Mapping> ConfigBase::get_3_tier_model_mapping(const std::string& module_id,
                                                            const std::string& impl_id) const {
    const auto module_tier_mappings = this->get_module_3_tier_model_mappings(module_id);
    if (not module_tier_mappings.has_value()) {
        return std::nullopt;
    }
    const auto& mapping = module_tier_mappings.value();
    if (mapping.implementations.find(impl_id) == mapping.implementations.end()) {
        // if no specific implementation mapping is given, use the module mapping
        return mapping.module;
    }
    return mapping.implementations.at(impl_id);
}

// ManagerConfig
void ManagerConfig::load_and_validate_manifest(const std::string& module_id, const json& module_config) {
    const std::string module_name = module_config.at("module");

    this->module_config_cache[module_id] = ConfigCache();
    this->module_names[module_id] = module_name;
    EVLOG_debug << fmt::format("Found module {}, loading and verifying manifest...", printable_identifier(module_id));

    // load and validate module manifest.json
    const fs::path manifest_path = this->ms.runtime_settings->modules_dir / module_name / "manifest.yaml";
    try {

        if (module_name != "ProbeModule") {
            // FIXME (aw): this is implicit logic, because we know, that the ProbeModule manifest had been set up
            // manually already
            EVLOG_debug << fmt::format("Loading module manifest file at: {}", fs::canonical(manifest_path).string());
            this->manifests[module_name] = load_yaml(manifest_path);
        }

        const auto patch = this->validators.manifest.validate(this->manifests[module_name]);
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

    for (const auto& impl : this->manifests[module_name]["provides"].items()) {
        try {
            validate_config_schema(impl.value().at("config"));
        } catch (const std::exception& e) {
            EVLOG_AND_THROW(
                EverestConfigError(fmt::format("Failed to validate the implementation configuration meta-schema "
                                               "for implementation '{}' in module '{}'. Reason:\n{}",
                                               impl.key(), module_name, e.what())));
        }
    }

    const std::set<std::string> provided_impls = Config::keys(this->manifests[module_name]["provides"]);

    this->interfaces[module_name] = json({});
    this->module_config_cache[module_name].provides_impl = provided_impls;

    for (const auto& impl_id : provided_impls) {
        EVLOG_debug << fmt::format("Loading interface for implementation: {}", impl_id);
        auto intf_name = this->manifests[module_name]["provides"][impl_id]["interface"].get<std::string>();
        auto seen_interfaces = std::set<std::string>();
        this->interfaces[module_name][impl_id] = intf_name;
        resolve_interface(intf_name);
        this->module_config_cache[module_name].cmds[impl_id] = this->interface_definitions.at(intf_name).at("cmds");
    }

    // check if config only contains impl_ids listed in manifest file
    std::set<std::string> unknown_impls_in_config;
    const std::set<std::string> configured_impls = Config::keys(this->main[module_id]["config_implementation"]);

    std::set_difference(configured_impls.begin(), configured_impls.end(), provided_impls.begin(), provided_impls.end(),
                        std::inserter(unknown_impls_in_config, unknown_impls_in_config.end()));

    if (!unknown_impls_in_config.empty()) {
        EVLOG_AND_THROW(EverestApiError(
            fmt::format("Implementation id(s)[{}] mentioned in config, but not defined in manifest of module '{}'!",
                        fmt::join(unknown_impls_in_config, " "), module_config.at("module"))));
    }

    // validate config entries against manifest file
    for (const auto& impl_id : provided_impls) {
        EVLOG_verbose << fmt::format(
            "Validating implementation config of {} against json schemas defined in module mainfest...",
            printable_identifier(module_id, impl_id));

        const json config_map =
            module_config.value("config_implementation", json::object()).value(impl_id, json::object());
        const json config_map_schema =
            this->manifests[module_config.at("module").get<std::string>()]["provides"][impl_id]["config"];

        try {
            const auto parsed_config_map = parse_config_map(config_map_schema, config_map);
            if (parsed_config_map.unknown_config_entries.size()) {
                for (const auto& unknown_entry : parsed_config_map.unknown_config_entries) {
                    EVLOG_error << fmt::format(
                        "Unknown config entry '{}' of {} of module '{}' ignored, please fix your config file!",
                        unknown_entry, printable_identifier(module_id, impl_id), module_config.at("module"));
                }
            }
            this->main[module_id]["config_maps"][impl_id] = parsed_config_map.parsed_config_map;
        } catch (const ConfigParseException& err) {
            if (err.err_t == ConfigParseException::MISSING_ENTRY) {
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
        const json& config_map = module_config.at("config_module");
        const json config_map_schema = this->manifests[module_config.at("module").get<std::string>()]["config"];

        try {
            auto parsed_config_map = parse_config_map(config_map_schema, config_map);
            if (parsed_config_map.unknown_config_entries.size()) {
                for (const auto& unknown_entry : parsed_config_map.unknown_config_entries) {
                    EVLOG_error << fmt::format(
                        "Unknown config entry '{}' of module '{}' ignored, please fix your config file!", unknown_entry,
                        module_config.at("module"));
                }
            }
            this->main[module_id]["config_maps"]["!module"] = parsed_config_map.parsed_config_map;
        } catch (const ConfigParseException& err) {
            if (err.err_t == ConfigParseException::MISSING_ENTRY) {
                EVLOG_AND_THROW(
                    EverestConfigError(fmt::format("Missing mandatory config entry '{}' for module config in module {}",
                                                   err.entry, module_config.at("module"))));
            } else if (err.err_t == ConfigParseException::SCHEMA) {
                EVLOG_AND_THROW(EverestConfigError(fmt::format(
                    "Schema validation for config entry '{}' failed for module config in module {}! Reason:\n{}",
                    err.entry, module_config.at("module"), err.what)));
            } else {
                throw err;
            }
        }
    }
}

std::tuple<json, int64_t> ManagerConfig::load_and_validate_with_schema(const fs::path& file_path, const json& schema) {
    const json json_to_validate = load_yaml(file_path);
    int64_t validation_ms = 0;

    const auto start_time_validate = std::chrono::system_clock::now();
    json_validator validator(loader, format_checker);
    validator.set_root_schema(schema);
    validator.validate(json_to_validate);
    const auto end_time_validate = std::chrono::system_clock::now();
    EVLOG_debug
        << "YAML validation of " << file_path.string() << " took: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end_time_validate - start_time_validate).count()
        << "ms";

    validation_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time_validate - start_time_validate).count();

    return {json_to_validate, validation_ms};
}

json ManagerConfig::resolve_interface(const std::string& intf_name) {
    // load and validate interface.json and mark interface as seen
    const auto intf_definition = load_interface_file(intf_name);

    this->interface_definitions[intf_name] = intf_definition;
    return intf_definition;
}

json ManagerConfig::load_interface_file(const std::string& intf_name) {
    BOOST_LOG_FUNCTION();
    const fs::path intf_path = this->ms.interfaces_dir / (intf_name + ".yaml");
    try {
        EVLOG_debug << fmt::format("Loading interface file at: {}", fs::canonical(intf_path).string());

        json interface_json = load_yaml(intf_path);

        // this subschema can not use allOf with the draft-07 schema because that will cause our validator to
        // add all draft-07 default values which never validate (the {"not": true} default contradicts everything)
        // --> validating against draft-07 will be done in an extra step below
        auto patch = this->validators.interface.validate(interface_json);
        if (!patch.is_null()) {
            // extend config entry with default values
            interface_json = interface_json.patch(patch);
        }
        interface_json = ManagerConfig::replace_error_refs(interface_json);

        // erase "description"
        if (interface_json.contains("description")) {
            interface_json.erase("description");
        }

        // validate every cmd arg/result and var definition against draft-07 schema
        for (auto& var_entry : interface_json["vars"].items()) {
            auto& var_value = var_entry.value();
            // erase "description"
            if (var_value.contains("description")) {
                var_value.erase("description");
            }
            if (var_value.contains("items")) {
                auto& items = var_value.at("items");
                if (items.contains("description")) {
                    items.erase("description");
                }
                if (items.contains("properties")) {
                    for (auto& property : items.at("properties").items()) {
                        auto& property_value = property.value();
                        if (property_value.contains("description")) {
                            property_value.erase("description");
                        }
                    }
                }
            }
            this->draft7_validator->validate(var_value);
        }
        for (auto& cmd_entry : interface_json["cmds"].items()) {
            auto& cmd = interface_json["cmds"][cmd_entry.key()];
            // erase "description"
            if (cmd.contains("description")) {
                cmd.erase("description");
            }
            for (auto& arguments_entry : cmd["arguments"].items()) {
                auto& arg_entry = arguments_entry.value();
                // erase "description"
                if (arg_entry.contains("description")) {
                    arg_entry.erase("description");
                }
                this->draft7_validator->validate(arg_entry);
            }
            auto& result = interface_json["cmds"][cmd_entry.key()]["result"];
            // erase "description"
            if (result.contains("description")) {
                result.erase("description");
            }
            this->draft7_validator->validate(result);
        }

        return interface_json;
    } catch (const std::exception& e) {
        EVLOG_AND_THROW(EverestConfigError(fmt::format("Failed to load and parse interface file {}: {}",
                                                       fs::weakly_canonical(intf_path).string(), e.what())));
    }
}

std::list<json> ManagerConfig::resolve_error_ref(const std::string& reference) {
    BOOST_LOG_FUNCTION();
    const std::string ref_prefix = "/errors/";
    const std::string err_ref = reference.substr(ref_prefix.length());
    const auto result = err_ref.find("#/");
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
    const fs::path path = this->ms.errors_dir / (err_namespace + ".yaml");
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

json ManagerConfig::replace_error_refs(json& interface_json) {
    BOOST_LOG_FUNCTION();
    if (!interface_json.contains("errors")) {
        return interface_json;
    }
    json errors_new = json::object();
    for (auto& error_entry : interface_json.at("errors")) {
        const std::list<json> errors = resolve_error_ref(error_entry.at("reference"));
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

void ManagerConfig::resolve_all_requirements() {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << "Resolving module requirements...";
    // this whole code will not check existence of keys defined by config or
    // manifest metaschemas these have already been checked by schema validation
    for (auto& element : this->main.items()) {
        const auto& module_id = element.key();

        auto& module_config = element.value();

        std::set<std::string> unknown_requirement_entries;
        const std::set<std::string> module_config_connections_set = Config::keys(module_config["connections"]);
        const std::set<std::string> manifest_module_requires_set =
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
            const auto& requirement = element.value();

            if (!module_config["connections"].contains(requirement_id)) {
                if (requirement.at("min_connections") < 1) {
                    EVLOG_debug << fmt::format("Manifest of {} lists OPTIONAL requirement '{}' which could not be "
                                               "fulfilled and will be ignored...",
                                               printable_identifier(module_id), requirement_id);
                    continue; // stop here, there is nothing we can do
                }
                EVLOG_AND_THROW(EverestConfigError(fmt::format(
                    "Requirement '{}' of module {} not fulfilled: requirement id '{}' not listed in connections!",
                    requirement_id, printable_identifier(module_id), requirement_id)));
            }
            const json& connections = module_config["connections"][requirement_id];

            // check if min_connections and max_connections are fulfilled
            if (connections.size() < requirement.at("min_connections") ||
                connections.size() > requirement.at("max_connections")) {
                EVLOG_AND_THROW(EverestConfigError(
                    fmt::format("Requirement '{}' of module {} not fulfilled: requirement list does "
                                "not have an entry count between {} and {}!",
                                requirement_id, printable_identifier(module_id), requirement.at("min_connections"),
                                requirement.at("max_connections"))));
            }

            for (uint64_t connection_num = 0; connection_num < connections.size(); connection_num++) {
                const auto& connection = connections[connection_num];
                const std::string& connection_module_id = connection.at("module_id");
                if (!this->main.contains(connection_module_id)) {
                    EVLOG_AND_THROW(EverestConfigError(fmt::format(
                        "Requirement '{}' of module {} not fulfilled: module id '{}' (configured in "
                        "connection {}) not loaded in config!",
                        requirement_id, printable_identifier(module_id), connection_module_id, connection_num)));
                }

                const std::string& connection_module_name = this->main[connection_module_id]["module"];
                const std::string& connection_impl_id = connection.at("implementation_id");
                const auto& connection_manifest = this->manifests[connection_module_name];
                if (!connection_manifest.at("provides").contains(connection_impl_id)) {
                    EVLOG_AND_THROW(EverestConfigError(
                        fmt::format("Requirement '{}' of module {} not fulfilled: required module {} does not provide "
                                    "an implementation for '{}'!",
                                    requirement_id, printable_identifier(module_id),
                                    printable_identifier(connection.at("module_id")), connection_impl_id)));
                }

                // FIXME: copy here so we can safely erase description and config entries
                // FIXME: if we were to copy here this costs us a huge amount of performance during startup
                // FIXME: or does it really? tests are inconclusive right now...
                auto connection_provides = connection_manifest.at("provides").at(connection_impl_id);
                if (connection_provides.contains("config")) {
                    connection_provides.erase("config");
                }
                if (connection_provides.contains("description")) {
                    connection_provides.erase("description");
                }
                const std::string& requirement_interface = requirement.at("interface");

                // check interface requirement
                if (requirement_interface != connection_provides.at("interface")) {
                    EVLOG_AND_THROW(EverestConfigError(fmt::format(
                        "Requirement '{}' of module {} not fulfilled by connection to module {}: required "
                        "interface "
                        "'{}' is not provided by this implementation! Connected implementation provides interface "
                        "'{}'.",
                        requirement_id, printable_identifier(module_id),
                        printable_identifier(connection.at("module_id"), connection_impl_id), requirement_interface,
                        connection_provides.at("interface").get<std::string>())));
                }

                module_config["connections"][requirement_id][connection_num]["provides"] = connection_provides;
                module_config["connections"][requirement_id][connection_num]["required_interface"] =
                    requirement_interface;
                EVLOG_debug << fmt::format(
                    "Manifest of {} lists requirement '{}' which will be fulfilled by {}...",
                    printable_identifier(module_id), requirement_id,
                    printable_identifier(connection.at("module_id"), connection.at("implementation_id")));
            }
        }
    }
    EVLOG_debug << "All module requirements resolved successfully...";
}

void ManagerConfig::parse(json config) {
    this->main = std::move(config);
    // load type files
    if (this->ms.runtime_settings->validate_schema) {
        int64_t total_time_validation_ms = 0, total_time_parsing_ms = 0;
        for (auto const& types_entry : fs::recursive_directory_iterator(this->ms.types_dir)) {
            const auto start_time = std::chrono::system_clock::now();
            const auto& type_file_path = types_entry.path();
            if (fs::is_regular_file(type_file_path) && type_file_path.extension() == ".yaml") {
                const auto type_path =
                    std::string("/") + fs::relative(type_file_path, this->ms.types_dir).stem().string();

                try {
                    // load and validate type file, store validated result in this->types
                    EVLOG_verbose << fmt::format("Loading type file at: {}", fs::canonical(type_file_path).c_str());

                    const auto [type_json, validate_ms] =
                        load_and_validate_with_schema(type_file_path, this->schemas.type);
                    total_time_validation_ms += validate_ms;

                    this->types[type_path] = type_json.at("types");
                } catch (const std::exception& e) {
                    EVLOG_AND_THROW(EverestConfigError(fmt::format(
                        "Failed to load and parse type file '{}', reason: {}", type_file_path.string(), e.what())));
                }
            }
            const auto end_time = std::chrono::system_clock::now();
            total_time_parsing_ms +=
                std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            EVLOG_debug << "Parsing of type " << types_entry.path().string() << " took: "
                        << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms";
        }
        EVLOG_info << "- Types loaded in [" << total_time_parsing_ms - total_time_validation_ms << "ms]";
        EVLOG_info << "- Types validated [" << total_time_validation_ms << "ms]";
    }

    // load error files
    if (this->ms.runtime_settings->validate_schema) {
        int64_t total_time_validation_ms = 0, total_time_parsing_ms = 0;
        for (auto const& errors_entry : fs::recursive_directory_iterator(this->ms.errors_dir)) {
            const auto start_time = std::chrono::system_clock::now();
            const auto& error_file_path = errors_entry.path();
            if (fs::is_regular_file(error_file_path) && error_file_path.extension() == ".yaml") {
                try {
                    // load and validate error file
                    EVLOG_verbose << fmt::format("Loading error file at: {}", fs::canonical(error_file_path).c_str());

                    const auto [error_json, validate_ms] =
                        load_and_validate_with_schema(error_file_path, this->schemas.error_declaration_list);
                    total_time_validation_ms += validate_ms;

                } catch (const std::exception& e) {
                    EVLOG_AND_THROW(EverestConfigError(fmt::format(
                        "Failed to load and parse error file '{}', reason: {}", error_file_path.string(), e.what())));
                }
            }
            const auto end_time = std::chrono::system_clock::now();
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
    for (const auto& element : this->main.items()) {
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
        setup_probe_module_manifest(*probe_module_id, this->main, this->manifests);

        load_and_validate_manifest(*probe_module_id, this->main.at(*probe_module_id));
    }

    // load telemetry configs
    for (const auto& element : this->main.items()) {
        const auto& module_id = element.key();
        auto& module_config = element.value();
        if (module_config.contains("telemetry")) {
            const auto& telemetry = module_config.at("telemetry");
            if (telemetry.contains("id")) {
                this->telemetry_configs[module_id].emplace(TelemetryConfig{telemetry.at("id").get<int>()});
            }
        }
    }

    resolve_all_requirements();
    parse_3_tier_model_mapping();

    // TODO: cleanup "descriptions" from config ?
}

void ManagerConfig::parse_3_tier_model_mapping() {
    for (const auto& element : this->main.items()) {
        const auto& module_id = element.key();
        const auto& module_name = this->get_module_name(module_id);
        const auto& provides = this->manifests.at(module_name).at("provides");

        ModuleTierMappings module_tier_mappings;
        const auto& module_config = element.value();
        const auto& config_mapping = module_config.at("mapping");
        // an empty mapping means it is mapped to the charging station and gets no specific mapping attached
        if (not config_mapping.empty()) {
            if (config_mapping.contains("module")) {
                const auto& module_mapping = config_mapping.at("module");
                if (module_mapping.contains("evse")) {
                    auto mapping = Mapping(module_mapping.at("evse").get<int>());
                    if (module_mapping.contains("connector")) {
                        mapping.connector = module_mapping.at("connector").get<int>();
                    }
                    module_tier_mappings.module = mapping;
                }
            }

            if (config_mapping.contains("implementations")) {
                const auto& implementations_mapping = config_mapping.at("implementations");
                for (auto& tier_mapping : implementations_mapping.items()) {
                    const auto& impl_id = tier_mapping.key();
                    const auto& tier_mapping_value = tier_mapping.value();
                    if (provides.contains(impl_id)) {
                        if (tier_mapping_value.contains("connector")) {
                            module_tier_mappings.implementations[impl_id] =
                                Mapping(tier_mapping_value.at("evse").get<int>(),
                                        tier_mapping_value.at("connector").get<int>());
                        } else {
                            module_tier_mappings.implementations[impl_id] =
                                Mapping(tier_mapping_value.at("evse").get<int>());
                        }
                    } else {
                        EVLOG_warning << fmt::format("Mapping {} of module {} in config refers to a provides that does "
                                                     "not exist, please fix this",
                                                     impl_id, printable_identifier(module_id));
                    }
                }
            }
        }
        this->tier_mappings[module_id] = module_tier_mappings;
    }
}

ManagerConfig::ManagerConfig(const ManagerSettings& ms) : ConfigBase(ms.mqtt_settings), ms(ms) {
    BOOST_LOG_FUNCTION();

    this->manifests = json({});
    this->interfaces = json({});
    this->interface_definitions = json({});
    this->types = json({});
    auto schema_validation = load_schemas(this->ms.schemas_dir);
    this->schemas = schema_validation.schemas;
    this->validators = std::move(schema_validation.validators);
    this->error_map = error::ErrorTypeMap(this->ms.errors_dir);
    this->draft7_validator = std::make_unique<json_validator>(loader, format_checker);
    this->draft7_validator->set_root_schema(draft07);

    // load and process config file
    const fs::path config_path = this->ms.config_file;

    try {
        EVLOG_info << fmt::format("Loading config file at: {}", fs::canonical(config_path).string());
        auto complete_config = this->ms.config;
        // try to load user config from a directory "user-config" that might be in the same parent directory as the
        // config_file. The config is supposed to have the same name as the parent config.
        // TODO(kai): introduce a parameter that can overwrite the location of the user config?
        // TODO(kai): or should we introduce a "meta-config" that references all configs that should be merged here?
        const auto user_config_path = config_path.parent_path() / "user-config" / config_path.filename();
        if (fs::exists(user_config_path)) {
            EVLOG_info << fmt::format("Loading user-config file at: {}", fs::canonical(user_config_path).string());
            auto user_config = load_yaml(user_config_path);
            EVLOG_debug << "Augmenting main config with user-config entries";
            complete_config.merge_patch(user_config);
        } else {
            EVLOG_verbose << "No user-config provided.";
        }

        const auto patch = this->validators.config.validate(complete_config);
        if (!patch.is_null()) {
            // extend config with default values
            complete_config = complete_config.patch(patch);
        }

        const auto config = complete_config.at("active_modules");
        this->settings = this->ms.get_runtime_settings();
        this->parse(config);
    } catch (const std::exception& e) {
        EVLOG_AND_THROW(EverestConfigError(fmt::format("Failed to load and parse config file: {}", e.what())));
    }
}

json ManagerConfig::serialize() {
    return json::object({{"main", this->main}, {"module_names", this->module_names}});
}

std::optional<TelemetryConfig> ManagerConfig::get_telemetry_config(const std::string& module_id) {
    BOOST_LOG_FUNCTION();

    if (this->telemetry_configs.find(module_id) == this->telemetry_configs.end()) {
        return std::nullopt;
    }

    return this->telemetry_configs.at(module_id);
}

// Config

Config::Config(const MQTTSettings& mqtt_settings, json serialized_config) : ConfigBase(mqtt_settings) {
    this->main = serialized_config.value("module_config", json({}));
    this->manifests = serialized_config.value("manifests", json({}));
    this->interfaces = serialized_config.value("module_provides", json({}));
    this->interface_definitions = serialized_config.value("interface_definitions", json({}));
    this->types = serialized_config.value("types", json({}));
    this->module_names = serialized_config.at("module_names");
    this->module_config_cache = serialized_config.at("module_config_cache");
    if (serialized_config.contains("mappings") and !serialized_config.at("mappings").is_null()) {
        this->tier_mappings = serialized_config.at("mappings");
    }
    if (serialized_config.contains("telemetry_config") and !serialized_config.at("telemetry_config").is_null()) {
        this->telemetry_config = serialized_config.at("telemetry_config");
    }

    this->schemas = serialized_config.at("schemas");
    this->error_map = error::ErrorTypeMap();
    this->error_map.load_error_types_map(serialized_config.at("error_map"));
}

error::ErrorTypeMap Config::get_error_map() const {
    return this->error_map;
}

bool Config::module_provides(const std::string& module_name, const std::string& impl_id) {
    const auto& provides = this->module_config_cache.at(module_name).provides_impl;
    return (provides.find(impl_id) != provides.end());
}

json Config::get_module_cmds(const std::string& module_name, const std::string& impl_id) {
    return this->module_config_cache.at(module_name).cmds.at(impl_id);
}

RequirementInitialization Config::get_requirement_initialization(const std::string& module_id) const {
    BOOST_LOG_FUNCTION();

    RequirementInitialization res;

    for (const auto& [requirement, fulfillment] : this->resolve_requirements(module_id)) {
        const auto& mapping = this->get_3_tier_model_mapping(fulfillment.module_id, fulfillment.implementation_id);
        res[requirement.id].push_back({requirement, fulfillment, mapping});
    }

    return res;
}

ModuleConfigs Config::get_module_configs(const std::string& module_id) const {
    BOOST_LOG_FUNCTION();
    ModuleConfigs module_configs;

    // FIXME (aw): throw exception if module_id does not exist
    if (contains(module_id)) {
        const auto module_type = this->main.at(module_id).at("module").get<std::string>();
        const json config_maps = this->main.at(module_id).at("config_maps");
        const json manifest = this->manifests.at(module_type);

        for (const auto& conf_map : config_maps.items()) {
            const json config_schema = (conf_map.key() == "!module")
                                           ? manifest.at("config")
                                           : manifest.at("provides").at(conf_map.key()).at("config");
            ConfigMap processed_conf_map;
            for (const auto& entry : conf_map.value().items()) {
                const json entry_type = config_schema.at(entry.key()).at("type");
                ConfigEntry value;
                const json& data = entry.value();

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

// FIXME (aw): check if module_id does not exist
json Config::get_module_json_config(const std::string& module_id) {
    BOOST_LOG_FUNCTION();
    return this->main[module_id]["config_maps"];
}

ModuleInfo Config::get_module_info(const std::string& module_id) const {
    BOOST_LOG_FUNCTION();
    // FIXME (aw): the following if block is used so often, it should be
    //             refactored into a helper function
    if (!this->main.contains(module_id)) {
        EVTHROW(EverestApiError(fmt::format("Module id '{}' not found in config!", module_id)));
    }

    ModuleInfo module_info;
    module_info.id = module_id;
    module_info.name = this->main.at(module_id).at("module").get<std::string>();
    module_info.global_errors_enabled = this->manifests.at(module_info.name).at("enable_global_errors");
    const auto& module_metadata = this->manifests.at(module_info.name).at("metadata");
    for (auto& author : module_metadata.at("authors")) {
        module_info.authors.emplace_back(author.get<std::string>());
    }
    module_info.license = module_metadata.at("license").get<std::string>();

    return module_info;
}

std::optional<TelemetryConfig> Config::get_telemetry_config() {
    return this->telemetry_config;
}

json Config::get_interface_definition(const std::string& interface_name) const {
    BOOST_LOG_FUNCTION();
    return this->interface_definitions.value(interface_name, json());
}

void Config::ref_loader(const json_uri& uri, json& schema) {
    BOOST_LOG_FUNCTION();

    if (uri.location() == "http://json-schema.org/draft-07/schema") {
        schema = nlohmann::json_schema::draft7_schema_builtin;
        return;
    } else {
        const auto& path = uri.path();
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

json Config::load_all_manifests(const std::string& modules_dir, const std::string& schemas_dir) {
    BOOST_LOG_FUNCTION();

    json manifests = json({});

    auto schema_validation = load_schemas(schemas_dir);

    const fs::path modules_path = fs::path(modules_dir);

    for (auto&& module_path : fs::directory_iterator(modules_path)) {
        const fs::path manifest_path = module_path.path() / "manifest.yaml";
        if (!fs::exists(manifest_path)) {
            continue;
        }

        const std::string module_name = module_path.path().filename().string();
        EVLOG_debug << fmt::format("Found module {}, loading and verifying manifest...", module_name);

        try {
            manifests[module_name] = load_yaml(manifest_path);

            schema_validation.validators.manifest.validate(manifests.at(module_name));
        } catch (const std::exception& e) {
            EVLOG_AND_THROW(EverestConfigError(
                fmt::format("Failed to load and parse module manifest file of module {}: {}", module_name, e.what())));
        }
    }

    return manifests;
}

std::set<std::string> Config::keys(const json& object) {
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

    for (const auto& element : object.items()) {
        keys.insert(element.key());
    }

    return keys;
}

} // namespace Everest

NLOHMANN_JSON_NAMESPACE_BEGIN
void adl_serializer<Everest::Schemas>::to_json(nlohmann::json& j, const Everest::Schemas& s) {
    j = {{"config", s.config},
         {"manifest", s.manifest},
         {"interface", s.interface},
         {"type", s.type},
         {"error_declaration_list", s.error_declaration_list}};
}

void adl_serializer<Everest::Schemas>::from_json(const nlohmann::json& j, Everest::Schemas& s) {
    s.config = j.at("config");
    s.manifest = j.at("manifest");
    s.interface = j.at("interface");
    s.type = j.at("type");
    s.error_declaration_list = j.at("error_declaration_list");
}
NLOHMANN_JSON_NAMESPACE_END
