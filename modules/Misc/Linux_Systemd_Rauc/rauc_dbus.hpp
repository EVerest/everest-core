// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef RAUC_DBUS_HPP
#define RAUC_DBUS_HPP

#include <everest/system/rauc_dbus_base.hpp>
#include <sigslot/signal.hpp>

#include <generated/types/system.hpp>

#include <atomic>
#include <cstdint>
#include <unistd.h>

namespace module {
namespace rauc_dbus = everest::lib::system::rauc_dbus;

class Rauc : public rauc_dbus::RaucBaseSync {
public:
    // Note OCPP defaults to -1 when not provided
    static constexpr std::int32_t request_id_default = 0;

private:
    constexpr static std::uint64_t timeout_us = 10 * 1000 * 1000; // 10 seconds

    std::atomic<int32_t> update_request_id{request_id_default};
    std::atomic_bool is_installing{false};
    std::atomic_bool signature_verified{false};

    void configure_handlers() override;
    bool decide_if_good(const rauc_dbus::rauc_messages::UpdateTransaction& saved, const CurrentState& current) override;

public:
    using CmdResult = rauc_dbus::rauc_messages::CmdResult;
    using UpdateTransaction = rauc_dbus::rauc_messages::UpdateTransaction;
    using Operation = rauc_dbus::rauc_messages::Operation;
    using Progress = rauc_dbus::rauc_messages::Progress;

    using rauc_dbus::RaucBaseSync::RaucBaseSync;
    Rauc(sdbus::dont_run_event_loop_thread_t) = delete;

    void check_previous_transaction(UpdateTransaction t);
    bool is_idle();

    CmdResult install_bundle(const std::string& filename, int32_t request_id);

    void mark(const std::string& mark_s, const std::string& slot) {
        rauc_dbus::RaucBaseSync::mark(mark_s, slot, timeout_us);
    }

    sigslot::signal<types::system::FirmwareUpdateStatusEnum, int32_t> signal_firmware_update_status;
    // Emitted when installed update is ready for reboot, the transaction needs to be stored persistently. On next boot,
    // call check_previous_transaction() with this as an argument
    sigslot::signal<UpdateTransaction> signal_store_update_transaction;
    sigslot::signal<> signal_remove_update_transaction;
};

} // namespace module

#endif
