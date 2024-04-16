// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#include <string>

#include <nlohmann/json.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>

#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Conversion from a given AdditionalInfo \p k to a given json object \p j
void to_json(json& j, const AdditionalInfo& k) {
    // the required parts of the message
    j = json{
        {"additionalIdToken", k.additionalIdToken},
        {"type", k.type},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given AdditionalInfo \p k
void from_json(const json& j, AdditionalInfo& k) {
    // the required parts of the message
    k.additionalIdToken = j.at("additionalIdToken");
    k.type = j.at("type");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given AdditionalInfo \p k to the given output stream \p os
/// \returns an output stream with the AdditionalInfo written to
std::ostream& operator<<(std::ostream& os, const AdditionalInfo& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given IdToken \p k to a given json object \p j
void to_json(json& j, const IdToken& k) {
    // the required parts of the message
    j = json{
        {"idToken", k.idToken},
        {"type", conversions::id_token_enum_to_string(k.type)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.additionalInfo) {
        j["additionalInfo"] = json::array();
        for (auto val : k.additionalInfo.value()) {
            j["additionalInfo"].push_back(val);
        }
    }
}

/// \brief Conversion from a given json object \p j to a given IdToken \p k
void from_json(const json& j, IdToken& k) {
    // the required parts of the message
    k.idToken = j.at("idToken");
    k.type = conversions::string_to_id_token_enum(j.at("type"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("additionalInfo")) {
        json arr = j.at("additionalInfo");
        std::vector<AdditionalInfo> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.additionalInfo.emplace(vec);
    }
}

// \brief Writes the string representation of the given IdToken \p k to the given output stream \p os
/// \returns an output stream with the IdToken written to
std::ostream& operator<<(std::ostream& os, const IdToken& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given OCSPRequestData \p k to a given json object \p j
void to_json(json& j, const OCSPRequestData& k) {
    // the required parts of the message
    j = json{
        {"hashAlgorithm", conversions::hash_algorithm_enum_to_string(k.hashAlgorithm)},
        {"issuerNameHash", k.issuerNameHash},
        {"issuerKeyHash", k.issuerKeyHash},
        {"serialNumber", k.serialNumber},
        {"responderURL", k.responderURL},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given OCSPRequestData \p k
void from_json(const json& j, OCSPRequestData& k) {
    // the required parts of the message
    k.hashAlgorithm = conversions::string_to_hash_algorithm_enum(j.at("hashAlgorithm"));
    k.issuerNameHash = j.at("issuerNameHash");
    k.issuerKeyHash = j.at("issuerKeyHash");
    k.serialNumber = j.at("serialNumber");
    k.responderURL = j.at("responderURL");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given OCSPRequestData \p k to the given output stream \p os
/// \returns an output stream with the OCSPRequestData written to
std::ostream& operator<<(std::ostream& os, const OCSPRequestData& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given MessageContent \p k to a given json object \p j
void to_json(json& j, const MessageContent& k) {
    // the required parts of the message
    j = json{
        {"format", conversions::message_format_enum_to_string(k.format)},
        {"content", k.content},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.language) {
        j["language"] = k.language.value();
    }
}

/// \brief Conversion from a given json object \p j to a given MessageContent \p k
void from_json(const json& j, MessageContent& k) {
    // the required parts of the message
    k.format = conversions::string_to_message_format_enum(j.at("format"));
    k.content = j.at("content");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("language")) {
        k.language.emplace(j.at("language"));
    }
}

// \brief Writes the string representation of the given MessageContent \p k to the given output stream \p os
/// \returns an output stream with the MessageContent written to
std::ostream& operator<<(std::ostream& os, const MessageContent& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given IdTokenInfo \p k to a given json object \p j
void to_json(json& j, const IdTokenInfo& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::authorization_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.cacheExpiryDateTime) {
        j["cacheExpiryDateTime"] = k.cacheExpiryDateTime.value().to_rfc3339();
    }
    if (k.chargingPriority) {
        j["chargingPriority"] = k.chargingPriority.value();
    }
    if (k.language1) {
        j["language1"] = k.language1.value();
    }
    if (k.evseId) {
        j["evseId"] = json::array();
        for (auto val : k.evseId.value()) {
            j["evseId"].push_back(val);
        }
    }
    if (k.groupIdToken) {
        j["groupIdToken"] = k.groupIdToken.value();
    }
    if (k.language2) {
        j["language2"] = k.language2.value();
    }
    if (k.personalMessage) {
        j["personalMessage"] = k.personalMessage.value();
    }
}

/// \brief Conversion from a given json object \p j to a given IdTokenInfo \p k
void from_json(const json& j, IdTokenInfo& k) {
    // the required parts of the message
    k.status = conversions::string_to_authorization_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("cacheExpiryDateTime")) {
        k.cacheExpiryDateTime.emplace(j.at("cacheExpiryDateTime").get<std::string>());
    }
    if (j.contains("chargingPriority")) {
        k.chargingPriority.emplace(j.at("chargingPriority"));
    }
    if (j.contains("language1")) {
        k.language1.emplace(j.at("language1"));
    }
    if (j.contains("evseId")) {
        json arr = j.at("evseId");
        std::vector<int32_t> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.evseId.emplace(vec);
    }
    if (j.contains("groupIdToken")) {
        k.groupIdToken.emplace(j.at("groupIdToken"));
    }
    if (j.contains("language2")) {
        k.language2.emplace(j.at("language2"));
    }
    if (j.contains("personalMessage")) {
        k.personalMessage.emplace(j.at("personalMessage"));
    }
}

// \brief Writes the string representation of the given IdTokenInfo \p k to the given output stream \p os
/// \returns an output stream with the IdTokenInfo written to
std::ostream& operator<<(std::ostream& os, const IdTokenInfo& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given Modem \p k to a given json object \p j
void to_json(json& j, const Modem& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.iccid) {
        j["iccid"] = k.iccid.value();
    }
    if (k.imsi) {
        j["imsi"] = k.imsi.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Modem \p k
void from_json(const json& j, Modem& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("iccid")) {
        k.iccid.emplace(j.at("iccid"));
    }
    if (j.contains("imsi")) {
        k.imsi.emplace(j.at("imsi"));
    }
}

// \brief Writes the string representation of the given Modem \p k to the given output stream \p os
/// \returns an output stream with the Modem written to
std::ostream& operator<<(std::ostream& os, const Modem& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ChargingStation \p k to a given json object \p j
void to_json(json& j, const ChargingStation& k) {
    // the required parts of the message
    j = json{
        {"model", k.model},
        {"vendorName", k.vendorName},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.serialNumber) {
        j["serialNumber"] = k.serialNumber.value();
    }
    if (k.modem) {
        j["modem"] = k.modem.value();
    }
    if (k.firmwareVersion) {
        j["firmwareVersion"] = k.firmwareVersion.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingStation \p k
void from_json(const json& j, ChargingStation& k) {
    // the required parts of the message
    k.model = j.at("model");
    k.vendorName = j.at("vendorName");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("serialNumber")) {
        k.serialNumber.emplace(j.at("serialNumber"));
    }
    if (j.contains("modem")) {
        k.modem.emplace(j.at("modem"));
    }
    if (j.contains("firmwareVersion")) {
        k.firmwareVersion.emplace(j.at("firmwareVersion"));
    }
}

// \brief Writes the string representation of the given ChargingStation \p k to the given output stream \p os
/// \returns an output stream with the ChargingStation written to
std::ostream& operator<<(std::ostream& os, const ChargingStation& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given StatusInfo \p k to a given json object \p j
void to_json(json& j, const StatusInfo& k) {
    // the required parts of the message
    j = json{
        {"reasonCode", k.reasonCode},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.additionalInfo) {
        j["additionalInfo"] = k.additionalInfo.value();
    }
}

/// \brief Conversion from a given json object \p j to a given StatusInfo \p k
void from_json(const json& j, StatusInfo& k) {
    // the required parts of the message
    k.reasonCode = j.at("reasonCode");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("additionalInfo")) {
        k.additionalInfo.emplace(j.at("additionalInfo"));
    }
}

// \brief Writes the string representation of the given StatusInfo \p k to the given output stream \p os
/// \returns an output stream with the StatusInfo written to
std::ostream& operator<<(std::ostream& os, const StatusInfo& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given EVSE \p k to a given json object \p j
void to_json(json& j, const EVSE& k) {
    // the required parts of the message
    j = json{
        {"id", k.id},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.connectorId) {
        j["connectorId"] = k.connectorId.value();
    }
}

/// \brief Conversion from a given json object \p j to a given EVSE \p k
void from_json(const json& j, EVSE& k) {
    // the required parts of the message
    k.id = j.at("id");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("connectorId")) {
        k.connectorId.emplace(j.at("connectorId"));
    }
}

// \brief Writes the string representation of the given EVSE \p k to the given output stream \p os
/// \returns an output stream with the EVSE written to
std::ostream& operator<<(std::ostream& os, const EVSE& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ClearChargingProfile \p k to a given json object \p j
void to_json(json& j, const ClearChargingProfile& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.evseId) {
        j["evseId"] = k.evseId.value();
    }
    if (k.chargingProfilePurpose) {
        j["chargingProfilePurpose"] =
            conversions::charging_profile_purpose_enum_to_string(k.chargingProfilePurpose.value());
    }
    if (k.stackLevel) {
        j["stackLevel"] = k.stackLevel.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ClearChargingProfile \p k
void from_json(const json& j, ClearChargingProfile& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("evseId")) {
        k.evseId.emplace(j.at("evseId"));
    }
    if (j.contains("chargingProfilePurpose")) {
        k.chargingProfilePurpose.emplace(
            conversions::string_to_charging_profile_purpose_enum(j.at("chargingProfilePurpose")));
    }
    if (j.contains("stackLevel")) {
        k.stackLevel.emplace(j.at("stackLevel"));
    }
}

// \brief Writes the string representation of the given ClearChargingProfile \p k to the given output stream \p os
/// \returns an output stream with the ClearChargingProfile written to
std::ostream& operator<<(std::ostream& os, const ClearChargingProfile& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ClearMonitoringResult \p k to a given json object \p j
void to_json(json& j, const ClearMonitoringResult& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::clear_monitoring_status_enum_to_string(k.status)},
        {"id", k.id},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ClearMonitoringResult \p k
void from_json(const json& j, ClearMonitoringResult& k) {
    // the required parts of the message
    k.status = conversions::string_to_clear_monitoring_status_enum(j.at("status"));
    k.id = j.at("id");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

// \brief Writes the string representation of the given ClearMonitoringResult \p k to the given output stream \p os
/// \returns an output stream with the ClearMonitoringResult written to
std::ostream& operator<<(std::ostream& os, const ClearMonitoringResult& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given CertificateHashDataType \p k to a given json object \p j
void to_json(json& j, const CertificateHashDataType& k) {
    // the required parts of the message
    j = json{
        {"hashAlgorithm", conversions::hash_algorithm_enum_to_string(k.hashAlgorithm)},
        {"issuerNameHash", k.issuerNameHash},
        {"issuerKeyHash", k.issuerKeyHash},
        {"serialNumber", k.serialNumber},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given CertificateHashDataType \p k
void from_json(const json& j, CertificateHashDataType& k) {
    // the required parts of the message
    k.hashAlgorithm = conversions::string_to_hash_algorithm_enum(j.at("hashAlgorithm"));
    k.issuerNameHash = j.at("issuerNameHash");
    k.issuerKeyHash = j.at("issuerKeyHash");
    k.serialNumber = j.at("serialNumber");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given CertificateHashDataType \p k to the given output stream \p os
/// \returns an output stream with the CertificateHashDataType written to
std::ostream& operator<<(std::ostream& os, const CertificateHashDataType& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ChargingProfileCriterion \p k to a given json object \p j
void to_json(json& j, const ChargingProfileCriterion& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.chargingProfilePurpose) {
        j["chargingProfilePurpose"] =
            conversions::charging_profile_purpose_enum_to_string(k.chargingProfilePurpose.value());
    }
    if (k.stackLevel) {
        j["stackLevel"] = k.stackLevel.value();
    }
    if (k.chargingProfileId) {
        if (j.size() == 0) {
            j = json{{"chargingProfileId", json::array()}};
        } else {
            j["chargingProfileId"] = json::array();
        }
        for (auto val : k.chargingProfileId.value()) {
            j["chargingProfileId"].push_back(val);
        }
    }
    if (k.chargingLimitSource) {
        if (j.size() == 0) {
            j = json{{"chargingLimitSource", json::array()}};
        } else {
            j["chargingLimitSource"] = json::array();
        }
        for (auto val : k.chargingLimitSource.value()) {
            j["chargingLimitSource"].push_back(val);
        }
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingProfileCriterion \p k
void from_json(const json& j, ChargingProfileCriterion& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("chargingProfilePurpose")) {
        k.chargingProfilePurpose.emplace(
            conversions::string_to_charging_profile_purpose_enum(j.at("chargingProfilePurpose")));
    }
    if (j.contains("stackLevel")) {
        k.stackLevel.emplace(j.at("stackLevel"));
    }
    if (j.contains("chargingProfileId")) {
        json arr = j.at("chargingProfileId");
        std::vector<int32_t> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.chargingProfileId.emplace(vec);
    }
    if (j.contains("chargingLimitSource")) {
        json arr = j.at("chargingLimitSource");
        std::vector<ChargingLimitSourceEnum> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.chargingLimitSource.emplace(vec);
    }
}

// \brief Writes the string representation of the given ChargingProfileCriterion \p k to the given output stream \p os
/// \returns an output stream with the ChargingProfileCriterion written to
std::ostream& operator<<(std::ostream& os, const ChargingProfileCriterion& k) {
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
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.numberPhases) {
        j["numberPhases"] = k.numberPhases.value();
    }
    if (k.phaseToUse) {
        j["phaseToUse"] = k.phaseToUse.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingSchedulePeriod \p k
void from_json(const json& j, ChargingSchedulePeriod& k) {
    // the required parts of the message
    k.startPeriod = j.at("startPeriod");
    k.limit = j.at("limit");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("numberPhases")) {
        k.numberPhases.emplace(j.at("numberPhases"));
    }
    if (j.contains("phaseToUse")) {
        k.phaseToUse.emplace(j.at("phaseToUse"));
    }
}

// \brief Writes the string representation of the given ChargingSchedulePeriod \p k to the given output stream \p os
/// \returns an output stream with the ChargingSchedulePeriod written to
std::ostream& operator<<(std::ostream& os, const ChargingSchedulePeriod& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given CompositeSchedule \p k to a given json object \p j
void to_json(json& j, const CompositeSchedule& k) {
    // the required parts of the message
    j = json{
        {"chargingSchedulePeriod", k.chargingSchedulePeriod},
        {"evseId", k.evseId},
        {"duration", k.duration},
        {"scheduleStart", k.scheduleStart.to_rfc3339()},
        {"chargingRateUnit", conversions::charging_rate_unit_enum_to_string(k.chargingRateUnit)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given CompositeSchedule \p k
void from_json(const json& j, CompositeSchedule& k) {
    // the required parts of the message
    for (auto val : j.at("chargingSchedulePeriod")) {
        k.chargingSchedulePeriod.push_back(val);
    }
    k.evseId = j.at("evseId");
    k.duration = j.at("duration");
    k.scheduleStart = ocpp::DateTime(std::string(j.at("scheduleStart")));
    ;
    k.chargingRateUnit = conversions::string_to_charging_rate_unit_enum(j.at("chargingRateUnit"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given CompositeSchedule \p k to the given output stream \p os
/// \returns an output stream with the CompositeSchedule written to
std::ostream& operator<<(std::ostream& os, const CompositeSchedule& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given CertificateHashDataChain \p k to a given json object \p j
void to_json(json& j, const CertificateHashDataChain& k) {
    // the required parts of the message
    j = json{
        {"certificateHashData", k.certificateHashData},
        {"certificateType", conversions::get_certificate_id_use_enum_to_string(k.certificateType)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
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
    k.certificateType = conversions::string_to_get_certificate_id_use_enum(j.at("certificateType"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
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

/// \brief Conversion from a given LogParameters \p k to a given json object \p j
void to_json(json& j, const LogParameters& k) {
    // the required parts of the message
    j = json{
        {"remoteLocation", k.remoteLocation},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.oldestTimestamp) {
        j["oldestTimestamp"] = k.oldestTimestamp.value().to_rfc3339();
    }
    if (k.latestTimestamp) {
        j["latestTimestamp"] = k.latestTimestamp.value().to_rfc3339();
    }
}

/// \brief Conversion from a given json object \p j to a given LogParameters \p k
void from_json(const json& j, LogParameters& k) {
    // the required parts of the message
    k.remoteLocation = j.at("remoteLocation");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("oldestTimestamp")) {
        k.oldestTimestamp.emplace(j.at("oldestTimestamp").get<std::string>());
    }
    if (j.contains("latestTimestamp")) {
        k.latestTimestamp.emplace(j.at("latestTimestamp").get<std::string>());
    }
}

// \brief Writes the string representation of the given LogParameters \p k to the given output stream \p os
/// \returns an output stream with the LogParameters written to
std::ostream& operator<<(std::ostream& os, const LogParameters& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given Component \p k to a given json object \p j
void to_json(json& j, const Component& k) {
    // the required parts of the message
    j = json{
        {"name", k.name},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.evse) {
        j["evse"] = k.evse.value();
    }
    if (k.instance) {
        j["instance"] = k.instance.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Component \p k
void from_json(const json& j, Component& k) {
    // the required parts of the message
    k.name = j.at("name");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("evse")) {
        k.evse.emplace(j.at("evse"));
    }
    if (j.contains("instance")) {
        k.instance.emplace(j.at("instance"));
    }
}

// \brief Writes the string representation of the given Component \p k to the given output stream \p os
/// \returns an output stream with the Component written to
std::ostream& operator<<(std::ostream& os, const Component& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given Variable \p k to a given json object \p j
void to_json(json& j, const Variable& k) {
    // the required parts of the message
    j = json{
        {"name", k.name},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.instance) {
        j["instance"] = k.instance.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Variable \p k
void from_json(const json& j, Variable& k) {
    // the required parts of the message
    k.name = j.at("name");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("instance")) {
        k.instance.emplace(j.at("instance"));
    }
}

// \brief Writes the string representation of the given Variable \p k to the given output stream \p os
/// \returns an output stream with the Variable written to
std::ostream& operator<<(std::ostream& os, const Variable& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ComponentVariable \p k to a given json object \p j
void to_json(json& j, const ComponentVariable& k) {
    // the required parts of the message
    j = json{
        {"component", k.component},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.variable) {
        j["variable"] = k.variable.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ComponentVariable \p k
void from_json(const json& j, ComponentVariable& k) {
    // the required parts of the message
    k.component = j.at("component");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("variable")) {
        k.variable.emplace(j.at("variable"));
    }
}

// \brief Writes the string representation of the given ComponentVariable \p k to the given output stream \p os
/// \returns an output stream with the ComponentVariable written to
std::ostream& operator<<(std::ostream& os, const ComponentVariable& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given GetVariableData \p k to a given json object \p j
void to_json(json& j, const GetVariableData& k) {
    // the required parts of the message
    j = json{
        {"component", k.component},
        {"variable", k.variable},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.attributeType) {
        j["attributeType"] = conversions::attribute_enum_to_string(k.attributeType.value());
    }
}

/// \brief Conversion from a given json object \p j to a given GetVariableData \p k
void from_json(const json& j, GetVariableData& k) {
    // the required parts of the message
    k.component = j.at("component");
    k.variable = j.at("variable");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("attributeType")) {
        k.attributeType.emplace(conversions::string_to_attribute_enum(j.at("attributeType")));
    }
}

// \brief Writes the string representation of the given GetVariableData \p k to the given output stream \p os
/// \returns an output stream with the GetVariableData written to
std::ostream& operator<<(std::ostream& os, const GetVariableData& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given GetVariableResult \p k to a given json object \p j
void to_json(json& j, const GetVariableResult& k) {
    // the required parts of the message
    j = json{
        {"attributeStatus", conversions::get_variable_status_enum_to_string(k.attributeStatus)},
        {"component", k.component},
        {"variable", k.variable},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.attributeStatusInfo) {
        j["attributeStatusInfo"] = k.attributeStatusInfo.value();
    }
    if (k.attributeType) {
        j["attributeType"] = conversions::attribute_enum_to_string(k.attributeType.value());
    }
    if (k.attributeValue) {
        j["attributeValue"] = k.attributeValue.value();
    }
}

/// \brief Conversion from a given json object \p j to a given GetVariableResult \p k
void from_json(const json& j, GetVariableResult& k) {
    // the required parts of the message
    k.attributeStatus = conversions::string_to_get_variable_status_enum(j.at("attributeStatus"));
    k.component = j.at("component");
    k.variable = j.at("variable");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("attributeStatusInfo")) {
        k.attributeStatusInfo.emplace(j.at("attributeStatusInfo"));
    }
    if (j.contains("attributeType")) {
        k.attributeType.emplace(conversions::string_to_attribute_enum(j.at("attributeType")));
    }
    if (j.contains("attributeValue")) {
        k.attributeValue.emplace(j.at("attributeValue"));
    }
}

// \brief Writes the string representation of the given GetVariableResult \p k to the given output stream \p os
/// \returns an output stream with the GetVariableResult written to
std::ostream& operator<<(std::ostream& os, const GetVariableResult& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given SignedMeterValue \p k to a given json object \p j
void to_json(json& j, const SignedMeterValue& k) {
    // the required parts of the message
    j = json{
        {"signedMeterData", k.signedMeterData},
        {"signingMethod", k.signingMethod},
        {"encodingMethod", k.encodingMethod},
        {"publicKey", k.publicKey},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given SignedMeterValue \p k
void from_json(const json& j, SignedMeterValue& k) {
    // the required parts of the message
    k.signedMeterData = j.at("signedMeterData");
    k.signingMethod = j.at("signingMethod");
    k.encodingMethod = j.at("encodingMethod");
    k.publicKey = j.at("publicKey");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given SignedMeterValue \p k to the given output stream \p os
/// \returns an output stream with the SignedMeterValue written to
std::ostream& operator<<(std::ostream& os, const SignedMeterValue& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given UnitOfMeasure \p k to a given json object \p j
void to_json(json& j, const UnitOfMeasure& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.unit) {
        j["unit"] = k.unit.value();
    }
    if (k.multiplier) {
        j["multiplier"] = k.multiplier.value();
    }
}

/// \brief Conversion from a given json object \p j to a given UnitOfMeasure \p k
void from_json(const json& j, UnitOfMeasure& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("unit")) {
        k.unit.emplace(j.at("unit"));
    }
    if (j.contains("multiplier")) {
        k.multiplier.emplace(j.at("multiplier"));
    }
}

// \brief Writes the string representation of the given UnitOfMeasure \p k to the given output stream \p os
/// \returns an output stream with the UnitOfMeasure written to
std::ostream& operator<<(std::ostream& os, const UnitOfMeasure& k) {
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
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.context) {
        j["context"] = conversions::reading_context_enum_to_string(k.context.value());
    }
    if (k.measurand) {
        j["measurand"] = conversions::measurand_enum_to_string(k.measurand.value());
    }
    if (k.phase) {
        j["phase"] = conversions::phase_enum_to_string(k.phase.value());
    }
    if (k.location) {
        j["location"] = conversions::location_enum_to_string(k.location.value());
    }
    if (k.signedMeterValue) {
        j["signedMeterValue"] = k.signedMeterValue.value();
    }
    if (k.unitOfMeasure) {
        j["unitOfMeasure"] = k.unitOfMeasure.value();
    }
}

/// \brief Conversion from a given json object \p j to a given SampledValue \p k
void from_json(const json& j, SampledValue& k) {
    // the required parts of the message
    k.value = j.at("value");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("context")) {
        k.context.emplace(conversions::string_to_reading_context_enum(j.at("context")));
    }
    if (j.contains("measurand")) {
        k.measurand.emplace(conversions::string_to_measurand_enum(j.at("measurand")));
    }
    if (j.contains("phase")) {
        k.phase.emplace(conversions::string_to_phase_enum(j.at("phase")));
    }
    if (j.contains("location")) {
        k.location.emplace(conversions::string_to_location_enum(j.at("location")));
    }
    if (j.contains("signedMeterValue")) {
        k.signedMeterValue.emplace(j.at("signedMeterValue"));
    }
    if (j.contains("unitOfMeasure")) {
        k.unitOfMeasure.emplace(j.at("unitOfMeasure"));
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
        {"sampledValue", k.sampledValue},
        {"timestamp", k.timestamp.to_rfc3339()},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given MeterValue \p k
void from_json(const json& j, MeterValue& k) {
    // the required parts of the message
    for (auto val : j.at("sampledValue")) {
        k.sampledValue.push_back(val);
    }
    k.timestamp = ocpp::DateTime(std::string(j.at("timestamp")));
    ;

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given MeterValue \p k to the given output stream \p os
/// \returns an output stream with the MeterValue written to
std::ostream& operator<<(std::ostream& os, const MeterValue& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given RelativeTimeInterval \p k to a given json object \p j
void to_json(json& j, const RelativeTimeInterval& k) {
    // the required parts of the message
    j = json{
        {"start", k.start},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.duration) {
        j["duration"] = k.duration.value();
    }
}

/// \brief Conversion from a given json object \p j to a given RelativeTimeInterval \p k
void from_json(const json& j, RelativeTimeInterval& k) {
    // the required parts of the message
    k.start = j.at("start");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("duration")) {
        k.duration.emplace(j.at("duration"));
    }
}

// \brief Writes the string representation of the given RelativeTimeInterval \p k to the given output stream \p os
/// \returns an output stream with the RelativeTimeInterval written to
std::ostream& operator<<(std::ostream& os, const RelativeTimeInterval& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given Cost \p k to a given json object \p j
void to_json(json& j, const Cost& k) {
    // the required parts of the message
    j = json{
        {"costKind", conversions::cost_kind_enum_to_string(k.costKind)},
        {"amount", k.amount},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.amountMultiplier) {
        j["amountMultiplier"] = k.amountMultiplier.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Cost \p k
void from_json(const json& j, Cost& k) {
    // the required parts of the message
    k.costKind = conversions::string_to_cost_kind_enum(j.at("costKind"));
    k.amount = j.at("amount");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("amountMultiplier")) {
        k.amountMultiplier.emplace(j.at("amountMultiplier"));
    }
}

// \brief Writes the string representation of the given Cost \p k to the given output stream \p os
/// \returns an output stream with the Cost written to
std::ostream& operator<<(std::ostream& os, const Cost& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ConsumptionCost \p k to a given json object \p j
void to_json(json& j, const ConsumptionCost& k) {
    // the required parts of the message
    j = json{
        {"startValue", k.startValue},
        {"cost", k.cost},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ConsumptionCost \p k
void from_json(const json& j, ConsumptionCost& k) {
    // the required parts of the message
    k.startValue = j.at("startValue");
    for (auto val : j.at("cost")) {
        k.cost.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given ConsumptionCost \p k to the given output stream \p os
/// \returns an output stream with the ConsumptionCost written to
std::ostream& operator<<(std::ostream& os, const ConsumptionCost& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given SalesTariffEntry \p k to a given json object \p j
void to_json(json& j, const SalesTariffEntry& k) {
    // the required parts of the message
    j = json{
        {"relativeTimeInterval", k.relativeTimeInterval},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.ePriceLevel) {
        j["ePriceLevel"] = k.ePriceLevel.value();
    }
    if (k.consumptionCost) {
        j["consumptionCost"] = json::array();
        for (auto val : k.consumptionCost.value()) {
            j["consumptionCost"].push_back(val);
        }
    }
}

/// \brief Conversion from a given json object \p j to a given SalesTariffEntry \p k
void from_json(const json& j, SalesTariffEntry& k) {
    // the required parts of the message
    k.relativeTimeInterval = j.at("relativeTimeInterval");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("ePriceLevel")) {
        k.ePriceLevel.emplace(j.at("ePriceLevel"));
    }
    if (j.contains("consumptionCost")) {
        json arr = j.at("consumptionCost");
        std::vector<ConsumptionCost> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.consumptionCost.emplace(vec);
    }
}

// \brief Writes the string representation of the given SalesTariffEntry \p k to the given output stream \p os
/// \returns an output stream with the SalesTariffEntry written to
std::ostream& operator<<(std::ostream& os, const SalesTariffEntry& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given SalesTariff \p k to a given json object \p j
void to_json(json& j, const SalesTariff& k) {
    // the required parts of the message
    j = json{
        {"id", k.id},
        {"salesTariffEntry", k.salesTariffEntry},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.salesTariffDescription) {
        j["salesTariffDescription"] = k.salesTariffDescription.value();
    }
    if (k.numEPriceLevels) {
        j["numEPriceLevels"] = k.numEPriceLevels.value();
    }
}

/// \brief Conversion from a given json object \p j to a given SalesTariff \p k
void from_json(const json& j, SalesTariff& k) {
    // the required parts of the message
    k.id = j.at("id");
    for (auto val : j.at("salesTariffEntry")) {
        k.salesTariffEntry.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("salesTariffDescription")) {
        k.salesTariffDescription.emplace(j.at("salesTariffDescription"));
    }
    if (j.contains("numEPriceLevels")) {
        k.numEPriceLevels.emplace(j.at("numEPriceLevels"));
    }
}

// \brief Writes the string representation of the given SalesTariff \p k to the given output stream \p os
/// \returns an output stream with the SalesTariff written to
std::ostream& operator<<(std::ostream& os, const SalesTariff& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ChargingSchedule \p k to a given json object \p j
void to_json(json& j, const ChargingSchedule& k) {
    // the required parts of the message
    j = json{
        {"id", k.id},
        {"chargingRateUnit", conversions::charging_rate_unit_enum_to_string(k.chargingRateUnit)},
        {"chargingSchedulePeriod", k.chargingSchedulePeriod},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.startSchedule) {
        j["startSchedule"] = k.startSchedule.value().to_rfc3339();
    }
    if (k.duration) {
        j["duration"] = k.duration.value();
    }
    if (k.minChargingRate) {
        j["minChargingRate"] = k.minChargingRate.value();
    }
    if (k.salesTariff) {
        j["salesTariff"] = k.salesTariff.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingSchedule \p k
void from_json(const json& j, ChargingSchedule& k) {
    // the required parts of the message
    k.id = j.at("id");
    k.chargingRateUnit = conversions::string_to_charging_rate_unit_enum(j.at("chargingRateUnit"));
    for (auto val : j.at("chargingSchedulePeriod")) {
        k.chargingSchedulePeriod.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("startSchedule")) {
        k.startSchedule.emplace(j.at("startSchedule").get<std::string>());
    }
    if (j.contains("duration")) {
        k.duration.emplace(j.at("duration"));
    }
    if (j.contains("minChargingRate")) {
        k.minChargingRate.emplace(j.at("minChargingRate"));
    }
    if (j.contains("salesTariff")) {
        k.salesTariff.emplace(j.at("salesTariff"));
    }
}

// \brief Writes the string representation of the given ChargingSchedule \p k to the given output stream \p os
/// \returns an output stream with the ChargingSchedule written to
std::ostream& operator<<(std::ostream& os, const ChargingSchedule& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ChargingLimit \p k to a given json object \p j
void to_json(json& j, const ChargingLimit& k) {
    // the required parts of the message
    j = json{
        {"chargingLimitSource", conversions::charging_limit_source_enum_to_string(k.chargingLimitSource)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.isGridCritical) {
        j["isGridCritical"] = k.isGridCritical.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingLimit \p k
void from_json(const json& j, ChargingLimit& k) {
    // the required parts of the message
    k.chargingLimitSource = conversions::string_to_charging_limit_source_enum(j.at("chargingLimitSource"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("isGridCritical")) {
        k.isGridCritical.emplace(j.at("isGridCritical"));
    }
}

// \brief Writes the string representation of the given ChargingLimit \p k to the given output stream \p os
/// \returns an output stream with the ChargingLimit written to
std::ostream& operator<<(std::ostream& os, const ChargingLimit& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given MessageInfo \p k to a given json object \p j
void to_json(json& j, const MessageInfo& k) {
    // the required parts of the message
    j = json{
        {"id", k.id},
        {"priority", conversions::message_priority_enum_to_string(k.priority)},
        {"message", k.message},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.display) {
        j["display"] = k.display.value();
    }
    if (k.state) {
        j["state"] = conversions::message_state_enum_to_string(k.state.value());
    }
    if (k.startDateTime) {
        j["startDateTime"] = k.startDateTime.value().to_rfc3339();
    }
    if (k.endDateTime) {
        j["endDateTime"] = k.endDateTime.value().to_rfc3339();
    }
    if (k.transactionId) {
        j["transactionId"] = k.transactionId.value();
    }
}

/// \brief Conversion from a given json object \p j to a given MessageInfo \p k
void from_json(const json& j, MessageInfo& k) {
    // the required parts of the message
    k.id = j.at("id");
    k.priority = conversions::string_to_message_priority_enum(j.at("priority"));
    k.message = j.at("message");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("display")) {
        k.display.emplace(j.at("display"));
    }
    if (j.contains("state")) {
        k.state.emplace(conversions::string_to_message_state_enum(j.at("state")));
    }
    if (j.contains("startDateTime")) {
        k.startDateTime.emplace(j.at("startDateTime").get<std::string>());
    }
    if (j.contains("endDateTime")) {
        k.endDateTime.emplace(j.at("endDateTime").get<std::string>());
    }
    if (j.contains("transactionId")) {
        k.transactionId.emplace(j.at("transactionId"));
    }
}

// \brief Writes the string representation of the given MessageInfo \p k to the given output stream \p os
/// \returns an output stream with the MessageInfo written to
std::ostream& operator<<(std::ostream& os, const MessageInfo& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ACChargingParameters \p k to a given json object \p j
void to_json(json& j, const ACChargingParameters& k) {
    // the required parts of the message
    j = json{
        {"energyAmount", k.energyAmount},
        {"evMinCurrent", k.evMinCurrent},
        {"evMaxCurrent", k.evMaxCurrent},
        {"evMaxVoltage", k.evMaxVoltage},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ACChargingParameters \p k
void from_json(const json& j, ACChargingParameters& k) {
    // the required parts of the message
    k.energyAmount = j.at("energyAmount");
    k.evMinCurrent = j.at("evMinCurrent");
    k.evMaxCurrent = j.at("evMaxCurrent");
    k.evMaxVoltage = j.at("evMaxVoltage");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given ACChargingParameters \p k to the given output stream \p os
/// \returns an output stream with the ACChargingParameters written to
std::ostream& operator<<(std::ostream& os, const ACChargingParameters& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given DCChargingParameters \p k to a given json object \p j
void to_json(json& j, const DCChargingParameters& k) {
    // the required parts of the message
    j = json{
        {"evMaxCurrent", k.evMaxCurrent},
        {"evMaxVoltage", k.evMaxVoltage},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.energyAmount) {
        j["energyAmount"] = k.energyAmount.value();
    }
    if (k.evMaxPower) {
        j["evMaxPower"] = k.evMaxPower.value();
    }
    if (k.stateOfCharge) {
        j["stateOfCharge"] = k.stateOfCharge.value();
    }
    if (k.evEnergyCapacity) {
        j["evEnergyCapacity"] = k.evEnergyCapacity.value();
    }
    if (k.fullSoC) {
        j["fullSoC"] = k.fullSoC.value();
    }
    if (k.bulkSoC) {
        j["bulkSoC"] = k.bulkSoC.value();
    }
}

/// \brief Conversion from a given json object \p j to a given DCChargingParameters \p k
void from_json(const json& j, DCChargingParameters& k) {
    // the required parts of the message
    k.evMaxCurrent = j.at("evMaxCurrent");
    k.evMaxVoltage = j.at("evMaxVoltage");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("energyAmount")) {
        k.energyAmount.emplace(j.at("energyAmount"));
    }
    if (j.contains("evMaxPower")) {
        k.evMaxPower.emplace(j.at("evMaxPower"));
    }
    if (j.contains("stateOfCharge")) {
        k.stateOfCharge.emplace(j.at("stateOfCharge"));
    }
    if (j.contains("evEnergyCapacity")) {
        k.evEnergyCapacity.emplace(j.at("evEnergyCapacity"));
    }
    if (j.contains("fullSoC")) {
        k.fullSoC.emplace(j.at("fullSoC"));
    }
    if (j.contains("bulkSoC")) {
        k.bulkSoC.emplace(j.at("bulkSoC"));
    }
}

// \brief Writes the string representation of the given DCChargingParameters \p k to the given output stream \p os
/// \returns an output stream with the DCChargingParameters written to
std::ostream& operator<<(std::ostream& os, const DCChargingParameters& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ChargingNeeds \p k to a given json object \p j
void to_json(json& j, const ChargingNeeds& k) {
    // the required parts of the message
    j = json{
        {"requestedEnergyTransfer", conversions::energy_transfer_mode_enum_to_string(k.requestedEnergyTransfer)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.acChargingParameters) {
        j["acChargingParameters"] = k.acChargingParameters.value();
    }
    if (k.dcChargingParameters) {
        j["dcChargingParameters"] = k.dcChargingParameters.value();
    }
    if (k.departureTime) {
        j["departureTime"] = k.departureTime.value().to_rfc3339();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingNeeds \p k
void from_json(const json& j, ChargingNeeds& k) {
    // the required parts of the message
    k.requestedEnergyTransfer = conversions::string_to_energy_transfer_mode_enum(j.at("requestedEnergyTransfer"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("acChargingParameters")) {
        k.acChargingParameters.emplace(j.at("acChargingParameters"));
    }
    if (j.contains("dcChargingParameters")) {
        k.dcChargingParameters.emplace(j.at("dcChargingParameters"));
    }
    if (j.contains("departureTime")) {
        k.departureTime.emplace(j.at("departureTime").get<std::string>());
    }
}

// \brief Writes the string representation of the given ChargingNeeds \p k to the given output stream \p os
/// \returns an output stream with the ChargingNeeds written to
std::ostream& operator<<(std::ostream& os, const ChargingNeeds& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given EventData \p k to a given json object \p j
void to_json(json& j, const EventData& k) {
    // the required parts of the message
    j = json{
        {"eventId", k.eventId},
        {"timestamp", k.timestamp.to_rfc3339()},
        {"trigger", conversions::event_trigger_enum_to_string(k.trigger)},
        {"actualValue", k.actualValue},
        {"component", k.component},
        {"eventNotificationType", conversions::event_notification_enum_to_string(k.eventNotificationType)},
        {"variable", k.variable},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.cause) {
        j["cause"] = k.cause.value();
    }
    if (k.techCode) {
        j["techCode"] = k.techCode.value();
    }
    if (k.techInfo) {
        j["techInfo"] = k.techInfo.value();
    }
    if (k.cleared) {
        j["cleared"] = k.cleared.value();
    }
    if (k.transactionId) {
        j["transactionId"] = k.transactionId.value();
    }
    if (k.variableMonitoringId) {
        j["variableMonitoringId"] = k.variableMonitoringId.value();
    }
}

/// \brief Conversion from a given json object \p j to a given EventData \p k
void from_json(const json& j, EventData& k) {
    // the required parts of the message
    k.eventId = j.at("eventId");
    k.timestamp = ocpp::DateTime(std::string(j.at("timestamp")));
    ;
    k.trigger = conversions::string_to_event_trigger_enum(j.at("trigger"));
    k.actualValue = j.at("actualValue");
    k.component = j.at("component");
    k.eventNotificationType = conversions::string_to_event_notification_enum(j.at("eventNotificationType"));
    k.variable = j.at("variable");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("cause")) {
        k.cause.emplace(j.at("cause"));
    }
    if (j.contains("techCode")) {
        k.techCode.emplace(j.at("techCode"));
    }
    if (j.contains("techInfo")) {
        k.techInfo.emplace(j.at("techInfo"));
    }
    if (j.contains("cleared")) {
        k.cleared.emplace(j.at("cleared"));
    }
    if (j.contains("transactionId")) {
        k.transactionId.emplace(j.at("transactionId"));
    }
    if (j.contains("variableMonitoringId")) {
        k.variableMonitoringId.emplace(j.at("variableMonitoringId"));
    }
}

// \brief Writes the string representation of the given EventData \p k to the given output stream \p os
/// \returns an output stream with the EventData written to
std::ostream& operator<<(std::ostream& os, const EventData& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given VariableMonitoring \p k to a given json object \p j
void to_json(json& j, const VariableMonitoring& k) {
    // the required parts of the message
    j = json{
        {"id", k.id},
        {"transaction", k.transaction},
        {"value", k.value},
        {"type", conversions::monitor_enum_to_string(k.type)},
        {"severity", k.severity},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given VariableMonitoring \p k
void from_json(const json& j, VariableMonitoring& k) {
    // the required parts of the message
    k.id = j.at("id");
    k.transaction = j.at("transaction");
    k.value = j.at("value");
    k.type = conversions::string_to_monitor_enum(j.at("type"));
    k.severity = j.at("severity");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given VariableMonitoring \p k to the given output stream \p os
/// \returns an output stream with the VariableMonitoring written to
std::ostream& operator<<(std::ostream& os, const VariableMonitoring& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given MonitoringData \p k to a given json object \p j
void to_json(json& j, const MonitoringData& k) {
    // the required parts of the message
    j = json{
        {"component", k.component},
        {"variable", k.variable},
        {"variableMonitoring", k.variableMonitoring},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given MonitoringData \p k
void from_json(const json& j, MonitoringData& k) {
    // the required parts of the message
    k.component = j.at("component");
    k.variable = j.at("variable");
    for (auto val : j.at("variableMonitoring")) {
        k.variableMonitoring.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given MonitoringData \p k to the given output stream \p os
/// \returns an output stream with the MonitoringData written to
std::ostream& operator<<(std::ostream& os, const MonitoringData& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given VariableAttribute \p k to a given json object \p j
void to_json(json& j, const VariableAttribute& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.type) {
        j["type"] = conversions::attribute_enum_to_string(k.type.value());
    }
    if (k.value) {
        j["value"] = k.value.value();
    }
    if (k.mutability) {
        j["mutability"] = conversions::mutability_enum_to_string(k.mutability.value());
    }
    if (k.persistent) {
        j["persistent"] = k.persistent.value();
    }
    if (k.constant) {
        j["constant"] = k.constant.value();
    }
}

/// \brief Conversion from a given json object \p j to a given VariableAttribute \p k
void from_json(const json& j, VariableAttribute& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("type")) {
        k.type.emplace(conversions::string_to_attribute_enum(j.at("type")));
    }
    if (j.contains("value")) {
        k.value.emplace(j.at("value"));
    }
    if (j.contains("mutability")) {
        k.mutability.emplace(conversions::string_to_mutability_enum(j.at("mutability")));
    }
    if (j.contains("persistent")) {
        k.persistent.emplace(j.at("persistent"));
    }
    if (j.contains("constant")) {
        k.constant.emplace(j.at("constant"));
    }
}

// \brief Writes the string representation of the given VariableAttribute \p k to the given output stream \p os
/// \returns an output stream with the VariableAttribute written to
std::ostream& operator<<(std::ostream& os, const VariableAttribute& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given VariableCharacteristics \p k to a given json object \p j
void to_json(json& j, const VariableCharacteristics& k) {
    // the required parts of the message
    j = json{
        {"dataType", conversions::data_enum_to_string(k.dataType)},
        {"supportsMonitoring", k.supportsMonitoring},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.unit) {
        j["unit"] = k.unit.value();
    }
    if (k.minLimit) {
        j["minLimit"] = k.minLimit.value();
    }
    if (k.maxLimit) {
        j["maxLimit"] = k.maxLimit.value();
    }
    if (k.valuesList) {
        j["valuesList"] = k.valuesList.value();
    }
}

/// \brief Conversion from a given json object \p j to a given VariableCharacteristics \p k
void from_json(const json& j, VariableCharacteristics& k) {
    // the required parts of the message
    k.dataType = conversions::string_to_data_enum(j.at("dataType"));
    k.supportsMonitoring = j.at("supportsMonitoring");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("unit")) {
        k.unit.emplace(j.at("unit"));
    }
    if (j.contains("minLimit")) {
        k.minLimit.emplace(j.at("minLimit"));
    }
    if (j.contains("maxLimit")) {
        k.maxLimit.emplace(j.at("maxLimit"));
    }
    if (j.contains("valuesList")) {
        k.valuesList.emplace(j.at("valuesList"));
    }
}

// \brief Writes the string representation of the given VariableCharacteristics \p k to the given output stream \p os
/// \returns an output stream with the VariableCharacteristics written to
std::ostream& operator<<(std::ostream& os, const VariableCharacteristics& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ReportData \p k to a given json object \p j
void to_json(json& j, const ReportData& k) {
    // the required parts of the message
    j = json{
        {"component", k.component},
        {"variable", k.variable},
        {"variableAttribute", k.variableAttribute},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.variableCharacteristics) {
        j["variableCharacteristics"] = k.variableCharacteristics.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ReportData \p k
void from_json(const json& j, ReportData& k) {
    // the required parts of the message
    k.component = j.at("component");
    k.variable = j.at("variable");
    for (auto val : j.at("variableAttribute")) {
        k.variableAttribute.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("variableCharacteristics")) {
        k.variableCharacteristics.emplace(j.at("variableCharacteristics"));
    }
}

// \brief Writes the string representation of the given ReportData \p k to the given output stream \p os
/// \returns an output stream with the ReportData written to
std::ostream& operator<<(std::ostream& os, const ReportData& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ChargingProfile \p k to a given json object \p j
void to_json(json& j, const ChargingProfile& k) {
    // the required parts of the message
    j = json{
        {"id", k.id},
        {"stackLevel", k.stackLevel},
        {"chargingProfilePurpose", conversions::charging_profile_purpose_enum_to_string(k.chargingProfilePurpose)},
        {"chargingProfileKind", conversions::charging_profile_kind_enum_to_string(k.chargingProfileKind)},
        {"chargingSchedule", k.chargingSchedule},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.recurrencyKind) {
        j["recurrencyKind"] = conversions::recurrency_kind_enum_to_string(k.recurrencyKind.value());
    }
    if (k.validFrom) {
        j["validFrom"] = k.validFrom.value().to_rfc3339();
    }
    if (k.validTo) {
        j["validTo"] = k.validTo.value().to_rfc3339();
    }
    if (k.transactionId) {
        j["transactionId"] = k.transactionId.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingProfile \p k
void from_json(const json& j, ChargingProfile& k) {
    // the required parts of the message
    k.id = j.at("id");
    k.stackLevel = j.at("stackLevel");
    k.chargingProfilePurpose = conversions::string_to_charging_profile_purpose_enum(j.at("chargingProfilePurpose"));
    k.chargingProfileKind = conversions::string_to_charging_profile_kind_enum(j.at("chargingProfileKind"));
    for (auto val : j.at("chargingSchedule")) {
        k.chargingSchedule.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("recurrencyKind")) {
        k.recurrencyKind.emplace(conversions::string_to_recurrency_kind_enum(j.at("recurrencyKind")));
    }
    if (j.contains("validFrom")) {
        k.validFrom.emplace(j.at("validFrom").get<std::string>());
    }
    if (j.contains("validTo")) {
        k.validTo.emplace(j.at("validTo").get<std::string>());
    }
    if (j.contains("transactionId")) {
        k.transactionId.emplace(j.at("transactionId"));
    }
}

// \brief Writes the string representation of the given ChargingProfile \p k to the given output stream \p os
/// \returns an output stream with the ChargingProfile written to
std::ostream& operator<<(std::ostream& os, const ChargingProfile& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given AuthorizationData \p k to a given json object \p j
void to_json(json& j, const AuthorizationData& k) {
    // the required parts of the message
    j = json{
        {"idToken", k.idToken},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.idTokenInfo) {
        j["idTokenInfo"] = k.idTokenInfo.value();
    }
}

/// \brief Conversion from a given json object \p j to a given AuthorizationData \p k
void from_json(const json& j, AuthorizationData& k) {
    // the required parts of the message
    k.idToken = j.at("idToken");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("idTokenInfo")) {
        k.idTokenInfo.emplace(j.at("idTokenInfo"));
    }
}

// \brief Writes the string representation of the given AuthorizationData \p k to the given output stream \p os
/// \returns an output stream with the AuthorizationData written to
std::ostream& operator<<(std::ostream& os, const AuthorizationData& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given APN \p k to a given json object \p j
void to_json(json& j, const APN& k) {
    // the required parts of the message
    j = json{
        {"apn", k.apn},
        {"apnAuthentication", conversions::apnauthentication_enum_to_string(k.apnAuthentication)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.apnUserName) {
        j["apnUserName"] = k.apnUserName.value();
    }
    if (k.apnPassword) {
        j["apnPassword"] = k.apnPassword.value();
    }
    if (k.simPin) {
        j["simPin"] = k.simPin.value();
    }
    if (k.preferredNetwork) {
        j["preferredNetwork"] = k.preferredNetwork.value();
    }
    if (k.useOnlyPreferredNetwork) {
        j["useOnlyPreferredNetwork"] = k.useOnlyPreferredNetwork.value();
    }
}

/// \brief Conversion from a given json object \p j to a given APN \p k
void from_json(const json& j, APN& k) {
    // the required parts of the message
    k.apn = j.at("apn");
    k.apnAuthentication = conversions::string_to_apnauthentication_enum(j.at("apnAuthentication"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("apnUserName")) {
        k.apnUserName.emplace(j.at("apnUserName"));
    }
    if (j.contains("apnPassword")) {
        k.apnPassword.emplace(j.at("apnPassword"));
    }
    if (j.contains("simPin")) {
        k.simPin.emplace(j.at("simPin"));
    }
    if (j.contains("preferredNetwork")) {
        k.preferredNetwork.emplace(j.at("preferredNetwork"));
    }
    if (j.contains("useOnlyPreferredNetwork")) {
        k.useOnlyPreferredNetwork.emplace(j.at("useOnlyPreferredNetwork"));
    }
}

// \brief Writes the string representation of the given APN \p k to the given output stream \p os
/// \returns an output stream with the APN written to
std::ostream& operator<<(std::ostream& os, const APN& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given VPN \p k to a given json object \p j
void to_json(json& j, const VPN& k) {
    // the required parts of the message
    j = json{
        {"server", k.server},
        {"user", k.user},
        {"password", k.password},
        {"key", k.key},
        {"type", conversions::vpnenum_to_string(k.type)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.group) {
        j["group"] = k.group.value();
    }
}

/// \brief Conversion from a given json object \p j to a given VPN \p k
void from_json(const json& j, VPN& k) {
    // the required parts of the message
    k.server = j.at("server");
    k.user = j.at("user");
    k.password = j.at("password");
    k.key = j.at("key");
    k.type = conversions::string_to_vpnenum(j.at("type"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("group")) {
        k.group.emplace(j.at("group"));
    }
}

// \brief Writes the string representation of the given VPN \p k to the given output stream \p os
/// \returns an output stream with the VPN written to
std::ostream& operator<<(std::ostream& os, const VPN& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given NetworkConnectionProfile \p k to a given json object \p j
void to_json(json& j, const NetworkConnectionProfile& k) {
    // the required parts of the message
    j = json{
        {"ocppVersion", conversions::ocppversion_enum_to_string(k.ocppVersion)},
        {"ocppTransport", conversions::ocpptransport_enum_to_string(k.ocppTransport)},
        {"ocppCsmsUrl", k.ocppCsmsUrl},
        {"messageTimeout", k.messageTimeout},
        {"securityProfile", k.securityProfile},
        {"ocppInterface", conversions::ocppinterface_enum_to_string(k.ocppInterface)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.apn) {
        j["apn"] = k.apn.value();
    }
    if (k.vpn) {
        j["vpn"] = k.vpn.value();
    }
}

/// \brief Conversion from a given json object \p j to a given NetworkConnectionProfile \p k
void from_json(const json& j, NetworkConnectionProfile& k) {
    // the required parts of the message
    k.ocppVersion = conversions::string_to_ocppversion_enum(j.at("ocppVersion"));
    k.ocppTransport = conversions::string_to_ocpptransport_enum(j.at("ocppTransport"));
    k.ocppCsmsUrl = j.at("ocppCsmsUrl");
    k.messageTimeout = j.at("messageTimeout");
    k.securityProfile = j.at("securityProfile");
    k.ocppInterface = conversions::string_to_ocppinterface_enum(j.at("ocppInterface"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("apn")) {
        k.apn.emplace(j.at("apn"));
    }
    if (j.contains("vpn")) {
        k.vpn.emplace(j.at("vpn"));
    }
}

// \brief Writes the string representation of the given NetworkConnectionProfile \p k to the given output stream \p os
/// \returns an output stream with the NetworkConnectionProfile written to
std::ostream& operator<<(std::ostream& os, const NetworkConnectionProfile& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given SetMonitoringData \p k to a given json object \p j
void to_json(json& j, const SetMonitoringData& k) {
    // the required parts of the message
    j = json{
        {"value", k.value},       {"type", conversions::monitor_enum_to_string(k.type)},
        {"severity", k.severity}, {"component", k.component},
        {"variable", k.variable},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.id) {
        j["id"] = k.id.value();
    }
    if (k.transaction) {
        j["transaction"] = k.transaction.value();
    }
}

/// \brief Conversion from a given json object \p j to a given SetMonitoringData \p k
void from_json(const json& j, SetMonitoringData& k) {
    // the required parts of the message
    k.value = j.at("value");
    k.type = conversions::string_to_monitor_enum(j.at("type"));
    k.severity = j.at("severity");
    k.component = j.at("component");
    k.variable = j.at("variable");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("id")) {
        k.id.emplace(j.at("id"));
    }
    if (j.contains("transaction")) {
        k.transaction.emplace(j.at("transaction"));
    }
}

// \brief Writes the string representation of the given SetMonitoringData \p k to the given output stream \p os
/// \returns an output stream with the SetMonitoringData written to
std::ostream& operator<<(std::ostream& os, const SetMonitoringData& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given SetMonitoringResult \p k to a given json object \p j
void to_json(json& j, const SetMonitoringResult& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::set_monitoring_status_enum_to_string(k.status)},
        {"type", conversions::monitor_enum_to_string(k.type)},
        {"component", k.component},
        {"variable", k.variable},
        {"severity", k.severity},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.id) {
        j["id"] = k.id.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

/// \brief Conversion from a given json object \p j to a given SetMonitoringResult \p k
void from_json(const json& j, SetMonitoringResult& k) {
    // the required parts of the message
    k.status = conversions::string_to_set_monitoring_status_enum(j.at("status"));
    k.type = conversions::string_to_monitor_enum(j.at("type"));
    k.component = j.at("component");
    k.variable = j.at("variable");
    k.severity = j.at("severity");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("id")) {
        k.id.emplace(j.at("id"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

// \brief Writes the string representation of the given SetMonitoringResult \p k to the given output stream \p os
/// \returns an output stream with the SetMonitoringResult written to
std::ostream& operator<<(std::ostream& os, const SetMonitoringResult& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given SetVariableData \p k to a given json object \p j
void to_json(json& j, const SetVariableData& k) {
    // the required parts of the message
    j = json{
        {"attributeValue", k.attributeValue},
        {"component", k.component},
        {"variable", k.variable},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.attributeType) {
        j["attributeType"] = conversions::attribute_enum_to_string(k.attributeType.value());
    }
}

/// \brief Conversion from a given json object \p j to a given SetVariableData \p k
void from_json(const json& j, SetVariableData& k) {
    // the required parts of the message
    k.attributeValue = j.at("attributeValue");
    k.component = j.at("component");
    k.variable = j.at("variable");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("attributeType")) {
        k.attributeType.emplace(conversions::string_to_attribute_enum(j.at("attributeType")));
    }
}

// \brief Writes the string representation of the given SetVariableData \p k to the given output stream \p os
/// \returns an output stream with the SetVariableData written to
std::ostream& operator<<(std::ostream& os, const SetVariableData& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given SetVariableResult \p k to a given json object \p j
void to_json(json& j, const SetVariableResult& k) {
    // the required parts of the message
    j = json{
        {"attributeStatus", conversions::set_variable_status_enum_to_string(k.attributeStatus)},
        {"component", k.component},
        {"variable", k.variable},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.attributeType) {
        j["attributeType"] = conversions::attribute_enum_to_string(k.attributeType.value());
    }
    if (k.attributeStatusInfo) {
        j["attributeStatusInfo"] = k.attributeStatusInfo.value();
    }
}

/// \brief Conversion from a given json object \p j to a given SetVariableResult \p k
void from_json(const json& j, SetVariableResult& k) {
    // the required parts of the message
    k.attributeStatus = conversions::string_to_set_variable_status_enum(j.at("attributeStatus"));
    k.component = j.at("component");
    k.variable = j.at("variable");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("attributeType")) {
        k.attributeType.emplace(conversions::string_to_attribute_enum(j.at("attributeType")));
    }
    if (j.contains("attributeStatusInfo")) {
        k.attributeStatusInfo.emplace(j.at("attributeStatusInfo"));
    }
}

// \brief Writes the string representation of the given SetVariableResult \p k to the given output stream \p os
/// \returns an output stream with the SetVariableResult written to
std::ostream& operator<<(std::ostream& os, const SetVariableResult& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given Transaction \p k to a given json object \p j
void to_json(json& j, const Transaction& k) {
    // the required parts of the message
    j = json{
        {"transactionId", k.transactionId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.chargingState) {
        j["chargingState"] = conversions::charging_state_enum_to_string(k.chargingState.value());
    }
    if (k.timeSpentCharging) {
        j["timeSpentCharging"] = k.timeSpentCharging.value();
    }
    if (k.stoppedReason) {
        j["stoppedReason"] = conversions::reason_enum_to_string(k.stoppedReason.value());
    }
    if (k.remoteStartId) {
        j["remoteStartId"] = k.remoteStartId.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Transaction \p k
void from_json(const json& j, Transaction& k) {
    // the required parts of the message
    k.transactionId = j.at("transactionId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("chargingState")) {
        k.chargingState.emplace(conversions::string_to_charging_state_enum(j.at("chargingState")));
    }
    if (j.contains("timeSpentCharging")) {
        k.timeSpentCharging.emplace(j.at("timeSpentCharging"));
    }
    if (j.contains("stoppedReason")) {
        k.stoppedReason.emplace(conversions::string_to_reason_enum(j.at("stoppedReason")));
    }
    if (j.contains("remoteStartId")) {
        k.remoteStartId.emplace(j.at("remoteStartId"));
    }
}

// \brief Writes the string representation of the given Transaction \p k to the given output stream \p os
/// \returns an output stream with the Transaction written to
std::ostream& operator<<(std::ostream& os, const Transaction& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given Firmware \p k to a given json object \p j
void to_json(json& j, const Firmware& k) {
    // the required parts of the message
    j = json{
        {"location", k.location},
        {"retrieveDateTime", k.retrieveDateTime.to_rfc3339()},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.installDateTime) {
        j["installDateTime"] = k.installDateTime.value().to_rfc3339();
    }
    if (k.signingCertificate) {
        j["signingCertificate"] = k.signingCertificate.value();
    }
    if (k.signature) {
        j["signature"] = k.signature.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Firmware \p k
void from_json(const json& j, Firmware& k) {
    // the required parts of the message
    k.location = j.at("location");
    k.retrieveDateTime = ocpp::DateTime(std::string(j.at("retrieveDateTime")));
    ;

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("installDateTime")) {
        k.installDateTime.emplace(j.at("installDateTime").get<std::string>());
    }
    if (j.contains("signingCertificate")) {
        k.signingCertificate.emplace(j.at("signingCertificate"));
    }
    if (j.contains("signature")) {
        k.signature.emplace(j.at("signature"));
    }
}

// \brief Writes the string representation of the given Firmware \p k to the given output stream \p os
/// \returns an output stream with the Firmware written to
std::ostream& operator<<(std::ostream& os, const Firmware& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
