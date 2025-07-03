// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <thread>
#include <unordered_map>

#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <everest/logging.hpp>
#include <framework/everest.hpp>
#include <framework/runtime.hpp>
#include <utils/config.hpp>
#include <utils/mqtt_abstraction.hpp>
#include <utils/status_fifo.hpp>

#include "controller/ipc.hpp"
#include "system_unix.hpp"
#include <generated/version_information.hpp>

namespace po = boost::program_options;
namespace fs = std::filesystem;

using namespace Everest;

const auto PARENT_DIED_SIGNAL = SIGTERM;
const int CONTROLLER_IPC_READ_TIMEOUT_MS = 50;
const auto complete_start_time = std::chrono::system_clock::now();

#ifdef ENABLE_ADMIN_PANEL
class ControllerHandle {
public:
    ControllerHandle(pid_t pid, int socket_fd) : pid(pid), socket_fd(socket_fd) {
        // we do "non-blocking" read
        controller_ipc::set_read_timeout(socket_fd, CONTROLLER_IPC_READ_TIMEOUT_MS);
    }

    void send_message(const nlohmann::json& msg) {
        controller_ipc::send_message(socket_fd, msg);
    }

    controller_ipc::Message receive_message() {
        return controller_ipc::receive_message(socket_fd);
    }

    void shutdown() {
        // FIXME (aw): tbd
    }

    const pid_t pid;

private:
    const int socket_fd;
};
#endif

// Helper struct keeping information on how to start module
struct ModuleStartInfo {
    enum class Language {
        cpp,
        javascript,
        python
    };
    ModuleStartInfo(const std::string& name_, const std::string& printable_name_, Language lang_, const fs::path& path_,
                    std::vector<std::string> capabilities_) :
        name(name_),
        printable_name(printable_name_),
        language(lang_),
        path(path_),
        capabilities(std::move(capabilities_)) {
    }
    std::string name;
    std::string printable_name;
    Language language;
    fs::path path;

    // required capabilities of this module
    std::vector<std::string> capabilities;
};

namespace {
/// \brief Setup common environment variables for everestjs and everestpy
void setup_environment(const ModuleStartInfo& module_info, const RuntimeSettings& rs,
                       const MQTTSettings& mqtt_settings) {
    setenv(EV_MODULE, module_info.name.c_str(), 1);
    setenv(EV_PREFIX, rs.prefix.c_str(), 0);
    setenv(EV_LOG_CONF_FILE, rs.logging_config_file.c_str(), 0);
    setenv(EV_MQTT_EVEREST_PREFIX, mqtt_settings.everest_prefix.c_str(), 0);
    setenv(EV_MQTT_EXTERNAL_PREFIX, mqtt_settings.external_prefix.c_str(), 0);
    if (mqtt_settings.uses_socket()) {
        setenv(EV_MQTT_BROKER_SOCKET_PATH, mqtt_settings.broker_socket_path.c_str(), 0);
    } else {
        setenv(EV_MQTT_BROKER_HOST, mqtt_settings.broker_host.c_str(), 0);
        setenv(EV_MQTT_BROKER_PORT, std::to_string(mqtt_settings.broker_port).c_str(), 0);
    }

    if (rs.validate_schema) {
        setenv(EV_VALIDATE_SCHEMA, "1", 1);
    }
}

std::vector<char*> arguments_to_exec_argv(std::vector<std::string>& arguments) {
    std::vector<char*> argv_list(arguments.size() + 1);
    std::transform(arguments.begin(), arguments.end(), argv_list.begin(),
                   [](std::string& value) { return value.data(); });

    // add NULL for exec
    argv_list.back() = nullptr;
    return argv_list;
}

void exec_cpp_module(system::SubProcess& proc_handle, const ModuleStartInfo& module_info, const RuntimeSettings& rs,
                     const MQTTSettings& mqtt_settings) {
    const auto exec_binary = module_info.path.c_str();
    std::vector<std::string> arguments = {
        module_info.printable_name,
        "--prefix",
        rs.prefix.string(),
        "--module",
        module_info.name,
        "--log_config",
        rs.logging_config_file.string(),
        "--mqtt_everest_prefix",
        mqtt_settings.everest_prefix,
        "--mqtt_external_prefix",
        mqtt_settings.external_prefix}; // TODO: check if this is empty and do not append if needed?

    if (mqtt_settings.uses_socket()) {
        arguments.insert(arguments.end(), {"--mqtt_broker_socket_path", mqtt_settings.broker_socket_path});
    } else {
        arguments.insert(arguments.end(), {"--mqtt_broker_host", mqtt_settings.broker_host, "--mqtt_broker_port",
                                           std::to_string(mqtt_settings.broker_port)});
    }

    const auto argv_list = arguments_to_exec_argv(arguments);
    execv(exec_binary, argv_list.data());

    // exec failed
    proc_handle.send_error_and_exit(fmt::format("Syscall to execv() with \"{} {}\" failed ({})", exec_binary,
                                                fmt::join(arguments.begin() + 1, arguments.end(), " "),
                                                strerror(errno)));
}

void exec_javascript_module(system::SubProcess& proc_handle, const ModuleStartInfo& module_info,
                            const RuntimeSettings& rs, const MQTTSettings& mqtt_settings) {
    // instead of using setenv, using execvpe might be a better way for a controlled environment!

    // FIXME (aw): everest directory layout
    const auto node_modules_path = rs.prefix / defaults::LIB_DIR / defaults::NAMESPACE / "node_modules";
    setenv("NODE_PATH", node_modules_path.c_str(), 0);

    setup_environment(module_info, rs, mqtt_settings);

    const auto node_binary = "node";

    std::vector<std::string> arguments = {
        "node",
        "--unhandled-rejections=strict",
        module_info.path.string(),
    };

    const auto argv_list = arguments_to_exec_argv(arguments);
    execvp(node_binary, argv_list.data());

    // exec failed
    proc_handle.send_error_and_exit(fmt::format("Syscall to execv() with \"{} {}\" failed ({})", node_binary,
                                                fmt::join(arguments.begin() + 1, arguments.end(), " "),
                                                strerror(errno)));
}

void exec_python_module(system::SubProcess& proc_handle, const ModuleStartInfo& module_info, const RuntimeSettings& rs,
                        const MQTTSettings& mqtt_settings) {
    // instead of using setenv, using execvpe might be a better way for a controlled environment!

    const auto pythonpath = rs.prefix / defaults::LIB_DIR / defaults::NAMESPACE / "everestpy";

    setenv("PYTHONPATH", pythonpath.c_str(), 0);

    setup_environment(module_info, rs, mqtt_settings);

    const auto python_binary = "python3";

    std::vector<std::string> arguments = {python_binary, module_info.path.c_str()};

    const auto argv_list = arguments_to_exec_argv(arguments);
    execvp(python_binary, argv_list.data());

    // exec failed
    proc_handle.send_error_and_exit(fmt::format("Syscall to execv() with \"{} {}\" failed ({})", python_binary,
                                                fmt::join(arguments.begin() + 1, arguments.end(), " "),
                                                strerror(errno)));
}

void exec_module(const RuntimeSettings& rs, const MQTTSettings& mqtt_settings, const ModuleStartInfo& module,
                 system::SubProcess& proc_handle) {
    switch (module.language) {
    case ModuleStartInfo::Language::cpp:
        exec_cpp_module(proc_handle, module, rs, mqtt_settings);
        break;
    case ModuleStartInfo::Language::javascript:
        exec_javascript_module(proc_handle, module, rs, mqtt_settings);
        break;
    case ModuleStartInfo::Language::python:
        exec_python_module(proc_handle, module, rs, mqtt_settings);
        break;
    default:
        throw std::logic_error("Module language not in enum");
        break;
    }
}

std::map<pid_t, std::string> spawn_modules(const std::vector<ModuleStartInfo>& modules, const ManagerSettings& ms) {
    std::map<pid_t, std::string> started_modules;

    const auto& rs = ms.runtime_settings;

    for (const auto& module : modules) {

        auto proc_handle = system::SubProcess::create(ms.run_as_user, module.capabilities);

        if (proc_handle.is_child()) {
            // first, check if we need any capabilities

            try {
                exec_module(rs, ms.mqtt_settings, module, proc_handle);
            } catch (const std::exception& err) {
                proc_handle.send_error_and_exit(err.what());
            }
        }

        // we can only come here, if we're the parent!
        const auto child_pid = proc_handle.check_child_executed();

        EVLOG_debug << fmt::format("Forked module {} with pid: {}", module.name, child_pid);
        started_modules[child_pid] = module.name;
    }

    return started_modules;
}
} // namespace

struct ModuleReadyInfo {
    bool ready;
    std::shared_ptr<TypedHandler> ready_token;
    std::shared_ptr<TypedHandler> get_config_token;
};

// FIXME (aw): these are globals here, because they are used in the ready callback handlers
using ModulesReadyType = std::unordered_map<std::string, ModuleReadyInfo>;
namespace {
ModulesReadyType modules_ready; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
// Don't hold the mutex and use any function of the `mqtt_abstraction` since the
// mutex is also held inside the `ready` handler which can deadlock.
std::mutex modules_ready_mutex; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

std::map<pid_t, std::string> start_modules(ManagerConfig& config, MQTTAbstraction& mqtt_abstraction,
                                           const std::vector<std::string>& ignored_modules,
                                           const std::vector<std::string>& standalone_modules,
                                           const ManagerSettings& ms, StatusFifo& status_fifo, bool retain_topics) {
    BOOST_LOG_FUNCTION();

    std::vector<ModuleStartInfo> modules_to_spawn;

    const auto& module_configurations = config.get_module_configurations();
    const auto& module_names = config.get_module_names();
    modules_to_spawn.reserve(module_configurations.size());
    const auto number_of_modules = module_configurations.size();
    EVLOG_info << "Starting " << number_of_modules << " modules";

    const auto interface_definitions = config.get_interface_definitions();
    std::vector<std::string> interface_names;
    for (auto& interface_definition : interface_definitions.items()) {
        interface_names.push_back(interface_definition.key());
    }
    mqtt_abstraction.publish(fmt::format("{}interfaces", ms.mqtt_settings.everest_prefix), interface_names, QOS::QOS2,
                             true);

    for (const auto& interface_definition : interface_definitions.items()) {
        mqtt_abstraction.publish(
            fmt::format("{}interface_definitions/{}", ms.mqtt_settings.everest_prefix, interface_definition.key()),
            interface_definition.value(), QOS::QOS2, true);
    }

    const auto type_definitions = config.get_types();
    std::vector<std::string> type_names;
    for (auto& type_definition : type_definitions.items()) {
        type_names.push_back(type_definition.key());
    }
    mqtt_abstraction.publish(fmt::format("{}types", ms.mqtt_settings.everest_prefix), type_names, QOS::QOS2, true);
    for (const auto& type_definition : type_definitions.items()) {
        // type_definition keys already start with a / so omit it in the topic name
        mqtt_abstraction.publish(
            fmt::format("{}type_definitions{}", ms.mqtt_settings.everest_prefix, type_definition.key()),
            type_definition.value(), QOS::QOS2, true);
    }

    const auto settings = config.get_settings();
    mqtt_abstraction.publish(fmt::format("{}settings", ms.mqtt_settings.everest_prefix), settings, QOS::QOS2, true);

    if (ms.runtime_settings.validate_schema) {
        const auto schemas = config.get_schemas();
        mqtt_abstraction.publish(fmt::format("{}schemas", ms.mqtt_settings.everest_prefix), schemas, QOS::QOS2, true);
    }
    const auto manifests = config.get_manifests();

    for (const auto& manifest : manifests.items()) {
        auto manifest_copy = manifest.value();
        manifest_copy.erase("config");
        mqtt_abstraction.publish(fmt::format("{}manifests/{}", ms.mqtt_settings.everest_prefix, manifest.key()),
                                 manifest_copy, QOS::QOS2, true);
    }

    mqtt_abstraction.publish(fmt::format("{}module_names", ms.mqtt_settings.everest_prefix), module_names, QOS::QOS2,
                             true);

    for (const auto& [module_id_, module_config] : module_configurations) {
        const auto& module_name = module_config.module_name;
        const auto& module_id = module_id_;
        if (std::any_of(ignored_modules.begin(), ignored_modules.end(),
                        [module_id](const auto& element) { return element == module_id; })) {
            EVLOG_info << fmt::format("Ignoring module: {}", module_id);
            continue;
        }
        const auto serialized_mod_config = get_serialized_module_config(module_id, module_configurations);

        // FIXME (aw): implicitely adding ModuleReadyInfo and setting its ready member
        auto module_it = modules_ready.emplace(module_id, ModuleReadyInfo{false, nullptr, nullptr}).first;

        const std::string config_topic = fmt::format("{}/config", config.mqtt_module_prefix(module_id));
        const Handler module_get_config_handler = [module_id, config_topic, serialized_mod_config,
                                                   &mqtt_abstraction](const std::string&, const nlohmann::json&) {
            mqtt_abstraction.publish(config_topic, serialized_mod_config.dump(), QOS::QOS2);
        };

        const std::string get_config_topic = fmt::format("{}/get_config", config.mqtt_module_prefix(module_id));
        module_it->second.get_config_token = std::make_shared<TypedHandler>(
            HandlerType::ExternalMQTT, std::make_shared<Handler>(module_get_config_handler));
        mqtt_abstraction.register_handler(get_config_topic, module_it->second.get_config_token, QOS::QOS2);

        std::vector<std::string> capabilities;
        const auto& module_capabilities = module_configurations.at(module_id).capabilities;
        if (module_capabilities.has_value()) {
            capabilities.push_back(module_capabilities.value());
        }

        const Handler module_ready_handler = [module_id, &mqtt_abstraction, &config, standalone_modules,
                                              mqtt_everest_prefix = ms.mqtt_settings.everest_prefix, &status_fifo,
                                              retain_topics](const std::string&, const nlohmann::json& json) {
            EVLOG_debug << fmt::format("received module ready signal for module: {}({})", module_id, json.dump());
            const std::unique_lock<std::mutex> lock(modules_ready_mutex);
            // FIXME (aw): here are race conditions, if the ready handler gets called while modules are shut down!
            try {
                modules_ready.at(module_id).ready = json.get<bool>();
            } catch (const std::out_of_range& ex) {
                // This can happen if we're shutting down and a module becomes
                // ready.
                EVLOG_error << "The module " << module_id << " is not in `modules_ready`: " << ex.what();
                return;
            }
            std::size_t modules_spawned = 0;
            for (const auto& mod : modules_ready) {
                const std::string text_ready =
                    fmt::format((mod.second.ready) ? TERMINAL_STYLE_OK : TERMINAL_STYLE_ERROR, "ready");
                EVLOG_debug << fmt::format("  {}: {}", mod.first, text_ready);
                if (mod.second.ready) {
                    modules_spawned += 1;
                }
            }
            if (!standalone_modules.empty() && std::find(standalone_modules.begin(), standalone_modules.end(),
                                                         module_id) != standalone_modules.end()) {
                EVLOG_info << fmt::format("Standalone module {} initialized.", module_id);
            }
            if (std::all_of(modules_ready.begin(), modules_ready.end(),
                            [](const auto& element) { return element.second.ready; })) {
                const auto complete_end_time = std::chrono::system_clock::now();
                status_fifo.update(StatusFifo::ALL_MODULES_STARTED);
                if (not retain_topics) {
                    EVLOG_info << "Clearing retained topics published by manager during startup";
                    mqtt_abstraction.clear_retained_topics();
                } else {
                    EVLOG_info << "Keeping retained topics published by manager during startup for inspection";
                }
                EVLOG_info << fmt::format(
                    TERMINAL_STYLE_OK, "🚙🚙🚙 All modules are initialized. EVerest up and running [{}ms] 🚙🚙🚙",
                    std::chrono::duration_cast<std::chrono::milliseconds>(complete_end_time - complete_start_time)
                        .count());
                mqtt_abstraction.publish(fmt::format("{}ready", mqtt_everest_prefix), nlohmann::json(true));
            } else if (!standalone_modules.empty()) {
                if (modules_spawned == modules_ready.size() - standalone_modules.size()) {
                    EVLOG_info << fmt::format(fg(fmt::terminal_color::green),
                                              "Modules started by manager are ready, waiting for standalone modules.");
                    status_fifo.update(StatusFifo::WAITING_FOR_STANDALONE_MODULES);
                }
            }
        };

        const std::string ready_topic = fmt::format("{}/ready", config.mqtt_module_prefix(module_id));
        module_it->second.ready_token =
            std::make_shared<TypedHandler>(HandlerType::ExternalMQTT, std::make_shared<Handler>(module_ready_handler));
        mqtt_abstraction.register_handler(ready_topic, module_it->second.ready_token, QOS::QOS2);

        if (std::any_of(standalone_modules.begin(), standalone_modules.end(),
                        [module_id](const auto& element) { return element == module_id; })) {
            EVLOG_info << "Not starting standalone module: " << fmt::format(TERMINAL_STYLE_BLUE, "{}", module_id);
            continue;
        }

        const std::string binary_filename = fmt::format("{}", module_name);
        const std::string javascript_library_filename = "index.js";
        const std::string python_filename = "module.py";
        const auto module_path = ms.runtime_settings.modules_dir / module_name;
        const auto printable_module_name = config.printable_identifier(module_id);
        const auto binary_path = module_path / binary_filename;
        const auto javascript_library_path = module_path / javascript_library_filename;
        const auto python_module_path = module_path / python_filename;

        if (fs::exists(binary_path)) {
            EVLOG_debug << fmt::format("module: {} ({}) provided as binary", module_id, module_name);
            modules_to_spawn.emplace_back(module_id, printable_module_name, ModuleStartInfo::Language::cpp, binary_path,
                                          capabilities);
        } else if (fs::exists(javascript_library_path)) {
            EVLOG_debug << fmt::format("module: {} ({}) provided as javascript library", module_id, module_name);
            modules_to_spawn.emplace_back(module_id, printable_module_name, ModuleStartInfo::Language::javascript,
                                          fs::canonical(javascript_library_path), capabilities);
        } else if (fs::exists(python_module_path)) {
            EVLOG_verbose << fmt::format("module: {} ({}) provided as python module", module_id, module_name);
            modules_to_spawn.emplace_back(module_id, printable_module_name, ModuleStartInfo::Language::python,
                                          fs::canonical(python_module_path), capabilities);
        } else {
            if (module_id == "probe" || module_name == "ProbeModule") {
                EVLOG_error << "You are trying to start the probe module as binary, please check "
                               "your test case, did you add \"@pytest.mark.probe_module\" to your test case?";
            }
            throw std::runtime_error(
                fmt::format("module: {} ({}) cannot be loaded because no Binary, JavaScript or Python "
                            "module has been found\n"
                            "  checked paths:\n"
                            "    binary: {}\n"
                            "    js:  {}\n"
                            "    py:  {}\n",
                            module_id, module_name, binary_path.string(), javascript_library_path.string(),
                            python_module_path.string()));
        }
    }

    return spawn_modules(modules_to_spawn, ms);
}

void shutdown_modules(const std::map<pid_t, std::string>& modules, ManagerConfig& config,
                      MQTTAbstraction& mqtt_abstraction) {

    ModulesReadyType modules_ready_moved;
    {
        const std::lock_guard<std::mutex> lck(modules_ready_mutex);
        modules_ready_moved = std::move(modules_ready);
        // Probably not needed after our move but lets be explicit.
        modules_ready.clear();
    }

    for (const auto& module : modules_ready_moved) {
        const auto& ready_info = module.second;
        const auto& module_name = module.first;
        const std::string topic = fmt::format("{}/ready", config.mqtt_module_prefix(module_name));
        mqtt_abstraction.unregister_handler(topic, ready_info.ready_token);
    }

    for (const auto& child : modules) {
        auto retval = kill(child.first, SIGTERM);
        // FIXME (aw): supply errno strings
        if (retval != 0) {
            EVLOG_critical << fmt::format("SIGTERM of child: {} (pid: {}) {}: {}. Escalating to SIGKILL", child.second,
                                          child.first, fmt::format(TERMINAL_STYLE_ERROR, "failed"), retval);
            retval = kill(child.first, SIGKILL);
            if (retval != 0) {
                EVLOG_critical << fmt::format("SIGKILL of child: {} (pid: {}) {}: {}.", child.second, child.first,
                                              fmt::format(TERMINAL_STYLE_ERROR, "failed"), retval);
            } else {
                EVLOG_info << fmt::format("SIGKILL of child: {} (pid: {}) {}.", child.second, child.first,
                                          fmt::format(TERMINAL_STYLE_OK, "succeeded"));
            }
        } else {
            EVLOG_info << fmt::format("SIGTERM of child: {} (pid: {}) {}.", child.second, child.first,
                                      fmt::format(TERMINAL_STYLE_OK, "succeeded"));
        }
    }
}

#ifdef ENABLE_ADMIN_PANEL
ControllerHandle start_controller(const ManagerSettings& ms) {
    std::array<int, 2> socket_pair; // NOLINT(cppcoreguidelines-pro-type-member-init): this is always initialized in the
                                    // following socketpair call

    // FIXME (aw): destroy this socketpair somewhere
    socketpair(AF_UNIX, SOCK_DGRAM, 0, socket_pair.data());
    const int manager_socket = socket_pair[0];
    const int controller_socket = socket_pair[1];

    auto proc_handle = system::SubProcess::create(ms.run_as_user);

    if (proc_handle.is_child()) {
        // FIXME (aw): hack to get the correct directory of the controller
        const auto bin_dir = fs::canonical("/proc/self/exe").parent_path();

        const auto controller_binary = bin_dir / "controller";

        close(manager_socket);
        dup2(controller_socket, STDIN_FILENO);
        close(controller_socket);

        execl(controller_binary.c_str(), MAGIC_CONTROLLER_ARG0, NULL);

        // exec failed
        proc_handle.send_error_and_exit(
            fmt::format("Syscall to execl() with \"{} {}\" failed ({})", controller_binary.string(), strerror(errno)));
    }

    close(controller_socket);

    // send initial config to controller
    controller_ipc::send_message(manager_socket,
                                 {
                                     {"method", "boot"},
                                     {"params",
                                      {
                                          {"module_dir", ms.runtime_settings.modules_dir.string()},
                                          {"interface_dir", ms.interfaces_dir.string()},
                                          {"www_dir", ms.www_dir.string()},
                                          {"configs_dir", ms.configs_dir.string()},
                                          {"logging_config_file", ms.runtime_settings.logging_config_file.string()},
                                          {"controller_port", ms.controller_port},
                                          {"controller_rpc_timeout_ms", ms.controller_rpc_timeout_ms},
                                      }},
                                 });

    return {proc_handle.check_child_executed(), manager_socket};
}
#endif

ConfigBootMode parse_config_boot_mode(const std::string& config_opt, const std::string& db_opt, const bool db_init) {
    if (config_opt.empty() and db_opt.empty()) {
        // no config or db option given, use default
        return ConfigBootMode::YamlFile;
    }
    if (!config_opt.empty() && !db_opt.empty()) {
        if (db_init == false) {
            throw BootException("Both --config and --db options are set, but no --db-init option is given. "
                                "This is not allowed.");
        }
        return ConfigBootMode::DatabaseInit;
    }
    if (!config_opt.empty()) {
        // only config option given, use yaml file
        return ConfigBootMode::YamlFile;
    }
    if (!db_opt.empty()) {
        // only db option given, use database
        return ConfigBootMode::Database;
    }
    throw std::logic_error("Could not parse config boot source, this should never happen.");
}

int boot(const po::variables_map& vm) {
    const bool check = (vm.count("check") != 0);

    const auto prefix_opt = parse_string_option(vm, "prefix");
    const auto config_opt = parse_string_option(vm, "config");
    const auto db_opt = parse_string_option(vm, "db");
    const auto db_init = vm.count("db-init") != 0;
    ConfigBootMode boot_mode = parse_config_boot_mode(config_opt, db_opt, db_init);

    ManagerSettings ms;

    switch (boot_mode) {
    case ConfigBootMode::YamlFile:
        ms = ManagerSettings(prefix_opt, config_opt);
        break;
    case ConfigBootMode::Database:
        ms = ManagerSettings(prefix_opt, db_opt, DatabaseTag{});
        break;
    case ConfigBootMode::DatabaseInit:
        ms = ManagerSettings(prefix_opt, config_opt, db_opt);
        break;
    default:
        throw BootException(fmt::format("Invalid boot source: {}", static_cast<int>(boot_mode)));
    }

    Logging::init(ms.runtime_settings.logging_config_file.string());

    EVLOG_info << "  \033[0;1;35;95m_\033[0;1;31;91m__\033[0;1;33;93m__\033[0;1;32;92m__\033[0;1;36;96m_\033[0m      "
                  "\033[0;1;31;91m_\033[0;1;33;93m_\033[0m                \033[0;1;36;96m_\033[0m   ";
    EVLOG_info << " \033[0;1;31;91m|\033[0m  \033[0;1;33;93m_\033[0;1;32;92m__\033[0;1;36;96m_\\\033[0m "
                  "\033[0;1;34;94m\\\033[0m    \033[0;1;33;93m/\033[0m \033[0;1;32;92m/\033[0m               "
                  "\033[0;1;34;94m|\033[0m \033[0;1;35;95m|\033[0m";
    EVLOG_info
        << " \033[0;1;33;93m|\033[0m \033[0;1;32;92m|_\033[0;1;36;96m_\033[0m   \033[0;1;35;95m\\\033[0m "
           "\033[0;1;31;91m\\\033[0m  \033[0;1;33;93m/\033[0m \033[0;1;32;92m/\033[0;1;36;96m__\033[0m "
           "\033[0;1;34;94m_\033[0m \033[0;1;35;95m_\033[0;1;31;91m_\033[0m \033[0;1;33;93m__\033[0;1;32;92m_\033[0m  "
           "\033[0;1;36;96m_\033[0;1;34;94m__\033[0;1;35;95m|\033[0m \033[0;1;31;91m|_\033[0m";
    EVLOG_info << " \033[0;1;32;92m|\033[0m  \033[0;1;36;96m_\033[0;1;34;94m_|\033[0m   \033[0;1;31;91m\\\033[0m "
                  "\033[0;1;33;93m\\\033[0;1;32;92m/\033[0m \033[0;1;36;96m/\033[0m \033[0;1;34;94m_\033[0m "
                  "\033[0;1;35;95m\\\033[0m \033[0;1;31;91m'_\033[0;1;33;93m_/\033[0m \033[0;1;32;92m_\033[0m "
                  "\033[0;1;36;96m\\\033[0;1;34;94m/\033[0m \033[0;1;35;95m__\033[0;1;31;91m|\033[0m "
                  "\033[0;1;33;93m__\033[0;1;32;92m|\033[0m";
    EVLOG_info << " \033[0;1;36;96m|\033[0m \033[0;1;34;94m|_\033[0;1;35;95m__\033[0;1;31;91m_\033[0m   "
                  "\033[0;1;32;92m\\\033[0m  \033[0;1;36;96m/\033[0m  \033[0;1;35;95m__\033[0;1;31;91m/\033[0m "
                  "\033[0;1;33;93m|\033[0m \033[0;1;32;92m|\033[0m  "
                  "\033[0;1;36;96m_\033[0;1;34;94m_/\033[0;1;35;95m\\_\033[0;1;31;91m_\033[0m \033[0;1;33;93m\\\033[0m "
                  "\033[0;1;32;92m|_\033[0m";
    EVLOG_info << " \033[0;1;34;94m|_\033[0;1;35;95m__\033[0;1;31;91m__\033[0;1;33;93m_|\033[0m   "
                  "\033[0;1;36;96m\\\033[0;1;34;94m/\033[0m "
                  "\033[0;1;35;95m\\_\033[0;1;31;91m__\033[0;1;33;93m|_\033[0;1;32;92m|\033[0m  "
                  "\033[0;1;36;96m\\\033[0;1;34;94m__\033[0;1;35;95m_|\033[0;1;31;91m|_\033[0;1;33;93m__\033[0;1;32;"
                  "92m/\\\033[0;1;36;96m__\033[0;1;34;94m|\033[0m";
    EVLOG_info << "";
    EVLOG_info << PROJECT_NAME << " " << PROJECT_VERSION << " " << GIT_VERSION;
    EVLOG_info << ms.version_information;
    EVLOG_info << "";

    if (not ms.mqtt_settings.uses_socket()) {
        EVLOG_info << "Using MQTT broker " << ms.mqtt_settings.broker_host << ":" << ms.mqtt_settings.broker_port;
    } else {
        EVLOG_info << "Using MQTT broker unix domain sockets:" << ms.mqtt_settings.broker_socket_path;
    }
    if (ms.runtime_settings.telemetry_enabled) {
        EVLOG_info << "Telemetry enabled";
    }
    if (not ms.run_as_user.empty()) {
        EVLOG_info << "EVerest will run as system user: " << ms.run_as_user;
    }

#ifdef ENABLE_ADMIN_PANEL
    auto controller_handle = start_controller(ms);
#endif

    EVLOG_verbose << fmt::format("EVerest prefix was set to {}", ms.runtime_settings.prefix.string());

    // dump all manifests if requested and terminate afterwards
    if (vm.count("dumpmanifests")) {
        const auto dumpmanifests_path = fs::path(vm["dumpmanifests"].as<std::string>());
        EVLOG_debug << fmt::format("Dumping all known validated manifests into '{}'", dumpmanifests_path.string());

        auto manifests = Config::load_all_manifests(ms.runtime_settings.modules_dir.string(), ms.schemas_dir.string());

        for (const auto& module : manifests.items()) {
            const std::string filename = module.key() + ".yaml";
            const auto module_output_path = dumpmanifests_path / filename;
            // FIXME (aw): should we check if the directory exists?
            std::ofstream output_stream(module_output_path);

            // FIXME (aw): this should be either YAML prettyfied, or better, directly copied
            output_stream << module.value().dump(DUMP_INDENT);
        }

        return EXIT_SUCCESS;
    }

    const bool retain_topics = (vm.count("retain-topics") != 0);

    const auto start_time = std::chrono::system_clock::now();
    std::unique_ptr<ManagerConfig> config;
    try {
        config = std::make_unique<ManagerConfig>(ms);
    } catch (EverestInternalError& e) {
        EVLOG_error << fmt::format("Failed to load and validate config!\n{}", boost::diagnostic_information(e, true));
        return EXIT_FAILURE;
    } catch (boost::exception& e) {
        EVLOG_error << "Failed to load and validate config!";
        EVLOG_critical << fmt::format("Caught top level boost::exception:\n{}", boost::diagnostic_information(e, true));
        return EXIT_FAILURE;
    } catch (std::exception& e) {
        EVLOG_error << "Failed to load and validate config!";
        EVLOG_critical << fmt::format("Caught top level std::exception:\n{}", boost::diagnostic_information(e, true));
        return EXIT_FAILURE;
    }
    const auto end_time = std::chrono::system_clock::now();
    EVLOG_info << "Config loading completed in "
               << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms";

    // dump config if requested
    if (vm.count("dump")) {
        const auto dump_path = fs::path(vm["dump"].as<std::string>());
        EVLOG_debug << fmt::format("Dumping validated config and manifests into '{}'", dump_path.string());

        const auto config_dump_path = dump_path / "config.json";

        std::ofstream output_config_stream(config_dump_path);

        output_config_stream << json(config->get_module_configurations()).dump(DUMP_INDENT);

        const auto manifests = config->get_manifests();

        for (const auto& module : manifests.items()) {
            const std::string filename = module.key() + ".json";
            const auto module_output_path = dump_path / filename;
            std::ofstream output_stream(module_output_path);

            output_stream << module.value().dump(DUMP_INDENT);
        }
    }

    // only config check (and or config dumping) was requested, log check result and exit
    if (check) {
        EVLOG_debug << "Config is valid, terminating as requested";
        return EXIT_SUCCESS;
    }

    std::vector<std::string> standalone_modules;
    if (vm.count("standalone")) {
        standalone_modules = vm["standalone"].as<std::vector<std::string>>();
    }

    const auto& module_configurations = config->get_module_configurations();
    for (const auto& [module_id, module_config] : module_configurations) {
        // check if standalone parameter is set
        if (module_config.standalone) {
            if (std::find(standalone_modules.begin(), standalone_modules.end(), module_id) ==
                standalone_modules.end()) {
                EVLOG_info << "Module " << fmt::format(TERMINAL_STYLE_BLUE, "{}", module_id)
                           << " marked as standalone in config";

                standalone_modules.push_back(module_id);
            }
        }
    }

    std::vector<std::string> ignored_modules;
    if (vm.count("ignore")) {
        ignored_modules = vm["ignore"].as<std::vector<std::string>>();
    }

    // create StatusFifo object
    auto status_fifo = StatusFifo::create_from_path(vm["status-fifo"].as<std::string>());

    auto mqtt_abstraction = MQTTAbstraction(ms.mqtt_settings);

    if (!mqtt_abstraction.connect()) {
        if (not ms.mqtt_settings.uses_socket()) {
            EVLOG_error << fmt::format("Cannot connect to MQTT broker at {}:{}", ms.mqtt_settings.broker_host,
                                       ms.mqtt_settings.broker_port);
        } else {
            EVLOG_error << fmt::format("Cannot connect to MQTT broker socket at {}",
                                       ms.mqtt_settings.broker_socket_path);
        }
        return EXIT_FAILURE;
    }

    mqtt_abstraction.spawn_main_loop_thread();

    auto module_handles =
        start_modules(*config, mqtt_abstraction, ignored_modules, standalone_modules, ms, status_fifo, retain_topics);
    bool modules_started = true;
    bool restart_modules = false;

#ifndef ENABLE_ADMIN_PANEL
    // switch to low privilege user if configured
    if (not ms.run_as_user.empty()) {
        auto err_set_user = system::set_real_user(ms.run_as_user);
        if (not err_set_user.empty()) {
            EVLOG_error << "Error switching manager to user " << ms.run_as_user << ": " << err_set_user;
            return EXIT_FAILURE;
        }
    }
#endif

    int wstatus; // NOLINT(cppcoreguidelines-init-variables): this is always initialized in the following waitpid call

    while (true) {
// check if anyone died
#ifdef ENABLE_ADMIN_PANEL
        // non-blocking if admin panel is enabled, as this main loop also processes controller RPC
        auto pid = waitpid(-1, &wstatus, WNOHANG);
#else
        // block if admin panel is disabled, no controller RPC is handled by main loop
        auto pid = waitpid(-1, &wstatus, 0);
#endif

        if (pid == 0) {
            // nothing new from our child process
        } else if (pid == -1) {
            throw std::runtime_error(fmt::format("Syscall to waitpid() failed ({})", strerror(errno)));
        } else {

#ifdef ENABLE_ADMIN_PANEL
            // one of our children exited (first check controller, then modules)
            if (pid == controller_handle.pid) {
                // FIXME (aw): what to do, if the controller exited? Restart it?
                throw std::runtime_error("The controller process exited.");
            }
#endif

            const auto module_iter = module_handles.find(pid);
            if (module_iter == module_handles.end()) {
                throw std::runtime_error(fmt::format("Unkown child width pid ({}) died.", pid));
            }

            const auto module_name = module_iter->second;
            module_handles.erase(module_iter);
            // one of our modules died -> kill 'em all
            if (modules_started) {
                EVLOG_critical << fmt::format("Module {} (pid: {}) exited with status: {}. Terminating all modules.",
                                              module_name, pid, wstatus);
                shutdown_modules(module_handles, *config, mqtt_abstraction);

                mqtt_abstraction.clear_retained_topics();

                // Exit if a module died, this gives systemd a change to restart manager
                EVLOG_critical << "Exiting manager.";
                return EXIT_FAILURE;
            } else {
                EVLOG_info << fmt::format("Module {} (pid: {}) exited with status: {}.", module_name, pid, wstatus);
            }
        }

#ifdef ENABLE_ADMIN_PANEL
        if (module_handles.size() == 0 && restart_modules) {
            module_handles = start_modules(*config, mqtt_abstraction, ignored_modules, standalone_modules, ms,
                                           status_fifo, retain_topics);
            restart_modules = false;
            modules_started = true;
        }

        // check for news from the controller
        const auto msg = controller_handle.receive_message();
        if (msg.status == controller_ipc::MESSAGE_RETURN_STATUS::OK) {
            // FIXME (aw): implement all possible messages here, for now just log them
            const auto& payload = msg.json;
            if (payload.at("method") == "restart_modules") {
                shutdown_modules(module_handles, *config, mqtt_abstraction);
                config = std::make_unique<ManagerConfig>(ms);
                modules_started = false;
                restart_modules = true;
            } else if (payload.at("method") == "check_config") {
                const std::string check_config_file_path = payload.at("params");

                try {
                    // check the config
                    auto cfg = ManagerConfig(ManagerSettings(prefix_opt, check_config_file_path));
                    controller_handle.send_message({{"id", payload.at("id")}});
                } catch (const std::exception& e) {
                    controller_handle.send_message({{"result", e.what()}, {"id", payload.at("id")}});
                }
            } else {
                // unknown payload
                EVLOG_error << fmt::format("Received unkown command via controller ipc:\n{}\n... ignoring",
                                           payload.dump(DUMP_INDENT));
            }
        } else if (msg.status == controller_ipc::MESSAGE_RETURN_STATUS::ERROR) {
            fmt::print("Error in IPC communication with controller: {}\nExiting\n", msg.json.at("error").dump(2));
            return EXIT_FAILURE;
        } else {
            // TIMEOUT fall-through
        }
#endif
    }

    return EXIT_SUCCESS;
}
} // namespace

int main(int argc, char* argv[]) {
    po::options_description desc("EVerest manager");
    desc.add_options()("version", "Print version and exit");
    desc.add_options()("help,h", "produce help message");
    desc.add_options()("check", "Check and validate all config files and exit (0=success)");
    desc.add_options()("dump", po::value<std::string>(),
                       "Dump validated and augmented main config and all used module manifests into dir");
    desc.add_options()("dumpmanifests", po::value<std::string>(),
                       "Dump manifests of all modules into dir (even modules not used in config) and exit");
    desc.add_options()("prefix", po::value<std::string>(), "Prefix path of everest installation");
    desc.add_options()("standalone,s", po::value<std::vector<std::string>>()->multitoken(),
                       "Module ID(s) to not automatically start child processes for (those must be started manually to "
                       "make the framework start!).");
    desc.add_options()("ignore", po::value<std::vector<std::string>>()->multitoken(),
                       "Module ID(s) to ignore: Do not automatically start child processes and do not require that "
                       "they are started.");
    desc.add_options()("dontvalidateschema", "Don't validate json schema on every message");
    desc.add_options()("config", po::value<std::string>(),
                       "Full path to a config file.  If the file does not exist and has no extension, it will be "
                       "looked up in the default config directory");
    desc.add_options()("db", po::value<std::string>(), "Full path to the configuration database file");
    desc.add_options()("db-init", "Indicator to initialize the database if it does not contain a valid configuration. "
                                  "Requires --config and --db to be set.");
    desc.add_options()("status-fifo", po::value<std::string>()->default_value(""),
                       "Path to a named pipe, that shall be used for status updates from the manager");
    desc.add_options()("retain-topics", "Retain configuration MQTT topics setup by manager for inspection, by default "
                                        "these will be cleared after startup");

    po::variables_map vm;

    try {
        const auto default_logging_cfg =
            defaults::PREFIX / fs::path(defaults::SYSCONF_DIR) / defaults::NAMESPACE / defaults::LOGGING_CONFIG_NAME;
        if (fs::exists(default_logging_cfg)) {
            Logging::init(default_logging_cfg.string());
        }
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help") != 0) {
            desc.print(std::cout);
            return EXIT_SUCCESS;
        }

        if (vm.count("version") != 0) {
            std::string argv0;
            if (argc > 0) {
                argv0 = *argv;
            }
            std::cout << argv0 << " (" << PROJECT_NAME << " " << PROJECT_VERSION << " " << GIT_VERSION << ") "
                      << std::endl;
            return EXIT_SUCCESS;
        }

        return boot(vm);

    } catch (const BootException& e) {
        EVLOG_error << "Failed to start up everest:\n" << e.what();
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        EVLOG_error << "Main manager process exits because of caught exception:\n" << e.what();
        return EXIT_FAILURE;
    }
}
