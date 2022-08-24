// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <framework/runtime.hpp>

namespace Everest {
RuntimeSettings::RuntimeSettings(const po::variables_map& vm) : main_dir(vm["main_dir"].as<std::string>()) {
    if (vm.count("schemas_dir")) {
        schemas_dir = vm["schemas_dir"].as<std::string>();
    } else {
        schemas_dir = main_dir / "schemas";
    }

    if (vm.count("modules_dir")) {
        modules_dir = vm["modules_dir"].as<std::string>();
    } else {
        modules_dir = main_dir / "modules";
    }

    if (vm.count("interfaces_dir")) {
        interfaces_dir = vm["interfaces_dir"].as<std::string>();
    } else {
        interfaces_dir = main_dir / "interfaces";
    }

    if (vm.count("types_dir")) {
        types_dir = vm["types_dir"].as<std::string>();
    } else {
        types_dir = main_dir / "types";
    }

    if (vm.count("log_conf")) {
        logging_config = vm["log_conf"].as<std::string>();
    } else {
        logging_config = main_dir / "conf/logging.ini";
    }

    if (vm.count("conf")) {
        config_file = vm["conf"].as<std::string>();
    } else {
        config_file = main_dir / "conf/config.json";
    }

    validate_schema = (vm.count("dontvalidateschema") != 0);

    // make all paths canonical
    std::reference_wrapper<fs::path> list[] = {
        main_dir, schemas_dir, modules_dir, interfaces_dir, logging_config, config_file,
    };

    for (auto ref_wrapped_item : list) {
        auto& item = ref_wrapped_item.get();
        item = fs::canonical(item);
    }

    // FIXME (aw): we don't have a way yet, to specify to configs dir, so by default we're using the folder, which
    // contains the config file
    configs_dir = config_file.parent_path();
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
    if (!this->runtime_settings) {
        return 0;
    }

    auto& rs = this->runtime_settings;

    Logging::init(rs->logging_config.string(), this->module_id);

    try {
        Config config = Config(rs->schemas_dir.string(), rs->config_file.string(), rs->modules_dir.string(),
                               rs->interfaces_dir.string(), rs->types_dir.string());

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

        // NOLINTNEXTLINE(concurrency-mt-unsafe): not problematic that this function is not threadsafe here
        const char* mqtt_server_address = std::getenv("MQTT_SERVER_ADDRESS");
        if (mqtt_server_address == nullptr) {
            mqtt_server_address = "localhost";
        }

        // NOLINTNEXTLINE(concurrency-mt-unsafe): not problematic that this function is not threadsafe here
        const char* mqtt_server_port = std::getenv("MQTT_SERVER_PORT");
        if (mqtt_server_port == nullptr) {
            mqtt_server_port = "1883";
        }

        Everest& everest = Everest::Everest::get_instance(this->module_id, config, rs->validate_schema,
                                                          mqtt_server_address, mqtt_server_port);

        // module import
        EVLOG_debug << fmt::format("Initializing module {}...", module_identifier);

        if (!everest.connect()) {
            EVLOG_error << fmt::format("Cannot connect to MQTT broker at {}:{}", mqtt_server_address,
                                           mqtt_server_port);
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

        // NOLINTNEXTLINE(modernize-avoid-bind): prefer bind here for readability
        module_adapter.ext_mqtt_publish =
            std::bind(&Everest::Everest::external_mqtt_publish, &everest, std::placeholders::_1, std::placeholders::_2);

        // NOLINTNEXTLINE(modernize-avoid-bind): prefer bind here for readability
        module_adapter.ext_mqtt_subscribe = std::bind(&Everest::Everest::provide_external_mqtt_handler, &everest,
                                                      std::placeholders::_1, std::placeholders::_2);

        this->callbacks.register_module_adapter(module_adapter);

        // FIXME (aw): would be nice to move this config related thing toward the module_init function
        std::vector<cmd> cmds =
            this->callbacks.everest_register(config.get_main_config()[this->module_id]["connections"]);

        for (auto const& command : cmds) {
            everest.provide_cmd(command);
        }

        auto module_configs = config.get_module_configs(this->module_id);
        const auto module_info = config.get_module_info(this->module_id);

        this->callbacks.init(module_configs, module_info);

        everest.spawn_main_loop_thread();

        // register the modules ready handler with the framework
        // this handler gets called when the global ready signal is received
        everest.register_on_ready_handler(this->callbacks.ready);

        // the module should now be ready
        everest.signal_ready();

        everest.wait_for_main_loop_end();

        EVLOG_info << "Exiting...";
    } catch (boost::exception& e) {
        EVLOG_critical << fmt::format("Caught top level boost::exception:\n{}",
                                       boost::diagnostic_information(e, true));
    } catch (std::exception& e) {
        EVLOG_critical << fmt::format("Caught top level std::exception:\n{}", boost::diagnostic_information(e, true));
    }

    return 0;
}

bool ModuleLoader::parse_command_line(int argc, char* argv[]) {
    po::options_description desc("EVerest");
    desc.add_options()("help,h", "produce help message");
    desc.add_options()("main_dir", po::value<std::string>(), "Set main EVerest directory");
    desc.add_options()("module,m", po::value<std::string>(),
                       "Which module should be executed (module id from config file)");
    desc.add_options()("log_conf", po::value<std::string>(), "The path to a custom logging.ini");
    desc.add_options()("dontvalidateschema", "Don't validate json schema on every message");
    desc.add_options()("conf", po::value<std::string>(), "The path to a custom config.json");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") != 0) {
        std::cout << desc << "\n";
        return false;
    }

    if (vm.count("main_dir") == 0) {
        EVTHROW(EVEXCEPTION(EverestApiError, "--main_dir parameter is required"));
    }

    this->runtime_settings = std::make_unique<RuntimeSettings>(vm);
    this->original_process_name = argv[0];

    if (vm.count("module") != 0) {
        this->module_id = vm["module"].as<std::string>();
    } else {
        EVTHROW(EVEXCEPTION(EverestApiError, "--module parameter is required"));
    }

    return true;
}

} // namespace Everest
