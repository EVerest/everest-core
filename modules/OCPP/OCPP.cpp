// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "OCPP.hpp"

#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/process.hpp>

namespace module {

void OCPP::init() {
    invoke_init(*p_main);
    invoke_init(*p_auth_validator);

    boost::filesystem::path config_path = boost::filesystem::path(this->config.ChargePointConfigPath);

    std::ifstream ifs(config_path.c_str());
    std::string config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    json json_config = json::parse(config_file);
    json_config.at("Core").at("NumberOfConnectors") = this->r_evse_manager.size();

    std::shared_ptr<ocpp1_6::ChargePointConfiguration> configuration =
        std::make_shared<ocpp1_6::ChargePointConfiguration>(json_config, this->config.SchemasPath,
                                                            this->config.DatabasePath);

    this->charge_point = new ocpp1_6::ChargePoint(configuration);
    // TODO(kai): select appropriate EVSE based on connector
    this->charge_point->register_pause_charging_callback([this](int32_t connector) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->r_evse_manager.at(connector - 1)->call_pause_charging();
        } else {
            return false;
        }
    });
    this->charge_point->register_resume_charging_callback([this](int32_t connector) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->r_evse_manager.at(connector - 1)->call_resume_charging();
        } else {
            return false;
        }
    });
    this->charge_point->register_cancel_charging_callback([this](int32_t connector) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->r_evse_manager.at(connector - 1)->call_cancel_charging();
        } else {
            return false;
        }
    });
    this->charge_point->register_reserve_now_callback(
        [this](int32_t connector, ocpp1_6::CiString20Type idTag, std::chrono::seconds timeout) {
            if (connector > 0 && connector <= this->r_evse_manager.size()) {
                return this->r_evse_manager.at(connector - 1)->call_reserve_now(idTag.get(), timeout.count());
            } else {
                return false;
            }
        });
    this->charge_point->register_cancel_reservation_callback([this](int32_t connector) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->r_evse_manager.at(connector - 1)->call_cancel_reservation();
        } else {
            return false;
        }
    });
    this->charge_point->register_upload_diagnostics_callback([this](std::string location) {
        // create temporary file
        std::string date_time = "DATETIME"; // FIXME
        std::string file_name = "diagnostics-" + date_time + "-%%%%-%%%%-%%%%-%%%%";

        auto diagnostics_file_name = boost::filesystem::unique_path(boost::filesystem::path(file_name));
        auto diagnostics_file_path = boost::filesystem::temp_directory_path() / diagnostics_file_name;

        auto diagnostics = json::object({{"diagnostics", json::object({{"key", "value"}})}});
        std::ofstream diagnostics_file(diagnostics_file_path.c_str());
        diagnostics_file << diagnostics.dump();
        this->upload_diagnostics_thread = std::thread([this, location, diagnostics_file_name, diagnostics_file_path]() {
            auto diagnostics_uploader = boost::filesystem::path("bin/diagnostics_uploader.sh");

            boost::process::ipstream stream;
            std::vector<std::string> args = {location, diagnostics_file_name.string(), diagnostics_file_path.string()};
            boost::process::child cmd(diagnostics_uploader, boost::process::args(args),
                                      boost::process::std_out > stream);
            std::string temp;
            while (std::getline(stream, temp)) {
                if (temp == "uploaded") {
                    this->charge_point->send_diagnostic_status_notification(ocpp1_6::DiagnosticsStatus::Uploaded);
                } else if (temp == "failed") {
                    this->charge_point->send_diagnostic_status_notification(ocpp1_6::DiagnosticsStatus::UploadFailed);
                } else {
                    this->charge_point->send_diagnostic_status_notification(ocpp1_6::DiagnosticsStatus::Uploading);
                }
            }
            cmd.wait();
        });

        return diagnostics_file_name.string();
    });
    this->charge_point->register_update_firmware_callback([this](std::string location) {
        // // create temporary file
        std::string date_time = "DATETIME"; // FIXME
        std::string file_name = "firmware-" + date_time + "-%%%%-%%%%-%%%%-%%%%";

        auto firmware_file_name = boost::filesystem::unique_path(boost::filesystem::path(file_name));
        auto firmware_file_path = boost::filesystem::temp_directory_path() / firmware_file_name;

        this->update_firmware_thread = std::thread([this, location, firmware_file_path]() {
            auto firmware_updater = boost::filesystem::path("bin/firmware_updater.sh");

            boost::process::ipstream stream;
            std::vector<std::string> args = {location, firmware_file_path.string()};
            boost::process::child cmd(firmware_updater, boost::process::args(args), boost::process::std_out > stream);
            std::string temp;
            while (std::getline(stream, temp)) {
                if (temp == "downloaded") {
                    this->charge_point->send_firmware_status_notification(ocpp1_6::FirmwareStatus::Downloaded);
                } else if (temp == "download_failed") {
                    this->charge_point->send_firmware_status_notification(ocpp1_6::FirmwareStatus::DownloadFailed);
                } else if (temp == "installing") {
                    this->charge_point->send_firmware_status_notification(ocpp1_6::FirmwareStatus::Installing);
                } else if (temp == "installed") {
                    this->charge_point->send_firmware_status_notification(ocpp1_6::FirmwareStatus::Installed);
                } else if (temp == "installation_failed") {
                    this->charge_point->send_firmware_status_notification(ocpp1_6::FirmwareStatus::InstallationFailed);
                } else {
                    this->charge_point->send_firmware_status_notification(ocpp1_6::FirmwareStatus::Downloading);
                }
            }
            cmd.wait();
        });
    });

    this->charge_point->start();

    int32_t connector = 1;
    for (auto& evse : this->r_evse_manager) {
        evse->subscribe_powermeter([this, connector](Object powermeter) {
            this->charge_point->receive_power_meter(connector, powermeter); //
        });

        evse->subscribe_limits([this, connector](Object limits) {
            auto max_current = limits["max_current"].get<double>();
            this->charge_point->receive_max_current_offered(connector, max_current);

            auto number_of_phases = limits["nr_of_phases_available"].get<int32_t>();
            this->charge_point->receive_number_of_phases_available(connector, number_of_phases);
        });

        evse->subscribe_session_events([this, connector](Object session_events) {
            auto event = session_events["event"];
            if (event == "Enabled") {
                // TODO(kai): decide if we need to inform libocpp about such an event
            } else if (event == "Disabled") {
                // TODO(kai): decide if we need to inform libocpp about such an event
            } else if (event == "SessionStarted") {
                auto session_started = session_events["session_started"];
                auto timestamp = ocpp1_6::DateTime(std::chrono::system_clock::time_point(
                    std::chrono::seconds(session_started["timestamp"].get<int>())));
                auto energy_Wh_import = session_started["energy_Wh_import"].get<double>();
                this->charge_point->start_session(connector, timestamp, energy_Wh_import);
            } else if (event == "ChargingStarted") {
                this->charge_point->start_charging(connector);
            } else if (event == "ChargingPausedEV") {
                this->charge_point->suspend_charging_ev(connector);
            } else if (event == "ChargingPausedEVSE") {
                this->charge_point->suspend_charging_evse(connector);
            } else if (event == "ChargingResumed") {
                this->charge_point->resume_charging(connector);
            } else if (event == "SessionFinished") {
                auto session_finished = session_events["session_finished"];
                auto timestamp = std::chrono::system_clock::time_point(
                    std::chrono::seconds(session_finished["timestamp"].get<int>()));
                auto energy_Wh_import = session_finished["energy_Wh_import"].get<double>();
                this->charge_point->stop_session(connector, ocpp1_6::DateTime(timestamp), energy_Wh_import);
            } else if (event == "Error") {
                auto error = session_events["error"];
                if (error == "OverCurrent") {
                    this->charge_point->error(connector, ocpp1_6::ChargePointErrorCode::OverCurrentFailure);
                } else {
                    this->charge_point->vendor_error(connector, error);
                }
            } else if (event == "PermanentFault") {
                this->charge_point->permanent_fault(connector);
            }
        });

        connector += 1;
    }
}

void OCPP::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_auth_validator);
}

} // namespace module
