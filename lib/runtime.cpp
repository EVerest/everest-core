// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <framework/runtime.hpp>
#include <utils/error.hpp>
#include <utils/error_json.hpp>

#include <algorithm>
#include <cstdlib>

#include <boost/program_options.hpp>

namespace Everest {

namespace po = boost::program_options;

std::string parse_string_option(const po::variables_map& vm, const char* option) {
    if (vm.count(option) == 0) {
        return "";
    }

    return vm[option].as<std::string>();
}

static fs::path assert_dir(const std::string& path, const std::string& path_alias = "The") {
    auto fs_path = fs::path(path);

    if (!fs::exists(fs_path)) {
        throw BootException(fmt::format("{} path '{}' does not exist", path_alias, path));
    }

    fs_path = fs::canonical(fs_path);

    if (!fs::is_directory(fs_path)) {
        throw BootException(fmt::format("{} path '{}' is not a directory", path_alias, path));
    }

    return fs_path;
}

static fs::path assert_file(const std::string& path, const std::string& file_alias = "The") {
    auto fs_file = fs::path(path);

    if (!fs::exists(fs_file)) {
        throw BootException(fmt::format("{} file '{}' does not exist", file_alias, path));
    }

    fs_file = fs::canonical(fs_file);

    if (!fs::is_regular_file(fs_file)) {
        throw BootException(fmt::format("{} file '{}' is not a regular file", file_alias, path));
    }

    return fs_file;
}

static bool has_extension(const std::string& path, const std::string& ext) {
    auto path_ext = fs::path(path).stem().string();

    // lowercase the string
    std::transform(path_ext.begin(), path_ext.end(), path_ext.begin(), [](unsigned char c) { return std::tolower(c); });

    return path_ext == ext;
}

static std::string get_prefixed_path_from_json(const nlohmann::json& value, const fs::path& prefix) {
    auto settings_configs_dir = value.get<std::string>();
    if (fs::path(settings_configs_dir).is_relative()) {
        settings_configs_dir = (prefix / settings_configs_dir).string();
    }
    return settings_configs_dir;
}

void populate_module_info_path_from_runtime_settings(ModuleInfo& mi, std::shared_ptr<RuntimeSettings> rs) {
    mi.paths.etc = rs->etc_dir;
    mi.paths.libexec = rs->modules_dir / mi.name;
    mi.paths.share = rs->data_dir / defaults::MODULES_DIR / mi.name;
}

RuntimeSettings::RuntimeSettings(const std::string& prefix_, const std::string& config_) {
    // if prefix or config is empty, we assume they have not been set!
    // if they have been set, check their validity, otherwise bail out!

    if (config_.length() != 0) {
        try {
            config_file = assert_file(config_, "User profided config");
        } catch (const BootException& e) {
            if (has_extension(config_file, ".yaml")) {
                throw;
            }

            // otherwise, we propbably got a simple config file name
        }
    }

    if (prefix_.length() != 0) {
        // user provided
        prefix = assert_dir(prefix_, "User provided prefix");
    }

    if (config_file.empty()) {
        auto config_file_prefix = prefix;
        if (config_file_prefix.empty()) {
            config_file_prefix = assert_dir(defaults::PREFIX, "Default prefix");
        }

        if (config_file_prefix.string() == "/usr") {
            // we're going to look in /etc, which isn't prefixed by /usr
            config_file_prefix = "/";
        }

        if (config_.length() != 0) {
            // user provided short form

            const auto user_config_file =
                config_file_prefix / defaults::SYSCONF_DIR / defaults::NAMESPACE / fmt::format("{}.yaml", config_);

            const auto short_form_alias = fmt::format("User provided (by using short form: '{}')", config_);

            config_file = assert_file(user_config_file, short_form_alias);
        } else {
            // default
            config_file =
                assert_file(config_file_prefix / defaults::SYSCONF_DIR / defaults::NAMESPACE / defaults::CONFIG_NAME,
                            "Default config");
        }
    }

    // now the config file should have been found
    if (config_file.empty()) {
        throw std::runtime_error("Assertion for found config file failed");
    }

    config = load_yaml(config_file);
    if (config == nullptr) {
        EVLOG_info << "Config file is null, treating it as empty";
        config = json::object();
    } else if (!config.is_object()) {
        throw BootException(fmt::format("Config file '{}' is not an object", config_file.string()));
    }

    const auto settings = config.value("settings", json::object());

    if (prefix.empty()) {
        const auto settings_prefix_it = settings.find("prefix");
        if (settings_prefix_it != settings.end()) {
            const auto settings_prefix = settings_prefix_it->get<std::string>();
            if (!fs::path(settings_prefix).is_absolute()) {
                throw BootException("Setting a non-absolute directory for the prefix is not allowed");
            }

            prefix = assert_dir(settings_prefix, "Config provided prefix");
        } else {
            prefix = assert_dir(defaults::PREFIX, "Default prefix");
        }
    }

    {
        // etc directory
        const auto default_etc_dir = fs::path(defaults::SYSCONF_DIR) / defaults::NAMESPACE;
        if (prefix.string() != "/usr") {
            etc_dir = prefix / default_etc_dir;
        } else {
            etc_dir = fs::path("/") / default_etc_dir;
        }
        etc_dir = assert_dir(etc_dir.string(), "Default etc directory");
    }

    {
        // share directory
        data_dir =
            assert_dir((prefix / defaults::DATAROOT_DIR / defaults::NAMESPACE).string(), "Default share directory");
    }

    const auto settings_configs_dir_it = settings.find("configs_dir");
    if (settings_configs_dir_it != settings.end()) {
        auto settings_configs_dir = get_prefixed_path_from_json(*settings_configs_dir_it, prefix);
        configs_dir = assert_dir(settings_configs_dir, "Config provided configs directory");
    } else {
        configs_dir = assert_dir(etc_dir.string(), "Default configs directory");
    }

    const auto settings_schemas_dir_it = settings.find("schemas_dir");
    if (settings_schemas_dir_it != settings.end()) {
        const auto settings_schemas_dir = get_prefixed_path_from_json(*settings_schemas_dir_it, prefix);
        schemas_dir = assert_dir(settings_schemas_dir, "Config provided schema directory");
    } else {
        const auto default_schemas_dir = data_dir / defaults::SCHEMAS_DIR;
        schemas_dir = assert_dir(default_schemas_dir.string(), "Default schema directory");
    }

    const auto settings_interfaces_dir_it = settings.find("interfaces_dir");
    if (settings_interfaces_dir_it != settings.end()) {
        const auto settings_interfaces_dir = get_prefixed_path_from_json(*settings_interfaces_dir_it, prefix);
        interfaces_dir = assert_dir(settings_interfaces_dir, "Config provided interface directory");
    } else {
        const auto default_interfaces_dir = data_dir / defaults::INTERFACES_DIR;
        interfaces_dir = assert_dir(default_interfaces_dir, "Default interface directory");
    }

    const auto settings_modules_dir_it = settings.find("modules_dir");
    if (settings_modules_dir_it != settings.end()) {
        const auto settings_modules_dir = get_prefixed_path_from_json(*settings_modules_dir_it, prefix);
        modules_dir = assert_dir(settings_modules_dir, "Config provided module directory");
    } else {
        const auto default_modules_dir = prefix / defaults::LIBEXEC_DIR / defaults::NAMESPACE / defaults::MODULES_DIR;
        modules_dir = assert_dir(default_modules_dir, "Default module directory");
    }

    const auto settings_types_dir_it = settings.find("types_dir");
    if (settings_types_dir_it != settings.end()) {
        const auto settings_types_dir = get_prefixed_path_from_json(*settings_types_dir_it, prefix);
        types_dir = assert_dir(settings_types_dir, "Config provided type directory");
    } else {
        const auto default_types_dir = data_dir / defaults::TYPES_DIR;
        types_dir = assert_dir(default_types_dir, "Default type directory");
    }

    const auto settings_errors_dir_it = settings.find("errors_dir");
    if (settings_errors_dir_it != settings.end()) {
        const auto settings_errors_dir = get_prefixed_path_from_json(*settings_errors_dir_it, prefix);
        errors_dir = assert_dir(settings_errors_dir, "Config provided error directory");
    } else {
        const auto default_errors_dir = data_dir / defaults::ERRORS_DIR;
        errors_dir = assert_dir(default_errors_dir, "Default error directory");
    }

    const auto settings_www_dir_it = settings.find("www_dir");
    if (settings_www_dir_it != settings.end()) {
        const auto settings_www_dir = get_prefixed_path_from_json(*settings_www_dir_it, prefix);
        www_dir = assert_dir(settings_www_dir, "Config provided www directory");
    } else {
        const auto default_www_dir = data_dir / defaults::WWW_DIR;
        www_dir = assert_dir(default_www_dir, "Default www directory");
    }

    const auto settings_logging_config_file_it = settings.find("logging_config_file");
    if (settings_logging_config_file_it != settings.end()) {
        const auto settings_logging_config_file = get_prefixed_path_from_json(*settings_logging_config_file_it, prefix);
        logging_config_file = assert_file(settings_logging_config_file, "Config provided logging config");
    } else {
        auto default_logging_config_file =
            fs::path(defaults::SYSCONF_DIR) / defaults::NAMESPACE / defaults::LOGGING_CONFIG_NAME;
        if (prefix.string() != "/usr") {
            default_logging_config_file = prefix / default_logging_config_file;
        } else {
            default_logging_config_file = fs::path("/") / default_logging_config_file;
        }

        logging_config_file = assert_file(default_logging_config_file, "Default logging config");
    }

    const auto settings_controller_port_it = settings.find("controller_port");
    if (settings_controller_port_it != settings.end()) {
        controller_port = settings_controller_port_it->get<int>();
    } else {
        controller_port = defaults::CONTROLLER_PORT;
    }

    const auto settings_controller_rpc_timeout_ms_it = settings.find("controller_rpc_timeout_ms");
    if (settings_controller_rpc_timeout_ms_it != settings.end()) {
        controller_rpc_timeout_ms = settings_controller_rpc_timeout_ms_it->get<int>();
    } else {
        controller_rpc_timeout_ms = defaults::CONTROLLER_RPC_TIMEOUT_MS;
    }

    const auto settings_mqtt_broker_host_it = settings.find("mqtt_broker_host");
    if (settings_mqtt_broker_host_it != settings.end()) {
        mqtt_broker_host = settings_mqtt_broker_host_it->get<std::string>();
    } else {
        mqtt_broker_host = defaults::MQTT_BROKER_HOST;
    }

    // overwrite mqtt broker host with environment variable
    // NOLINTNEXTLINE(concurrency-mt-unsafe): not problematic that this function is not threadsafe here
    const char* mqtt_server_address = std::getenv("MQTT_SERVER_ADDRESS");
    if (mqtt_server_address != nullptr) {
        mqtt_broker_host = mqtt_server_address;
    }

    const auto settings_mqtt_broker_port_it = settings.find("mqtt_broker_port");
    if (settings_mqtt_broker_port_it != settings.end()) {
        mqtt_broker_port = settings_mqtt_broker_port_it->get<int>();
    } else {
        mqtt_broker_port = defaults::MQTT_BROKER_PORT;
    }

    // overwrite mqtt broker port with environment variable
    // NOLINTNEXTLINE(concurrency-mt-unsafe): not problematic that this function is not threadsafe here
    const char* mqtt_server_port = std::getenv("MQTT_SERVER_PORT");
    if (mqtt_server_port != nullptr) {
        mqtt_broker_port = std::stoi(mqtt_server_port);
    }

    const auto settings_mqtt_everest_prefix_it = settings.find("mqtt_everest_prefix");
    if (settings_mqtt_everest_prefix_it != settings.end()) {
        mqtt_everest_prefix = settings_mqtt_everest_prefix_it->get<std::string>();
    } else {
        mqtt_everest_prefix = defaults::MQTT_EVEREST_PREFIX;
    }

    // always make sure the everest mqtt prefix ends with '/'
    if (mqtt_everest_prefix.length() > 0 && mqtt_everest_prefix.back() != '/') {
        mqtt_everest_prefix = mqtt_everest_prefix += "/";
    }

    const auto settings_mqtt_external_prefix_it = settings.find("mqtt_external_prefix");
    if (settings_mqtt_external_prefix_it != settings.end()) {
        mqtt_external_prefix = settings_mqtt_external_prefix_it->get<std::string>();
    } else {
        mqtt_external_prefix = defaults::MQTT_EXTERNAL_PREFIX;
    }

    if (mqtt_everest_prefix == mqtt_external_prefix) {
        throw BootException(fmt::format("mqtt_everest_prefix '{}' cannot be equal to mqtt_external_prefix '{}'!",
                                        mqtt_everest_prefix, mqtt_external_prefix));
    }

    const auto settings_telemetry_prefix_it = settings.find("telemetry_prefix");
    if (settings_telemetry_prefix_it != settings.end()) {
        telemetry_prefix = settings_telemetry_prefix_it->get<std::string>();
    } else {
        telemetry_prefix = defaults::TELEMETRY_PREFIX;
    }

    // always make sure the telemetry mqtt prefix ends with '/'
    if (telemetry_prefix.length() > 0 && telemetry_prefix.back() != '/') {
        telemetry_prefix = telemetry_prefix += "/";
    }

    const auto settings_telemetry_enabled_it = settings.find("telemetry_enabled");
    if (settings_telemetry_enabled_it != settings.end()) {
        telemetry_enabled = settings_telemetry_enabled_it->get<bool>();
    } else {
        telemetry_enabled = defaults::TELEMETRY_ENABLED;
    }

    const auto settings_validate_schema_it = settings.find("validate_schema");
    if (settings_validate_schema_it != settings.end()) {
        validate_schema = settings_validate_schema_it->get<bool>();
    } else {
        validate_schema = defaults::VALIDATE_SCHEMA;
    }
}

ModuleCallbacks::ModuleCallbacks(const std::function<void(ModuleAdapter module_adapter)>& register_module_adapter,
                                 const std::function<std::vector<cmd>(const json& connections)>& everest_register,
                                 const std::function<void(ModuleConfigs module_configs, const ModuleInfo& info)>& init,
                                 const std::function<void()>& ready) :
    register_module_adapter(register_module_adapter), everest_register(everest_register), init(init), ready(ready) {
}

ModuleLoader::ModuleLoader(int argc, char* argv[], ModuleCallbacks callbacks) :
    runtime_settings(nullptr), callbacks(callbacks) {
    if (!this->parse_command_line(argc, argv)) {
        return;
    }
}

int ModuleLoader::initialize() {
    auto start_time = std::chrono::system_clock::now();
    if (!this->runtime_settings) {
        return 0;
    }

    auto& rs = this->runtime_settings;
    Logging::init(rs->logging_config_file.string(), this->module_id);
    try {
        Config config = Config(rs);

        if (!config.contains(this->module_id)) {
            EVLOG_error << fmt::format("Module id '{}' not found in config!", this->module_id);
            return 2;
        }

        const std::string module_identifier = config.printable_identifier(this->module_id);
        EVLOG_debug << fmt::format("Initializing framework for module {}...", module_identifier);
        EVLOG_verbose << fmt::format("Setting process name to: '{}'...", module_identifier);
        int prctl_return = prctl(PR_SET_NAME, module_identifier.c_str());
        if (prctl_return == 1) {
            EVLOG_warning << fmt::format("Could not set process name to '{}', it remains '{}'", module_identifier,
                                         this->original_process_name);
        }
        Logging::update_process_name(module_identifier);

        auto everest =
            Everest(this->module_id, config, rs->validate_schema, rs->mqtt_broker_host, rs->mqtt_broker_port,
                    rs->mqtt_everest_prefix, rs->mqtt_external_prefix, rs->telemetry_prefix, rs->telemetry_enabled);

        // module import
        EVLOG_debug << fmt::format("Initializing module {}...", module_identifier);

        if (!everest.connect()) {
            EVLOG_error << fmt::format("Cannot connect to MQTT broker at {}:{}", rs->mqtt_broker_host,
                                       rs->mqtt_broker_port);
            return 1;
        }

        ModuleAdapter module_adapter;

        module_adapter.call = [&everest](const Requirement& req, const std::string& cmd_name, Parameters args) {
            return everest.call_cmd(req, cmd_name, args);
        };

        module_adapter.publish = [&everest](const std::string& param1, const std::string& param2, Value param3) {
            return everest.publish_var(param1, param2, param3);
        };

        module_adapter.subscribe = [&everest](const Requirement& req, const std::string& var_name,
                                              const ValueCallback& callback) {
            return everest.subscribe_var(req, var_name, callback);
        };

        module_adapter.subscribe_error = [&everest](const Requirement& req, const std::string& error_type,
                                                    const error::ErrorCallback& error_callback) {
            JsonCallback json_callback = [error_callback](json j) { error_callback(error::json_to_error(j)); };
            return everest.subscribe_error(req, error_type, json_callback);
        };

        module_adapter.subscribe_error_cleared = [&everest](const Requirement& req, const std::string& error_type,
                                                            const error::ErrorCallback& error_callback) {
            JsonCallback json_callback = [error_callback](json j) { return error_callback(error::json_to_error(j)); };
            return everest.subscribe_error_cleared(req, error_type, json_callback);
        };

        module_adapter.raise_error = [&everest](const std::string& impl_id, const std::string& type,
                                                const std::string& message, const error::Severity& severity) {
            return error::ErrorHandle(everest.raise_error(impl_id, type, message, error::severity_to_string(severity)));
        };

        module_adapter.request_clear_all_errors_of_module = [&everest](const std::string& impl_id) {
            return everest.request_clear_error(error::RequestClearErrorOption::ClearAllOfModule, impl_id, "", "");
        };

        module_adapter.request_clear_all_errors_of_type_of_module = [&everest](const std::string& impl_id,
                                                                               const std::string& error_type) {
            return everest.request_clear_error(error::RequestClearErrorOption::ClearAllOfTypeOfModule, impl_id, "",
                                               error_type);
        };

        module_adapter.request_clear_error_uuid = [&everest](const std::string& impl_id,
                                                             const error::ErrorHandle& handle) {
            return everest.request_clear_error(error::RequestClearErrorOption::ClearUUID, impl_id, handle.uuid, "");
        };

        // NOLINTNEXTLINE(modernize-avoid-bind): prefer bind here for readability
        module_adapter.ext_mqtt_publish =
            std::bind(&Everest::Everest::external_mqtt_publish, &everest, std::placeholders::_1, std::placeholders::_2);

        // NOLINTNEXTLINE(modernize-avoid-bind): prefer bind here for readability
        module_adapter.ext_mqtt_subscribe = std::bind(&Everest::Everest::provide_external_mqtt_handler, &everest,
                                                      std::placeholders::_1, std::placeholders::_2);

        module_adapter.telemetry_publish = [&everest](const std::string& category, const std::string& subcategory,
                                                      const std::string& type, const TelemetryMap& telemetry) {
            return everest.telemetry_publish(category, subcategory, type, telemetry);
        };

        this->callbacks.register_module_adapter(module_adapter);

        // FIXME (aw): would be nice to move this config related thing toward the module_init function
        std::vector<cmd> cmds =
            this->callbacks.everest_register(config.get_main_config()[this->module_id]["connections"]);

        for (auto const& command : cmds) {
            everest.provide_cmd(command);
        }

        auto module_configs = config.get_module_configs(this->module_id);
        auto module_info = config.get_module_info(this->module_id);
        populate_module_info_path_from_runtime_settings(module_info, rs);
        module_info.telemetry_enabled = everest.is_telemetry_enabled();

        this->callbacks.init(module_configs, module_info);

        everest.spawn_main_loop_thread();

        // register the modules ready handler with the framework
        // this handler gets called when the global ready signal is received
        everest.register_on_ready_handler(this->callbacks.ready);

        // the module should now be ready
        everest.signal_ready();

        auto end_time = std::chrono::system_clock::now();
        EVLOG_info << "Module " << fmt::format(TERMINAL_STYLE_BLUE, "{}", module_id) << " initialized ["
                   << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms]";

        everest.wait_for_main_loop_end();

        EVLOG_info << "Exiting...";
    } catch (boost::exception& e) {
        EVLOG_critical << fmt::format("Caught top level boost::exception:\n{}", boost::diagnostic_information(e, true));
    } catch (std::exception& e) {
        EVLOG_critical << fmt::format("Caught top level std::exception:\n{}", boost::diagnostic_information(e, true));
    }

    return 0;
}

bool ModuleLoader::parse_command_line(int argc, char* argv[]) {
    po::options_description desc("EVerest");
    desc.add_options()("help,h", "produce help message");
    desc.add_options()("prefix", po::value<std::string>(), "Set main EVerest directory");
    desc.add_options()("module,m", po::value<std::string>(),
                       "Which module should be executed (module id from config file)");
    desc.add_options()("dontvalidateschema", "Don't validate json schema on every message");
    desc.add_options()("config", po::value<std::string>(), "The path to a custom config.json");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") != 0) {
        std::cout << desc << "\n";
        return false;
    }

    const auto prefix_opt = parse_string_option(vm, "prefix");
    const auto config_opt = parse_string_option(vm, "config");
    this->runtime_settings = std::make_unique<RuntimeSettings>(prefix_opt, config_opt);
    this->original_process_name = argv[0];

    if (vm.count("module") != 0) {
        this->module_id = vm["module"].as<std::string>();
    } else {
        EVTHROW(EVEXCEPTION(EverestApiError, "--module parameter is required"));
    }

    return true;
}

} // namespace Everest
