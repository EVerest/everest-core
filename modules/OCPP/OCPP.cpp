// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "OCPP.hpp"
#include <fmt/core.h>
#include <fstream>

#include <boost/process.hpp>

namespace module {

const std::string CERTS_SUB_DIR = "certs";
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
    } else if (evse_error == "NoError") {
        return ocpp::v16::ChargePointErrorCode::NoError;
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

void OCPP::set_external_limits(const std::map<int32_t, ocpp::v16::ChargingSchedule>& charging_schedules) {
    const auto start_time = ocpp::DateTime();

    // iterate over all schedules reported by the libocpp to create ExternalLimits
    // for each connector
    for (auto const& [connector_id, schedule] : charging_schedules) {
        types::energy::ExternalLimits limits;
        std::vector<types::energy::ScheduleReqEntry> schedule_import;
        for (const auto period : schedule.chargingSchedulePeriod) {
            types::energy::ScheduleReqEntry schedule_req_entry;
            types::energy::LimitsReq limits_req;
            const auto timestamp = start_time.to_time_point() + std::chrono::seconds(period.startPeriod);
            schedule_req_entry.timestamp = ocpp::DateTime(timestamp).to_rfc3339();
            if (schedule.chargingRateUnit == ocpp::v16::ChargingRateUnit::A) {
                limits_req.ac_max_current_A = period.limit;
                if (period.numberPhases.has_value()) {
                    limits_req.ac_max_phase_count = period.numberPhases.value();
                }
                if (schedule.minChargingRate.has_value()) {
                    limits_req.ac_min_current_A = schedule.minChargingRate.value();
                }
            } else {
                limits_req.total_power_W = period.limit;
            }
            schedule_req_entry.limits_to_leaves = limits_req;
            schedule_import.push_back(schedule_req_entry);
        }
        limits.schedule_import.emplace(schedule_import);

        if (connector_id == 0) {
            if (!this->r_connector_zero_sink.empty()) {
                EVLOG_debug << "OCPP sets the following external limits for connector 0: \n" << limits;
                this->r_connector_zero_sink.at(0)->call_set_external_limits(limits);
            } else {
                EVLOG_debug << "OCPP cannot set external limits for connector 0. No "
                               "sink is configured.";
            }
        } else {
            EVLOG_debug << "OCPP sets the following external limits for connector " << connector_id << ": \n" << limits;
            this->r_evse_manager.at(connector_id - 1)->call_set_external_limits(limits);
        }
    }
}

void OCPP::publish_charging_schedules(const std::map<int32_t, ocpp::v16::ChargingSchedule>& charging_schedules) {
    // publish the schedule over mqtt
    Object j;
    for (const auto charging_schedule : charging_schedules) {
        j[std::to_string(charging_schedule.first)] = charging_schedule.second;
    }
    this->p_main->publish_charging_schedules(j);
}

bool OCPP::all_evse_ready() {
    for (auto const& [connector, ready] : this->connector_ready_map) {
        if (!ready) {
            return false;
        }
    }
    return true;
}

void OCPP::init() {
    invoke_init(*p_main);
    invoke_init(*p_auth_validator);
    invoke_init(*p_auth_provider);

    this->ocpp_share_path = this->info.paths.share;

    const auto etc_certs_path = [&]() {
        if (this->config.CertsPath.empty()) {
            return this->info.paths.etc / CERTS_SUB_DIR;
        } else {
            return fs::path(this->config.CertsPath);
        }
    }();
    EVLOG_info << "OCPP certificates path: " << etc_certs_path.string();

    auto configured_config_path = fs::path(this->config.ChargePointConfigPath);

    // try to find the config file if it has been provided as a relative path
    if (!fs::exists(configured_config_path) && configured_config_path.is_relative()) {
        configured_config_path = this->ocpp_share_path / configured_config_path;
    }
    if (!fs::exists(configured_config_path)) {
        EVLOG_AND_THROW(Everest::EverestConfigError(
            fmt::format("OCPP config file is not available at given path: {} which was "
                        "resolved to: {}",
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

    this->charge_point = std::make_unique<ocpp::v16::ChargePoint>(
        json_config.dump(), this->ocpp_share_path, user_config_path, std::filesystem::path(this->config.DatabasePath),
        sql_init_path, std::filesystem::path(this->config.MessageLogPath), etc_certs_path);

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
            return this->r_evse_manager.at(connector - 1)->call_force_unlock(1);
        } else {
            return false;
        }
    });

    // int32_t reservation_id, CiString<20> auth_token, DateTime expiry_time,
    // std::string parent_id
    this->charge_point->register_reserve_now_callback([this](int32_t reservation_id, int32_t connector,
                                                             ocpp::DateTime expiryDate, ocpp::CiString<20> idTag,
                                                             std::optional<ocpp::CiString<20>> parent_id) {
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
            provided_token.authorization_type = types::authorization::AuthorizationType::OCPP;
            provided_token.connectors.emplace(referenced_connectors);
            provided_token.prevalidated.emplace(prevalidated);
            this->p_auth_provider->publish_provided_token(provided_token);
        });

    this->charge_point->register_set_connection_timeout_callback(
        [this](int32_t connection_timeout) { this->r_auth->call_set_connection_timeout(connection_timeout); });

    this->charge_point->register_disable_evse_callback([this](int32_t connector) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->r_evse_manager.at(connector - 1)->call_disable(0);
        } else {
            return false;
        }
    });

    this->charge_point->register_enable_evse_callback([this](int32_t connector) {
        if (connector > 0 && connector <= this->r_evse_manager.size()) {
            return this->r_evse_manager.at(connector - 1)->call_enable(0);
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

    this->charging_schedules_timer = std::make_unique<Everest::SteadyTimer>([this]() {
        const auto charging_schedules =
            this->charge_point->get_all_composite_charging_schedules(this->config.PublishChargingScheduleDurationS);
        this->set_external_limits(charging_schedules);
        this->publish_charging_schedules(charging_schedules);
    });
    if (this->config.PublishChargingScheduleIntervalS > 0) {
        this->charging_schedules_timer->interval(std::chrono::seconds(this->config.PublishChargingScheduleIntervalS));
    }

    this->charge_point->register_signal_set_charging_profiles_callback([this]() {
        // this is executed when CSMS sends new ChargingProfile that is accepted by
        // the ChargePoint
        EVLOG_info << "Received new Charging Schedules from CSMS";
        const auto charging_schedules =
            this->charge_point->get_all_composite_charging_schedules(this->config.PublishChargingScheduleDurationS);
        this->set_external_limits(charging_schedules);
        this->publish_charging_schedules(charging_schedules);
    });

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

    this->charge_point->register_get_15118_ev_certificate_response_callback(
        [this](const int32_t connector_id, const ocpp::v201::Get15118EVCertificateResponse& certificate_response,
               const ocpp::v201::CertificateActionEnum& certificate_action) {
            types::iso15118_charger::Response_Exi_Stream_Status response;
            response.status = types::iso15118_charger::string_to_status(
                ocpp::v201::conversions::iso15118evcertificate_status_enum_to_string(certificate_response.status));
            response.exiResponse.emplace(certificate_response.exiResponse.get());
            response.certificateAction = types::iso15118_charger::string_to_certificate_action_enum(
                ocpp::v201::conversions::certificate_action_enum_to_string(certificate_action));
            this->r_evse_manager.at(connector_id - 1)->call_set_get_certificate_response(response);
        });

    int32_t connector = 1;
    for (auto& evse : this->r_evse_manager) {
        connector_ready_map.insert({connector, false});

        evse->subscribe_powermeter([this, connector](types::powermeter::Powermeter powermeter) {
            json powermeter_json = powermeter;
            this->charge_point->on_meter_values(connector, powermeter_json); //
        });

        evse->subscribe_limits([this, connector](types::evse_manager::Limits limits) {
            double max_current = limits.max_current;
            this->charge_point->on_max_current_offered(connector, max_current);
        });

        evse->subscribe_session_event([this, connector](types::evse_manager::SessionEvent session_event) {
            if (this->ocpp_stopped) {
                // dont call any on handler in case ocpp is stopped
                return;
            }

            auto event = types::evse_manager::session_event_enum_to_string(session_event.event);

            if (event == "Enabled") {
                std::lock_guard<std::mutex> lk(this->evse_ready_mutex);
                this->connector_ready_map.at(connector) = true;
                if (started) {
                    this->charge_point->on_enabled(connector);
                } else {
                    // EvseManager sends initial enable on startup
                    if (this->all_evse_ready()) {
                        EVLOG_info << "All EVSE ready: Starting OCPP";
                        started = true;
                        this->charge_point->start();
                    }
                }
            } else if (event == "Disabled") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received Disabled";
                this->charge_point->on_disabled(connector);
            } else if (event == "TransactionStarted") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received TransactionStarted";
                const auto transaction_started = session_event.transaction_started.value();

                const auto timestamp = ocpp::DateTime(transaction_started.timestamp);
                const auto energy_Wh_import = transaction_started.meter_value.energy_Wh_import.total;
                const auto session_id = session_event.uuid;
                const auto id_token = transaction_started.id_tag.id_token;
                const auto signed_meter_value = transaction_started.signed_meter_value;
                std::optional<int32_t> reservation_id_opt = std::nullopt;
                if (transaction_started.reservation_id) {
                    reservation_id_opt.emplace(transaction_started.reservation_id.value());
                }
                this->charge_point->on_transaction_started(connector, session_event.uuid, id_token, energy_Wh_import,
                                                           reservation_id_opt, timestamp, signed_meter_value);
            } else if (event == "ChargingPausedEV") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received ChargingPausedEV";
                this->charge_point->on_suspend_charging_ev(connector);
            } else if (event == "ChargingPausedEVSE" or event == "WaitingForEnergy") {
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
                const auto energy_Wh_import = transaction_finished.meter_value.energy_Wh_import.total;
                const auto reason = ocpp::v16::conversions::string_to_reason(
                    types::evse_manager::stop_transaction_reason_to_string(transaction_finished.reason.value()));
                const auto signed_meter_value = transaction_finished.signed_meter_value;
                std::optional<ocpp::CiString<20>> id_tag_opt = std::nullopt;
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
                    types::evse_manager::start_session_reason_to_string(session_started.reason),
                    session_started.logging_path);
            } else if (event == "SessionFinished") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received SessionFinished";
                // ev side disconnect
                this->charge_point->on_session_stopped(connector, session_event.uuid);
            } else if (event == "Error") {
                EVLOG_debug << "Connector#" << connector << ": "
                            << "Received Error";
                const auto evse_error =
                    types::evse_manager::error_enum_to_string(session_event.error.value().error_code);
                ocpp::v16::ChargePointErrorCode ocpp_error_code = get_ocpp_error_code(evse_error);
                this->charge_point->on_error(connector, ocpp_error_code);
            } else if (event == "AllErrorsCleared") {
                this->charge_point->on_fault(connector, ocpp::v16::ChargePointErrorCode::NoError);
            } else if (event == "PermanentFault") {
                const auto evse_error =
                    types::evse_manager::error_enum_to_string(session_event.error.value().error_code);
                ocpp::v16::ChargePointErrorCode ocpp_error_code = get_ocpp_error_code(evse_error);
                this->charge_point->on_fault(connector, ocpp_error_code);
            } else if (event == "ReservationStart") {
                this->charge_point->on_reservation_start(connector);
            } else if (event == "ReservationEnd") {
                this->charge_point->on_reservation_end(connector);
            } else if (event == "ReservationAuthtokenMismatch") {
            } else if (event == "PluginTimeout") {
                this->charge_point->on_plugin_timeout(connector);
            }
        });

        evse->subscribe_iso15118_certificate_request(
            [this, connector](types::iso15118_charger::Request_Exi_Stream_Schema request) {
                this->charge_point->data_transfer_pnc_get_15118_ev_certificate(
                    connector, request.exiRequest, request.iso15118SchemaVersion,
                    ocpp::v201::conversions::string_to_certificate_action_enum(
                        types::iso15118_charger::certificate_action_enum_to_string(request.certificateAction)));
            });

        connector++;
    }
}

void OCPP::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_auth_validator);
    invoke_ready(*p_auth_provider);

    this->charge_point->call_set_connection_timeout();
    int connector = 1;
    for (const auto& evse : this->r_evse_manager) {
        if (connector != evse->call_get_evse().id) {
            EVLOG_AND_THROW(std::runtime_error("Connector Ids of EVSE manager are not configured in ascending order "
                                               "starting with 1. This is mandatory when using the OCPP module"));
        }
        connector++;
    }
}

} // namespace module
