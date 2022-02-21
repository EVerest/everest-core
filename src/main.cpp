// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <iostream>
#include <sys/prctl.h>
#include <thread>

#include <boost/bind/bind.hpp>
#include <boost/dll/import.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <everest/logging.hpp>
#include <fmt/core.h>

#include <framework/everest.hpp>
#include <utils/config.hpp>
#include <utils/mqtt_abstraction.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    po::options_description desc("EVerest");
    desc.add_options()("help,h", "produce help message");
    desc.add_options()("maindir", po::value<std::string>(), "set dir in which the main binaries reside");
    desc.add_options()("schemasdir", po::value<std::string>(), "set dir in which the schemes folder resides");
    desc.add_options()("modulesdir", po::value<std::string>(), "set dir in which the modules reside ");
    desc.add_options()("classesdir", po::value<std::string>(), "set dir in which the classes reside ");
    desc.add_options()("module,m", po::value<std::string>(),
                       "Which module should be executed (module id from config file)");
    desc.add_options()("logconf", po::value<std::string>(), "The path to a custom logging.ini");
    desc.add_options()("dontvalidateschema", "Don't validate json schema on every message");
    desc.add_options()("conf", po::value<std::string>(), "The path to a custom config.json");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") != 0) {
        std::cout << desc << "\n";
        return 1;
    }

    std::string module_id;
    if (vm.count("module") != 0) {
        module_id = vm["module"].as<std::string>();
    } else {
        EVTHROW(EVEXCEPTION(Everest::EverestApiError, "--module parameter is required"));
    }

    std::string maindir = ".";
    if (vm.count("maindir") != 0) {
        maindir = vm["maindir"].as<std::string>();
    }

    std::string config_file = maindir + "/conf/config.json";
    if (vm.count("conf") != 0) {
        config_file = vm["conf"].as<std::string>();
    }

    std::string schemasdir = maindir + "/schemes";
    if (vm.count("schemasdir") != 0) {
        schemasdir = vm["schemasdir"].as<std::string>();
    }

    std::string modulesdir = maindir + "/modules";
    if (vm.count("modulesdir") != 0) {
        modulesdir = vm["modulesdir"].as<std::string>();
    }

    std::string classesdir = maindir + "/classes";
    if (vm.count("classesdir") != 0) {
        classesdir = vm["classesdir"].as<std::string>();
    }

    bool validate_data_with_schema = true;
    if (vm.count("dontvalidateschema") != 0) {
        validate_data_with_schema = false;
    }

    // initialize logging as early as possible
    std::string logging_config = maindir + "/conf/logging.ini";
    if (vm.count("logconf") != 0) {
        logging_config = vm["logconf"].as<std::string>();
    }
    Everest::Logging::init(logging_config, module_id);

    EVLOG(debug) << fmt::format("module was set to {}", module_id);
    EVLOG(debug) << fmt::format("maindir was set to {}", maindir);
    EVLOG(debug) << fmt::format("config was set to {}", config_file);
    EVLOG(debug) << fmt::format("logging_config was set to {}", logging_config);
    EVLOG(debug) << fmt::format("schemasdir was set to {}", schemasdir);
    EVLOG(debug) << fmt::format("modulesdir was set to {}", modulesdir);
    EVLOG(debug) << fmt::format("classesdir was set to {}", classesdir);

    try {
        Everest::Config config = Everest::Config(schemasdir, config_file, modulesdir, classesdir);

        if (!config.contains(module_id)) {
            EVLOG(error) << fmt::format("Module id '{}' not found in config!", module_id);
            return 2;
        }

        const std::string module_identifier = config.printable_identifier(module_id);
        EVLOG(info) << fmt::format("Initializing framework for module {}...", module_identifier);
        EVLOG(debug) << fmt::format("Setting process name to: '{}'...", module_identifier);
        int prctl_return = prctl(PR_SET_NAME, module_identifier.c_str());
        if (prctl_return == 1) {
            EVLOG(warning) << fmt::format("Could not set process name to '{}', it remains '{}'", module_identifier,
                                          argv[0]);
        }
        Everest::Logging::update_process_name(module_identifier);

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

        Everest::Everest& everest = Everest::Everest::get_instance(module_id, config, validate_data_with_schema,
                                                                   mqtt_server_address, mqtt_server_port);

        // module import
        EVLOG(info) << fmt::format("Initializing module {}...", module_identifier);

        std::string module_name = config.get_main_config()[module_id]["module"].get<std::string>();

        if (!everest.connect()) {
            EVLOG(critical) << fmt::format("Cannot connect to MQTT broker at {}:{}", mqtt_server_address,
                                           mqtt_server_port);
            return 1;
        }

        std::string module_file_name = fmt::format("libmod{}.so", module_name);

        boost::filesystem::path path = boost::filesystem::path(modulesdir) / module_name / module_file_name;
        EVLOG(info) << fmt::format("Loading module shared library from {}", path.string());

        boost::dll::shared_library lib(path);

        auto everest_register_call_cmd_callback = boost::dll::import_alias<void(
            std::function<Result(const Requirement& req, const std::string&, Parameters)>)>(
            path, "everest_register_call_cmd_callback");

        auto call_cmd = [&everest](const Requirement& req, const std::string& cmd_name, Parameters args) {
            return everest.call_cmd(req, cmd_name, args);
        };

        everest_register_call_cmd_callback(call_cmd);

        auto everest_register_publish_var_callback =
            boost::dll::import_alias<void(std::function<void(const std::string&, const std::string&, Value)>)>(
                path, "everest_register_publish_var_callback");

        auto publish_var = [&everest](const std::string& param1, const std::string& param2, Value param3) {
            return everest.publish_var(param1, param2, param3);
        };

        everest_register_publish_var_callback(publish_var);

        auto everest_register_subscribe_var_callback = boost::dll::import_alias<void(
            std::function<void(const Requirement& req, const std::string&, ValueCallback)>)>(
            path, "everest_register_subscribe_var_callback");

        auto subscribe_var = [&everest](const Requirement& req, const std::string& var_name,
                                        const ValueCallback& callback) {
            return everest.subscribe_var(req, var_name, callback);
        };

        everest_register_subscribe_var_callback(subscribe_var);

        auto everest_register_external_mqtt_publish_callback =
            boost::dll::import_alias<void(std::function<void(const std::string&, const std::string&)>)>(
                path, "everest_register_external_mqtt_publish_callback");

        // NOLINTNEXTLINE(modernize-avoid-bind): prefer bind here for readability
        auto external_mqtt_publish =
            std::bind(&Everest::Everest::external_mqtt_publish, &everest, std::placeholders::_1, std::placeholders::_2);

        everest_register_external_mqtt_publish_callback(external_mqtt_publish);

        auto everest_register_external_mqtt_handler_callback =
            boost::dll::import_alias<void(std::function<void(const std::string&, StringHandler)>)>(
                path, "everest_register_external_mqtt_handler_callback");

        // NOLINTNEXTLINE(modernize-avoid-bind): prefer bind here for readability
        auto external_mqtt_handler = std::bind(&Everest::Everest::provide_external_mqtt_handler, &everest,
                                               std::placeholders::_1, std::placeholders::_2);

        everest_register_external_mqtt_handler_callback(external_mqtt_handler);

        auto everest_register = boost::dll::import_alias<std::vector<Everest::cmd>(json)>(path, "everest_register");

        // FIXME (aw): would be nice to move this config related thing toward the module_init function
        std::vector<Everest::cmd> cmds = everest_register(config.get_main_config()[module_id]["connections"]);

        for (auto const& command : cmds) {
            everest.provide_cmd(command);
        }

        auto module_configs = config.get_module_configs(module_id);
        const auto module_info = config.get_module_info(module_id);

        auto module_init = boost::dll::import_alias<void(ModuleConfigs, ModuleInfo)>(path, "init");
        module_init(module_configs, module_info);

        std::thread mainloop_thread = std::thread(&Everest::Everest::mainloop, &everest);

        auto module_ready = boost::dll::import_alias<void()>(path, "ready");

        // register the modules ready handler with the framework
        // this handler gets called when the global ready signal is received
        everest.register_on_ready_handler(module_ready);

        // the module should now be ready
        everest.signal_ready();

        mainloop_thread.join();

        EVLOG(info) << "Exiting...";
    } catch (boost::exception& e) {
        EVLOG(critical) << fmt::format("Caught top level boost::exception:\n{}",
                                       boost::diagnostic_information(e, true));
    } catch (std::exception& e) {
        EVLOG(critical) << fmt::format("Caught top level std::exception:\n{}", boost::diagnostic_information(e, true));
    }
    return 0;
}
