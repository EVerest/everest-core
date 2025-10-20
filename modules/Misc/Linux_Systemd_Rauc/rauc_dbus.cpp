// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "rauc_dbus.hpp"

#include <everest/logging.hpp>

namespace module {
using namespace everest::lib::system;

void Rauc::configure_handlers() {
    namespace interface = rauc_dbus::interface;
    namespace property = rauc_dbus::property;
    namespace signal = rauc_dbus::signal;

    // Subscribe to Complete signal (when install_bundle command finishes)
    dbus::registerSignalHandler(proxy, interface::Installer, signal::Completed, [this](sdbus::Signal signal) {
        // Complete signal has one int argument
        int i;
        signal >> i;
        if (update_request_id != request_id_default) {
            if (i == 0) {
                EVLOG_info << "RAUC: Installation successful, needs reboot to activate";
                // Signal to the module code to store the transaction in the database.
                // We will use this on next boot to signal a Success/Failed Installation
                signal_store_update_transaction(create_transaction(update_request_id, timeout_us));
                // The module code should reboot now since we signal InstallRebooting.
                signal_firmware_update_status(types::system::FirmwareUpdateStatusEnum::InstallRebooting,
                                              update_request_id);
            } else {
                EVLOG_error << "RAUC: Installation failed with error code: " << i;
                if (is_installing) {
                    signal_firmware_update_status(types::system::FirmwareUpdateStatusEnum::InstallVerificationFailed,
                                                  update_request_id);
                    is_installing = false;
                }
            }
        } else {
            EVLOG_debug << "RAUC: status from another source";
        }

        // this was the last message, so reset request_id
        update_request_id = request_id_default;
    });

    // Subscribe to property changes std::function<void(Signal signal)>;
    dbus::registerSignalHandler(
        proxy, interface::DBus_Properties, signal::PropertiesChanged, [this](sdbus::Signal signal) {
            //  org.freedesktop.DBus.Properties.PropertiesChanged (STRING interface_name,
            //                                                     ARRAY of DICT_ENTRY<STRING,VARIANT>
            //                                                     changed_properties, ARRAY<STRING>
            //                                                     invalidated_properties);
            std::string interface;
            signal >> interface;
            // ignore updates when initiated by someone else
            if (update_request_id != request_id_default) {
                if (interface == interface::Installer) {
                    std::map<std::string, sdbus::Variant> changed_properties;
                    signal >> changed_properties;
                    for (const auto& [key, value] : changed_properties) {
                        if (key == property::Progress) {
                            Progress r = value.get<Progress>();
                            EVLOG_info << "Progress " << r.percent << "% " << r.description;

                            // Map progress to OCPP structs

                            if (r.description.find("Verifying signature done") != std::string::npos) {
                                signal_firmware_update_status(types::system::FirmwareUpdateStatusEnum::Downloaded,
                                                              update_request_id);
                                signal_firmware_update_status(
                                    types::system::FirmwareUpdateStatusEnum::SignatureVerified, update_request_id);
                                signature_verified = true;

                            } else if (r.description.find("Verifying signature failed") != std::string::npos) {
                                signal_firmware_update_status(types::system::FirmwareUpdateStatusEnum::Downloaded,
                                                              update_request_id);
                                signal_firmware_update_status(types::system::FirmwareUpdateStatusEnum::InvalidSignature,
                                                              update_request_id);
                                signature_verified = true;

                                // If bundle checking failed but we never got to signature verification download must
                                // have failed
                            } else if (!signature_verified &&
                                       r.description.find("Checking bundle failed") != std::string::npos) {
                                signal_firmware_update_status(types::system::FirmwareUpdateStatusEnum::DownloadFailed,
                                                              update_request_id);

                            } else if (r.description.find("Copying") != std::string::npos &&
                                       r.description.find("done") == std::string::npos) {
                                is_installing = true;
                                signal_firmware_update_status(types::system::FirmwareUpdateStatusEnum::Installing,
                                                              update_request_id);
                            }

                        } else if (key == property::Operation) {
                            auto r = rauc_dbus::rauc_messages::string_to_operation(value.get<std::string>());

                            if (r == Operation::Idle) {
                                EVLOG_info << "RAUC operation: Idle";
                            } else if (r == Operation::Installing) {
                                EVLOG_info << "RAUC operation: Installing";
                            }
                        }
                    }

                    std::vector<std::string> invalidated_properties;
                    signal >> invalidated_properties;
                }
            }
        });
}

bool Rauc::decide_if_good(const rauc_dbus::rauc_messages::UpdateTransaction& saved, const CurrentState& current) {
    // The original approach uses the primary slot, however this might not be
    // as reliable as hoped. A change in boot slot should be more reliable
    // however prior to this change the boot slot wasn't saved

    bool result{false};

    if (RaucBase::decide_if_good(saved, current)) {
        if (saved.boot_slot.empty()) {
            // use the previous approach
            EVLOG_warning << "OTA: fallback to using primary slot";
            result = saved.primary_slot == current.primary_slot;
        } else {
            // use the new approach
            result = saved.boot_slot != current.boot_slot;
        }
    }
    return result;
}

// Call on boot and pass a previous transaction that was not closed yet
void Rauc::check_previous_transaction(UpdateTransaction t) {
    signal_remove_update_transaction();

    if (rauc_dbus::RaucBaseSync::check_previous_transaction(t, timeout_us)) {
        signal_firmware_update_status(types::system::FirmwareUpdateStatusEnum::Installed, t.request_id);
    } else {
        signal_firmware_update_status(types::system::FirmwareUpdateStatusEnum::InstallationFailed, t.request_id);
    }
}

// Returns immediately. Progress is signalled with signal_firmware_update_status
rauc_dbus::rauc_messages::CmdResult Rauc::install_bundle(const std::string& filename, int32_t request_id) {
    signature_verified = false;
    is_installing = false;
    update_request_id = request_id;

    const auto ret = rauc_dbus::RaucBaseSync::install_bundle(filename, timeout_us);
    if (ret.success) {
        signal_firmware_update_status(types::system::FirmwareUpdateStatusEnum::Downloading, update_request_id);
    } else {
        update_request_id = request_id_default;
    }

    return ret;
}

bool Rauc::is_idle() {
    // Note it is important to query rauc here as well as it may be busy with a local install
    return (get_operation() == rauc_dbus::rauc_messages::Operation::Idle) && !is_installing;
}

} // namespace module
