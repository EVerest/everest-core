// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "OCPP201.hpp"

#include <fmt/core.h>
#include <fstream>

#include <websocketpp_utils/uri.hpp>

#include <conversions.hpp>
#include <device_model/composed_device_model_storage.hpp>
#include <error_handling.hpp>
#include <evse_security_ocpp.hpp>
#include <external_energy_limits.hpp>
#include <ocpp_conversions.hpp>

namespace module {

const std::string SQL_CORE_MIGRATIONS = "core_migrations";
const std::string CERTS_DIR = "certs";

namespace fs = std::filesystem;

TxEvent get_tx_event(const ocpp::v2::ReasonEnum reason) {
    switch (reason) {
    case ocpp::v2::ReasonEnum::DeAuthorized:
    case ocpp::v2::ReasonEnum::Remote:
    case ocpp::v2::ReasonEnum::Local:
    case ocpp::v2::ReasonEnum::MasterPass:
    case ocpp::v2::ReasonEnum::StoppedByEV:
    case ocpp::v2::ReasonEnum::ReqEnergyTransferRejected:
        return TxEvent::DEAUTHORIZED;
    case ocpp::v2::ReasonEnum::EVDisconnected:
        return TxEvent::EV_DISCONNECTED;
    case ocpp::v2::ReasonEnum::ImmediateReset:
        return TxEvent::IMMEDIATE_RESET;
    // FIXME(kai): these reasons definitely do not all map to NONE
    case ocpp::v2::ReasonEnum::EmergencyStop:
    case ocpp::v2::ReasonEnum::EnergyLimitReached:
    case ocpp::v2::ReasonEnum::GroundFault:
    case ocpp::v2::ReasonEnum::LocalOutOfCredit:
    case ocpp::v2::ReasonEnum::Other:
    case ocpp::v2::ReasonEnum::OvercurrentFault:
    case ocpp::v2::ReasonEnum::PowerLoss:
    case ocpp::v2::ReasonEnum::PowerQuality:
    case ocpp::v2::ReasonEnum::Reboot:
    case ocpp::v2::ReasonEnum::SOCLimitReached:
    case ocpp::v2::ReasonEnum::TimeLimitReached:
    case ocpp::v2::ReasonEnum::Timeout:
        return TxEvent::NONE;
    }
    return TxEvent::NONE;
}

std::set<TxStartStopPoint> get_tx_start_stop_points(const std::string& tx_start_stop_point_csl) {
    std::set<TxStartStopPoint> tx_start_stop_points;
    std::vector<std::string> csv;
    std::string str;
    std::stringstream ss(tx_start_stop_point_csl);
    while (std::getline(ss, str, ',')) {
        csv.push_back(str);
    }

    for (const auto& tx_start_stop_point : csv) {
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

ocpp::v2::TriggerReasonEnum stop_reason_to_trigger_reason_enum(const ocpp::v2::ReasonEnum& stop_reason) {
    switch (stop_reason) {
    case ocpp::v2::ReasonEnum::DeAuthorized:
        return ocpp::v2::TriggerReasonEnum::Deauthorized;
    case ocpp::v2::ReasonEnum::EmergencyStop:
        return ocpp::v2::TriggerReasonEnum::AbnormalCondition;
    case ocpp::v2::ReasonEnum::EnergyLimitReached:
        return ocpp::v2::TriggerReasonEnum::EnergyLimitReached;
    case ocpp::v2::ReasonEnum::EVDisconnected:
        return ocpp::v2::TriggerReasonEnum::EVCommunicationLost;
    case ocpp::v2::ReasonEnum::GroundFault:
        return ocpp::v2::TriggerReasonEnum::AbnormalCondition;
    case ocpp::v2::ReasonEnum::ImmediateReset:
        return ocpp::v2::TriggerReasonEnum::ResetCommand;
    case ocpp::v2::ReasonEnum::Local:
        return ocpp::v2::TriggerReasonEnum::StopAuthorized;
    case ocpp::v2::ReasonEnum::LocalOutOfCredit:
        return ocpp::v2::TriggerReasonEnum::ChargingStateChanged;
    case ocpp::v2::ReasonEnum::MasterPass:
        return ocpp::v2::TriggerReasonEnum::StopAuthorized;
    case ocpp::v2::ReasonEnum::Other:
        return ocpp::v2::TriggerReasonEnum::AbnormalCondition;
    case ocpp::v2::ReasonEnum::OvercurrentFault:
        return ocpp::v2::TriggerReasonEnum::AbnormalCondition;
    case ocpp::v2::ReasonEnum::PowerLoss:
        return ocpp::v2::TriggerReasonEnum::AbnormalCondition;
    case ocpp::v2::ReasonEnum::PowerQuality:
        return ocpp::v2::TriggerReasonEnum::AbnormalCondition;
    case ocpp::v2::ReasonEnum::Reboot:
        return ocpp::v2::TriggerReasonEnum::ResetCommand;
    case ocpp::v2::ReasonEnum::Remote:
        return ocpp::v2::TriggerReasonEnum::RemoteStop;
    case ocpp::v2::ReasonEnum::SOCLimitReached:
        return ocpp::v2::TriggerReasonEnum::EnergyLimitReached;
    case ocpp::v2::ReasonEnum::StoppedByEV:
        return ocpp::v2::TriggerReasonEnum::StopAuthorized;
    case ocpp::v2::ReasonEnum::TimeLimitReached:
        return ocpp::v2::TriggerReasonEnum::TimeLimitReached;
    case ocpp::v2::ReasonEnum::Timeout:
        return ocpp::v2::TriggerReasonEnum::EVConnectTimeout;
    case ocpp::v2::ReasonEnum::ReqEnergyTransferRejected:
        return ocpp::v2::TriggerReasonEnum::AbnormalCondition;
    default:
        return ocpp::v2::TriggerReasonEnum::AbnormalCondition;
    }
}

int32_t get_connector_id_from_error(const Everest::error::Error& error) {
    if (error.origin.mapping.has_value() and error.origin.mapping.value().connector.has_value()) {
        return error.origin.mapping.value().connector.value();
    }
    return 1;
}

void OCPP201::init_evse_maps() {
    std::lock_guard<std::mutex> lk(this->evse_ready_mutex);
    for (size_t evse_id = 1; evse_id <= this->r_evse_manager.size(); evse_id++) {
        this->evse_ready_map[evse_id] = false;
        this->evse_soc_map[evse_id] = std::nullopt;
    }
}

void OCPP201::init_module_configuration() {
    const auto ev_connection_timeout_request_value_response = this->charge_point->request_value<int32_t>(
        ocpp::v2::Component{"TxCtrlr"}, ocpp::v2::Variable{"EVConnectionTimeOut"}, ocpp::v2::AttributeEnum::Actual);
    if (ev_connection_timeout_request_value_response.status == ocpp::v2::GetVariableStatusEnum::Accepted and
        ev_connection_timeout_request_value_response.value.has_value()) {
        this->r_auth->call_set_connection_timeout(ev_connection_timeout_request_value_response.value.value());
    }

    const auto master_pass_group_id_response = this->charge_point->request_value<std::string>(
        ocpp::v2::Component{"AuthCtrlr"}, ocpp::v2::Variable{"MasterPassGroupId"}, ocpp::v2::AttributeEnum::Actual);
    if (master_pass_group_id_response.status == ocpp::v2::GetVariableStatusEnum::Accepted and
        master_pass_group_id_response.value.has_value()) {
        this->r_auth->call_set_master_pass_group_id(master_pass_group_id_response.value.value());
    }

    types::evse_manager::PlugAndChargeConfiguration pnc_config;
    const auto iso15118_pnc_enabled_response =
        this->charge_point->request_value<bool>(ocpp::v2::Component{"ISO15118Ctrlr"}, ocpp::v2::Variable{"PncEnabled"}, ocpp::v2::AttributeEnum::Actual);
    if (iso15118_pnc_enabled_response.status == ocpp::v2::GetVariableStatusEnum::Accepted and
        iso15118_pnc_enabled_response.value.has_value()) {
        pnc_config.pnc_enabled = iso15118_pnc_enabled_response.value.value();
    }

    const auto central_contract_validation_allowed_response = this->charge_point->request_value<bool>(
        ocpp::v2::Component{"ISO15118Ctrlr"}, ocpp::v2::Variable{"CentralContractValidationAllowed"}, ocpp::v2::AttributeEnum::Actual);
    if (central_contract_validation_allowed_response.status == ocpp::v2::GetVariableStatusEnum::Accepted and
        central_contract_validation_allowed_response.value.has_value()) {
        pnc_config.central_contract_validation_allowed = central_contract_validation_allowed_response.value.value();
    }

    const auto contract_certificate_installation_enabled_response = this->charge_point->request_value<bool>(
        ocpp::v2::Component{"ISO15118Ctrlr"}, ocpp::v2::Variable{"ContractCertificateInstallationEnabled"}, ocpp::v2::AttributeEnum::Actual);
    if (contract_certificate_installation_enabled_response.status == ocpp::v2::GetVariableStatusEnum::Accepted and
        contract_certificate_installation_enabled_response.value.has_value()) {
        pnc_config.central_contract_validation_allowed =
            contract_certificate_installation_enabled_response.value.value();
    }

    for (const auto& evse_manager : this->r_evse_manager) {
        evse_manager->call_set_plug_and_charge_configuration(pnc_config);
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

std::optional<ocpp::v2::IdToken> get_authorized_id_token(const types::evse_manager::SessionEvent& session_event) {
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

ocpp::v2::ChargingRateUnitEnum get_unit_or_default(const std::string& unit_string) {
    try {
        return ocpp::v2::conversions::string_to_charging_rate_unit_enum(unit_string);
    } catch (const std::out_of_range& e) {
        EVLOG_warning << "RequestCompositeScheduleUnit configured incorrectly with: " << unit_string
                      << ". Defaulting to using Amps.";
        return ocpp::v2::ChargingRateUnitEnum::A;
    }
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
    invoke_init(*p_auth_provider);
    invoke_init(*p_auth_validator);

    source_ext_limit = info.id + "/OCPP_set_external_limits";

    // ensure all evse_energy_sink(s) that are connected have an evse id mapping
    for (const auto& evse_sink : this->r_evse_energy_sink) {
        if (not evse_sink->get_mapping().has_value()) {
            EVLOG_critical << "Please configure an evse mapping in your configuration file for the connected "
                              "r_evse_energy_sink with module_id: "
                           << evse_sink->module_id;
            throw std::runtime_error("At least one connected evse_energy_sink misses a mapping to an evse.");
        }
    }

    this->init_evse_maps();

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

    const auto device_model_database_migration_path = [&]() {
        const auto config_device_model_database_migration_path =
            fs::path(this->config.DeviceModelDatabaseMigrationPath);
        if (config_device_model_database_migration_path.is_relative()) {
            return this->ocpp_share_path / config_device_model_database_migration_path;
        } else {
            return config_device_model_database_migration_path;
        }
    }();

    const auto device_model_config_path = [&]() {
        const auto config_device_model_config_path = fs::path(this->config.DeviceModelConfigPath);
        if (config_device_model_config_path.is_relative()) {
            return this->ocpp_share_path / config_device_model_config_path;
        } else {
            return config_device_model_config_path;
        }
    }();

    if (!fs::exists(this->config.MessageLogPath)) {
        try {
            fs::create_directory(this->config.MessageLogPath);
        } catch (const fs::filesystem_error& e) {
            EVLOG_AND_THROW(e);
        }
    }

    ocpp::v2::Callbacks callbacks;
    callbacks.is_reset_allowed_callback = [this](const std::optional<const int32_t> evse_id,
                                                 const ocpp::v2::ResetEnum& type) {
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
    callbacks.reset_callback = [this](const std::optional<const int32_t> evse_id, const ocpp::v2::ResetEnum& type) {
        if (evse_id.has_value()) {
            EVLOG_warning << "Reset of EVSE is currently not supported";
            return;
        }

        bool scheduled = type == ocpp::v2::ResetEnum::OnIdle;

        try {
            this->r_system->call_reset(types::system::ResetType::NotSpecified, scheduled);
        } catch (std::out_of_range& e) {
            EVLOG_warning << "Could not convert OCPP ResetEnum to EVerest ResetType while executing reset_callack. No "
                             "reset will be executed.";
        }
    };

    callbacks.connector_effective_operative_status_changed_callback =
        [this](const int32_t evse_id, const int32_t connector_id, const ocpp::v2::OperationalStatusEnum new_status) {
            if (new_status == ocpp::v2::OperationalStatusEnum::Operative) {
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

    callbacks.remote_start_transaction_callback = [this](const ocpp::v2::RequestStartTransactionRequest& request,
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
        return ocpp::v2::RequestStartStopStatusEnum::Accepted;
    };

    callbacks.stop_transaction_callback = [this](const int32_t evse_id, const ocpp::v2::ReasonEnum& stop_reason) {
        if (evse_id <= 0 or evse_id > this->r_evse_manager.size()) {
            return ocpp::v2::RequestStartStopStatusEnum::Rejected;
        }

        types::evse_manager::StopTransactionRequest req;
        req.reason = conversions::to_everest_stop_transaction_reason(stop_reason);

        return this->r_evse_manager.at(evse_id - 1)->call_stop_transaction(req)
                   ? ocpp::v2::RequestStartStopStatusEnum::Accepted
                   : ocpp::v2::RequestStartStopStatusEnum::Rejected;
    };

    callbacks.pause_charging_callback = [this](const int32_t evse_id) {
        if (evse_id > 0 && evse_id <= this->r_evse_manager.size()) {
            this->r_evse_manager.at(evse_id - 1)->call_pause_charging();
        }
    };

    callbacks.unlock_connector_callback = [this](const int32_t evse_id, const int32_t connector_id) {
        // FIXME: This needs to properly handle different connectors
        ocpp::v2::UnlockConnectorResponse response;
        if (evse_id > 0 && evse_id <= this->r_evse_manager.size()) {
            if (this->r_evse_manager.at(evse_id - 1)->call_force_unlock(connector_id)) {
                response.status = ocpp::v2::UnlockStatusEnum::Unlocked;
            } else {
                response.status = ocpp::v2::UnlockStatusEnum::UnlockFailed;
            }
        } else {
            response.status = ocpp::v2::UnlockStatusEnum::UnknownConnector;
        }

        return response;
    };

    callbacks.get_log_request_callback = [this](const ocpp::v2::GetLogRequest& request) {
        const auto response = this->r_system->call_upload_logs(conversions::to_everest_upload_logs_request(request));
        return conversions::to_ocpp_get_log_response(response);
    };

    callbacks.is_reservation_for_token_callback = [this](const int32_t evse_id, const ocpp::CiString<255> idToken,
                                                         const std::optional<ocpp::CiString<255>> groupIdToken) {
        if (this->r_reservation.empty() || this->r_reservation.at(0) == nullptr) {
            return ocpp::ReservationCheckStatus::NotReserved;
        }

        types::reservation::ReservationCheck reservation_check_request;
        reservation_check_request.evse_id = evse_id;
        reservation_check_request.id_token = idToken.get();
        if (groupIdToken.has_value()) {
            reservation_check_request.group_id_token = groupIdToken.value().get();
        }

        const types::reservation::ReservationCheckStatus reservation_status =
            this->r_reservation.at(0)->call_exists_reservation(reservation_check_request);

        return ocpp_conversions::to_ocpp_reservation_check_status(reservation_status);
    };

    callbacks.update_firmware_request_callback = [this](const ocpp::v2::UpdateFirmwareRequest& request) {
        const auto response =
            this->r_system->call_update_firmware(conversions::to_everest_firmware_update_request(request));
        return conversions::to_ocpp_update_firmware_response(response);
    };

    callbacks.variable_changed_callback = [this](const ocpp::v2::SetVariableData& set_variable_data) {
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
        } else if (set_variable_data.component.name == "ISO15118Ctrlr" and
                   set_variable_data.variable.name == "PncEnabled") {
            types::evse_manager::PlugAndChargeConfiguration pnc_config;
            pnc_config.pnc_enabled = ocpp::conversions::string_to_bool(set_variable_data.attributeValue.get());
            for (const auto& evse_manager : this->r_evse_manager) {
                evse_manager->call_set_plug_and_charge_configuration(pnc_config);
            }
        } else if (set_variable_data.component.name == "ISO15118Ctrlr" and
                   set_variable_data.variable.name == "CentralContractValidationAllowed") {
            types::evse_manager::PlugAndChargeConfiguration pnc_config;
            pnc_config.central_contract_validation_allowed =
                ocpp::conversions::string_to_bool(set_variable_data.attributeValue.get());
            for (const auto& evse_manager : this->r_evse_manager) {
                evse_manager->call_set_plug_and_charge_configuration(pnc_config);
            }
        } else if (set_variable_data.component.name == "ISO15118Ctrlr" and
                   set_variable_data.variable.name == "ContractCertificateInstallationEnabled") {
            types::evse_manager::PlugAndChargeConfiguration pnc_config;
            pnc_config.contract_certificate_installation_enabled =
                ocpp::conversions::string_to_bool(set_variable_data.attributeValue.get());
            for (const auto& evse_manager : this->r_evse_manager) {
                evse_manager->call_set_plug_and_charge_configuration(pnc_config);
            }
        }
    };

    callbacks.validate_network_profile_callback =
        [this](const int32_t configuration_slot, const ocpp::v2::NetworkConnectionProfile& network_connection_profile) {
            auto ws_uri = ocpp::uri(network_connection_profile.ocppCsmsUrl.get());

            if (ws_uri.get_valid()) {
                return ocpp::v2::SetNetworkProfileStatusEnum::Accepted;
            } else {
                return ocpp::v2::SetNetworkProfileStatusEnum::Rejected;
            }
            // TODO(piet): Add further validation of the NetworkConnectionProfile
        };

    callbacks.configure_network_connection_profile_callback =
        [this](const int32_t configuration_slot, const ocpp::v2::NetworkConnectionProfile& network_connection_profile) {
            std::promise<ocpp::v2::ConfigNetworkResult> promise;
            std::future<ocpp::v2::ConfigNetworkResult> future = promise.get_future();
            ocpp::v2::ConfigNetworkResult result;
            result.success = true;
            promise.set_value(result);
            return future;
        };

    callbacks.all_connectors_unavailable_callback = [this]() {
        EVLOG_info << "All connectors unavailable, proceed with firmware installation";
        this->r_system->call_allow_firmware_installation();
    };

    callbacks.transaction_event_callback = [this](const ocpp::v2::TransactionEventRequest& transaction_event) {
        const auto ocpp_transaction_event = conversions::to_everest_ocpp_transaction_event(transaction_event);
        this->p_ocpp_generic->publish_ocpp_transaction_event(ocpp_transaction_event);
    };

    callbacks.transaction_event_response_callback =
        [this](const ocpp::v2::TransactionEventRequest& transaction_event,
               const ocpp::v2::TransactionEventResponse& transaction_event_response) {
            auto ocpp_transaction_event = conversions::to_everest_ocpp_transaction_event(transaction_event);
            auto ocpp_transaction_event_response =
                conversions::to_everest_transaction_event_response(transaction_event_response);
            ocpp_transaction_event_response.original_transaction_event = ocpp_transaction_event;
            this->p_ocpp_generic->publish_ocpp_transaction_event_response(ocpp_transaction_event_response);
        };

    callbacks.boot_notification_callback =
        [this](const ocpp::v2::BootNotificationResponse& boot_notification_response) {
            const auto everest_boot_notification_response =
                conversions::to_everest_boot_notification_response(boot_notification_response);
            this->p_ocpp_generic->publish_boot_notification_response(everest_boot_notification_response);
        };

    callbacks.set_display_message_callback =
        [this](const std::vector<ocpp::DisplayMessage>& messages) -> ocpp::v2::SetDisplayMessageResponse {
        ocpp::v2::SetDisplayMessageResponse response;
        if (this->r_display_message.empty()) {
            response.status = ocpp::v2::DisplayMessageStatusEnum::Rejected;
            return response;
        }

        std::vector<types::display_message::DisplayMessage> display_messages;
        for (const ocpp::DisplayMessage& message : messages) {
            const types::display_message::DisplayMessage m = ocpp_conversions::to_everest_display_message(message);
            display_messages.push_back(m);
        }

        const types::display_message::SetDisplayMessageResponse display_message_response =
            this->r_display_message.at(0)->call_set_display_message(display_messages);
        response = conversions::to_ocpp_set_display_message_response(display_message_response);

        return response;
    };

    callbacks.get_display_message_callback =
        [this](const ocpp::v2::GetDisplayMessagesRequest& request) -> std::vector<ocpp::DisplayMessage> {
        if (this->r_display_message.empty()) {
            return {};
        }
        types::display_message::GetDisplayMessageRequest get_request;

        types::display_message::GetDisplayMessageResponse response =
            this->r_display_message.at(0)->call_get_display_messages(
                conversions::to_everest_display_message_request(request));

        if (!response.messages.has_value() || response.messages.value().empty()) {
            return {};
        }

        std::vector<ocpp::DisplayMessage> ocpp_display_messages;
        for (const auto& message : response.messages.value()) {
            ocpp_display_messages.push_back(ocpp_conversions::to_ocpp_display_message(message));
        }

        return ocpp_display_messages;
    };

    callbacks.clear_display_message_callback =
        [this](const ocpp::v2::ClearDisplayMessageRequest& request) -> ocpp::v2::ClearDisplayMessageResponse {
        if (this->r_display_message.empty()) {
            ocpp::v2::ClearDisplayMessageResponse response;
            response.status = ocpp::v2::ClearMessageStatusEnum::Unknown;
            return response;
        }

        types::display_message::ClearDisplayMessageResponse response =
            this->r_display_message.at(0)->call_clear_display_message(
                conversions::to_everest_clear_display_message_request(request));
        return conversions::to_ocpp_clear_display_message_response(response);
    };

    if (this->p_session_cost != nullptr) {
        callbacks.set_running_cost_callback = [this](const ocpp::RunningCost& running_cost,
                                                     const uint32_t number_of_decimals,
                                                     const std::optional<std::string>& currency_code) {
            std::optional<types::money::CurrencyCode> currency;
            if (currency_code.has_value()) {
                try {
                    currency = types::money::string_to_currency_code(currency_code.value());
                } catch (const std::out_of_range& e) {
                    // If conversion fails, we just don't add the currency code. But we want to see it in the logging.
                    EVLOG_error << e.what();
                }
            }
            const types::session_cost::SessionCost cost =
                ocpp_conversions::create_session_cost(running_cost, number_of_decimals, currency);
            this->p_session_cost->publish_session_cost(cost);
        };
    }

    if (!this->r_data_transfer.empty()) {
        callbacks.data_transfer_callback = [this](const ocpp::v2::DataTransferRequest& request) {
            types::ocpp::DataTransferRequest data_transfer_request =
                conversions::to_everest_data_transfer_request(request);
            types::ocpp::DataTransferResponse data_transfer_response =
                this->r_data_transfer.at(0)->call_data_transfer(data_transfer_request);
            ocpp::v2::DataTransferResponse response =
                conversions::to_ocpp_data_transfer_response(data_transfer_response);
            return response;
        };
    }

    callbacks.connection_state_changed_callback =
        [this](const bool is_connected, const int /*configuration_slot*/,
               const ocpp::v2::NetworkConnectionProfile& /*network_connection_profile*/,
               const auto) { this->p_ocpp_generic->publish_is_connected(is_connected); };

    callbacks.security_event_callback = [this](const ocpp::CiString<50>& event_type,
                                               const std::optional<ocpp::CiString<255>>& tech_info) {
        types::ocpp::SecurityEvent event;
        event.type = event_type.get();
        EVLOG_info << "Security Event in OCPP occurred: " << event.type;
        if (tech_info.has_value()) {
            event.info = tech_info.value().get();
        }
        this->p_ocpp_generic->publish_security_event(event);
    };

    const auto composite_schedule_unit = get_unit_or_default(this->config.RequestCompositeScheduleUnit);

    // this callback publishes the schedules within EVerest and applies the schedules for the individual
    // r_evse_energy_sink
    const auto charging_schedules_callback = [this, composite_schedule_unit]() {
        const auto composite_schedules = this->charge_point->get_all_composite_schedules(
            this->config.RequestCompositeScheduleDurationS, composite_schedule_unit);
        this->publish_charging_schedules(composite_schedules);
        this->set_external_limits(composite_schedules);
    };

    callbacks.set_charging_profiles_callback = charging_schedules_callback;

    callbacks.time_sync_callback = [this](const ocpp::DateTime& current_time) {
        this->r_system->call_set_system_time(current_time.to_rfc3339());
    };

    callbacks.reserve_now_callback =
        [this](const ocpp::v2::ReserveNowRequest& request) -> ocpp::v2::ReserveNowStatusEnum {
        ocpp::v2::ReserveNowResponse response;
        if (this->r_reservation.empty() || this->r_reservation.at(0) == nullptr) {
            EVLOG_info << "Reservation rejected because the interface r_reservation is a nullptr";
            return ocpp::v2::ReserveNowStatusEnum::Rejected;
        }

        types::reservation::Reservation reservation;
        reservation.reservation_id = request.id;
        reservation.expiry_time = request.expiryDateTime.to_rfc3339();
        reservation.id_token = request.idToken.idToken;
        reservation.evse_id = request.evseId;
        if (request.groupIdToken.has_value()) {
            reservation.parent_id_token = request.groupIdToken.value().idToken;
        }
        if (request.connectorType.has_value()) {
            reservation.connector_type =
                types::evse_manager::string_to_connector_type_enum(request.connectorType.value());
        }

        types::reservation::ReservationResult result = this->r_reservation.at(0)->call_reserve_now(reservation);
        return conversions::to_ocpp_reservation_status(result);
    };

    callbacks.cancel_reservation_callback = [this](const int32_t reservation_id) -> bool {
        EVLOG_debug << "Received cancel reservation request for reservation id " << reservation_id;
        if (this->r_reservation.empty() || this->r_reservation.at(0) == nullptr) {
            return false;
        }

        return this->r_reservation.at(0)->call_cancel_reservation(reservation_id);
    };

    const auto sql_init_path = this->ocpp_share_path / SQL_CORE_MIGRATIONS;

    std::map<int32_t, int32_t> evse_connector_structure = this->get_connector_structure();
    std::unique_ptr<module::device_model::ComposedDeviceModelStorage> device_model_storage =
        std::make_unique<module::device_model::ComposedDeviceModelStorage>(
            device_model_database_path, true, device_model_database_migration_path, device_model_config_path);
    this->charge_point = std::make_unique<ocpp::v2::ChargePoint>(
        evse_connector_structure, std::move(device_model_storage), this->ocpp_share_path.string(),
        this->config.CoreDatabasePath, sql_init_path.string(), this->config.MessageLogPath,
        std::make_shared<EvseSecurity>(*this->r_security), callbacks);

    const auto error_handler = [this](const Everest::error::Error& error) {
        if (error.type == EVSE_MANAGER_INOPERATIVE_ERROR) {
            // handled by specific evse_manager error handler
            return;
        }
        const auto event_data = get_event_data(error, false, this->event_id_counter++);
        this->charge_point->on_event({event_data});
    };

    const auto error_cleared_handler = [this](const Everest::error::Error& error) {
        if (error.type == EVSE_MANAGER_INOPERATIVE_ERROR) {
            // handled by specific evse_manager error handler
            return;
        }
        const auto event_data = get_event_data(error, true, this->event_id_counter++);
        this->charge_point->on_event({event_data});
    };

    subscribe_global_all_errors(error_handler, error_cleared_handler);

    // publish charging schedules at least once on startup
    charging_schedules_callback();

    if (this->config.CompositeScheduleIntervalS > 0) {
        this->charging_schedules_timer.interval(charging_schedules_callback,
                                                std::chrono::seconds(this->config.CompositeScheduleIntervalS));
    }

    this->init_module_configuration();

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
        ocpp::v2::Component{"TxCtrlr"}, ocpp::v2::Variable{"TxStartPoint"}, ocpp::v2::AttributeEnum::Actual);
    if (tx_start_point_request_value_response.status == ocpp::v2::GetVariableStatusEnum::Accepted and
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
        ocpp::v2::Component{"TxCtrlr"}, ocpp::v2::Variable{"TxStopPoint"}, ocpp::v2::AttributeEnum::Actual);
    if (tx_stop_point_request_value_response.status == ocpp::v2::GetVariableStatusEnum::Accepted and
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
            auto meter_value = conversions::to_ocpp_meter_value(
                power_meter, ocpp::v2::ReadingContextEnum::Sample_Periodic, power_meter.signed_meter_value);
            if (this->evse_soc_map[evse_id].has_value()) {
                auto sampled_soc_value = conversions::to_ocpp_sampled_value(
                    ocpp::v2::ReadingContextEnum::Sample_Periodic, ocpp::v2::MeasurandEnum::SoC, "Percent",
                    std::nullopt, ocpp::v2::LocationEnum::EV);
                sampled_soc_value.value = this->evse_soc_map[evse_id].value();
                meter_value.sampledValue.push_back(sampled_soc_value);
            }
            this->charge_point->on_meter_value(evse_id, meter_value);
        });

        evse->subscribe_ev_info([this, evse_id](const types::evse_manager::EVInfo& ev_info) {
            if (ev_info.soc.has_value()) {
                this->evse_soc_map[evse_id] = ev_info.soc.value();
            }
        });

        auto fault_handler = [this, evse_id](const Everest::error::Error& error) {
            this->charge_point->on_faulted(evse_id, get_connector_id_from_error(error));
        };

        auto fault_cleared_handler = [this, evse_id](const Everest::error::Error& error) {
            this->charge_point->on_fault_cleared(evse_id, get_connector_id_from_error(error));
        };

        // A permanent fault from the evse requirement indicates that the evse should move to faulted state
        evse->subscribe_error(EVSE_MANAGER_INOPERATIVE_ERROR, fault_handler, fault_cleared_handler);

        evse_id++;
    }

    int32_t extensions_id = 0;
    for (auto& extension : this->r_extensions_15118) {
        extension->subscribe_iso15118_certificate_request(
            [this, extensions_id](const types::iso15118::RequestExiStreamSchema& certificate_request) {
                auto ocpp_response = this->charge_point->on_get_15118_ev_certificate_request(
                    conversions::to_ocpp_get_15118_certificate_request(certificate_request));
                EVLOG_debug << "Received response from get_15118_ev_certificate_request: " << ocpp_response;
                // transform response, inject action, send to associated EvseManager
                types::iso15118::ResponseExiStreamStatus everest_response;
                everest_response.status = conversions::to_everest_iso15118_status(ocpp_response.status);
                everest_response.certificate_action = certificate_request.certificate_action;
                if (not ocpp_response.exiResponse.get().empty()) {
                    // since exi_response is an optional in the EVerest type we only set it when not empty
                    everest_response.exi_response = ocpp_response.exiResponse.get();
                }

                this->r_extensions_15118.at(extensions_id)->call_set_get_certificate_response(everest_response);
            });

        extension->subscribe_charging_needs([this,
                                             extensions_id](const types::iso15118::ChargingNeeds& charging_needs) {
            const auto& mapping = this->r_extensions_15118.at(extensions_id)->get_mapping();
            if (mapping.has_value()) {
                try {
                    auto charge_needs = conversions::to_ocpp_notify_ev_charging_needs_request(charging_needs);
                    charge_needs.evseId = mapping.value().evse;

                    this->charge_point->on_ev_charging_needs(charge_needs);
                } catch (const std::out_of_range& e) {
                    EVLOG_warning << "Could not convert iso15118 ChargingNeeds to OCPP NotifyEVChargingNeedsRequest: "
                                  << e.what();
                }
            } else {
                EVLOG_warning << "ISO15118 Extension interface mapping not set! Not sending 'ChargingNeeds'!";
            }
        });

        extensions_id++;
    }

    r_system->subscribe_firmware_update_status([this](const types::system::FirmwareUpdateStatus status) {
        this->charge_point->on_firmware_update_status_notification(
            status.request_id, conversions::to_ocpp_firmware_status_enum(status.firmware_update_status));
    });

    r_system->subscribe_log_status([this](types::system::LogStatus status) {
        this->charge_point->on_log_status_notification(conversions::to_ocpp_upload_logs_status_enum(status.log_status),
                                                       status.request_id);
    });

    if (!this->r_reservation.empty() && this->r_reservation.at(0) != nullptr) {
        r_reservation.at(0)->subscribe_reservation_update(
            [this](const types::reservation::ReservationUpdateStatus status) {
                if (status.reservation_status == types::reservation::Reservation_status::Expired ||
                    status.reservation_status == types::reservation::Reservation_status::Removed) {
                    EVLOG_debug << "Received reservation status update for reservation " << status.reservation_id
                                << ": "
                                << (status.reservation_status == types::reservation::Reservation_status::Expired
                                        ? "Expired"
                                        : "Removed");
                    try {
                        this->charge_point->on_reservation_status(
                            status.reservation_id,
                            conversions::to_ocpp_reservation_update_status_enum(status.reservation_status));
                    } catch (const std::out_of_range& e) {
                    }
                }
            });
    }

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
    std::lock_guard<std::mutex> lg(this->session_event_mutex);
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
    case types::evse_manager::SessionEventEnum::ReservationStart: {
        this->process_reserved(evse_id, connector_id);
        break;
    }
    case types::evse_manager::SessionEventEnum::ReservationEnd: {
        this->process_reservation_end(evse_id, connector_id);
        break;
    }
    // explicitly ignore the following session events for now
    // TODO(kai): implement
    case types::evse_manager::SessionEventEnum::AuthRequired:
    case types::evse_manager::SessionEventEnum::PrepareCharging:
    case types::evse_manager::SessionEventEnum::WaitingForEnergy:
    case types::evse_manager::SessionEventEnum::StoppingCharging:
    case types::evse_manager::SessionEventEnum::ChargingFinished:
    case types::evse_manager::SessionEventEnum::ReplugStarted:
    case types::evse_manager::SessionEventEnum::ReplugFinished:
    case types::evse_manager::SessionEventEnum::PluginTimeout:
    case types::evse_manager::SessionEventEnum::SwitchingPhases:
    case types::evse_manager::SessionEventEnum::SessionResumed:
        break;
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
    transaction_data->timestamp = ocpp_conversions::to_ocpp_datetime_or_now(session_event.timestamp);

    if (tx_event_effect == TxEventEffect::START_TRANSACTION) {
        transaction_data->started = true;
        transaction_data->meter_value = conversions::to_ocpp_meter_value(
            get_meter_value(session_event), ocpp::v2::ReadingContextEnum::Transaction_Begin,
            get_signed_meter_value(session_event));
        this->charge_point->on_transaction_started(
            evse_id, transaction_data->connector_id, transaction_data->session_id, transaction_data->timestamp,
            transaction_data->trigger_reason, transaction_data->meter_value, transaction_data->id_token,
            transaction_data->group_id_token, transaction_data->reservation_id, transaction_data->remote_start_id,
            transaction_data->charging_state);
    } else if (tx_event_effect == TxEventEffect::STOP_TRANSACTION) {
        transaction_data->meter_value = conversions::to_ocpp_meter_value(get_meter_value(session_event),
                                                                         ocpp::v2::ReadingContextEnum::Transaction_End,
                                                                         get_signed_meter_value(session_event));
        this->charge_point->on_transaction_finished(evse_id, transaction_data->timestamp, transaction_data->meter_value,
                                                    transaction_data->stop_reason, transaction_data->trigger_reason,
                                                    transaction_data->id_token, std::nullopt,
                                                    transaction_data->charging_state);
        this->transaction_handler->reset_transaction_data(evse_id);
    }
}

void OCPP201::process_session_started(const int32_t evse_id, const int32_t connector_id,
                                      const types::evse_manager::SessionEvent& session_event) {
    if (!session_event.session_started.has_value()) {
        throw std::runtime_error("SessionEvent SessionStarted does not contain session_started context");
    }

    const auto session_started = session_event.session_started.value();

    std::optional<ocpp::v2::IdToken> id_token = std::nullopt;
    std::optional<ocpp::v2::IdToken> group_id_token = std::nullopt;
    std::optional<int32_t> remote_start_id = std::nullopt;
    auto charging_state = ocpp::v2::ChargingStateEnum::Idle;
    auto trigger_reason = ocpp::v2::TriggerReasonEnum::Authorized;
    auto tx_event = TxEvent::AUTHORIZED;
    if (session_started.reason == types::evse_manager::StartSessionReason::EVConnected) {
        tx_event = TxEvent::EV_CONNECTED;
        trigger_reason = ocpp::v2::TriggerReasonEnum::CablePluggedIn;
        charging_state = ocpp::v2::ChargingStateEnum::EVConnected;
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
            trigger_reason = ocpp::v2::TriggerReasonEnum::RemoteStart;
        }
    }
    const auto timestamp = ocpp_conversions::to_ocpp_datetime_or_now(session_event.timestamp);
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
    this->evse_soc_map[evse_id].reset();
    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    if (transaction_data != nullptr) {
        transaction_data->charging_state = ocpp::v2::ChargingStateEnum::Idle;
        transaction_data->stop_reason = ocpp::v2::ReasonEnum::EVDisconnected;
        transaction_data->trigger_reason = ocpp::v2::TriggerReasonEnum::EVCommunicationLost;
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
        EVLOG_warning << "Could not update transaction data because no transaction data is present. This might happen "
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
    auto trigger_reason = ocpp::v2::TriggerReasonEnum::Authorized;
    const auto transaction_started = session_event.transaction_started.value();
    if (transaction_started.reservation_id.has_value()) {
        transaction_data->reservation_id = transaction_started.reservation_id;
    }
    transaction_data->remote_start_id = transaction_started.id_tag.request_id;
    const auto id_token = conversions::to_ocpp_id_token(transaction_started.id_tag.id_token);
    transaction_data->id_token = id_token;

    std::optional<ocpp::v2::IdToken> group_id_token = std::nullopt;
    if (transaction_started.id_tag.parent_id_token.has_value()) {
        transaction_data->group_id_token =
            conversions::to_ocpp_id_token(transaction_started.id_tag.parent_id_token.value());
    }

    // if session started reason was Authorized, Transaction is started because of EV plug in event
    if (transaction_data->trigger_reason == ocpp::v2::TriggerReasonEnum::Authorized or
        transaction_data->trigger_reason == ocpp::v2::TriggerReasonEnum::RemoteStart) {
        trigger_reason = ocpp::v2::TriggerReasonEnum::CablePluggedIn;
        transaction_data->charging_state = ocpp::v2::ChargingStateEnum::EVConnected;
        this->charge_point->on_session_started(evse_id, connector_id);
        tx_event = TxEvent::EV_CONNECTED;
    }

    if (transaction_started.id_tag.authorization_type == types::authorization::AuthorizationType::OCPP) {
        trigger_reason = ocpp::v2::TriggerReasonEnum::RemoteStart;
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
    auto reason = ocpp::v2::ReasonEnum::Other;
    if (transaction_finished.reason.has_value()) {
        reason = conversions::to_ocpp_reason(transaction_finished.reason.value());
        tx_event = get_tx_event(reason);
    }
    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    if (transaction_data != nullptr) {
        std::optional<ocpp::v2::IdToken> id_token = std::nullopt;
        if (transaction_finished.id_tag.has_value()) {
            id_token = conversions::to_ocpp_id_token(transaction_finished.id_tag.value().id_token);
        }

        // this is required to report the correct charging_state within a TransactionEvent(Ended) message
        auto charging_state = transaction_data->charging_state;
        if (reason == ocpp::v2::ReasonEnum::EVDisconnected) {
            charging_state = ocpp::v2::ChargingStateEnum::Idle;
        } else if (reason == ocpp::v2::ReasonEnum::ImmediateReset &&
                   charging_state != ocpp::v2::ChargingStateEnum::Idle) {
            charging_state = ocpp::v2::ChargingStateEnum::EVConnected;
        } else if (tx_event == TxEvent::DEAUTHORIZED) {
            charging_state = ocpp::v2::ChargingStateEnum::EVConnected;
        }
        transaction_data->trigger_reason = stop_reason_to_trigger_reason_enum(reason);
        transaction_data->stop_reason = reason;
        transaction_data->id_token = id_token;
        transaction_data->charging_state = charging_state;
    }

    // tx_event could be DEAUTHORIZED or EV_DISCONNECTED
    const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, tx_event);
    this->process_tx_event_effect(evse_id, tx_event_effect, session_event);

    if (tx_event == TxEvent::DEAUTHORIZED) {
        if (reason == ocpp::v2::ReasonEnum::Remote) {
            this->charge_point->on_charging_state_changed(evse_id, ocpp::v2::ChargingStateEnum::EVConnected,
                                                          ocpp::v2::TriggerReasonEnum::RemoteStop);
        } else {
            this->charge_point->on_charging_state_changed(evse_id, ocpp::v2::ChargingStateEnum::EVConnected,
                                                          ocpp::v2::TriggerReasonEnum::StopAuthorized);
        }
    } else {
        // TODO(piet): If StopTxOnEVSideDisconnect is false, authorization shall still be present. This cannot only be
        // handled within this module, but probably also within EvseManager and Auth

        // authorization is always withdrawn in case of TransactionFinished, so in case we haven't updated the
        // transaction handler yet, we have to do it
        // now
        const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, TxEvent::DEAUTHORIZED);
        this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
    }
}

void OCPP201::process_charging_started(const int32_t evse_id, const int32_t connector_id,
                                       const types::evse_manager::SessionEvent& session_event) {
    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    if (transaction_data != nullptr) {
        transaction_data->trigger_reason = ocpp::v2::TriggerReasonEnum::ChargingStateChanged;
        transaction_data->charging_state = ocpp::v2::ChargingStateEnum::Charging;
    }
    const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, TxEvent::ENERGY_TRANSFER_STARTED);
    this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
    this->charge_point->on_charging_state_changed(evse_id, ocpp::v2::ChargingStateEnum::Charging);
}

void OCPP201::process_charging_resumed(const int32_t evse_id, const int32_t connector_id,
                                       const types::evse_manager::SessionEvent& session_event) {
    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    if (transaction_data != nullptr) {
        transaction_data->charging_state = ocpp::v2::ChargingStateEnum::Charging;
    }
    this->charge_point->on_charging_state_changed(evse_id, ocpp::v2::ChargingStateEnum::Charging);
}

void OCPP201::process_charging_paused_ev(const int32_t evse_id, const int32_t connector_id,
                                         const types::evse_manager::SessionEvent& session_event) {
    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    if (transaction_data != nullptr) {
        transaction_data->charging_state = ocpp::v2::ChargingStateEnum::SuspendedEV;
        transaction_data->trigger_reason = ocpp::v2::TriggerReasonEnum::ChargingStateChanged;
        transaction_data->stop_reason = ocpp::v2::ReasonEnum::StoppedByEV;
    }
    const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, TxEvent::ENERGY_TRANSFER_STOPPED);
    this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
    this->charge_point->on_charging_state_changed(evse_id, ocpp::v2::ChargingStateEnum::SuspendedEV);
}

void OCPP201::process_charging_paused_evse(const int32_t evse_id, const int32_t connector_id,
                                           const types::evse_manager::SessionEvent& session_event) {
    auto transaction_data = this->transaction_handler->get_transaction_data(evse_id);
    auto trigger_reason = ocpp::v2::TriggerReasonEnum::ChargingStateChanged;
    if (transaction_data != nullptr) {
        transaction_data->charging_state = ocpp::v2::ChargingStateEnum::SuspendedEVSE;
        if (transaction_data->stop_reason == ocpp::v2::ReasonEnum::Remote) {
            trigger_reason = ocpp::v2::TriggerReasonEnum::RemoteStop;
            transaction_data->trigger_reason = ocpp::v2::TriggerReasonEnum::ChargingStateChanged;
        }
    }
    const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, TxEvent::ENERGY_TRANSFER_STOPPED);
    this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
    this->charge_point->on_charging_state_changed(evse_id, ocpp::v2::ChargingStateEnum::SuspendedEVSE, trigger_reason);
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
        transaction_data->trigger_reason = ocpp::v2::TriggerReasonEnum::StopAuthorized;
    }
    const auto tx_event_effect = this->transaction_handler->submit_event(evse_id, TxEvent::DEAUTHORIZED);
    this->process_tx_event_effect(evse_id, tx_event_effect, session_event);
}

void OCPP201::process_reserved(const int32_t evse_id, const int32_t connector_id) {
    this->charge_point->on_reserved(evse_id, connector_id);
}

void OCPP201::process_reservation_end(const int32_t evse_id, const int32_t connector_id) {
    this->charge_point->on_reservation_cleared(evse_id, connector_id);
}

void OCPP201::publish_charging_schedules(const std::vector<ocpp::v2::CompositeSchedule>& composite_schedules) {
    const auto everest_schedules = conversions::to_everest_charging_schedules(composite_schedules);
    this->p_ocpp_generic->publish_charging_schedules(everest_schedules);
}

void OCPP201::set_external_limits(const std::vector<ocpp::v2::CompositeSchedule>& composite_schedules) {
    const auto start_time = ocpp::DateTime();

    // iterate over all schedules reported by the libocpp to create ExternalLimits
    // for each connector

    for (const auto& composite_schedule : composite_schedules) {
        auto evse_id = composite_schedule.evseId;
        if (not external_energy_limits::is_evse_sink_configured(this->r_evse_energy_sink, evse_id)) {
            EVLOG_warning << "Can not apply external limits! No evse energy sink configured for evse_id: " << evse_id;
            continue;
        }

        types::energy::ExternalLimits limits;
        std::vector<types::energy::ScheduleReqEntry> schedule_import;

        for (const auto& period : composite_schedule.chargingSchedulePeriod) {
            types::energy::ScheduleReqEntry schedule_req_entry;
            types::energy::LimitsReq limits_req;
            const auto timestamp = start_time.to_time_point() + std::chrono::seconds(period.startPeriod);
            schedule_req_entry.timestamp = ocpp::DateTime(timestamp).to_rfc3339();
            if (composite_schedule.chargingRateUnit == ocpp::v2::ChargingRateUnitEnum::A) {
                if (period.limit.has_value()) {
                    limits_req.ac_max_current_A = {period.limit.value(), source_ext_limit};
                }
                if (period.numberPhases.has_value()) {
                    limits_req.ac_max_phase_count = {period.numberPhases.value(), source_ext_limit};
                }
            } else {
                if (period.limit.has_value()) {
                    limits_req.total_power_W = {period.limit.value(), source_ext_limit};
                }
            }
            schedule_req_entry.limits_to_leaves = limits_req;
            schedule_import.push_back(schedule_req_entry);
        }
        limits.schedule_import = schedule_import;

        auto& evse_sink = external_energy_limits::get_evse_sink_by_evse_id(this->r_evse_energy_sink, evse_id);
        evse_sink.call_set_external_limits(limits);
    }
}

} // namespace module
