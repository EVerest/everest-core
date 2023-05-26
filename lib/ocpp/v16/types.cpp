// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <optional>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>

#include <ocpp/v16/types.hpp>

namespace ocpp {
namespace v16 {

namespace conversions {
std::string messagetype_to_string(MessageType m) {
    switch (m) {
    case MessageType::Authorize:
        return "Authorize";
    case MessageType::AuthorizeResponse:
        return "AuthorizeResponse";
    case MessageType::BootNotification:
        return "BootNotification";
    case MessageType::BootNotificationResponse:
        return "BootNotificationResponse";
    case MessageType::CancelReservation:
        return "CancelReservation";
    case MessageType::CancelReservationResponse:
        return "CancelReservationResponse";
    case MessageType::CertificateSigned:
        return "CertificateSigned";
    case MessageType::CertificateSignedResponse:
        return "CertificateSignedResponse";
    case MessageType::ChangeAvailability:
        return "ChangeAvailability";
    case MessageType::ChangeAvailabilityResponse:
        return "ChangeAvailabilityResponse";
    case MessageType::ChangeConfiguration:
        return "ChangeConfiguration";
    case MessageType::ChangeConfigurationResponse:
        return "ChangeConfigurationResponse";
    case MessageType::ClearCache:
        return "ClearCache";
    case MessageType::ClearCacheResponse:
        return "ClearCacheResponse";
    case MessageType::ClearChargingProfile:
        return "ClearChargingProfile";
    case MessageType::ClearChargingProfileResponse:
        return "ClearChargingProfileResponse";
    case MessageType::DataTransfer:
        return "DataTransfer";
    case MessageType::DataTransferResponse:
        return "DataTransferResponse";
    case MessageType::DeleteCertificate:
        return "DeleteCertificate";
    case MessageType::DeleteCertificateResponse:
        return "DeleteCertificateResponse";
    case MessageType::DiagnosticsStatusNotification:
        return "DiagnosticsStatusNotification";
    case MessageType::DiagnosticsStatusNotificationResponse:
        return "DiagnosticsStatusNotificationResponse";
    case MessageType::ExtendedTriggerMessage:
        return "ExtendedTriggerMessage";
    case MessageType::ExtendedTriggerMessageResponse:
        return "ExtendedTriggerMessageResponse";
    case MessageType::FirmwareStatusNotification:
        return "FirmwareStatusNotification";
    case MessageType::FirmwareStatusNotificationResponse:
        return "FirmwareStatusNotificationResponse";
    case MessageType::GetCompositeSchedule:
        return "GetCompositeSchedule";
    case MessageType::GetCompositeScheduleResponse:
        return "GetCompositeScheduleResponse";
    case MessageType::GetConfiguration:
        return "GetConfiguration";
    case MessageType::GetConfigurationResponse:
        return "GetConfigurationResponse";
    case MessageType::GetDiagnostics:
        return "GetDiagnostics";
    case MessageType::GetDiagnosticsResponse:
        return "GetDiagnosticsResponse";
    case MessageType::GetInstalledCertificateIds:
        return "GetInstalledCertificateIds";
    case MessageType::GetInstalledCertificateIdsResponse:
        return "GetInstalledCertificateIdsResponse";
    case MessageType::GetLocalListVersion:
        return "GetLocalListVersion";
    case MessageType::GetLocalListVersionResponse:
        return "GetLocalListVersionResponse";
    case MessageType::GetLog:
        return "GetLog";
    case MessageType::GetLogResponse:
        return "GetLogResponse";
    case MessageType::Heartbeat:
        return "Heartbeat";
    case MessageType::HeartbeatResponse:
        return "HeartbeatResponse";
    case MessageType::InstallCertificate:
        return "InstallCertificate";
    case MessageType::InstallCertificateResponse:
        return "InstallCertificateResponse";
    case MessageType::LogStatusNotification:
        return "LogStatusNotification";
    case MessageType::LogStatusNotificationResponse:
        return "LogStatusNotificationResponse";
    case MessageType::MeterValues:
        return "MeterValues";
    case MessageType::MeterValuesResponse:
        return "MeterValuesResponse";
    case MessageType::RemoteStartTransaction:
        return "RemoteStartTransaction";
    case MessageType::RemoteStartTransactionResponse:
        return "RemoteStartTransactionResponse";
    case MessageType::RemoteStopTransaction:
        return "RemoteStopTransaction";
    case MessageType::RemoteStopTransactionResponse:
        return "RemoteStopTransactionResponse";
    case MessageType::ReserveNow:
        return "ReserveNow";
    case MessageType::ReserveNowResponse:
        return "ReserveNowResponse";
    case MessageType::Reset:
        return "Reset";
    case MessageType::ResetResponse:
        return "ResetResponse";
    case MessageType::SecurityEventNotification:
        return "SecurityEventNotification";
    case MessageType::SecurityEventNotificationResponse:
        return "SecurityEventNotificationResponse";
    case MessageType::SendLocalList:
        return "SendLocalList";
    case MessageType::SendLocalListResponse:
        return "SendLocalListResponse";
    case MessageType::SetChargingProfile:
        return "SetChargingProfile";
    case MessageType::SetChargingProfileResponse:
        return "SetChargingProfileResponse";
    case MessageType::SignCertificate:
        return "SignCertificate";
    case MessageType::SignCertificateResponse:
        return "SignCertificateResponse";
    case MessageType::SignedFirmwareStatusNotification:
        return "SignedFirmwareStatusNotification";
    case MessageType::SignedFirmwareStatusNotificationResponse:
        return "SignedFirmwareStatusNotificationResponse";
    case MessageType::SignedUpdateFirmware:
        return "SignedUpdateFirmware";
    case MessageType::SignedUpdateFirmwareResponse:
        return "SignedUpdateFirmwareResponse";
    case MessageType::StartTransaction:
        return "StartTransaction";
    case MessageType::StartTransactionResponse:
        return "StartTransactionResponse";
    case MessageType::StatusNotification:
        return "StatusNotification";
    case MessageType::StatusNotificationResponse:
        return "StatusNotificationResponse";
    case MessageType::StopTransaction:
        return "StopTransaction";
    case MessageType::StopTransactionResponse:
        return "StopTransactionResponse";
    case MessageType::TriggerMessage:
        return "TriggerMessage";
    case MessageType::TriggerMessageResponse:
        return "TriggerMessageResponse";
    case MessageType::UnlockConnector:
        return "UnlockConnector";
    case MessageType::UnlockConnectorResponse:
        return "UnlockConnectorResponse";
    case MessageType::UpdateFirmware:
        return "UpdateFirmware";
    case MessageType::UpdateFirmwareResponse:
        return "UpdateFirmwareResponse";
    case MessageType::InternalError:
        EVLOG_error << "No known string conversion for InternalError MessageType";
        return "InternalError";
    }

    throw std::out_of_range("No known string conversion for provided enum of type MessageType");
}

MessageType string_to_messagetype(const std::string& s) {
    if (s == "Authorize") {
        return MessageType::Authorize;
    }
    if (s == "AuthorizeResponse") {
        return MessageType::AuthorizeResponse;
    }
    if (s == "BootNotification") {
        return MessageType::BootNotification;
    }
    if (s == "BootNotificationResponse") {
        return MessageType::BootNotificationResponse;
    }
    if (s == "CancelReservation") {
        return MessageType::CancelReservation;
    }
    if (s == "CancelReservationResponse") {
        return MessageType::CancelReservationResponse;
    }
    if (s == "CertificateSigned") {
        return MessageType::CertificateSigned;
    }
    if (s == "CertificateSignedResponse") {
        return MessageType::CertificateSignedResponse;
    }
    if (s == "ChangeAvailability") {
        return MessageType::ChangeAvailability;
    }
    if (s == "ChangeAvailabilityResponse") {
        return MessageType::ChangeAvailabilityResponse;
    }
    if (s == "ChangeConfiguration") {
        return MessageType::ChangeConfiguration;
    }
    if (s == "ChangeConfigurationResponse") {
        return MessageType::ChangeConfigurationResponse;
    }
    if (s == "ClearCache") {
        return MessageType::ClearCache;
    }
    if (s == "ClearCacheResponse") {
        return MessageType::ClearCacheResponse;
    }
    if (s == "ClearChargingProfile") {
        return MessageType::ClearChargingProfile;
    }
    if (s == "ClearChargingProfileResponse") {
        return MessageType::ClearChargingProfileResponse;
    }
    if (s == "DataTransfer") {
        return MessageType::DataTransfer;
    }
    if (s == "DataTransferResponse") {
        return MessageType::DataTransferResponse;
    }
    if (s == "DeleteCertificate") {
        return MessageType::DeleteCertificate;
    }
    if (s == "DeleteCertificateResponse") {
        return MessageType::DeleteCertificateResponse;
    }
    if (s == "DiagnosticsStatusNotification") {
        return MessageType::DiagnosticsStatusNotification;
    }
    if (s == "DiagnosticsStatusNotificationResponse") {
        return MessageType::DiagnosticsStatusNotificationResponse;
    }
    if (s == "ExtendedTriggerMessage") {
        return MessageType::ExtendedTriggerMessage;
    }
    if (s == "ExtendedTriggerMessageResponse") {
        return MessageType::ExtendedTriggerMessageResponse;
    }
    if (s == "FirmwareStatusNotification") {
        return MessageType::FirmwareStatusNotification;
    }
    if (s == "FirmwareStatusNotificationResponse") {
        return MessageType::FirmwareStatusNotificationResponse;
    }
    if (s == "GetCompositeSchedule") {
        return MessageType::GetCompositeSchedule;
    }
    if (s == "GetCompositeScheduleResponse") {
        return MessageType::GetCompositeScheduleResponse;
    }
    if (s == "GetConfiguration") {
        return MessageType::GetConfiguration;
    }
    if (s == "GetConfigurationResponse") {
        return MessageType::GetConfigurationResponse;
    }
    if (s == "GetDiagnostics") {
        return MessageType::GetDiagnostics;
    }
    if (s == "GetDiagnosticsResponse") {
        return MessageType::GetDiagnosticsResponse;
    }
    if (s == "GetInstalledCertificateIds") {
        return MessageType::GetInstalledCertificateIds;
    }
    if (s == "GetInstalledCertificateIdsResponse") {
        return MessageType::GetInstalledCertificateIdsResponse;
    }
    if (s == "GetLocalListVersion") {
        return MessageType::GetLocalListVersion;
    }
    if (s == "GetLocalListVersionResponse") {
        return MessageType::GetLocalListVersionResponse;
    }
    if (s == "GetLog") {
        return MessageType::GetLog;
    }
    if (s == "GetLogResponse") {
        return MessageType::GetLogResponse;
    }
    if (s == "Heartbeat") {
        return MessageType::Heartbeat;
    }
    if (s == "HeartbeatResponse") {
        return MessageType::HeartbeatResponse;
    }
    if (s == "InstallCertificate") {
        return MessageType::InstallCertificate;
    }
    if (s == "InstallCertificateResponse") {
        return MessageType::InstallCertificateResponse;
    }
    if (s == "LogStatusNotification") {
        return MessageType::LogStatusNotification;
    }
    if (s == "LogStatusNotificationResponse") {
        return MessageType::LogStatusNotificationResponse;
    }
    if (s == "MeterValues") {
        return MessageType::MeterValues;
    }
    if (s == "MeterValuesResponse") {
        return MessageType::MeterValuesResponse;
    }
    if (s == "RemoteStartTransaction") {
        return MessageType::RemoteStartTransaction;
    }
    if (s == "RemoteStartTransactionResponse") {
        return MessageType::RemoteStartTransactionResponse;
    }
    if (s == "RemoteStopTransaction") {
        return MessageType::RemoteStopTransaction;
    }
    if (s == "RemoteStopTransactionResponse") {
        return MessageType::RemoteStopTransactionResponse;
    }
    if (s == "ReserveNow") {
        return MessageType::ReserveNow;
    }
    if (s == "ReserveNowResponse") {
        return MessageType::ReserveNowResponse;
    }
    if (s == "Reset") {
        return MessageType::Reset;
    }
    if (s == "ResetResponse") {
        return MessageType::ResetResponse;
    }
    if (s == "SecurityEventNotification") {
        return MessageType::SecurityEventNotification;
    }
    if (s == "SecurityEventNotificationResponse") {
        return MessageType::SecurityEventNotificationResponse;
    }
    if (s == "SendLocalList") {
        return MessageType::SendLocalList;
    }
    if (s == "SendLocalListResponse") {
        return MessageType::SendLocalListResponse;
    }
    if (s == "SetChargingProfile") {
        return MessageType::SetChargingProfile;
    }
    if (s == "SetChargingProfileResponse") {
        return MessageType::SetChargingProfileResponse;
    }
    if (s == "SignCertificate") {
        return MessageType::SignCertificate;
    }
    if (s == "SignCertificateResponse") {
        return MessageType::SignCertificateResponse;
    }
    if (s == "SignedFirmwareStatusNotification") {
        return MessageType::SignedFirmwareStatusNotification;
    }
    if (s == "SignedFirmwareStatusNotificationResponse") {
        return MessageType::SignedFirmwareStatusNotificationResponse;
    }
    if (s == "SignedUpdateFirmware") {
        return MessageType::SignedUpdateFirmware;
    }
    if (s == "SignedUpdateFirmwareResponse") {
        return MessageType::SignedUpdateFirmwareResponse;
    }
    if (s == "StartTransaction") {
        return MessageType::StartTransaction;
    }
    if (s == "StartTransactionResponse") {
        return MessageType::StartTransactionResponse;
    }
    if (s == "StatusNotification") {
        return MessageType::StatusNotification;
    }
    if (s == "StatusNotificationResponse") {
        return MessageType::StatusNotificationResponse;
    }
    if (s == "StopTransaction") {
        return MessageType::StopTransaction;
    }
    if (s == "StopTransactionResponse") {
        return MessageType::StopTransactionResponse;
    }
    if (s == "TriggerMessage") {
        return MessageType::TriggerMessage;
    }
    if (s == "TriggerMessageResponse") {
        return MessageType::TriggerMessageResponse;
    }
    if (s == "UnlockConnector") {
        return MessageType::UnlockConnector;
    }
    if (s == "UnlockConnectorResponse") {
        return MessageType::UnlockConnectorResponse;
    }
    if (s == "UpdateFirmware") {
        return MessageType::UpdateFirmware;
    }
    if (s == "UpdateFirmwareResponse") {
        return MessageType::UpdateFirmwareResponse;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type MessageType");
}

} // namespace conversions

std::ostream& operator<<(std::ostream& os, const MessageType& message_type) {
    os << conversions::messagetype_to_string(message_type);
    return os;
}

namespace conversions {
/// \brief Converts the given SupportedFeatureProfiles \p e to std::string
/// \returns a string representation of the SupportedFeatureProfiles
std::string supported_feature_profiles_to_string(SupportedFeatureProfiles e) {
    switch (e) {
    case SupportedFeatureProfiles::Internal:
        return "Internal";
    case SupportedFeatureProfiles::Core:
        return "Core";
    case SupportedFeatureProfiles::FirmwareManagement:
        return "FirmwareManagement";
    case SupportedFeatureProfiles::LocalAuthListManagement:
        return "LocalAuthListManagement";
    case SupportedFeatureProfiles::Reservation:
        return "Reservation";
    case SupportedFeatureProfiles::SmartCharging:
        return "SmartCharging";
    case SupportedFeatureProfiles::RemoteTrigger:
        return "RemoteTrigger";
    case SupportedFeatureProfiles::Security:
        return "Security";
    case SupportedFeatureProfiles::PnC:
        return "PnC";
    }

    throw std::out_of_range("No known string conversion for provided enum of type SupportedFeatureProfiles");
}

/// \brief Converts the given std::string \p s to SupportedFeatureProfiles
/// \returns a SupportedFeatureProfiles from a string representation
SupportedFeatureProfiles string_to_supported_feature_profiles(const std::string& s) {
    if (s == "Internal") {
        return SupportedFeatureProfiles::Internal;
    }
    if (s == "Core") {
        return SupportedFeatureProfiles::Core;
    }
    if (s == "FirmwareManagement") {
        return SupportedFeatureProfiles::FirmwareManagement;
    }
    if (s == "LocalAuthListManagement") {
        return SupportedFeatureProfiles::LocalAuthListManagement;
    }
    if (s == "Reservation") {
        return SupportedFeatureProfiles::Reservation;
    }
    if (s == "SmartCharging") {
        return SupportedFeatureProfiles::SmartCharging;
    }
    if (s == "RemoteTrigger") {
        return SupportedFeatureProfiles::RemoteTrigger;
    }
    if (s == "Security") {
        return SupportedFeatureProfiles::Security;
    }
    if (s == "PnC") {
        return SupportedFeatureProfiles::PnC;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type SupportedFeatureProfiles");
}
} // namespace conversions

/// \brief Writes the string representation of the given \p supported_feature_profiles to the given output stream \p os
/// \returns an output stream with the SupportedFeatureProfiles written to
std::ostream& operator<<(std::ostream& os, const SupportedFeatureProfiles& supported_feature_profiles) {
    os << conversions::supported_feature_profiles_to_string(supported_feature_profiles);
    return os;
}

namespace conversions {

std::string charge_point_connection_state_to_string(ChargePointConnectionState e) {
    switch (e) {
    case ChargePointConnectionState::Disconnected:
        return "Disconnected";
    case ChargePointConnectionState::Connected:
        return "Connected";
    case ChargePointConnectionState::Booted:
        return "Booted";
    case ChargePointConnectionState::Pending:
        return "Pending";
    case ChargePointConnectionState::Rejected:
        return "Rejected";
    }

    throw std::out_of_range("No known string conversion for provided enum of type ChargePointConnectionState");
}

ChargePointConnectionState string_to_charge_point_connection_state(const std::string& s) {
    if (s == "Disconnected") {
        return ChargePointConnectionState::Disconnected;
    }
    if (s == "Connected") {
        return ChargePointConnectionState::Connected;
    }
    if (s == "Booted") {
        return ChargePointConnectionState::Booted;
    }
    if (s == "Pending") {
        return ChargePointConnectionState::Pending;
    }
    if (s == "Rejected") {
        return ChargePointConnectionState::Rejected;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type ChargePointConnectionState");
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ChargePointConnectionState& charge_point_connection_state) {
    os << conversions::charge_point_connection_state_to_string(charge_point_connection_state);
    return os;
}

bool MeasurandWithPhase::operator==(MeasurandWithPhase measurand_with_phase) {
    if (this->measurand == measurand_with_phase.measurand) {
        if (this->phase || measurand_with_phase.phase) {
            if (this->phase && measurand_with_phase.phase) {
                if (this->phase.value() == measurand_with_phase.phase.value()) {
                    return true;
                }
            }
        } else {
            return true;
        }
    }
    return false;
}

} // namespace v16
} // namespace ocpp
