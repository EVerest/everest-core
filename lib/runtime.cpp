// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <framework/runtime.hpp>
#include <utils/error.hpp>
#include <utils/error/error_factory.hpp>
#include <utils/error/error_json.hpp>
#include <utils/error/error_manager_impl.hpp>
#include <utils/error/error_manager_req.hpp>
#include <utils/error/error_state_monitor.hpp>
#include <utils/filesystem.hpp>

#include <algorithm>
#include <cstdlib>
#include <fstream>

#include <boost/program_options.hpp>

namespace Everest {

namespace po = boost::program_options;

std::string parse_string_option(const po::variables_map& vm, const char* option) {
    if (vm.count(option) == 0) {
        return "";
    }

    return vm[option].as<std::string>();
}

void populate_module_info_path_from_runtime_settings(ModuleInfo& mi, const RuntimeSettings& rs) {
    mi.paths.etc = rs.etc_dir;
    mi.paths.libexec = rs.modules_dir / mi.name;
    mi.paths.share = rs.data_dir / defaults::MODULES_DIR / mi.name;
}

ManagerSettings::ManagerSettings(const std::string& prefix_, const std::string& config_) {
    // if prefix or config is empty, we assume they have not been set!
    // if they have been set, check their validity, otherwise bail out!

    this->boot_source = ConfigBootSource::YamlFile;

    init_prefix_and_data_dir(prefix_);
    init_config_file(config_);
    const auto settings = everest::config::parse_settings(config.value("settings", json::object()));
    if (settings.prefix.has_value()) {
        EVLOG_warning << "Setting the prefix in the config file is deprecated. Please use the --prefix command line "
                         "option instead.";
    }
    init_settings(settings);
}

ManagerSettings::ManagerSettings(const std::string& prefix_, const std::string& db_, DatabaseTag) {
    this->boot_source = ConfigBootSource::Database;
    init_prefix_and_data_dir(prefix_);
    throw BootException("Database boot source is not supported in this version of EVerest");
}

ManagerSettings::ManagerSettings(const std::string& prefix_, const std::string& config_, const std::string& db_) {
    this->boot_source = ConfigBootSource::DatabaseFallbackYaml;
    init_prefix_and_data_dir(prefix_);
    throw BootException("Database fallback YAML boot source is not supported in this version of EVerest");
}

void ManagerSettings::init_settings(const everest::config::Settings& settings) {
    if (this->runtime_settings.prefix.empty()) {
        throw std::runtime_error(
            "Prefix must be set before initializing the settings. Please call init_prefix_and_data_dir() first.");
    }

    const auto prefix = this->runtime_settings.prefix;
    fs::path etc_dir;
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

    fs::path data_dir;
    {
        // share directory
        data_dir =
            assert_dir((prefix / defaults::DATAROOT_DIR / defaults::NAMESPACE).string(), "Default share directory");
    }

    if (settings.configs_dir.has_value()) {
        auto settings_configs_dir = get_prefixed_path_from_json(settings.configs_dir.value().string(), prefix);
        configs_dir = assert_dir(settings_configs_dir, "Config provided configs directory");
    } else {
        configs_dir = assert_dir(etc_dir.string(), "Default configs directory");
    }

    if (settings.schemas_dir.has_value()) {
        const auto settings_schemas_dir = get_prefixed_path_from_json(settings.schemas_dir.value().string(), prefix);
        schemas_dir = assert_dir(settings_schemas_dir, "Config provided schema directory");
    } else {
        const auto default_schemas_dir = data_dir / defaults::SCHEMAS_DIR;
        schemas_dir = assert_dir(default_schemas_dir.string(), "Default schema directory");
    }

    if (settings.interfaces_dir.has_value()) {
        const auto settings_interfaces_dir =
            get_prefixed_path_from_json(settings.interfaces_dir.value().string(), prefix);
        interfaces_dir = assert_dir(settings_interfaces_dir, "Config provided interface directory");
    } else {
        const auto default_interfaces_dir = data_dir / defaults::INTERFACES_DIR;
        interfaces_dir = assert_dir(default_interfaces_dir, "Default interface directory");
    }

    fs::path modules_dir;
    if (settings.modules_dir.has_value()) {
        const auto settings_modules_dir = get_prefixed_path_from_json(settings.modules_dir.value().string(), prefix);
        modules_dir = assert_dir(settings_modules_dir, "Config provided module directory");
    } else {
        const auto default_modules_dir = prefix / defaults::LIBEXEC_DIR / defaults::NAMESPACE / defaults::MODULES_DIR;
        modules_dir = assert_dir(default_modules_dir, "Default module directory");
    }

    if (settings.types_dir.has_value()) {
        const auto settings_types_dir = get_prefixed_path_from_json(settings.types_dir.value().string(), prefix);
        types_dir = assert_dir(settings_types_dir, "Config provided type directory");
    } else {
        const auto default_types_dir = data_dir / defaults::TYPES_DIR;
        types_dir = assert_dir(default_types_dir, "Default type directory");
    }

    if (settings.errors_dir.has_value()) {
        const auto settings_errors_dir = get_prefixed_path_from_json(settings.errors_dir.value().string(), prefix);
        errors_dir = assert_dir(settings_errors_dir, "Config provided error directory");
    } else {
        const auto default_errors_dir = data_dir / defaults::ERRORS_DIR;
        errors_dir = assert_dir(default_errors_dir, "Default error directory");
    }

    if (settings.www_dir.has_value()) {
        const auto settings_www_dir = get_prefixed_path_from_json(settings.www_dir.value().string(), prefix);
        www_dir = assert_dir(settings_www_dir, "Config provided www directory");
    } else {
        const auto default_www_dir = data_dir / defaults::WWW_DIR;
        www_dir = assert_dir(default_www_dir, "Default www directory");
    }

    fs::path logging_config_file;
    if (settings.logging_config_file.has_value()) {
        const auto settings_logging_config_file =
            get_prefixed_path_from_json(settings.logging_config_file.value().string(), prefix);
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

    if (settings.controller_port.has_value()) {
        controller_port = settings.controller_port.value();
    } else {
        controller_port = defaults::CONTROLLER_PORT;
    }

    if (settings.controller_rpc_timeout_ms.has_value()) {
        controller_rpc_timeout_ms = settings.controller_rpc_timeout_ms.value();
    } else {
        controller_rpc_timeout_ms = defaults::CONTROLLER_RPC_TIMEOUT_MS;
    }

    std::string mqtt_broker_socket_path;
    std::string mqtt_broker_host;
    int mqtt_broker_port = 0;
    std::string mqtt_everest_prefix;
    std::string mqtt_external_prefix;

    // Unix Domain Socket configuration MUST be set in the configuration,
    // doesn't have a default value if not provided thus it takes precedence
    // over default values - this is to have backward compatiblity in term of configuration
    // in case both UDS (Unix Domain Sockets) and IDS (Internet Domain Sockets) are set in config, raise exception
    if (settings.mqtt_broker_socket_path.has_value()) {
        mqtt_broker_socket_path = settings.mqtt_broker_socket_path.value();
    }

    if (settings.mqtt_broker_host.has_value()) {
        mqtt_broker_host = settings.mqtt_broker_host.value();
        if (!mqtt_broker_socket_path.empty()) {
            // invalid configuration, can't have both UDS and IDS
            throw BootException(
                fmt::format("Setting both the Unix Domain Socket {} and Internet Domain Socket {} in config is invalid",
                            mqtt_broker_socket_path, mqtt_broker_host));
        }
    } else {
        mqtt_broker_host = defaults::MQTT_BROKER_HOST;
    }

    // overwrite mqtt broker host with environment variable
    // NOLINTNEXTLINE(concurrency-mt-unsafe): not problematic that this function is not threadsafe here
    const char* mqtt_server_address = std::getenv("MQTT_SERVER_ADDRESS");
    if (mqtt_server_address != nullptr) {
        mqtt_broker_host = mqtt_server_address;
        if (!mqtt_broker_socket_path.empty()) {
            // invalid configuration, can't have both UDS and IDS
            throw BootException(
                fmt::format("Setting both the Unix Domain Socket {} and Internet Domain Socket {} in "
                            "config and as environment variable respectivelly (as MQTT_SERVER_ADDRESS) is not allowed",
                            mqtt_broker_socket_path, mqtt_broker_host));
        }
    }

    if (settings.mqtt_broker_port.has_value()) {
        mqtt_broker_port = settings.mqtt_broker_port.value();
    } else {
        mqtt_broker_port = defaults::MQTT_BROKER_PORT;
    }

    // overwrite mqtt broker port with environment variable
    // NOLINTNEXTLINE(concurrency-mt-unsafe): not problematic that this function is not threadsafe here
    const char* mqtt_server_port = std::getenv("MQTT_SERVER_PORT");

    if (mqtt_server_port != nullptr) {
        try {
            mqtt_broker_port = std::stoi(mqtt_server_port);
        } catch (...) {
            EVLOG_warning << "Environment variable MQTT_SERVER_PORT set, but not set to an integer. Ignoring.";
        }
    }

    if (settings.mqtt_everest_prefix.has_value()) {
        mqtt_everest_prefix = settings.mqtt_everest_prefix.value();
    } else {
        mqtt_everest_prefix = defaults::MQTT_EVEREST_PREFIX;
    }

    // always make sure the everest mqtt prefix ends with '/'
    if (mqtt_everest_prefix.length() > 0 && mqtt_everest_prefix.back() != '/') {
        mqtt_everest_prefix = mqtt_everest_prefix += "/";
    }

    if (settings.mqtt_external_prefix.has_value()) {
        mqtt_external_prefix = settings.mqtt_external_prefix.value();
    } else {
        mqtt_external_prefix = defaults::MQTT_EXTERNAL_PREFIX;
    }

    if (mqtt_everest_prefix == mqtt_external_prefix) {
        throw BootException(fmt::format("mqtt_everest_prefix '{}' cannot be equal to mqtt_external_prefix '{}'!",
                                        mqtt_everest_prefix, mqtt_external_prefix));
    }

    if (not mqtt_broker_socket_path.empty()) {
        populate_mqtt_settings(this->mqtt_settings, mqtt_broker_socket_path, mqtt_everest_prefix, mqtt_external_prefix);
    } else {
        populate_mqtt_settings(this->mqtt_settings, mqtt_broker_host, mqtt_broker_port, mqtt_everest_prefix,
                               mqtt_external_prefix);
    }

    run_as_user = settings.run_as_user.value_or("");

    auto version_information_path = data_dir / VERSION_INFORMATION_FILE;
    if (fs::exists(version_information_path)) {
        std::ifstream ifs(version_information_path.string());
        version_information = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    } else {
        version_information = "unknown";
    }

    std::string telemetry_prefix;
    if (settings.telemetry_prefix.has_value()) {
        telemetry_prefix = settings.telemetry_prefix.value();
    } else {
        telemetry_prefix = defaults::TELEMETRY_PREFIX;
    }

    // always make sure the telemetry mqtt prefix ends with '/'
    if (telemetry_prefix.length() > 0 && telemetry_prefix.back() != '/') {
        telemetry_prefix = telemetry_prefix += "/";
    }

    bool telemetry_enabled = defaults::TELEMETRY_ENABLED;
    if (settings.telemetry_enabled.has_value()) {
        telemetry_enabled = settings.telemetry_enabled.value();
    } else {
        telemetry_enabled = defaults::TELEMETRY_ENABLED;
    }

    bool validate_schema = defaults::VALIDATE_SCHEMA;
    if (settings.validate_schema.has_value()) {
        validate_schema = settings.validate_schema.value();
    } else {
        validate_schema = defaults::VALIDATE_SCHEMA;
    }

    populate_runtime_settings(this->runtime_settings, prefix, etc_dir, data_dir, modules_dir, logging_config_file,
                              telemetry_prefix, telemetry_enabled, validate_schema);
}

void ManagerSettings::init_prefix_and_data_dir(const std::string& prefix_) {
    fs::path prefix;
    if (prefix_.length() != 0) {
        // user provided
        prefix = assert_dir(prefix_, "User provided prefix");
    }
    if (prefix.empty()) {
        prefix = assert_dir(defaults::PREFIX, "Default prefix");
    }
    runtime_settings.data_dir =
        assert_dir((prefix / defaults::DATAROOT_DIR / defaults::NAMESPACE).string(), "Default share directory");
    runtime_settings.prefix = prefix;
}

void ManagerSettings::init_config_file(const std::string& config_) {
    if (this->runtime_settings.prefix.empty()) {
        throw std::runtime_error(
            "Prefix must be set before initializing the config file. Please call init_prefix_and_data_dir() first.");
    }

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

    if (config_file.empty()) {
        auto config_file_prefix = this->runtime_settings.prefix;
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
}

ModuleCallbacks::ModuleCallbacks(
    const std::function<void(ModuleAdapter module_adapter)>& register_module_adapter,
    const std::function<std::vector<cmd>(const RequirementInitialization& requirement_init)>& everest_register,
    const std::function<void(ModuleConfigs module_configs, const ModuleInfo& info)>& init,
    const std::function<void()>& ready) :
    register_module_adapter(register_module_adapter), everest_register(everest_register), init(init), ready(ready) {
}

ModuleLoader::ModuleLoader(int argc, char* argv[], ModuleCallbacks callbacks, VersionInformation version_information) :
    runtime_settings(nullptr), callbacks(std::move(callbacks)), version_information(std::move(version_information)) {
    try {
        if (!this->parse_command_line(argc, argv)) {
            this->should_exit = true;
        }
    } catch (std::exception& e) {
        std::cout << "Error during command line parsing: " << e.what() << "\n";
        this->should_exit = true;
    }
}

int ModuleLoader::initialize() {
    if (this->should_exit) {
        return EXIT_FAILURE;
    }
    Logging::init(this->logging_config_file.string(), this->module_id);

    const auto start_time = std::chrono::system_clock::now();

    this->mqtt = std::make_shared<MQTTAbstraction>(this->mqtt_settings);
    this->mqtt->connect();
    this->mqtt->spawn_main_loop_thread();

    const auto result = get_module_config(this->mqtt, this->module_id);
    const auto get_config_time = std::chrono::system_clock::now();
    EVLOG_debug << "Module " << fmt::format(TERMINAL_STYLE_OK, "{}", module_id) << " get_config() ["
                << std::chrono::duration_cast<std::chrono::milliseconds>(get_config_time - start_time).count() << "ms]";

    RuntimeSettings result_settings = result.at("settings");
    this->runtime_settings = std::make_unique<RuntimeSettings>(std::move(result_settings));

    if (!this->runtime_settings) {
        return 0;
    }

    const auto& rs = this->runtime_settings;
    try {
        const auto config = Config(this->mqtt_settings, result);
        const auto config_instantiation_time = std::chrono::system_clock::now();
        EVLOG_debug
            << "Module " << fmt::format(TERMINAL_STYLE_OK, "{}", module_id) << " after Config() instantiation ["
            << std::chrono::duration_cast<std::chrono::milliseconds>(config_instantiation_time - start_time).count()
            << "ms]";

        if (!config.contains(this->module_id)) {
            EVLOG_error << fmt::format("Module id '{}' not found in config!", this->module_id);
            return 2;
        }

        const std::string module_identifier = config.printable_identifier(this->module_id);
        const auto module_name = config.get_module_name(this->module_id);
        if ((this->application_name != module_name) and (this->application_name != module_identifier)) {
            EVLOG_error << fmt::format(
                "Module id '{}': Expected a '{}' module, but it looks like you started a '{}' module.", this->module_id,
                module_name, this->application_name);
        }
        EVLOG_debug << fmt::format("Initializing framework for module {}...", module_identifier);
        EVLOG_verbose << fmt::format("Setting process name to: '{}'...", module_identifier);
        const int prctl_return = prctl(PR_SET_NAME, module_identifier.c_str());
        if (prctl_return == 1) {
            EVLOG_warning << fmt::format("Could not set process name to '{}', it remains '{}'", module_identifier,
                                         this->original_process_name);
        }
        Logging::update_process_name(module_identifier);

        auto everest = Everest(this->module_id, config, rs->validate_schema, this->mqtt, rs->telemetry_prefix,
                               rs->telemetry_enabled);

        // module import
        EVLOG_debug << fmt::format("Initializing module {}...", module_identifier);

        if (!everest.connect()) {
            if (this->mqtt_settings.broker_socket_path.empty()) {
                EVLOG_error << fmt::format("Cannot connect to MQTT broker at {}:{}", this->mqtt_settings.broker_host,
                                           this->mqtt_settings.broker_port);
            } else {
                EVLOG_error << fmt::format("Cannot connect to MQTT broker socket at {}",
                                           this->mqtt_settings.broker_socket_path);
            }
            return 1;
        }

        ModuleAdapter module_adapter;

        module_adapter.call = [&everest](const Requirement& req, const std::string& cmd_name, Parameters args) {
            return everest.call_cmd(req, cmd_name, std::move(args));
        };

        module_adapter.publish = [&everest](const std::string& param1, const std::string& param2, Value param3) {
            return everest.publish_var(param1, param2, std::move(param3));
        };

        module_adapter.subscribe = [&everest](const Requirement& req, const std::string& var_name,
                                              const ValueCallback& callback) {
            return everest.subscribe_var(req, var_name, callback);
        };

        module_adapter.get_error_manager_impl = [&everest](const std::string& impl_id) {
            return everest.get_error_manager_impl(impl_id);
        };

        module_adapter.get_error_state_monitor_impl = [&everest](const std::string& impl_id) {
            return everest.get_error_state_monitor_impl(impl_id);
        };

        module_adapter.get_error_factory = [&everest](const std::string& impl_id) {
            return everest.get_error_factory(impl_id);
        };

        module_adapter.get_error_manager_req = [&everest](const Requirement& req) {
            return everest.get_error_manager_req(req);
        };

        module_adapter.get_error_state_monitor_req = [&everest](const Requirement& req) {
            return everest.get_error_state_monitor_req(req);
        };

        module_adapter.get_global_error_manager = [&everest]() { return everest.get_global_error_manager(); };

        module_adapter.get_global_error_state_monitor = [&everest]() {
            return everest.get_global_error_state_monitor();
        };

        // NOLINTNEXTLINE(modernize-avoid-bind): prefer bind here for readability
        module_adapter.ext_mqtt_publish =
            std::bind(&Everest::Everest::external_mqtt_publish, &everest, std::placeholders::_1, std::placeholders::_2);

        module_adapter.ext_mqtt_subscribe = [&everest](const std::string& topic, const StringHandler& handler) {
            return everest.provide_external_mqtt_handler(topic, handler);
        };

        module_adapter.ext_mqtt_subscribe_pair = [&everest](const std::string& topic,
                                                            const StringPairHandler& handler) {
            return everest.provide_external_mqtt_handler(topic, handler);
        };

        module_adapter.telemetry_publish = [&everest](const std::string& category, const std::string& subcategory,
                                                      const std::string& type, const TelemetryMap& telemetry) {
            return everest.telemetry_publish(category, subcategory, type, telemetry);
        };

        module_adapter.get_mapping = [&everest]() { return everest.get_3_tier_model_mapping(); };

        this->callbacks.register_module_adapter(module_adapter);

        // FIXME (aw): would be nice to move this config related thing toward the module_init function
        const std::vector<cmd> cmds =
            this->callbacks.everest_register(config.get_requirement_initialization(this->module_id));

        for (const auto& command : cmds) {
            everest.provide_cmd(command);
        }

        const auto module_configs = config.get_module_configs(this->module_id);
        auto module_info = config.get_module_info(this->module_id);
        populate_module_info_path_from_runtime_settings(module_info, *rs);
        module_info.telemetry_enabled = everest.is_telemetry_enabled();
        const auto module_mappings = everest.get_3_tier_model_mapping();
        if (module_mappings.has_value()) {
            module_info.mapping = module_mappings.value().module;
        }

        this->callbacks.init(module_configs, module_info);

        everest.spawn_main_loop_thread();

        // register the modules ready handler with the framework
        // this handler gets called when the global ready signal is received
        everest.register_on_ready_handler(this->callbacks.ready);

        // the module should now be ready
        everest.signal_ready();

        const auto end_time = std::chrono::system_clock::now();
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
    desc.add_options()("version", "Print version and exit");
    desc.add_options()("help,h", "produce help message");
    desc.add_options()("prefix", po::value<std::string>(), "Set main EVerest directory");
    desc.add_options()("module,m", po::value<std::string>(),
                       "Which module should be executed (module id from config file)");
    desc.add_options()("dontvalidateschema", "Don't validate json schema on every message");
    desc.add_options()("config", po::value<std::string>(), "Just kept for compatibility, not used anymore.");
    desc.add_options()("log_config", po::value<std::string>(), "The path to a custom logging config");
    desc.add_options()("mqtt_broker_socket_path", po::value<std::string>(), "The MQTT broker socket path");
    desc.add_options()("mqtt_broker_host", po::value<std::string>(), "The MQTT broker hostname");
    desc.add_options()("mqtt_broker_port", po::value<int>(), "The MQTT broker port");
    desc.add_options()("mqtt_everest_prefix", po::value<std::string>(), "The MQTT everest prefix");
    desc.add_options()("mqtt_external_prefix", po::value<std::string>(), "The external MQTT prefix");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (argc > 0) {
        const std::string argv0 = argv[0];
        if (not argv0.empty()) {
            this->application_name = fs::path(argv0).stem().string();
        }
    }

    if (vm.count("help") != 0) {
        std::cout << desc << "\n";
        return false;
    }

    if (vm.count("version") != 0) {
        std::cout << argv[0] << " (" << this->version_information.project_name << " "
                  << this->version_information.project_version << " " << this->version_information.git_version << ")"
                  << std::endl;
        return false;
    }

    if (vm.count("config") != 0) {
        std::cout
            << "--config is not used anymore, modules request their config automatically via MQTT"
            << "\n"
            << "If you want to influence this config loading behavior you can specify the appropriate --mqtt_* flags"
            << "\n";
    }

    std::string mqtt_broker_socket_path;
    if (vm.count("mqtt_broker_socket_path") != 0) {
        mqtt_broker_socket_path = vm["mqtt_broker_socket_path"].as<std::string>();
    }

    std::string mqtt_broker_host;
    if (vm.count("mqtt_broker_host") != 0) {
        mqtt_broker_host = vm["mqtt_broker_host"].as<std::string>();
        if (!mqtt_broker_socket_path.empty()) {
            // invalid configuration, can't have both UDS and IDS
            throw BootException(
                fmt::format("Setting both the Unix Domain Socket {} and Internet Domain Socket {} in config is invalid",
                            mqtt_broker_socket_path, mqtt_broker_host));
        }
    } else {
        mqtt_broker_host = defaults::MQTT_BROKER_HOST;
    }

    // overwrite mqtt broker host with environment variable
    // NOLINTNEXTLINE(concurrency-mt-unsafe): not problematic that this function is not threadsafe here
    const char* mqtt_server_address = std::getenv("MQTT_SERVER_ADDRESS");
    if (mqtt_server_address != nullptr) {
        mqtt_broker_host = mqtt_server_address;

        if (!mqtt_broker_socket_path.empty()) {
            // invalid configuration, can't have both UDS and IDS
            throw BootException(
                fmt::format("Setting both the Unix Domain Socket {} and Internet Domain Socket {} in "
                            "config and as environment variable respectivelly (as MQTT_SERVER_ADDRESS) is not allowed",
                            mqtt_broker_socket_path, mqtt_broker_host));
        }
    }

    int mqtt_broker_port = defaults::MQTT_BROKER_PORT;
    if (vm.count("mqtt_broker_port") != 0) {
        mqtt_broker_port = vm["mqtt_broker_port"].as<int>();
    }

    // overwrite mqtt broker port with environment variable
    // NOLINTNEXTLINE(concurrency-mt-unsafe): not problematic that this function is not threadsafe here
    const char* mqtt_server_port = std::getenv("MQTT_SERVER_PORT");

    if (mqtt_server_port != nullptr) {
        try {
            mqtt_broker_port = std::stoi(mqtt_server_port);
        } catch (...) {
            EVLOG_warning << "Environment variable MQTT_SERVER_PORT set, but not set to an integer. Ignoring.";
        }
    }

    std::string mqtt_everest_prefix;
    if (vm.count("mqtt_everest_prefix") != 0) {
        mqtt_everest_prefix = vm["mqtt_everest_prefix"].as<std::string>();
    } else {
        mqtt_everest_prefix = defaults::MQTT_EVEREST_PREFIX;
    }

    // always make sure the everest mqtt prefix ends with '/'
    if (mqtt_everest_prefix.length() > 0 && mqtt_everest_prefix.back() != '/') {
        mqtt_everest_prefix = mqtt_everest_prefix += "/";
    }

    std::string mqtt_external_prefix;
    if (vm.count("mqtt_external_prefix") != 0) {
        mqtt_external_prefix = vm["mqtt_external_prefix"].as<std::string>();
    } else {
        mqtt_external_prefix = "";
    }

    if (not mqtt_broker_socket_path.empty()) {
        populate_mqtt_settings(this->mqtt_settings, mqtt_broker_socket_path, mqtt_everest_prefix, mqtt_external_prefix);
    } else {
        populate_mqtt_settings(this->mqtt_settings, mqtt_broker_host, mqtt_broker_port, mqtt_everest_prefix,
                               mqtt_external_prefix);
    }

    if (vm.count("log_config") != 0) {
        auto command_line_logging_config_file = vm["log_config"].as<std::string>();
        this->logging_config_file =
            assert_file(command_line_logging_config_file, "Command line provided logging config");

    } else {
        auto default_logging_config_file =
            fs::path(defaults::SYSCONF_DIR) / defaults::NAMESPACE / defaults::LOGGING_CONFIG_NAME;

        if (std::strcmp(defaults::PREFIX, "/usr") != 0) {
            default_logging_config_file = defaults::PREFIX / default_logging_config_file;
        } else {
            default_logging_config_file = fs::path("/") / default_logging_config_file;
        }
        this->logging_config_file = assert_file(default_logging_config_file, "Default logging config");
    }

    this->original_process_name = argv[0];

    if (vm.count("module") != 0) {
        this->module_id = vm["module"].as<std::string>();
    } else {
        EVTHROW(EVEXCEPTION(EverestApiError, "--module parameter is required"));
    }

    return true;
}

} // namespace Everest
