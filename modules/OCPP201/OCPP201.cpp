// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "OCPP201.hpp"

#include <fmt/core.h>
#include <fstream>
#include <websocketpp/uri.hpp>

#include <conversions.hpp>
#include <evse_security_ocpp.hpp>
namespace module {

const std::string INIT_SQL = "init_core.sql";
const std::string CERTS_DIR = "certs";

namespace fs = std::filesystem;

TxStartPoint get_tx_start_point(const std::string& tx_start_point_string) {
    if (tx_start_point_string == "ParkingBayOccupancy") {
        return TxStartPoint::ParkingBayOccupancy;
    } else if (tx_start_point_string == "EVConnected") {
        return TxStartPoint::EVConnected;
    } else if (tx_start_point_string == "Authorized") {
        return TxStartPoint::Authorized;
    } else if (tx_start_point_string == "PowerPathClosed") {
        return TxStartPoint::PowerPathClosed;
    } else if (tx_start_point_string == "EnergyTransfer") {
        return TxStartPoint::EnergyTransfer;
    } else if (tx_start_point_string == "DataSigned") {
        return TxStartPoint::DataSigned;
    }

    // default to PowerPathClosed for now
    return TxStartPoint::PowerPathClosed;
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

    const auto etc_certs_path = [&]() {
        if (this->config.CertsPath.empty()) {
            return fs::path(this->info.paths.etc) / CERTS_DIR;
        } else {
            return fs::path(this->config.CertsPath);
        }
    }();
    EVLOG_info << "OCPP certificates path: " << etc_certs_path.string();

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
            EVLOG_warning
                << "Could not convert OCPP ResetEnum to EVerest ResetType while executing is_reset_allowed_callback.";
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
                if (this->r_evse_manager.at(evse_id - 1)
                        ->call_enable_disable(connector_id, {types::evse_manager::Enable_source::CSMS,
                                                             types::evse_manager::Enable_state::Enable, 5000})) {
                    this->charge_point->on_enabled(evse_id, connector_id);
                }
            } else {
                if (this->r_evse_manager.at(evse_id - 1)
                        ->call_enable_disable(connector_id, {types::evse_manager::Enable_source::CSMS,
                                                             types::evse_manager::Enable_state::Disable, 5000})) {
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
        }
    };

    callbacks.validate_network_profile_callback =
        [this](const int32_t configuration_slot,
               const ocpp::v201::NetworkConnectionProfile& network_connection_profile) {
            auto ws_uri = websocketpp::uri(network_connection_profile.ocppCsmsUrl.get());

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

    if (!this->r_data_transfer.empty()) {
        callbacks.data_transfer_callback = [this](const ocpp::v201::DataTransferRequest& request) {
            types::ocpp::DataTransferRequest data_transfer_request;
            data_transfer_request.vendor_id = request.vendorId.get();
            if (request.messageId.has_value()) {
                data_transfer_request.message_id = request.messageId.value().get();
            }
            data_transfer_request.data = request.data;
            types::ocpp::DataTransferResponse data_transfer_response =
                this->r_data_transfer.at(0)->call_data_transfer(data_transfer_request);
            ocpp::v201::DataTransferResponse response;
            response.status = conversions::to_ocpp_data_transfer_status_enum(data_transfer_response.status);
            response.data = data_transfer_response.data;
            return response;
        };
    }

    const auto sql_init_path = this->ocpp_share_path / INIT_SQL;

    std::map<int32_t, int32_t> evse_connector_structure = this->get_connector_structure();
    this->charge_point = std::make_unique<ocpp::v201::ChargePoint>(
        evse_connector_structure, device_model_database_path, this->ocpp_share_path.string(),
        this->config.CoreDatabasePath, sql_init_path.string(), this->config.MessageLogPath,
        std::make_shared<EvseSecurity>(*this->r_security), callbacks);

    const auto tx_start_point_request_value_response = this->charge_point->request_value<std::string>(
        ocpp::v201::Component{"TxCtrlr"}, ocpp::v201::Variable{"TxStartPoint"}, ocpp::v201::AttributeEnum::Actual);
    if (tx_start_point_request_value_response.status == ocpp::v201::GetVariableStatusEnum::Accepted and
        tx_start_point_request_value_response.value.has_value()) {
        auto tx_start_point_string = tx_start_point_request_value_response.value.value();
        this->tx_start_point = get_tx_start_point(tx_start_point_string);
        EVLOG_info << "TxStartPoint from device model: " << tx_start_point_string;
    } else {
        this->tx_start_point = TxStartPoint::PowerPathClosed;
    }

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

    int evse_id = 1;
    for (const auto& evse : this->r_evse_manager) {
        evse->subscribe_session_event([this, evse_id](types::evse_manager::SessionEvent session_event) {
            const auto connector_id = session_event.connector_id.value_or(1);
            const auto evse_connector = std::make_pair(evse_id, connector_id);
            switch (session_event.event) {
            case types::evse_manager::SessionEventEnum::SessionStarted: {
                if (!session_event.session_started.has_value()) {
                    this->session_started_reasons[evse_connector] =
                        types::evse_manager::StartSessionReason::EVConnected;
                } else {
                    this->session_started_reasons[evse_connector] = session_event.session_started.value().reason;
                }

                switch (this->tx_start_point) {
                case TxStartPoint::EVConnected:
                    [[fallthrough]];
                case TxStartPoint::Authorized:
                    [[fallthrough]];
                case TxStartPoint::PowerPathClosed:
                    [[fallthrough]];
                case TxStartPoint::EnergyTransfer:
                    this->charge_point->on_session_started(evse_id, connector_id);
                    break;
                }
                break;
            }
            case types::evse_manager::SessionEventEnum::SessionFinished: {
                this->charge_point->on_session_finished(evse_id, connector_id);
                break;
            }
            case types::evse_manager::SessionEventEnum::TransactionStarted: {
                const auto transaction_started = session_event.transaction_started.value();
                const auto timestamp = ocpp::DateTime(transaction_started.timestamp);
                const auto meter_value = conversions::to_ocpp_meter_value(
                    transaction_started.meter_value, ocpp::v201::ReadingContextEnum::Transaction_Begin,
                    transaction_started.signed_meter_value);
                const auto session_id = session_event.uuid;
                const auto reservation_id = transaction_started.reservation_id;
                const auto remote_start_id = transaction_started.id_tag.request_id;
                const auto id_token = conversions::to_ocpp_id_token(transaction_started.id_tag.id_token);

                std::optional<ocpp::v201::IdToken> group_id_token = std::nullopt;
                if (transaction_started.id_tag.parent_id_token.has_value()) {
                    group_id_token = conversions::to_ocpp_id_token(transaction_started.id_tag.parent_id_token.value());
                }

                // assume cable has been plugged in first and then authorized
                auto trigger_reason = ocpp::v201::TriggerReasonEnum::Authorized;

                // if session started reason was Authorized, Transaction is started because of EV plug in event
                if (this->session_started_reasons[evse_connector] ==
                    types::evse_manager::StartSessionReason::Authorized) {
                    trigger_reason = ocpp::v201::TriggerReasonEnum::CablePluggedIn;
                }

                if (transaction_started.id_tag.authorization_type == types::authorization::AuthorizationType::OCPP) {
                    trigger_reason = ocpp::v201::TriggerReasonEnum::RemoteStart;
                }

                if (this->tx_start_point == TxStartPoint::EnergyTransfer) {
                    this->transaction_starts[evse_connector].emplace(TransactionStart{
                        evse_id, connector_id, session_id, timestamp, trigger_reason, meter_value, id_token,
                        group_id_token, reservation_id, remote_start_id, ocpp::v201::ChargingStateEnum::Charging});
                } else {
                    this->charge_point->on_transaction_started(
                        evse_id, connector_id, session_id, timestamp, trigger_reason, meter_value, id_token,
                        group_id_token, reservation_id, remote_start_id,
                        ocpp::v201::ChargingStateEnum::EVConnected); // FIXME(piet): add proper groupIdToken +
                                                                     // ChargingStateEnum
                }

                break;
            }
            case types::evse_manager::SessionEventEnum::TransactionFinished: {
                const auto transaction_finished = session_event.transaction_finished.value();
                const auto timestamp = ocpp::DateTime(transaction_finished.timestamp);
                const auto signed_meter_value = transaction_finished.signed_meter_value;
                const auto meter_value = conversions::to_ocpp_meter_value(
                    transaction_finished.meter_value, ocpp::v201::ReadingContextEnum::Transaction_End,
                    signed_meter_value);
                ocpp::v201::ReasonEnum reason = ocpp::v201::ReasonEnum::Other;
                if (transaction_finished.reason.has_value()) {
                    reason = conversions::to_ocpp_reason(transaction_finished.reason.value());
                }

                std::optional<ocpp::v201::IdToken> id_token = std::nullopt;
                if (transaction_finished.id_tag.has_value()) {
                    id_token = conversions::to_ocpp_id_token(transaction_finished.id_tag.value().id_token);
                }

                this->charge_point->on_transaction_finished(evse_id, timestamp, meter_value, reason, id_token, "",
                                                            ocpp::v201::ChargingStateEnum::EVConnected);
                break;
            }
            case types::evse_manager::SessionEventEnum::ChargingStarted: {
                if (this->tx_start_point == TxStartPoint::EnergyTransfer) {
                    if (this->transaction_starts[evse_connector].has_value()) {
                        auto transaction_start = this->transaction_starts[evse_connector].value();
                        this->charge_point->on_transaction_started(
                            transaction_start.evse_id, transaction_start.connector_id, transaction_start.session_id,
                            transaction_start.timestamp, transaction_start.trigger_reason,
                            transaction_start.meter_start, transaction_start.id_token, transaction_start.group_id_token,
                            transaction_start.reservation_id, transaction_start.remote_start_id,
                            transaction_start.charging_state);
                        this->transaction_starts[evse_connector].reset();
                    } else {
                        EVLOG_error
                            << "ChargingStarted with TxStartPoint EnergyTransfer but no TransactionStart was available";
                    }
                }
                this->charge_point->on_charging_state_changed(evse_id, ocpp::v201::ChargingStateEnum::Charging);
                break;
            }
            case types::evse_manager::SessionEventEnum::ChargingResumed: {
                this->charge_point->on_charging_state_changed(evse_id, ocpp::v201::ChargingStateEnum::Charging);
                break;
            }
            case types::evse_manager::SessionEventEnum::ChargingPausedEV: {
                this->charge_point->on_charging_state_changed(evse_id, ocpp::v201::ChargingStateEnum::SuspendedEV);
                break;
            }
            case types::evse_manager::SessionEventEnum::ChargingPausedEVSE: {
                this->charge_point->on_charging_state_changed(evse_id, ocpp::v201::ChargingStateEnum::SuspendedEVSE);
                break;
            }
            case types::evse_manager::SessionEventEnum::Disabled: {
                this->charge_point->on_unavailable(evse_id, connector_id);
                break;
            }
            case types::evse_manager::SessionEventEnum::Enabled: {
                // A single connector was enabled
                this->charge_point->on_enabled(evse_id, connector_id);
                break;
            }
            }
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

} // namespace module
