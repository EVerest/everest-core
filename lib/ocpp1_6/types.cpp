// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <string>

#include <boost/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

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
    case MessageType::DiagnosticsStatusNotification:
        return "DiagnosticsStatusNotification";
    case MessageType::DiagnosticsStatusNotificationResponse:
        return "DiagnosticsStatusNotificationResponse";
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
    case MessageType::GetLocalListVersion:
        return "GetLocalListVersion";
    case MessageType::GetLocalListVersionResponse:
        return "GetLocalListVersionResponse";
    case MessageType::Heartbeat:
        return "Heartbeat";
    case MessageType::HeartbeatResponse:
        return "HeartbeatResponse";
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
    case MessageType::SendLocalList:
        return "SendLocalList";
    case MessageType::SendLocalListResponse:
        return "SendLocalListResponse";
    case MessageType::SetChargingProfile:
        return "SetChargingProfile";
    case MessageType::SetChargingProfileResponse:
        return "SetChargingProfileResponse";
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
        throw std::out_of_range("No known string conversion for InternalError MessageType");
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
    if (s == "DiagnosticsStatusNotification") {
        return MessageType::DiagnosticsStatusNotification;
    }
    if (s == "DiagnosticsStatusNotificationResponse") {
        return MessageType::DiagnosticsStatusNotificationResponse;
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
    if (s == "GetLocalListVersion") {
        return MessageType::GetLocalListVersion;
    }
    if (s == "GetLocalListVersionResponse") {
        return MessageType::GetLocalListVersionResponse;
    }
    if (s == "Heartbeat") {
        return MessageType::Heartbeat;
    }
    if (s == "HeartbeatResponse") {
        return MessageType::HeartbeatResponse;
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

std::string bool_to_string(bool b) {
    if (b) {
        return "true";
    }
    return "false";
}

bool string_to_bool(const std::string& s) {
    if (s == "true") {
        return true;
    }
    return false;
}

std::string double_to_string(double d, int precision) {
    std::ostringstream str;
    str.precision(precision);
    str << std::fixed << d;
    return str.str();
}

std::string double_to_string(double d) {
    return conversions::double_to_string(d, 2);
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const MessageType& message_type) {
    os << conversions::messagetype_to_string(message_type);
    return os;
}

CiString20Type::CiString20Type(const std::string& data) : CiString(data, 20) {
}

CiString20Type::CiString20Type(const json& data) : CiString(data.get<std::string>(), 20) {
}

CiString20Type::CiString20Type() : CiString(20) {
}

CiString20Type& CiString20Type::operator=(const std::string& s) {
    this->set(s);
    return *this;
}

CiString20Type& CiString20Type::operator=(const char* c) {
    this->set(std::string(c));
    return *this;
}

CiString20Type& CiString20Type::operator=(const json& j) {
    this->set(j.get<std::string>());
    return *this;
}

void to_json(json& j, const CiString20Type& k) {
    j = json(k.get());
}

void from_json(const json& j, CiString20Type& k) {
    k.set(j);
}

CiString25Type::CiString25Type(const std::string& data) : CiString(data, 25) {
}

CiString25Type::CiString25Type(const json& data) : CiString(data.get<std::string>(), 25) {
}

CiString25Type::CiString25Type() : CiString(25) {
}

CiString25Type& CiString25Type::operator=(const std::string& s) {
    this->set(s);
    return *this;
}

CiString25Type& CiString25Type::operator=(const char* c) {
    this->set(std::string(c));
    return *this;
}

CiString25Type& CiString25Type::operator=(const json& j) {
    this->set(j.get<std::string>());
    return *this;
}

void to_json(json& j, const CiString25Type& k) {
    j = json(k.get());
}

void from_json(const json& j, CiString25Type& k) {
    k.set(j);
}

CiString50Type::CiString50Type(const std::string& data) : CiString(data, 50) {
}

CiString50Type::CiString50Type(const json& data) : CiString(data.get<std::string>(), 50) {
}

CiString50Type::CiString50Type() : CiString(50) {
}

CiString50Type& CiString50Type::operator=(const std::string& s) {
    this->set(s);
    return *this;
}

CiString50Type& CiString50Type::operator=(const char* c) {
    this->set(std::string(c));
    return *this;
}

CiString50Type& CiString50Type::operator=(const json& j) {
    this->set(j.get<std::string>());
    return *this;
}

void to_json(json& j, const CiString50Type& k) {
    j = json(k.get());
}

void from_json(const json& j, CiString50Type& k) {
    k.set(j);
}

CiString255Type::CiString255Type(const std::string& data) : CiString(data, 255) {
}

CiString255Type::CiString255Type(const json& data) : CiString(data.get<std::string>(), 255) {
}

CiString255Type::CiString255Type() : CiString(255) {
}

CiString255Type& CiString255Type::operator=(const std::string& s) {
    this->set(s);
    return *this;
}

CiString255Type& CiString255Type::operator=(const char* c) {
    this->set(std::string(c));
    return *this;
}

CiString255Type& CiString255Type::operator=(const json& j) {
    this->set(j.get<std::string>());
    return *this;
}

void to_json(json& j, const CiString255Type& k) {
    j = json(k.get());
}

void from_json(const json& j, CiString255Type& k) {
    k.set(j);
}

CiString500Type::CiString500Type(const std::string& data) : CiString(data, 500) {
}

CiString500Type::CiString500Type(const json& data) : CiString(data.get<std::string>(), 500) {
}

CiString500Type::CiString500Type() : CiString(500) {
}

CiString500Type& CiString500Type::operator=(const std::string& s) {
    this->set(s);
    return *this;
}

CiString500Type& CiString500Type::operator=(const char* c) {
    this->set(std::string(c));
    return *this;
}

CiString500Type& CiString500Type::operator=(const json& j) {
    this->set(j.get<std::string>());
    return *this;
}

void to_json(json& j, const CiString500Type& k) {
    j = json(k.get());
}

void from_json(const json& j, CiString500Type& k) {
    k.set(j);
}

MessageId::MessageId(const std::string& data) : CiString(data, 36) {
}

MessageId::MessageId() : CiString(36) {
}

MessageId& MessageId::operator=(const std::string& s) {
    this->set(s);
    return *this;
}

MessageId& MessageId::operator=(const char* c) {
    this->set(std::string(c));
    return *this;
}

MessageId& MessageId::operator=(const json& j) {
    this->set(j.get<std::string>());
    return *this;
}

bool MessageId::operator<(const MessageId& rhs) {
    return this->get() < rhs.get();
}

bool operator<(const MessageId& lhs, const MessageId& rhs) {
    return lhs.get() < rhs.get();
}

void to_json(json& j, const MessageId& k) {
    j = json(k.get());
}

void from_json(const json& j, MessageId& k) {
    k.set(j);
}

DateTime::DateTime() : DateTimeImpl() {
}

DateTime::DateTime(std::chrono::time_point<std::chrono::system_clock> timepoint) : DateTimeImpl(timepoint) {
}

DateTime::DateTime(const std::string& timepoint_str) : DateTimeImpl(timepoint_str) {
}

DateTime& DateTime::operator=(const std::string& s) {
    this->from_rfc3339(s);
    return *this;
}

DateTime& DateTime::operator=(const char* c) {
    this->from_rfc3339(std::string(c));
    return *this;
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
};

CallError::CallError() {
}

CallError::CallError(const MessageId& uniqueId, const std::string& errorCode, const std::string& errorDescription,
                     const json& errorDetails) {
    this->uniqueId = uniqueId;
    this->errorCode = errorCode;
    this->errorDescription = errorDescription;
    this->errorDetails = errorDetails;
}

void to_json(json& j, const CallError& c) {
    j = json::array();
    j.push_back(MessageTypeId::CALLERROR);
    j.push_back(c.uniqueId.get());
    j.push_back(c.errorCode);
    j.push_back(c.errorDescription);
    j.push_back(c.errorDetails);
}

void from_json(const json& j, CallError& c) {
    // the required parts of the message
    c.uniqueId.set(j.at(MESSAGE_ID));
    c.errorCode = j.at(CALLERROR_ERROR_CODE);
    c.errorDescription = j.at(CALLERROR_ERROR_DESCRIPTION);
    c.errorDetails = j.at(CALLERROR_ERROR_DETAILS);
}

std::ostream& operator<<(std::ostream& os, const CallError& c) {
    os << json(c).dump(4);
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

namespace conversions {
/// \brief Converts the given SupportedFeatureProfiles \p e to std::string
/// \returns a string representation of the SupportedFeatureProfiles
std::string supported_feature_profiles_to_string(SupportedFeatureProfiles e) {
    switch (e) {
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
    }

    throw std::out_of_range("No known string conversion for provided enum of type SupportedFeatureProfiles");
}

/// \brief Converts the given std::string \p s to SupportedFeatureProfiles
/// \returns a SupportedFeatureProfiles from a string representation
SupportedFeatureProfiles string_to_supported_feature_profiles(const std::string& s) {
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

} // namespace ocpp1_6
