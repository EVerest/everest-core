// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "manager_admin_panel.hpp"

#include <array>
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <optional>
#include <signal.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include <fmt/core.h>

#include <everest/logging.hpp>
#include <framework/runtime.hpp>
#include <utils/config.hpp>
#include <utils/mqtt_abstraction.hpp>

#include "controller/ipc.hpp"
#include "system_unix.hpp"

namespace fs = std::filesystem;
using namespace Everest;

namespace {
const int CONTROLLER_IPC_READ_TIMEOUT_MS = 50;
} // namespace

#ifdef ENABLE_ADMIN_PANEL
struct ControllerHandle {
    ControllerHandle(pid_t pid, int socket_fd) : pid(pid), socket_fd(socket_fd) {
        controller_ipc::set_read_timeout(socket_fd, CONTROLLER_IPC_READ_TIMEOUT_MS);
    }

    void send_message(const nlohmann::json& msg) {
        controller_ipc::send_message(socket_fd, msg);
    }

    controller_ipc::Message receive_message() {
        return controller_ipc::receive_message(socket_fd);
    }

    const pid_t pid;

private:
    const int socket_fd;
};

struct ManagerAdminPanel::Impl {
    explicit Impl(ControllerHandle h) : handle(h) {
    }

    static ControllerHandle start_controller(const ManagerSettings& ms) {
        std::array<int, 2> socket_pair{};

        socketpair(AF_UNIX, SOCK_DGRAM, 0, socket_pair.data());
        const int manager_socket = socket_pair[0];
        const int controller_socket = socket_pair[1];

        auto proc_handle = system::SubProcess::create(ms.run_as_user);

        if (proc_handle.is_child()) {
            const auto bin_dir = fs::canonical("/proc/self/exe").parent_path();
            const auto controller_binary = bin_dir / "controller";

            // The manager may block SIGINT/SIGTERM for signalfd polling.
            // Unblock them in the controller child so external SIGTERM can stop it.
            sigset_t unblocked_signals;
            sigemptyset(&unblocked_signals);
            sigaddset(&unblocked_signals, SIGINT);
            sigaddset(&unblocked_signals, SIGTERM);
            if (sigprocmask(SIG_UNBLOCK, &unblocked_signals, nullptr) != 0) {
                proc_handle.send_error_and_exit(
                    fmt::format("Syscall to sigprocmask() failed while preparing controller ({})", strerror(errno)));
            }

            close(manager_socket);
            dup2(controller_socket, STDIN_FILENO);
            close(controller_socket);

            execl(controller_binary.c_str(), MAGIC_CONTROLLER_ARG0, NULL);

            proc_handle.send_error_and_exit(fmt::format("Syscall to execl() with \"{} {}\" failed ({})",
                                                        controller_binary.string(), strerror(errno)));
        }

        close(controller_socket);

        controller_ipc::send_message(manager_socket,
                                     {{"method", "boot"},
                                      {"params",
                                       {{"module_dir", ms.runtime_settings.modules_dir.string()},
                                        {"interface_dir", ms.interfaces_dir.string()},
                                        {"www_dir", ms.www_dir.string()},
                                        {"configs_dir", ms.configs_dir.string()},
                                        {"logging_config_file", ms.runtime_settings.logging_config_file.string()},
                                        {"controller_port", ms.controller_port},
                                        {"controller_rpc_timeout_ms", ms.controller_rpc_timeout_ms}}}});

        return {proc_handle.check_child_executed(), manager_socket};
    }

    ControllerHandle handle;
};

ManagerAdminPanel::ManagerAdminPanel(std::unique_ptr<Impl> impl_) : impl(std::move(impl_)) {
}
#endif

#ifndef ENABLE_ADMIN_PANEL
ManagerAdminPanel::ManagerAdminPanel() = default;
#endif
ManagerAdminPanel::~ManagerAdminPanel() = default;
ManagerAdminPanel::ManagerAdminPanel(ManagerAdminPanel&&) noexcept = default;
ManagerAdminPanel& ManagerAdminPanel::operator=(ManagerAdminPanel&&) noexcept = default;

ManagerAdminPanel ManagerAdminPanel::create(const ManagerSettings& ms) {
#ifdef ENABLE_ADMIN_PANEL
    return ManagerAdminPanel(std::make_unique<Impl>(Impl::start_controller(ms)));
#else
    static_cast<void>(ms);
    return ManagerAdminPanel{};
#endif
}

void ManagerAdminPanel::throw_if_controller_exited(pid_t pid) const {
#ifdef ENABLE_ADMIN_PANEL
    if (not impl) {
        throw std::runtime_error("ManagerAdminPanel is not initialized.");
    }
    if (pid == impl->handle.pid) {
        throw std::runtime_error("The controller process exited.");
    }
#else
    static_cast<void>(pid);
#endif
}

bool ManagerAdminPanel::is_controller_process(pid_t pid) const {
#ifdef ENABLE_ADMIN_PANEL
    if (not impl) {
        return false;
    }
    return pid == impl->handle.pid;
#else
    static_cast<void>(pid);
    return false;
#endif
}

void ManagerAdminPanel::shutdown_controller() const {
#ifdef ENABLE_ADMIN_PANEL
    if (not impl) {
        return;
    }
    if (kill(impl->handle.pid, SIGTERM) != 0 && errno != ESRCH) {
        EVLOG_warning << fmt::format("Failed to SIGTERM controller process {} ({})", impl->handle.pid, strerror(errno));
    }
#endif
}

std::optional<int> ManagerAdminPanel::poll_controller_ipc(bool& restart_modules, bool& modules_started,
                                                          MQTTAbstraction& mqtt, const ManagerSettings& ms,
                                                          const std::string& prefix_opt) {
#ifdef ENABLE_ADMIN_PANEL
    if (not impl) {
        return EXIT_FAILURE;
    }
    const auto msg = impl->handle.receive_message();
    if (msg.status == controller_ipc::MESSAGE_RETURN_STATUS::OK) {
        const auto& payload = msg.json;
        if (payload.at("method") == "restart_modules") {
            restart_modules = true;
            modules_started = false;
            mqtt.publish(fmt::format("{}shutdown", ms.mqtt_settings.everest_prefix), std::string("true"), QOS::QOS2,
                         false);
            EVLOG_info << "Controller requested graceful module restart (config will be reloaded).";
        } else if (payload.at("method") == "check_config") {
            const std::string check_config_file_path = payload.at("params");

            try {
                auto cfg = ManagerConfig(ManagerSettings(prefix_opt, check_config_file_path));
                static_cast<void>(cfg);
                impl->handle.send_message({{"id", payload.at("id")}, {"result", {{"ok", true}}}});
            } catch (const std::exception& e) {
                impl->handle.send_message({{"result", e.what()}, {"id", payload.at("id")}});
            }
        } else {
            EVLOG_error << fmt::format("Received unknown command via controller ipc:\n{}\n... ignoring",
                                       payload.dump(2));
        }
    } else if (msg.status == controller_ipc::MESSAGE_RETURN_STATUS::ERROR) {
        fmt::print("Error in IPC communication with controller: {}\nExiting\n", msg.json.at("error").dump(2));
        return EXIT_FAILURE;
    }
    return std::nullopt;
#else
    static_cast<void>(restart_modules);
    static_cast<void>(modules_started);
    static_cast<void>(mqtt);
    static_cast<void>(ms);
    static_cast<void>(prefix_opt);
    return std::nullopt;
#endif
}

std::optional<std::string> ManagerAdminPanel::switch_manager_user_if_needed(const ManagerSettings& ms) {
#ifndef ENABLE_ADMIN_PANEL
    if (not ms.run_as_user.empty()) {
        auto err_set_user = system::set_real_user(ms.run_as_user);
        if (not err_set_user.empty()) {
            return err_set_user;
        }
    }
#else
    static_cast<void>(ms);
#endif
    return std::nullopt;
}
