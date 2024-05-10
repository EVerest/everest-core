// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>
#include <ocpp/common/call_types.hpp>
#include <ocpp/common/types.hpp>

namespace ocpp {

DateTime::DateTime() : DateTimeImpl() {
}

DateTime::DateTime(std::chrono::time_point<date::utc_clock> timepoint) : DateTimeImpl(timepoint) {
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

DateTimeImpl::DateTimeImpl() {
    this->timepoint = date::utc_clock::now();
}

DateTimeImpl::DateTimeImpl(std::chrono::time_point<date::utc_clock> timepoint) : timepoint(timepoint) {
}

DateTimeImpl::DateTimeImpl(const std::string& timepoint_str) {
    this->from_rfc3339(timepoint_str);
}

std::string DateTimeImpl::to_rfc3339() const {
    return date::format("%FT%TZ", std::chrono::time_point_cast<std::chrono::milliseconds>(this->timepoint));
}

void DateTimeImpl::from_rfc3339(const std::string& timepoint_str) {
    std::istringstream in{timepoint_str};
    in >> date::parse("%FT%T%Ez", this->timepoint);
    if (in.fail()) {
        in.clear();
        in.seekg(0);
        in >> date::parse("%FT%TZ", this->timepoint);
        if (in.fail()) {
            in.clear();
            in.seekg(0);
            in >> date::parse("%FT%T", this->timepoint);
            if (in.fail()) {
                EVLOG_error << "Timepoint string parsing failed. Could not convert: \"" << timepoint_str
                            << "\" into DateTime.";
            }
        }
    }
}

std::chrono::time_point<date::utc_clock> DateTimeImpl::to_time_point() const {
    return this->timepoint;
}

DateTimeImpl& DateTimeImpl::operator=(const DateTimeImpl& dt) {
    this->timepoint = dt.timepoint;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const DateTimeImpl& dt) {
    os << dt.to_rfc3339();
    return os;
}

bool operator>(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
    return lhs.timepoint > rhs.timepoint;
}

bool operator>=(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
    return lhs.timepoint >= rhs.timepoint;
}

bool operator<(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
    return lhs.timepoint < rhs.timepoint;
}

bool operator<=(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
    return lhs.timepoint <= rhs.timepoint;
}

bool operator==(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
    return lhs.timepoint == rhs.timepoint;
}

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

std::string session_started_reason_to_string(SessionStartedReason e) {
    switch (e) {
    case SessionStartedReason::Authorized:
        return "Authorized";
    case SessionStartedReason::EVConnected:
        return "EVConnected";
    }
    throw std::out_of_range("No known string conversion for provided enum of type SessionStartedReason");
}

SessionStartedReason string_to_session_started_reason(const std::string& s) {
    if (s == "Authorized") {
        return SessionStartedReason::Authorized;
    }
    if (s == "EVConnected") {
        return SessionStartedReason::EVConnected;
    }
    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type SessionStartedReason");
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const SessionStartedReason& session_started_reason) {
    os << conversions::session_started_reason_to_string(session_started_reason);
    return os;
}

void to_json(json& j, const Current& k) {
    // the required parts of the type
    j = json({}, true);
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

void from_json(const json& j, Current& k) {
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

std::ostream& operator<<(std::ostream& os, const Current& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const Voltage& k) {
    // the required parts of the type
    j = json({}, true);
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

void from_json(const json& j, Voltage& k) {
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

std::ostream& operator<<(std::ostream& os, const Voltage& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const Frequency& k) {
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

void from_json(const json& j, Frequency& k) {
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

std::ostream& operator<<(std::ostream& os, const Frequency& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const Power& k) {
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

void from_json(const json& j, Power& k) {
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

std::ostream& operator<<(std::ostream& os, const Power& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const Energy& k) {
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

void from_json(const json& j, Energy& k) {
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

std::ostream& operator<<(std::ostream& os, const Energy& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const ReactivePower& k) {
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

void from_json(const json& j, ReactivePower& k) {
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

std::ostream& operator<<(std::ostream& os, const ReactivePower& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const Powermeter& k) {
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

void from_json(const json& j, Powermeter& k) {
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

std::ostream& operator<<(std::ostream& os, const Powermeter& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const Temperature& k) {
    // the required parts of the type
    j = json{
        {"value", k.value},
    };
    // the optional parts of the type
    if (k.location) {
        j["location"] = k.location.value();
    }
}

void from_json(const json& j, Temperature& k) {
    // the required parts of the type
    k.value = j.at("value");

    // the optional parts of the type
    if (j.contains("location")) {
        k.location.emplace(j.at("location"));
    }
}

std::ostream& operator<<(std::ostream& os, const Temperature& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const StateOfCharge& k) {
    // the required parts of the type
    j = json{
        {"value", k.value},
    };
    // the optional parts of the type
    if (k.location) {
        j["location"] = k.location.value();
    }
}

void from_json(const json& j, StateOfCharge& k) {
    // the required parts of the type
    k.value = j.at("value");

    // the optional parts of the type
    if (j.contains("location")) {
        k.location.emplace(j.at("location"));
    }
}

std::ostream& operator<<(std::ostream& os, const StateOfCharge& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const RPM& k) {
    // the required parts of the type
    j = json{
        {"value", k.value},
    };
    // the optional parts of the type
    if (k.location) {
        j["location"] = k.location.value();
    }
}

void from_json(const json& j, RPM& k) {
    // the required parts of the type
    k.value = j.at("value");

    // the optional parts of the type
    if (j.contains("location")) {
        k.location.emplace(j.at("location"));
    }
}

std::ostream& operator<<(std::ostream& os, const RPM& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const Measurement& k) {
    to_json(j, k.power_meter);
    if (k.temperature_C) {
        j["temperature_C"] = k.temperature_C.value();
    }
    if (k.soc_Percent) {
        j["soc_Percent"] = k.soc_Percent.value();
    }
    if (k.rpm) {
        j["rpm"] = k.rpm.value();
    }
}

void from_json(const json& j, Measurement& k) {
    from_json(j, k.power_meter);
    if (j.contains("temperature_C")) {
        k.temperature_C.emplace(j.at("temperature_C"));
    }
    if (j.contains("soc_Percent")) {
        k.soc_Percent.emplace(j.at("soc_Percent"));
    }
    if (j.contains("rpm")) {
        k.rpm.emplace(j.at("rpm"));
    }
}

std::ostream& operator<<(std::ostream& os, const Measurement& k) {
    os << json(k).dump(4);
    return os;
}

namespace conversions {
std::string ca_certificate_type_to_string(CaCertificateType e) {
    switch (e) {
    case CaCertificateType::V2G:
        return "V2G";
    case CaCertificateType::MO:
        return "MO";
    case CaCertificateType::CSMS:
        return "CSMS";
    case CaCertificateType::MF:
        return "MF";
    }

    throw std::out_of_range("No known string conversion for provided enum of type CaCertificateType");
}

CaCertificateType string_to_ca_certificate_type(const std::string& s) {
    if (s == "V2G") {
        return CaCertificateType::V2G;
    } else if (s == "MO") {
        return CaCertificateType::MO;
    } else if (s == "CSMS") {
        return CaCertificateType::CSMS;
    } else if (s == "MF") {
        return CaCertificateType::MF;
    }
    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type CertificateType");
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const CaCertificateType& ca_certificate_type) {
    os << conversions::ca_certificate_type_to_string(ca_certificate_type);
    return os;
}

namespace conversions {
std::string certificate_validation_result_to_string(CertificateValidationResult e) {
    switch (e) {
    case CertificateValidationResult::Valid:
        return "Valid";
    case CertificateValidationResult::Expired:
        return "Expired";
    case CertificateValidationResult::InvalidSignature:
        return "InvalidSignature";
    case CertificateValidationResult::IssuerNotFound:
        return "IssuerNotFound";
    case CertificateValidationResult::InvalidLeafSignature:
        return "InvalidLeafSignature";
    case CertificateValidationResult::InvalidChain:
        return "InvalidChain";
    case CertificateValidationResult::Unknown:
        return "Unknown";
    }

    throw std::out_of_range("No known string conversion for provided enum of type CertificateValidationResult");
}

CertificateValidationResult string_to_certificate_validation_result(const std::string& s) {
    if (s == "Valid") {
        return CertificateValidationResult::Valid;
    }
    if (s == "Expired") {
        return CertificateValidationResult::Expired;
    }
    if (s == "InvalidSignature") {
        return CertificateValidationResult::InvalidSignature;
    }
    if (s == "IssuerNotFound") {
        return CertificateValidationResult::IssuerNotFound;
    }
    if (s == "InvalidLeafSignature") {
        return CertificateValidationResult::InvalidLeafSignature;
    }
    if (s == "InvalidChain") {
        return CertificateValidationResult::InvalidChain;
    }
    if (s == "Unknown") {
        return CertificateValidationResult::Unknown;
    }
    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type CertificateValidationResult");
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const CertificateValidationResult& certificate_validation_result) {
    os << conversions::certificate_validation_result_to_string(certificate_validation_result);
    return os;
}

namespace conversions {
std::string install_certificate_result_to_string(InstallCertificateResult e) {
    switch (e) {
    case InstallCertificateResult::InvalidSignature:
        return "InvalidSignature";
    case InstallCertificateResult::InvalidCertificateChain:
        return "InvalidCertificateChain";
    case InstallCertificateResult::InvalidFormat:
        return "InvalidFormat";
    case InstallCertificateResult::InvalidCommonName:
        return "InvalidCommonName";
    case InstallCertificateResult::NoRootCertificateInstalled:
        return "NoRootCertificateInstalled";
    case InstallCertificateResult::Expired:
        return "Expired";
    case InstallCertificateResult::CertificateStoreMaxLengthExceeded:
        return "CertificateStoreMaxLengthExceeded";
    case InstallCertificateResult::WriteError:
        return "WriteError";
    case InstallCertificateResult::Accepted:
        return "Accepted";
    }

    throw std::out_of_range("No known string conversion for provided enum of type UpdateFirmwareStatusEnumType");
}

InstallCertificateResult string_to_install_certificate_result(const std::string& s) {
    if (s == "InvalidSignature") {
        return InstallCertificateResult::InvalidSignature;
    }
    if (s == "InvalidCertificateChain") {
        return InstallCertificateResult::InvalidCertificateChain;
    }
    if (s == "InvalidFormat") {
        return InstallCertificateResult::InvalidFormat;
    }
    if (s == "InvalidCommonName") {
        return InstallCertificateResult::InvalidCommonName;
    }
    if (s == "NoRootCertificateInstalled") {
        return InstallCertificateResult::NoRootCertificateInstalled;
    }
    if (s == "Expired") {
        return InstallCertificateResult::Expired;
    }
    if (s == "CertificateStoreMaxLengthExceeded") {
        return InstallCertificateResult::CertificateStoreMaxLengthExceeded;
    }
    if (s == "WriteError") {
        return InstallCertificateResult::WriteError;
    }
    if (s == "Accepted") {
        return InstallCertificateResult::Accepted;
    }
    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type InstallCertificateResult");
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const InstallCertificateResult& install_certificate_result) {
    os << conversions::install_certificate_result_to_string(install_certificate_result);
    return os;
}

namespace conversions {
std::string delete_certificate_result_to_string(DeleteCertificateResult e) {
    switch (e) {
    case DeleteCertificateResult::Accepted:
        return "Accepted";
    case DeleteCertificateResult::Failed:
        return "Failed";
    case DeleteCertificateResult::NotFound:
        return "NotFound";
    }

    throw std::out_of_range("No known string conversion for provided enum of type DeleteCertificateResult");
}

DeleteCertificateResult string_to_delete_certificate_result(const std::string& s) {
    if (s == "Accepted") {
        return DeleteCertificateResult::Accepted;
    }
    if (s == "Failed") {
        return DeleteCertificateResult::Failed;
    }
    if (s == "NotFound") {
        return DeleteCertificateResult::NotFound;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type DeleteCertificateResult");
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const DeleteCertificateResult& delete_certificate_result) {
    os << conversions::delete_certificate_result_to_string(delete_certificate_result);
    return os;
}

/// \brief Conversion from a given CertificateHashDataType \p k to a given json object \p j
void to_json(json& j, const CertificateHashDataType& k) {
    // the required parts of the message
    j = json{
        {"hashAlgorithm", conversions::hash_algorithm_enum_type_to_string(k.hashAlgorithm)},
        {"issuerNameHash", k.issuerNameHash},
        {"issuerKeyHash", k.issuerKeyHash},
        {"serialNumber", k.serialNumber},
    };
    // the optional parts of the message
}

/// \brief Conversion from a given json object \p j to a given CertificateHashDataType \p k
void from_json(const json& j, CertificateHashDataType& k) {
    // the required parts of the message
    k.hashAlgorithm = conversions::string_to_hash_algorithm_enum_type(j.at("hashAlgorithm"));
    k.issuerNameHash = j.at("issuerNameHash");
    k.issuerKeyHash = j.at("issuerKeyHash");
    k.serialNumber = j.at("serialNumber");

    // the optional parts of the message
}

// \brief Writes the string representation of the given CertificateHashDataType \p k to the given output stream \p os
/// \returns an output stream with the CertificateHashDataType written to
std::ostream& operator<<(std::ostream& os, const CertificateHashDataType& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given CertificateHashDataChain \p k to a given json object \p j
void to_json(json& j, const CertificateHashDataChain& k) {
    // the required parts of the message
    j = json{
        {"certificateHashData", k.certificateHashData},
        {"certificateType", conversions::certificate_type_to_string(k.certificateType)},
    };
    // the optional parts of the message
    if (k.childCertificateHashData) {
        j["childCertificateHashData"] = json::array();
        for (auto val : k.childCertificateHashData.value()) {
            j["childCertificateHashData"].push_back(val);
        }
    }
}

/// \brief Conversion from a given json object \p j to a given CertificateHashDataChain \p k
void from_json(const json& j, CertificateHashDataChain& k) {
    // the required parts of the message
    k.certificateHashData = j.at("certificateHashData");
    k.certificateType = conversions::string_to_certificate_type(j.at("certificateType"));

    // the optional parts of the message
    if (j.contains("childCertificateHashData")) {
        json arr = j.at("childCertificateHashData");
        std::vector<CertificateHashDataType> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.childCertificateHashData.emplace(vec);
    }
}

// \brief Writes the string representation of the given CertificateHashDataChain \p k to the given output stream \p os
/// \returns an output stream with the CertificateHashDataChain written to
std::ostream& operator<<(std::ostream& os, const CertificateHashDataChain& k) {
    os << json(k).dump(4);
    return os;
}

// from: DeleteCertificateRequest
namespace conversions {
std::string hash_algorithm_enum_type_to_string(HashAlgorithmEnumType e) {
    switch (e) {
    case HashAlgorithmEnumType::SHA256:
        return "SHA256";
    case HashAlgorithmEnumType::SHA384:
        return "SHA384";
    case HashAlgorithmEnumType::SHA512:
        return "SHA512";
    }

    throw std::out_of_range("No known string conversion for provided enum of type HashAlgorithmEnumType");
}

HashAlgorithmEnumType string_to_hash_algorithm_enum_type(const std::string& s) {
    if (s == "SHA256") {
        return HashAlgorithmEnumType::SHA256;
    }
    if (s == "SHA384") {
        return HashAlgorithmEnumType::SHA384;
    }
    if (s == "SHA512") {
        return HashAlgorithmEnumType::SHA512;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type HashAlgorithmEnumType");
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const HashAlgorithmEnumType& hash_algorithm_enum_type) {
    os << conversions::hash_algorithm_enum_type_to_string(hash_algorithm_enum_type);
    return os;
}

namespace conversions {
std::string ocpp_protocol_version_to_string(OcppProtocolVersion e) {
    switch (e) {
    case OcppProtocolVersion::v16:
        return "ocpp1.6";
    case OcppProtocolVersion::v201:
        return "ocpp2.0.1";
    }

    throw std::out_of_range("No known string conversion for provided enum of type OcppProtocolVersion");
}

OcppProtocolVersion string_to_ocpp_protocol_version(const std::string& s) {
    if (s == "ocpp1.6") {
        return OcppProtocolVersion::v16;
    }
    if (s == "ocpp2.0.1") {
        return OcppProtocolVersion::v201;
    }
    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type OcppProtocolVersion");
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const OcppProtocolVersion& ocpp_protocol_version) {
    os << conversions::ocpp_protocol_version_to_string(ocpp_protocol_version);
    return os;
}

namespace conversions {
std::string certificate_signing_use_enum_to_string(CertificateSigningUseEnum e) {
    switch (e) {
    case CertificateSigningUseEnum::ChargingStationCertificate:
        return "ChargingStationCertificate";
    case CertificateSigningUseEnum::V2GCertificate:
        return "V2GCertificate";
    case CertificateSigningUseEnum::ManufacturerCertificate:
        return "ManufacturerCertificate";
    }

    throw std::out_of_range("No known string conversion for provided enum of type CertificateSigningUseEnum");
}

CertificateSigningUseEnum string_to_certificate_signing_use_enum(const std::string& s) {
    if (s == "ChargingStationCertificate") {
        return CertificateSigningUseEnum::ChargingStationCertificate;
    }
    if (s == "V2GCertificate") {
        return CertificateSigningUseEnum::V2GCertificate;
    }
    if (s == "ManufacturerCertificate") {
        return CertificateSigningUseEnum::ManufacturerCertificate;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type CertificateSigningUseEnum");
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const CertificateSigningUseEnum& certificate_signing_use_enum) {
    os << conversions::certificate_signing_use_enum_to_string(certificate_signing_use_enum);
    return os;
}

namespace conversions {
std::string certificate_type_to_string(CertificateType e) {
    switch (e) {
    case CertificateType::V2GRootCertificate:
        return "V2GRootCertificate";
    case CertificateType::MORootCertificate:
        return "MORootCertificate";
    case CertificateType::CSMSRootCertificate:
        return "CSMSRootCertificate";
    case CertificateType::V2GCertificateChain:
        return "V2GCertificateChain";
    case CertificateType::MFRootCertificate:
        return "MFRootCertificate";
    }

    throw std::out_of_range("No known string conversion for provided enum of type CertificateType");
}

CertificateType string_to_certificate_type(const std::string& s) {
    if (s == "V2GRootCertificate") {
        return CertificateType::V2GRootCertificate;
    }
    if (s == "MORootCertificate") {
        return CertificateType::MORootCertificate;
    }
    if (s == "CSMSRootCertificate") {
        return CertificateType::CSMSRootCertificate;
    }
    if (s == "V2GCertificateChain") {
        return CertificateType::V2GCertificateChain;
    }
    if (s == "MFRootCertificate") {
        return CertificateType::MFRootCertificate;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type CertificateType");
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const CertificateType& certificate_type) {
    os << conversions::certificate_type_to_string(certificate_type);
    return os;
}

namespace conversions {

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
    return ocpp::conversions::double_to_string(d, 2);
}

} // namespace conversions

namespace conversions {
std::string generate_certificate_signing_request_status_to_string(const GetCertificateSignRequestStatus status) {
    switch (status) {
    case GetCertificateSignRequestStatus::Accepted:
        return "Accepted";
    case GetCertificateSignRequestStatus::InvalidRequestedType:
        return "InvalidRequestedType";
    case GetCertificateSignRequestStatus::KeyGenError:
        return "KeyGenError";
    case GetCertificateSignRequestStatus::GenerationError:
        return "GenerationError";
    default:
        throw std::out_of_range("Could not convert GetCertificateSignRequestStatus to string");
    }
}
} // namespace conversions

namespace conversions {
v16::FirmwareStatus firmware_status_notification_to_firmware_status(const FirmwareStatusNotification status) {
    switch (status) {
    case FirmwareStatusNotification::Downloaded:
        return v16::FirmwareStatus::Downloaded;
    case FirmwareStatusNotification::DownloadFailed:
        return v16::FirmwareStatus::DownloadFailed;
    case FirmwareStatusNotification::Downloading:
        return v16::FirmwareStatus::Downloading;
    case FirmwareStatusNotification::Idle:
        return v16::FirmwareStatus::Idle;
    case FirmwareStatusNotification::InstallationFailed:
        return v16::FirmwareStatus::InstallationFailed;
    case FirmwareStatusNotification::Installing:
        return v16::FirmwareStatus::Installing;
    case FirmwareStatusNotification::Installed:
        return v16::FirmwareStatus::Installed;
    default:
        throw std::out_of_range("Could not convert to v16::FirmwareStatus");
    }
}

v16::FirmwareStatusEnumType
firmware_status_notification_to_firmware_status_enum_type(const FirmwareStatusNotification status) {
    switch (status) {
    case FirmwareStatusNotification::Downloaded:
        return v16::FirmwareStatusEnumType::Downloaded;
    case FirmwareStatusNotification::DownloadFailed:
        return v16::FirmwareStatusEnumType::DownloadFailed;
    case FirmwareStatusNotification::Downloading:
        return v16::FirmwareStatusEnumType::Downloading;
    case FirmwareStatusNotification::DownloadScheduled:
        return v16::FirmwareStatusEnumType::DownloadScheduled;
    case FirmwareStatusNotification::DownloadPaused:
        return v16::FirmwareStatusEnumType::DownloadPaused;
    case FirmwareStatusNotification::Idle:
        return v16::FirmwareStatusEnumType::Idle;
    case FirmwareStatusNotification::InstallationFailed:
        return v16::FirmwareStatusEnumType::InstallationFailed;
    case FirmwareStatusNotification::Installing:
        return v16::FirmwareStatusEnumType::Installing;
    case FirmwareStatusNotification::Installed:
        return v16::FirmwareStatusEnumType::Installed;
    case FirmwareStatusNotification::InstallRebooting:
        return v16::FirmwareStatusEnumType::InstallRebooting;
    case FirmwareStatusNotification::InstallScheduled:
        return v16::FirmwareStatusEnumType::InstallScheduled;
    case FirmwareStatusNotification::InstallVerificationFailed:
        return v16::FirmwareStatusEnumType::InstallVerificationFailed;
    case FirmwareStatusNotification::InvalidSignature:
        return v16::FirmwareStatusEnumType::InvalidSignature;
    case FirmwareStatusNotification::SignatureVerified:
        return v16::FirmwareStatusEnumType::SignatureVerified;
    default:
        throw std::out_of_range("Could not convert to v16::FirmwareStatusEnumType");
    }
}

} // namespace conversions

} // namespace ocpp
