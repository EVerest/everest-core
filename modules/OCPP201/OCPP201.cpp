// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "OCPP201.hpp"

#include <fmt/core.h>
#include <fstream>

namespace module {

const std::string INIT_SQL = "init_core.sql";
const std::string CERTS_DIR = "certs";

namespace fs = std::filesystem;

types::evse_manager::StopTransactionReason get_stop_reason(const ocpp::v201::ReasonEnum& stop_reason) {
    switch (stop_reason) {
    case ocpp::v201::ReasonEnum::DeAuthorized:
        return types::evse_manager::StopTransactionReason::DeAuthorized;
    case ocpp::v201::ReasonEnum::EmergencyStop:
        return types::evse_manager::StopTransactionReason::EmergencyStop;
    case ocpp::v201::ReasonEnum::EnergyLimitReached:
        return types::evse_manager::StopTransactionReason::EnergyLimitReached;
    case ocpp::v201::ReasonEnum::EVDisconnected:
        return types::evse_manager::StopTransactionReason::EVDisconnected;
    case ocpp::v201::ReasonEnum::GroundFault:
        return types::evse_manager::StopTransactionReason::GroundFault;
    case ocpp::v201::ReasonEnum::ImmediateReset:
        return types::evse_manager::StopTransactionReason::HardReset;
    case ocpp::v201::ReasonEnum::Local:
        return types::evse_manager::StopTransactionReason::Local;
    case ocpp::v201::ReasonEnum::LocalOutOfCredit:
        return types::evse_manager::StopTransactionReason::LocalOutOfCredit;
    case ocpp::v201::ReasonEnum::MasterPass:
        return types::evse_manager::StopTransactionReason::MasterPass;
    case ocpp::v201::ReasonEnum::Other:
        return types::evse_manager::StopTransactionReason::Other;
    case ocpp::v201::ReasonEnum::OvercurrentFault:
        return types::evse_manager::StopTransactionReason::OvercurrentFault;
    case ocpp::v201::ReasonEnum::PowerLoss:
        return types::evse_manager::StopTransactionReason::PowerLoss;
    case ocpp::v201::ReasonEnum::PowerQuality:
        return types::evse_manager::StopTransactionReason::PowerQuality;
    case ocpp::v201::ReasonEnum::Reboot:
        return types::evse_manager::StopTransactionReason::Reboot;
    case ocpp::v201::ReasonEnum::Remote:
        return types::evse_manager::StopTransactionReason::Remote;
    case ocpp::v201::ReasonEnum::SOCLimitReached:
        return types::evse_manager::StopTransactionReason::SOCLimitReached;
    case ocpp::v201::ReasonEnum::StoppedByEV:
        return types::evse_manager::StopTransactionReason::StoppedByEV;
    case ocpp::v201::ReasonEnum::TimeLimitReached:
        return types::evse_manager::StopTransactionReason::TimeLimitReached;
    case ocpp::v201::ReasonEnum::Timeout:
        return types::evse_manager::StopTransactionReason::Timeout;
    default:
        return types::evse_manager::StopTransactionReason::Other;
    }
}

ocpp::v201::SampledValue get_sampled_value(const ocpp::v201::ReadingContextEnum& reading_context,
                                           const ocpp::v201::MeasurandEnum& measurand, const std::string& unit,
                                           const std::optional<ocpp::v201::PhaseEnum> phase) {
    ocpp::v201::SampledValue sampled_value;
    ocpp::v201::UnitOfMeasure unit_of_measure;
    sampled_value.context = reading_context;
    sampled_value.location = ocpp::v201::LocationEnum::Outlet;
    sampled_value.measurand = measurand;
    unit_of_measure.unit = unit;
    sampled_value.unitOfMeasure = unit_of_measure;
    sampled_value.phase = phase;
    return sampled_value;
}

ocpp::v201::MeterValue get_meter_value(const types::powermeter::Powermeter& power_meter,
                                       const ocpp::v201::ReadingContextEnum& reading_context) {
    ocpp::v201::MeterValue meter_value;
    meter_value.timestamp = ocpp::DateTime(power_meter.timestamp);

    // Energy.Active.Import.Register
    ocpp::v201::SampledValue sampled_value = get_sampled_value(
        reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Import_Register, "Wh", std::nullopt);
    sampled_value.value = power_meter.energy_Wh_import.total;
    meter_value.sampledValue.push_back(sampled_value);
    if (power_meter.energy_Wh_import.L1.has_value()) {
        sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Import_Register,
                                          "Wh", ocpp::v201::PhaseEnum::L1);
        sampled_value.value = power_meter.energy_Wh_import.L1.value();
        meter_value.sampledValue.push_back(sampled_value);
    }
    if (power_meter.energy_Wh_import.L2.has_value()) {
        sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Import_Register,
                                          "Wh", ocpp::v201::PhaseEnum::L2);
        sampled_value.value = power_meter.energy_Wh_import.L2.value();
        meter_value.sampledValue.push_back(sampled_value);
    }
    if (power_meter.energy_Wh_import.L3.has_value()) {
        sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Import_Register,
                                          "Wh", ocpp::v201::PhaseEnum::L3);
        sampled_value.value = power_meter.energy_Wh_import.L3.value();
        meter_value.sampledValue.push_back(sampled_value);
    }

    // Energy.Active.Export.Register
    if (power_meter.energy_Wh_export.has_value()) {
        auto sampled_value = get_sampled_value(
            reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Export_Register, "Wh", std::nullopt);
        sampled_value.value = power_meter.energy_Wh_export.value().total;
        meter_value.sampledValue.push_back(sampled_value);
        if (power_meter.energy_Wh_export.value().L1.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Export_Register,
                                              "Wh", ocpp::v201::PhaseEnum::L1);
            sampled_value.value = power_meter.energy_Wh_import.L1.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.energy_Wh_export.value().L2.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Export_Register,
                                              "Wh", ocpp::v201::PhaseEnum::L2);
            sampled_value.value = power_meter.energy_Wh_import.L2.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.energy_Wh_export.value().L3.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Export_Register,
                                              "Wh", ocpp::v201::PhaseEnum::L3);
            sampled_value.value = power_meter.energy_Wh_import.L3.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
    }

    // Power.Active.Import
    if (power_meter.power_W.has_value()) {
        auto sampled_value =
            get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Active_Import, "W", std::nullopt);
        sampled_value.value = power_meter.power_W.value().total;
        meter_value.sampledValue.push_back(sampled_value);
        if (power_meter.power_W.value().L1.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Active_Import, "W",
                                              ocpp::v201::PhaseEnum::L1);
            sampled_value.value = power_meter.energy_Wh_import.L1.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.power_W.value().L2.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Active_Import, "W",
                                              ocpp::v201::PhaseEnum::L2);
            sampled_value.value = power_meter.energy_Wh_import.L2.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.power_W.value().L3.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Active_Import, "W",
                                              ocpp::v201::PhaseEnum::L3);
            sampled_value.value = power_meter.energy_Wh_import.L3.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
    }

    // Power.Reactive.Import
    if (power_meter.VAR.has_value()) {
        auto sampled_value =
            get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Reactive_Import, "var", std::nullopt);
        sampled_value.value = power_meter.VAR.value().total;
        meter_value.sampledValue.push_back(sampled_value);
        if (power_meter.VAR.value().L1.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Reactive_Import, "var",
                                              ocpp::v201::PhaseEnum::L1);
            sampled_value.value = power_meter.energy_Wh_import.L1.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.VAR.value().L2.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Reactive_Import, "var",
                                              ocpp::v201::PhaseEnum::L2);
            sampled_value.value = power_meter.energy_Wh_import.L2.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.VAR.value().L3.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Reactive_Import, "var",
                                              ocpp::v201::PhaseEnum::L3);
            sampled_value.value = power_meter.energy_Wh_import.L3.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
    }

    // Current.Import
    if (power_meter.current_A.has_value()) {
        auto sampled_value =
            get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Current_Import, "A", std::nullopt);
        if (power_meter.current_A.value().L1.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Current_Import, "A",
                                              ocpp::v201::PhaseEnum::L1);
            sampled_value.value = power_meter.current_A.value().L1.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.current_A.value().L2.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Current_Import, "A",
                                              ocpp::v201::PhaseEnum::L2);
            sampled_value.value = power_meter.current_A.value().L2.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.current_A.value().L3.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Current_Import, "A",
                                              ocpp::v201::PhaseEnum::L3);
            sampled_value.value = power_meter.current_A.value().L3.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.current_A.value().DC.has_value()) {
            sampled_value =
                get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Current_Import, "A", std::nullopt);
            sampled_value.value = power_meter.current_A.value().DC.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.current_A.value().N.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Current_Import, "A",
                                              ocpp::v201::PhaseEnum::N);
            sampled_value.value = power_meter.current_A.value().N.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
    }

    // Voltage
    if (power_meter.voltage_V.has_value()) {
        if (power_meter.voltage_V.value().L1.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Voltage, "V",
                                              ocpp::v201::PhaseEnum::L1_N);
            sampled_value.value = power_meter.voltage_V.value().L1.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.voltage_V.value().L2.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Voltage, "V",
                                              ocpp::v201::PhaseEnum::L2_N);
            sampled_value.value = power_meter.voltage_V.value().L2.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.voltage_V.value().L3.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Voltage, "V",
                                              ocpp::v201::PhaseEnum::L3_N);
            sampled_value.value = power_meter.voltage_V.value().L3.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.voltage_V.value().DC.has_value()) {
            sampled_value = get_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Voltage, "V", std::nullopt);
            sampled_value.value = power_meter.voltage_V.value().L1.value();
            meter_value.sampledValue.push_back(sampled_value);
        }
    }
    return meter_value;
}

ocpp::v201::LogStatusEnum get_log_status_enum(types::system::UploadLogsStatus log_status) {
    switch (log_status) {
    case types::system::UploadLogsStatus::Accepted:
        return ocpp::v201::LogStatusEnum::Accepted;
    case types::system::UploadLogsStatus::Rejected:
        return ocpp::v201::LogStatusEnum::Rejected;
    case types::system::UploadLogsStatus::AcceptedCancelled:
        return ocpp::v201::LogStatusEnum::AcceptedCanceled;
    default:
        throw std::runtime_error("Could not convert UploadLogsStatus");
    }
}

types::system::UploadLogsRequest get_log_request(const ocpp::v201::GetLogRequest& request) {
    types::system::UploadLogsRequest _request;
    _request.location = request.log.remoteLocation.get();
    _request.retries = request.retries;
    _request.retry_interval_s = request.retryInterval;

    if (request.log.oldestTimestamp.has_value()) {
        _request.oldest_timestamp = request.log.oldestTimestamp.value().to_rfc3339();
    }
    if (request.log.latestTimestamp.has_value()) {
        _request.latest_timestamp = request.log.latestTimestamp.value().to_rfc3339();
    }
    _request.type = ocpp::v201::conversions::log_enum_to_string(request.logType);
    _request.request_id = request.requestId;
    return _request;
}

ocpp::v201::GetLogResponse get_log_response(const types::system::UploadLogsResponse& response) {
    ocpp::v201::GetLogResponse _response;
    _response.status = get_log_status_enum(response.upload_logs_status);
    _response.filename = response.file_name;
    return _response;
}

ocpp::v201::UpdateFirmwareStatusEnum
get_update_firmware_status_enum(const types::system::UpdateFirmwareResponse& response) {
    switch (response) {
    case types::system::UpdateFirmwareResponse::Accepted:
        return ocpp::v201::UpdateFirmwareStatusEnum::Accepted;
    case types::system::UpdateFirmwareResponse::Rejected:
        return ocpp::v201::UpdateFirmwareStatusEnum::Rejected;
    case types::system::UpdateFirmwareResponse::AcceptedCancelled:
        return ocpp::v201::UpdateFirmwareStatusEnum::AcceptedCanceled;
    case types::system::UpdateFirmwareResponse::InvalidCertificate:
        return ocpp::v201::UpdateFirmwareStatusEnum::InvalidCertificate;
    case types::system::UpdateFirmwareResponse::RevokedCertificate:
        return ocpp::v201::UpdateFirmwareStatusEnum::RevokedCertificate;
    default:
        throw std::runtime_error("Could not convert UpdateFirmwareResponse");
    }
}

types::system::FirmwareUpdateRequest get_firmware_update_request(const ocpp::v201::UpdateFirmwareRequest& request) {
    types::system::FirmwareUpdateRequest _request;
    _request.request_id = request.requestId;
    _request.location = request.firmware.location.get();
    _request.retries = request.retries;
    _request.retry_interval_s = request.retryInterval;
    _request.retrieve_timestamp = request.firmware.retrieveDateTime.to_rfc3339();
    if (request.firmware.installDateTime.has_value()) {
        _request.install_timestamp = request.firmware.installDateTime.value().to_rfc3339();
    }
    if (request.firmware.signingCertificate.has_value()) {
        _request.signing_certificate = request.firmware.signingCertificate.value().get();
    }
    if (request.firmware.signature.has_value()) {
        _request.signature = request.firmware.signature.value().get();
    }
    return _request;
}

ocpp::v201::UpdateFirmwareResponse get_update_firmware_response(const types::system::UpdateFirmwareResponse& response) {
    ocpp::v201::UpdateFirmwareResponse _response;
    _response.status = get_update_firmware_status_enum(response);
    return _response;
}

ocpp::v201::UploadLogStatusEnum get_upload_log_status_enum(types::system::LogStatusEnum status) {
    switch (status) {
    case types::system::LogStatusEnum::BadMessage:
        return ocpp::v201::UploadLogStatusEnum::BadMessage;
    case types::system::LogStatusEnum::Idle:
        return ocpp::v201::UploadLogStatusEnum::Idle;
    case types::system::LogStatusEnum::NotSupportedOperation:
        return ocpp::v201::UploadLogStatusEnum::NotSupportedOperation;
    case types::system::LogStatusEnum::PermissionDenied:
        return ocpp::v201::UploadLogStatusEnum::PermissionDenied;
    case types::system::LogStatusEnum::Uploaded:
        return ocpp::v201::UploadLogStatusEnum::Uploaded;
    case types::system::LogStatusEnum::UploadFailure:
        return ocpp::v201::UploadLogStatusEnum::UploadFailure;
    case types::system::LogStatusEnum::Uploading:
        return ocpp::v201::UploadLogStatusEnum::Uploading;
    default:
        throw std::runtime_error("Could not convert UploadLogStatusEnum");
    }
}

void OCPP201::init() {
    invoke_init(*p_main);
    invoke_init(*p_auth_provider);
    invoke_init(*p_auth_validator);

    this->ocpp_share_path = this->info.paths.share;
    this->operational_evse_states[0] = ocpp::v201::OperationalStatusEnum::Operative;

    const auto etc_certs_path = [&]() {
        if (this->config.CertsPath.empty()) {
            return fs::path(this->info.paths.etc) / CERTS_DIR;
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
            fmt::format("OCPP config file is not available at given path: {} which was resolved to: {}",
                        this->config.ChargePointConfigPath, configured_config_path.string())));
    }
    EVLOG_info << "OCPP config: " << configured_config_path.string();

    std::ifstream ifs(configured_config_path.c_str());
    std::string config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    auto json_config = json::parse(config_file);

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
        // FIXME: use evse_id!
        try {
            const auto reset_type =
                types::system::string_to_reset_type(ocpp::v201::conversions::reset_enum_to_string(type));
            return this->r_system->call_is_reset_allowed(reset_type);
        } catch (std::out_of_range& e) {
            EVLOG_warning
                << "Could not convert OCPP ResetEnum to EVerest ResetType while executing is_reset_allowed_callback.";
            return false;
        }
    };
    callbacks.reset_callback = [this](const std::optional<const int32_t> evse_id, const ocpp::v201::ResetEnum& type) {
        // FIXME: use evse_id!
        try {
            const auto reset_type =
                types::system::string_to_reset_type(ocpp::v201::conversions::reset_enum_to_string(type));
            this->r_system->call_reset(types::system::ResetType::Soft); // FIXME(piet): fix reset type in system module
                                                                        // according to v201 reset type
        } catch (std::out_of_range& e) {
            EVLOG_warning << "Could not convert OCPP ResetEnum to EVerest ResetType while executing reset_callack. No "
                             "reset will be executed.";
        }
    };

    callbacks.change_availability_callback = [this](const ocpp::v201::ChangeAvailabilityRequest& request,
                                                    const bool persist) {
        // FIXME: use persist flag!
        if (request.evse.has_value()) {
            if (request.evse.value().connectorId.has_value()) {
                // Connector is adressed
                // Today EVSE in EVerest has always one connector
                this->operational_connector_states[request.evse.value().id] = request.operationalStatus;
            } else {
                // EVSE is adressed
                this->operational_evse_states[request.evse.value().id] = request.operationalStatus;
            }

            if (this->operational_evse_states[0] == ocpp::v201::OperationalStatusEnum::Operative and
                this->operational_evse_states[request.evse.value().id] ==
                    ocpp::v201::OperationalStatusEnum::Operative and
                this->operational_connector_states[request.evse.value().id] ==
                    ocpp::v201::OperationalStatusEnum::Operative) {
                this->r_evse_manager.at(request.evse.value().id - 1)
                    ->call_enable(request.evse.value().connectorId.value_or(0));
            } else if (request.operationalStatus == ocpp::v201::OperationalStatusEnum::Inoperative) {
                this->r_evse_manager.at(request.evse.value().id - 1)
                    ->call_disable(request.evse.value().connectorId.value_or(0));
            }
        } else {
            // whole charging station is adressed
            this->operational_evse_states[0] = request.operationalStatus;
            for (const auto& evse : this->r_evse_manager) {
                if (this->operational_evse_states[0] == ocpp::v201::OperationalStatusEnum::Operative and
                    this->operational_evse_states[evse->call_get_evse().id] ==
                        ocpp::v201::OperationalStatusEnum::Operative and
                    this->operational_connector_states[evse->call_get_evse().id] ==
                        ocpp::v201::OperationalStatusEnum::Operative) {
                    evse->call_enable(0);
                } else if (request.operationalStatus == ocpp::v201::OperationalStatusEnum::Inoperative) {
                    evse->call_disable(0);
                }
            }
        }
    };

    callbacks.remote_start_transaction_callback = [this](const ocpp::v201::RequestStartTransactionRequest& request,
                                                         const bool authorize_remote_start) {
        types::authorization::ProvidedIdToken provided_token;
        provided_token.id_token = request.idToken.idToken.get();
        provided_token.authorization_type = types::authorization::AuthorizationType::OCPP;
        provided_token.id_token_type = types::authorization::string_to_id_token_type(
            ocpp::v201::conversions::id_token_enum_to_string(request.idToken.type));
        provided_token.prevalidated = !authorize_remote_start;
        provided_token.request_id = request.remoteStartId;

        if (request.evseId.has_value()) {
            provided_token.connectors = std::vector<int32_t>{request.evseId.value()};
        }
        this->p_auth_provider->publish_provided_token(provided_token);
    };

    callbacks.stop_transaction_callback = [this](const int32_t evse_id, const ocpp::v201::ReasonEnum& stop_reason) {
        if (evse_id > 0 && evse_id <= this->r_evse_manager.size()) {
            types::evse_manager::StopTransactionRequest req;
            req.reason = get_stop_reason(stop_reason);
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
        const auto response = this->r_system->call_upload_logs(get_log_request(request));
        return get_log_response(response);
    };

    callbacks.is_reservation_for_token_callback = [](const int32_t evse_id, const ocpp::CiString<36> idToken,
                                                     const std::optional<ocpp::CiString<36>> groupIdToken) {
        // FIXME: This is just a stub, replace with functionality
        EVLOG_warning << "is_reservation_for_token_callback is still a stub";
        return false;
    };

    callbacks.update_firmware_request_callback = [this](const ocpp::v201::UpdateFirmwareRequest& request) {
        const auto response = this->r_system->call_update_firmware(get_firmware_update_request(request));
        return get_update_firmware_response(response);
    };

    callbacks.validate_network_profile_callback =
        [this](const int32_t configuration_slot,
               const ocpp::v201::NetworkConnectionProfile& network_connection_profile) {
            return ocpp::v201::SetNetworkProfileStatusEnum::Accepted;
        };

    callbacks.configure_network_connection_profile_callback =
        [this](const ocpp::v201::NetworkConnectionProfile& network_connection_profile) { return true; };

    const auto sql_init_path = this->ocpp_share_path / INIT_SQL;
    std::map<int32_t, int32_t> evse_connector_structure;
    int evse_id = 1;
    for (const auto& evse : this->r_evse_manager) {
        evse_connector_structure.emplace(evse_id, 1);
        evse_id++;
    }

    this->charge_point = std::make_unique<ocpp::v201::ChargePoint>(
        evse_connector_structure, this->config.DeviceModelDatabasePath, this->ocpp_share_path.string(),
        this->config.CoreDatabasePath, sql_init_path.string(), this->config.MessageLogPath, etc_certs_path, callbacks);

    if (this->config.EnableExternalWebsocketControl) {
        const std::string connect_topic = "everest_api/ocpp/cmd/connect";
        this->mqtt.subscribe(connect_topic,
                             [this](const std::string& data) { this->charge_point->connect_websocket(); });

        const std::string disconnect_topic = "everest_api/ocpp/cmd/disconnect";
        this->mqtt.subscribe(disconnect_topic,
                             [this](const std::string& data) { this->charge_point->disconnect_websocket(); });
    }

    evse_id = 1;
    for (const auto& evse : this->r_evse_manager) {
        evse->subscribe_session_event([this, evse_id](types::evse_manager::SessionEvent session_event) {
            ocpp::v201::EVSE evse_ref{evse_id}; // in EVerest
            switch (session_event.event) {
            case types::evse_manager::SessionEventEnum::SessionStarted: {
                this->charge_point->on_session_started(evse_id, 1);
                break;
            }
            case types::evse_manager::SessionEventEnum::SessionFinished: {
                this->charge_point->on_session_finished(evse_id, 1);
                break;
            }
            case types::evse_manager::SessionEventEnum::TransactionStarted: {
                const auto transaction_started = session_event.transaction_started.value();
                const auto timestamp = ocpp::DateTime(transaction_started.timestamp);
                const auto meter_value =
                    get_meter_value(transaction_started.meter_value, ocpp::v201::ReadingContextEnum::Transaction_Begin);
                const auto session_id = session_event.uuid;
                const auto signed_meter_value = transaction_started.signed_meter_value;
                const auto reservation_id = transaction_started.reservation_id;
                const auto remote_start_id = transaction_started.id_tag.request_id;

                ocpp::v201::IdToken id_token;
                id_token.idToken = transaction_started.id_tag.id_token;
                if (transaction_started.id_tag.id_token_type.has_value()) {
                    id_token.type =
                        ocpp::v201::conversions::string_to_id_token_enum(types::authorization::id_token_type_to_string(
                            transaction_started.id_tag.id_token_type.value()));
                } else {
                    id_token.type = ocpp::v201::IdTokenEnum::Local; // FIXME(piet)
                }

                this->charge_point->on_transaction_started(
                    evse_id, 1, session_id, timestamp,
                    ocpp::v201::TriggerReasonEnum::RemoteStart, // FIXME(piet): Use proper reason here
                    meter_value, id_token, std::nullopt, reservation_id, remote_start_id,
                    ocpp::v201::ChargingStateEnum::Charging);   // FIXME(piet): add proper groupIdToken +
                                                                // ChargingStateEnum
                break;
            }
            case types::evse_manager::SessionEventEnum::TransactionFinished: {
                const auto transaction_finished = session_event.transaction_finished.value();
                const auto timestamp = ocpp::DateTime(transaction_finished.timestamp);
                const auto meter_value =
                    get_meter_value(transaction_finished.meter_value, ocpp::v201::ReadingContextEnum::Transaction_End);
                ocpp::v201::ReasonEnum reason;
                try {
                    reason = ocpp::v201::conversions::string_to_reason_enum(
                        types::evse_manager::stop_transaction_reason_to_string(transaction_finished.reason.value()));
                } catch (std::out_of_range& e) {
                    reason = ocpp::v201::ReasonEnum::Other;
                }
                const auto signed_meter_value = transaction_finished.signed_meter_value;
                const auto id_token = transaction_finished.id_tag;

                this->charge_point->on_transaction_finished(evse_id, timestamp, meter_value, reason, id_token,
                                                            signed_meter_value,
                                                            ocpp::v201::ChargingStateEnum::EVConnected);
                break;
            }
            case types::evse_manager::SessionEventEnum::ChargingStarted: {
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
                this->charge_point->on_unavailable(evse_id, 1);
                break;
            }
            case types::evse_manager::SessionEventEnum::Enabled: {
                this->operational_evse_states[evse_id] = ocpp::v201::OperationalStatusEnum::Operative;
                this->operational_connector_states[evse_id] = ocpp::v201::OperationalStatusEnum::Operative;
                this->charge_point->on_operative(evse_id, 1);
                break;
            }
            }
        });

        evse->subscribe_powermeter([this, evse_id](const types::powermeter::Powermeter& power_meter) {
            const auto meter_value = get_meter_value(power_meter, ocpp::v201::ReadingContextEnum::Sample_Periodic);
            this->charge_point->on_meter_value(evse_id, meter_value);
        });
        evse_id++;

        r_system->subscribe_firmware_update_status([this](const types::system::FirmwareUpdateStatus status) {
            this->charge_point->on_firmware_update_status_notification(
                status.request_id, types::system::firmware_update_status_enum_to_string(status.firmware_update_status));
        });

        r_system->subscribe_log_status([this](types::system::LogStatus status) {
            this->charge_point->on_log_status_notification(get_upload_log_status_enum(status.log_status),
                                                           status.request_id);
        });
    }

    this->charge_point->start();
}

void OCPP201::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_auth_provider);
    invoke_ready(*p_auth_validator);
}

} // namespace module
