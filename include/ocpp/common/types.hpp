// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_TYPES_HPP
#define OCPP_COMMON_TYPES_HPP

#include <chrono>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <optional>

#include <boost/algorithm/string/predicate.hpp>

#include <nlohmann/json.hpp>

#include <date/date.h>
#include <date/tz.h>

#include <ocpp/common/cistring.hpp>

using json = nlohmann::json;

namespace ocpp {

// The locations inside the message array
const auto MESSAGE_TYPE_ID = 0;
const auto MESSAGE_ID = 1;
const auto CALL_ACTION = 2;
const auto CALL_PAYLOAD = 3;
const auto CALLRESULT_PAYLOAD = 2;
const auto CALLERROR_ERROR_CODE = 2;
const auto CALLERROR_ERROR_DESCRIPTION = 3;
const auto CALLERROR_ERROR_DETAILS = 4;

/// \brief Contains a MessageId implementation based on a case insensitive string with a maximum length of 36 printable
/// ASCII characters
class MessageId : public CiString<36> {
    using CiString::CiString;
};

/// \brief Comparison operator< between two MessageId \p lhs and \p rhs
bool operator<(const MessageId& lhs, const MessageId& rhs);

/// \brief Conversion from a given MessageId \p k to a given json object \p j
void to_json(json& j, const MessageId& k);

/// \brief Conversion from a given json object \p j to a given MessageId \p k
void from_json(const json& j, MessageId& k);

/// \brief Contains the different message type ids
enum class MessageTypeId {
    CALL = 2,
    CALLRESULT = 3,
    CALLERROR = 4,
    UNKNOWN,
};

/// \brief Parent structure for all OCPP messages supported by this implementation
struct Message {
    /// \brief Provides the type of the message
    /// \returns the message type as a string
    virtual std::string get_type() const = 0;
};

/// \brief Contains a DateTime implementation that can parse and create RFC 3339 compatible strings
class DateTimeImpl {
private:
    std::chrono::time_point<date::utc_clock> timepoint;

public:
    /// \brief Creates a new DateTimeImpl object with the current utc time
    DateTimeImpl();

    ~DateTimeImpl() = default;

    /// \brief Creates a new DateTimeImpl object from the given \p timepoint
    explicit DateTimeImpl(std::chrono::time_point<date::utc_clock> timepoint);

    /// \brief Creates a new DateTimeImpl object from the given \p timepoint_str
    explicit DateTimeImpl(const std::string& timepoint_str);

    /// \brief Converts this DateTimeImpl to a RFC 3339 compatible string
    /// \returns a RFC 3339 compatible string representation of the stored DateTime
    std::string to_rfc3339() const;

    /// \brief Sets the timepoint of this DateTimeImpl to the given \p timepoint_str
    void from_rfc3339(const std::string& timepoint_str);

    /// \brief Converts this DateTimeImpl to a std::chrono::time_point
    /// \returns a std::chrono::time_point
    std::chrono::time_point<date::utc_clock> to_time_point() const;

    /// \brief Writes the given DateTimeImpl \p dt to the given output stream \p os as a RFC 3339 compatible string
    /// \returns an output stream with the DateTimeImpl as a RFC 3339 compatible string written to
    friend std::ostream& operator<<(std::ostream& os, const DateTimeImpl& dt);
    operator std::string() {
        return this->to_rfc3339();
    }

    /// \brief Comparison operator> between two DateTimeImpl \p lhs and \p rhs
    friend bool operator>(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
        return lhs.to_rfc3339() > rhs.to_rfc3339();
    }

    /// \brief Comparison operator>= between two DateTimeImpl \p lhs and \p rhs
    friend bool operator>=(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
        return lhs.to_rfc3339() >= rhs.to_rfc3339();
    }

    /// \brief Comparison operator< between two DateTimeImpl \p lhs and \p rhs
    friend bool operator<(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
        return lhs.to_rfc3339() < rhs.to_rfc3339();
    }

    /// \brief Comparison operator<= between two DateTimeImpl \p lhs and \p rhs
    friend bool operator<=(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
        return lhs.to_rfc3339() <= rhs.to_rfc3339();
    }

    /// \brief Comparison operator== between two DateTimeImpl \p lhs and \p rhs
    friend bool operator==(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
        return lhs.to_rfc3339() == rhs.to_rfc3339();
    }

    DateTimeImpl& operator=(const DateTimeImpl& dt) {
        this->timepoint = dt.timepoint;
        return *this;
    }
};

/// \brief Contains a DateTime implementation that can parse and create RFC 3339 compatible strings
class DateTime : public DateTimeImpl {
public:
    /// \brief Creates a new DateTime object with the current system time
    DateTime();

    ~DateTime() = default;

    /// \brief Creates a new DateTime object from the given \p timepoint
    explicit DateTime(std::chrono::time_point<date::utc_clock> timepoint);

    /// \brief Creates a new DateTime object from the given \p timepoint_str
    explicit DateTime(const std::string& timepoint_str);

    /// \brief Assignment operator= that converts a given string \p s into a DateTime
    DateTime& operator=(const std::string& s);

    /// \brief Assignment operator= that converts a given char* \p c into a DateTime
    DateTime& operator=(const char* c);
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
enum SessionStartedReason {
    EVConnected,
    Authorized
};
namespace conversions {
/// \brief Converts the given SessionStartedReason \p e to std::string
/// \returns a string representation of the SessionStartedReason
std::string session_started_reason_to_string(SessionStartedReason e);

/// \brief Converts the given std::string \p s to SessionStartedReason
/// \returns a SessionStartedReason from a string representation
SessionStartedReason string_to_session_started_reason(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given \p session_started_reason
/// to the given output stream \p os \returns an output stream with the SessionStartedReason written to
std::ostream& operator<<(std::ostream& os, const SessionStartedReason& session_started_reason);

struct Current {
    std::optional<float> DC; ///< DC current
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only
    std::optional<float> N;  ///< AC Neutral value only

    /// \brief Conversion from a given Current \p k to a given json object \p j
    friend void to_json(json& j, const Current& k) {
        // the required parts of the type
        j = json({});
        // the optional parts of the type
        if (k.DC) {
            j["DC"] = k.DC.value();
        }
        if (k.L1) {
            j["L1"] = k.L1.value();
        }
        if (k.L2) {
            j["L2"] = k.L2.value();
        }
        if (k.L3) {
            j["L3"] = k.L3.value();
        }
        if (k.N) {
            j["N"] = k.N.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given Current \p k
    friend void from_json(const json& j, Current& k) {
        // the required parts of the type

        // the optional parts of the type
        if (j.contains("DC")) {
            k.DC.emplace(j.at("DC"));
        }
        if (j.contains("L1")) {
            k.L1.emplace(j.at("L1"));
        }
        if (j.contains("L2")) {
            k.L2.emplace(j.at("L2"));
        }
        if (j.contains("L3")) {
            k.L3.emplace(j.at("L3"));
        }
        if (j.contains("N")) {
            k.N.emplace(j.at("N"));
        }
    }

    // \brief Writes the string representation of the given Current \p k to the given output stream \p os
    /// \returns an output stream with the Current written to
    friend std::ostream& operator<<(std::ostream& os, const Current& k) {
        os << json(k).dump(4);
        return os;
    }
};
struct Voltage {
    std::optional<float> DC; ///< DC voltage
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only

    /// \brief Conversion from a given Voltage \p k to a given json object \p j
    friend void to_json(json& j, const Voltage& k) {
        // the required parts of the type
        j = json({});
        // the optional parts of the type
        if (k.DC) {
            j["DC"] = k.DC.value();
        }
        if (k.L1) {
            j["L1"] = k.L1.value();
        }
        if (k.L2) {
            j["L2"] = k.L2.value();
        }
        if (k.L3) {
            j["L3"] = k.L3.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given Voltage \p k
    friend void from_json(const json& j, Voltage& k) {
        // the required parts of the type

        // the optional parts of the type
        if (j.contains("DC")) {
            k.DC.emplace(j.at("DC"));
        }
        if (j.contains("L1")) {
            k.L1.emplace(j.at("L1"));
        }
        if (j.contains("L2")) {
            k.L2.emplace(j.at("L2"));
        }
        if (j.contains("L3")) {
            k.L3.emplace(j.at("L3"));
        }
    }

    // \brief Writes the string representation of the given Voltage \p k to the given output stream \p os
    /// \returns an output stream with the Voltage written to
    friend std::ostream& operator<<(std::ostream& os, const Voltage& k) {
        os << json(k).dump(4);
        return os;
    }
};
struct Frequency {
    float L1;                ///< AC L1 value
    std::optional<float> L2; ///< AC L2 value
    std::optional<float> L3; ///< AC L3 value

    /// \brief Conversion from a given Frequency \p k to a given json object \p j
    friend void to_json(json& j, const Frequency& k) {
        // the required parts of the type
        j = json{
            {"L1", k.L1},
        };
        // the optional parts of the type
        if (k.L2) {
            j["L2"] = k.L2.value();
        }
        if (k.L3) {
            j["L3"] = k.L3.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given Frequency \p k
    friend void from_json(const json& j, Frequency& k) {
        // the required parts of the type
        k.L1 = j.at("L1");

        // the optional parts of the type
        if (j.contains("L2")) {
            k.L2.emplace(j.at("L2"));
        }
        if (j.contains("L3")) {
            k.L3.emplace(j.at("L3"));
        }
    }

    // \brief Writes the string representation of the given Frequency \p k to the given output stream \p os
    /// \returns an output stream with the Frequency written to
    friend std::ostream& operator<<(std::ostream& os, const Frequency& k) {
        os << json(k).dump(4);
        return os;
    }
};
struct Power {
    float total;             ///< DC / AC Sum value
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only

    /// \brief Conversion from a given Power \p k to a given json object \p j
    friend void to_json(json& j, const Power& k) {
        // the required parts of the type
        j = json{
            {"total", k.total},
        };
        // the optional parts of the type
        if (k.L1) {
            j["L1"] = k.L1.value();
        }
        if (k.L2) {
            j["L2"] = k.L2.value();
        }
        if (k.L3) {
            j["L3"] = k.L3.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given Power \p k
    friend void from_json(const json& j, Power& k) {
        // the required parts of the type
        k.total = j.at("total");

        // the optional parts of the type
        if (j.contains("L1")) {
            k.L1.emplace(j.at("L1"));
        }
        if (j.contains("L2")) {
            k.L2.emplace(j.at("L2"));
        }
        if (j.contains("L3")) {
            k.L3.emplace(j.at("L3"));
        }
    }

    // \brief Writes the string representation of the given Power \p k to the given output stream \p os
    /// \returns an output stream with the Power written to
    friend std::ostream& operator<<(std::ostream& os, const Power& k) {
        os << json(k).dump(4);
        return os;
    }
};
struct Energy {
    float total;             ///< DC / AC Sum value (which is relevant for billing)
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only

    /// \brief Conversion from a given Energy \p k to a given json object \p j
    friend void to_json(json& j, const Energy& k) {
        // the required parts of the type
        j = json{
            {"total", k.total},
        };
        // the optional parts of the type
        if (k.L1) {
            j["L1"] = k.L1.value();
        }
        if (k.L2) {
            j["L2"] = k.L2.value();
        }
        if (k.L3) {
            j["L3"] = k.L3.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given Energy \p k
    friend void from_json(const json& j, Energy& k) {
        // the required parts of the type
        k.total = j.at("total");

        // the optional parts of the type
        if (j.contains("L1")) {
            k.L1.emplace(j.at("L1"));
        }
        if (j.contains("L2")) {
            k.L2.emplace(j.at("L2"));
        }
        if (j.contains("L3")) {
            k.L3.emplace(j.at("L3"));
        }
    }

    // \brief Writes the string representation of the given Energy \p k to the given output stream \p os
    /// \returns an output stream with the Energy written to
    friend std::ostream& operator<<(std::ostream& os, const Energy& k) {
        os << json(k).dump(4);
        return os;
    }
};
struct ReactivePower {
    float total;                 ///< VAR total
    std::optional<float> VARphA; ///< VAR phase A
    std::optional<float> VARphB; ///< VAR phase B
    std::optional<float> VARphC; ///< VAR phase C

    /// \brief Conversion from a given ReactivePower \p k to a given json object \p j
    friend void to_json(json& j, const ReactivePower& k) {
        // the required parts of the type
        j = json{
            {"total", k.total},
        };
        // the optional parts of the type
        if (k.VARphA) {
            j["VARphA"] = k.VARphA.value();
        }
        if (k.VARphB) {
            j["VARphB"] = k.VARphB.value();
        }
        if (k.VARphC) {
            j["VARphC"] = k.VARphC.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given ReactivePower \p k
    friend void from_json(const json& j, ReactivePower& k) {
        // the required parts of the type
        k.total = j.at("total");

        // the optional parts of the type
        if (j.contains("VARphA")) {
            k.VARphA.emplace(j.at("VARphA"));
        }
        if (j.contains("VARphB")) {
            k.VARphB.emplace(j.at("VARphB"));
        }
        if (j.contains("VARphC")) {
            k.VARphC.emplace(j.at("VARphC"));
        }
    }

    // \brief Writes the string representation of the given ReactivePower \p k to the given output stream \p os
    /// \returns an output stream with the ReactivePower written to
    friend std::ostream& operator<<(std::ostream& os, const ReactivePower& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct Powermeter {
    std::string timestamp;                  ///< Timestamp of measurement
    Energy energy_Wh_import;                ///< Imported energy in Wh (from grid)
    std::optional<std::string> meter_id;    ///< A (user defined) meter if (e.g. id printed on the case)
    std::optional<bool> phase_seq_error;    ///< AC only: true for 3 phase rotation error (ccw)
    std::optional<Energy> energy_Wh_export; ///< Exported energy in Wh (to grid)
    std::optional<Power>
        power_W; ///< Instantaneous power in Watt. Negative values are exported, positive values imported Energy.
    std::optional<Voltage> voltage_V;      ///< Voltage in Volts
    std::optional<ReactivePower> VAR;      ///< Reactive power VAR
    std::optional<Current> current_A;      ///< Current in ampere
    std::optional<Frequency> frequency_Hz; ///< Grid frequency in Hertz

    /// \brief Conversion from a given Powermeter \p k to a given json object \p j
    friend void to_json(json& j, const Powermeter& k) {
        // the required parts of the type
        j = json{
            {"timestamp", k.timestamp},
            {"energy_Wh_import", k.energy_Wh_import},
        };
        // the optional parts of the type
        if (k.meter_id) {
            j["meter_id"] = k.meter_id.value();
        }
        if (k.phase_seq_error) {
            j["phase_seq_error"] = k.phase_seq_error.value();
        }
        if (k.energy_Wh_export) {
            j["energy_Wh_export"] = k.energy_Wh_export.value();
        }
        if (k.power_W) {
            j["power_W"] = k.power_W.value();
        }
        if (k.voltage_V) {
            j["voltage_V"] = k.voltage_V.value();
        }
        if (k.VAR) {
            j["VAR"] = k.VAR.value();
        }
        if (k.current_A) {
            j["current_A"] = k.current_A.value();
        }
        if (k.frequency_Hz) {
            j["frequency_Hz"] = k.frequency_Hz.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given Powermeter \p k
    friend void from_json(const json& j, Powermeter& k) {
        // the required parts of the type
        k.timestamp = j.at("timestamp");
        k.energy_Wh_import = j.at("energy_Wh_import");

        // the optional parts of the type
        if (j.contains("meter_id")) {
            k.meter_id.emplace(j.at("meter_id"));
        }
        if (j.contains("phase_seq_error")) {
            k.phase_seq_error.emplace(j.at("phase_seq_error"));
        }
        if (j.contains("energy_Wh_export")) {
            k.energy_Wh_export.emplace(j.at("energy_Wh_export"));
        }
        if (j.contains("power_W")) {
            k.power_W.emplace(j.at("power_W"));
        }
        if (j.contains("voltage_V")) {
            k.voltage_V.emplace(j.at("voltage_V"));
        }
        if (j.contains("VAR")) {
            k.VAR.emplace(j.at("VAR"));
        }
        if (j.contains("current_A")) {
            k.current_A.emplace(j.at("current_A"));
        }
        if (j.contains("frequency_Hz")) {
            k.frequency_Hz.emplace(j.at("frequency_Hz"));
        }
    }

    // \brief Writes the string representation of the given Powermeter \p k to the given output stream \p os
    /// \returns an output stream with the Powermeter written to
    friend std::ostream& operator<<(std::ostream& os, const Powermeter& k) {
        os << json(k).dump(4);
        return os;
    }
};

enum class CertificateType {
    CentralSystemRootCertificate,
    ManufacturerRootCertificate,
    CSMSRootCertificate,
    MORootCertificate,
    ClientCertificate,
    V2GRootCertificate,
    V2GCertificateChain,
    CertificateProvisioningServiceRootCertificate,
    OEMRootCertificate
};

namespace conversions {
/// \brief Converts the given CertificateType \p e to human readable string
/// \returns a string representation of the CertificateType
std::string certificate_type_to_string(CertificateType e);

/// \brief Converts the given std::string \p s to CertificateType
/// \returns a CertificateType from a string representation
CertificateType string_to_certificate_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CertificateType \p
/// certificate_type to the given output stream \p os \returns an output stream with the
/// CertificateType written to
std::ostream& operator<<(std::ostream& os, const CertificateType& certificate_type);

enum class CertificateVerificationResult {
    Expired,
    InvalidSignature,
    InvalidCertificateChain,
    InvalidCommonName,
    NoRootCertificateInstalled,
    Valid
};

namespace conversions {
/// \brief Converts the given CertificateVerificationResult \p e to human readable string
/// \returns a string representation of the CertificateVerificationResult
std::string certificate_verification_result_to_string(CertificateVerificationResult e);

/// \brief Converts the given std::string \p s to CertificateVerificationResult
/// \returns a CertificateVerificationResult from a string representation
CertificateVerificationResult string_to_certificate_verification_result(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CertificateVerificationResult \p
/// certificate_verification_result to the given output stream \p os \returns an output stream with the
/// CertificateVerificationResult written to
std::ostream& operator<<(std::ostream& os, const CertificateVerificationResult& certificate_verification_result);

enum class InstallCertificateResult {
    InvalidSignature,
    InvalidCertificateChain,
    InvalidFormat,
    Valid,
    Ok,
    CertificateStoreMaxLengthExceeded,
    WriteError
};

namespace conversions {
/// \brief Converts the given InstallCertificateResult \p e to human readable string
/// \returns a string representation of the InstallCertificateResult
std::string install_certificate_result_to_string(InstallCertificateResult e);

/// \brief Converts the given std::string \p s to InstallCertificateResult
/// \returns a InstallCertificateResult from a string representation
InstallCertificateResult string_to_install_certificate_result(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given InstallCertificateResult \p
/// install_certificate_result to the given output stream \p os \returns an output stream with the
/// InstallCertificateResult written to
std::ostream& operator<<(std::ostream& os, const InstallCertificateResult& install_certificate_result);

enum class DeleteCertificateResult {
    Accepted,
    Failed,
    NotFound,
};

namespace conversions {
/// \brief Converts the given DeleteCertificateResult \p e to human readable string
/// \returns a string representation of the DeleteCertificateResult
std::string delete_certificate_result_to_string(DeleteCertificateResult e);

/// \brief Converts the given std::string \p s to DeleteCertificateResult
/// \returns a DeleteCertificateResult from a string representation
DeleteCertificateResult string_to_delete_certificate_result(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given DeleteCertificateResult \p
/// delete_certificate_result to the given output stream \p os \returns an output stream with the
/// DeleteCertificateResult written to
std::ostream& operator<<(std::ostream& os, const DeleteCertificateResult& delete_certificate_result);

// from: GetInstalledCertificateIdsResponse
enum class HashAlgorithmEnumType {
    SHA256,
    SHA384,
    SHA512,
};

namespace conversions {
/// \brief Converts the given HashAlgorithmEnumType \p e to human readable string
/// \returns a string representation of the HashAlgorithmEnumType
std::string hash_algorithm_enum_type_to_string(HashAlgorithmEnumType e);

/// \brief Converts the given std::string \p s to HashAlgorithmEnumType
/// \returns a HashAlgorithmEnumType from a string representation
HashAlgorithmEnumType string_to_hash_algorithm_enum_type(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given HashAlgorithmEnumType \p hash_algorithm_enum_type to the given
/// output stream \p os \returns an output stream with the HashAlgorithmEnumType written to
std::ostream& operator<<(std::ostream& os, const HashAlgorithmEnumType& hash_algorithm_enum_type);

struct CertificateHashDataType {
    HashAlgorithmEnumType hashAlgorithm;
    CiString<128> issuerNameHash;
    CiString<128> issuerKeyHash;
    CiString<40> serialNumber;
};
/// \brief Conversion from a given CertificateHashDataType \p k to a given json object \p j
void to_json(json& j, const CertificateHashDataType& k);

/// \brief Conversion from a given json object \p j to a given CertificateHashDataType \p k
void from_json(const json& j, CertificateHashDataType& k);

// \brief Writes the string representation of the given CertificateHashDataType \p k to the given output stream \p os
/// \returns an output stream with the CertificateHashDataType written to
std::ostream& operator<<(std::ostream& os, const CertificateHashDataType& k);

struct CertificateHashDataChain {
    CertificateHashDataType certificateHashData;
    CertificateType certificateType;
    std::optional<std::vector<CertificateHashDataType>> childCertificateHashData;
};
/// \brief Conversion from a given CertificateHashDataChain \p k to a given json object \p j
void to_json(json& j, const CertificateHashDataChain& k);

/// \brief Conversion from a given json object \p j to a given CertificateHashDataChain \p k
void from_json(const json& j, CertificateHashDataChain& k);

// \brief Writes the string representation of the given CertificateHashDataChain \p k to the given output stream \p os
/// \returns an output stream with the CertificateHashDataChain written to
std::ostream& operator<<(std::ostream& os, const CertificateHashDataChain& k);

enum class OcppProtocolVersion {
    v16,
    v201
};

namespace conversions {
/// \brief Converts the given OcppProtocolVersion \p e to human readable string
/// \returns a string representation of the OcppProtocolVersion
std::string ocpp_protocol_version_to_string(OcppProtocolVersion e);

/// \brief Converts the given std::string \p s to OcppProtocolVersion
/// \returns a OcppProtocolVersion from a string representation
OcppProtocolVersion string_to_ocpp_protocol_version(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given OcppProtocolVersion \p
/// ocpp_protocol_version to the given output stream \p os \returns an output stream with the
/// OcppProtocolVersion written to
std::ostream& operator<<(std::ostream& os, const OcppProtocolVersion& ocpp_protocol_version);

enum class CertificateSigningUseEnum {
    ChargingStationCertificate,
    V2GCertificate,
};

namespace conversions {
/// \brief Converts the given CertificateSigningUseEnum \p e to human readable string
/// \returns a string representation of the CertificateSigningUseEnum
std::string certificate_signing_use_enum_to_string(CertificateSigningUseEnum e);

/// \brief Converts the given std::string \p s to CertificateSigningUseEnum
/// \returns a CertificateSigningUseEnum from a string representation
CertificateSigningUseEnum string_to_certificate_signing_use_enum(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given CertificateSigningUseEnum \p certificate_signing_use_enum to
/// the given output stream \p os \returns an output stream with the CertificateSigningUseEnum written to
std::ostream& operator<<(std::ostream& os, const CertificateSigningUseEnum& certificate_signing_use_enum);

namespace conversions {
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

} // namespace ocpp

#endif
