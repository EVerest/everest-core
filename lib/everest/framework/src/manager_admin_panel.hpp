// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <sys/types.h>

namespace Everest {
class MQTTAbstraction;
struct ManagerSettings;
} // namespace Everest

struct ManagerAdminPanel {
    ~ManagerAdminPanel();
    ManagerAdminPanel(ManagerAdminPanel&&) noexcept;
    ManagerAdminPanel& operator=(ManagerAdminPanel&&) noexcept;

    ManagerAdminPanel(const ManagerAdminPanel&) = delete;
    ManagerAdminPanel& operator=(const ManagerAdminPanel&) = delete;

    static ManagerAdminPanel create(const Everest::ManagerSettings& ms);
    static std::optional<std::string> switch_manager_user_if_needed(const Everest::ManagerSettings& ms);
    void shutdown_controller() const;
    bool is_controller_process(pid_t pid) const;

    void throw_if_controller_exited(pid_t pid) const;

    /// Process controller IPC for one main-loop iteration.
    /// Returns an exit code for `boot`, or `nullopt` to continue.
    std::optional<int> poll_controller_ipc(bool& restart_modules, bool& modules_started, Everest::MQTTAbstraction& mqtt,
                                           const Everest::ManagerSettings& ms, const std::string& prefix_opt);

private:
#ifdef ENABLE_ADMIN_PANEL
    struct Impl;
    std::unique_ptr<Impl> impl;

    ManagerAdminPanel() = delete;
    explicit ManagerAdminPanel(std::unique_ptr<Impl> impl_);
#else
    ManagerAdminPanel();
#endif
};
