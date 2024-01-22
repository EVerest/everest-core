// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "Linux_Systemd_Rauc.hpp"
#include <companion/system/companion_logging.hpp>
#include <exception>
#include <string_view>
#include <type_traits>

namespace {
template <typename T> void safe_extract(T& dst, const Object& src, const std::string_view& item) {
    try {
        std::string s{item};
        dst = src.at(s);
    } catch (const std::exception& ex) {
        EVLOG_error << "Store[" << item << "] error: " << ex.what();
        if constexpr (std::is_integral_v<T>) {
            dst = module::Rauc::request_id_default;
        } else {
            dst.clear();
        }
    }
}
} // namespace

#include <stdexcept>

#include "companion/system/safe-system.hpp"

namespace module {

void Linux_Systemd_Rauc::init() {
    basecamp::companion::system::everest::configure_everest_logging();
    invoke_init(*p_main);
    invoke_init(*p_rauc);
    rauc.configure();

    store_path = info.id + "_update_transaction";

    // This is a transaction we should store permanently
    rauc.signal_store_update_transaction.connect([this](Rauc::UpdateTransaction t) {
        EVLOG_info << "Store update transaction: " << t.boot_slot << ' ' << t.primary_slot << " id: " << t.request_id;
        Object tx = {
            {"current_primary_slot", t.primary_slot}, {"current_boot_slot", t.boot_slot}, {"request_id", t.request_id}};
        r_store->call_store(store_path, tx);
    });

    rauc.signal_remove_update_transaction.connect([this]() {
        EVLOG_info << "Update transaction removed.";
        r_store->call_delete(store_path);
    });

    rauc.signal_firmware_update_status.connect([this](const auto& s) {
        EVLOG_info << "Report status to OCPP: "
                   << types::system::firmware_update_status_enum_to_string(s.firmware_update_status)
                   << " Request id: " << s.request_id << " Progress: " << s.progress << '%';
        p_main->publish_firmware_update_status({s.firmware_update_status, s.request_id});
        p_rauc->publish_rauc_update_status(s);

        if (s.firmware_update_status == types::system::FirmwareUpdateStatusEnum::InstallRebooting) {

            std::lock_guard<std::recursive_mutex> lock(this->firmware_update_progress_mx);
            if (this->firmware_update_waiting_for_ocpp_unblocking) {
                EVLOG_info << "Reboot is blocked by OCPP (waiting for 'allow_firmware_installation' call)";
                this->firmware_update_reboot_scheduled = true;
            } else {
                reboot_after_firmware_update();
            }
        }

        if (s.firmware_update_status == types::system::FirmwareUpdateStatusEnum::InstallVerificationFailed) {
            EVLOG_info << "Resetting firmware update state due to reported 'InstallVerificationFailed' status.";
            std::unique_lock<std::recursive_mutex> lock(this->firmware_update_progress_mx);
            this->firmware_update_waiting_for_ocpp_unblocking = false;
        }
    });
}

void Linux_Systemd_Rauc::reboot_after_firmware_update() {
    {
        std::lock_guard<std::recursive_mutex> lock(this->firmware_update_progress_mx);
        this->firmware_update_reboot_scheduled = false;
    }
    EVLOG_error << "-------------- Reboot after installation of update in 10 seconds ---------------";
    sleep(10);
    try {
        auto [cmd, args] = basecamp::companion::split_command_line(config.RebootCommand);
        auto res = basecamp::companion::safe_system(cmd, &args);
        if (res.status != basecamp::companion::CommandExecutionStatus::CMD_SUCCESS || res.code != 0) {
            EVLOG_error << "Unable to trigger reboot: ("
                        << basecamp::companion::cmd_execution_status_to_string(res.status) << ": "
                        << std::to_string(res.code) << ")";
            EVLOG_info << "Failed command: '" << basecamp::companion::command_string_repr(cmd, args) << "'";
        }
    } catch (const std::exception& ex) {
        EVLOG_error << "Configured reboot command is invalid: " << ex.what();
    }
}

void Linux_Systemd_Rauc::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_rauc);

    // Check if we are booting directly after an update install,
    // in this case close the update process on the CSMS
    if (r_store->call_exists(store_path)) {
        Rauc::UpdateTransaction tx;
        auto t = std::get<Object>(r_store->call_load(store_path));
        safe_extract(tx.boot_slot, t, "current_boot_slot");
        safe_extract(tx.primary_slot, t, "current_primary_slot");
        safe_extract(tx.request_id, t, "request_id");
        rauc.check_previous_transaction(tx);
    }
}

void Linux_Systemd_Rauc::firmware_update_may_proceed_with_reboot_callback() {
    std::lock_guard<std::recursive_mutex> lock(this->firmware_update_progress_mx);
    this->firmware_update_waiting_for_ocpp_unblocking = false;
    if (this->firmware_update_reboot_scheduled) {
        this->reboot_after_firmware_update();
    }
}

void Linux_Systemd_Rauc::install_firmware_bundle(const std::string& filename, int32_t request_id) {
    {
        std::lock_guard<std::recursive_mutex> lock(this->firmware_update_progress_mx);
        this->firmware_update_waiting_for_ocpp_unblocking = true;
    }
    this->rauc.install_bundle(filename, request_id);
}

} // namespace module
