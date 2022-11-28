// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <string>

#include <boost/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/types.hpp>

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Conversion from a given IdTagInfo \p k to a given json object \p j
void to_json(json& j, const IdTagInfo& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::authorization_status_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.expiryDate) {
        j["expiryDate"] = k.expiryDate.value().to_rfc3339();
    }
    if (k.parentIdTag) {
        j["parentIdTag"] = k.parentIdTag.value();
    }
}

/// \brief Conversion from a given json object \p j to a given IdTagInfo \p k
void from_json(const json& j, IdTagInfo& k) {
    // the required parts of the message
    k.status = conversions::string_to_authorization_status(j.at("status"));

    // the optional parts of the message
    if (j.contains("expiryDate")) {
        k.expiryDate.emplace(j.at("expiryDate").get<std::string>());
    }
    if (j.contains("parentIdTag")) {
        k.parentIdTag.emplace(j.at("parentIdTag"));
    }
}

// \brief Writes the string representation of the given IdTagInfo \p k to the given output stream \p os
/// \returns an output stream with the IdTagInfo written to
std::ostream& operator<<(std::ostream& os, const IdTagInfo& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ChargingSchedulePeriod \p k to a given json object \p j
void to_json(json& j, const ChargingSchedulePeriod& k) {
    // the required parts of the message
    j = json{
        {"startPeriod", k.startPeriod},
        {"limit", k.limit},
    };
    // the optional parts of the message
    if (k.numberPhases) {
        j["numberPhases"] = k.numberPhases.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingSchedulePeriod \p k
void from_json(const json& j, ChargingSchedulePeriod& k) {
    // the required parts of the message
    k.startPeriod = j.at("startPeriod");
    k.limit = j.at("limit");

    // the optional parts of the message
    if (j.contains("numberPhases")) {
        k.numberPhases.emplace(j.at("numberPhases"));
    }
}

// \brief Writes the string representation of the given ChargingSchedulePeriod \p k to the given output stream \p os
/// \returns an output stream with the ChargingSchedulePeriod written to
std::ostream& operator<<(std::ostream& os, const ChargingSchedulePeriod& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ChargingSchedule \p k to a given json object \p j
void to_json(json& j, const ChargingSchedule& k) {
    // the required parts of the message
    j = json{
        {"chargingRateUnit", conversions::charging_rate_unit_to_string(k.chargingRateUnit)},
        {"chargingSchedulePeriod", k.chargingSchedulePeriod},
    };
    // the optional parts of the message
    if (k.duration) {
        j["duration"] = k.duration.value();
    }
    if (k.startSchedule) {
        j["startSchedule"] = k.startSchedule.value().to_rfc3339();
    }
    if (k.minChargingRate) {
        j["minChargingRate"] = k.minChargingRate.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingSchedule \p k
void from_json(const json& j, ChargingSchedule& k) {
    // the required parts of the message
    k.chargingRateUnit = conversions::string_to_charging_rate_unit(j.at("chargingRateUnit"));
    for (auto val : j.at("chargingSchedulePeriod")) {
        k.chargingSchedulePeriod.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("duration")) {
        k.duration.emplace(j.at("duration"));
    }
    if (j.contains("startSchedule")) {
        k.startSchedule.emplace(j.at("startSchedule").get<std::string>());
    }
    if (j.contains("minChargingRate")) {
        k.minChargingRate.emplace(j.at("minChargingRate"));
    }
}

// \brief Writes the string representation of the given ChargingSchedule \p k to the given output stream \p os
/// \returns an output stream with the ChargingSchedule written to
std::ostream& operator<<(std::ostream& os, const ChargingSchedule& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given KeyValue \p k to a given json object \p j
void to_json(json& j, const KeyValue& k) {
    // the required parts of the message
    j = json{
        {"key", k.key},
        {"readonly", k.readonly},
    };
    // the optional parts of the message
    if (k.value) {
        j["value"] = k.value.value();
    }
}

/// \brief Conversion from a given json object \p j to a given KeyValue \p k
void from_json(const json& j, KeyValue& k) {
    // the required parts of the message
    k.key = j.at("key");
    k.readonly = j.at("readonly");

    // the optional parts of the message
    if (j.contains("value")) {
        k.value.emplace(j.at("value"));
    }
}

// \brief Writes the string representation of the given KeyValue \p k to the given output stream \p os
/// \returns an output stream with the KeyValue written to
std::ostream& operator<<(std::ostream& os, const KeyValue& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given SampledValue \p k to a given json object \p j
void to_json(json& j, const SampledValue& k) {
    // the required parts of the message
    j = json{
        {"value", k.value},
    };
    // the optional parts of the message
    if (k.context) {
        j["context"] = conversions::reading_context_to_string(k.context.value());
    }
    if (k.format) {
        j["format"] = conversions::value_format_to_string(k.format.value());
    }
    if (k.measurand) {
        j["measurand"] = conversions::measurand_to_string(k.measurand.value());
    }
    if (k.phase) {
        j["phase"] = conversions::phase_to_string(k.phase.value());
    }
    if (k.location) {
        j["location"] = conversions::location_to_string(k.location.value());
    }
    if (k.unit) {
        j["unit"] = conversions::unit_of_measure_to_string(k.unit.value());
    }
}

/// \brief Conversion from a given json object \p j to a given SampledValue \p k
void from_json(const json& j, SampledValue& k) {
    // the required parts of the message
    k.value = j.at("value");

    // the optional parts of the message
    if (j.contains("context")) {
        k.context.emplace(j.at("context"));
    }
    if (j.contains("format")) {
        k.format.emplace(j.at("format"));
    }
    if (j.contains("measurand")) {
        k.measurand.emplace(j.at("measurand"));
    }
    if (j.contains("phase")) {
        k.phase.emplace(j.at("phase"));
    }
    if (j.contains("location")) {
        k.location.emplace(j.at("location"));
    }
    if (j.contains("unit")) {
        k.unit.emplace(j.at("unit"));
    }
}

// \brief Writes the string representation of the given SampledValue \p k to the given output stream \p os
/// \returns an output stream with the SampledValue written to
std::ostream& operator<<(std::ostream& os, const SampledValue& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given MeterValue \p k to a given json object \p j
void to_json(json& j, const MeterValue& k) {
    // the required parts of the message
    j = json{
        {"timestamp", k.timestamp.to_rfc3339()},
        {"sampledValue", k.sampledValue},
    };
    // the optional parts of the message
}

/// \brief Conversion from a given json object \p j to a given MeterValue \p k
void from_json(const json& j, MeterValue& k) {
    // the required parts of the message
    k.timestamp = DateTime(std::string(j.at("timestamp")));
    ;
    for (auto val : j.at("sampledValue")) {
        k.sampledValue.push_back(val);
    }

    // the optional parts of the message
}

// \brief Writes the string representation of the given MeterValue \p k to the given output stream \p os
/// \returns an output stream with the MeterValue written to
std::ostream& operator<<(std::ostream& os, const MeterValue& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ChargingProfile \p k to a given json object \p j
void to_json(json& j, const ChargingProfile& k) {
    // the required parts of the message
    j = json{
        {"chargingProfileId", k.chargingProfileId},
        {"stackLevel", k.stackLevel},
        {"chargingProfilePurpose", conversions::charging_profile_purpose_type_to_string(k.chargingProfilePurpose)},
        {"chargingProfileKind", conversions::charging_profile_kind_type_to_string(k.chargingProfileKind)},
        {"chargingSchedule", k.chargingSchedule},
    };
    // the optional parts of the message
    if (k.transactionId) {
        j["transactionId"] = k.transactionId.value();
    }
    if (k.recurrencyKind) {
        j["recurrencyKind"] = conversions::recurrency_kind_type_to_string(k.recurrencyKind.value());
    }
    if (k.validFrom) {
        j["validFrom"] = k.validFrom.value().to_rfc3339();
    }
    if (k.validTo) {
        j["validTo"] = k.validTo.value().to_rfc3339();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingProfile \p k
void from_json(const json& j, ChargingProfile& k) {
    // the required parts of the message
    k.chargingProfileId = j.at("chargingProfileId");
    k.stackLevel = j.at("stackLevel");
    k.chargingProfilePurpose = conversions::string_to_charging_profile_purpose_type(j.at("chargingProfilePurpose"));
    k.chargingProfileKind = conversions::string_to_charging_profile_kind_type(j.at("chargingProfileKind"));
    k.chargingSchedule = j.at("chargingSchedule");

    // the optional parts of the message
    if (j.contains("transactionId")) {
        k.transactionId.emplace(j.at("transactionId"));
    }
    if (j.contains("recurrencyKind")) {
        k.recurrencyKind.emplace(conversions::string_to_recurrency_kind_type(j.at("recurrencyKind")));
    }
    if (j.contains("validFrom")) {
        k.validFrom.emplace(j.at("validFrom").get<std::string>());
    }
    if (j.contains("validTo")) {
        k.validTo.emplace(j.at("validTo").get<std::string>());
    }
}

// \brief Writes the string representation of the given ChargingProfile \p k to the given output stream \p os
/// \returns an output stream with the ChargingProfile written to
std::ostream& operator<<(std::ostream& os, const ChargingProfile& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given LocalAuthorizationList \p k to a given json object \p j
void to_json(json& j, const LocalAuthorizationList& k) {
    // the required parts of the message
    j = json{
        {"idTag", k.idTag},
    };
    // the optional parts of the message
    if (k.idTagInfo) {
        j["idTagInfo"] = k.idTagInfo.value();
    }
}

/// \brief Conversion from a given json object \p j to a given LocalAuthorizationList \p k
void from_json(const json& j, LocalAuthorizationList& k) {
    // the required parts of the message
    k.idTag = j.at("idTag");

    // the optional parts of the message
    if (j.contains("idTagInfo")) {
        k.idTagInfo.emplace(j.at("idTagInfo"));
    }
}

// \brief Writes the string representation of the given LocalAuthorizationList \p k to the given output stream \p os
/// \returns an output stream with the LocalAuthorizationList written to
std::ostream& operator<<(std::ostream& os, const LocalAuthorizationList& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TransactionData \p k to a given json object \p j
void to_json(json& j, const TransactionData& k) {
    // the required parts of the message
    j = json{
        {"timestamp", k.timestamp.to_rfc3339()},
        {"sampledValue", k.sampledValue},
    };
    // the optional parts of the message
}

/// \brief Conversion from a given json object \p j to a given TransactionData \p k
void from_json(const json& j, TransactionData& k) {
    // the required parts of the message
    k.timestamp = DateTime(std::string(j.at("timestamp")));
    ;
    for (auto val : j.at("sampledValue")) {
        k.sampledValue.push_back(val);
    }

    // the optional parts of the message
}

// \brief Writes the string representation of the given TransactionData \p k to the given output stream \p os
/// \returns an output stream with the TransactionData written to
std::ostream& operator<<(std::ostream& os, const TransactionData& k) {
    os << json(k).dump(4);
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

/// \brief Conversion from a given LogParametersType \p k to a given json object \p j
void to_json(json& j, const LogParametersType& k) {
    // the required parts of the message
    j = json{
        {"remoteLocation", k.remoteLocation},
    };
    // the optional parts of the message
    if (k.oldestTimestamp) {
        j["oldestTimestamp"] = k.oldestTimestamp.value().to_rfc3339();
    }
    if (k.latestTimestamp) {
        j["latestTimestamp"] = k.latestTimestamp.value().to_rfc3339();
    }
}

/// \brief Conversion from a given json object \p j to a given LogParametersType \p k
void from_json(const json& j, LogParametersType& k) {
    // the required parts of the message
    k.remoteLocation = j.at("remoteLocation");

    // the optional parts of the message
    if (j.contains("oldestTimestamp")) {
        k.oldestTimestamp.emplace(j.at("oldestTimestamp").get<std::string>());
    }
    if (j.contains("latestTimestamp")) {
        k.latestTimestamp.emplace(j.at("latestTimestamp").get<std::string>());
    }
}

// \brief Writes the string representation of the given LogParametersType \p k to the given output stream \p os
/// \returns an output stream with the LogParametersType written to
std::ostream& operator<<(std::ostream& os, const LogParametersType& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given FirmwareType \p k to a given json object \p j
void to_json(json& j, const FirmwareType& k) {
    // the required parts of the message
    j = json{
        {"location", k.location},
        {"retrieveDateTime", k.retrieveDateTime.to_rfc3339()},
        {"signingCertificate", k.signingCertificate},
        {"signature", k.signature},
    };
    // the optional parts of the message
    if (k.installDateTime) {
        j["installDateTime"] = k.installDateTime.value().to_rfc3339();
    }
}

/// \brief Conversion from a given json object \p j to a given FirmwareType \p k
void from_json(const json& j, FirmwareType& k) {
    // the required parts of the message
    k.location = j.at("location");
    k.retrieveDateTime = DateTime(std::string(j.at("retrieveDateTime")));
    ;
    k.signingCertificate = j.at("signingCertificate");
    k.signature = j.at("signature");

    // the optional parts of the message
    if (j.contains("installDateTime")) {
        k.installDateTime.emplace(j.at("installDateTime").get<std::string>());
    }
}

// \brief Writes the string representation of the given FirmwareType \p k to the given output stream \p os
/// \returns an output stream with the FirmwareType written to
std::ostream& operator<<(std::ostream& os, const FirmwareType& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace ocpp1_6
