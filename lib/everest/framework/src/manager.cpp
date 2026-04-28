// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>

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

#include "manager.hpp"
#include "manager_admin_panel.hpp"
#include "system_unix.hpp"
#include <generated/version_information.hpp>

namespace po = boost::program_options;
namespace fs = std::filesystem;

using namespace Everest;

const auto PARENT_DIED_SIGNAL = SIGTERM;
const auto SHUTDOWN_TIMEOUT_MS = 5000;
const std::uint8_t MAX_UNEXPECTED_MODULE_RESTARTS = 3;
const auto complete_start_time = std::chrono::system_clock::now();

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

// File-local helpers and spawn/runtime utilities.
// Keep these outside Manager when they are stateless, process-exec focused, and
// not part of manager state-machine behavior:
// - wait status formatting/inspection
// - module process environment setup and exec wrappers
// - module spawning helpers

/// \brief Convert a wait status code to a compact readable string.
std::string format_wait_status(int status) {
    if (WIFEXITED(status)) {
        return fmt::format("exit_code={}", WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status)) {
        return fmt::format("signal={}", WTERMSIG(status));
    }
    return fmt::format("raw={}", status);
}

/// \brief Return true if process exited normally with code 0.
bool is_clean_exit(int status) {
    return WIFEXITED(status) && (WEXITSTATUS(status) == 0);
}

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

/// \brief Execute a module process and report exec failures to parent.
static void exec_module_binary(const std::string& bin, std::vector<std::string>& arguments,
                               system::SubProcess& proc_handle) {
    // Convert the argument list to the format required by `execv*()`.
    std::vector<char*> argv_list(arguments.size() + 1);
    std::transform(arguments.begin(), arguments.end(), argv_list.begin(), [](auto& value) { return value.data(); });
    argv_list.back() = nullptr; // Add a null terminator

    // Execute the module binary, replacing the current process.
    execvp(bin.c_str(), argv_list.data());

    // `execv()` failed, notify the parent process and exit.
    const auto msg = fmt::format("Syscall to execv() with \"{} {}\" failed ({})", bin,
                                 fmt::join(arguments.begin() + 1, arguments.end(), " "), strerror(errno));
    proc_handle.send_error_and_exit(msg);
}

/// \brief Build argv and execute a C++ module binary.
void exec_cpp_module(system::SubProcess& proc_handle, const ModuleStartInfo& module_info, const RuntimeSettings& rs,
                     const MQTTSettings& mqtt_settings) {
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

    exec_module_binary(module_info.path.string(), arguments, proc_handle);
}

/// \brief Prepare environment and execute a JavaScript module via node.
void exec_javascript_module(system::SubProcess& proc_handle, const ModuleStartInfo& module_info,
                            const RuntimeSettings& rs, const MQTTSettings& mqtt_settings) {
    // FIXME (aw): everest directory layout
    const auto node_modules_path = rs.prefix / defaults::LIB_DIR / defaults::NAMESPACE / "node_modules";
    setenv("NODE_PATH", node_modules_path.c_str(), 0);
    setup_environment(module_info, rs, mqtt_settings);

    std::vector<std::string> arguments = {
        "node",
        "--unhandled-rejections=strict",
        module_info.path.string(),
    };

    exec_module_binary("node", arguments, proc_handle);
}

/// \brief Prepare environment and execute a Python module.
void exec_python_module(system::SubProcess& proc_handle, const ModuleStartInfo& module_info, const RuntimeSettings& rs,
                        const MQTTSettings& mqtt_settings) {
    setup_environment(module_info, rs, mqtt_settings);

    // Prepend the everestpy path to $PYTHONPATH. This ensures modules can always find everestpy.
    const auto everestpy_path = rs.prefix / defaults::LIB_DIR / defaults::NAMESPACE / "everestpy";
    if (const auto prev_pythonpath = std::getenv("PYTHONPATH")) {
        const auto pythonpath = fmt::format("{}:{}", everestpy_path.string(), prev_pythonpath);
        setenv("PYTHONPATH", pythonpath.c_str(), 1);
    } else {
        setenv("PYTHONPATH", everestpy_path.c_str(), 1);
    }

    std::string python_binary = "python3";

    // Check if a virtual environment exists in the module directory, and if so use its python runtime.
    const auto venv_dir = module_info.path.parent_path() / ".venv";
    if (fs::exists(venv_dir)) {
        const auto venv_bin_dir = venv_dir / "bin";
        const auto venv_python = venv_bin_dir / "python3";
        if (fs::exists(venv_python)) {
            // Activate the virtual environment. This approximates the behaviour of the `.venv/bin/activate` script.
            python_binary = venv_python.string();
            setenv("VIRTUAL_ENV", venv_dir.c_str(), 1);
            setenv("VIRTUAL_ENV_PROMPT", "venv", 1);
            unsetenv("PYTHONHOME");

            if (const auto prev_path = std::getenv("PATH")) {
                const auto path = fmt::format("{}:{}", venv_bin_dir.string(), prev_path);
                setenv("PATH", path.c_str(), 1);
            } else {
                setenv("PATH", venv_bin_dir.c_str(), 1);
            }
        }
    }

    std::vector<std::string> arguments = {python_binary, module_info.path.c_str()};
    exec_module_binary(python_binary, arguments, proc_handle);
}

/// \brief Dispatch module execution to the language-specific executor.
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

/// \brief Spawn configured module processes and return pid-to-module mapping.
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

/// \brief Publish startup metadata, register handlers, and spawn module processes.
void Manager::publishStartupMetadata(const RuntimeContext& ctx) const {
    const auto& config = *ctx.config;
    auto& mqtt_abstraction = ctx.mqtt_abstraction;
    const auto& ms = ctx.ms;

    const auto interface_definitions = config.get_interface_definitions();
    std::vector<std::string> interface_names;
    for (auto& interface_definition : interface_definitions.items()) {
        interface_names.push_back(interface_definition.key());
    }

    MqttMessagePayload payload{MqttMessageType::GetConfigResponse, interface_names};
    mqtt_abstraction.publish(fmt::format("{}interfaces", ms.mqtt_settings.everest_prefix), payload, QOS::QOS2, true);

    for (const auto& interface_definition : interface_definitions.items()) {
        MqttMessagePayload interface_definition_payload{MqttMessageType::GetConfigResponse,
                                                        interface_definition.value()};
        mqtt_abstraction.publish(
            fmt::format("{}interface_definitions/{}", ms.mqtt_settings.everest_prefix, interface_definition.key()),
            interface_definition_payload, QOS::QOS2, true);
    }

    const auto type_definitions = config.get_types();
    std::vector<std::string> type_names;
    for (auto& type_definition : type_definitions.items()) {
        type_names.push_back(type_definition.key());
    }

    MqttMessagePayload type_names_payload{MqttMessageType::GetConfigResponse, type_names};
    mqtt_abstraction.publish(fmt::format("{}types", ms.mqtt_settings.everest_prefix), type_names_payload, QOS::QOS2,
                             true);
    for (const auto& type_definition : type_definitions.items()) {
        MqttMessagePayload type_definition_payload{MqttMessageType::GetConfigResponse, type_definition.value()};
        // type_definition keys already start with a / so omit it in the topic name
        mqtt_abstraction.publish(
            fmt::format("{}type_definitions{}", ms.mqtt_settings.everest_prefix, type_definition.key()),
            type_definition_payload, QOS::QOS2, true);
    }

    const auto settings = config.get_settings();
    MqttMessagePayload settings_payload{MqttMessageType::GetConfigResponse, settings};
    mqtt_abstraction.publish(fmt::format("{}settings", ms.mqtt_settings.everest_prefix), settings_payload, QOS::QOS2,
                             true);

    if (ms.runtime_settings.validate_schema) {
        const auto schemas = config.get_schemas();
        MqttMessagePayload schemas_payload{MqttMessageType::GetConfigResponse, schemas};
        mqtt_abstraction.publish(fmt::format("{}schemas", ms.mqtt_settings.everest_prefix), schemas_payload, QOS::QOS2,
                                 true);
    }

    const auto manifests = config.get_manifests();
    for (const auto& manifest : manifests.items()) {
        auto manifest_copy = manifest.value();
        manifest_copy.erase("config");
        MqttMessagePayload manifest_payload{MqttMessageType::GetConfigResponse, manifest_copy};
        mqtt_abstraction.publish(fmt::format("{}manifests/{}", ms.mqtt_settings.everest_prefix, manifest.key()),
                                 manifest_payload, QOS::QOS2, true);
    }

    const auto module_names = config.get_module_names();
    MqttMessagePayload module_names_payload{MqttMessageType::GetConfigResponse, module_names};
    mqtt_abstraction.publish(fmt::format("{}module_names", ms.mqtt_settings.everest_prefix), module_names_payload,
                             QOS::QOS2, true);
}

/// \brief Unregister all module ready handlers and clear tracked ready state.
void Manager::unregisterModuleReadyHandlers(ManagerConfig& config, MQTTAbstraction& mqtt_abstraction) {
    ModulesReadyType modules_ready_moved;
    {
        const std::lock_guard<std::mutex> lck(modules_ready_mutex_);
        modules_ready_moved = std::move(modules_ready_);
        // Probably not needed after our move but lets be explicit.
        modules_ready_.clear();
    }

    for (const auto& module : modules_ready_moved) {
        const auto& ready_info = module.second;
        const auto& module_name = module.first;
        const std::string topic = fmt::format("{}/ready", config.mqtt_module_prefix(module_name));
        mqtt_abstraction.unregister_handler(topic, ready_info.ready_token);
    }
}

/// \brief Stop all remaining module processes, escalating SIGTERM to SIGKILL.
void Manager::shutdownModules(const std::map<pid_t, std::string>& modules, ManagerConfig& config,
                              MQTTAbstraction& mqtt_abstraction) {

    unregisterModuleReadyHandlers(config, mqtt_abstraction);

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

namespace {

/// \brief Select configuration boot mode from CLI options.
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

/// \brief Disconnect MQTT and perform manager process cleanup.
void cleanup(MQTTAbstraction& mqtt_abstraction) {
    mqtt_abstraction.disconnect();
}

/// \brief Print startup banner and version information.
void print_start_message(const std::string& version_information) {
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
    EVLOG_info << version_information;
    EVLOG_info << "";
}

/// \brief Print final shutdown message including elapsed shutdown duration.
void print_shutdown_message(const std::optional<std::chrono::system_clock::time_point> shutdown_start_time,
                            const std::string& message_prefix = "") {
    auto shutdown_duration = 0LL;
    if (shutdown_start_time.has_value()) {
        shutdown_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() -
                                                                                  shutdown_start_time.value())
                                .count();
    } else {
        EVLOG_info << "shutdown start time is not set?";
    }
    EVLOG_info << fmt::format(
        TERMINAL_STYLE_ERROR, "👋👋👋 {}{}", message_prefix,
        fmt::format(TERMINAL_STYLE_ERROR, "EVerest manager is exiting [{}ms] 👋👋👋", shutdown_duration));
}

} // namespace

// ---- State/event handlers ---------------------------------------------------

void Manager::handleRestartModulesAfterShutdown(RuntimeContext& ctx) {
    unregisterModuleReadyHandlers(*ctx.config, ctx.mqtt_abstraction);
    ctx.mqtt_abstraction.clear_retained_topics();

    ctx.config = std::make_shared<ManagerConfig>(ctx.ms);
    module_handles_ = handleStartModules(ctx);
    shutdown_cause_ = ShutdownCause::None;
    shutdown_start_time_ = std::nullopt;
    shutdown_info_.clear();
    EVLOG_info << "Modules restart initiated with reloaded configuration.";
}

std::optional<int> Manager::handleFinishNormalShutdown(MQTTAbstraction& mqtt_abstraction,
                                                       ManagerAdminPanel& admin_panel) {
    std::string bad_modules;
    for (const auto& shutdown_info_entry : shutdown_info_) {
        if (!is_clean_exit(shutdown_info_entry.wstatus)) {
            bad_modules +=
                fmt::format(" {} ({})", shutdown_info_entry.id, format_wait_status(shutdown_info_entry.wstatus));
        }
    }
    if (sigint_received_) {
        if (bad_modules.empty()) {
            print_shutdown_message(shutdown_start_time_,
                                   fmt::format(TERMINAL_STYLE_OK, "All modules shut down properly. "));
        } else {
            EVLOG_warning << "Modules that did not shut down cleanly:" << bad_modules;
            print_shutdown_message(shutdown_start_time_);
        }
        admin_panel.shutdown_controller();
        cleanup(mqtt_abstraction);
        shutdown_cause_ = ShutdownCause::None;
        transitionTo(ManagerState::Exiting);
        return EXIT_SUCCESS;
    }

    if (!bad_modules.empty()) {
        EVLOG_warning << "Modules that did not shut down cleanly:" << bad_modules;
    }
    shutdown_info_.clear();
    shutdown_start_time_ = std::nullopt;
    mqtt_abstraction.clear_retained_topics();
    shutdown_cause_ = ShutdownCause::None;
    transitionTo(ManagerState::Idle);
    EVLOG_info << "Manager is idle after module shutdown. Send SIGINT/SIGTERM to stop.";
    return std::nullopt;
}

void Manager::handleFinishCrashRecovery(MQTTAbstraction& mqtt_abstraction) {
    const auto duration_ms = shutdown_start_time_.has_value()
                                 ? std::chrono::duration_cast<std::chrono::milliseconds>(
                                       std::chrono::system_clock::now() - shutdown_start_time_.value())
                                       .count()
                                 : 0;
    std::string bad_modules;
    for (const auto& info : shutdown_info_) {
        if (!is_clean_exit(info.wstatus)) {
            bad_modules += fmt::format(" {} ({})", info.id, format_wait_status(info.wstatus));
        }
    }
    if (bad_modules.empty()) {
        EVLOG_info << fmt::format("All {} modules shut down gracefully after crash [{}ms].", shutdown_info_.size(),
                                  duration_ms);
    } else {
        EVLOG_warning << fmt::format(
            "Crash recovery shutdown completed in {} ms. Modules that did not shut down cleanly: {}", duration_ms,
            bad_modules);
    }

    mqtt_abstraction.clear_retained_topics();
    shutdown_info_.clear();
    shutdown_start_time_ = std::nullopt;
    shutdown_cause_ = ShutdownCause::None;

    {
        const std::lock_guard<std::mutex> lock(modules_ready_mutex_);
        modules_ready_.clear();
    }

    transitionTo(ManagerState::Idle);
    EVLOG_info << "Crash recovery completed, manager is idle after module shutdown. Send SIGINT/SIGTERM to stop.";
}

std::optional<int> Manager::handleFinalizeShutdownTransition(RuntimeContext& ctx, ManagerAdminPanel& admin_panel,
                                                             bool restart_requested, bool crash_in_progress) {
    if (crash_in_progress) {
        handleFinishCrashRecovery(ctx.mqtt_abstraction);
        return std::nullopt;
    }
    if (restart_requested) {
        handleRestartModulesAfterShutdown(ctx);
        return std::nullopt;
    }
    return handleFinishNormalShutdown(ctx.mqtt_abstraction, admin_panel);
}

void Manager::handleInitiateGracefulShutdown(const std::chrono::system_clock::time_point& module_exited_time,
                                             bool publish_when_sigint_received, const std::string* info_log,
                                             MQTTAbstraction& mqtt_abstraction, const ManagerSettings& ms) {
    if (isInShutdownFlowState()) {
        return;
    }
    transitionTo(ManagerState::ShutdownRequested);
    shutdown_start_time_ = module_exited_time;
    if (publish_when_sigint_received || not sigint_received_) {
        if (info_log != nullptr) {
            EVLOG_critical << *info_log;
        }
        mqtt_abstraction.publish(fmt::format("{}shutdown", ms.mqtt_settings.everest_prefix), std::string("true"),
                                 QOS::QOS2, false);
    }
}

// ---- Core run loop ----------------------------------------------------------

int Manager::run() {
    const bool check = (vm_.count("check") != 0);
    sigint_received_ = false;
    shutdown_cause_ = ShutdownCause::None;
    transitionTo(ManagerState::Initializing);
    unexpected_module_exit_count_ = 0;
    shutdown_start_time_ = std::nullopt;
    auto signal_polling = system::SignalPolling();

    const auto prefix_opt = parse_string_option(vm_, "prefix");
    const auto config_opt = parse_string_option(vm_, "config");
    const auto db_opt = parse_string_option(vm_, "db");
    const auto db_init = vm_.count("db-init") != 0;
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

    // CLI override for mqtt_everest_prefix (e.g. for parallel test execution).
    if (vm_.count("mqtt_everest_prefix") != 0) {
        auto prefix = vm_["mqtt_everest_prefix"].as<std::string>();
        if (!prefix.empty() && prefix.back() != '/') {
            prefix += "/";
        }
        ms.mqtt_settings.everest_prefix = prefix;
    }

    Logging::init(ms.runtime_settings.logging_config_file.string());

    print_start_message(ms.version_information);

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
    if (ms.runtime_settings.forward_exceptions) {
        EVLOG_info << "Catching and forwarding command exceptions to callers";
    }

    auto admin_panel = ManagerAdminPanel::create(ms);

    EVLOG_verbose << fmt::format("EVerest prefix was set to {}", ms.runtime_settings.prefix.string());

    // dump all manifests if requested and terminate afterwards
    if (vm_.count("dumpmanifests")) {
        const auto dumpmanifests_path = fs::path(vm_["dumpmanifests"].as<std::string>());
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

    const bool retain_topics = (vm_.count("retain-topics") != 0);

    std::shared_ptr<ManagerConfig> config; // TODO: maybe this can stay unique when we re-work start_modules()
    try {
        config = loadAndValidateConfig(ms);
    } catch (...) {
        return EXIT_FAILURE;
    }

    // dump config if requested
    if (vm_.count("dump")) {
        const auto dump_path = fs::path(vm_["dump"].as<std::string>());
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

    std::vector<std::string> standalone_modules = collectStandaloneModules(*config);
    std::vector<std::string> ignored_modules = collectIgnoredModules();

    // create StatusFifo object
    auto status_fifo = StatusFifo::create_from_path(vm_["status-fifo"].as<std::string>());

    auto mqtt_abstraction = createAndConnectMqtt(ms);
    if (!mqtt_abstraction) {
        return EXIT_FAILURE;
    }

    auto config_service = std::make_unique<config::ConfigService>(*mqtt_abstraction, config);

    RuntimeContext runtime_ctx{config, *mqtt_abstraction, ignored_modules, standalone_modules,
                               ms,     status_fifo,       retain_topics};
    module_handles_ = handleStartModules(runtime_ctx);

    if (const auto err_set_user = ManagerAdminPanel::switch_manager_user_if_needed(runtime_ctx.ms)) {
        EVLOG_error << "Error switching manager to user " << runtime_ctx.ms.run_as_user << ": " << *err_set_user;
        return EXIT_FAILURE;
    }

    int wstatus; // NOLINT(cppcoreguidelines-init-variables): this is always initialized in the following waitpid call
    shutdown_info_.clear();

    while (true) {
        if (handleWaitpidEvent(wstatus, runtime_ctx, admin_panel)) {
            continue;
        }

        const auto lifecycle_advance = advanceLifecycleStateIfReady(runtime_ctx, admin_panel);
        if (lifecycle_advance.status == LifecycleAdvanceResult::Status::ExitRequested) {
            return *lifecycle_advance.exit_code;
        }
        if (lifecycle_advance.status == LifecycleAdvanceResult::Status::TransitionApplied) {
            continue;
        }

        if (const auto exit_from_panel = handleControllerIpcPoll(runtime_ctx, admin_panel, prefix_opt)) {
            return *exit_from_panel;
        }
        if (const auto exit_from_signal = handleSignalPoll(signal_polling, runtime_ctx, admin_panel)) {
            return *exit_from_signal;
        }

        handleShutdownTimeout(runtime_ctx);
    }

    return EXIT_SUCCESS;
}

// ---- Setup/helpers ----------------------------------------------------------

const char* Manager::stateToString(ManagerState state) const {
    switch (state) {
    case ManagerState::Initializing:
        return "Initializing";
    case ManagerState::StartingModules:
        return "StartingModules";
    case ManagerState::Running:
        return "Running";
    case ManagerState::RestartRequested:
        return "RestartRequested";
    case ManagerState::CrashShutdownInProgress:
        return "CrashShutdownInProgress";
    case ManagerState::ShutdownRequested:
        return "ShutdownRequested";
    case ManagerState::ForceTerminating:
        return "ForceTerminating";
    case ManagerState::ShutdownFinalizing:
        return "ShutdownFinalizing";
    case ManagerState::Idle:
        return "Idle";
    case ManagerState::Exiting:
        return "Exiting";
    default:
        return "Unknown";
    }
}

std::shared_ptr<ManagerConfig> Manager::loadAndValidateConfig(const ManagerSettings& ms) const {
    const auto start_time = std::chrono::system_clock::now();
    std::shared_ptr<ManagerConfig> config;
    try {
        config = std::make_shared<ManagerConfig>(ms);
    } catch (EverestInternalError& e) {
        EVLOG_error << fmt::format("Failed to load and validate config!\n{}", boost::diagnostic_information(e, true));
        throw;
    } catch (boost::exception& e) {
        EVLOG_error << "Failed to load and validate config!";
        EVLOG_critical << fmt::format("Caught top level boost::exception:\n{}", boost::diagnostic_information(e, true));
        throw;
    } catch (std::exception& e) {
        EVLOG_error << "Failed to load and validate config!";
        EVLOG_critical << fmt::format("Caught top level std::exception:\n{}", boost::diagnostic_information(e, true));
        throw;
    }
    const auto end_time = std::chrono::system_clock::now();
    EVLOG_info << "Config loading completed in "
               << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms";
    return config;
}

std::unique_ptr<MQTTAbstraction> Manager::createAndConnectMqtt(const ManagerSettings& ms) const {
    auto mqtt_abstraction = make_mqtt_abstraction(ms.mqtt_settings);
    if (!mqtt_abstraction->connect()) {
        if (not ms.mqtt_settings.uses_socket()) {
            EVLOG_error << fmt::format("Cannot connect to MQTT broker at {}:{}", ms.mqtt_settings.broker_host,
                                       ms.mqtt_settings.broker_port);
        } else {
            EVLOG_error << fmt::format("Cannot connect to MQTT broker socket at {}",
                                       ms.mqtt_settings.broker_socket_path);
        }
        return nullptr;
    }

    mqtt_abstraction->spawn_main_loop_thread();
    return mqtt_abstraction;
}

std::vector<std::string> Manager::collectStandaloneModules(const ManagerConfig& config) const {
    std::vector<std::string> standalone_modules;
    if (vm_.count("standalone")) {
        standalone_modules = vm_["standalone"].as<std::vector<std::string>>();
    }

    const auto& module_configurations = config.get_module_configurations();
    for (const auto& [module_id, module_config] : module_configurations) {
        if (!module_config.standalone) {
            continue;
        }
        if (std::find(standalone_modules.begin(), standalone_modules.end(), module_id) == standalone_modules.end()) {
            EVLOG_info << "Module " << fmt::format(TERMINAL_STYLE_BLUE, "{}", module_id)
                       << " marked as standalone in config";
            standalone_modules.push_back(module_id);
        }
    }

    return standalone_modules;
}

std::vector<std::string> Manager::collectIgnoredModules() const {
    if (vm_.count("ignore")) {
        return vm_["ignore"].as<std::vector<std::string>>();
    }
    return {};
}

void Manager::transitionTo(ManagerState new_state) {
    if (state_ == new_state) {
        return;
    }
    EVLOG_info << "Manager state transition: " << stateToString(state_) << " -> " << stateToString(new_state);
    state_ = new_state;
}

Manager::Manager(const po::variables_map& vm) : vm_(vm) {
}

// ---- State predicates -------------------------------------------------------

bool Manager::isInShutdownFlowState() const {
    return (state_ == ManagerState::ShutdownRequested) || (state_ == ManagerState::CrashShutdownInProgress) ||
           (state_ == ManagerState::ForceTerminating) || (state_ == ManagerState::RestartRequested) ||
           (state_ == ManagerState::ShutdownFinalizing);
}

bool Manager::isRestartRequested() const {
    return state_ == ManagerState::RestartRequested;
}

bool Manager::isCrashInProgress() const {
    return state_ == ManagerState::CrashShutdownInProgress;
}

bool Manager::areModulesStarted() const {
    return state_ == ManagerState::Running;
}

// ---- Event loop dispatch handlers ------------------------------------------

/// \brief Handle module startup by publishing metadata, registering handlers, and spawning module processes.
std::map<pid_t, std::string> Manager::handleStartModules(const RuntimeContext& ctx) {
    BOOST_LOG_FUNCTION();
    transitionTo(ManagerState::StartingModules);
    auto& config = *ctx.config;
    auto& mqtt_abstraction = ctx.mqtt_abstraction;
    const auto& ignored_modules = ctx.ignored_modules;
    const auto& standalone_modules = ctx.standalone_modules;
    const auto& ms = ctx.ms;
    auto& status_fifo = ctx.status_fifo;
    const bool retain_topics = ctx.retain_topics;

    std::vector<ModuleStartInfo> modules_to_spawn;

    const auto& module_configurations = config.get_module_configurations();
    modules_to_spawn.reserve(module_configurations.size());
    const auto number_of_modules = module_configurations.size();
    EVLOG_info << "Starting " << number_of_modules << " modules";

    publishStartupMetadata(ctx);

    for (const auto& [module_id_, module_config] : module_configurations) {
        const auto& module_name = module_config.module_name;
        const auto& module_id = module_id_;
        if (std::any_of(ignored_modules.begin(), ignored_modules.end(),
                        [module_id](const auto& element) { return element == module_id; })) {
            EVLOG_info << fmt::format("Ignoring module: {}", module_id);
            continue;
        }

        auto module_it = modules_ready_.emplace(module_id, ModuleReadyInfo{}).first;

        std::vector<std::string> capabilities =
            module_configurations.at(module_id).capabilities.value_or(std::vector<std::string>{});

        if (not capabilities.empty()) {
            EVLOG_info << fmt::format("Module {} wants to acquire the following capabilities: {}", module_name,
                                      fmt::join(capabilities.begin(), capabilities.end(), " "));
        }

        const Handler module_ready_handler = [this, module_id, &mqtt_abstraction, &config, standalone_modules,
                                              mqtt_everest_prefix = ms.mqtt_settings.everest_prefix, &status_fifo,
                                              retain_topics](const std::string&, const nlohmann::json& json) {
            EVLOG_debug << fmt::format("received module ready signal for module: {}({})", module_id, json.dump());
            const std::unique_lock<std::mutex> lock(modules_ready_mutex_);
            // FIXME (aw): here are race conditions, if the ready handler gets called while modules are shut down!
            try {
                modules_ready_.at(module_id).ready = json.get<bool>();
            } catch (const std::out_of_range& ex) {
                // This can happen if we're shutting down and a module becomes
                // ready.
                EVLOG_error << "The module " << module_id << " is not in `modules_ready`: " << ex.what();
                return;
            }
            std::size_t modules_spawned = 0;
            for (const auto& mod : modules_ready_) {
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
            if (std::all_of(modules_ready_.begin(), modules_ready_.end(),
                            [](const auto& element) { return element.second.ready; })) {
                const auto complete_end_time = std::chrono::system_clock::now();
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

                if (sigint_received_ || isInShutdownFlowState()) {
                    EVLOG_info << "All modules reported ready while shutdown is already in progress. "
                                  "Skipping transition to Running.";
                    return;
                }
                transitionTo(ManagerState::Running);
                status_fifo.update(StatusFifo::ALL_MODULES_STARTED);
                MqttMessagePayload payload{MqttMessageType::GlobalReady, nlohmann::json(true)};

                mqtt_abstraction.publish(fmt::format("{}ready", mqtt_everest_prefix), payload);
            } else if (!standalone_modules.empty()) {
                if (modules_spawned == modules_ready_.size() - standalone_modules.size()) {
                    EVLOG_info << fmt::format(fg(fmt::terminal_color::green),
                                              "Modules started by manager are ready, waiting for standalone modules.");
                    status_fifo.update(StatusFifo::WAITING_FOR_STANDALONE_MODULES);
                }
            }
        };

        const std::string ready_topic = fmt::format("{}/ready", config.mqtt_module_prefix(module_id));
        module_it->second.ready_token =
            std::make_shared<TypedHandler>(HandlerType::ModuleReady, std::make_shared<Handler>(module_ready_handler));
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

Manager::LifecycleAdvanceResult Manager::advanceLifecycleStateIfReady(RuntimeContext& ctx,
                                                                      ManagerAdminPanel& admin_panel) {
    const bool in_shutdown_flow = isInShutdownFlowState();
    const bool crash_in_progress = (shutdown_cause_ == ShutdownCause::Crash);
    const bool restart_requested = (shutdown_cause_ == ShutdownCause::Restart);

    // Finalize shutdown as soon as all module processes are gone, even if we got here through ECHILD
    // after a timeout-triggered force shutdown.
    if (in_shutdown_flow && module_handles_.empty() && state_ != ManagerState::ShutdownFinalizing) {
        transitionTo(ManagerState::ShutdownFinalizing);
        if (crash_in_progress && unexpected_module_exit_count_ <= MAX_UNEXPECTED_MODULE_RESTARTS) {
            EVLOG_warning << fmt::format(
                "Unexpected module exit recovery attempt {}/{}. Reloading config and restarting "
                "modules.",
                unexpected_module_exit_count_, MAX_UNEXPECTED_MODULE_RESTARTS);
            handleRestartModulesAfterShutdown(ctx);
            return LifecycleAdvanceResult::transitionApplied();
        }
        if (crash_in_progress && unexpected_module_exit_count_ > MAX_UNEXPECTED_MODULE_RESTARTS) {
            EVLOG_error << fmt::format("Reached maximum unexpected module exit recovery attempts ({}/{}). "
                                       "Manager will stay idle after shutdown.",
                                       unexpected_module_exit_count_, MAX_UNEXPECTED_MODULE_RESTARTS);
        }
        if (const auto exit_code =
                handleFinalizeShutdownTransition(ctx, admin_panel, restart_requested, crash_in_progress)) {
            return LifecycleAdvanceResult::exitRequested(*exit_code);
        }
        return LifecycleAdvanceResult::transitionApplied();
    }

    // Admin restart can mark restart_modules while modules are still draining.
    // If all children are gone, restart immediately with reloaded config.
    if (restart_requested && module_handles_.empty()) {
        handleRestartModulesAfterShutdown(ctx);
        return LifecycleAdvanceResult::transitionApplied();
    }

    return LifecycleAdvanceResult::noTransition();
}

bool Manager::handleChildExit(pid_t pid, int wstatus, RuntimeContext& ctx, ManagerAdminPanel& admin_panel) {
    auto module_exited_time = std::chrono::system_clock::now();
    if (admin_panel.is_controller_process(pid)) {
        // During intentional manager shutdown/restart, controller exit is expected.
        if (isInShutdownFlowState() || state_ == ManagerState::Exiting || sigint_received_ || isRestartRequested()) {
            EVLOG_info << "Controller process exited during manager shutdown/restart.";
            return true;
        }
        admin_panel.throw_if_controller_exited(pid);
    }
    const std::string wait_status = format_wait_status(wstatus);

    const auto module_iter = module_handles_.find(pid);
    if (module_iter == module_handles_.end()) {
        throw std::runtime_error(fmt::format("Unknown child width pid ({}) died.", pid));
    }

    const auto module_name = module_iter->second;
    module_handles_.erase(module_iter);

    const bool unexpected_exit_during_start_or_run =
        (state_ == ManagerState::StartingModules || state_ == ManagerState::Running);
    if (unexpected_exit_during_start_or_run) {
        // During startup/running, an exiting module is unexpected: trigger graceful shutdown.
        shutdown_cause_ = ShutdownCause::Crash;
        ++unexpected_module_exit_count_;
        const auto shutdown_info_log = "Module " + fmt::format(TERMINAL_STYLE_BLUE, "{}", module_name) +
                                       " exited unexpectedly, signaling remaining modules to shut down gracefully...";
        handleInitiateGracefulShutdown(module_exited_time, true, &shutdown_info_log, ctx.mqtt_abstraction, ctx.ms);
        transitionTo(ManagerState::CrashShutdownInProgress);
        shutdown_info_.push_back({module_name, wstatus});
        return true;
    }

    if (isInShutdownFlowState() || sigint_received_) {
        // During shutdown drain, keep collecting statuses for final shutdown summary.
        EVLOG_info << "Module " << fmt::format(TERMINAL_STYLE_BLUE, "{}", module_name) << " (pid " << pid
                   << ") shutdown ["
                   << std::chrono::duration_cast<std::chrono::milliseconds>(
                          module_exited_time - shutdown_start_time_.value_or(module_exited_time))
                          .count()
                   << "ms] with status: " << wait_status;
        shutdown_info_.push_back({module_name, wstatus});
        return true;
    }

    EVLOG_info << fmt::format("Module {} (pid: {}) exited with status: {}.", module_name, pid, wait_status);
    return false;
}

std::optional<int> Manager::handleSignal(int signo, RuntimeContext& ctx, ManagerAdminPanel& admin_panel) {
    if (signo != SIGINT && signo != SIGTERM) {
        return std::nullopt;
    }
    if (!sigint_received_) {
        EVLOG_info << "SIGINT/SIGTERM received";
        sigint_received_ = true;
        shutdown_cause_ = ShutdownCause::Normal;
        shutdown_start_time_ = std::chrono::system_clock::now();
        if (module_handles_.empty()) {
            print_shutdown_message(shutdown_start_time_);
            admin_panel.shutdown_controller();
            cleanup(ctx.mqtt_abstraction);
            transitionTo(ManagerState::Exiting);
            return EXIT_SUCCESS;
        }
        transitionTo(ManagerState::ShutdownRequested);
        EVLOG_info << "Shutting down modules...";
        ctx.mqtt_abstraction.publish(fmt::format("{}shutdown", ctx.ms.mqtt_settings.everest_prefix),
                                     std::string("true"), QOS::QOS2, false);
        return std::nullopt;
    }

    EVLOG_info << "Terminating manager";
    admin_panel.shutdown_controller();
    transitionTo(ManagerState::Exiting);
    return EXIT_FAILURE;
}

void Manager::handleShutdownTimeout(RuntimeContext& ctx) {
    const bool should_check_shutdown_timeout =
        (state_ == ManagerState::ShutdownRequested || state_ == ManagerState::CrashShutdownInProgress ||
         state_ == ManagerState::RestartRequested) &&
        shutdown_start_time_.has_value();
    if (!should_check_shutdown_timeout) {
        return;
    }

    if (std::chrono::system_clock::now() <
        shutdown_start_time_.value() + std::chrono::milliseconds(SHUTDOWN_TIMEOUT_MS)) {
        return;
    }
    if (state_ == ManagerState::ShutdownFinalizing) {
        return;
    }

    transitionTo(ManagerState::ForceTerminating);
    EVLOG_warning << "Not all modules shut down within the timeout. Forcefully terminating remaining modules.";
    shutdownModules(module_handles_, *ctx.config, ctx.mqtt_abstraction);
}

bool Manager::handleWaitpidEvent(int& wstatus, RuntimeContext& ctx, ManagerAdminPanel& admin_panel) {
    // non-blocking as this main loop also processes controller RPC and the signal fd
    const auto pid = waitpid(-1, &wstatus, WNOHANG);
    if (pid == 0) {
        return false;
    }
    if (pid == -1) {
        if (errno != ECHILD) {
            throw std::runtime_error(fmt::format("Syscall to waitpid() failed ({})", strerror(errno)));
        }
        // ECHILD: no more children — normal when idle or when admin panel restart is in progress
        return false;
    }
    return handleChildExit(pid, wstatus, ctx, admin_panel);
}

std::optional<int> Manager::handleControllerIpcPoll(RuntimeContext& ctx, ManagerAdminPanel& admin_panel,
                                                    const std::string& prefix_opt) {
    bool modules_started = areModulesStarted();
    bool restart_requested = isRestartRequested();
    if (const auto exit_from_panel = admin_panel.poll_controller_ipc(restart_requested, modules_started,
                                                                     ctx.mqtt_abstraction, ctx.ms, prefix_opt)) {
        return *exit_from_panel;
    }

    // Keep RestartRequested while modules are draining; this preserves restart intent
    // for advanceLifecycleStateIfReady() once all children have exited.
    if (restart_requested) {
        shutdown_cause_ = ShutdownCause::Restart;
        transitionTo(ManagerState::RestartRequested);
    }
    // This also enables timeout/fallback handling if one or more modules do not exit after MQTT shutdown.
    if (restart_requested && !module_handles_.empty()) {
        shutdown_start_time_ = std::chrono::system_clock::now();
    }

    return std::nullopt;
}

std::optional<int> Manager::handleSignalPoll(system::SignalPolling& signal_polling, RuntimeContext& ctx,
                                             ManagerAdminPanel& admin_panel) {
    const auto signal_received = signal_polling.poll_signal();
    if (!signal_received.has_value()) {
        return std::nullopt;
    }
    return handleSignal(signal_received.value(), ctx, admin_panel);
}

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
    desc.add_options()("mqtt_everest_prefix", po::value<std::string>(),
                       "Override the MQTT everest prefix (useful for running multiple instances in parallel)");
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

        Manager manager(vm);
        return manager.run();

    } catch (const BootException& e) {
        EVLOG_error << "Failed to start up everest:\n" << e.what();
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        EVLOG_error << "Main manager process exits because of caught exception:\n" << e.what();
        return EXIT_FAILURE;
    }
}
