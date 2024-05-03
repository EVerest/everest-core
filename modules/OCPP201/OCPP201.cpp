// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "OCPP201.hpp"

#include <fmt/core.h>
#include <fstream>

#include <websocketpp_utils/uri.hpp>

#include <conversions.hpp>
#include <evse_security_ocpp.hpp>
namespace module {

const std::string SQL_CORE_MIGRTATIONS = "core_migrations";
const std::string CERTS_DIR = "certs";

namespace fs = std::filesystem;

TxEvent get_tx_event(const ocpp::v201::ReasonEnum reason) {
    switch (reason) {
    case ocpp::v201::ReasonEnum::DeAuthorized:
    case ocpp::v201::ReasonEnum::Remote:
    case ocpp::v201::ReasonEnum::Local:
    case ocpp::v201::ReasonEnum::MasterPass:
        return TxEvent::DEAUTHORIZED;
    case ocpp::v201::ReasonEnum::EVDisconnected:
        return TxEvent::EV_DISCONNECTED;
    default:
        return TxEvent::NONE;
    }
}

std::set<TxStartStopPoint> get_tx_start_stop_points(const std::string& tx_start_stop_point_csl) {
    std::set<TxStartStopPoint> tx_start_stop_points;
    std::vector<std::string> csv;
    std::string str;
    std::stringstream ss(tx_start_stop_point_csl);
    while (std::getline(ss, str, ',')) {
        csv.push_back(str);
    }

    for (const auto tx_start_stop_point : csv) {
        if (tx_start_stop_point == "ParkingBayOccupancy") {
            tx_start_stop_points.insert(TxStartStopPoint::ParkingBayOccupancy);
        } else if (tx_start_stop_point == "EVConnected") {
            tx_start_stop_points.insert(TxStartStopPoint::EVConnected);
        } else if (tx_start_stop_point == "Authorized") {
            tx_start_stop_points.insert(TxStartStopPoint::Authorized);
        } else if (tx_start_stop_point == "PowerPathClosed") {
            tx_start_stop_points.insert(TxStartStopPoint::PowerPathClosed);
        } else if (tx_start_stop_point == "EnergyTransfer") {
            tx_start_stop_points.insert(TxStartStopPoint::EnergyTransfer);
        } else if (tx_start_stop_point == "DataSigned") {
            tx_start_stop_points.insert(TxStartStopPoint::DataSigned);
        } else {
            // default to PowerPathClosed for now
            tx_start_stop_points.insert(TxStartStopPoint::PowerPathClosed);
        }
    }
    return tx_start_stop_points;
}

void OCPP201::init_evse_ready_map() {
    std::lock_guard<std::mutex> lk(this->evse_ready_mutex);
    for (size_t evse_id = 1; evse_id <= this->r_evse_manager.size(); evse_id++) {
        this->evse_ready_map[evse_id] = false;
    }
}

std::map<int32_t, int32_t> OCPP201::get_connector_structure() {
    std::map<int32_t, int32_t> evse_connector_structure;
    int evse_id = 1;
    for (const auto& evse : this->r_evse_manager) {
        auto _evse = evse->call_get_evse();
        int32_t num_connectors = _evse.connectors.size();

        if (_evse.id != evse_id) {
            throw std::runtime_error("Configured evse_id(s) must start with 1 counting upwards");
        }
        if (num_connectors > 0) {
            int connector_id = 1;
            for (const auto& connector : _evse.connectors) {
                if (connector.id != connector_id) {
                    throw std::runtime_error("Configured connector_id(s) must start with 1 counting upwards");
                }
                connector_id++;
            }
        } else {
            num_connectors = 1;
        }

        evse_connector_structure[evse_id] = num_connectors;
        evse_id++;
    }
    return evse_connector_structure;
}

types::powermeter::Powermeter get_meter_value(const types::evse_manager::SessionEvent& session_event) {
    const auto event_type = session_event.event;
    if (event_type == types::evse_manager::SessionEventEnum::SessionStarted) {
        if (!session_event.session_started.has_value()) {
            throw std::runtime_error("SessionEvent SessionStarted does not contain session_started context");
        }
        return session_event.session_started.value().meter_value;
    } else if (event_type == types::evse_manager::SessionEventEnum::SessionFinished) {
        if (!session_event.session_finished.has_value()) {
            throw std::runtime_error("SessionEvent SessionFinished does not contain session_finished context");
        }
        return session_event.session_finished.value().meter_value;
    } else if (event_type == types::evse_manager::SessionEventEnum::TransactionStarted) {
        if (!session_event.transaction_started.has_value()) {
            throw std::runtime_error("SessionEvent TransactionStarted does not contain transaction_started context");
        }
        return session_event.transaction_started.value().meter_value;
    } else if (event_type == types::evse_manager::SessionEventEnum::TransactionFinished) {
        if (!session_event.transaction_finished.has_value()) {
            throw std::runtime_error("SessionEvent TransactionFinished does not contain transaction_finished context");
        }
        return session_event.transaction_finished.value().meter_value;
    } else if (event_type == types::evse_manager::SessionEventEnum::ChargingStarted or
               event_type == types::evse_manager::SessionEventEnum::ChargingResumed or
               event_type == types::evse_manager::SessionEventEnum::ChargingPausedEV or
               event_type == types::evse_manager::SessionEventEnum::ChargingPausedEVSE) {
        if (!session_event.charging_state_changed_event.has_value()) {
            throw std::runtime_error("SessionEvent does not contain charging_state_changed_event context");
        }
        return session_event.charging_state_changed_event.value().meter_value;
    } else if (event_type == types::evse_manager::SessionEventEnum::Authorized or
               event_type == types::evse_manager::SessionEventEnum::Deauthorized) {
        if (!session_event.authorization_event.has_value()) {
            throw std::runtime_error(
                "SessionEvent Authorized or Deauthorized does not contain authorization_event context");
        }
        return session_event.authorization_event.value().meter_value;
    } else {
        throw std::runtime_error("Could not retrieve meter value from SessionEvent");
    }
}

std::optional<types::units_signed::SignedMeterValue>
get_signed_meter_value(const types::evse_manager::SessionEvent& session_event) {
    const auto event_type = session_event.event;
    if (event_type == types::evse_manager::SessionEventEnum::SessionStarted) {
        if (!session_event.session_started.has_value()) {
            throw std::runtime_error("SessionEvent SessionStarted does not contain session_started context");
        }
        return session_event.session_started.value().signed_meter_value;
    } else if (event_type == types::evse_manager::SessionEventEnum::TransactionStarted) {
        if (!session_event.transaction_started.has_value()) {
            throw std::runtime_error("SessionEvent TransactionStarted does not contain transaction_started context");
        }
        return session_event.transaction_started.value().signed_meter_value;
    } else if (event_type == types::evse_manager::SessionEventEnum::TransactionFinished) {
        if (!session_event.transaction_finished.has_value()) {
            throw std::runtime_error("SessionEvent TransactionFinished does not contain transaction_finished context");
        }
        return session_event.transaction_finished.value().signed_meter_value;
    }
    return std::nullopt;
}

std::optional<ocpp::v201::IdToken> get_authorized_id_token(const types::evse_manager::SessionEvent& session_event) {
    const auto event_type = session_event.event;
    if (event_type == types::evse_manager::SessionEventEnum::SessionStarted) {
        if (!session_event.session_started.has_value()) {
            throw std::runtime_error("SessionEvent SessionStarted does not contain session_started context");
        }
        const auto session_started = session_event.session_started.value();
        if (session_started.id_tag.has_value()) {
            return conversions::to_ocpp_id_token(session_started.id_tag.value().id_token);
        }
    } else if (event_type == types::evse_manager::SessionEventEnum::TransactionStarted) {
        if (!session_event.transaction_started.has_value()) {
            throw std::runtime_error("SessionEvent TransactionStarted does not contain transaction_started context");
        }
        const auto transaction_started = session_event.transaction_started.value();
        return conversions::to_ocpp_id_token(transaction_started.id_tag.id_token);
    }
    return std::nullopt;
}

bool OCPP201::all_evse_ready() {
    for (auto const& [evse, ready] : this->evse_ready_map) {
        if (!ready) {
            return false;
        }
    }
    EVLOG_info << "All EVSE ready. Starting OCPP2.0.1 service";
    return true;
}

void OCPP201::init() {
    invoke_init(*p_main);
    invoke_init(*p_auth_provider);
    invoke_init(*p_auth_validator);

    this->init_evse_ready_map();

    for (size_t evse_id = 1; evse_id <= this->r_evse_manager.size(); evse_id++) {
        this->r_evse_manager.at(evse_id - 1)->subscribe_ready([this, evse_id](bool ready) {
            std::lock_guard<std::mutex> lk(this->evse_ready_mutex);
            if (ready) {
                EVLOG_info << "EVSE " << evse_id << " ready.";
                this->evse_ready_map[evse_id] = true;
                this->evse_ready_cv.notify_one();
            }
        });
    }
}

void OCPP201::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_auth_provider);
    invoke_ready(*p_auth_validator);

    this->ocpp_share_path = this->info.paths.share;

    const auto device_model_database_path = [&]() {
        const auto config_device_model_path = fs::path(this->config.DeviceModelDatabasePath);
        if (config_device_model_path.is_relative()) {
            return this->ocpp_share_path / config_device_model_path;
        } else {
            return config_device_model_path;
        }
    }();

    if (!fs::exists(this->config.MessageLogPath)) {
        try {
            fs::create_directory(this->config.MessageLogPath);
        } catch (const fs::filesystem_error& e) {
            EVLOG_AND_THROW(e);
        }
    }

    ocpp::v201::Callbacks callbacks;
    callbacks.is_reset_allowed_callback = [this](const std::optional<const int32_t> evse_id,
                                                 const ocpp::v201::ResetEnum& type) {
        if (evse_id.has_value()) {
            return false; // Reset of EVSE is currently not supported
        }
        try {
            return this->r_system->call_is_reset_allowed(types::system::ResetType::NotSpecified);
        } catch (std::out_of_range& e) {
            EVLOG_warning << "Could not convert OCPP ResetEnum to EVerest ResetType while executing "
                             "is_reset_allowed_callback.";
            return false;
        }
    };
    callbacks.reset_callback = [this](const std::optional<const int32_t> evse_id, const ocpp::v201::ResetEnum& type) {
        if (evse_id.has_value()) {
            EVLOG_warning << "Reset of EVSE is currently not supported";
            return;
        }

        bool scheduled = type == ocpp::v201::ResetEnum::OnIdle;

        try {
            this->r_system->call_reset(types::system::ResetType::NotSpecified, scheduled);
        } catch (std::out_of_range& e) {
            EVLOG_warning << "Could not convert OCPP ResetEnum to EVerest ResetType while executing reset_callack. No "
                             "reset will be executed.";
        }
    };

    callbacks.connector_effective_operative_status_changed_callback =
        [this](const int32_t evse_id, const int32_t connector_id, const ocpp::v201::OperationalStatusEnum new_status) {
            if (new_status == ocpp::v201::OperationalStatusEnum::Operative) {
                if (this->r_evse_manager.at(evse_id - 1)->call_enable(connector_id)) {
                    this->charge_point->on_enabled(evse_id, connector_id);
                }
            } else {
                if (this->r_evse_manager.at(evse_id - 1)->call_disable(connector_id)) {
                    this->charge_point->on_unavailable(evse_id, connector_id);
                }
            }
        };

    callbacks.remote_start_transaction_callback = [this](const ocpp::v201::RequestStartTransactionRequest& request,
                                                         const bool authorize_remote_start) {
        types::authorization::ProvidedIdToken provided_token;
        provided_token.id_token = conversions::to_everest_id_token(request.idToken);
        provided_token.authorization_type = types::authorization::AuthorizationType::OCPP;
        provided_token.prevalidated = !authorize_remote_start;
        provided_token.request_id = request.remoteStartId;

        if (request.groupIdToken.has_value()) {
            provided_token.parent_id_token = conversions::to_everest_id_token(request.groupIdToken.value());
        }

        if (request.evseId.has_value()) {
            provided_token.connectors = std::vector<int32_t>{request.evseId.value()};
        }
        this->p_auth_provider->publish_provided_token(provided_token);
    };

    callbacks.stop_transaction_callback = [this](const int32_t evse_id, const ocpp::v201::ReasonEnum& stop_reason) {
        if (evse_id > 0 && evse_id <= this->r_evse_manager.size()) {
            types::evse_manager::StopTransactionRequest req;
            req.reason = conversions::to_everest_stop_transaction_reason(stop_reason);
            this->r_evse_manager.at(evse_id - 1)->call_stop_transaction(req);
        }
    };

    callbacks.pause_charging_callback = [this](const int32_t evse_id) {
        if (evse_id > 0 && evse_id <= this->r_evse_manager.size()) {
            this->r_evse_manager.at(evse_id - 1)->call_pause_charging();
        }
    };

    callbacks.unlock_connector_callback = [this](const int32_t evse_id, const int32_t connector_id) {
        // FIXME: This needs to properly handle different connectors
        ocpp::v201::UnlockConnectorResponse response;
        if (evse_id > 0 && evse_id <= this->r_evse_manager.size()) {
            if (this->r_evse_manager.at(evse_id - 1)->call_force_unlock(connector_id)) {
                response.status = ocpp::v201::UnlockStatusEnum::Unlocked;
            } else {
                response.status = ocpp::v201::UnlockStatusEnum::UnlockFailed;
            }
        } else {
            response.status = ocpp::v201::UnlockStatusEnum::UnknownConnector;
        }

        return response;
    };

    callbacks.get_log_request_callback = [this](const ocpp::v201::GetLogRequest& request) {
        const auto response = this->r_system->call_upload_logs(conversions::to_everest_upload_logs_request(request));
        return conversions::to_ocpp_get_log_response(response);
    };

    callbacks.is_reservation_for_token_callback = [](const int32_t evse_id, const ocpp::CiString<36> idToken,
                                                     const std::optional<ocpp::CiString<36>> groupIdToken) {
        // FIXME: This is just a stub, replace with functionality
        EVLOG_warning << "is_reservation_for_token_callback is still a stub";
        return false;
    };

    callbacks.update_firmware_request_callback = [this](const ocpp::v201::UpdateFirmwareRequest& request) {
        const auto response =
            this->r_system->call_update_firmware(conversions::to_everest_firmware_update_request(request));
        return conversions::to_ocpp_update_firmware_response(response);
    };

    callbacks.variable_changed_callback = [this](const ocpp::v201::SetVariableData& set_variable_data) {
        if (set_variable_data.component.name == "TxCtrlr" and
            set_variable_data.variable.name == "EVConnectionTimeOut") {
            try {
                auto ev_connection_timeout = std::stoi(set_variable_data.attributeValue.get());
                this->r_auth->call_set_connection_timeout(ev_connection_timeout);
            } catch (const std::exception& e) {
                EVLOG_error << "Could not parse EVConnectionTimeOut and did not set it in Auth module, error: "
                            << e.what();
                return;
            }
        } else if (set_variable_data.component.name == "AuthCtrlr" and
                   set_variable_data.variable.name == "MasterPassGroupId") {
            this->r_auth->call_set_master_pass_group_id(set_variable_data.attributeValue.get());
        } else if (set_variable_data.component.name == "TxCtrlr" and
                   set_variable_data.variable.name == "TxStartPoint") {
            const auto tx_start_points = get_tx_start_stop_points(set_variable_data.attributeValue.get());
            if (tx_start_points.empty()) {
                EVLOG_warning << "Could not set TxStartPoints";
                return;
            }
            this->transaction_handler->set_tx_start_points(tx_start_points);
        } else if (set_variable_data.component.name == "TxCtrlr" and set_variable_data.variable.name == "TxStopPoint") {
            const auto tx_stop_points = get_tx_start_stop_points(set_variable_data.attributeValue.get());
            if (tx_stop_points.empty()) {
                EVLOG_warning << "Could not set TxStartPoints";
                return;
            }
            this->transaction_handler->set_tx_stop_points(tx_stop_points);
        }
    };

    callbacks.validate_network_profile_callback =
        [this](const int32_t configuration_slot,
               const ocpp::v201::NetworkConnectionProfile& network_connection_profile) {
            auto ws_uri = ocpp::uri(network_connection_profile.ocppCsmsUrl.get());

            if (ws_uri.get_valid()) {
                return ocpp::v201::SetNetworkProfileStatusEnum::Accepted;
            } else {
                return ocpp::v201::SetNetworkProfileStatusEnum::Rejected;
            }
            // TODO(piet): Add further validation of the NetworkConnectionProfile
        };

    callbacks.configure_network_connection_profile_callback =
        [this](const ocpp::v201::NetworkConnectionProfile& network_connection_profile) { return true; };

    callbacks.all_connectors_unavailable_callback = [this]() {
        EVLOG_info << "All connectors unavailable, proceed with firmware installation";
        this->r_system->call_allow_firmware_installation();
    };

    callbacks.transaction_event_callback = [this](const ocpp::v201::TransactionEventRequest& transaction_event) {
        const auto ocpp_transaction_event = conversions::to_everest_ocpp_transaction_event(transaction_event);
        this->p_ocpp_generic->publish_ocpp_transaction_event(ocpp_transaction_event);
    };

    callbacks.transaction_event_response_callback =
        [this](const ocpp::v201::TransactionEventRequest& transaction_event,
               const ocpp::v201::TransactionEventResponse& transaction_event_response) {
            auto ocpp_transaction_event = conversions::to_everest_ocpp_transaction_event(transaction_event);
            auto ocpp_transaction_event_response =
                conversions::to_everest_transaction_event_response(transaction_event_response);
            ocpp_transaction_event_response.original_transaction_event = ocpp_transaction_event;
            this->p_ocpp_generic->publish_ocpp_transaction_event_response(ocpp_transaction_event_response);
        };

    callbacks.boot_notification_callback =
        [this](const ocpp::v201::BootNotificationResponse& boot_notification_response) {
            const auto everest_boot_notification_response =
                conversions::to_everest_boot_notification_response(boot_notification_response);
            this->p_ocpp_generic->publish_boot_notification_response(everest_boot_notification_response);
        };

    if (!this->r_data_transfer.empty()) {
        callbacks.data_transfer_callback = [this](const ocpp::v201::DataTransferRequest& request) {
            types::ocpp::DataTransferRequest data_transfer_request =
                conversions::to_everest_data_transfer_request(request);
            types::ocpp::DataTransferResponse data_transfer_response =
                this->r_data_transfer.at(0)->call_data_transfer(data_transfer_request);
            ocpp::v201::DataTransferResponse response =
                conversions::to_ocpp_data_transfer_response(data_transfer_response);
            return response;
        };
    }

    const auto sql_init_path = this->ocpp_share_path / SQL_CORE_MIGRTATIONS;

    std::map<int32_t, int32_t> evse_connector_structure = this->get_connector_structure();
    this->charge_point = std::make_unique<ocpp::v201::ChargePoint>(
        evse_connector_structure, device_model_database_path, this->ocpp_share_path.string(),
        this->config.CoreDatabasePath, sql_init_path.string(), this->config.MessageLogPath,
        std::make_shared<EvseSecurity>(*this->r_security), callbacks);

    const auto ev_connection_timeout_request_value_response = this->charge_point->request_value<int32_t>(
        ocpp::v201::Component{"TxCtrlr"}, ocpp::v201::Variable{"EVConnectionTimeOut"},
        ocpp::v201::AttributeEnum::Actual);
    if (ev_connection_timeout_request_value_response.status == ocpp::v201::GetVariableStatusEnum::Accepted and
        ev_connection_timeout_request_value_response.value.has_value()) {
        this->r_auth->call_set_connection_timeout(ev_connection_timeout_request_value_response.value.value());
    }

    const auto master_pass_group_id_response = this->charge_point->request_value<std::string>(
        ocpp::v201::Component{"AuthCtrlr"}, ocpp::v201::Variable{"MasterPassGroupId"},
        ocpp::v201::AttributeEnum::Actual);
    if (master_pass_group_id_response.status == ocpp::v201::GetVariableStatusEnum::Accepted and
        master_pass_group_id_response.value.has_value()) {
        this->r_auth->call_set_master_pass_group_id(master_pass_group_id_response.value.value());
    }

    if (this->config.EnableExternalWebsocketControl) {
        const std::string connect_topic = "everest_api/ocpp/cmd/connect";
        this->mqtt.subscribe(connect_topic,
                             [this](const std::string& data) { this->charge_point->connect_websocket(); });

        const std::string disconnect_topic = "everest_api/ocpp/cmd/disconnect";
        this->mqtt.subscribe(disconnect_topic,
                             [this](const std::string& data) { this->charge_point->disconnect_websocket(); });
    }

    std::set<TxStartStopPoint> tx_start_points;
    std::set<TxStartStopPoint> tx_stop_points;

    const auto tx_start_point_request_value_response = this->charge_point->request_value<std::string>(
        ocpp::v201::Component{"TxCtrlr"}, ocpp::v201::Variable{"TxStartPoint"}, ocpp::v201::AttributeEnum::Actual);
    if (tx_start_point_request_value_response.status == ocpp::v201::GetVariableStatusEnum::Accepted and
        tx_start_point_request_value_response.value.has_value()) {
        auto tx_start_point_csl =
            tx_start_point_request_value_response.value.value(); // contains comma seperated list of TxStartPoints
        tx_start_points = get_tx_start_stop_points(tx_start_point_csl);
        EVLOG_info << "TxStartPoints from device model: " << tx_start_point_csl;
    }

    if (tx_start_points.empty()) {
        tx_start_points = {TxStartStopPoint::PowerPathClosed};
    }

    const auto tx_stop_point_request_value_response = this->charge_point->request_value<std::string>(
        ocpp::v201::Component{"TxCtrlr"}, ocpp::v201::Variable{"TxStopPoint"}, ocpp::v201::AttributeEnum::Actual);
    if (tx_stop_point_request_value_response.status == ocpp::v201::GetVariableStatusEnum::Accepted and
        tx_stop_point_request_value_response.value.has_value()) {
        auto tx_stop_point_csl =
            tx_stop_point_request_value_response.value.value(); // contains comma seperated list of TxStartPoints
        tx_stop_points = get_tx_start_stop_points(tx_stop_point_csl);
        EVLOG_info << "TxStopPoints from device model: " << tx_stop_point_csl;
    }

    if (tx_stop_points.empty()) {
        tx_stop_points = {TxStartStopPoint::EVConnected, TxStartStopPoint::Authorized};
    }

    this->transaction_handler =
        std::make_unique<TransactionHandler>(this->r_evse_manager.size(), tx_start_points, tx_stop_points);

    int evse_id = 1;
    for (const auto& evse : this->r_evse_manager) {
        evse->subscribe_session_event([this, evse_id](types::evse_manager::SessionEvent session_event) {
            this->process_session_event(evse_id, session_event);
        });

        evse->subscribe_powermeter([this, evse_id](const types::powermeter::Powermeter& power_meter) {
            const auto meter_value = conversions::to_ocpp_meter_value(
                power_meter, ocpp::v201::ReadingContextEnum::Sample_Periodic, power_meter.signed_meter_value);
            this->charge_point->on_meter_value(evse_id, meter_value);
        });

        evse->subscribe_iso15118_certificate_request(
            [this, evse_id](const types::iso15118_charger::Request_Exi_Stream_Schema& certificate_request) {
                auto ocpp_response = this->charge_point->on_get_15118_ev_certificate_request(
                    conversions::to_ocpp_get_15118_certificate_request(certificate_request));
                EVLOG_debug << "Received response from get_15118_ev_certificate_request: " << ocpp_response;
                // transform response, inject action, send to associated EvseManager
                const auto everest_response_status =
                    conversions::to_everest_iso15118_charger_status(ocpp_response.status);
                const types::iso15118_charger::Response_Exi_Stream_Status everest_response{
                    everest_response_status, certificate_request.certificateAction, ocpp_response.exiResponse};
                this->r_evse_manager.at(evse_id - 1)->call_set_get_certificate_response(everest_response);
            });

        evse_id++;
    }
    r_system->subscribe_firmware_update_status([this](const types::system::FirmwareUpdateStatus status) {
        this->charge_point->on_firmware_update_status_notification(
            status.request_id, conversions::to_ocpp_firmware_status_enum(status.firmware_update_status));
    });

    r_system->subscribe_log_status([this](types::system::LogStatus status) {
        this->charge_point->on_log_status_notification(conversions::to_ocpp_upload_logs_status_enum(status.log_status),
                                                       status.request_id);
    });

    std::unique_lock lk(this->evse_ready_mutex);
    while (!this->all_evse_ready()) {
        this->evse_ready_cv.wait(lk);
    }
    // In case (for some reason) EvseManager ready signals are sent after this point, this will prevent a hang
    lk.unlock();

    const auto boot_reason = conversions::to_ocpp_boot_reason(this->r_system->call_get_boot_reason());
    this->charge_point->set_message_queue_resume_delay(std::chrono::seconds(this->config.MessageQueueResumeDelay));
    this->charge_point->start(boot_reason);
}

void OCPP201::process_session_event(const int32_t evse_id, const types::evse_manager::SessionEvent& session_event) {
    const auto connector_id = session_event.connector_id.value_or(1);
    switch (session_event.event) {
    case types::evse_manager::SessionEventEnum::SessionStarted: {
        this->process_session_started(evse_id, connector_id, session_event);
        break;
    }
    case types::evse_manager::SessionEventEnum::SessionFinished: {
        this->process_session_finished(evse_id, connector_id, session_event);
        break;
    }
    case types::evse_manager::SessionEventEnum::TransactionStarted: {
        this->process_transaction_started(evse_id, connector_id, session_event);
        break;
    }
    case types::evse_manager::SessionEventEnum::TransactionFinished: {
        this->process_transaction_finished(evse_id, connector_id, session_event);
        break;
    }
    case types::evse_manager::SessionEventEnum::ChargingStarted: {
        this->process_charging_started(evse_id, connector_id, session_event);
        break;
    }
    case types::evse_manager::SessionEventEnum::ChargingResumed: {
        this->process_charging_resumed(evse_id, connector_id, session_event);
        break;
    }
    case types::evse_manager::SessionEventEnum::ChargingPausedEV: {
        this->process_charging_paused_ev(evse_id, connector_id, session_event);
        break;
    }
    case types::evse_manager::SessionEventEnum::ChargingPausedEVSE: {
        this->process_charging_paused_evse(evse_id, connector_id, session_event);
        break;
    }
    case types::evse_manager::SessionEventEnum::Disabled: {
        this->process_disabled(evse_id, connector_id, session_event);
        break;
    }
    case types::evse_manager::SessionEventEnum::Enabled: {
        this->process_enabled(evse_id, connector_id, session_event);
        break;
    }
    case types::evse_manager::SessionEventEnum::Authorized: {
        this->process_authorized(evse_id, connector_id, session_event);
        break;
    }
    case types::evse_manager::SessionEventEnum::Deauthorized: {
        this->process_deauthorized(evse_id, connector_id, session_event);
        break;
    }
    }

    // process authorized event which will inititate a TransactionEvent(Updated) message in case the token has not yet
    // been authorized by the CSMS
    const auto authorized_id_token = get_authorized_id_token(session_event);
    if (authorized_id_token.has_value()) {
        this->charge_point->on_authorized(evse_id, connector_id, authorized_id_token.value());
    }
}

void OCPP201::process_tx_event_effect(const int32_t evse_id, const TxEventEffect tx_event_effect,
                                      const types::evse_manager::SessionEvent& session_event) {
    if (tx_event_effect == TxEventEffect::NONE) {
        return;
    }

    const auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    if (transaction_data == nullptr) {
        throw std::runtime_error("Could not start transaction because no tranasaction data is present");
    }
    transaction_data->timestamp = ocpp::DateTime(session_event.timestamp);

    if (tx_event_effect == TxEventEffect::START_TRANSACTION) {
        transaction_data->started = true;
        transaction_data->meter_value = conversions::to_ocpp_meter_value(
            get_meter_value(session_event), ocpp::v201::ReadingContextEnum::Transaction_Begin,
            get_signed_meter_value(session_event));
        this->charge_point->on_transaction_started(
            evse_id, transaction_data->connector_id, transaction_data->session_id, transaction_data->timestamp,
            transaction_data->trigger_reason, transaction_data->meter_value, transaction_data->id_token,
            transaction_data->group_id_token, transaction_data->reservation_id, transaction_data->remote_start_id,
            transaction_data->charging_state);
    } else if (tx_event_effect == TxEventEffect::STOP_TRANSACTION) {
        transaction_data->meter_value = conversions::to_ocpp_meter_value(
            get_meter_value(session_event), ocpp::v201::ReadingContextEnum::Transaction_End,
            get_signed_meter_value(session_event));
        this->charge_point->on_transaction_finished(evse_id, transaction_data->timestamp, transaction_data->meter_value,
                                                    transaction_data->stop_reason, transaction_data->id_token,
                                                    std::nullopt, transaction_data->charging_state);
        this->transaction_handler->reset_transaction_data(evse_id);
    }
}

void OCPP201::process_session_started(const int32_t evse_id, const int32_t connector_id,
                                      const types::evse_manager::SessionEvent& session_event) {
    if (!session_event.session_started.has_value()) {
        throw std::runtime_error("SessionEvent SessionStarted does not contain session_started context");
    }

    const auto session_started = session_event.session_started.value();

    std::optional<ocpp::v201::IdToken> id_token = std::nullopt;
    std::optional<ocpp::v201::IdToken> group_id_token = std::nullopt;
    std::optional<int32_t> remote_start_id = std::nullopt;
    auto charging_state = ocpp::v201::ChargingStateEnum::Idle;
    auto trigger_reason = ocpp::v201::TriggerReasonEnum::Authorized;
    auto tx_event = TxEvent::AUTHORIZED;
    if (session_started.reason == types::evse_manager::StartSessionReason::EVConnected) {
        tx_event = TxEvent::EV_CONNECTED;
        trigger_reason = ocpp::v201::TriggerReasonEnum::CablePluggedIn;
        charging_state = ocpp::v201::ChargingStateEnum::EVConnected;
    } else if (!session_started.id_tag.has_value()) {
        EVLOG_warning << "Session started with reason Authorized, but no id_tag provided as part of the "
                         "session event";
    } else {
        id_token = conversions::to_ocpp_id_token(session_started.id_tag.value().id_token);
        remote_start_id = session_started.id_tag.value().request_id;
        if (session_started.id_tag.value().parent_id_token.has_value()) {
            group_id_token = conversions::to_ocpp_id_token(session_started.id_tag.value().parent_id_token.value());
        }
        if (session_started.id_tag.value().authorization_type == types::authorization::AuthorizationType::OCPP) {
            trigger_reason = ocpp::v201::TriggerReasonEnum::RemoteStart;
        }
    }
    const auto timestamp = ocpp::DateTime(session_event.timestamp);
    const auto reservation_id = session_started.reservation_id;

    // this is always the first transaction related interaction, so we create TransactionData here
    auto transaction_data =
        std::make_shared<TransactionData>(connector_id, session_event.uuid, timestamp, trigger_reason, charging_state);
    transaction_data->id_token = id_token;
    transaction_data->group_id_token = group_id_token;
    transaction_data->remote_start_id = remote_start_id;
    transaction_data->reservation_id = reservation_id;
    this->transaction_handler->add_transaction_data(evse_id, transaction_data);

    const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, tx_event);
    this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
    if (session_started.reason == types::evse_manager::StartSessionReason::EVConnected) {
        this->charge_point->on_session_started(evse_id, connector_id);
    }
}

void OCPP201::process_session_finished(const int32_t evse_id, const int32_t connector_id,
                                       const types::evse_manager::SessionEvent& session_event) {
    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    if (transaction_data != nullptr) {
        transaction_data->charging_state = ocpp::v201::ChargingStateEnum::Idle;
    }
    const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, TxEvent::EV_DISCONNECTED);
    this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
    this->charge_point->on_session_finished(evse_id, connector_id);
}

void OCPP201::process_transaction_started(const int32_t evse_id, const int32_t connector_id,
                                          const types::evse_manager::SessionEvent& session_event) {
    if (!session_event.transaction_started.has_value()) {
        throw std::runtime_error("SessionEvent TransactionStarted does not contain session_started context");
    }

    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    if (transaction_data == nullptr) {
        EVLOG_warning << "Could not update transaction data because no tranasaction data is present. This might happen "
                         "in case a TxStopPoint is already active when a TransactionStarted event occurs (e.g. "
                         "TxStopPoint is EnergyTransfer or ParkingBayOccupied)";
        this->charge_point->on_session_started(evse_id, connector_id);
        auto tx_event_effect = this->transaction_handler->submit_event(evse_id, TxEvent::AUTHORIZED);
        this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
        tx_event_effect = this->transaction_handler->submit_event(evse_id, TxEvent::EV_CONNECTED);
        this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
        return;
    }

    // at this point we dont know if the TransactionStarted event was triggered because of an Authorization or EV Plug
    // in event. We assume cable has been plugged in first and then authorized and update if other order was applied
    auto tx_event = TxEvent::AUTHORIZED;
    auto trigger_reason = ocpp::v201::TriggerReasonEnum::Authorized;
    const auto transaction_started = session_event.transaction_started.value();
    transaction_data->reservation_id = transaction_started.reservation_id;
    transaction_data->remote_start_id = transaction_started.id_tag.request_id;
    const auto id_token = conversions::to_ocpp_id_token(transaction_started.id_tag.id_token);
    transaction_data->id_token = id_token;

    std::optional<ocpp::v201::IdToken> group_id_token = std::nullopt;
    if (transaction_started.id_tag.parent_id_token.has_value()) {
        transaction_data->group_id_token =
            conversions::to_ocpp_id_token(transaction_started.id_tag.parent_id_token.value());
    }

    // if session started reason was Authorized, Transaction is started because of EV plug in event
    if (transaction_data->trigger_reason == ocpp::v201::TriggerReasonEnum::Authorized or
        transaction_data->trigger_reason == ocpp::v201::TriggerReasonEnum::RemoteStart) {
        trigger_reason = ocpp::v201::TriggerReasonEnum::CablePluggedIn;
        transaction_data->charging_state = ocpp::v201::ChargingStateEnum::EVConnected;
        this->charge_point->on_session_started(evse_id, connector_id);
        tx_event = TxEvent::EV_CONNECTED;
    }

    if (transaction_started.id_tag.authorization_type == types::authorization::AuthorizationType::OCPP) {
        trigger_reason = ocpp::v201::TriggerReasonEnum::RemoteStart;
    }

    transaction_data->trigger_reason = trigger_reason;
    const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, tx_event);
    this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
}

void OCPP201::process_transaction_finished(const int32_t evse_id, const int32_t connector_id,
                                           const types::evse_manager::SessionEvent& session_event) {
    if (!session_event.transaction_finished.has_value()) {
        throw std::runtime_error("SessionEvent TransactionFinished does not contain session_started context");
    }
    const auto transaction_finished = session_event.transaction_finished.value();
    auto tx_event = TxEvent::NONE;
    auto reason = ocpp::v201::ReasonEnum::Other;
    if (transaction_finished.reason.has_value()) {
        reason = conversions::to_ocpp_reason(transaction_finished.reason.value());
        tx_event = get_tx_event(reason);
    }
    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    if (transaction_data != nullptr) {
        std::optional<ocpp::v201::IdToken> id_token = std::nullopt;
        if (transaction_finished.id_tag.has_value()) {
            id_token = conversions::to_ocpp_id_token(transaction_finished.id_tag.value().id_token);
        }

        // this is required to report the correct charging_state within a TransactionEvent(Ended) message
        auto charging_state = transaction_data->charging_state;
        if (reason == ocpp::v201::ReasonEnum::EVDisconnected) {
            charging_state = ocpp::v201::ChargingStateEnum::Idle;
        } else if (tx_event == TxEvent::DEAUTHORIZED) {
            charging_state = ocpp::v201::ChargingStateEnum::EVConnected;
        }
        transaction_data->stop_reason = reason;
        transaction_data->id_token = id_token;
        transaction_data->charging_state = charging_state;
    }
    const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, tx_event);
    this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
}

void OCPP201::process_charging_started(const int32_t evse_id, const int32_t connector_id,
                                       const types::evse_manager::SessionEvent& session_event) {
    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    if (transaction_data != nullptr) {
        transaction_data->trigger_reason = ocpp::v201::TriggerReasonEnum::ChargingStateChanged;
        transaction_data->charging_state = ocpp::v201::ChargingStateEnum::Charging;
    }
    const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, TxEvent::ENERGY_TRANSFER_STARTED);
    this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
    this->charge_point->on_charging_state_changed(evse_id, ocpp::v201::ChargingStateEnum::Charging);
}

void OCPP201::process_charging_resumed(const int32_t evse_id, const int32_t connector_id,
                                       const types::evse_manager::SessionEvent& session_event) {
    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    if (transaction_data != nullptr) {
        transaction_data->charging_state = ocpp::v201::ChargingStateEnum::Charging;
    }
    this->charge_point->on_charging_state_changed(evse_id, ocpp::v201::ChargingStateEnum::Charging);
}

void OCPP201::process_charging_paused_ev(const int32_t evse_id, const int32_t connector_id,
                                         const types::evse_manager::SessionEvent& session_event) {
    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    if (transaction_data != nullptr) {
        transaction_data->charging_state = ocpp::v201::ChargingStateEnum::SuspendedEV;
        transaction_data->trigger_reason = ocpp::v201::TriggerReasonEnum::ChargingStateChanged;
        transaction_data->stop_reason = ocpp::v201::ReasonEnum::StoppedByEV;
    }
    const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, TxEvent::ENERGY_TRANSFER_STOPPED);
    this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
    this->charge_point->on_charging_state_changed(evse_id, ocpp::v201::ChargingStateEnum::SuspendedEV);
}

void OCPP201::process_charging_paused_evse(const int32_t evse_id, const int32_t connector_id,
                                           const types::evse_manager::SessionEvent& session_event) {
    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    auto trigger_reason = ocpp::v201::TriggerReasonEnum::ChargingStateChanged;
    if (transaction_data != nullptr) {
        transaction_data->charging_state = ocpp::v201::ChargingStateEnum::SuspendedEVSE;
        if (transaction_data->stop_reason == ocpp::v201::ReasonEnum::Remote) {
            trigger_reason = ocpp::v201::TriggerReasonEnum::RemoteStop;
            transaction_data->trigger_reason = ocpp::v201::TriggerReasonEnum::ChargingStateChanged;
        }
    }
    const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, TxEvent::ENERGY_TRANSFER_STOPPED);
    this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
    this->charge_point->on_charging_state_changed(evse_id, ocpp::v201::ChargingStateEnum::SuspendedEVSE,
                                                  trigger_reason);
}

void OCPP201::process_enabled(const int32_t evse_id, const int32_t connector_id,
                              const types::evse_manager::SessionEvent& session_event) {
    this->charge_point->on_enabled(evse_id, connector_id);
}

void OCPP201::process_disabled(const int32_t evse_id, const int32_t connector_id,
                               const types::evse_manager::SessionEvent& session_event) {
    this->charge_point->on_unavailable(evse_id, connector_id);
}

void OCPP201::process_authorized(const int32_t evse_id, const int32_t connector_id,
                                 const types::evse_manager::SessionEvent& session_event) {
    // currently handled as part of SessionStarted and TransactionStarted events
}

void OCPP201::process_deauthorized(const int32_t evse_id, const int32_t connector_id,
                                   const types::evse_manager::SessionEvent& session_event) {
    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    if (transaction_data != nullptr) {
        transaction_data->trigger_reason = ocpp::v201::TriggerReasonEnum::StopAuthorized;
    }
    const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, TxEvent::DEAUTHORIZED);
    this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
}

} // namespace module
