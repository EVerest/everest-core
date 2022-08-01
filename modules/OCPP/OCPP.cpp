// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "OCPP.hpp"
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/process.hpp>

namespace module {

static ocpp1_6::ChargePointErrorCode get_ocpp_error_code(const std::string& evse_error) {
    if (evse_error == "Car") {
        return ocpp1_6::ChargePointErrorCode::OtherError;
    } else if (evse_error == "CarDiodeFault") {
        return ocpp1_6::ChargePointErrorCode::EVCommunicationError;
    } else if (evse_error == "Relais") {
        return ocpp1_6::ChargePointErrorCode::OtherError;
    } else if (evse_error == "RCD") {
        return ocpp1_6::ChargePointErrorCode::GroundFailure;
    } else if (evse_error == "VentilationNotAvailable") {
        return ocpp1_6::ChargePointErrorCode::OtherError;
    } else if (evse_error == "OverCurrent") {
        return ocpp1_6::ChargePointErrorCode::OverCurrentFailure;
    } else if (evse_error == "Internal") {
        return ocpp1_6::ChargePointErrorCode::OtherError;
    } else if (evse_error == "SLAC") {
        return ocpp1_6::ChargePointErrorCode::EVCommunicationError;
    } else if (evse_error == "HLC") {
        return ocpp1_6::ChargePointErrorCode::EVCommunicationError;
    } else {
        return ocpp1_6::ChargePointErrorCode::OtherError;
    }
}

void create_empty_user_config(const boost::filesystem::path& user_config_path) {
    boost::filesystem::create_directory(user_config_path.parent_path());
    std::ofstream fs(user_config_path.c_str());
    auto user_config = json::object();
    fs << user_config << std::endl;
    fs.close();
}

void OCPP::init() {
    invoke_init(*p_main);
    invoke_init(*p_auth_validator);
    invoke_init(*p_auth_provider);

    boost::filesystem::path config_path = boost::filesystem::path(this->config.ChargePointConfigPath);
    boost::filesystem::path user_config_path =
        boost::filesystem::path(this->config.OcppMainPath) / "user_config" / "user_config.json";

    std::ifstream ifs(config_path.c_str());
    std::string config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    json json_config = json::parse(config_file);
    json_config.at("Core").at("NumberOfConnectors") = this->r_evse_manager.size();

    if (boost::filesystem::exists(user_config_path)) {
        std::ifstream ifs(user_config_path.c_str());
        std::string user_config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

        try {
            auto user_config = json::parse(user_config_file);
            EVLOG_info << "Augmenting chargepoint config with user_config entries";
            json_config.merge_patch(user_config);
        } catch (const json::parse_error& e) {
            // rewrite empty user config if it is not parsable
            EVLOG_error << "Error while parsing user config file.";
            throw;
        }
    } else {
        EVLOG_debug << "No user-config provided. Creating user config file";
        create_empty_user_config(user_config_path);
    }

    std::shared_ptr<ocpp1_6::ChargePointConfiguration> configuration =
        std::make_shared<ocpp1_6::ChargePointConfiguration>(json_config, this->config.SchemasPath,
                                                            this->config.SchemasPath, this->config.DatabasePath);

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
    this->charge_point->register_cancel_charging_callback([this](int32_t connector, ocpp1_6::Reason reason) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->r_evse_manager.at(connector - 1)
                ->call_cancel_charging(types::evse_manager::string_to_session_cancellation_reason(ocpp1_6::conversions::reason_to_string(reason)));
        } else {
            return false;
        }
    });

    this->charge_point->register_unlock_connector_callback([this](int32_t connector) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            EVLOG_info << "Executing unlock connector callback";
            return this->r_evse_manager.at(connector - 1)->call_force_unlock();
        } else {
            return false;
        }
    });

    // int32_t reservation_id, CiString20Type auth_token, DateTime expiry_date, std::string parent_id
    this->charge_point->register_reserve_now_callback(
        [this](int32_t reservation_id, int32_t connector, ocpp1_6::DateTime expiryDate, ocpp1_6::CiString20Type idTag,
               boost::optional<ocpp1_6::CiString20Type> parent_id) {
            if (connector > 0 && connector <= this->r_evse_manager.size()) {
                auto response =
                    this->r_evse_manager.at(connector - 1)
                        ->call_reserve_now(reservation_id, idTag.get(), expiryDate.to_rfc3339(),
                                           parent_id.value_or(ocpp1_6::CiString20Type(std::string(""))).get());
                return this->ResStatMap.at(types::evse_manager::reservation_result_to_string(response));
            } else {
                return ocpp1_6::ReservationStatus::Unavailable;
            }
        });

    this->charge_point->register_cancel_reservation_callback([this](int32_t connector) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->can_res_stat_map.at(this->r_evse_manager.at(connector - 1)->call_cancel_reservation());
        } else {
            return this->can_res_stat_map.at(false);
        }
    });

    this->charge_point->register_upload_diagnostics_callback([this](const ocpp1_6::GetDiagnosticsRequest& msg) {
        std::string date_time = ocpp1_6::DateTime().to_rfc3339();

        // TODO(piet): consider start time and end time
        std::string file_name = "diagnostics-" + date_time + "-%%%%-%%%%-%%%%-%%%%";

        const auto diagnostics_file_name = boost::filesystem::unique_path(boost::filesystem::path(file_name));
        const auto diagnostics_file_path = boost::filesystem::temp_directory_path() / diagnostics_file_name;

        const auto diagnostics = json({{"diagnostics", {{"key", "value"}}}});
        std::ofstream diagnostics_file(diagnostics_file_path.c_str());
        diagnostics_file << diagnostics.dump();
        this->upload_diagnostics_thread = std::thread([this, msg, diagnostics_file_name, diagnostics_file_path]() {
            const auto diagnostics_uploader =
                boost::filesystem::path(this->config.ScriptsPath) / "diagnostics_uploader.sh";
            const auto constants = boost::filesystem::path(this->config.ScriptsPath) / "constants.env";

            std::vector<std::string> args = {constants.string(), msg.location, diagnostics_file_name.string(),
                                             diagnostics_file_path.string()};
            bool upload_failed = true;
            int32_t retries = 0;
            const auto total_retries = msg.retries.get_value_or(this->config.DefaultRetries);
            const auto retry_interval = msg.retryInterval.get_value_or(this->config.DefaultRetryInterval);

            while (upload_failed && retries <= total_retries) {
                boost::process::ipstream stream;
                boost::process::child cmd(diagnostics_uploader, boost::process::args(args),
                                          boost::process::std_out > stream);
                std::string temp;
                retries += 1;
                while (std::getline(stream, temp)) {
                    if (temp == "Uploaded") {
                        this->charge_point->send_diagnostic_status_notification(ocpp1_6::DiagnosticsStatus::Uploaded);
                        upload_failed = false;
                    } else if (temp == "UploadFailure" || temp == "PermissionDenied" || temp == "BadMessage" ||
                               temp == "NotSupportedOperation") {
                        this->charge_point->send_diagnostic_status_notification(
                            ocpp1_6::DiagnosticsStatus::UploadFailed);
                        if (retries <= total_retries) {
                            std::this_thread::sleep_for(std::chrono::seconds(retry_interval));
                        }
                    } else {
                        this->charge_point->send_diagnostic_status_notification(ocpp1_6::DiagnosticsStatus::Uploading);
                    }
                }
                cmd.wait();
            }
        });
        this->upload_diagnostics_thread.detach();

        return diagnostics_file_name.string();
    });
    this->charge_point->register_upload_logs_callback([this](ocpp1_6::GetLogRequest msg) {
        EVLOG_debug << "Executing callback for log upload with requestId: " << msg.requestId;
        // create temporary file
        std::string date_time = ocpp1_6::DateTime().to_rfc3339();
        std::string file_name =
            ocpp1_6::conversions::log_enum_type_to_string(msg.logType) + "-" + date_time + "-%%%%-%%%%-%%%%-%%%%";

        const auto log_file_name = boost::filesystem::unique_path(boost::filesystem::path(file_name));
        const auto log_file_path = boost::filesystem::temp_directory_path() / log_file_name;
        const auto constants = boost::filesystem::path(this->config.ScriptsPath) / "constants.env";

        const auto diagnostics = json({{"logs", {{"key", "value"}}}});
        std::ofstream log_file(log_file_path.c_str());
        log_file << diagnostics.dump();
        this->upload_logs_thread = std::thread([this, msg, log_file_name, log_file_path, constants]() {
            const auto diagnostics_uploader =
                boost::filesystem::path(this->config.ScriptsPath) / "diagnostics_uploader.sh";

            std::vector<std::string> args = {constants.string(), msg.log.remoteLocation.get(), log_file_name.string(),
                                             log_file_path.string()};

            bool terminated = false;
            int32_t retries = 0;
            const auto total_retries = msg.retries.get_value_or(this->config.DefaultRetries);
            const auto retry_interval = msg.retryInterval.get_value_or(this->config.DefaultRetryInterval);
            ocpp1_6::UploadLogStatusEnumType log_status = ocpp1_6::UploadLogStatusEnumType::UploadFailure;

            while (log_status != ocpp1_6::UploadLogStatusEnumType::Uploaded && retries <= total_retries &&
                   !terminated) {
                boost::process::ipstream stream;
                boost::process::child cmd(diagnostics_uploader, boost::process::args(args),
                                          boost::process::std_out > stream);
                std::string temp;
                retries += 1;
                while (std::getline(stream, temp) && !this->charge_point->interrupt_log_upload) {
                    log_status = ocpp1_6::conversions::string_to_upload_log_status_enum_type(temp);
                    this->charge_point->logStatusNotification(log_status, msg.requestId);
                }

                if (this->charge_point->interrupt_log_upload) {
                    EVLOG_debug << "Uploading Logs was interrupted, terminating upload script, requestId: "
                                << msg.requestId;
                    cmd.terminate();
                    terminated = true;
                } else {
                    EVLOG_debug << "Uploading Logs finished without interruption, requestId: " << msg.requestId;
                    if (log_status != ocpp1_6::UploadLogStatusEnumType::Uploaded) {
                        std::this_thread::sleep_for(std::chrono::seconds(retry_interval));
                    }
                }
            }
            this->charge_point->logStatusNotification(ocpp1_6::UploadLogStatusEnumType::Idle, msg.requestId);
            {
                std::lock_guard<std::mutex> lk(this->charge_point->log_upload_mutex);
                this->charge_point->interrupt_log_upload.exchange(false);
            }
            this->charge_point->cv.notify_one();
        });
        this->upload_logs_thread.detach();

        return log_file_name.string();
    });
    this->charge_point->register_update_firmware_callback([this](const ocpp1_6::UpdateFirmwareRequest msg) {
        // // create temporary file
        const auto date_time = ocpp1_6::DateTime().to_rfc3339();
        const auto file_name = "firmware-" + date_time + "-%%%%-%%%%-%%%%-%%%%";

        const auto firmware_file_name = boost::filesystem::unique_path(boost::filesystem::path(file_name));
        const auto firmware_file_path = boost::filesystem::temp_directory_path() / firmware_file_name;
        const auto constants = boost::filesystem::path(this->config.ScriptsPath) / "constants.env";

        this->update_firmware_thread = std::thread([this, msg, firmware_file_path, constants]() {
            auto firmware_updater = boost::filesystem::path(this->config.ScriptsPath) / "firmware_updater.sh";

            const std::vector<std::string> args = {constants.string(), msg.location, firmware_file_path.string()};
            int32_t retries = 0;
            const auto total_retries = msg.retries.get_value_or(this->config.DefaultRetries);
            const auto retry_interval = msg.retryInterval.get_value_or(this->config.DefaultRetryInterval);
            ocpp1_6::FirmwareStatus firmware_status = ocpp1_6::FirmwareStatus::DownloadFailed;

            while (firmware_status == ocpp1_6::FirmwareStatus::DownloadFailed && retries <= total_retries) {
                boost::process::ipstream stream;
                boost::process::child cmd(firmware_updater, boost::process::args(args),
                                          boost::process::std_out > stream);
                std::string temp;
                retries += 1;
                while (std::getline(stream, temp)) {
                    firmware_status = ocpp1_6::conversions::string_to_firmware_status(temp);
                    this->charge_point->send_firmware_status_notification(firmware_status);
                }
                if (firmware_status == ocpp1_6::FirmwareStatus::DownloadFailed) {
                    std::this_thread::sleep_for(std::chrono::seconds(retry_interval));
                }
                cmd.wait();
            }
        });
        this->update_firmware_thread.detach();
    });

    this->charge_point->register_signed_update_firmware_download_callback(
        [this](ocpp1_6::SignedUpdateFirmwareRequest msg) {
            EVLOG_debug << "Executing signed firmware update download callback";

            this->signed_update_firmware_thread = std::thread([this, msg]() {
                // // create temporary file
                const std::string date_time = ocpp1_6::DateTime().to_rfc3339();
                const std::string file_name = "signed_firmware-" + date_time + "-%%%%-%%%%-%%%%-%%%%" + ".pnx";

                const auto firmware_file_name = boost::filesystem::unique_path(boost::filesystem::path(file_name));
                const auto firmware_file_path = boost::filesystem::temp_directory_path() / firmware_file_name;

                const auto firmware_downloader =
                    boost::filesystem::path(this->config.ScriptsPath) / "signed_firmware_downloader.sh";
                const auto constants = boost::filesystem::path(this->config.ScriptsPath) / "constants.env";

                const std::vector<std::string> download_args = {
                    constants.string(), msg.firmware.location.get(), firmware_file_path.string(),
                    msg.firmware.signature.get(), msg.firmware.signingCertificate.get()};
                int32_t retries = 0;
                const auto total_retries = msg.retries.get_value_or(this->config.DefaultRetries);
                const auto retry_interval = msg.retryInterval.get_value_or(this->config.DefaultRetryInterval);
                ocpp1_6::FirmwareStatusEnumType status = ocpp1_6::FirmwareStatusEnumType::DownloadFailed;

                while (status == ocpp1_6::FirmwareStatusEnumType::DownloadFailed && retries <= total_retries) {
                    boost::process::ipstream download_stream;
                    boost::process::child download_cmd(firmware_downloader, boost::process::args(download_args),
                                                       boost::process::std_out > download_stream);
                    std::string temp;
                    retries += 1;
                    while (std::getline(download_stream, temp)) {
                        status = ocpp1_6::conversions::string_to_firmware_status_enum_type(temp);
                        this->charge_point->signedFirmwareUpdateStatusNotification(status, msg.requestId);
                    }
                    if (status == ocpp1_6::FirmwareStatusEnumType::DownloadFailed) {
                        std::this_thread::sleep_for(std::chrono::seconds(retry_interval));
                    }
                }
                // FIXME(piet): This can be removed when we actually update the firmware and reboot
                if (status == ocpp1_6::FirmwareStatusEnumType::SignatureVerified) {
                    this->charge_point->notify_signed_firmware_update_downloaded(msg, firmware_file_path);
                } else {
                    this->charge_point->set_signed_firmware_update_running(false);
                }
            });
            this->signed_update_firmware_thread.detach();
        });

    this->charge_point->register_signed_update_firmware_install_callback(
        [this](ocpp1_6::SignedUpdateFirmwareRequest msg, boost::filesystem::path file_path) {
            this->signed_update_firmware_thread = std::thread([this, msg]() {
                boost::process::ipstream install_stream;
                const auto firmware_installer =
                    boost::filesystem::path(this->config.ScriptsPath) / "signed_firmware_installer.sh";
                const auto constants = boost::filesystem::path(this->config.ScriptsPath) / "constants.env";
                const std::vector<std::string> install_args = {constants.string()};
                boost::process::child install_cmd(firmware_installer, boost::process::args(install_args),
                                                  boost::process::std_out > install_stream);
                std::string temp;
                ocpp1_6::FirmwareStatusEnumType status;

                while (std::getline(install_stream, temp)) {
                    status = ocpp1_6::conversions::string_to_firmware_status_enum_type(temp);
                    this->charge_point->signedFirmwareUpdateStatusNotification(status, msg.requestId);
                }
                if (status == ocpp1_6::FirmwareStatusEnumType::Installed) {
                    this->charge_point->trigger_boot_notification(); // to make OCTT happy
                }
                this->charge_point->set_signed_firmware_update_running(false);
            });
            this->signed_update_firmware_thread.detach();
        });

    this->charge_point->register_remote_start_transaction_callback(
        [this](int32_t connector, ocpp1_6::CiString20Type idTag) {
            Object id_tag;
            id_tag["token"] = idTag;
            id_tag["type"] = "ocpp_authorized";
            id_tag["timeout"] = 10;
            this->p_auth_provider->publish_token(id_tag);
        });

    this->charge_point->register_disable_evse_callback([this](int32_t connector) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->r_evse_manager.at(connector - 1)->call_disable();
        } else {
            return false;
        }
    });

    this->charge_point->register_enable_evse_callback([this](int32_t connector) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->r_evse_manager.at(connector - 1)->call_enable();
        } else {
            return false;
        }
    });

    this->charge_point->start();

    int32_t connector = 1;
    for (auto& evse : this->r_evse_manager) {
        evse->subscribe_powermeter([this, connector](types::powermeter::Powermeter powermeter) {
            json powermeter_json = powermeter;
            this->charge_point->receive_power_meter(connector, powermeter_json); //
        });

        evse->subscribe_limits([this, connector](Object limits) {
            auto max_current = limits["max_current"].get<double>();
            this->charge_point->receive_max_current_offered(connector, max_current);

            auto number_of_phases = limits["nr_of_phases_available"].get<int32_t>();
            this->charge_point->receive_number_of_phases_available(connector, number_of_phases);
        });

        evse->subscribe_session_events([this, connector](types::evse_manager::SessionEvents session_events) {
            auto event = types::evse_manager::session_event_to_string(session_events.event);
            if (event == "Enabled") {
                // TODO(kai): decide if we need to inform libocpp about such an event
            } else if (event == "Disabled") {
                // TODO(kai): decide if we need to inform libocpp about such an event
            } else if (event == "AuthRequired") {
                auto authorized_id_tag = this->charge_point->get_authorized_id_tag(connector);
                if (authorized_id_tag != boost::none) {
                    Object id_tag;
                    id_tag["token"] = authorized_id_tag.get();
                    id_tag["type"] = "ocpp_authorized";
                    id_tag["timeout"] = 10;
                    this->p_auth_provider->publish_token(id_tag);
                }
            } else if (event == "SessionStarted") {
                auto session_started = session_events.session_started.value();
                auto timestamp = ocpp1_6::DateTime(std::chrono::time_point<date::utc_clock>(
                    std::chrono::seconds(static_cast<int>(session_started.timestamp))));
                auto energy_Wh_import = session_started.energy_Wh_import;
                boost::optional<int32_t> reservation_id_opt = boost::none;
                if (session_started.reservation_id) {
                    reservation_id_opt.emplace(session_started.reservation_id.value());
                }
                this->charge_point->start_session(connector, timestamp, energy_Wh_import, reservation_id_opt);
            } else if (event == "ChargingStarted") {
                this->charge_point->start_charging(connector);
            } else if (event == "ChargingPausedEV") {
                this->charge_point->suspend_charging_ev(connector);
            } else if (event == "ChargingPausedEVSE") {
                this->charge_point->suspend_charging_evse(connector);
            } else if (event == "ChargingResumed") {
                this->charge_point->resume_charging(connector);
            } else if (event == "SessionCancelled") {
                auto session_cancelled = session_events.session_cancelled.value();
                auto timestamp = std::chrono::time_point<date::utc_clock>(
                    std::chrono::seconds(static_cast<int>(session_cancelled.timestamp)));
                auto energy_Wh_import = session_cancelled.energy_Wh_import;
                auto reason = types::evse_manager::session_cancellation_reason_to_string(session_cancelled.reason.value());
                // always triggered by libocpp
                this->charge_point->cancel_session(connector, ocpp1_6::DateTime(timestamp), energy_Wh_import,
                                                   ocpp1_6::conversions::string_to_reason(reason));
            } else if (event == "SessionFinished") {
                // ev side disconnect
                auto session_finished = session_events.session_finished.value();
                auto timestamp = std::chrono::time_point<date::utc_clock>(
                    std::chrono::seconds(static_cast<int>(session_finished.timestamp)));
                auto energy_Wh_import = session_finished.energy_Wh_import;
                this->charge_point->stop_session(connector, ocpp1_6::DateTime(timestamp), energy_Wh_import);
                this->charge_point->plug_disconnected(connector);
            } else if (event == "Error") {
                const auto evse_error = types::evse_manager::error_to_string(session_events.error.value());
                ocpp1_6::ChargePointErrorCode ocpp_error_code = get_ocpp_error_code(evse_error);
                this->charge_point->error(connector, ocpp_error_code);
            } else if (event == "PermanentFault") {
                this->charge_point->permanent_fault(connector);
            } else if (event == "ReservationStart") {
                auto reservation_start = session_events.reservation_start.value();
                auto reservation_id = reservation_start.reservation_id;
                auto id_tag = reservation_start.id_tag;
                this->charge_point->reservation_start(connector, reservation_id, id_tag);
            } else if (event == "ReservationEnd") {
                auto reservation_end = session_events.reservation_end.value();
                auto reservation_id = reservation_end.reservation_id;
                auto reason = types::evse_manager::reservation_end_reason_to_string(reservation_end.reason);
                this->charge_point->reservation_end(connector, reservation_id, reason);
            } else if (event == "ReservationAuthtokenMismatch") {
            }
        });

        connector += 1;
    }

    if (this->config.EnableExternalWebsocketControl) {
        const std::string connect_topic = "everest_api/ocpp/cmd/connect";
        this->mqtt.subscribe(connect_topic,
                             [this](const std::string& data) { this->charge_point->connect_websocket(); });

        const std::string disconnect_topic = "everest_api/ocpp/cmd/disconnect";
        this->mqtt.subscribe(disconnect_topic,
                             [this](const std::string& data) { this->charge_point->disconnect_websocket(); });
    }
}

void OCPP::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_auth_validator);
    invoke_ready(*p_auth_provider);
}

} // namespace module
