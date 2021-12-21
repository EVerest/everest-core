// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_TYPES_HPP
#define OCPP1_6_TYPES_HPP

#include <future>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <everest/logging.hpp>
#include <everest/timer.hpp>
#include <nlohmann/json.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/types_internal.hpp>

using json = nlohmann::json;

namespace ocpp1_6 {

// The locations inside the message array
const auto MESSAGE_TYPE_ID = 0;
const auto MESSAGE_ID = 1;
const auto CALL_ACTION = 2;
const auto CALL_PAYLOAD = 3;
const auto CALLRESULT_PAYLOAD = 2;
const auto CALLERROR_ERROR_CODE = 2;
const auto CALLERROR_ERROR_DESCRIPTION = 3;
const auto CALLERROR_ERROR_DETAILS = 4;

// The different message type ids
enum class MessageTypeId
{
    CALL = 2,
    CALLRESULT = 3,
    CALLERROR = 4,
    UNKNOWN,
};

const auto future_wait_seconds = std::chrono::seconds(5);
const auto default_heartbeat_interval = 0;

struct Message {
    virtual std::string get_type() const = 0;
};

enum class MessageType
{
    Authorize,
    AuthorizeResponse,
    BootNotification,
    BootNotificationResponse,
    CancelReservation,
    CancelReservationResponse,
    ChangeAvailability,
    ChangeAvailabilityResponse,
    ChangeConfiguration,
    ChangeConfigurationResponse,
    ClearCache,
    ClearCacheResponse,
    ClearChargingProfile,
    ClearChargingProfileResponse,
    DataTransfer,
    DataTransferResponse,
    DiagnosticsStatusNotification,
    DiagnosticsStatusNotificationResponse,
    FirmwareStatusNotification,
    FirmwareStatusNotificationResponse,
    GetCompositeSchedule,
    GetCompositeScheduleResponse,
    GetConfiguration,
    GetConfigurationResponse,
    GetDiagnostics,
    GetDiagnosticsResponse,
    GetLocalListVersion,
    GetLocalListVersionResponse,
    Heartbeat,
    HeartbeatResponse,
    MeterValues,
    MeterValuesResponse,
    RemoteStartTransaction,
    RemoteStartTransactionResponse,
    RemoteStopTransaction,
    RemoteStopTransactionResponse,
    ReserveNow,
    ReserveNowResponse,
    Reset,
    ResetResponse,
    SendLocalList,
    SendLocalListResponse,
    SetChargingProfile,
    SetChargingProfileResponse,
    StartTransaction,
    StartTransactionResponse,
    StatusNotification,
    StatusNotificationResponse,
    StopTransaction,
    StopTransactionResponse,
    TriggerMessage,
    TriggerMessageResponse,
    UnlockConnector,
    UnlockConnectorResponse,
    UpdateFirmware,
    UpdateFirmwareResponse,
    InternalError, // not in spec, for internal use
};

class Conversions {
public:
    static const std::string messagetype_to_string(MessageType m) {
        const std::map<MessageType, std::string> MessageTypeStrings{
            {MessageType::Authorize, "Authorize"},
            {MessageType::AuthorizeResponse, "AuthorizeResponse"},
            {MessageType::BootNotification, "BootNotification"},
            {MessageType::BootNotificationResponse, "BootNotificationResponse"},
            {MessageType::CancelReservation, "CancelReservation"},
            {MessageType::CancelReservationResponse, "CancelReservationResponse"},
            {MessageType::ChangeAvailability, "ChangeAvailability"},
            {MessageType::ChangeAvailabilityResponse, "ChangeAvailabilityResponse"},
            {MessageType::ChangeConfiguration, "ChangeConfiguration"},
            {MessageType::ChangeConfigurationResponse, "ChangeConfigurationResponse"},
            {MessageType::ClearCache, "ClearCache"},
            {MessageType::ClearCacheResponse, "ClearCacheResponse"},
            {MessageType::ClearChargingProfile, "ClearChargingProfile"},
            {MessageType::ClearChargingProfileResponse, "ClearChargingProfileResponse"},
            {MessageType::DataTransfer, "DataTransfer"},
            {MessageType::DataTransferResponse, "DataTransferResponse"},
            {MessageType::DiagnosticsStatusNotification, "DiagnosticsStatusNotification"},
            {MessageType::DiagnosticsStatusNotificationResponse, "DiagnosticsStatusNotificationResponse"},
            {MessageType::FirmwareStatusNotification, "FirmwareStatusNotification"},
            {MessageType::FirmwareStatusNotificationResponse, "FirmwareStatusNotificationResponse"},
            {MessageType::GetCompositeSchedule, "GetCompositeSchedule"},
            {MessageType::GetCompositeScheduleResponse, "GetCompositeScheduleResponse"},
            {MessageType::GetConfiguration, "GetConfiguration"},
            {MessageType::GetConfigurationResponse, "GetConfigurationResponse"},
            {MessageType::GetDiagnostics, "GetDiagnostics"},
            {MessageType::GetDiagnosticsResponse, "GetDiagnosticsResponse"},
            {MessageType::GetLocalListVersion, "GetLocalListVersion"},
            {MessageType::GetLocalListVersionResponse, "GetLocalListVersionResponse"},
            {MessageType::Heartbeat, "Heartbeat"},
            {MessageType::HeartbeatResponse, "HeartbeatResponse"},
            {MessageType::MeterValues, "MeterValues"},
            {MessageType::MeterValuesResponse, "MeterValuesResponse"},
            {MessageType::RemoteStartTransaction, "RemoteStartTransaction"},
            {MessageType::RemoteStartTransactionResponse, "RemoteStartTransactionResponse"},
            {MessageType::RemoteStopTransaction, "RemoteStopTransaction"},
            {MessageType::RemoteStopTransactionResponse, "RemoteStopTransactionResponse"},
            {MessageType::ReserveNow, "ReserveNow"},
            {MessageType::ReserveNowResponse, "ReserveNowResponse"},
            {MessageType::Reset, "Reset"},
            {MessageType::ResetResponse, "ResetResponse"},
            {MessageType::SendLocalList, "SendLocalList"},
            {MessageType::SendLocalListResponse, "SendLocalListResponse"},
            {MessageType::SetChargingProfile, "SetChargingProfile"},
            {MessageType::SetChargingProfileResponse, "SetChargingProfileResponse"},
            {MessageType::StartTransaction, "StartTransaction"},
            {MessageType::StartTransactionResponse, "StartTransactionResponse"},
            {MessageType::StatusNotification, "StatusNotification"},
            {MessageType::StatusNotificationResponse, "StatusNotificationResponse"},
            {MessageType::StopTransaction, "StopTransaction"},
            {MessageType::StopTransactionResponse, "StopTransactionResponse"},
            {MessageType::TriggerMessage, "TriggerMessage"},
            {MessageType::TriggerMessageResponse, "TriggerMessageResponse"},
            {MessageType::UnlockConnector, "UnlockConnector"},
            {MessageType::UnlockConnectorResponse, "UnlockConnectorResponse"},
            {MessageType::UpdateFirmware, "UpdateFirmware"},
            {MessageType::UpdateFirmwareResponse, "UpdateFirmwareResponse"},
        };
        auto it = MessageTypeStrings.find(m);
        if (it == MessageTypeStrings.end()) {
            return "InternalError";
        }
        return it->second;
    }
    static const MessageType string_to_messagetype(std::string s) {
        const std::map<std::string, MessageType> MessageTypeStrings{
            {"Authorize", MessageType::Authorize},
            {"AuthorizeResponse", MessageType::AuthorizeResponse},
            {"BootNotification", MessageType::BootNotification},
            {"BootNotificationResponse", MessageType::BootNotificationResponse},
            {"CancelReservation", MessageType::CancelReservation},
            {"CancelReservationResponse", MessageType::CancelReservationResponse},
            {"ChangeAvailability", MessageType::ChangeAvailability},
            {"ChangeAvailabilityResponse", MessageType::ChangeAvailabilityResponse},
            {"ChangeConfiguration", MessageType::ChangeConfiguration},
            {"ChangeConfigurationResponse", MessageType::ChangeConfigurationResponse},
            {"ClearCache", MessageType::ClearCache},
            {"ClearCacheResponse", MessageType::ClearCacheResponse},
            {"ClearChargingProfile", MessageType::ClearChargingProfile},
            {"ClearChargingProfileResponse", MessageType::ClearChargingProfileResponse},
            {"DataTransfer", MessageType::DataTransfer},
            {"DataTransferResponse", MessageType::DataTransferResponse},
            {"DiagnosticsStatusNotification", MessageType::DiagnosticsStatusNotification},
            {"DiagnosticsStatusNotificationResponse", MessageType::DiagnosticsStatusNotificationResponse},
            {"FirmwareStatusNotification", MessageType::FirmwareStatusNotification},
            {"FirmwareStatusNotificationResponse", MessageType::FirmwareStatusNotificationResponse},
            {"GetCompositeSchedule", MessageType::GetCompositeSchedule},
            {"GetCompositeScheduleResponse", MessageType::GetCompositeScheduleResponse},
            {"GetConfiguration", MessageType::GetConfiguration},
            {"GetConfigurationResponse", MessageType::GetConfigurationResponse},
            {"GetDiagnostics", MessageType::GetDiagnostics},
            {"GetDiagnosticsResponse", MessageType::GetDiagnosticsResponse},
            {"GetLocalListVersion", MessageType::GetLocalListVersion},
            {"GetLocalListVersionResponse", MessageType::GetLocalListVersionResponse},
            {"Heartbeat", MessageType::Heartbeat},
            {"HeartbeatResponse", MessageType::HeartbeatResponse},
            {"MeterValues", MessageType::MeterValues},
            {"MeterValuesResponse", MessageType::MeterValuesResponse},
            {"RemoteStartTransaction", MessageType::RemoteStartTransaction},
            {"RemoteStartTransactionResponse", MessageType::RemoteStartTransactionResponse},
            {"RemoteStopTransaction", MessageType::RemoteStopTransaction},
            {"RemoteStopTransactionResponse", MessageType::RemoteStopTransactionResponse},
            {"ReserveNow", MessageType::ReserveNow},
            {"ReserveNowResponse", MessageType::ReserveNowResponse},
            {"Reset", MessageType::Reset},
            {"ResetResponse", MessageType::ResetResponse},
            {"SendLocalList", MessageType::SendLocalList},
            {"SendLocalListResponse", MessageType::SendLocalListResponse},
            {"SetChargingProfile", MessageType::SetChargingProfile},
            {"SetChargingProfileResponse", MessageType::SetChargingProfileResponse},
            {"StartTransaction", MessageType::StartTransaction},
            {"StartTransactionResponse", MessageType::StartTransactionResponse},
            {"StatusNotification", MessageType::StatusNotification},
            {"StatusNotificationResponse", MessageType::StatusNotificationResponse},
            {"StopTransaction", MessageType::StopTransaction},
            {"StopTransactionResponse", MessageType::StopTransactionResponse},
            {"TriggerMessage", MessageType::TriggerMessage},
            {"TriggerMessageResponse", MessageType::TriggerMessageResponse},
            {"UnlockConnector", MessageType::UnlockConnector},
            {"UnlockConnectorResponse", MessageType::UnlockConnectorResponse},
            {"UpdateFirmware", MessageType::UpdateFirmware},
            {"UpdateFirmwareResponse", MessageType::UpdateFirmwareResponse},
        };
        auto it = MessageTypeStrings.find(s);
        if (it == MessageTypeStrings.end()) {
            return MessageType::InternalError;
        }
        return it->second;
    }

    static const std::string bool_to_string(bool b) {
        if (b) {
            return "true";
        }
        return "false";
    }
    static const bool string_to_bool(const std::string& s) {
        if (s == "true") {
            return true;
        }
        return false;
    }

    static const std::string double_to_string(double d) {
        return double_to_string(d, 2);
    }

    static const std::string double_to_string(double d, int precision) {
        std::ostringstream str;
        str.precision(precision);
        str << std::fixed << d;
        return str.str();
    }
};

inline std::ostream& operator<<(std::ostream& os, const MessageType& message_type) {
    os << Conversions::messagetype_to_string(message_type);
    return os;
}

class CiString20Type : public CiString {
public:
    CiString20Type(const std::string& data) : CiString(data, 20) {
    }
    CiString20Type(const json& data) : CiString(data.get<std::string>(), 20) {
    }
    CiString20Type() : CiString(20) {
    }
    CiString20Type& operator=(const std::string& s) {
        this->set(s);
        return *this;
    }
    CiString20Type& operator=(const char* c) {
        this->set(std::string(c));
        return *this;
    }
    CiString20Type& operator=(const json& j) {
        this->set(j.get<std::string>());
        return *this;
    }
    friend void to_json(json& j, const CiString20Type& k) {
        j = json(k.get());
    }
    friend void from_json(const json& j, CiString20Type& k) {
        k.set(j);
    }
};

class CiString25Type : public CiString {
public:
    CiString25Type(const std::string& data) : CiString(data, 25) {
    }
    CiString25Type(const json& data) : CiString(data.get<std::string>(), 25) {
    }
    CiString25Type() : CiString(25) {
    }
    CiString25Type& operator=(const std::string& s) {
        this->set(s);
        return *this;
    }
    CiString25Type& operator=(const char* c) {
        this->set(std::string(c));
        return *this;
    }
    CiString25Type& operator=(const json& j) {
        this->set(j.get<std::string>());
        return *this;
    }
    friend void to_json(json& j, const CiString25Type& k) {
        j = json(k.get());
    }
    friend void from_json(const json& j, CiString25Type& k) {
        k.set(j);
    }
};

class CiString50Type : public CiString {
public:
    CiString50Type(const std::string& data) : CiString(data, 50) {
    }
    CiString50Type(const json& data) : CiString(data.get<std::string>(), 50) {
    }
    CiString50Type() : CiString(50) {
    }
    CiString50Type& operator=(const std::string& s) {
        this->set(s);
        return *this;
    }
    CiString50Type& operator=(const char* c) {
        this->set(std::string(c));
        return *this;
    }
    CiString50Type& operator=(const json& j) {
        this->set(j.get<std::string>());
        return *this;
    }
    friend void to_json(json& j, const CiString50Type& k) {
        j = json(k.get());
    }
    friend void from_json(const json& j, CiString50Type& k) {
        k.set(j);
    }
};

class CiString255Type : public CiString {
public:
    CiString255Type(const std::string& data) : CiString(data, 255) {
    }
    CiString255Type(const json& data) : CiString(data.get<std::string>(), 255) {
    }
    CiString255Type() : CiString(255) {
    }
    CiString255Type& operator=(const std::string& s) {
        this->set(s);
        return *this;
    }
    CiString255Type& operator=(const char* c) {
        this->set(std::string(c));
        return *this;
    }
    CiString255Type& operator=(const json& j) {
        this->set(j.get<std::string>());
        return *this;
    }
    friend void to_json(json& j, const CiString255Type& k) {
        j = json(k.get());
    }
    friend void from_json(const json& j, CiString255Type& k) {
        k.set(j);
    }
};

class CiString500Type : public CiString {
public:
    CiString500Type(const std::string& data) : CiString(data, 500) {
    }
    CiString500Type(const json& data) : CiString(data.get<std::string>(), 500) {
    }
    CiString500Type() : CiString(500) {
    }
    CiString500Type& operator=(const std::string& s) {
        this->set(s);
        return *this;
    }
    CiString500Type& operator=(const char* c) {
        this->set(std::string(c));
        return *this;
    }
    CiString500Type& operator=(const json& j) {
        this->set(j.get<std::string>());
        return *this;
    }
    friend void to_json(json& j, const CiString500Type& k) {
        j = json(k.get());
    }
    friend void from_json(const json& j, CiString500Type& k) {
        k.set(j);
    }
};

class MessageId : public CiString {
public:
    MessageId(const std::string& data) : CiString(data, 36) {
    }
    MessageId() : CiString(36) {
    }
    MessageId& operator=(const std::string& s) {
        this->set(s);
        return *this;
    }
    MessageId& operator=(const char* c) {
        this->set(std::string(c));
        return *this;
    }
    MessageId& operator=(const json& j) {
        this->set(j.get<std::string>());
        return *this;
    }
    bool operator<(const MessageId& rhs) {
        return this->get() < rhs.get();
    }
    friend bool operator<(const MessageId& lhs, const MessageId& rhs) {
        return lhs.get() < rhs.get();
    }
    friend void to_json(json& j, const MessageId& k) {
        j = json(k.get());
    }
    friend void from_json(const json& j, MessageId& k) {
        k.set(j);
    }
};

class DateTime : public DateTimeImpl {
public:
    DateTime() : DateTimeImpl() {
    }
    DateTime(std::chrono::time_point<std::chrono::system_clock> timepoint) : DateTimeImpl(timepoint) {
    }
    DateTime(const std::string& timepoint_str) : DateTimeImpl(timepoint_str) {
    }
};

struct MeasurandWithPhase {
    ocpp1_6::Measurand measurand;
    boost::optional<ocpp1_6::Phase> phase;

    bool operator==(MeasurandWithPhase measurand_with_phase) {
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
};

struct MeasurandWithPhases {
    ocpp1_6::Measurand measurand;
    std::vector<ocpp1_6::Phase> phases;
};

template <class T> struct Call {
    T msg;
    MessageId uniqueId;

    Call() {
    }

    Call(T msg, MessageId uniqueId) {
        this->msg = msg;
        this->uniqueId = uniqueId;
    }

    friend void to_json(json& j, const Call& c) {
        j = json::array();
        j.push_back(MessageTypeId::CALL);
        j.push_back(c.uniqueId.get());
        j.push_back(c.msg.get_type());
        j.push_back(json(c.msg));
    }

    friend void from_json(const json& j, Call& c) {
        // the required parts of the message
        c.msg = j.at(CALL_PAYLOAD);
        c.uniqueId.set(j.at(MESSAGE_ID));
    }

    friend std::ostream& operator<<(std::ostream& os, const Call& c) {
        os << json(c).dump(4);
        return os;
    }
};

template <class T> struct CallResult {
    T msg;
    MessageId uniqueId;

    CallResult() {
    }

    CallResult(T msg, MessageId uniqueId) {
        this->msg = msg;
        this->uniqueId = uniqueId;
    }

    friend void to_json(json& j, const CallResult& c) {
        j = json::array();
        j.push_back(MessageTypeId::CALLRESULT);
        j.push_back(c.uniqueId.get());
        j.push_back(json(c.msg));
    }

    friend void from_json(const json& j, CallResult& c) {
        // the required parts of the message
        c.msg = j.at(CALLRESULT_PAYLOAD);
        c.uniqueId.set(j.at(MESSAGE_ID));
    }

    friend std::ostream& operator<<(std::ostream& os, const CallResult& c) {
        os << json(c).dump(4);
        return os;
    }
};

struct CallError {
    MessageId uniqueId;
    std::string errorCode;
    std::string errorDescription;
    json errorDetails;

    CallError() {
    }

    CallError(const MessageId& uniqueId, const std::string& errorCode, const std::string& errorDescription,
              const json& errorDetails) {
        this->uniqueId = uniqueId;
        this->errorCode = errorCode;
        this->errorDescription = errorDescription;
        this->errorDetails = errorDetails;
    }

    friend void to_json(json& j, const CallError& c) {
        j = json::array();
        j.push_back(MessageTypeId::CALLERROR);
        j.push_back(c.uniqueId.get());
        j.push_back(c.errorCode);
        j.push_back(c.errorDescription);
        j.push_back(c.errorDetails);
    }

    friend void from_json(const json& j, CallError& c) {
        // the required parts of the message
        c.uniqueId.set(j.at(MESSAGE_ID));
        c.errorCode = j.at(CALLERROR_ERROR_CODE);
        c.errorDescription = j.at(CALLERROR_ERROR_DESCRIPTION);
        c.errorDetails = j.at(CALLERROR_ERROR_DETAILS);
    }

    friend std::ostream& operator<<(std::ostream& os, const CallError& c) {
        os << json(c).dump(4);
        return os;
    }
};

enum ChargePointConnectionState
{
    Disconnected,
    Connected,
    Booted,
    Pending,
    Rejected,
};
namespace conversions {
static const std::map<ChargePointConnectionState, std::string> ChargePointConnectionStateStrings{
    {ChargePointConnectionState::Disconnected, "Disconnected"},
    {ChargePointConnectionState::Connected, "Connected"},
    {ChargePointConnectionState::Booted, "Booted"},
    {ChargePointConnectionState::Pending, "Pending"},
    {ChargePointConnectionState::Rejected, "Rejected"},
};
static const std::map<std::string, ChargePointConnectionState> ChargePointConnectionStateValues{
    {"Disconnected", ChargePointConnectionState::Disconnected},
    {"Connected", ChargePointConnectionState::Connected},
    {"Booted", ChargePointConnectionState::Booted},
    {"Pending", ChargePointConnectionState::Pending},
    {"Rejected", ChargePointConnectionState::Rejected},
};
static const std::string charge_point_connection_state_to_string(ChargePointConnectionState e) {
    return ChargePointConnectionStateStrings.at(e);
}
static const ChargePointConnectionState string_to_charge_point_connection_state(std::string s) {
    return ChargePointConnectionStateValues.at(s);
}
} // namespace conversions
inline std::ostream& operator<<(std::ostream& os, const ChargePointConnectionState& charge_point_connection_state) {
    os << conversions::charge_point_connection_state_to_string(charge_point_connection_state);
    return os;
}

enum SupportedFeatureProfiles
{
    Core,
    FirmwareManagement,
    LocalAuthListManagement,
    Reservation,
    SmartCharging,
    RemoteTrigger,
};
namespace conversions {
static const std::map<SupportedFeatureProfiles, std::string> SupportedFeatureProfilesStrings{
    {SupportedFeatureProfiles::Core, "Core"},
    {SupportedFeatureProfiles::FirmwareManagement, "FirmwareManagement"},
    {SupportedFeatureProfiles::LocalAuthListManagement, "LocalAuthListManagement"},
    {SupportedFeatureProfiles::Reservation, "Reservation"},
    {SupportedFeatureProfiles::SmartCharging, "SmartCharging"},
    {SupportedFeatureProfiles::RemoteTrigger, "RemoteTrigger"},
};
static const std::map<std::string, SupportedFeatureProfiles> SupportedFeatureProfilesValues{
    {"Core", SupportedFeatureProfiles::Core},
    {"FirmwareManagement", SupportedFeatureProfiles::FirmwareManagement},
    {"LocalAuthListManagement", SupportedFeatureProfiles::LocalAuthListManagement},
    {"Reservation", SupportedFeatureProfiles::Reservation},
    {"SmartCharging", SupportedFeatureProfiles::SmartCharging},
    {"RemoteTrigger", SupportedFeatureProfiles::RemoteTrigger},
};
static const std::string supported_feature_profiles_to_string(SupportedFeatureProfiles e) {
    return SupportedFeatureProfilesStrings.at(e);
}
static const SupportedFeatureProfiles string_to_supported_feature_profiles(std::string s) {
    return SupportedFeatureProfilesValues.at(s);
}
} // namespace conversions
inline std::ostream& operator<<(std::ostream& os, const SupportedFeatureProfiles& supported_feature_profiles) {
    os << conversions::supported_feature_profiles_to_string(supported_feature_profiles);
    return os;
}

struct ChargePointConnection {
    std::string charge_point_id;
    ChargePointConnectionState state;
};

struct ChargePointRequest {
    std::string charge_point_id;
    std::string request;
};

enum class ChargePointStatusTransition
{
    A2_UsageInitiated,
    A3_UsageInitiatedWithoutAuthorization,
    A4_UsageInitiatedEVDoesNotStartCharging,
    A5_UsageInitiatedEVSEDoesNotAllowCharging,
    A7_ReserveNowReservesConnector,
    A8_ChangeAvailabilityToUnavailable,
    A9_FaultDetected,
    B1_IntendedUsageIsEnded,
    B3_PrerequisitesForChargingMetAndChargingStarts,
    B4_PrerequisitesForChargingMetEVDoesNotStartCharging,
    B5_PrerequisitesForChargingMetEVSEDoesNotAllowCharging,
    B6_TimedOut, // Usage was initiated but ID Tag was not presented within timeout
    B9_FaultDetected,
    C1_ChargingSessionEndsNoUserActionRequired, // e.g. fixed cable removed on EV side
    C4_ChargingStopsUponEVRequest,
    C5_ChargingStopsUponEVSERequest,
    C6_TransactionStoppedAndUserActionRequired, // e.g. remove cable leave parking bay
    C8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable,
    C9_FaultDetected,
    D1_ChargingSessionEndsNoUserActionRequired,
    D3_ChargingResumesUponEVRequest,
    D5_ChargingSuspendedByEVSE,
    D6_TransactionStoppedNoUserActionRequired,
    D8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable,
    D9_FaultDetected,
    E1_ChargingSessionEndsNoUserActionRequired,
    E3_ChargingResumesEVSERestrictionLifted,
    E4_EVSERestrictionLiftedEVDoesNotStartCharging,
    E6_TransactionStoppedAndUserActionRequired,
    E8_ChargingSessionEndsNoUserActionRequiredConnectorScheduledToBecomeUnavailable,
    E9_FaultDetected,
    F1_AllUserActionsCompleted,
    F2_UsersRestartChargingSession, // cable reconnect, ID Tag presented again -> creates new transaction
    F8_AllUserActionsCompletedConnectorScheduledToBecomeUnavailable,
    F9_FaultDetected,
    G1_ReservationExpiresOrCancelReservationReceived,
    G2_ReservationIdentityPresented,
    G8_ReservationExpiresOrCancelReservationReceivedConnectorScheduledToBecomeUnavailable,
    G9_FaultDetected,
    H1_ConnectorSetAvailableByChangeAvailability,
    H2_ConnectorSetAvailableAfterUserInteractedWithChargePoint,
    H3_ConnectorSetAvailableNoUserActionRequiredToStartCharging,
    H4_ConnectorSetAvailableNoUserActionRequiredEVDoesNotStartCharging,
    H5_ConnectorSetAvailableNoUserActionRequiredEVSEDoesNotAllowCharging,
    H9_FaultDetected,
    I1_ReturnToAvailable,
    I2_ReturnToPreparing,
    I3_ReturnToCharging,
    I4_ReturnToSuspendedEV,
    I5_ReturnToSuspendedEVSE,
    I6_ReturnToFinishing,
    I7_ReturnToReserved,
    I8_ReturnToUnavailable,
};

} // namespace ocpp1_6
#endif // OCPP1_6_TYPES_HPP
