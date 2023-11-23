// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "OCPP201.hpp"

#include <fmt/core.h>
#include <fstream>
#include <websocketpp/uri.hpp>

#include <evse_security_ocpp.hpp>
namespace module {

const std::string INIT_SQL = "init_core.sql";
const std::string CERTS_DIR = "certs";

const std::string KVS_OCPP201_INOPERATIVE_KEY_PREFIX = "OCPP201_INOPERATIVE_";

namespace fs = std::filesystem;

ocpp::v201::FirmwareStatusEnum get_firmware_status_notification(const types::system::FirmwareUpdateStatusEnum status) {
    switch (status) {
    case types::system::FirmwareUpdateStatusEnum::Downloaded:
        return ocpp::v201::FirmwareStatusEnum::Downloaded;
    case types::system::FirmwareUpdateStatusEnum::DownloadFailed:
        return ocpp::v201::FirmwareStatusEnum::DownloadFailed;
    case types::system::FirmwareUpdateStatusEnum::Downloading:
        return ocpp::v201::FirmwareStatusEnum::Downloading;
    case types::system::FirmwareUpdateStatusEnum::DownloadScheduled:
        return ocpp::v201::FirmwareStatusEnum::DownloadScheduled;
    case types::system::FirmwareUpdateStatusEnum::DownloadPaused:
        return ocpp::v201::FirmwareStatusEnum::DownloadPaused;
    case types::system::FirmwareUpdateStatusEnum::Idle:
        return ocpp::v201::FirmwareStatusEnum::Idle;
    case types::system::FirmwareUpdateStatusEnum::InstallationFailed:
        return ocpp::v201::FirmwareStatusEnum::InstallationFailed;
    case types::system::FirmwareUpdateStatusEnum::Installing:
        return ocpp::v201::FirmwareStatusEnum::Installing;
    case types::system::FirmwareUpdateStatusEnum::Installed:
        return ocpp::v201::FirmwareStatusEnum::Installed;
    case types::system::FirmwareUpdateStatusEnum::InstallRebooting:
        return ocpp::v201::FirmwareStatusEnum::InstallRebooting;
    case types::system::FirmwareUpdateStatusEnum::InstallScheduled:
        return ocpp::v201::FirmwareStatusEnum::InstallScheduled;
    case types::system::FirmwareUpdateStatusEnum::InstallVerificationFailed:
        return ocpp::v201::FirmwareStatusEnum::InstallVerificationFailed;
    case types::system::FirmwareUpdateStatusEnum::InvalidSignature:
        return ocpp::v201::FirmwareStatusEnum::InvalidSignature;
    case types::system::FirmwareUpdateStatusEnum::SignatureVerified:
        return ocpp::v201::FirmwareStatusEnum::SignatureVerified;
    default:
        throw std::out_of_range("Could not convert FirmwareUpdateStatusEnum to FirmwareStatusEnum");
    }
}

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
            sampled_value.value = power_meter.voltage_V.value().DC.value();
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

ocpp::v201::BootReasonEnum get_boot_reason(types::system::BootReason reason) {
    switch (reason) {
    case types::system::BootReason::ApplicationReset:
        return ocpp::v201::BootReasonEnum::ApplicationReset;
    case types::system::BootReason::FirmwareUpdate:
        return ocpp::v201::BootReasonEnum::FirmwareUpdate;
    case types::system::BootReason::LocalReset:
        return ocpp::v201::BootReasonEnum::LocalReset;
    case types::system::BootReason::PowerUp:
        return ocpp::v201::BootReasonEnum::PowerUp;
    case types::system::BootReason::RemoteReset:
        return ocpp::v201::BootReasonEnum::RemoteReset;
    case types::system::BootReason::ScheduledReset:
        return ocpp::v201::BootReasonEnum::ScheduledReset;
    case types::system::BootReason::Triggered:
        return ocpp::v201::BootReasonEnum::Triggered;
    case types::system::BootReason::Unknown:
        return ocpp::v201::BootReasonEnum::Unknown;
    case types::system::BootReason::Watchdog:
        return ocpp::v201::BootReasonEnum::Watchdog;
    default:
        throw std::runtime_error("Could not convert BootReasonEnum");
    }
}

ocpp::v201::ReasonEnum get_reason(types::evse_manager::StopTransactionReason reason) {
    switch (reason) {
    case types::evse_manager::StopTransactionReason::DeAuthorized:
        return ocpp::v201::ReasonEnum::DeAuthorized;
    case types::evse_manager::StopTransactionReason::EmergencyStop:
        return ocpp::v201::ReasonEnum::EmergencyStop;
    case types::evse_manager::StopTransactionReason::EnergyLimitReached:
        return ocpp::v201::ReasonEnum::EnergyLimitReached;
    case types::evse_manager::StopTransactionReason::EVDisconnected:
        return ocpp::v201::ReasonEnum::EVDisconnected;
    case types::evse_manager::StopTransactionReason::GroundFault:
        return ocpp::v201::ReasonEnum::GroundFault;
    case types::evse_manager::StopTransactionReason::HardReset:
        return ocpp::v201::ReasonEnum::ImmediateReset;
    case types::evse_manager::StopTransactionReason::Local:
        return ocpp::v201::ReasonEnum::Local;
    case types::evse_manager::StopTransactionReason::LocalOutOfCredit:
        return ocpp::v201::ReasonEnum::LocalOutOfCredit;
    case types::evse_manager::StopTransactionReason::MasterPass:
        return ocpp::v201::ReasonEnum::MasterPass;
    case types::evse_manager::StopTransactionReason::Other:
        return ocpp::v201::ReasonEnum::Other;
    case types::evse_manager::StopTransactionReason::OvercurrentFault:
        return ocpp::v201::ReasonEnum::OvercurrentFault;
    case types::evse_manager::StopTransactionReason::PowerLoss:
        return ocpp::v201::ReasonEnum::PowerLoss;
    case types::evse_manager::StopTransactionReason::PowerQuality:
        return ocpp::v201::ReasonEnum::PowerQuality;
    case types::evse_manager::StopTransactionReason::Reboot:
        return ocpp::v201::ReasonEnum::Reboot;
    case types::evse_manager::StopTransactionReason::Remote:
        return ocpp::v201::ReasonEnum::Remote;
    case types::evse_manager::StopTransactionReason::SOCLimitReached:
        return ocpp::v201::ReasonEnum::SOCLimitReached;
    case types::evse_manager::StopTransactionReason::StoppedByEV:
        return ocpp::v201::ReasonEnum::StoppedByEV;
    case types::evse_manager::StopTransactionReason::TimeLimitReached:
        return ocpp::v201::ReasonEnum::TimeLimitReached;
    case types::evse_manager::StopTransactionReason::Timeout:
        return ocpp::v201::ReasonEnum::Timeout;
    default:
        return ocpp::v201::ReasonEnum::Other;
    }
}

void OCPP201::init_evse_ready_map() {
    std::lock_guard<std::mutex> lk(this->evse_ready_mutex);
    for (size_t evse_id = 1; evse_id <= this->r_evse_manager.size(); evse_id++) {
        this->evse_ready_map[evse_id] = false;
    }
}

void OCPP201::init_evses() {

    if (this->r_kvs->call_exists(KVS_OCPP201_INOPERATIVE_KEY_PREFIX + "0")) {
        this->cs_operational_status = ocpp::v201::OperationalStatusEnum::Inoperative;
    } else {
        this->cs_operational_status = ocpp::v201::OperationalStatusEnum::Operative;
    }

    int evse_id = 1;
    for (const auto& evse : this->r_evse_manager) {
        int connector_id = 1;
        auto _evse = evse->call_get_evse();
        if (_evse.id != evse_id) {
            throw std::runtime_error("Configured evse_id(s) must start with 1 counting upwards");
        }

        if (_evse.connectors.size() == 0) {
            _evse.connectors.push_back({1});
        }

        Evse module_evse;
        module_evse.evse_id = evse_id;

        if (this->r_kvs->call_exists(KVS_OCPP201_INOPERATIVE_KEY_PREFIX + std::to_string(evse_id))) {
            module_evse.operational_state = ocpp::v201::OperationalStatusEnum::Inoperative;
        } else {
            module_evse.operational_state = ocpp::v201::OperationalStatusEnum::Operative;
        }

        for (const auto& connector : _evse.connectors) {
            if (connector.id != connector_id) {
                throw std::runtime_error("Configured connector_id(s) must start with 1 counting upwards");
            }

            auto connector_id_str = std::to_string(evse_id) + "." + std::to_string(connector_id);
            if (this->r_kvs->call_exists(KVS_OCPP201_INOPERATIVE_KEY_PREFIX + connector_id_str)) {
                module_evse.connectors[connector_id] = ocpp::v201::OperationalStatusEnum::Inoperative;
            } else {
                module_evse.connectors[connector_id] = ocpp::v201::OperationalStatusEnum::Operative;
            }
            connector_id++;
        }

        this->evses[_evse.id] = module_evse;
        evse_id++;
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

void OCPP201::set_connector_operational_status(const ocpp::v201::OperationalStatusEnum operational_status,
                                               const int32_t evse_id, const int32_t connector_id, const bool persist) {
    if (operational_status == ocpp::v201::OperationalStatusEnum::Operative) {
        this->evses.at(evse_id).connectors.at(connector_id) = ocpp::v201::OperationalStatusEnum::Operative;
        auto connector_id_str = std::to_string(evse_id) + "." + std::to_string(connector_id);
        this->r_kvs->call_delete(KVS_OCPP201_INOPERATIVE_KEY_PREFIX + connector_id_str);
    } else if (persist) {
        this->evses.at(evse_id).connectors.at(connector_id) = ocpp::v201::OperationalStatusEnum::Inoperative;
        auto connector_id_str = std::to_string(evse_id) + "." + std::to_string(connector_id);
        this->r_kvs->call_store(KVS_OCPP201_INOPERATIVE_KEY_PREFIX + connector_id_str, true);
    }
}

void OCPP201::set_evse_operational_status(const ocpp::v201::OperationalStatusEnum operational_status,
                                          const int32_t evse_id, const bool persist) {
    if (operational_status == ocpp::v201::OperationalStatusEnum::Operative) {
        this->evses.at(evse_id).operational_state = ocpp::v201::OperationalStatusEnum::Operative;
        this->r_kvs->call_delete(KVS_OCPP201_INOPERATIVE_KEY_PREFIX + std::to_string(evse_id));
    } else if (persist) {
        this->evses.at(evse_id).operational_state = ocpp::v201::OperationalStatusEnum::Inoperative;
        this->r_kvs->call_store(KVS_OCPP201_INOPERATIVE_KEY_PREFIX + std::to_string(evse_id), true);
    }
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
                this->evse_ready_map[evse_id] = true;
                this->evse_ready_cv.notify_one();
            }
        });
    }
}

ocpp::v201::CertificateActionEnum get_certificate_action(const types::iso15118_charger::CertificateActionEnum& action) {
    switch (action) {
    case types::iso15118_charger::CertificateActionEnum::Install:
        return ocpp::v201::CertificateActionEnum::Install;
    case types::iso15118_charger::CertificateActionEnum::Update:
        return ocpp::v201::CertificateActionEnum::Update;
    }
    throw std::out_of_range("Could not convert CertificateActionEnum"); // this should never happen
}

types::iso15118_charger::Status get_iso15118_charger_status(const ocpp::v201::Iso15118EVCertificateStatusEnum& status) {
    switch (status) {
    case ocpp::v201::Iso15118EVCertificateStatusEnum::Accepted:
        return types::iso15118_charger::Status::Accepted;
    case ocpp::v201::Iso15118EVCertificateStatusEnum::Failed:
        return types::iso15118_charger::Status::Failed;
    }
    throw std::out_of_range("Could not convert Iso15118EVCertificateStatusEnum"); // this should never happen
}

void OCPP201::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_auth_provider);
    invoke_ready(*p_auth_validator);

    this->ocpp_share_path = this->info.paths.share;
    this->cs_operational_status = ocpp::v201::OperationalStatusEnum::Operative;

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

    callbacks.change_availability_callback = [this](const ocpp::v201::ChangeAvailabilityRequest& request,
                                                    const bool persist) {
        if (request.evse.has_value()) {
            auto evse_id = request.evse.value().id;
            auto connector_id = request.evse.value().connectorId;
            if (request.operationalStatus == ocpp::v201::OperationalStatusEnum::Operative) {
                // change to operative
                if (connector_id.has_value()) {
                    // connector is addressed
                    this->set_connector_operational_status(ocpp::v201::OperationalStatusEnum::Operative, evse_id,
                                                           connector_id.value(), persist);
                } else {
                    // EVSE is addressed
                    this->set_evse_operational_status(ocpp::v201::OperationalStatusEnum::Operative, evse_id, persist);
                }
                this->r_evse_manager.at(evse_id - 1)->call_enable(request.evse.value().connectorId.value_or(0));
            } else {
                // change to inoperative
                if (connector_id.has_value()) {
                    // connector is addressed
                    this->set_connector_operational_status(ocpp::v201::OperationalStatusEnum::Inoperative, evse_id,
                                                           connector_id.value(), persist);
                } else {
                    // EVSE is addressed
                    this->set_evse_operational_status(ocpp::v201::OperationalStatusEnum::Inoperative, evse_id, persist);
                }
                this->r_evse_manager.at(evse_id - 1)->call_disable(request.evse.value().connectorId.value_or(0));
            }
        } else {
            // whole charging station is adressed
            this->cs_operational_status = request.operationalStatus;
            for (size_t evse_id = 1; evse_id <= this->r_evse_manager.size(); evse_id++) {
                if (this->cs_operational_status == ocpp::v201::OperationalStatusEnum::Operative and
                    this->evses.at(evse_id).operational_state == ocpp::v201::OperationalStatusEnum::Operative) {
                    this->r_evse_manager.at(evse_id - 1)->call_enable(0);
                    this->r_kvs->call_delete(KVS_OCPP201_INOPERATIVE_KEY_PREFIX + "0");
                } else if (request.operationalStatus == ocpp::v201::OperationalStatusEnum::Inoperative) {
                    this->r_evse_manager.at(evse_id - 1)->call_disable(0);
                    this->r_kvs->call_store(KVS_OCPP201_INOPERATIVE_KEY_PREFIX + "0", true);
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

    const auto sql_init_path = this->ocpp_share_path / INIT_SQL;

    this->init_evses();

    std::map<int32_t, int32_t> evse_connector_structure;
    for (const auto [evse_id, evse] : this->evses) {
        evse_connector_structure[evse_id] = evse.connectors.size();
    }

    this->charge_point = std::make_unique<ocpp::v201::ChargePoint>(
        evse_connector_structure, device_model_database_path, this->ocpp_share_path.string(),
        this->config.CoreDatabasePath, sql_init_path.string(), this->config.MessageLogPath,
        std::make_shared<EvseSecurity>(*this->r_security), callbacks);

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
            switch (session_event.event) {
            case types::evse_manager::SessionEventEnum::SessionStarted: {
                this->session_started_reason = session_event.session_started.value().reason;
                this->charge_point->on_session_started(evse_id, connector_id);
                break;
            }
            case types::evse_manager::SessionEventEnum::SessionFinished: {
                this->charge_point->on_session_finished(evse_id, connector_id);
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

                // assume cable has been plugged in first and then authorized
                auto trigger_reason = ocpp::v201::TriggerReasonEnum::Authorized;

                // if session started reason was Authorized, Transaction is started because of EV plug in event
                if (this->session_started_reason == types::evse_manager::StartSessionReason::Authorized) {
                    trigger_reason = ocpp::v201::TriggerReasonEnum::CablePluggedIn;
                }

                if (transaction_started.id_tag.authorization_type == types::authorization::AuthorizationType::OCPP) {
                    trigger_reason = ocpp::v201::TriggerReasonEnum::RemoteStart;
                }

                this->charge_point->on_transaction_started(
                    evse_id, connector_id, session_id, timestamp, trigger_reason, meter_value, id_token, std::nullopt,
                    reservation_id, remote_start_id,
                    ocpp::v201::ChargingStateEnum::EVConnected); // FIXME(piet): add proper groupIdToken +
                                                                 // ChargingStateEnum
                break;
            }
            case types::evse_manager::SessionEventEnum::TransactionFinished: {
                const auto transaction_finished = session_event.transaction_finished.value();
                const auto timestamp = ocpp::DateTime(transaction_finished.timestamp);
                const auto meter_value =
                    get_meter_value(transaction_finished.meter_value, ocpp::v201::ReadingContextEnum::Transaction_End);
                ocpp::v201::ReasonEnum reason = ocpp::v201::ReasonEnum::Other;

                if (transaction_finished.reason.has_value()) {
                    reason = get_reason(transaction_finished.reason.value());
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
                if (session_event.connector_id.has_value()) {
                    this->charge_point->on_unavailable(evse_id, connector_id);
                } else {
                    for (size_t index = 1; index <= this->evses.at(evse_id).connectors.size(); index++) {
                        this->charge_point->on_unavailable(evse_id, index);
                    }
                }
                break;
            }
            case types::evse_manager::SessionEventEnum::Enabled: {
                if (session_event.connector_id.has_value()) {
                    if (this->evses.at(evse_id).operational_state == ocpp::v201::OperationalStatusEnum::Operative) {
                        this->charge_point->on_operative(evse_id, connector_id);
                    }
                } else {
                    for (size_t index = 1; index <= this->evses.at(evse_id).connectors.size(); index++) {
                        if (this->evses.at(evse_id).connectors.at(index) ==
                            ocpp::v201::OperationalStatusEnum::Operative) {
                            this->charge_point->on_operative(evse_id, index);
                        }
                    }
                }
                break;
            }
            }
        });

        evse->subscribe_powermeter([this, evse_id](const types::powermeter::Powermeter& power_meter) {
            const auto meter_value = get_meter_value(power_meter, ocpp::v201::ReadingContextEnum::Sample_Periodic);
            this->charge_point->on_meter_value(evse_id, meter_value);
        });

        evse->subscribe_iso15118_certificate_request(
            [this, evse_id](const types::iso15118_charger::Request_Exi_Stream_Schema& certificate_request) {
                // transform request forward to libocpp
                ocpp::v201::Get15118EVCertificateRequest ocpp_request;
                ocpp_request.exiRequest = certificate_request.exiRequest;
                ocpp_request.iso15118SchemaVersion = certificate_request.iso15118SchemaVersion;
                ocpp_request.action = get_certificate_action(certificate_request.certificateAction);

                auto ocpp_response = this->charge_point->on_get_15118_ev_certificate_request(ocpp_request);
                EVLOG_debug << "Received response from get_15118_ev_certificate_request: " << ocpp_response;
                // transform response, inject action, send to associated EvseManager
                const auto everest_response_status = get_iso15118_charger_status(ocpp_response.status);
                const types::iso15118_charger::Response_Exi_Stream_Status everest_response{
                    everest_response_status, certificate_request.certificateAction, ocpp_response.exiResponse};
                this->r_evse_manager.at(evse_id - 1)->call_set_get_certificate_response(everest_response);
            });

        evse_id++;
    }
    r_system->subscribe_firmware_update_status([this](const types::system::FirmwareUpdateStatus status) {
        this->charge_point->on_firmware_update_status_notification(
            status.request_id, get_firmware_status_notification(status.firmware_update_status));
    });

    r_system->subscribe_log_status([this](types::system::LogStatus status) {
        this->charge_point->on_log_status_notification(get_upload_log_status_enum(status.log_status),
                                                       status.request_id);
    });

    std::unique_lock lk(this->evse_ready_mutex);
    while (!this->all_evse_ready()) {
        this->evse_ready_cv.wait(lk);
    }

    // align state machine based on operational status
    for (const auto& [evse_id, evse] : this->evses) {
        if (evse.operational_state == ocpp::v201::OperationalStatusEnum::Inoperative or
            this->cs_operational_status == ocpp::v201::OperationalStatusEnum::Inoperative) {
            this->r_evse_manager.at(evse_id - 1)->call_disable(0);
        }
        for (const auto [connector_id, operational_state] : evse.connectors) {
            if (operational_state == ocpp::v201::OperationalStatusEnum::Inoperative) {
                this->r_evse_manager.at(evse_id - 1)->call_disable(connector_id);
            }
        }
    }

    const auto boot_reason = get_boot_reason(this->r_system->call_get_boot_reason());
    this->charge_point->start(boot_reason);
}

} // namespace module
