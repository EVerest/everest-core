// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "OCPP.hpp"
#include <fmt/core.h>
#include <fstream>

#include <boost/process.hpp>

namespace module {

const std::string EVEREST_OCPP_SHARE_PATH = "share/everest/ocpp";
const std::string INIT_SQL = "init.sql";

namespace fs = std::filesystem;

static ocpp::v16::ChargePointErrorCode get_ocpp_error_code(const std::string& evse_error) {
    if (evse_error == "Car") {
        return ocpp::v16::ChargePointErrorCode::OtherError;
    } else if (evse_error == "CarDiodeFault") {
        return ocpp::v16::ChargePointErrorCode::EVCommunicationError;
    } else if (evse_error == "Relais") {
        return ocpp::v16::ChargePointErrorCode::OtherError;
    } else if (evse_error == "RCD") {
        return ocpp::v16::ChargePointErrorCode::GroundFailure;
    } else if (evse_error == "VentilationNotAvailable") {
        return ocpp::v16::ChargePointErrorCode::OtherError;
    } else if (evse_error == "OverCurrent") {
        return ocpp::v16::ChargePointErrorCode::OverCurrentFailure;
    } else if (evse_error == "Internal") {
        return ocpp::v16::ChargePointErrorCode::OtherError;
    } else if (evse_error == "SLAC") {
        return ocpp::v16::ChargePointErrorCode::EVCommunicationError;
    } else if (evse_error == "HLC") {
        return ocpp::v16::ChargePointErrorCode::EVCommunicationError;
    } else {
        return ocpp::v16::ChargePointErrorCode::OtherError;
    }
}

void create_empty_user_config(const fs::path& user_config_path) {
    if (fs::exists(user_config_path.parent_path())) {
        std::ofstream fs(user_config_path.c_str());
        auto user_config = json::object();
        fs << user_config << std::endl;
        fs.close();
    } else {
        EVLOG_AND_THROW(
            std::runtime_error(fmt::format("Provided UserConfigPath is invalid: {}", user_config_path.string())));
    }
}

void OCPP::publish_charging_schedules() {
    const auto charging_schedules =
        this->charge_point->get_all_composite_charging_schedules(this->config.PublishChargingScheduleDurationS);
    Object j;
    for (const auto charging_schedule : charging_schedules) {
        j[std::to_string(charging_schedule.first)] = charging_schedule.second;
    }
    this->p_main->publish_charging_schedules(j);
}

void OCPP::init() {
    invoke_init(*p_main);
    invoke_init(*p_auth_validator);
    invoke_init(*p_auth_provider);

    this->ocpp_share_path = fs::path(this->info.everest_prefix) / EVEREST_OCPP_SHARE_PATH;

    auto configured_config_path = fs::path(this->config.ChargePointConfigPath);

    // try to find the config file if it has been provided as a relative path
    if (!fs::exists(configured_config_path) && configured_config_path.is_relative()) {
        configured_config_path = this->ocpp_share_path / configured_config_path;
    }
    if (!fs::exists(configured_config_path)) {
        EVLOG_AND_THROW(Everest::EverestConfigError(
            fmt::format("OCPP config file is not available at given path: {} which was resolved to: {}",
                        this->config.ChargePointConfigPath, configured_config_path.string())));
    }
    const auto config_path = configured_config_path;
    EVLOG_info << "OCPP config: " << config_path.string();

    auto configured_user_config_path = fs::path(this->config.UserConfigPath);
    // try to find the user config file if it has been provided as a relative path
    if (!fs::exists(configured_user_config_path) && configured_user_config_path.is_relative()) {
        configured_user_config_path = this->ocpp_share_path / configured_user_config_path;
    }
    const auto user_config_path = configured_user_config_path;
    EVLOG_info << "OCPP user config: " << user_config_path.string();

    std::ifstream ifs(config_path.c_str());
    std::string config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    auto json_config = json::parse(config_file);
    json_config.at("Core").at("NumberOfConnectors") = this->r_evse_manager.size();

    if (fs::exists(user_config_path)) {
        std::ifstream ifs(user_config_path.c_str());
        std::string user_config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

        try {
            const auto user_config = json::parse(user_config_file);
            EVLOG_info << "Augmenting chargepoint config with user_config entries";
            json_config.merge_patch(user_config);
        } catch (const json::parse_error& e) {
            EVLOG_error << "Error while parsing user config file.";
            EVLOG_AND_THROW(e);
        }
    } else {
        EVLOG_debug << "No user-config provided. Creating user config file";
        create_empty_user_config(user_config_path);
    }

    const auto sql_init_path = this->ocpp_share_path / INIT_SQL;
    if (!fs::exists(this->config.MessageLogPath)) {
        try {
            fs::create_directory(this->config.MessageLogPath);
        } catch (const fs::filesystem_error& e) {
            EVLOG_AND_THROW(e);
        }
    }
    this->charge_point =
        std::make_unique<ocpp::v16::ChargePoint>(json_config, this->ocpp_share_path.string(), user_config_path.string(),
                                   this->config.DatabasePath, sql_init_path.string(), this->config.MessageLogPath);
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
    this->charge_point->register_stop_transaction_callback([this](int32_t connector, ocpp::v16::Reason reason) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            types::evse_manager::StopTransactionRequest req;
            req.reason = types::evse_manager::string_to_stop_transaction_reason(
                ocpp::v16::conversions::reason_to_string(reason));
            return this->r_evse_manager.at(connector - 1)->call_stop_transaction(req);
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

    // int32_t reservation_id, CiString<20> auth_token, DateTime expiry_time, std::string parent_id
    this->charge_point->register_reserve_now_callback([this](int32_t reservation_id, int32_t connector,
                                                             ocpp::DateTime expiryDate, ocpp::CiString<20> idTag,
                                                             boost::optional<ocpp::CiString<20>> parent_id) {
        types::reservation::Reservation reservation;
        reservation.id_token = idTag.get();
        reservation.reservation_id = reservation_id;
        reservation.expiry_time = expiryDate.to_rfc3339();
        if (parent_id) {
            reservation.parent_id_token.emplace(parent_id.value().get());
        }
        auto response = this->r_reservation->call_reserve_now(connector, reservation);
        return ocpp::v16::conversions::string_to_reservation_status(
            types::reservation::reservation_result_to_string(response));
    });

    this->charge_point->register_upload_diagnostics_callback([this](const ocpp::v16::GetDiagnosticsRequest& msg) {
        types::system::UploadLogsRequest upload_logs_request;
        upload_logs_request.location = msg.location;

        if (msg.stopTime.has_value()) {
            upload_logs_request.latest_timestamp.emplace(msg.stopTime.value().to_rfc3339());
        }
        if (msg.startTime.has_value()) {
            upload_logs_request.oldest_timestamp.emplace(msg.startTime.value().to_rfc3339());
        }
        if (msg.retries.has_value()) {
            upload_logs_request.retries.emplace(msg.retries.value());
        }
        if (msg.retryInterval.has_value()) {
            upload_logs_request.retry_interval_s.emplace(msg.retryInterval.value());
        }
        const auto upload_logs_response = this->r_system->call_upload_logs(upload_logs_request);
        ocpp::v16::GetLogResponse response;
        if (upload_logs_response.file_name.has_value()) {
            response.filename.emplace(ocpp::CiString<255>(upload_logs_response.file_name.value()));
        }
        response.status = ocpp::v16::conversions::string_to_log_status_enum_type(
            types::system::upload_logs_status_to_string(upload_logs_response.upload_logs_status));
        return response;
    });

    this->charge_point->register_upload_logs_callback([this](ocpp::v16::GetLogRequest msg) {
        types::system::UploadLogsRequest upload_logs_request;
        upload_logs_request.location = msg.log.remoteLocation;
        upload_logs_request.request_id = msg.requestId;
        upload_logs_request.type = ocpp::v16::conversions::log_enum_type_to_string(msg.logType);

        if (msg.log.latestTimestamp.has_value()) {
            upload_logs_request.latest_timestamp.emplace(msg.log.latestTimestamp.value().to_rfc3339());
        }
        if (msg.log.oldestTimestamp.has_value()) {
            upload_logs_request.oldest_timestamp.emplace(msg.log.oldestTimestamp.value().to_rfc3339());
        }
        if (msg.retries.has_value()) {
            upload_logs_request.retries.emplace(msg.retries.value());
        }
        if (msg.retryInterval.has_value()) {
            upload_logs_request.retry_interval_s.emplace(msg.retryInterval.value());
        }

        const auto upload_logs_response = this->r_system->call_upload_logs(upload_logs_request);

        ocpp::v16::GetLogResponse response;
        if (upload_logs_response.file_name.has_value()) {
            response.filename.emplace(ocpp::CiString<255>(upload_logs_response.file_name.value()));
        }
        response.status = ocpp::v16::conversions::string_to_log_status_enum_type(
            types::system::upload_logs_status_to_string(upload_logs_response.upload_logs_status));
        return response;
    });
    this->charge_point->register_update_firmware_callback([this](const ocpp::v16::UpdateFirmwareRequest msg) {
        types::system::FirmwareUpdateRequest firmware_update_request;
        firmware_update_request.location = msg.location;
        firmware_update_request.request_id = -1;
        firmware_update_request.retrieve_timestamp.emplace(msg.retrieveDate.to_rfc3339());
        if (msg.retries.has_value()) {
            firmware_update_request.retries.emplace(msg.retries.value());
        }
        if (msg.retryInterval.has_value()) {
            firmware_update_request.retry_interval_s.emplace(msg.retryInterval.value());
        }
        this->r_system->call_update_firmware(firmware_update_request);
    });

    this->charge_point->register_signed_update_firmware_callback([this](ocpp::v16::SignedUpdateFirmwareRequest msg) {
        types::system::FirmwareUpdateRequest firmware_update_request;
        firmware_update_request.request_id = msg.requestId;
        firmware_update_request.location = msg.firmware.location;
        firmware_update_request.retrieve_timestamp.emplace(msg.firmware.retrieveDateTime.to_rfc3339());
        firmware_update_request.signature.emplace(msg.firmware.signature.get());
        firmware_update_request.signing_certificate.emplace(msg.firmware.signingCertificate.get());

        if (msg.firmware.installDateTime.has_value()) {
            firmware_update_request.install_timestamp.emplace(msg.firmware.installDateTime.value());
        }
        if (msg.retries.has_value()) {
            firmware_update_request.retries.emplace(msg.retries.value());
        }
        if (msg.retryInterval.has_value()) {
            firmware_update_request.retry_interval_s.emplace(msg.retryInterval.value());
        }

        const auto system_response = this->r_system->call_update_firmware(firmware_update_request);

        return ocpp::v16::conversions::string_to_update_firmware_status_enum_type(
            types::system::update_firmware_response_to_string(system_response));
    });

    this->r_system->subscribe_log_status([this](types::system::LogStatus log_status) {
        this->charge_point->on_log_status_notification(log_status.request_id,
                                                       types::system::log_status_enum_to_string(log_status.log_status));
    });

    this->r_system->subscribe_firmware_update_status(
        [this](types::system::FirmwareUpdateStatus firmware_update_status) {
            this->charge_point->on_firmware_update_status_notification(
                firmware_update_status.request_id,
                types::system::firmware_update_status_enum_to_string(firmware_update_status.firmware_update_status));
        });

    this->charge_point->register_provide_token_callback(
        [this](const std::string& id_token, std::vector<int32_t> referenced_connectors, bool prevalidated) {
            types::authorization::ProvidedIdToken provided_token;
            provided_token.id_token = id_token;
            provided_token.type = types::authorization::TokenType::OCPP;
            provided_token.connectors.emplace(referenced_connectors);
            provided_token.prevalidated.emplace(prevalidated);
            this->p_auth_provider->publish_provided_token(provided_token);
        });

    this->charge_point->register_set_connection_timeout_callback(
        [this](int32_t connection_timeout) { this->r_auth->call_set_connection_timeout(connection_timeout); });

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

    this->charge_point->register_cancel_reservation_callback(
        [this](int32_t reservation_id) { return this->r_reservation->call_cancel_reservation(reservation_id); });

    if (this->config.EnableExternalWebsocketControl) {
        const std::string connect_topic = "everest_api/ocpp/cmd/connect";
        this->mqtt.subscribe(connect_topic,
                             [this](const std::string& data) { this->charge_point->connect_websocket(); });

        const std::string disconnect_topic = "everest_api/ocpp/cmd/disconnect";
        this->mqtt.subscribe(disconnect_topic,
                             [this](const std::string& data) { this->charge_point->disconnect_websocket(); });
    }

    this->charging_schedules_timer =
        std::make_unique<Everest::SteadyTimer>([this]() { this->publish_charging_schedules(); });
    this->charging_schedules_timer->interval(std::chrono::seconds(this->config.PublishChargingScheduleIntervalS));

    this->charge_point->register_signal_set_charging_profiles_callback(
        [this]() { this->publish_charging_schedules(); });

    this->charge_point->register_is_reset_allowed_callback([this](ocpp::v16::ResetType type) {
        const auto reset_type = types::system::string_to_reset_type(ocpp::v16::conversions::reset_type_to_string(type));
        return this->r_system->call_is_reset_allowed(reset_type);
    });

    this->charge_point->register_reset_callback([this](ocpp::v16::ResetType type) {
        const auto reset_type = types::system::string_to_reset_type(ocpp::v16::conversions::reset_type_to_string(type));
        this->r_system->call_reset(reset_type);
    });

    this->charge_point->register_connection_state_changed_callback(
        [this](bool is_connected) { this->p_main->publish_is_connected(is_connected); });

    int32_t connector = 1;
    for (auto& evse : this->r_evse_manager) {
        evse->subscribe_powermeter([this, connector](types::powermeter::Powermeter powermeter) {
            json powermeter_json = powermeter;
            this->charge_point->on_meter_values(connector, powermeter_json); //
        });

        evse->subscribe_limits([this, connector](types::evse_manager::Limits limits) {
            double max_current = limits.max_current;
            this->charge_point->on_max_current_offered(connector, max_current);
        });

        evse->subscribe_session_event([this, connector](types::evse_manager::SessionEvent session_event) {
            auto event = types::evse_manager::session_event_enum_to_string(session_event.event);

            if (event == "Enabled") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received Enabled";
            } else if (event == "Disabled") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received Disabled";
            } else if (event == "TransactionStarted") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received TransactionStarted";
                const auto transaction_started = session_event.transaction_started.value();

                const auto timestamp = ocpp::DateTime(transaction_started.timestamp);
                const auto energy_Wh_import = transaction_started.energy_Wh_import;
                const auto session_id = session_event.uuid;
                const auto id_token = transaction_started.id_tag;
                const auto signed_meter_value = transaction_started.signed_meter_value;
                boost::optional<int32_t> reservation_id_opt = boost::none;
                if (transaction_started.reservation_id) {
                    reservation_id_opt.emplace(transaction_started.reservation_id.value());
                }
                this->charge_point->on_transaction_started(connector, session_event.uuid, id_token, energy_Wh_import,
                                                           reservation_id_opt, timestamp, signed_meter_value);
            } else if (event == "ChargingPausedEV") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received ChargingPausedEV";
                this->charge_point->on_suspend_charging_ev(connector);
            } else if (event == "ChargingPausedEVSE") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received ChargingPausedEVSE";
                this->charge_point->on_suspend_charging_evse(connector);
            } else if (event == "ChargingStarted" || event == "ChargingResumed") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received ChargingResumed";
                this->charge_point->on_resume_charging(connector);
            } else if (event == "TransactionFinished") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received TransactionFinished";
                const auto transaction_finished = session_event.transaction_finished.value();
                const auto timestamp = ocpp::DateTime(transaction_finished.timestamp);
                const auto energy_Wh_import = transaction_finished.energy_Wh_import;
                const auto reason = ocpp::v16::conversions::string_to_reason(
                    types::evse_manager::stop_transaction_reason_to_string(transaction_finished.reason.value()));
                const auto signed_meter_value = transaction_finished.signed_meter_value;
                boost::optional<ocpp::CiString<20>> id_tag_opt = boost::none;
                if (transaction_finished.id_tag) {
                    id_tag_opt.emplace(ocpp::CiString<20>(transaction_finished.id_tag.value()));
                }
                this->charge_point->on_transaction_stopped(connector, session_event.uuid, reason, timestamp,
                                                           energy_Wh_import, id_tag_opt, signed_meter_value);
                // always triggered by libocpp
            } else if (event == "SessionStarted") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received SessionStarted";
                // ev side disconnect
                auto session_started = session_event.session_started.value();
                this->charge_point->on_session_started(
                    connector, session_event.uuid,
                    types::evse_manager::start_session_reason_to_string(session_started.reason));
            } else if (event == "SessionFinished") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received SessionFinished";
                // ev side disconnect
                this->charge_point->on_session_stopped(connector);
            } else if (event == "Error") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received Error";
                const auto evse_error = types::evse_manager::error_to_string(session_event.error.value());
                ocpp::v16::ChargePointErrorCode ocpp_error_code = get_ocpp_error_code(evse_error);
                this->charge_point->on_error(connector, ocpp_error_code);
            } else if (event == "ReservationStart") {
                this->charge_point->on_reservation_start(connector);
            } else if (event == "ReservationEnd") {
                this->charge_point->on_reservation_end(connector);
            } else if (event == "ReservationAuthtokenMismatch") {
            }
        });
        connector++;
    }
    this->charge_point->start();
}

void OCPP::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_auth_validator);
    invoke_ready(*p_auth_provider);

    this->charge_point->call_set_connection_timeout();
    int connector = 1;
    for (const auto& evse : this->r_evse_manager) {
        if (connector != evse->call_get_id()) {
            EVLOG_AND_THROW(std::runtime_error("Connector Ids of EVSE manager are not configured in ascending order "
                                               "starting with 1. This is mandatory when using the OCPP module"));
        }
        connector++;
    }
}

} // namespace module
