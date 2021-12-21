// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

#include <cstdlib>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp>
#include <everest/logging.hpp>

#include <framework/everest.hpp>
#include <utils/config.hpp>
#include <utils/mqtt_abstraction.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    po::options_description desc("EVerest manager");
    desc.add_options()("help,h", "produce help message");
    desc.add_options()("check", "Check and validate all config files and exit (0=success)");
    desc.add_options()("dump", po::value<std::string>(),
                       "Dump validated and augmented main config and all used module manifests into dir");
    desc.add_options()("dumpmanifests", po::value<std::string>(),
                       "Dump manifests of all modules into dir (even modules not used in config) and exit");
    desc.add_options()("main_dir", po::value<std::string>(), "set dir in which the main binaries reside");
    desc.add_options()("schemas_dir", po::value<std::string>(), "set dir in which the schemes folder resides");
    desc.add_options()("modules_dir", po::value<std::string>(), "set dir in which the modules reside ");
    desc.add_options()("interfaces_dir", po::value<std::string>(), "set dir in which the classes reside ");
    desc.add_options()("standalone,s", po::value<std::vector<std::string>>()->multitoken(),
                       "Module ID(s) to not automatically start child processes for (those must be started manually to "
                       "make the framework start!).");
    desc.add_options()(
        "ignore", po::value<std::vector<std::string>>()->multitoken(),
        "Module ID(s) to ignore: Do not automatically start child processes and do not require that they are started.");
    desc.add_options()("dontvalidateschema", "Don't validate json schema on every message");
    desc.add_options()("log_conf", po::value<std::string>(), "The path to a custom logging.ini");
    desc.add_options()("conf", po::value<std::string>(), "The path to a custom config.json");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") != 0) {
        std::cout << desc << "\n";
        return 1;
    }

    bool validate_data_with_schema = true;
    if (vm.count("dontvalidateschema") != 0) {
        validate_data_with_schema = false;
    }

    bool check = false;
    if (vm.count("check") != 0) {
        check = true;
    }

    std::string main_dir = "/usr/lib/everest";
    std::string main_binary = "./bin/main";
    if (vm.count("main_dir") != 0) {
        main_dir = vm["main_dir"].as<std::string>();
        main_binary = main_dir + "/bin/main";
    }
    main_dir = std::string(boost::filesystem::canonical(boost::filesystem::path(main_dir)).c_str());

    std::string schemas_dir = main_dir + "/schemas";
    if (vm.count("schemas_dir") != 0) {
        schemas_dir = vm["schemas_dir"].as<std::string>();
    }

    std::string modules_dir = main_dir + "/modules";
    if (vm.count("modules_dir") != 0) {
        modules_dir = vm["modules_dir"].as<std::string>();
    }

    std::string interfaces_dir = main_dir + "/interfaces";
    if (vm.count("interfaces_dir") != 0) {
        interfaces_dir = vm["interfaces_dir"].as<std::string>();
    }

    const int dump_indent = 4;

    // initialize logging as early as possible
    std::string logging_config = main_dir + "/conf/logging.ini";
    if (vm.count("log_conf") != 0) {
        logging_config = vm["log_conf"].as<std::string>();
    }
    Everest::Logging::init(logging_config);
    EVLOG(debug) << "main_dir was set to " << main_dir;
    EVLOG(debug) << "main_binary was set to " << main_binary;
    // dump all manifests if requested and terminate afterwards
    if (vm.count("dumpmanifests") != 0) {
        boost::filesystem::path dumpmanifests_path = boost::filesystem::path(vm["dumpmanifests"].as<std::string>());
        EVLOG(info) << "Dumping all known validated manifests into '" << dumpmanifests_path << "'";

        auto manifests = Everest::Config::load_all_manifests(modules_dir, schemas_dir);

        for (const auto& module : manifests.items()) {
            std::string filename = module.key() + ".json";
            boost::filesystem::path module_output_path = dumpmanifests_path / filename;
            boost::filesystem::ofstream output_stream(module_output_path);

            output_stream << module.value().dump(dump_indent);
        }

        return 0;
    }

    // load and validate whole config
    std::string config_file = main_dir + "conf/config.json";
    if (vm.count("conf") != 0) {
        config_file = vm["conf"].as<std::string>();
    }
    Everest::Config* config = nullptr;
    try {
        config = new Everest::Config(schemas_dir, config_file, modules_dir, interfaces_dir);
    } catch (Everest::EverestInternalError& e) {
        EVLOG(error) << "Failed to load and validate config!" << std::endl << boost::diagnostic_information(e, true);
        return 128;
    } catch (boost::exception& e) {
        EVLOG(error) << "Failed to load and validate config!";
        EVLOG(critical) << "Caught top level boost::exception:" << std::endl << boost::diagnostic_information(e, true);
        return 1;
    } catch (std::exception& e) {
        EVLOG(error) << "Failed to load and validate config!";
        EVLOG(critical) << "Caught top level std::exception:" << std::endl << boost::diagnostic_information(e, true);
        return 1;
    }

    // dump config if requested
    if (vm.count("dump") != 0) {
        boost::filesystem::path dump_path = boost::filesystem::path(vm["dump"].as<std::string>());
        EVLOG(info) << "Dumping validated config and manifests into '" << dump_path << "'";

        boost::filesystem::path config_dump_path = dump_path / "config.json";

        boost::filesystem::ofstream output_config_stream(config_dump_path);

        output_config_stream << config->get_main_config().dump(dump_indent);

        auto manifests = config->get_manifests();

        for (const auto& module : manifests.items()) {
            std::string filename = module.key() + ".json";
            boost::filesystem::path module_output_path = dump_path / filename;
            boost::filesystem::ofstream output_stream(module_output_path);

            output_stream << module.value().dump(dump_indent);
        }
    }

    // only config check (and or config dumping) was requested, log check result and exit
    if (check) {
        EVLOG(info) << "Config is valid, terminating as requested";
        return 0;
    }

    std::vector<std::string> standalone_modules;
    if (vm.count("standalone") != 0) {
        standalone_modules = vm["standalone"].as<std::vector<std::string>>();
    }

    std::vector<std::string> ignored_modules;
    if (vm.count("ignore") != 0) {
        ignored_modules = vm["ignore"].as<std::vector<std::string>>();
    }

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

    Everest::MQTTAbstraction& mqtt_abstraction =
        Everest::MQTTAbstraction::get_instance(mqtt_server_address, mqtt_server_port);
    mqtt_abstraction.connect();
    std::thread mainloop_thread = std::thread(&Everest::MQTTAbstraction::mainloop, &mqtt_abstraction);

    // necessary information for starting a module
    struct ModuleInfo {
        enum class Language
        {
            cpp,
            javascript
        };
        ModuleInfo(const std::string& name, Language lang, const boost::filesystem::path& path) :
            name(name), language(lang), path(path) {
        }
        std::string name;
        Language language;
        boost::filesystem::path path;
    };

    std::vector<ModuleInfo> modules_to_start;
    std::map<std::string, bool> modules_ready;
    std::mutex modules_ready_mutex;
    std::vector<Token> tokens;

    auto main_config = config->get_main_config();
    modules_to_start.reserve(main_config.size());

    for (const auto& module : main_config.items()) {
        std::string module_name = module.key();
        if (std::any_of(ignored_modules.begin(), ignored_modules.end(),
                        [module_name](const auto& element) { return element == module_name; })) {
            EVLOG(info) << "Ignoring module: " << module_name;
            continue;
        }
        std::string module_class = main_config[module_name]["module"];
        modules_ready[module_name] = false;

        Handler module_ready_handler = [module_name, &modules_ready, &modules_ready_mutex,
                                        &mqtt_abstraction](nlohmann::json json) {
            EVLOG(debug) << "received module ready signal for module: " << module_name << "(" << json << ")";
            std::unique_lock<std::mutex> lock(modules_ready_mutex);
            modules_ready[module_name] = json.get<bool>();
            for (const auto& mod : modules_ready) {
                std::string text_ready = "\033[1;32mready\033[0m";
                if (!mod.second) {
                    text_ready = "\033[1;31mnot ready\033[0m";
                }
                EVLOG(debug) << "  " << mod.first << ": " << text_ready;
            }
            if (std::all_of(modules_ready.begin(), modules_ready.end(),
                            [](const auto& element) { return element.second; })) {
                EVLOG(info) << "all modules are ready";
                mqtt_abstraction.publish("everest/ready", nlohmann::json(true));
            }
            // FIXME (aw): this unlock shouldn't be necessary because lock will get destroyed here anyway
            lock.unlock();
        };

        std::string module_id = config->printable_identifier(module_name);

        std::string topic = "everest/" + module_id + "/ready";

        Token token = mqtt_abstraction.register_handler(topic, module_ready_handler);
        tokens.push_back(token);

        if (std::any_of(standalone_modules.begin(), standalone_modules.end(),
                        [module_name](const auto& element) { return element == module_name; })) {
            EVLOG(info) << "Not starting standalone module: " << module_name;
            continue;
        }

        std::string shared_library_filename = "libmod" + module_class + ".so";
        std::string javascript_library_filename = "index.js";
        boost::filesystem::path module_path = boost::filesystem::path(modules_dir) / module_class;
        boost::filesystem::path shared_library_path = module_path / shared_library_filename;
        boost::filesystem::path javascript_library_path = module_path / javascript_library_filename;

        if (boost::filesystem::exists(shared_library_path)) {
            EVLOG(debug) << "module: " << module_name << " (" << module_class << ") provided as shared library";
            modules_to_start.emplace_back(module_name, ModuleInfo::Language::cpp, shared_library_path);
        } else if (boost::filesystem::exists(javascript_library_path)) {
            EVLOG(debug) << "module: " << module_name << " (" << module_class << ") provided as javascript library";
            modules_to_start.emplace_back(module_name, ModuleInfo::Language::javascript,
                                          boost::filesystem::canonical(javascript_library_path));
        } else {
            EVLOG(error) << "module: " << module_name << " (" << module_class
                         << ") cannot be loaded because no C++ or JavaScript library has been found";
            EVLOG(error) << "  checked paths:";
            EVLOG(error) << "    cpp: " << shared_library_path;
            EVLOG(error) << "    js:  " << javascript_library_path;
            // FIXME (aw): 123?
            return 123;
        }
    }

    // fork modules
    std::map<pid_t, std::string> children;
    for (const auto& module : modules_to_start) {
        pid_t pid = fork();
        if (pid == 0) {
            EVLOG(info) << "hello from child: " << module.name;
            char* dontvalidate_schema = nullptr;
            if (!validate_data_with_schema) {
                dontvalidate_schema = const_cast<char*>("--dontvalidateschema");
            }
            if (module.language == ModuleInfo::Language::cpp) {
                const size_t arguments = 17;
                std::array<char*, arguments> argv_list = {
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    const_cast<char*>(config->printable_identifier(module.name).c_str()),
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    const_cast<char*>("--maindir"), const_cast<char*>(main_dir.c_str()),
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    const_cast<char*>("--schemasdir"), const_cast<char*>(schemas_dir.c_str()),
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    const_cast<char*>("--modulesdir"), const_cast<char*>(modules_dir.c_str()),
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    const_cast<char*>("--classesdir"), const_cast<char*>(interfaces_dir.c_str()),
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    const_cast<char*>("--logconf"), const_cast<char*>(logging_config.c_str()),
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    const_cast<char*>("--conf"), const_cast<char*>(config_file.c_str()),
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    const_cast<char*>("--module"), const_cast<char*>(module.name.c_str()), dontvalidate_schema,
                    nullptr};
                int ret = execv(main_binary.c_str(), static_cast<char**>(argv_list.data()));
                EVLOG(error) << "error execv: " << ret;
                return 1;
            }
            if (module.language == ModuleInfo::Language::javascript) {
                // instead of using setenv, using execvpe might be a better way for a controlled environment!

                auto node_modules_path = boost::filesystem::path(main_dir) / "everestjs" / "node_modules";
                setenv("NODE_PATH", node_modules_path.c_str(), 0);

                setenv("EV_MODULE", module.name.c_str(), 1);
                setenv("EV_MAIN_DIR", boost::filesystem::canonical(main_dir).string().c_str(), 0);
                setenv("EV_SCHEMAS_DIR", boost::filesystem::canonical(schemas_dir).string().c_str(), 0);
                setenv("EV_MODULES_DIR", boost::filesystem::canonical(modules_dir).string().c_str(), 0);
                setenv("EV_INTERFACES_DIR", boost::filesystem::canonical(interfaces_dir).string().c_str(), 0);
                setenv("EV_CONF_FILE", boost::filesystem::canonical(config_file).string().c_str(), 0);
                setenv("EV_LOG_CONF_FILE", boost::filesystem::canonical(logging_config).string().c_str(), 0);

                if (!validate_data_with_schema)
                    setenv("EV_DONT_VALIDATE_SCHEMA", "", 0);

                chdir(main_dir.c_str());
                const size_t arguments = 4;
                std::array<char*, arguments> argv_list = {
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    const_cast<char*>("node"),
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    const_cast<char*>("--unhandled-rejections=strict"),
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    const_cast<char*>(module.path.string().c_str()),
                    nullptr,
                };
                int ret = execvp("node", static_cast<char**>(argv_list.data()));
                EVLOG(error) << "error execv: " << ret;
                return 1;
            }

            EVLOG(critical) << "Could not start module " << module.name << " of unknown type";
            return 1;
        } else {
            EVLOG(debug) << "Module " << module.name << " has pid: " << pid;
            children[pid] = module.name;
        }
    }

    int status = 0;
    bool running = true;
    while (running) {
        pid_t child_pid = waitpid(-1, &status, 0);
        std::string child_name = "manager";
        if (child_pid != -1 && children.count(child_pid) != 0) {
            child_name = children[child_pid];
        }
        EVLOG(critical) << "Something happened to child: " << child_name << " (pid: " << child_pid
                        << "). Status: " << status << ". Terminating all children.";
        running = false;
        for (const auto& child : children) {
            if (child.first == -1) {
                EVLOG(error)
                    << "Child with pid -1 in list of children, this cannot be correct and won't be terminated.";
                continue;
            }
            int result = kill(child.first, SIGTERM);
            if (result != 0) {
                EVLOG(critical) << "SIGTERM of child: " << child.second << " (pid: " << child.first
                                << ") \033[1;31mfailed\033[0m: " << result << ". Escalating to SIGKILL";
                result = kill(child.first, SIGKILL);
                if (result != 0) {
                    EVLOG(critical) << "SIGKILL of child: " << child.second << " (pid: " << child.first
                                    << ") \033[1;31mfailed\033[0m: " << result << ".";
                } else {
                    EVLOG(info) << "SIGKILL of child: " << child.second << " (pid: " << child.first
                                << ") \033[1;32msucceeded\033[0m.";
                }
            } else {
                EVLOG(info) << "SIGTERM of child: " << child.second << " (pid: " << child.first
                            << ") \033[1;32msucceeded\033[0m.";
            }
        }
        return 1;
    }

    return 0;
}
