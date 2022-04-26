// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_TYPES_HPP
#define OCPP1_6_TYPES_HPP

#include <sstream>

#include <boost/optional.hpp>
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

/// \brief Contains the different message type ids
enum class MessageTypeId
{
    CALL = 2,
    CALLRESULT = 3,
    CALLERROR = 4,
    UNKNOWN,
};

const auto future_wait_seconds = std::chrono::seconds(5);
const auto default_heartbeat_interval = 0;

/// \brief Parent structure for all OCPP 1.6 messages supported by this implementation
struct Message {
    /// \brief Provides the type of the message
    /// \returns the message type as a string
    virtual std::string get_type() const = 0;
};

/// \brief Contains all supported OCPP 1.6 message types
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

namespace conversions {
/// \brief Converts the given MessageType \p m to std::string
/// \returns a string representation of the MessageType
std::string messagetype_to_string(MessageType m);

/// \brief Converts the given std::string \p s to MessageType
/// \returns a MessageType from a string representation
MessageType string_to_messagetype(const std::string& s);

/// \brief Converts the given bool \p b to a string representation of "true" or "false"
/// \returns a string representation of the given bool
std::string bool_to_string(bool b);

/// \brief Converts the given string \p s to a bool value. "true" is converted into true, anything else to false
/// \returns a bool from the given string representation
bool string_to_bool(const std::string& s);

/// \brief Converts the given double \p d to a string representation with the given \p precision
/// \returns a string representation of the given double using the given precision
std::string double_to_string(double d, int precision);

/// \brief Converts the given double \p d to a string representation with a fixed precision of 2
/// \returns a string representation of the given double using a fixed precision of 2
std::string double_to_string(double d);
} // namespace conversions

/// \brief Writes the string representation of the given \p message_type to the given output stream \p os
/// \returns an output stream with the MessageType written to
std::ostream& operator<<(std::ostream& os, const MessageType& message_type);

/// \brief Contains a CaseInsensitive string implementation with a maximum length of 20 printable ASCII characters
class CiString20Type : public CiString {
public:
    /// \brief Creates a case insensitive string from the given string \p data and a maximum length of 20
    explicit CiString20Type(const std::string& data);

    /// \brief Creates a case insensitive string from the given json \p data and a maximum length of 20
    explicit CiString20Type(const json& data);

    /// \brief Creates a case insensitive string with a maximum length of 20
    CiString20Type();

    /// \brief Assignment operator= that converts a given string \p s into a CiString20Type
    CiString20Type& operator=(const std::string& s);

    /// \brief Assignment operator= that converts a given char* \p c into a CiString20Type
    CiString20Type& operator=(const char* c);

    /// \brief Assignment operator= that converts a given json \p j into a CiString20Type
    CiString20Type& operator=(const json& j);
};

/// \brief Conversion from a given CiString20Type \p k to a given json object \p j
void to_json(json& j, const CiString20Type& k);

/// \brief Conversion from a given json object \p j to a given CiString20Type \p k
void from_json(const json& j, CiString20Type& k);

/// \brief Contains a CaseInsensitive string implementation with a maximum length of 25 printable ASCII characters
class CiString25Type : public CiString {
public:
    /// \brief Creates a case insensitive string from the given string \p data and a maximum length of 25
    explicit CiString25Type(const std::string& data);

    /// \brief Creates a case insensitive string from the given json \p data and a maximum length of 25
    explicit CiString25Type(const json& data);

    /// \brief Creates a case insensitive string with a maximum length of 20
    CiString25Type();

    /// \brief Assignment operator= that converts a given string \p s into a CiString25Type
    CiString25Type& operator=(const std::string& s);

    /// \brief Assignment operator= that converts a given char* \p c into a CiString25Type
    CiString25Type& operator=(const char* c);

    /// \brief Assignment operator= that converts a given json \p j into a CiString25Type
    CiString25Type& operator=(const json& j);
};

/// \brief Conversion from a given CiString25Type \p k to a given json object \p j
void to_json(json& j, const CiString25Type& k);

/// \brief Conversion from a given json object \p j to a given CiString25Type \p k
void from_json(const json& j, CiString25Type& k);

/// \brief Contains a CaseInsensitive string implementation with a maximum length of 50 printable ASCII characters
class CiString50Type : public CiString {
public:
    /// \brief Creates a case insensitive string from the given string \p data and a maximum length of 50
    explicit CiString50Type(const std::string& data);

    /// \brief Creates a case insensitive string from the given json \p data and a maximum length of 50
    explicit CiString50Type(const json& data);
    /// \brief Creates a case insensitive string with a maximum length of 50
    CiString50Type();

    /// \brief Assignment operator= that converts a given string \p s into a CiString50Type
    CiString50Type& operator=(const std::string& s);

    /// \brief Assignment operator= that converts a given char* \p c into a CiString50Type
    CiString50Type& operator=(const char* c);

    /// \brief Assignment operator= that converts a given json \p j into a CiString50Type
    CiString50Type& operator=(const json& j);
};

/// \brief Conversion from a given CiString50Type \p k to a given json object \p j
void to_json(json& j, const CiString50Type& k);

/// \brief Conversion from a given json object \p j to a given CiString50Type \p k
void from_json(const json& j, CiString50Type& k);

/// \brief Contains a CaseInsensitive string implementation with a maximum length of 255 printable ASCII characters
class CiString255Type : public CiString {
public:
    /// \brief Creates a case insensitive string from the given string \p data and a maximum length of 255
    explicit CiString255Type(const std::string& data);

    /// \brief Creates a case insensitive string from the given json \p data and a maximum length of 255
    explicit CiString255Type(const json& data);

    /// \brief Creates a case insensitive string with a maximum length of 255
    CiString255Type();

    /// \brief Assignment operator= that converts a given string \p s into a CiString255Type
    CiString255Type& operator=(const std::string& s);

    /// \brief Assignment operator= that converts a given char* \p c into a CiString255Type
    CiString255Type& operator=(const char* c);

    /// \brief Assignment operator= that converts a given json \p j into a CiString255Type
    CiString255Type& operator=(const json& j);
};

/// \brief Conversion from a given CiString255Type \p k to a given json object \p j
void to_json(json& j, const CiString255Type& k);

/// \brief Conversion from a given json object \p j to a given CiString255Type \p k
void from_json(const json& j, CiString255Type& k);

/// \brief Contains a CaseInsensitive string implementation with a maximum length of 500 printable ASCII characters
class CiString500Type : public CiString {
public:
    /// \brief Creates a case insensitive string from the given string \p data and a maximum length of 500

    explicit CiString500Type(const std::string& data);

    /// \brief Creates a case insensitive string from the given json \p data and a maximum length of 500
    explicit CiString500Type(const json& data);

    /// \brief Creates a case insensitive string with a maximum length of 500
    CiString500Type();

    /// \brief Assignment operator= that converts a given string \p s into a CiString500Type
    CiString500Type& operator=(const std::string& s);

    /// \brief Assignment operator= that converts a given char* \p c into a CiString500Type
    CiString500Type& operator=(const char* c);

    /// \brief Assignment operator= that converts a given json \p j into a CiString500Type
    CiString500Type& operator=(const json& j);
};

/// \brief Conversion from a given CiString500Type \p k to a given json object \p j
void to_json(json& j, const CiString500Type& k);

/// \brief Conversion from a given json object \p j to a given CiString500Type \p k
void from_json(const json& j, CiString500Type& k);

/// \brief Contains a MessageId implementation based on a case insensitive string with a maximum length of 36 printable
/// ASCII characters
class MessageId : public CiString {
public:
    /// \brief Creates a case insensitive MessageId from the given string \p data and a maximum length of 36
    explicit MessageId(const std::string& data);

    /// \brief Creates a case insensitive MessageId with a maximum length of 36
    MessageId();

    /// \brief Assignment operator= that converts a given string \p s into a MessageId
    MessageId& operator=(const std::string& s);

    /// \brief Assignment operator= that converts a given char* \p c into a MessageId
    MessageId& operator=(const char* c);

    /// \brief Assignment operator= that converts a given json \p j into a MessageId
    MessageId& operator=(const json& j);

    /// \brief Comparison operator< between this MessageId and the givenn MessageId \p rhs
    bool operator<(const MessageId& rhs);
};

/// \brief Comparison operator< between two MessageId \p lhs and \p rhs
bool operator<(const MessageId& lhs, const MessageId& rhs);

/// \brief Conversion from a given MessageId \p k to a given json object \p j
void to_json(json& j, const MessageId& k);

/// \brief Conversion from a given json object \p j to a given MessageId \p k
void from_json(const json& j, MessageId& k);

/// \brief Contains a DateTime implementation that can parse and create RFC 3339 compatible strings
class DateTime : public DateTimeImpl {
public:
    /// \brief Creates a new DateTime object with the current system time
    DateTime();

    ~DateTime() = default;

    /// \brief Creates a new DateTime object from the given \p timepoint
    explicit DateTime(std::chrono::time_point<std::chrono::system_clock> timepoint);

    /// \brief Creates a new DateTime object from the given \p timepoint_str
    explicit DateTime(const std::string& timepoint_str);

    /// \brief Assignment operator= that converts a given string \p s into a DateTime
    DateTime& operator=(const std::string& s);

    /// \brief Assignment operator= that converts a given char* \p c into a DateTime
    DateTime& operator=(const char* c);
};

/// \brief Combines a Measurand with an optional Phase
struct MeasurandWithPhase {
    ocpp1_6::Measurand measurand;          ///< A OCPP Measurand
    boost::optional<ocpp1_6::Phase> phase; ///< If applicable and available a Phase

    /// \brief Comparison operator== between this MeasurandWithPhase and the given \p measurand_with_phase
    /// \returns true when measurand and phase are equal
    bool operator==(MeasurandWithPhase measurand_with_phase);
};

/// \brief Combines a Measurand with a list of Phases
struct MeasurandWithPhases {
    ocpp1_6::Measurand measurand;       ///< A OCPP Measurand
    std::vector<ocpp1_6::Phase> phases; ///< A list of Phases
};

/// \brief Contains a OCPP Call message
template <class T> struct Call {
    T msg;
    MessageId uniqueId;

    /// \brief Creates a new Call message object
    Call() {
    }

    /// \brief Creates a new Call message object with the given OCPP message \p msg and \p uniqueID
    Call(T msg, MessageId uniqueId) {
        this->msg = msg;
        this->uniqueId = uniqueId;
    }

    /// \brief Conversion from a given Call message \p c to a given json object \p j
    friend void to_json(json& j, const Call& c) {
        j = json::array();
        j.push_back(MessageTypeId::CALL);
        j.push_back(c.uniqueId.get());
        j.push_back(c.msg.get_type());
        j.push_back(json(c.msg));
    }

    /// \brief Conversion from a given json object \p j to a given Call message \p c
    friend void from_json(const json& j, Call& c) {
        // the required parts of the message
        c.msg = j.at(CALL_PAYLOAD);
        c.uniqueId.set(j.at(MESSAGE_ID));
    }

    /// \brief Writes the given case Call \p c to the given output stream \p os
    /// \returns an output stream with the Call written to
    friend std::ostream& operator<<(std::ostream& os, const Call& c) {
        os << json(c).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP CallResult message
template <class T> struct CallResult {
    T msg;
    MessageId uniqueId;

    /// \brief Creates a new CallResult message object
    CallResult() {
    }

    /// \brief Creates a new CallResult message object with the given OCPP message \p msg and \p uniqueID
    CallResult(T msg, MessageId uniqueId) {
        this->msg = msg;
        this->uniqueId = uniqueId;
    }

    /// \brief Conversion from a given CallResult message \p c to a given json object \p j
    friend void to_json(json& j, const CallResult& c) {
        j = json::array();
        j.push_back(MessageTypeId::CALLRESULT);
        j.push_back(c.uniqueId.get());
        j.push_back(json(c.msg));
    }

    /// \brief Conversion from a given json object \p j to a given CallResult message \p c
    friend void from_json(const json& j, CallResult& c) {
        // the required parts of the message
        c.msg = j.at(CALLRESULT_PAYLOAD);
        c.uniqueId.set(j.at(MESSAGE_ID));
    }

    /// \brief Writes the given case CallResult \p c to the given output stream \p os
    /// \returns an output stream with the CallResult written to
    friend std::ostream& operator<<(std::ostream& os, const CallResult& c) {
        os << json(c).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP CallError message
struct CallError {
    MessageId uniqueId;
    std::string errorCode;
    std::string errorDescription;
    json errorDetails;

    /// \brief Creates a new CallError message object
    CallError();

    /// \brief Creates a new CallResult message object with the given \p uniqueID \p errorCode \p errorDescription and
    /// \p errorDetails
    CallError(const MessageId& uniqueId, const std::string& errorCode, const std::string& errorDescription,
              const json& errorDetails);
};

/// \brief Conversion from a given CallError message \p c to a given json object \p j
void to_json(json& j, const CallError& c);

/// \brief Conversion from a given json object \p j to a given CallError message \p c
void from_json(const json& j, CallError& c);

/// \brief Writes the given case CallError \p c to the given output stream \p os
/// \returns an output stream with the CallError written to
std::ostream& operator<<(std::ostream& os, const CallError& c);

/// \brief Contains the different connection states of the charge point
enum ChargePointConnectionState
{
    Disconnected,
    Connected,
    Booted,
    Pending,
    Rejected,
};
namespace conversions {
/// \brief Converts the given ChargePointConnectionState \p e to std::string
/// \returns a string representation of the ChargePointConnectionState
std::string charge_point_connection_state_to_string(ChargePointConnectionState e);

/// \brief Converts the given std::string \p s to ChargePointConnectionState
/// \returns a ChargePointConnectionState from a string representation
ChargePointConnectionState string_to_charge_point_connection_state(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given \p charge_point_connection_state
/// to the given output stream \p os \returns an output stream with the ChargePointConnectionState written to
std::ostream& operator<<(std::ostream& os, const ChargePointConnectionState& charge_point_connection_state);

/// \brief Contains the supported OCPP 1.6 feature profiles
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
/// \brief Converts the given SupportedFeatureProfiles \p e to std::string
/// \returns a string representation of the SupportedFeatureProfiles
std::string supported_feature_profiles_to_string(SupportedFeatureProfiles e);

/// \brief Converts the given std::string \p s to SupportedFeatureProfiles
/// \returns a SupportedFeatureProfiles from a string representation
SupportedFeatureProfiles string_to_supported_feature_profiles(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given \p supported_feature_profiles to the given output stream \p os
/// \returns an output stream with the SupportedFeatureProfiles written to
std::ostream& operator<<(std::ostream& os, const SupportedFeatureProfiles& supported_feature_profiles);

} // namespace ocpp1_6
#endif // OCPP1_6_TYPES_HPP
