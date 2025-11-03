// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/ocpp_types.hpp>

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>

namespace ocpp {
namespace v2 {

/// \brief Conversion from a given StatusInfo \p k to a given json object \p j
void to_json(json& j, const StatusInfo& k) {
    // the required parts of the message
    j = json{
        {"reasonCode", k.reasonCode},
    };
    // the optional parts of the message
    if (k.additionalInfo) {
        j["additionalInfo"] = k.additionalInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given StatusInfo \p k
void from_json(const json& j, StatusInfo& k) {
    // the required parts of the message
    k.reasonCode = j.at("reasonCode");

    // the optional parts of the message
    if (j.contains("additionalInfo")) {
        k.additionalInfo.emplace(j.at("additionalInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given StatusInfo \p k to the given output stream \p os
/// \returns an output stream with the StatusInfo written to
std::ostream& operator<<(std::ostream& os, const StatusInfo& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given PeriodicEventStreamParams \p k to a given json object \p j
void to_json(json& j, const PeriodicEventStreamParams& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.interval) {
        j["interval"] = k.interval.value();
    }
    if (k.values) {
        j["values"] = k.values.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given PeriodicEventStreamParams \p k
void from_json(const json& j, PeriodicEventStreamParams& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("interval")) {
        k.interval.emplace(j.at("interval"));
    }
    if (j.contains("values")) {
        k.values.emplace(j.at("values"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given PeriodicEventStreamParams \p k to the given output stream \p os
/// \returns an output stream with the PeriodicEventStreamParams written to
std::ostream& operator<<(std::ostream& os, const PeriodicEventStreamParams& k) {
    os << json(k).dump(4);
    return os;
}

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
        {"type", k.type},
    };
    // the optional parts of the message
    if (k.additionalInfo) {
        j["additionalInfo"] = json::array();
        for (auto val : k.additionalInfo.value()) {
            j["additionalInfo"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given IdToken \p k
void from_json(const json& j, IdToken& k) {
    // the required parts of the message
    k.idToken = j.at("idToken");
    k.type = j.at("type");

    // the optional parts of the message
    if (j.contains("additionalInfo")) {
        json arr = j.at("additionalInfo");
        std::vector<AdditionalInfo> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.additionalInfo.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.language) {
        j["language"] = k.language.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given MessageContent \p k
void from_json(const json& j, MessageContent& k) {
    // the required parts of the message
    k.format = conversions::string_to_message_format_enum(j.at("format"));
    k.content = j.at("content");

    // the optional parts of the message
    if (j.contains("language")) {
        k.language.emplace(j.at("language"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.cacheExpiryDateTime) {
        j["cacheExpiryDateTime"] = k.cacheExpiryDateTime.value().to_rfc3339();
    }
    if (k.chargingPriority) {
        j["chargingPriority"] = k.chargingPriority.value();
    }
    if (k.groupIdToken) {
        j["groupIdToken"] = k.groupIdToken.value();
    }
    if (k.language1) {
        j["language1"] = k.language1.value();
    }
    if (k.language2) {
        j["language2"] = k.language2.value();
    }
    if (k.evseId) {
        j["evseId"] = json::array();
        for (auto val : k.evseId.value()) {
            j["evseId"].push_back(val);
        }
    }
    if (k.personalMessage) {
        j["personalMessage"] = k.personalMessage.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given IdTokenInfo \p k
void from_json(const json& j, IdTokenInfo& k) {
    // the required parts of the message
    k.status = conversions::string_to_authorization_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("cacheExpiryDateTime")) {
        k.cacheExpiryDateTime.emplace(j.at("cacheExpiryDateTime").get<std::string>());
    }
    if (j.contains("chargingPriority")) {
        k.chargingPriority.emplace(j.at("chargingPriority"));
    }
    if (j.contains("groupIdToken")) {
        k.groupIdToken.emplace(j.at("groupIdToken"));
    }
    if (j.contains("language1")) {
        k.language1.emplace(j.at("language1"));
    }
    if (j.contains("language2")) {
        k.language2.emplace(j.at("language2"));
    }
    if (j.contains("evseId")) {
        json arr = j.at("evseId");
        std::vector<int32_t> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.evseId.emplace(vec);
    }
    if (j.contains("personalMessage")) {
        k.personalMessage.emplace(j.at("personalMessage"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given IdTokenInfo \p k to the given output stream \p os
/// \returns an output stream with the IdTokenInfo written to
std::ostream& operator<<(std::ostream& os, const IdTokenInfo& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TariffConditions \p k to a given json object \p j
void to_json(json& j, const TariffConditions& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.startTimeOfDay) {
        j["startTimeOfDay"] = k.startTimeOfDay.value();
    }
    if (k.endTimeOfDay) {
        j["endTimeOfDay"] = k.endTimeOfDay.value();
    }
    if (k.dayOfWeek) {
        if (j.size() == 0) {
            j = json{{"dayOfWeek", json::array()}};
        } else {
            j["dayOfWeek"] = json::array();
        }
        for (auto val : k.dayOfWeek.value()) {
            j["dayOfWeek"].push_back(conversions::day_of_week_enum_to_string(val));
        }
    }
    if (k.validFromDate) {
        j["validFromDate"] = k.validFromDate.value();
    }
    if (k.validToDate) {
        j["validToDate"] = k.validToDate.value();
    }
    if (k.evseKind) {
        j["evseKind"] = conversions::evse_kind_enum_to_string(k.evseKind.value());
    }
    if (k.minEnergy) {
        j["minEnergy"] = k.minEnergy.value();
    }
    if (k.maxEnergy) {
        j["maxEnergy"] = k.maxEnergy.value();
    }
    if (k.minCurrent) {
        j["minCurrent"] = k.minCurrent.value();
    }
    if (k.maxCurrent) {
        j["maxCurrent"] = k.maxCurrent.value();
    }
    if (k.minPower) {
        j["minPower"] = k.minPower.value();
    }
    if (k.maxPower) {
        j["maxPower"] = k.maxPower.value();
    }
    if (k.minTime) {
        j["minTime"] = k.minTime.value();
    }
    if (k.maxTime) {
        j["maxTime"] = k.maxTime.value();
    }
    if (k.minChargingTime) {
        j["minChargingTime"] = k.minChargingTime.value();
    }
    if (k.maxChargingTime) {
        j["maxChargingTime"] = k.maxChargingTime.value();
    }
    if (k.minIdleTime) {
        j["minIdleTime"] = k.minIdleTime.value();
    }
    if (k.maxIdleTime) {
        j["maxIdleTime"] = k.maxIdleTime.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TariffConditions \p k
void from_json(const json& j, TariffConditions& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("startTimeOfDay")) {
        k.startTimeOfDay.emplace(j.at("startTimeOfDay"));
    }
    if (j.contains("endTimeOfDay")) {
        k.endTimeOfDay.emplace(j.at("endTimeOfDay"));
    }
    if (j.contains("dayOfWeek")) {
        json arr = j.at("dayOfWeek");
        std::vector<DayOfWeekEnum> vec;
        for (auto val : arr) {
            vec.push_back(conversions::string_to_day_of_week_enum(val));
        }
        k.dayOfWeek.emplace(vec);
    }
    if (j.contains("validFromDate")) {
        k.validFromDate.emplace(j.at("validFromDate"));
    }
    if (j.contains("validToDate")) {
        k.validToDate.emplace(j.at("validToDate"));
    }
    if (j.contains("evseKind")) {
        k.evseKind.emplace(conversions::string_to_evse_kind_enum(j.at("evseKind")));
    }
    if (j.contains("minEnergy")) {
        k.minEnergy.emplace(j.at("minEnergy"));
    }
    if (j.contains("maxEnergy")) {
        k.maxEnergy.emplace(j.at("maxEnergy"));
    }
    if (j.contains("minCurrent")) {
        k.minCurrent.emplace(j.at("minCurrent"));
    }
    if (j.contains("maxCurrent")) {
        k.maxCurrent.emplace(j.at("maxCurrent"));
    }
    if (j.contains("minPower")) {
        k.minPower.emplace(j.at("minPower"));
    }
    if (j.contains("maxPower")) {
        k.maxPower.emplace(j.at("maxPower"));
    }
    if (j.contains("minTime")) {
        k.minTime.emplace(j.at("minTime"));
    }
    if (j.contains("maxTime")) {
        k.maxTime.emplace(j.at("maxTime"));
    }
    if (j.contains("minChargingTime")) {
        k.minChargingTime.emplace(j.at("minChargingTime"));
    }
    if (j.contains("maxChargingTime")) {
        k.maxChargingTime.emplace(j.at("maxChargingTime"));
    }
    if (j.contains("minIdleTime")) {
        k.minIdleTime.emplace(j.at("minIdleTime"));
    }
    if (j.contains("maxIdleTime")) {
        k.maxIdleTime.emplace(j.at("maxIdleTime"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TariffConditions \p k to the given output stream \p os
/// \returns an output stream with the TariffConditions written to
std::ostream& operator<<(std::ostream& os, const TariffConditions& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TariffEnergyPrice \p k to a given json object \p j
void to_json(json& j, const TariffEnergyPrice& k) {
    // the required parts of the message
    j = json{
        {"priceKwh", k.priceKwh},
    };
    // the optional parts of the message
    if (k.conditions) {
        j["conditions"] = k.conditions.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TariffEnergyPrice \p k
void from_json(const json& j, TariffEnergyPrice& k) {
    // the required parts of the message
    k.priceKwh = j.at("priceKwh");

    // the optional parts of the message
    if (j.contains("conditions")) {
        k.conditions.emplace(j.at("conditions"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TariffEnergyPrice \p k to the given output stream \p os
/// \returns an output stream with the TariffEnergyPrice written to
std::ostream& operator<<(std::ostream& os, const TariffEnergyPrice& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TaxRate \p k to a given json object \p j
void to_json(json& j, const TaxRate& k) {
    // the required parts of the message
    j = json{
        {"type", k.type},
        {"tax", k.tax},
    };
    // the optional parts of the message
    if (k.stack) {
        j["stack"] = k.stack.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TaxRate \p k
void from_json(const json& j, TaxRate& k) {
    // the required parts of the message
    k.type = j.at("type");
    k.tax = j.at("tax");

    // the optional parts of the message
    if (j.contains("stack")) {
        k.stack.emplace(j.at("stack"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TaxRate \p k to the given output stream \p os
/// \returns an output stream with the TaxRate written to
std::ostream& operator<<(std::ostream& os, const TaxRate& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TariffEnergy \p k to a given json object \p j
void to_json(json& j, const TariffEnergy& k) {
    // the required parts of the message
    j = json{
        {"prices", k.prices},
    };
    // the optional parts of the message
    if (k.taxRates) {
        j["taxRates"] = json::array();
        for (auto val : k.taxRates.value()) {
            j["taxRates"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TariffEnergy \p k
void from_json(const json& j, TariffEnergy& k) {
    // the required parts of the message
    for (auto val : j.at("prices")) {
        k.prices.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("taxRates")) {
        json arr = j.at("taxRates");
        std::vector<TaxRate> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.taxRates.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TariffEnergy \p k to the given output stream \p os
/// \returns an output stream with the TariffEnergy written to
std::ostream& operator<<(std::ostream& os, const TariffEnergy& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TariffTimePrice \p k to a given json object \p j
void to_json(json& j, const TariffTimePrice& k) {
    // the required parts of the message
    j = json{
        {"priceMinute", k.priceMinute},
    };
    // the optional parts of the message
    if (k.conditions) {
        j["conditions"] = k.conditions.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TariffTimePrice \p k
void from_json(const json& j, TariffTimePrice& k) {
    // the required parts of the message
    k.priceMinute = j.at("priceMinute");

    // the optional parts of the message
    if (j.contains("conditions")) {
        k.conditions.emplace(j.at("conditions"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TariffTimePrice \p k to the given output stream \p os
/// \returns an output stream with the TariffTimePrice written to
std::ostream& operator<<(std::ostream& os, const TariffTimePrice& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TariffTime \p k to a given json object \p j
void to_json(json& j, const TariffTime& k) {
    // the required parts of the message
    j = json{
        {"prices", k.prices},
    };
    // the optional parts of the message
    if (k.taxRates) {
        j["taxRates"] = json::array();
        for (auto val : k.taxRates.value()) {
            j["taxRates"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TariffTime \p k
void from_json(const json& j, TariffTime& k) {
    // the required parts of the message
    for (auto val : j.at("prices")) {
        k.prices.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("taxRates")) {
        json arr = j.at("taxRates");
        std::vector<TaxRate> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.taxRates.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TariffTime \p k to the given output stream \p os
/// \returns an output stream with the TariffTime written to
std::ostream& operator<<(std::ostream& os, const TariffTime& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TariffConditionsFixed \p k to a given json object \p j
void to_json(json& j, const TariffConditionsFixed& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.startTimeOfDay) {
        j["startTimeOfDay"] = k.startTimeOfDay.value();
    }
    if (k.endTimeOfDay) {
        j["endTimeOfDay"] = k.endTimeOfDay.value();
    }
    if (k.dayOfWeek) {
        if (j.size() == 0) {
            j = json{{"dayOfWeek", json::array()}};
        } else {
            j["dayOfWeek"] = json::array();
        }
        for (auto val : k.dayOfWeek.value()) {
            j["dayOfWeek"].push_back(conversions::day_of_week_enum_to_string(val));
        }
    }
    if (k.validFromDate) {
        j["validFromDate"] = k.validFromDate.value();
    }
    if (k.validToDate) {
        j["validToDate"] = k.validToDate.value();
    }
    if (k.evseKind) {
        j["evseKind"] = conversions::evse_kind_enum_to_string(k.evseKind.value());
    }
    if (k.paymentBrand) {
        j["paymentBrand"] = k.paymentBrand.value();
    }
    if (k.paymentRecognition) {
        j["paymentRecognition"] = k.paymentRecognition.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TariffConditionsFixed \p k
void from_json(const json& j, TariffConditionsFixed& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("startTimeOfDay")) {
        k.startTimeOfDay.emplace(j.at("startTimeOfDay"));
    }
    if (j.contains("endTimeOfDay")) {
        k.endTimeOfDay.emplace(j.at("endTimeOfDay"));
    }
    if (j.contains("dayOfWeek")) {
        json arr = j.at("dayOfWeek");
        std::vector<DayOfWeekEnum> vec;
        for (auto val : arr) {
            vec.push_back(conversions::string_to_day_of_week_enum(val));
        }
        k.dayOfWeek.emplace(vec);
    }
    if (j.contains("validFromDate")) {
        k.validFromDate.emplace(j.at("validFromDate"));
    }
    if (j.contains("validToDate")) {
        k.validToDate.emplace(j.at("validToDate"));
    }
    if (j.contains("evseKind")) {
        k.evseKind.emplace(conversions::string_to_evse_kind_enum(j.at("evseKind")));
    }
    if (j.contains("paymentBrand")) {
        k.paymentBrand.emplace(j.at("paymentBrand"));
    }
    if (j.contains("paymentRecognition")) {
        k.paymentRecognition.emplace(j.at("paymentRecognition"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TariffConditionsFixed \p k to the given output stream \p os
/// \returns an output stream with the TariffConditionsFixed written to
std::ostream& operator<<(std::ostream& os, const TariffConditionsFixed& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TariffFixedPrice \p k to a given json object \p j
void to_json(json& j, const TariffFixedPrice& k) {
    // the required parts of the message
    j = json{
        {"priceFixed", k.priceFixed},
    };
    // the optional parts of the message
    if (k.conditions) {
        j["conditions"] = k.conditions.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TariffFixedPrice \p k
void from_json(const json& j, TariffFixedPrice& k) {
    // the required parts of the message
    k.priceFixed = j.at("priceFixed");

    // the optional parts of the message
    if (j.contains("conditions")) {
        k.conditions.emplace(j.at("conditions"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TariffFixedPrice \p k to the given output stream \p os
/// \returns an output stream with the TariffFixedPrice written to
std::ostream& operator<<(std::ostream& os, const TariffFixedPrice& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TariffFixed \p k to a given json object \p j
void to_json(json& j, const TariffFixed& k) {
    // the required parts of the message
    j = json{
        {"prices", k.prices},
    };
    // the optional parts of the message
    if (k.taxRates) {
        j["taxRates"] = json::array();
        for (auto val : k.taxRates.value()) {
            j["taxRates"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TariffFixed \p k
void from_json(const json& j, TariffFixed& k) {
    // the required parts of the message
    for (auto val : j.at("prices")) {
        k.prices.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("taxRates")) {
        json arr = j.at("taxRates");
        std::vector<TaxRate> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.taxRates.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TariffFixed \p k to the given output stream \p os
/// \returns an output stream with the TariffFixed written to
std::ostream& operator<<(std::ostream& os, const TariffFixed& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given Price \p k to a given json object \p j
void to_json(json& j, const Price& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.exclTax) {
        j["exclTax"] = k.exclTax.value();
    }
    if (k.inclTax) {
        j["inclTax"] = k.inclTax.value();
    }
    if (k.taxRates) {
        if (j.size() == 0) {
            j = json{{"taxRates", json::array()}};
        } else {
            j["taxRates"] = json::array();
        }
        for (auto val : k.taxRates.value()) {
            j["taxRates"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Price \p k
void from_json(const json& j, Price& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("exclTax")) {
        k.exclTax.emplace(j.at("exclTax"));
    }
    if (j.contains("inclTax")) {
        k.inclTax.emplace(j.at("inclTax"));
    }
    if (j.contains("taxRates")) {
        json arr = j.at("taxRates");
        std::vector<TaxRate> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.taxRates.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given Price \p k to the given output stream \p os
/// \returns an output stream with the Price written to
std::ostream& operator<<(std::ostream& os, const Price& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given Tariff \p k to a given json object \p j
void to_json(json& j, const Tariff& k) {
    // the required parts of the message
    j = json{
        {"tariffId", k.tariffId},
        {"currency", k.currency},
    };
    // the optional parts of the message
    if (k.description) {
        j["description"] = json::array();
        for (auto val : k.description.value()) {
            j["description"].push_back(val);
        }
    }
    if (k.energy) {
        j["energy"] = k.energy.value();
    }
    if (k.validFrom) {
        j["validFrom"] = k.validFrom.value().to_rfc3339();
    }
    if (k.chargingTime) {
        j["chargingTime"] = k.chargingTime.value();
    }
    if (k.idleTime) {
        j["idleTime"] = k.idleTime.value();
    }
    if (k.fixedFee) {
        j["fixedFee"] = k.fixedFee.value();
    }
    if (k.reservationTime) {
        j["reservationTime"] = k.reservationTime.value();
    }
    if (k.reservationFixed) {
        j["reservationFixed"] = k.reservationFixed.value();
    }
    if (k.minCost) {
        j["minCost"] = k.minCost.value();
    }
    if (k.maxCost) {
        j["maxCost"] = k.maxCost.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Tariff \p k
void from_json(const json& j, Tariff& k) {
    // the required parts of the message
    k.tariffId = j.at("tariffId");
    k.currency = j.at("currency");

    // the optional parts of the message
    if (j.contains("description")) {
        json arr = j.at("description");
        std::vector<MessageContent> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.description.emplace(vec);
    }
    if (j.contains("energy")) {
        k.energy.emplace(j.at("energy"));
    }
    if (j.contains("validFrom")) {
        k.validFrom.emplace(j.at("validFrom").get<std::string>());
    }
    if (j.contains("chargingTime")) {
        k.chargingTime.emplace(j.at("chargingTime"));
    }
    if (j.contains("idleTime")) {
        k.idleTime.emplace(j.at("idleTime"));
    }
    if (j.contains("fixedFee")) {
        k.fixedFee.emplace(j.at("fixedFee"));
    }
    if (j.contains("reservationTime")) {
        k.reservationTime.emplace(j.at("reservationTime"));
    }
    if (j.contains("reservationFixed")) {
        k.reservationFixed.emplace(j.at("reservationFixed"));
    }
    if (j.contains("minCost")) {
        k.minCost.emplace(j.at("minCost"));
    }
    if (j.contains("maxCost")) {
        k.maxCost.emplace(j.at("maxCost"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given Tariff \p k to the given output stream \p os
/// \returns an output stream with the Tariff written to
std::ostream& operator<<(std::ostream& os, const Tariff& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given BatteryData \p k to a given json object \p j
void to_json(json& j, const BatteryData& k) {
    // the required parts of the message
    j = json{
        {"evseId", k.evseId},
        {"serialNumber", k.serialNumber},
        {"soC", k.soC},
        {"soH", k.soH},
    };
    // the optional parts of the message
    if (k.productionDate) {
        j["productionDate"] = k.productionDate.value().to_rfc3339();
    }
    if (k.vendorInfo) {
        j["vendorInfo"] = k.vendorInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given BatteryData \p k
void from_json(const json& j, BatteryData& k) {
    // the required parts of the message
    k.evseId = j.at("evseId");
    k.serialNumber = j.at("serialNumber");
    k.soC = j.at("soC");
    k.soH = j.at("soH");

    // the optional parts of the message
    if (j.contains("productionDate")) {
        k.productionDate.emplace(j.at("productionDate").get<std::string>());
    }
    if (j.contains("vendorInfo")) {
        k.vendorInfo.emplace(j.at("vendorInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given BatteryData \p k to the given output stream \p os
/// \returns an output stream with the BatteryData written to
std::ostream& operator<<(std::ostream& os, const BatteryData& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given Modem \p k to a given json object \p j
void to_json(json& j, const Modem& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.iccid) {
        j["iccid"] = k.iccid.value();
    }
    if (k.imsi) {
        j["imsi"] = k.imsi.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Modem \p k
void from_json(const json& j, Modem& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("iccid")) {
        k.iccid.emplace(j.at("iccid"));
    }
    if (j.contains("imsi")) {
        k.imsi.emplace(j.at("imsi"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.serialNumber) {
        j["serialNumber"] = k.serialNumber.value();
    }
    if (k.modem) {
        j["modem"] = k.modem.value();
    }
    if (k.firmwareVersion) {
        j["firmwareVersion"] = k.firmwareVersion.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingStation \p k
void from_json(const json& j, ChargingStation& k) {
    // the required parts of the message
    k.model = j.at("model");
    k.vendorName = j.at("vendorName");

    // the optional parts of the message
    if (j.contains("serialNumber")) {
        k.serialNumber.emplace(j.at("serialNumber"));
    }
    if (j.contains("modem")) {
        k.modem.emplace(j.at("modem"));
    }
    if (j.contains("firmwareVersion")) {
        k.firmwareVersion.emplace(j.at("firmwareVersion"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given ChargingStation \p k to the given output stream \p os
/// \returns an output stream with the ChargingStation written to
std::ostream& operator<<(std::ostream& os, const ChargingStation& k) {
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
    if (k.connectorId) {
        j["connectorId"] = k.connectorId.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given EVSE \p k
void from_json(const json& j, EVSE& k) {
    // the required parts of the message
    k.id = j.at("id");

    // the optional parts of the message
    if (j.contains("connectorId")) {
        k.connectorId.emplace(j.at("connectorId"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ClearChargingProfile \p k
void from_json(const json& j, ClearChargingProfile& k) {
    // the required parts of the message

    // the optional parts of the message
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
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given ClearChargingProfile \p k to the given output stream \p os
/// \returns an output stream with the ClearChargingProfile written to
std::ostream& operator<<(std::ostream& os, const ClearChargingProfile& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ClearTariffsResult \p k to a given json object \p j
void to_json(json& j, const ClearTariffsResult& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::tariff_clear_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.tariffId) {
        j["tariffId"] = k.tariffId.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ClearTariffsResult \p k
void from_json(const json& j, ClearTariffsResult& k) {
    // the required parts of the message
    k.status = conversions::string_to_tariff_clear_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("tariffId")) {
        k.tariffId.emplace(j.at("tariffId"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given ClearTariffsResult \p k to the given output stream \p os
/// \returns an output stream with the ClearTariffsResult written to
std::ostream& operator<<(std::ostream& os, const ClearTariffsResult& k) {
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
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ClearMonitoringResult \p k
void from_json(const json& j, ClearMonitoringResult& k) {
    // the required parts of the message
    k.status = conversions::string_to_clear_monitoring_status_enum(j.at("status"));
    k.id = j.at("id");

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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

/// \brief Conversion from a given CertificateStatusRequestInfo \p k to a given json object \p j
void to_json(json& j, const CertificateStatusRequestInfo& k) {
    // the required parts of the message
    j = json{
        {"certificateHashData", k.certificateHashData},
        {"source", conversions::certificate_status_source_enum_to_string(k.source)},
        {"urls", k.urls},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given CertificateStatusRequestInfo \p k
void from_json(const json& j, CertificateStatusRequestInfo& k) {
    // the required parts of the message
    k.certificateHashData = j.at("certificateHashData");
    k.source = conversions::string_to_certificate_status_source_enum(j.at("source"));
    for (auto val : j.at("urls")) {
        k.urls.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given CertificateStatusRequestInfo \p k to the given output stream \p
// os
/// \returns an output stream with the CertificateStatusRequestInfo written to
std::ostream& operator<<(std::ostream& os, const CertificateStatusRequestInfo& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given CertificateStatus \p k to a given json object \p j
void to_json(json& j, const CertificateStatus& k) {
    // the required parts of the message
    j = json{
        {"certificateHashData", k.certificateHashData},
        {"source", conversions::certificate_status_source_enum_to_string(k.source)},
        {"status", conversions::certificate_status_enum_to_string(k.status)},
        {"nextUpdate", k.nextUpdate.to_rfc3339()},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given CertificateStatus \p k
void from_json(const json& j, CertificateStatus& k) {
    // the required parts of the message
    k.certificateHashData = j.at("certificateHashData");
    k.source = conversions::string_to_certificate_status_source_enum(j.at("source"));
    k.status = conversions::string_to_certificate_status_enum(j.at("status"));
    k.nextUpdate = ocpp::DateTime(std::string(j.at("nextUpdate")));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given CertificateStatus \p k to the given output stream \p os
/// \returns an output stream with the CertificateStatus written to
std::ostream& operator<<(std::ostream& os, const CertificateStatus& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ChargingProfileCriterion \p k to a given json object \p j
void to_json(json& j, const ChargingProfileCriterion& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
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
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingProfileCriterion \p k
void from_json(const json& j, ChargingProfileCriterion& k) {
    // the required parts of the message

    // the optional parts of the message
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
        std::vector<CiString<20>> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.chargingLimitSource.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given ChargingProfileCriterion \p k to the given output stream \p os
/// \returns an output stream with the ChargingProfileCriterion written to
std::ostream& operator<<(std::ostream& os, const ChargingProfileCriterion& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given V2XFreqWattPoint \p k to a given json object \p j
void to_json(json& j, const V2XFreqWattPoint& k) {
    // the required parts of the message
    j = json{
        {"frequency", k.frequency},
        {"power", k.power},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given V2XFreqWattPoint \p k
void from_json(const json& j, V2XFreqWattPoint& k) {
    // the required parts of the message
    k.frequency = j.at("frequency");
    k.power = j.at("power");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given V2XFreqWattPoint \p k to the given output stream \p os
/// \returns an output stream with the V2XFreqWattPoint written to
std::ostream& operator<<(std::ostream& os, const V2XFreqWattPoint& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given V2XSignalWattPoint \p k to a given json object \p j
void to_json(json& j, const V2XSignalWattPoint& k) {
    // the required parts of the message
    j = json{
        {"signal", k.signal},
        {"power", k.power},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given V2XSignalWattPoint \p k
void from_json(const json& j, V2XSignalWattPoint& k) {
    // the required parts of the message
    k.signal = j.at("signal");
    k.power = j.at("power");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given V2XSignalWattPoint \p k to the given output stream \p os
/// \returns an output stream with the V2XSignalWattPoint written to
std::ostream& operator<<(std::ostream& os, const V2XSignalWattPoint& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ChargingSchedulePeriod \p k to a given json object \p j
void to_json(json& j, const ChargingSchedulePeriod& k) {
    // the required parts of the message
    j = json{
        {"startPeriod", k.startPeriod},
    };
    // the optional parts of the message
    if (k.limit) {
        j["limit"] = k.limit.value();
    }
    if (k.limit_L2) {
        j["limit_L2"] = k.limit_L2.value();
    }
    if (k.limit_L3) {
        j["limit_L3"] = k.limit_L3.value();
    }
    if (k.numberPhases) {
        j["numberPhases"] = k.numberPhases.value();
    }
    if (k.phaseToUse) {
        j["phaseToUse"] = k.phaseToUse.value();
    }
    if (k.dischargeLimit) {
        j["dischargeLimit"] = k.dischargeLimit.value();
    }
    if (k.dischargeLimit_L2) {
        j["dischargeLimit_L2"] = k.dischargeLimit_L2.value();
    }
    if (k.dischargeLimit_L3) {
        j["dischargeLimit_L3"] = k.dischargeLimit_L3.value();
    }
    if (k.setpoint) {
        j["setpoint"] = k.setpoint.value();
    }
    if (k.setpoint_L2) {
        j["setpoint_L2"] = k.setpoint_L2.value();
    }
    if (k.setpoint_L3) {
        j["setpoint_L3"] = k.setpoint_L3.value();
    }
    if (k.setpointReactive) {
        j["setpointReactive"] = k.setpointReactive.value();
    }
    if (k.setpointReactive_L2) {
        j["setpointReactive_L2"] = k.setpointReactive_L2.value();
    }
    if (k.setpointReactive_L3) {
        j["setpointReactive_L3"] = k.setpointReactive_L3.value();
    }
    if (k.preconditioningRequest) {
        j["preconditioningRequest"] = k.preconditioningRequest.value();
    }
    if (k.evseSleep) {
        j["evseSleep"] = k.evseSleep.value();
    }
    if (k.v2xBaseline) {
        j["v2xBaseline"] = k.v2xBaseline.value();
    }
    if (k.operationMode) {
        j["operationMode"] = conversions::operation_mode_enum_to_string(k.operationMode.value());
    }
    if (k.v2xFreqWattCurve) {
        j["v2xFreqWattCurve"] = json::array();
        for (auto val : k.v2xFreqWattCurve.value()) {
            j["v2xFreqWattCurve"].push_back(val);
        }
    }
    if (k.v2xSignalWattCurve) {
        j["v2xSignalWattCurve"] = json::array();
        for (auto val : k.v2xSignalWattCurve.value()) {
            j["v2xSignalWattCurve"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingSchedulePeriod \p k
void from_json(const json& j, ChargingSchedulePeriod& k) {
    // the required parts of the message
    k.startPeriod = j.at("startPeriod");

    // the optional parts of the message
    if (j.contains("limit")) {
        k.limit.emplace(j.at("limit"));
    }
    if (j.contains("limit_L2")) {
        k.limit_L2.emplace(j.at("limit_L2"));
    }
    if (j.contains("limit_L3")) {
        k.limit_L3.emplace(j.at("limit_L3"));
    }
    if (j.contains("numberPhases")) {
        k.numberPhases.emplace(j.at("numberPhases"));
    }
    if (j.contains("phaseToUse")) {
        k.phaseToUse.emplace(j.at("phaseToUse"));
    }
    if (j.contains("dischargeLimit")) {
        k.dischargeLimit.emplace(j.at("dischargeLimit"));
    }
    if (j.contains("dischargeLimit_L2")) {
        k.dischargeLimit_L2.emplace(j.at("dischargeLimit_L2"));
    }
    if (j.contains("dischargeLimit_L3")) {
        k.dischargeLimit_L3.emplace(j.at("dischargeLimit_L3"));
    }
    if (j.contains("setpoint")) {
        k.setpoint.emplace(j.at("setpoint"));
    }
    if (j.contains("setpoint_L2")) {
        k.setpoint_L2.emplace(j.at("setpoint_L2"));
    }
    if (j.contains("setpoint_L3")) {
        k.setpoint_L3.emplace(j.at("setpoint_L3"));
    }
    if (j.contains("setpointReactive")) {
        k.setpointReactive.emplace(j.at("setpointReactive"));
    }
    if (j.contains("setpointReactive_L2")) {
        k.setpointReactive_L2.emplace(j.at("setpointReactive_L2"));
    }
    if (j.contains("setpointReactive_L3")) {
        k.setpointReactive_L3.emplace(j.at("setpointReactive_L3"));
    }
    if (j.contains("preconditioningRequest")) {
        k.preconditioningRequest.emplace(j.at("preconditioningRequest"));
    }
    if (j.contains("evseSleep")) {
        k.evseSleep.emplace(j.at("evseSleep"));
    }
    if (j.contains("v2xBaseline")) {
        k.v2xBaseline.emplace(j.at("v2xBaseline"));
    }
    if (j.contains("operationMode")) {
        k.operationMode.emplace(conversions::string_to_operation_mode_enum(j.at("operationMode")));
    }
    if (j.contains("v2xFreqWattCurve")) {
        json arr = j.at("v2xFreqWattCurve");
        std::vector<V2XFreqWattPoint> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.v2xFreqWattCurve.emplace(vec);
    }
    if (j.contains("v2xSignalWattCurve")) {
        json arr = j.at("v2xSignalWattCurve");
        std::vector<V2XSignalWattPoint> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.v2xSignalWattCurve.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
        {"evseId", k.evseId},
        {"duration", k.duration},
        {"scheduleStart", k.scheduleStart.to_rfc3339()},
        {"chargingRateUnit", conversions::charging_rate_unit_enum_to_string(k.chargingRateUnit)},
        {"chargingSchedulePeriod", k.chargingSchedulePeriod},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given CompositeSchedule \p k
void from_json(const json& j, CompositeSchedule& k) {
    // the required parts of the message
    k.evseId = j.at("evseId");
    k.duration = j.at("duration");
    k.scheduleStart = ocpp::DateTime(std::string(j.at("scheduleStart")));
    k.chargingRateUnit = conversions::string_to_charging_rate_unit_enum(j.at("chargingRateUnit"));
    for (auto val : j.at("chargingSchedulePeriod")) {
        k.chargingSchedulePeriod.push_back(val);
    }

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
    if (k.childCertificateHashData) {
        j["childCertificateHashData"] = json::array();
        for (auto val : k.childCertificateHashData.value()) {
            j["childCertificateHashData"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given CertificateHashDataChain \p k
void from_json(const json& j, CertificateHashDataChain& k) {
    // the required parts of the message
    k.certificateHashData = j.at("certificateHashData");
    k.certificateType = conversions::string_to_get_certificate_id_use_enum(j.at("certificateType"));

    // the optional parts of the message
    if (j.contains("childCertificateHashData")) {
        json arr = j.at("childCertificateHashData");
        std::vector<CertificateHashDataType> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.childCertificateHashData.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.oldestTimestamp) {
        j["oldestTimestamp"] = k.oldestTimestamp.value().to_rfc3339();
    }
    if (k.latestTimestamp) {
        j["latestTimestamp"] = k.latestTimestamp.value().to_rfc3339();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given LogParameters \p k
void from_json(const json& j, LogParameters& k) {
    // the required parts of the message
    k.remoteLocation = j.at("remoteLocation");

    // the optional parts of the message
    if (j.contains("oldestTimestamp")) {
        k.oldestTimestamp.emplace(j.at("oldestTimestamp").get<std::string>());
    }
    if (j.contains("latestTimestamp")) {
        k.latestTimestamp.emplace(j.at("latestTimestamp").get<std::string>());
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.evse) {
        j["evse"] = k.evse.value();
    }
    if (k.instance) {
        j["instance"] = k.instance.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Component \p k
void from_json(const json& j, Component& k) {
    // the required parts of the message
    k.name = j.at("name");

    // the optional parts of the message
    if (j.contains("evse")) {
        k.evse.emplace(j.at("evse"));
    }
    if (j.contains("instance")) {
        k.instance.emplace(j.at("instance"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.instance) {
        j["instance"] = k.instance.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Variable \p k
void from_json(const json& j, Variable& k) {
    // the required parts of the message
    k.name = j.at("name");

    // the optional parts of the message
    if (j.contains("instance")) {
        k.instance.emplace(j.at("instance"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.variable) {
        j["variable"] = k.variable.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ComponentVariable \p k
void from_json(const json& j, ComponentVariable& k) {
    // the required parts of the message
    k.component = j.at("component");

    // the optional parts of the message
    if (j.contains("variable")) {
        k.variable.emplace(j.at("variable"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given ComponentVariable \p k to the given output stream \p os
/// \returns an output stream with the ComponentVariable written to
std::ostream& operator<<(std::ostream& os, const ComponentVariable& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ConstantStreamData \p k to a given json object \p j
void to_json(json& j, const ConstantStreamData& k) {
    // the required parts of the message
    j = json{
        {"id", k.id},
        {"params", k.params},
        {"variableMonitoringId", k.variableMonitoringId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ConstantStreamData \p k
void from_json(const json& j, ConstantStreamData& k) {
    // the required parts of the message
    k.id = j.at("id");
    k.params = j.at("params");
    k.variableMonitoringId = j.at("variableMonitoringId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given ConstantStreamData \p k to the given output stream \p os
/// \returns an output stream with the ConstantStreamData written to
std::ostream& operator<<(std::ostream& os, const ConstantStreamData& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TariffAssignment \p k to a given json object \p j
void to_json(json& j, const TariffAssignment& k) {
    // the required parts of the message
    j = json{
        {"tariffId", k.tariffId},
        {"tariffKind", conversions::tariff_kind_enum_to_string(k.tariffKind)},
    };
    // the optional parts of the message
    if (k.validFrom) {
        j["validFrom"] = k.validFrom.value().to_rfc3339();
    }
    if (k.evseIds) {
        j["evseIds"] = json::array();
        for (auto val : k.evseIds.value()) {
            j["evseIds"].push_back(val);
        }
    }
    if (k.idTokens) {
        j["idTokens"] = json::array();
        for (auto val : k.idTokens.value()) {
            j["idTokens"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TariffAssignment \p k
void from_json(const json& j, TariffAssignment& k) {
    // the required parts of the message
    k.tariffId = j.at("tariffId");
    k.tariffKind = conversions::string_to_tariff_kind_enum(j.at("tariffKind"));

    // the optional parts of the message
    if (j.contains("validFrom")) {
        k.validFrom.emplace(j.at("validFrom").get<std::string>());
    }
    if (j.contains("evseIds")) {
        json arr = j.at("evseIds");
        std::vector<int32_t> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.evseIds.emplace(vec);
    }
    if (j.contains("idTokens")) {
        json arr = j.at("idTokens");
        std::vector<CiString<255>> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.idTokens.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TariffAssignment \p k to the given output stream \p os
/// \returns an output stream with the TariffAssignment written to
std::ostream& operator<<(std::ostream& os, const TariffAssignment& k) {
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
    if (k.attributeType) {
        j["attributeType"] = conversions::attribute_enum_to_string(k.attributeType.value());
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given GetVariableData \p k
void from_json(const json& j, GetVariableData& k) {
    // the required parts of the message
    k.component = j.at("component");
    k.variable = j.at("variable");

    // the optional parts of the message
    if (j.contains("attributeType")) {
        k.attributeType.emplace(conversions::string_to_attribute_enum(j.at("attributeType")));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.attributeStatusInfo) {
        j["attributeStatusInfo"] = k.attributeStatusInfo.value();
    }
    if (k.attributeType) {
        j["attributeType"] = conversions::attribute_enum_to_string(k.attributeType.value());
    }
    if (k.attributeValue) {
        j["attributeValue"] = k.attributeValue.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given GetVariableResult \p k
void from_json(const json& j, GetVariableResult& k) {
    // the required parts of the message
    k.attributeStatus = conversions::string_to_get_variable_status_enum(j.at("attributeStatus"));
    k.component = j.at("component");
    k.variable = j.at("variable");

    // the optional parts of the message
    if (j.contains("attributeStatusInfo")) {
        k.attributeStatusInfo.emplace(j.at("attributeStatusInfo"));
    }
    if (j.contains("attributeType")) {
        k.attributeType.emplace(conversions::string_to_attribute_enum(j.at("attributeType")));
    }
    if (j.contains("attributeValue")) {
        k.attributeValue.emplace(j.at("attributeValue"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
        {"encodingMethod", k.encodingMethod},
    };
    // the optional parts of the message
    if (k.signingMethod) {
        j["signingMethod"] = k.signingMethod.value();
    }
    if (k.publicKey) {
        j["publicKey"] = k.publicKey.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given SignedMeterValue \p k
void from_json(const json& j, SignedMeterValue& k) {
    // the required parts of the message
    k.signedMeterData = j.at("signedMeterData");
    k.encodingMethod = j.at("encodingMethod");

    // the optional parts of the message
    if (j.contains("signingMethod")) {
        k.signingMethod.emplace(j.at("signingMethod"));
    }
    if (j.contains("publicKey")) {
        k.publicKey.emplace(j.at("publicKey"));
    }
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
    if (k.unit) {
        j["unit"] = k.unit.value();
    }
    if (k.multiplier) {
        j["multiplier"] = k.multiplier.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given UnitOfMeasure \p k
void from_json(const json& j, UnitOfMeasure& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("unit")) {
        k.unit.emplace(j.at("unit"));
    }
    if (j.contains("multiplier")) {
        k.multiplier.emplace(j.at("multiplier"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.measurand) {
        j["measurand"] = conversions::measurand_enum_to_string(k.measurand.value());
    }
    if (k.context) {
        j["context"] = conversions::reading_context_enum_to_string(k.context.value());
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
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given SampledValue \p k
void from_json(const json& j, SampledValue& k) {
    // the required parts of the message
    k.value = j.at("value");

    // the optional parts of the message
    if (j.contains("measurand")) {
        k.measurand.emplace(conversions::string_to_measurand_enum(j.at("measurand")));
    }
    if (j.contains("context")) {
        k.context.emplace(conversions::string_to_reading_context_enum(j.at("context")));
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
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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

/// \brief Conversion from a given LimitAtSoC \p k to a given json object \p j
void to_json(json& j, const LimitAtSoC& k) {
    // the required parts of the message
    j = json{
        {"soc", k.soc},
        {"limit", k.limit},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given LimitAtSoC \p k
void from_json(const json& j, LimitAtSoC& k) {
    // the required parts of the message
    k.soc = j.at("soc");
    k.limit = j.at("limit");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given LimitAtSoC \p k to the given output stream \p os
/// \returns an output stream with the LimitAtSoC written to
std::ostream& operator<<(std::ostream& os, const LimitAtSoC& k) {
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
    if (k.duration) {
        j["duration"] = k.duration.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given RelativeTimeInterval \p k
void from_json(const json& j, RelativeTimeInterval& k) {
    // the required parts of the message
    k.start = j.at("start");

    // the optional parts of the message
    if (j.contains("duration")) {
        k.duration.emplace(j.at("duration"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.amountMultiplier) {
        j["amountMultiplier"] = k.amountMultiplier.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Cost \p k
void from_json(const json& j, Cost& k) {
    // the required parts of the message
    k.costKind = conversions::string_to_cost_kind_enum(j.at("costKind"));
    k.amount = j.at("amount");

    // the optional parts of the message
    if (j.contains("amountMultiplier")) {
        k.amountMultiplier.emplace(j.at("amountMultiplier"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.ePriceLevel) {
        j["ePriceLevel"] = k.ePriceLevel.value();
    }
    if (k.consumptionCost) {
        j["consumptionCost"] = json::array();
        for (auto val : k.consumptionCost.value()) {
            j["consumptionCost"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given SalesTariffEntry \p k
void from_json(const json& j, SalesTariffEntry& k) {
    // the required parts of the message
    k.relativeTimeInterval = j.at("relativeTimeInterval");

    // the optional parts of the message
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
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.salesTariffDescription) {
        j["salesTariffDescription"] = k.salesTariffDescription.value();
    }
    if (k.numEPriceLevels) {
        j["numEPriceLevels"] = k.numEPriceLevels.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
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
    if (j.contains("salesTariffDescription")) {
        k.salesTariffDescription.emplace(j.at("salesTariffDescription"));
    }
    if (j.contains("numEPriceLevels")) {
        k.numEPriceLevels.emplace(j.at("numEPriceLevels"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given SalesTariff \p k to the given output stream \p os
/// \returns an output stream with the SalesTariff written to
std::ostream& operator<<(std::ostream& os, const SalesTariff& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given RationalNumber \p k to a given json object \p j
void to_json(json& j, const RationalNumber& k) {
    // the required parts of the message
    j = json{
        {"exponent", k.exponent},
        {"value", k.value},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given RationalNumber \p k
void from_json(const json& j, RationalNumber& k) {
    // the required parts of the message
    k.exponent = j.at("exponent");
    k.value = j.at("value");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given RationalNumber \p k to the given output stream \p os
/// \returns an output stream with the RationalNumber written to
std::ostream& operator<<(std::ostream& os, const RationalNumber& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given PriceRule \p k to a given json object \p j
void to_json(json& j, const PriceRule& k) {
    // the required parts of the message
    j = json{
        {"energyFee", k.energyFee},
        {"powerRangeStart", k.powerRangeStart},
    };
    // the optional parts of the message
    if (k.parkingFeePeriod) {
        j["parkingFeePeriod"] = k.parkingFeePeriod.value();
    }
    if (k.carbonDioxideEmission) {
        j["carbonDioxideEmission"] = k.carbonDioxideEmission.value();
    }
    if (k.renewableGenerationPercentage) {
        j["renewableGenerationPercentage"] = k.renewableGenerationPercentage.value();
    }
    if (k.parkingFee) {
        j["parkingFee"] = k.parkingFee.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given PriceRule \p k
void from_json(const json& j, PriceRule& k) {
    // the required parts of the message
    k.energyFee = j.at("energyFee");
    k.powerRangeStart = j.at("powerRangeStart");

    // the optional parts of the message
    if (j.contains("parkingFeePeriod")) {
        k.parkingFeePeriod.emplace(j.at("parkingFeePeriod"));
    }
    if (j.contains("carbonDioxideEmission")) {
        k.carbonDioxideEmission.emplace(j.at("carbonDioxideEmission"));
    }
    if (j.contains("renewableGenerationPercentage")) {
        k.renewableGenerationPercentage.emplace(j.at("renewableGenerationPercentage"));
    }
    if (j.contains("parkingFee")) {
        k.parkingFee.emplace(j.at("parkingFee"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given PriceRule \p k to the given output stream \p os
/// \returns an output stream with the PriceRule written to
std::ostream& operator<<(std::ostream& os, const PriceRule& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given PriceRuleStack \p k to a given json object \p j
void to_json(json& j, const PriceRuleStack& k) {
    // the required parts of the message
    j = json{
        {"duration", k.duration},
        {"priceRule", k.priceRule},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given PriceRuleStack \p k
void from_json(const json& j, PriceRuleStack& k) {
    // the required parts of the message
    k.duration = j.at("duration");
    for (auto val : j.at("priceRule")) {
        k.priceRule.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given PriceRuleStack \p k to the given output stream \p os
/// \returns an output stream with the PriceRuleStack written to
std::ostream& operator<<(std::ostream& os, const PriceRuleStack& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TaxRule \p k to a given json object \p j
void to_json(json& j, const TaxRule& k) {
    // the required parts of the message
    j = json{
        {"taxRuleID", k.taxRuleID},
        {"appliesToEnergyFee", k.appliesToEnergyFee},
        {"appliesToParkingFee", k.appliesToParkingFee},
        {"appliesToOverstayFee", k.appliesToOverstayFee},
        {"appliesToMinimumMaximumCost", k.appliesToMinimumMaximumCost},
        {"taxRate", k.taxRate},
    };
    // the optional parts of the message
    if (k.taxRuleName) {
        j["taxRuleName"] = k.taxRuleName.value();
    }
    if (k.taxIncludedInPrice) {
        j["taxIncludedInPrice"] = k.taxIncludedInPrice.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TaxRule \p k
void from_json(const json& j, TaxRule& k) {
    // the required parts of the message
    k.taxRuleID = j.at("taxRuleID");
    k.appliesToEnergyFee = j.at("appliesToEnergyFee");
    k.appliesToParkingFee = j.at("appliesToParkingFee");
    k.appliesToOverstayFee = j.at("appliesToOverstayFee");
    k.appliesToMinimumMaximumCost = j.at("appliesToMinimumMaximumCost");
    k.taxRate = j.at("taxRate");

    // the optional parts of the message
    if (j.contains("taxRuleName")) {
        k.taxRuleName.emplace(j.at("taxRuleName"));
    }
    if (j.contains("taxIncludedInPrice")) {
        k.taxIncludedInPrice.emplace(j.at("taxIncludedInPrice"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TaxRule \p k to the given output stream \p os
/// \returns an output stream with the TaxRule written to
std::ostream& operator<<(std::ostream& os, const TaxRule& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given OverstayRule \p k to a given json object \p j
void to_json(json& j, const OverstayRule& k) {
    // the required parts of the message
    j = json{
        {"overstayFee", k.overstayFee},
        {"startTime", k.startTime},
        {"overstayFeePeriod", k.overstayFeePeriod},
    };
    // the optional parts of the message
    if (k.overstayRuleDescription) {
        j["overstayRuleDescription"] = k.overstayRuleDescription.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given OverstayRule \p k
void from_json(const json& j, OverstayRule& k) {
    // the required parts of the message
    k.overstayFee = j.at("overstayFee");
    k.startTime = j.at("startTime");
    k.overstayFeePeriod = j.at("overstayFeePeriod");

    // the optional parts of the message
    if (j.contains("overstayRuleDescription")) {
        k.overstayRuleDescription.emplace(j.at("overstayRuleDescription"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given OverstayRule \p k to the given output stream \p os
/// \returns an output stream with the OverstayRule written to
std::ostream& operator<<(std::ostream& os, const OverstayRule& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given OverstayRuleList \p k to a given json object \p j
void to_json(json& j, const OverstayRuleList& k) {
    // the required parts of the message
    j = json{
        {"overstayRule", k.overstayRule},
    };
    // the optional parts of the message
    if (k.overstayPowerThreshold) {
        j["overstayPowerThreshold"] = k.overstayPowerThreshold.value();
    }
    if (k.overstayTimeThreshold) {
        j["overstayTimeThreshold"] = k.overstayTimeThreshold.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given OverstayRuleList \p k
void from_json(const json& j, OverstayRuleList& k) {
    // the required parts of the message
    for (auto val : j.at("overstayRule")) {
        k.overstayRule.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("overstayPowerThreshold")) {
        k.overstayPowerThreshold.emplace(j.at("overstayPowerThreshold"));
    }
    if (j.contains("overstayTimeThreshold")) {
        k.overstayTimeThreshold.emplace(j.at("overstayTimeThreshold"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given OverstayRuleList \p k to the given output stream \p os
/// \returns an output stream with the OverstayRuleList written to
std::ostream& operator<<(std::ostream& os, const OverstayRuleList& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given AdditionalSelectedServices \p k to a given json object \p j
void to_json(json& j, const AdditionalSelectedServices& k) {
    // the required parts of the message
    j = json{
        {"serviceFee", k.serviceFee},
        {"serviceName", k.serviceName},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given AdditionalSelectedServices \p k
void from_json(const json& j, AdditionalSelectedServices& k) {
    // the required parts of the message
    k.serviceFee = j.at("serviceFee");
    k.serviceName = j.at("serviceName");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given AdditionalSelectedServices \p k to the given output stream \p os
/// \returns an output stream with the AdditionalSelectedServices written to
std::ostream& operator<<(std::ostream& os, const AdditionalSelectedServices& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given AbsolutePriceSchedule \p k to a given json object \p j
void to_json(json& j, const AbsolutePriceSchedule& k) {
    // the required parts of the message
    j = json{
        {"timeAnchor", k.timeAnchor.to_rfc3339()},
        {"priceScheduleID", k.priceScheduleID},
        {"currency", k.currency},
        {"language", k.language},
        {"priceAlgorithm", k.priceAlgorithm},
        {"priceRuleStacks", k.priceRuleStacks},
    };
    // the optional parts of the message
    if (k.priceScheduleDescription) {
        j["priceScheduleDescription"] = k.priceScheduleDescription.value();
    }
    if (k.minimumCost) {
        j["minimumCost"] = k.minimumCost.value();
    }
    if (k.maximumCost) {
        j["maximumCost"] = k.maximumCost.value();
    }
    if (k.taxRules) {
        j["taxRules"] = json::array();
        for (auto val : k.taxRules.value()) {
            j["taxRules"].push_back(val);
        }
    }
    if (k.overstayRuleList) {
        j["overstayRuleList"] = k.overstayRuleList.value();
    }
    if (k.additionalSelectedServices) {
        j["additionalSelectedServices"] = json::array();
        for (auto val : k.additionalSelectedServices.value()) {
            j["additionalSelectedServices"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given AbsolutePriceSchedule \p k
void from_json(const json& j, AbsolutePriceSchedule& k) {
    // the required parts of the message
    k.timeAnchor = ocpp::DateTime(std::string(j.at("timeAnchor")));
    k.priceScheduleID = j.at("priceScheduleID");
    k.currency = j.at("currency");
    k.language = j.at("language");
    k.priceAlgorithm = j.at("priceAlgorithm");
    for (auto val : j.at("priceRuleStacks")) {
        k.priceRuleStacks.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("priceScheduleDescription")) {
        k.priceScheduleDescription.emplace(j.at("priceScheduleDescription"));
    }
    if (j.contains("minimumCost")) {
        k.minimumCost.emplace(j.at("minimumCost"));
    }
    if (j.contains("maximumCost")) {
        k.maximumCost.emplace(j.at("maximumCost"));
    }
    if (j.contains("taxRules")) {
        json arr = j.at("taxRules");
        std::vector<TaxRule> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.taxRules.emplace(vec);
    }
    if (j.contains("overstayRuleList")) {
        k.overstayRuleList.emplace(j.at("overstayRuleList"));
    }
    if (j.contains("additionalSelectedServices")) {
        json arr = j.at("additionalSelectedServices");
        std::vector<AdditionalSelectedServices> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.additionalSelectedServices.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given AbsolutePriceSchedule \p k to the given output stream \p os
/// \returns an output stream with the AbsolutePriceSchedule written to
std::ostream& operator<<(std::ostream& os, const AbsolutePriceSchedule& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given PriceLevelScheduleEntry \p k to a given json object \p j
void to_json(json& j, const PriceLevelScheduleEntry& k) {
    // the required parts of the message
    j = json{
        {"duration", k.duration},
        {"priceLevel", k.priceLevel},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given PriceLevelScheduleEntry \p k
void from_json(const json& j, PriceLevelScheduleEntry& k) {
    // the required parts of the message
    k.duration = j.at("duration");
    k.priceLevel = j.at("priceLevel");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given PriceLevelScheduleEntry \p k to the given output stream \p os
/// \returns an output stream with the PriceLevelScheduleEntry written to
std::ostream& operator<<(std::ostream& os, const PriceLevelScheduleEntry& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given PriceLevelSchedule \p k to a given json object \p j
void to_json(json& j, const PriceLevelSchedule& k) {
    // the required parts of the message
    j = json{
        {"priceLevelScheduleEntries", k.priceLevelScheduleEntries},
        {"timeAnchor", k.timeAnchor.to_rfc3339()},
        {"priceScheduleId", k.priceScheduleId},
        {"numberOfPriceLevels", k.numberOfPriceLevels},
    };
    // the optional parts of the message
    if (k.priceScheduleDescription) {
        j["priceScheduleDescription"] = k.priceScheduleDescription.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given PriceLevelSchedule \p k
void from_json(const json& j, PriceLevelSchedule& k) {
    // the required parts of the message
    for (auto val : j.at("priceLevelScheduleEntries")) {
        k.priceLevelScheduleEntries.push_back(val);
    }
    k.timeAnchor = ocpp::DateTime(std::string(j.at("timeAnchor")));
    k.priceScheduleId = j.at("priceScheduleId");
    k.numberOfPriceLevels = j.at("numberOfPriceLevels");

    // the optional parts of the message
    if (j.contains("priceScheduleDescription")) {
        k.priceScheduleDescription.emplace(j.at("priceScheduleDescription"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given PriceLevelSchedule \p k to the given output stream \p os
/// \returns an output stream with the PriceLevelSchedule written to
std::ostream& operator<<(std::ostream& os, const PriceLevelSchedule& k) {
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
    if (k.limitAtSoC) {
        j["limitAtSoC"] = k.limitAtSoC.value();
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
    if (k.powerTolerance) {
        j["powerTolerance"] = k.powerTolerance.value();
    }
    if (k.signatureId) {
        j["signatureId"] = k.signatureId.value();
    }
    if (k.digestValue) {
        j["digestValue"] = k.digestValue.value();
    }
    if (k.useLocalTime) {
        j["useLocalTime"] = k.useLocalTime.value();
    }
    if (k.randomizedDelay) {
        j["randomizedDelay"] = k.randomizedDelay.value();
    }
    if (k.salesTariff) {
        j["salesTariff"] = k.salesTariff.value();
    }
    if (k.absolutePriceSchedule) {
        j["absolutePriceSchedule"] = k.absolutePriceSchedule.value();
    }
    if (k.priceLevelSchedule) {
        j["priceLevelSchedule"] = k.priceLevelSchedule.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
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
    if (j.contains("limitAtSoC")) {
        k.limitAtSoC.emplace(j.at("limitAtSoC"));
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
    if (j.contains("powerTolerance")) {
        k.powerTolerance.emplace(j.at("powerTolerance"));
    }
    if (j.contains("signatureId")) {
        k.signatureId.emplace(j.at("signatureId"));
    }
    if (j.contains("digestValue")) {
        k.digestValue.emplace(j.at("digestValue"));
    }
    if (j.contains("useLocalTime")) {
        k.useLocalTime.emplace(j.at("useLocalTime"));
    }
    if (j.contains("randomizedDelay")) {
        k.randomizedDelay.emplace(j.at("randomizedDelay"));
    }
    if (j.contains("salesTariff")) {
        k.salesTariff.emplace(j.at("salesTariff"));
    }
    if (j.contains("absolutePriceSchedule")) {
        k.absolutePriceSchedule.emplace(j.at("absolutePriceSchedule"));
    }
    if (j.contains("priceLevelSchedule")) {
        k.priceLevelSchedule.emplace(j.at("priceLevelSchedule"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
        {"chargingLimitSource", k.chargingLimitSource},
    };
    // the optional parts of the message
    if (k.isLocalGeneration) {
        j["isLocalGeneration"] = k.isLocalGeneration.value();
    }
    if (k.isGridCritical) {
        j["isGridCritical"] = k.isGridCritical.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingLimit \p k
void from_json(const json& j, ChargingLimit& k) {
    // the required parts of the message
    k.chargingLimitSource = j.at("chargingLimitSource");

    // the optional parts of the message
    if (j.contains("isLocalGeneration")) {
        k.isLocalGeneration.emplace(j.at("isLocalGeneration"));
    }
    if (j.contains("isGridCritical")) {
        k.isGridCritical.emplace(j.at("isGridCritical"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.messageExtra) {
        j["messageExtra"] = json::array();
        for (auto val : k.messageExtra.value()) {
            j["messageExtra"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given MessageInfo \p k
void from_json(const json& j, MessageInfo& k) {
    // the required parts of the message
    k.id = j.at("id");
    k.priority = conversions::string_to_message_priority_enum(j.at("priority"));
    k.message = j.at("message");

    // the optional parts of the message
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
    if (j.contains("messageExtra")) {
        json arr = j.at("messageExtra");
        std::vector<MessageContent> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.messageExtra.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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

/// \brief Conversion from a given DERChargingParameters \p k to a given json object \p j
void to_json(json& j, const DERChargingParameters& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.evSupportedDERControl) {
        if (j.size() == 0) {
            j = json{{"evSupportedDERControl", json::array()}};
        } else {
            j["evSupportedDERControl"] = json::array();
        }
        for (auto val : k.evSupportedDERControl.value()) {
            j["evSupportedDERControl"].push_back(conversions::dercontrol_enum_to_string(val));
        }
    }
    if (k.evOverExcitedMaxDischargePower) {
        j["evOverExcitedMaxDischargePower"] = k.evOverExcitedMaxDischargePower.value();
    }
    if (k.evOverExcitedPowerFactor) {
        j["evOverExcitedPowerFactor"] = k.evOverExcitedPowerFactor.value();
    }
    if (k.evUnderExcitedMaxDischargePower) {
        j["evUnderExcitedMaxDischargePower"] = k.evUnderExcitedMaxDischargePower.value();
    }
    if (k.evUnderExcitedPowerFactor) {
        j["evUnderExcitedPowerFactor"] = k.evUnderExcitedPowerFactor.value();
    }
    if (k.maxApparentPower) {
        j["maxApparentPower"] = k.maxApparentPower.value();
    }
    if (k.maxChargeApparentPower) {
        j["maxChargeApparentPower"] = k.maxChargeApparentPower.value();
    }
    if (k.maxChargeApparentPower_L2) {
        j["maxChargeApparentPower_L2"] = k.maxChargeApparentPower_L2.value();
    }
    if (k.maxChargeApparentPower_L3) {
        j["maxChargeApparentPower_L3"] = k.maxChargeApparentPower_L3.value();
    }
    if (k.maxDischargeApparentPower) {
        j["maxDischargeApparentPower"] = k.maxDischargeApparentPower.value();
    }
    if (k.maxDischargeApparentPower_L2) {
        j["maxDischargeApparentPower_L2"] = k.maxDischargeApparentPower_L2.value();
    }
    if (k.maxDischargeApparentPower_L3) {
        j["maxDischargeApparentPower_L3"] = k.maxDischargeApparentPower_L3.value();
    }
    if (k.maxChargeReactivePower) {
        j["maxChargeReactivePower"] = k.maxChargeReactivePower.value();
    }
    if (k.maxChargeReactivePower_L2) {
        j["maxChargeReactivePower_L2"] = k.maxChargeReactivePower_L2.value();
    }
    if (k.maxChargeReactivePower_L3) {
        j["maxChargeReactivePower_L3"] = k.maxChargeReactivePower_L3.value();
    }
    if (k.minChargeReactivePower) {
        j["minChargeReactivePower"] = k.minChargeReactivePower.value();
    }
    if (k.minChargeReactivePower_L2) {
        j["minChargeReactivePower_L2"] = k.minChargeReactivePower_L2.value();
    }
    if (k.minChargeReactivePower_L3) {
        j["minChargeReactivePower_L3"] = k.minChargeReactivePower_L3.value();
    }
    if (k.maxDischargeReactivePower) {
        j["maxDischargeReactivePower"] = k.maxDischargeReactivePower.value();
    }
    if (k.maxDischargeReactivePower_L2) {
        j["maxDischargeReactivePower_L2"] = k.maxDischargeReactivePower_L2.value();
    }
    if (k.maxDischargeReactivePower_L3) {
        j["maxDischargeReactivePower_L3"] = k.maxDischargeReactivePower_L3.value();
    }
    if (k.minDischargeReactivePower) {
        j["minDischargeReactivePower"] = k.minDischargeReactivePower.value();
    }
    if (k.minDischargeReactivePower_L2) {
        j["minDischargeReactivePower_L2"] = k.minDischargeReactivePower_L2.value();
    }
    if (k.minDischargeReactivePower_L3) {
        j["minDischargeReactivePower_L3"] = k.minDischargeReactivePower_L3.value();
    }
    if (k.nominalVoltage) {
        j["nominalVoltage"] = k.nominalVoltage.value();
    }
    if (k.nominalVoltageOffset) {
        j["nominalVoltageOffset"] = k.nominalVoltageOffset.value();
    }
    if (k.maxNominalVoltage) {
        j["maxNominalVoltage"] = k.maxNominalVoltage.value();
    }
    if (k.minNominalVoltage) {
        j["minNominalVoltage"] = k.minNominalVoltage.value();
    }
    if (k.evInverterManufacturer) {
        j["evInverterManufacturer"] = k.evInverterManufacturer.value();
    }
    if (k.evInverterModel) {
        j["evInverterModel"] = k.evInverterModel.value();
    }
    if (k.evInverterSerialNumber) {
        j["evInverterSerialNumber"] = k.evInverterSerialNumber.value();
    }
    if (k.evInverterSwVersion) {
        j["evInverterSwVersion"] = k.evInverterSwVersion.value();
    }
    if (k.evInverterHwVersion) {
        j["evInverterHwVersion"] = k.evInverterHwVersion.value();
    }
    if (k.evIslandingDetectionMethod) {
        if (j.size() == 0) {
            j = json{{"evIslandingDetectionMethod", json::array()}};
        } else {
            j["evIslandingDetectionMethod"] = json::array();
        }
        for (auto val : k.evIslandingDetectionMethod.value()) {
            j["evIslandingDetectionMethod"].push_back(conversions::islanding_detection_enum_to_string(val));
        }
    }
    if (k.evIslandingTripTime) {
        j["evIslandingTripTime"] = k.evIslandingTripTime.value();
    }
    if (k.evMaximumLevel1DCInjection) {
        j["evMaximumLevel1DCInjection"] = k.evMaximumLevel1DCInjection.value();
    }
    if (k.evDurationLevel1DCInjection) {
        j["evDurationLevel1DCInjection"] = k.evDurationLevel1DCInjection.value();
    }
    if (k.evMaximumLevel2DCInjection) {
        j["evMaximumLevel2DCInjection"] = k.evMaximumLevel2DCInjection.value();
    }
    if (k.evDurationLevel2DCInjection) {
        j["evDurationLevel2DCInjection"] = k.evDurationLevel2DCInjection.value();
    }
    if (k.evReactiveSusceptance) {
        j["evReactiveSusceptance"] = k.evReactiveSusceptance.value();
    }
    if (k.evSessionTotalDischargeEnergyAvailable) {
        j["evSessionTotalDischargeEnergyAvailable"] = k.evSessionTotalDischargeEnergyAvailable.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given DERChargingParameters \p k
void from_json(const json& j, DERChargingParameters& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("evSupportedDERControl")) {
        json arr = j.at("evSupportedDERControl");
        std::vector<DERControlEnum> vec;
        for (auto val : arr) {
            vec.push_back(conversions::string_to_dercontrol_enum(val));
        }
        k.evSupportedDERControl.emplace(vec);
    }
    if (j.contains("evOverExcitedMaxDischargePower")) {
        k.evOverExcitedMaxDischargePower.emplace(j.at("evOverExcitedMaxDischargePower"));
    }
    if (j.contains("evOverExcitedPowerFactor")) {
        k.evOverExcitedPowerFactor.emplace(j.at("evOverExcitedPowerFactor"));
    }
    if (j.contains("evUnderExcitedMaxDischargePower")) {
        k.evUnderExcitedMaxDischargePower.emplace(j.at("evUnderExcitedMaxDischargePower"));
    }
    if (j.contains("evUnderExcitedPowerFactor")) {
        k.evUnderExcitedPowerFactor.emplace(j.at("evUnderExcitedPowerFactor"));
    }
    if (j.contains("maxApparentPower")) {
        k.maxApparentPower.emplace(j.at("maxApparentPower"));
    }
    if (j.contains("maxChargeApparentPower")) {
        k.maxChargeApparentPower.emplace(j.at("maxChargeApparentPower"));
    }
    if (j.contains("maxChargeApparentPower_L2")) {
        k.maxChargeApparentPower_L2.emplace(j.at("maxChargeApparentPower_L2"));
    }
    if (j.contains("maxChargeApparentPower_L3")) {
        k.maxChargeApparentPower_L3.emplace(j.at("maxChargeApparentPower_L3"));
    }
    if (j.contains("maxDischargeApparentPower")) {
        k.maxDischargeApparentPower.emplace(j.at("maxDischargeApparentPower"));
    }
    if (j.contains("maxDischargeApparentPower_L2")) {
        k.maxDischargeApparentPower_L2.emplace(j.at("maxDischargeApparentPower_L2"));
    }
    if (j.contains("maxDischargeApparentPower_L3")) {
        k.maxDischargeApparentPower_L3.emplace(j.at("maxDischargeApparentPower_L3"));
    }
    if (j.contains("maxChargeReactivePower")) {
        k.maxChargeReactivePower.emplace(j.at("maxChargeReactivePower"));
    }
    if (j.contains("maxChargeReactivePower_L2")) {
        k.maxChargeReactivePower_L2.emplace(j.at("maxChargeReactivePower_L2"));
    }
    if (j.contains("maxChargeReactivePower_L3")) {
        k.maxChargeReactivePower_L3.emplace(j.at("maxChargeReactivePower_L3"));
    }
    if (j.contains("minChargeReactivePower")) {
        k.minChargeReactivePower.emplace(j.at("minChargeReactivePower"));
    }
    if (j.contains("minChargeReactivePower_L2")) {
        k.minChargeReactivePower_L2.emplace(j.at("minChargeReactivePower_L2"));
    }
    if (j.contains("minChargeReactivePower_L3")) {
        k.minChargeReactivePower_L3.emplace(j.at("minChargeReactivePower_L3"));
    }
    if (j.contains("maxDischargeReactivePower")) {
        k.maxDischargeReactivePower.emplace(j.at("maxDischargeReactivePower"));
    }
    if (j.contains("maxDischargeReactivePower_L2")) {
        k.maxDischargeReactivePower_L2.emplace(j.at("maxDischargeReactivePower_L2"));
    }
    if (j.contains("maxDischargeReactivePower_L3")) {
        k.maxDischargeReactivePower_L3.emplace(j.at("maxDischargeReactivePower_L3"));
    }
    if (j.contains("minDischargeReactivePower")) {
        k.minDischargeReactivePower.emplace(j.at("minDischargeReactivePower"));
    }
    if (j.contains("minDischargeReactivePower_L2")) {
        k.minDischargeReactivePower_L2.emplace(j.at("minDischargeReactivePower_L2"));
    }
    if (j.contains("minDischargeReactivePower_L3")) {
        k.minDischargeReactivePower_L3.emplace(j.at("minDischargeReactivePower_L3"));
    }
    if (j.contains("nominalVoltage")) {
        k.nominalVoltage.emplace(j.at("nominalVoltage"));
    }
    if (j.contains("nominalVoltageOffset")) {
        k.nominalVoltageOffset.emplace(j.at("nominalVoltageOffset"));
    }
    if (j.contains("maxNominalVoltage")) {
        k.maxNominalVoltage.emplace(j.at("maxNominalVoltage"));
    }
    if (j.contains("minNominalVoltage")) {
        k.minNominalVoltage.emplace(j.at("minNominalVoltage"));
    }
    if (j.contains("evInverterManufacturer")) {
        k.evInverterManufacturer.emplace(j.at("evInverterManufacturer"));
    }
    if (j.contains("evInverterModel")) {
        k.evInverterModel.emplace(j.at("evInverterModel"));
    }
    if (j.contains("evInverterSerialNumber")) {
        k.evInverterSerialNumber.emplace(j.at("evInverterSerialNumber"));
    }
    if (j.contains("evInverterSwVersion")) {
        k.evInverterSwVersion.emplace(j.at("evInverterSwVersion"));
    }
    if (j.contains("evInverterHwVersion")) {
        k.evInverterHwVersion.emplace(j.at("evInverterHwVersion"));
    }
    if (j.contains("evIslandingDetectionMethod")) {
        json arr = j.at("evIslandingDetectionMethod");
        std::vector<IslandingDetectionEnum> vec;
        for (auto val : arr) {
            vec.push_back(conversions::string_to_islanding_detection_enum(val));
        }
        k.evIslandingDetectionMethod.emplace(vec);
    }
    if (j.contains("evIslandingTripTime")) {
        k.evIslandingTripTime.emplace(j.at("evIslandingTripTime"));
    }
    if (j.contains("evMaximumLevel1DCInjection")) {
        k.evMaximumLevel1DCInjection.emplace(j.at("evMaximumLevel1DCInjection"));
    }
    if (j.contains("evDurationLevel1DCInjection")) {
        k.evDurationLevel1DCInjection.emplace(j.at("evDurationLevel1DCInjection"));
    }
    if (j.contains("evMaximumLevel2DCInjection")) {
        k.evMaximumLevel2DCInjection.emplace(j.at("evMaximumLevel2DCInjection"));
    }
    if (j.contains("evDurationLevel2DCInjection")) {
        k.evDurationLevel2DCInjection.emplace(j.at("evDurationLevel2DCInjection"));
    }
    if (j.contains("evReactiveSusceptance")) {
        k.evReactiveSusceptance.emplace(j.at("evReactiveSusceptance"));
    }
    if (j.contains("evSessionTotalDischargeEnergyAvailable")) {
        k.evSessionTotalDischargeEnergyAvailable.emplace(j.at("evSessionTotalDischargeEnergyAvailable"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given DERChargingParameters \p k to the given output stream \p os
/// \returns an output stream with the DERChargingParameters written to
std::ostream& operator<<(std::ostream& os, const DERChargingParameters& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given EVPriceRule \p k to a given json object \p j
void to_json(json& j, const EVPriceRule& k) {
    // the required parts of the message
    j = json{
        {"energyFee", k.energyFee},
        {"powerRangeStart", k.powerRangeStart},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given EVPriceRule \p k
void from_json(const json& j, EVPriceRule& k) {
    // the required parts of the message
    k.energyFee = j.at("energyFee");
    k.powerRangeStart = j.at("powerRangeStart");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given EVPriceRule \p k to the given output stream \p os
/// \returns an output stream with the EVPriceRule written to
std::ostream& operator<<(std::ostream& os, const EVPriceRule& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given EVAbsolutePriceScheduleEntry \p k to a given json object \p j
void to_json(json& j, const EVAbsolutePriceScheduleEntry& k) {
    // the required parts of the message
    j = json{
        {"duration", k.duration},
        {"evPriceRule", k.evPriceRule},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given EVAbsolutePriceScheduleEntry \p k
void from_json(const json& j, EVAbsolutePriceScheduleEntry& k) {
    // the required parts of the message
    k.duration = j.at("duration");
    for (auto val : j.at("evPriceRule")) {
        k.evPriceRule.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given EVAbsolutePriceScheduleEntry \p k to the given output stream \p
// os
/// \returns an output stream with the EVAbsolutePriceScheduleEntry written to
std::ostream& operator<<(std::ostream& os, const EVAbsolutePriceScheduleEntry& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given EVAbsolutePriceSchedule \p k to a given json object \p j
void to_json(json& j, const EVAbsolutePriceSchedule& k) {
    // the required parts of the message
    j = json{
        {"timeAnchor", k.timeAnchor.to_rfc3339()},
        {"currency", k.currency},
        {"evAbsolutePriceScheduleEntries", k.evAbsolutePriceScheduleEntries},
        {"priceAlgorithm", k.priceAlgorithm},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given EVAbsolutePriceSchedule \p k
void from_json(const json& j, EVAbsolutePriceSchedule& k) {
    // the required parts of the message
    k.timeAnchor = ocpp::DateTime(std::string(j.at("timeAnchor")));
    k.currency = j.at("currency");
    for (auto val : j.at("evAbsolutePriceScheduleEntries")) {
        k.evAbsolutePriceScheduleEntries.push_back(val);
    }
    k.priceAlgorithm = j.at("priceAlgorithm");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given EVAbsolutePriceSchedule \p k to the given output stream \p os
/// \returns an output stream with the EVAbsolutePriceSchedule written to
std::ostream& operator<<(std::ostream& os, const EVAbsolutePriceSchedule& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given EVPowerScheduleEntry \p k to a given json object \p j
void to_json(json& j, const EVPowerScheduleEntry& k) {
    // the required parts of the message
    j = json{
        {"duration", k.duration},
        {"power", k.power},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given EVPowerScheduleEntry \p k
void from_json(const json& j, EVPowerScheduleEntry& k) {
    // the required parts of the message
    k.duration = j.at("duration");
    k.power = j.at("power");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given EVPowerScheduleEntry \p k to the given output stream \p os
/// \returns an output stream with the EVPowerScheduleEntry written to
std::ostream& operator<<(std::ostream& os, const EVPowerScheduleEntry& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given EVPowerSchedule \p k to a given json object \p j
void to_json(json& j, const EVPowerSchedule& k) {
    // the required parts of the message
    j = json{
        {"evPowerScheduleEntries", k.evPowerScheduleEntries},
        {"timeAnchor", k.timeAnchor.to_rfc3339()},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given EVPowerSchedule \p k
void from_json(const json& j, EVPowerSchedule& k) {
    // the required parts of the message
    for (auto val : j.at("evPowerScheduleEntries")) {
        k.evPowerScheduleEntries.push_back(val);
    }
    k.timeAnchor = ocpp::DateTime(std::string(j.at("timeAnchor")));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given EVPowerSchedule \p k to the given output stream \p os
/// \returns an output stream with the EVPowerSchedule written to
std::ostream& operator<<(std::ostream& os, const EVPowerSchedule& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given EVEnergyOffer \p k to a given json object \p j
void to_json(json& j, const EVEnergyOffer& k) {
    // the required parts of the message
    j = json{
        {"evPowerSchedule", k.evPowerSchedule},
    };
    // the optional parts of the message
    if (k.evAbsolutePriceSchedule) {
        j["evAbsolutePriceSchedule"] = k.evAbsolutePriceSchedule.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given EVEnergyOffer \p k
void from_json(const json& j, EVEnergyOffer& k) {
    // the required parts of the message
    k.evPowerSchedule = j.at("evPowerSchedule");

    // the optional parts of the message
    if (j.contains("evAbsolutePriceSchedule")) {
        k.evAbsolutePriceSchedule.emplace(j.at("evAbsolutePriceSchedule"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given EVEnergyOffer \p k to the given output stream \p os
/// \returns an output stream with the EVEnergyOffer written to
std::ostream& operator<<(std::ostream& os, const EVEnergyOffer& k) {
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
    if (k.evMaxPower) {
        j["evMaxPower"] = k.evMaxPower.value();
    }
    if (k.evEnergyCapacity) {
        j["evEnergyCapacity"] = k.evEnergyCapacity.value();
    }
    if (k.energyAmount) {
        j["energyAmount"] = k.energyAmount.value();
    }
    if (k.stateOfCharge) {
        j["stateOfCharge"] = k.stateOfCharge.value();
    }
    if (k.fullSoC) {
        j["fullSoC"] = k.fullSoC.value();
    }
    if (k.bulkSoC) {
        j["bulkSoC"] = k.bulkSoC.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given DCChargingParameters \p k
void from_json(const json& j, DCChargingParameters& k) {
    // the required parts of the message
    k.evMaxCurrent = j.at("evMaxCurrent");
    k.evMaxVoltage = j.at("evMaxVoltage");

    // the optional parts of the message
    if (j.contains("evMaxPower")) {
        k.evMaxPower.emplace(j.at("evMaxPower"));
    }
    if (j.contains("evEnergyCapacity")) {
        k.evEnergyCapacity.emplace(j.at("evEnergyCapacity"));
    }
    if (j.contains("energyAmount")) {
        k.energyAmount.emplace(j.at("energyAmount"));
    }
    if (j.contains("stateOfCharge")) {
        k.stateOfCharge.emplace(j.at("stateOfCharge"));
    }
    if (j.contains("fullSoC")) {
        k.fullSoC.emplace(j.at("fullSoC"));
    }
    if (j.contains("bulkSoC")) {
        k.bulkSoC.emplace(j.at("bulkSoC"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given DCChargingParameters \p k to the given output stream \p os
/// \returns an output stream with the DCChargingParameters written to
std::ostream& operator<<(std::ostream& os, const DCChargingParameters& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given V2XChargingParameters \p k to a given json object \p j
void to_json(json& j, const V2XChargingParameters& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.minChargePower) {
        j["minChargePower"] = k.minChargePower.value();
    }
    if (k.minChargePower_L2) {
        j["minChargePower_L2"] = k.minChargePower_L2.value();
    }
    if (k.minChargePower_L3) {
        j["minChargePower_L3"] = k.minChargePower_L3.value();
    }
    if (k.maxChargePower) {
        j["maxChargePower"] = k.maxChargePower.value();
    }
    if (k.maxChargePower_L2) {
        j["maxChargePower_L2"] = k.maxChargePower_L2.value();
    }
    if (k.maxChargePower_L3) {
        j["maxChargePower_L3"] = k.maxChargePower_L3.value();
    }
    if (k.minDischargePower) {
        j["minDischargePower"] = k.minDischargePower.value();
    }
    if (k.minDischargePower_L2) {
        j["minDischargePower_L2"] = k.minDischargePower_L2.value();
    }
    if (k.minDischargePower_L3) {
        j["minDischargePower_L3"] = k.minDischargePower_L3.value();
    }
    if (k.maxDischargePower) {
        j["maxDischargePower"] = k.maxDischargePower.value();
    }
    if (k.maxDischargePower_L2) {
        j["maxDischargePower_L2"] = k.maxDischargePower_L2.value();
    }
    if (k.maxDischargePower_L3) {
        j["maxDischargePower_L3"] = k.maxDischargePower_L3.value();
    }
    if (k.minChargeCurrent) {
        j["minChargeCurrent"] = k.minChargeCurrent.value();
    }
    if (k.maxChargeCurrent) {
        j["maxChargeCurrent"] = k.maxChargeCurrent.value();
    }
    if (k.minDischargeCurrent) {
        j["minDischargeCurrent"] = k.minDischargeCurrent.value();
    }
    if (k.maxDischargeCurrent) {
        j["maxDischargeCurrent"] = k.maxDischargeCurrent.value();
    }
    if (k.minVoltage) {
        j["minVoltage"] = k.minVoltage.value();
    }
    if (k.maxVoltage) {
        j["maxVoltage"] = k.maxVoltage.value();
    }
    if (k.evTargetEnergyRequest) {
        j["evTargetEnergyRequest"] = k.evTargetEnergyRequest.value();
    }
    if (k.evMinEnergyRequest) {
        j["evMinEnergyRequest"] = k.evMinEnergyRequest.value();
    }
    if (k.evMaxEnergyRequest) {
        j["evMaxEnergyRequest"] = k.evMaxEnergyRequest.value();
    }
    if (k.evMinV2XEnergyRequest) {
        j["evMinV2XEnergyRequest"] = k.evMinV2XEnergyRequest.value();
    }
    if (k.evMaxV2XEnergyRequest) {
        j["evMaxV2XEnergyRequest"] = k.evMaxV2XEnergyRequest.value();
    }
    if (k.targetSoC) {
        j["targetSoC"] = k.targetSoC.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given V2XChargingParameters \p k
void from_json(const json& j, V2XChargingParameters& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("minChargePower")) {
        k.minChargePower.emplace(j.at("minChargePower"));
    }
    if (j.contains("minChargePower_L2")) {
        k.minChargePower_L2.emplace(j.at("minChargePower_L2"));
    }
    if (j.contains("minChargePower_L3")) {
        k.minChargePower_L3.emplace(j.at("minChargePower_L3"));
    }
    if (j.contains("maxChargePower")) {
        k.maxChargePower.emplace(j.at("maxChargePower"));
    }
    if (j.contains("maxChargePower_L2")) {
        k.maxChargePower_L2.emplace(j.at("maxChargePower_L2"));
    }
    if (j.contains("maxChargePower_L3")) {
        k.maxChargePower_L3.emplace(j.at("maxChargePower_L3"));
    }
    if (j.contains("minDischargePower")) {
        k.minDischargePower.emplace(j.at("minDischargePower"));
    }
    if (j.contains("minDischargePower_L2")) {
        k.minDischargePower_L2.emplace(j.at("minDischargePower_L2"));
    }
    if (j.contains("minDischargePower_L3")) {
        k.minDischargePower_L3.emplace(j.at("minDischargePower_L3"));
    }
    if (j.contains("maxDischargePower")) {
        k.maxDischargePower.emplace(j.at("maxDischargePower"));
    }
    if (j.contains("maxDischargePower_L2")) {
        k.maxDischargePower_L2.emplace(j.at("maxDischargePower_L2"));
    }
    if (j.contains("maxDischargePower_L3")) {
        k.maxDischargePower_L3.emplace(j.at("maxDischargePower_L3"));
    }
    if (j.contains("minChargeCurrent")) {
        k.minChargeCurrent.emplace(j.at("minChargeCurrent"));
    }
    if (j.contains("maxChargeCurrent")) {
        k.maxChargeCurrent.emplace(j.at("maxChargeCurrent"));
    }
    if (j.contains("minDischargeCurrent")) {
        k.minDischargeCurrent.emplace(j.at("minDischargeCurrent"));
    }
    if (j.contains("maxDischargeCurrent")) {
        k.maxDischargeCurrent.emplace(j.at("maxDischargeCurrent"));
    }
    if (j.contains("minVoltage")) {
        k.minVoltage.emplace(j.at("minVoltage"));
    }
    if (j.contains("maxVoltage")) {
        k.maxVoltage.emplace(j.at("maxVoltage"));
    }
    if (j.contains("evTargetEnergyRequest")) {
        k.evTargetEnergyRequest.emplace(j.at("evTargetEnergyRequest"));
    }
    if (j.contains("evMinEnergyRequest")) {
        k.evMinEnergyRequest.emplace(j.at("evMinEnergyRequest"));
    }
    if (j.contains("evMaxEnergyRequest")) {
        k.evMaxEnergyRequest.emplace(j.at("evMaxEnergyRequest"));
    }
    if (j.contains("evMinV2XEnergyRequest")) {
        k.evMinV2XEnergyRequest.emplace(j.at("evMinV2XEnergyRequest"));
    }
    if (j.contains("evMaxV2XEnergyRequest")) {
        k.evMaxV2XEnergyRequest.emplace(j.at("evMaxV2XEnergyRequest"));
    }
    if (j.contains("targetSoC")) {
        k.targetSoC.emplace(j.at("targetSoC"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given V2XChargingParameters \p k to the given output stream \p os
/// \returns an output stream with the V2XChargingParameters written to
std::ostream& operator<<(std::ostream& os, const V2XChargingParameters& k) {
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
    if (k.acChargingParameters) {
        j["acChargingParameters"] = k.acChargingParameters.value();
    }
    if (k.derChargingParameters) {
        j["derChargingParameters"] = k.derChargingParameters.value();
    }
    if (k.evEnergyOffer) {
        j["evEnergyOffer"] = k.evEnergyOffer.value();
    }
    if (k.dcChargingParameters) {
        j["dcChargingParameters"] = k.dcChargingParameters.value();
    }
    if (k.v2xChargingParameters) {
        j["v2xChargingParameters"] = k.v2xChargingParameters.value();
    }
    if (k.availableEnergyTransfer) {
        j["availableEnergyTransfer"] = json::array();
        for (auto val : k.availableEnergyTransfer.value()) {
            j["availableEnergyTransfer"].push_back(conversions::energy_transfer_mode_enum_to_string(val));
        }
    }
    if (k.controlMode) {
        j["controlMode"] = conversions::control_mode_enum_to_string(k.controlMode.value());
    }
    if (k.mobilityNeedsMode) {
        j["mobilityNeedsMode"] = conversions::mobility_needs_mode_enum_to_string(k.mobilityNeedsMode.value());
    }
    if (k.departureTime) {
        j["departureTime"] = k.departureTime.value().to_rfc3339();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingNeeds \p k
void from_json(const json& j, ChargingNeeds& k) {
    // the required parts of the message
    k.requestedEnergyTransfer = conversions::string_to_energy_transfer_mode_enum(j.at("requestedEnergyTransfer"));

    // the optional parts of the message
    if (j.contains("acChargingParameters")) {
        k.acChargingParameters.emplace(j.at("acChargingParameters"));
    }
    if (j.contains("derChargingParameters")) {
        k.derChargingParameters.emplace(j.at("derChargingParameters"));
    }
    if (j.contains("evEnergyOffer")) {
        k.evEnergyOffer.emplace(j.at("evEnergyOffer"));
    }
    if (j.contains("dcChargingParameters")) {
        k.dcChargingParameters.emplace(j.at("dcChargingParameters"));
    }
    if (j.contains("v2xChargingParameters")) {
        k.v2xChargingParameters.emplace(j.at("v2xChargingParameters"));
    }
    if (j.contains("availableEnergyTransfer")) {
        json arr = j.at("availableEnergyTransfer");
        std::vector<EnergyTransferModeEnum> vec;
        for (auto val : arr) {
            vec.push_back(conversions::string_to_energy_transfer_mode_enum(val));
        }
        k.availableEnergyTransfer.emplace(vec);
    }
    if (j.contains("controlMode")) {
        k.controlMode.emplace(conversions::string_to_control_mode_enum(j.at("controlMode")));
    }
    if (j.contains("mobilityNeedsMode")) {
        k.mobilityNeedsMode.emplace(conversions::string_to_mobility_needs_mode_enum(j.at("mobilityNeedsMode")));
    }
    if (j.contains("departureTime")) {
        k.departureTime.emplace(j.at("departureTime").get<std::string>());
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.severity) {
        j["severity"] = k.severity.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given EventData \p k
void from_json(const json& j, EventData& k) {
    // the required parts of the message
    k.eventId = j.at("eventId");
    k.timestamp = ocpp::DateTime(std::string(j.at("timestamp")));
    k.trigger = conversions::string_to_event_trigger_enum(j.at("trigger"));
    k.actualValue = j.at("actualValue");
    k.component = j.at("component");
    k.eventNotificationType = conversions::string_to_event_notification_enum(j.at("eventNotificationType"));
    k.variable = j.at("variable");

    // the optional parts of the message
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
    if (j.contains("severity")) {
        k.severity.emplace(j.at("severity"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.eventNotificationType) {
        j["eventNotificationType"] = conversions::event_notification_enum_to_string(k.eventNotificationType.value());
    }
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
    if (j.contains("eventNotificationType")) {
        k.eventNotificationType.emplace(conversions::string_to_event_notification_enum(j.at("eventNotificationType")));
    }
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

/// \brief Conversion from a given StreamDataElement \p k to a given json object \p j
void to_json(json& j, const StreamDataElement& k) {
    // the required parts of the message
    j = json{
        {"t", k.t},
        {"v", k.v},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given StreamDataElement \p k
void from_json(const json& j, StreamDataElement& k) {
    // the required parts of the message
    k.t = j.at("t");
    k.v = j.at("v");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given StreamDataElement \p k to the given output stream \p os
/// \returns an output stream with the StreamDataElement written to
std::ostream& operator<<(std::ostream& os, const StreamDataElement& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given NotifyPeriodicEventStream \p k to a given json object \p j
void to_json(json& j, const NotifyPeriodicEventStream& k) {
    // the required parts of the message
    j = json{
        {"data", k.data},
        {"id", k.id},
        {"pending", k.pending},
        {"basetime", k.basetime.to_rfc3339()},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given NotifyPeriodicEventStream \p k
void from_json(const json& j, NotifyPeriodicEventStream& k) {
    // the required parts of the message
    for (auto val : j.at("data")) {
        k.data.push_back(val);
    }
    k.id = j.at("id");
    k.pending = j.at("pending");
    k.basetime = ocpp::DateTime(std::string(j.at("basetime")));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given NotifyPeriodicEventStream \p k to the given output stream \p os
/// \returns an output stream with the NotifyPeriodicEventStream written to
std::ostream& operator<<(std::ostream& os, const NotifyPeriodicEventStream& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given VariableAttribute \p k to a given json object \p j
void to_json(json& j, const VariableAttribute& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
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
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given VariableAttribute \p k
void from_json(const json& j, VariableAttribute& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("type")) {
        k.type.emplace(conversions::string_to_attribute_enum(j.at("type")));
    }
    if (j.contains("value")) {
        const json& value = j.at("value");
        if (value.is_string()) {
            k.value = value;
        } else if (value.is_boolean()) {
            if (value.get<bool>()) {
                // Convert to lower case if that is not the case currently.
                k.value = "true";
            } else {
                // Convert to lower case if that is not the case currently.
                k.value = "false";
            }
        } else if (value.is_array() || value.is_object()) {
            // Maybe this is correct and is just a string (json value string), so just return the string.
            k.value = value.dump();
        } else {
            k.value = value.dump();
        }
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
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.unit) {
        j["unit"] = k.unit.value();
    }
    if (k.minLimit) {
        j["minLimit"] = k.minLimit.value();
    }
    if (k.maxLimit) {
        j["maxLimit"] = k.maxLimit.value();
    }
    if (k.maxElements) {
        j["maxElements"] = k.maxElements.value();
    }
    if (k.valuesList) {
        j["valuesList"] = k.valuesList.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given VariableCharacteristics \p k
void from_json(const json& j, VariableCharacteristics& k) {
    // the required parts of the message
    k.dataType = conversions::string_to_data_enum(j.at("dataType"));
    k.supportsMonitoring = j.at("supportsMonitoring");

    // the optional parts of the message
    if (j.contains("unit")) {
        k.unit.emplace(j.at("unit"));
    }
    if (j.contains("minLimit")) {
        k.minLimit.emplace(j.at("minLimit"));
    }
    if (j.contains("maxLimit")) {
        k.maxLimit.emplace(j.at("maxLimit"));
    }
    if (j.contains("maxElements")) {
        k.maxElements.emplace(j.at("maxElements"));
    }
    if (j.contains("valuesList")) {
        k.valuesList.emplace(j.at("valuesList"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.variableCharacteristics) {
        j["variableCharacteristics"] = k.variableCharacteristics.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
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
    if (j.contains("variableCharacteristics")) {
        k.variableCharacteristics.emplace(j.at("variableCharacteristics"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given ReportData \p k to the given output stream \p os
/// \returns an output stream with the ReportData written to
std::ostream& operator<<(std::ostream& os, const ReportData& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given Address \p k to a given json object \p j
void to_json(json& j, const Address& k) {
    // the required parts of the message
    j = json{
        {"name", k.name},
        {"address1", k.address1},
        {"city", k.city},
        {"country", k.country},
    };
    // the optional parts of the message
    if (k.address2) {
        j["address2"] = k.address2.value();
    }
    if (k.postalCode) {
        j["postalCode"] = k.postalCode.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Address \p k
void from_json(const json& j, Address& k) {
    // the required parts of the message
    k.name = j.at("name");
    k.address1 = j.at("address1");
    k.city = j.at("city");
    k.country = j.at("country");

    // the optional parts of the message
    if (j.contains("address2")) {
        k.address2.emplace(j.at("address2"));
    }
    if (j.contains("postalCode")) {
        k.postalCode.emplace(j.at("postalCode"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given Address \p k to the given output stream \p os
/// \returns an output stream with the Address written to
std::ostream& operator<<(std::ostream& os, const Address& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ChargingScheduleUpdate \p k to a given json object \p j
void to_json(json& j, const ChargingScheduleUpdate& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.limit) {
        j["limit"] = k.limit.value();
    }
    if (k.limit_L2) {
        j["limit_L2"] = k.limit_L2.value();
    }
    if (k.limit_L3) {
        j["limit_L3"] = k.limit_L3.value();
    }
    if (k.dischargeLimit) {
        j["dischargeLimit"] = k.dischargeLimit.value();
    }
    if (k.dischargeLimit_L2) {
        j["dischargeLimit_L2"] = k.dischargeLimit_L2.value();
    }
    if (k.dischargeLimit_L3) {
        j["dischargeLimit_L3"] = k.dischargeLimit_L3.value();
    }
    if (k.setpoint) {
        j["setpoint"] = k.setpoint.value();
    }
    if (k.setpoint_L2) {
        j["setpoint_L2"] = k.setpoint_L2.value();
    }
    if (k.setpoint_L3) {
        j["setpoint_L3"] = k.setpoint_L3.value();
    }
    if (k.setpointReactive) {
        j["setpointReactive"] = k.setpointReactive.value();
    }
    if (k.setpointReactive_L2) {
        j["setpointReactive_L2"] = k.setpointReactive_L2.value();
    }
    if (k.setpointReactive_L3) {
        j["setpointReactive_L3"] = k.setpointReactive_L3.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingScheduleUpdate \p k
void from_json(const json& j, ChargingScheduleUpdate& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("limit")) {
        k.limit.emplace(j.at("limit"));
    }
    if (j.contains("limit_L2")) {
        k.limit_L2.emplace(j.at("limit_L2"));
    }
    if (j.contains("limit_L3")) {
        k.limit_L3.emplace(j.at("limit_L3"));
    }
    if (j.contains("dischargeLimit")) {
        k.dischargeLimit.emplace(j.at("dischargeLimit"));
    }
    if (j.contains("dischargeLimit_L2")) {
        k.dischargeLimit_L2.emplace(j.at("dischargeLimit_L2"));
    }
    if (j.contains("dischargeLimit_L3")) {
        k.dischargeLimit_L3.emplace(j.at("dischargeLimit_L3"));
    }
    if (j.contains("setpoint")) {
        k.setpoint.emplace(j.at("setpoint"));
    }
    if (j.contains("setpoint_L2")) {
        k.setpoint_L2.emplace(j.at("setpoint_L2"));
    }
    if (j.contains("setpoint_L3")) {
        k.setpoint_L3.emplace(j.at("setpoint_L3"));
    }
    if (j.contains("setpointReactive")) {
        k.setpointReactive.emplace(j.at("setpointReactive"));
    }
    if (j.contains("setpointReactive_L2")) {
        k.setpointReactive_L2.emplace(j.at("setpointReactive_L2"));
    }
    if (j.contains("setpointReactive_L3")) {
        k.setpointReactive_L3.emplace(j.at("setpointReactive_L3"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given ChargingScheduleUpdate \p k to the given output stream \p os
/// \returns an output stream with the ChargingScheduleUpdate written to
std::ostream& operator<<(std::ostream& os, const ChargingScheduleUpdate& k) {
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
    if (k.maxOfflineDuration) {
        j["maxOfflineDuration"] = k.maxOfflineDuration.value();
    }
    if (k.invalidAfterOfflineDuration) {
        j["invalidAfterOfflineDuration"] = k.invalidAfterOfflineDuration.value();
    }
    if (k.dynUpdateInterval) {
        j["dynUpdateInterval"] = k.dynUpdateInterval.value();
    }
    if (k.dynUpdateTime) {
        j["dynUpdateTime"] = k.dynUpdateTime.value().to_rfc3339();
    }
    if (k.priceScheduleSignature) {
        j["priceScheduleSignature"] = k.priceScheduleSignature.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
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
    if (j.contains("maxOfflineDuration")) {
        k.maxOfflineDuration.emplace(j.at("maxOfflineDuration"));
    }
    if (j.contains("invalidAfterOfflineDuration")) {
        k.invalidAfterOfflineDuration.emplace(j.at("invalidAfterOfflineDuration"));
    }
    if (j.contains("dynUpdateInterval")) {
        k.dynUpdateInterval.emplace(j.at("dynUpdateInterval"));
    }
    if (j.contains("dynUpdateTime")) {
        k.dynUpdateTime.emplace(j.at("dynUpdateTime").get<std::string>());
    }
    if (j.contains("priceScheduleSignature")) {
        k.priceScheduleSignature.emplace(j.at("priceScheduleSignature"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given ChargingProfile \p k to the given output stream \p os
/// \returns an output stream with the ChargingProfile written to
std::ostream& operator<<(std::ostream& os, const ChargingProfile& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given DERCurvePoints \p k to a given json object \p j
void to_json(json& j, const DERCurvePoints& k) {
    // the required parts of the message
    j = json{
        {"x", k.x},
        {"y", k.y},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given DERCurvePoints \p k
void from_json(const json& j, DERCurvePoints& k) {
    // the required parts of the message
    k.x = j.at("x");
    k.y = j.at("y");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given DERCurvePoints \p k to the given output stream \p os
/// \returns an output stream with the DERCurvePoints written to
std::ostream& operator<<(std::ostream& os, const DERCurvePoints& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given Hysteresis \p k to a given json object \p j
void to_json(json& j, const Hysteresis& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.hysteresisHigh) {
        j["hysteresisHigh"] = k.hysteresisHigh.value();
    }
    if (k.hysteresisLow) {
        j["hysteresisLow"] = k.hysteresisLow.value();
    }
    if (k.hysteresisDelay) {
        j["hysteresisDelay"] = k.hysteresisDelay.value();
    }
    if (k.hysteresisGradient) {
        j["hysteresisGradient"] = k.hysteresisGradient.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Hysteresis \p k
void from_json(const json& j, Hysteresis& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("hysteresisHigh")) {
        k.hysteresisHigh.emplace(j.at("hysteresisHigh"));
    }
    if (j.contains("hysteresisLow")) {
        k.hysteresisLow.emplace(j.at("hysteresisLow"));
    }
    if (j.contains("hysteresisDelay")) {
        k.hysteresisDelay.emplace(j.at("hysteresisDelay"));
    }
    if (j.contains("hysteresisGradient")) {
        k.hysteresisGradient.emplace(j.at("hysteresisGradient"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given Hysteresis \p k to the given output stream \p os
/// \returns an output stream with the Hysteresis written to
std::ostream& operator<<(std::ostream& os, const Hysteresis& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ReactivePowerParams \p k to a given json object \p j
void to_json(json& j, const ReactivePowerParams& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.vRef) {
        j["vRef"] = k.vRef.value();
    }
    if (k.autonomousVRefEnable) {
        j["autonomousVRefEnable"] = k.autonomousVRefEnable.value();
    }
    if (k.autonomousVRefTimeConstant) {
        j["autonomousVRefTimeConstant"] = k.autonomousVRefTimeConstant.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ReactivePowerParams \p k
void from_json(const json& j, ReactivePowerParams& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("vRef")) {
        k.vRef.emplace(j.at("vRef"));
    }
    if (j.contains("autonomousVRefEnable")) {
        k.autonomousVRefEnable.emplace(j.at("autonomousVRefEnable"));
    }
    if (j.contains("autonomousVRefTimeConstant")) {
        k.autonomousVRefTimeConstant.emplace(j.at("autonomousVRefTimeConstant"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given ReactivePowerParams \p k to the given output stream \p os
/// \returns an output stream with the ReactivePowerParams written to
std::ostream& operator<<(std::ostream& os, const ReactivePowerParams& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given VoltageParams \p k to a given json object \p j
void to_json(json& j, const VoltageParams& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.hv10MinMeanValue) {
        j["hv10MinMeanValue"] = k.hv10MinMeanValue.value();
    }
    if (k.hv10MinMeanTripDelay) {
        j["hv10MinMeanTripDelay"] = k.hv10MinMeanTripDelay.value();
    }
    if (k.powerDuringCessation) {
        j["powerDuringCessation"] = conversions::power_during_cessation_enum_to_string(k.powerDuringCessation.value());
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given VoltageParams \p k
void from_json(const json& j, VoltageParams& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("hv10MinMeanValue")) {
        k.hv10MinMeanValue.emplace(j.at("hv10MinMeanValue"));
    }
    if (j.contains("hv10MinMeanTripDelay")) {
        k.hv10MinMeanTripDelay.emplace(j.at("hv10MinMeanTripDelay"));
    }
    if (j.contains("powerDuringCessation")) {
        k.powerDuringCessation.emplace(
            conversions::string_to_power_during_cessation_enum(j.at("powerDuringCessation")));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given VoltageParams \p k to the given output stream \p os
/// \returns an output stream with the VoltageParams written to
std::ostream& operator<<(std::ostream& os, const VoltageParams& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given DERCurve \p k to a given json object \p j
void to_json(json& j, const DERCurve& k) {
    // the required parts of the message
    j = json{
        {"curveData", k.curveData},
        {"priority", k.priority},
        {"yUnit", conversions::derunit_enum_to_string(k.yUnit)},
    };
    // the optional parts of the message
    if (k.hysteresis) {
        j["hysteresis"] = k.hysteresis.value();
    }
    if (k.reactivePowerParams) {
        j["reactivePowerParams"] = k.reactivePowerParams.value();
    }
    if (k.voltageParams) {
        j["voltageParams"] = k.voltageParams.value();
    }
    if (k.responseTime) {
        j["responseTime"] = k.responseTime.value();
    }
    if (k.startTime) {
        j["startTime"] = k.startTime.value().to_rfc3339();
    }
    if (k.duration) {
        j["duration"] = k.duration.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given DERCurve \p k
void from_json(const json& j, DERCurve& k) {
    // the required parts of the message
    for (auto val : j.at("curveData")) {
        k.curveData.push_back(val);
    }
    k.priority = j.at("priority");
    k.yUnit = conversions::string_to_derunit_enum(j.at("yUnit"));

    // the optional parts of the message
    if (j.contains("hysteresis")) {
        k.hysteresis.emplace(j.at("hysteresis"));
    }
    if (j.contains("reactivePowerParams")) {
        k.reactivePowerParams.emplace(j.at("reactivePowerParams"));
    }
    if (j.contains("voltageParams")) {
        k.voltageParams.emplace(j.at("voltageParams"));
    }
    if (j.contains("responseTime")) {
        k.responseTime.emplace(j.at("responseTime"));
    }
    if (j.contains("startTime")) {
        k.startTime.emplace(j.at("startTime").get<std::string>());
    }
    if (j.contains("duration")) {
        k.duration.emplace(j.at("duration"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given DERCurve \p k to the given output stream \p os
/// \returns an output stream with the DERCurve written to
std::ostream& operator<<(std::ostream& os, const DERCurve& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given DERCurveGet \p k to a given json object \p j
void to_json(json& j, const DERCurveGet& k) {
    // the required parts of the message
    j = json{
        {"curve", k.curve},
        {"id", k.id},
        {"curveType", conversions::dercontrol_enum_to_string(k.curveType)},
        {"isDefault", k.isDefault},
        {"isSuperseded", k.isSuperseded},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given DERCurveGet \p k
void from_json(const json& j, DERCurveGet& k) {
    // the required parts of the message
    k.curve = j.at("curve");
    k.id = j.at("id");
    k.curveType = conversions::string_to_dercontrol_enum(j.at("curveType"));
    k.isDefault = j.at("isDefault");
    k.isSuperseded = j.at("isSuperseded");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given DERCurveGet \p k to the given output stream \p os
/// \returns an output stream with the DERCurveGet written to
std::ostream& operator<<(std::ostream& os, const DERCurveGet& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given EnterService \p k to a given json object \p j
void to_json(json& j, const EnterService& k) {
    // the required parts of the message
    j = json{
        {"priority", k.priority}, {"highVoltage", k.highVoltage}, {"lowVoltage", k.lowVoltage},
        {"highFreq", k.highFreq}, {"lowFreq", k.lowFreq},
    };
    // the optional parts of the message
    if (k.delay) {
        j["delay"] = k.delay.value();
    }
    if (k.randomDelay) {
        j["randomDelay"] = k.randomDelay.value();
    }
    if (k.rampRate) {
        j["rampRate"] = k.rampRate.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given EnterService \p k
void from_json(const json& j, EnterService& k) {
    // the required parts of the message
    k.priority = j.at("priority");
    k.highVoltage = j.at("highVoltage");
    k.lowVoltage = j.at("lowVoltage");
    k.highFreq = j.at("highFreq");
    k.lowFreq = j.at("lowFreq");

    // the optional parts of the message
    if (j.contains("delay")) {
        k.delay.emplace(j.at("delay"));
    }
    if (j.contains("randomDelay")) {
        k.randomDelay.emplace(j.at("randomDelay"));
    }
    if (j.contains("rampRate")) {
        k.rampRate.emplace(j.at("rampRate"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given EnterService \p k to the given output stream \p os
/// \returns an output stream with the EnterService written to
std::ostream& operator<<(std::ostream& os, const EnterService& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given EnterServiceGet \p k to a given json object \p j
void to_json(json& j, const EnterServiceGet& k) {
    // the required parts of the message
    j = json{
        {"enterService", k.enterService},
        {"id", k.id},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given EnterServiceGet \p k
void from_json(const json& j, EnterServiceGet& k) {
    // the required parts of the message
    k.enterService = j.at("enterService");
    k.id = j.at("id");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given EnterServiceGet \p k to the given output stream \p os
/// \returns an output stream with the EnterServiceGet written to
std::ostream& operator<<(std::ostream& os, const EnterServiceGet& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given FixedPF \p k to a given json object \p j
void to_json(json& j, const FixedPF& k) {
    // the required parts of the message
    j = json{
        {"priority", k.priority},
        {"displacement", k.displacement},
        {"excitation", k.excitation},
    };
    // the optional parts of the message
    if (k.startTime) {
        j["startTime"] = k.startTime.value().to_rfc3339();
    }
    if (k.duration) {
        j["duration"] = k.duration.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given FixedPF \p k
void from_json(const json& j, FixedPF& k) {
    // the required parts of the message
    k.priority = j.at("priority");
    k.displacement = j.at("displacement");
    k.excitation = j.at("excitation");

    // the optional parts of the message
    if (j.contains("startTime")) {
        k.startTime.emplace(j.at("startTime").get<std::string>());
    }
    if (j.contains("duration")) {
        k.duration.emplace(j.at("duration"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given FixedPF \p k to the given output stream \p os
/// \returns an output stream with the FixedPF written to
std::ostream& operator<<(std::ostream& os, const FixedPF& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given FixedPFGet \p k to a given json object \p j
void to_json(json& j, const FixedPFGet& k) {
    // the required parts of the message
    j = json{
        {"fixedPF", k.fixedPF},
        {"id", k.id},
        {"isDefault", k.isDefault},
        {"isSuperseded", k.isSuperseded},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given FixedPFGet \p k
void from_json(const json& j, FixedPFGet& k) {
    // the required parts of the message
    k.fixedPF = j.at("fixedPF");
    k.id = j.at("id");
    k.isDefault = j.at("isDefault");
    k.isSuperseded = j.at("isSuperseded");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given FixedPFGet \p k to the given output stream \p os
/// \returns an output stream with the FixedPFGet written to
std::ostream& operator<<(std::ostream& os, const FixedPFGet& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given FixedVar \p k to a given json object \p j
void to_json(json& j, const FixedVar& k) {
    // the required parts of the message
    j = json{
        {"priority", k.priority},
        {"setpoint", k.setpoint},
        {"unit", conversions::derunit_enum_to_string(k.unit)},
    };
    // the optional parts of the message
    if (k.startTime) {
        j["startTime"] = k.startTime.value().to_rfc3339();
    }
    if (k.duration) {
        j["duration"] = k.duration.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given FixedVar \p k
void from_json(const json& j, FixedVar& k) {
    // the required parts of the message
    k.priority = j.at("priority");
    k.setpoint = j.at("setpoint");
    k.unit = conversions::string_to_derunit_enum(j.at("unit"));

    // the optional parts of the message
    if (j.contains("startTime")) {
        k.startTime.emplace(j.at("startTime").get<std::string>());
    }
    if (j.contains("duration")) {
        k.duration.emplace(j.at("duration"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given FixedVar \p k to the given output stream \p os
/// \returns an output stream with the FixedVar written to
std::ostream& operator<<(std::ostream& os, const FixedVar& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given FixedVarGet \p k to a given json object \p j
void to_json(json& j, const FixedVarGet& k) {
    // the required parts of the message
    j = json{
        {"fixedVar", k.fixedVar},
        {"id", k.id},
        {"isDefault", k.isDefault},
        {"isSuperseded", k.isSuperseded},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given FixedVarGet \p k
void from_json(const json& j, FixedVarGet& k) {
    // the required parts of the message
    k.fixedVar = j.at("fixedVar");
    k.id = j.at("id");
    k.isDefault = j.at("isDefault");
    k.isSuperseded = j.at("isSuperseded");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given FixedVarGet \p k to the given output stream \p os
/// \returns an output stream with the FixedVarGet written to
std::ostream& operator<<(std::ostream& os, const FixedVarGet& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given FreqDroop \p k to a given json object \p j
void to_json(json& j, const FreqDroop& k) {
    // the required parts of the message
    j = json{
        {"priority", k.priority},   {"overFreq", k.overFreq},     {"underFreq", k.underFreq},
        {"overDroop", k.overDroop}, {"underDroop", k.underDroop}, {"responseTime", k.responseTime},
    };
    // the optional parts of the message
    if (k.startTime) {
        j["startTime"] = k.startTime.value().to_rfc3339();
    }
    if (k.duration) {
        j["duration"] = k.duration.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given FreqDroop \p k
void from_json(const json& j, FreqDroop& k) {
    // the required parts of the message
    k.priority = j.at("priority");
    k.overFreq = j.at("overFreq");
    k.underFreq = j.at("underFreq");
    k.overDroop = j.at("overDroop");
    k.underDroop = j.at("underDroop");
    k.responseTime = j.at("responseTime");

    // the optional parts of the message
    if (j.contains("startTime")) {
        k.startTime.emplace(j.at("startTime").get<std::string>());
    }
    if (j.contains("duration")) {
        k.duration.emplace(j.at("duration"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given FreqDroop \p k to the given output stream \p os
/// \returns an output stream with the FreqDroop written to
std::ostream& operator<<(std::ostream& os, const FreqDroop& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given FreqDroopGet \p k to a given json object \p j
void to_json(json& j, const FreqDroopGet& k) {
    // the required parts of the message
    j = json{
        {"freqDroop", k.freqDroop},
        {"id", k.id},
        {"isDefault", k.isDefault},
        {"isSuperseded", k.isSuperseded},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given FreqDroopGet \p k
void from_json(const json& j, FreqDroopGet& k) {
    // the required parts of the message
    k.freqDroop = j.at("freqDroop");
    k.id = j.at("id");
    k.isDefault = j.at("isDefault");
    k.isSuperseded = j.at("isSuperseded");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given FreqDroopGet \p k to the given output stream \p os
/// \returns an output stream with the FreqDroopGet written to
std::ostream& operator<<(std::ostream& os, const FreqDroopGet& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given Gradient \p k to a given json object \p j
void to_json(json& j, const Gradient& k) {
    // the required parts of the message
    j = json{
        {"priority", k.priority},
        {"gradient", k.gradient},
        {"softGradient", k.softGradient},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Gradient \p k
void from_json(const json& j, Gradient& k) {
    // the required parts of the message
    k.priority = j.at("priority");
    k.gradient = j.at("gradient");
    k.softGradient = j.at("softGradient");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given Gradient \p k to the given output stream \p os
/// \returns an output stream with the Gradient written to
std::ostream& operator<<(std::ostream& os, const Gradient& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given GradientGet \p k to a given json object \p j
void to_json(json& j, const GradientGet& k) {
    // the required parts of the message
    j = json{
        {"gradient", k.gradient},
        {"id", k.id},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given GradientGet \p k
void from_json(const json& j, GradientGet& k) {
    // the required parts of the message
    k.gradient = j.at("gradient");
    k.id = j.at("id");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given GradientGet \p k to the given output stream \p os
/// \returns an output stream with the GradientGet written to
std::ostream& operator<<(std::ostream& os, const GradientGet& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given LimitMaxDischarge \p k to a given json object \p j
void to_json(json& j, const LimitMaxDischarge& k) {
    // the required parts of the message
    j = json{
        {"priority", k.priority},
    };
    // the optional parts of the message
    if (k.pctMaxDischargePower) {
        j["pctMaxDischargePower"] = k.pctMaxDischargePower.value();
    }
    if (k.powerMonitoringMustTrip) {
        j["powerMonitoringMustTrip"] = k.powerMonitoringMustTrip.value();
    }
    if (k.startTime) {
        j["startTime"] = k.startTime.value().to_rfc3339();
    }
    if (k.duration) {
        j["duration"] = k.duration.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given LimitMaxDischarge \p k
void from_json(const json& j, LimitMaxDischarge& k) {
    // the required parts of the message
    k.priority = j.at("priority");

    // the optional parts of the message
    if (j.contains("pctMaxDischargePower")) {
        k.pctMaxDischargePower.emplace(j.at("pctMaxDischargePower"));
    }
    if (j.contains("powerMonitoringMustTrip")) {
        k.powerMonitoringMustTrip.emplace(j.at("powerMonitoringMustTrip"));
    }
    if (j.contains("startTime")) {
        k.startTime.emplace(j.at("startTime").get<std::string>());
    }
    if (j.contains("duration")) {
        k.duration.emplace(j.at("duration"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given LimitMaxDischarge \p k to the given output stream \p os
/// \returns an output stream with the LimitMaxDischarge written to
std::ostream& operator<<(std::ostream& os, const LimitMaxDischarge& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given LimitMaxDischargeGet \p k to a given json object \p j
void to_json(json& j, const LimitMaxDischargeGet& k) {
    // the required parts of the message
    j = json{
        {"id", k.id},
        {"isDefault", k.isDefault},
        {"isSuperseded", k.isSuperseded},
        {"limitMaxDischarge", k.limitMaxDischarge},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given LimitMaxDischargeGet \p k
void from_json(const json& j, LimitMaxDischargeGet& k) {
    // the required parts of the message
    k.id = j.at("id");
    k.isDefault = j.at("isDefault");
    k.isSuperseded = j.at("isSuperseded");
    k.limitMaxDischarge = j.at("limitMaxDischarge");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given LimitMaxDischargeGet \p k to the given output stream \p os
/// \returns an output stream with the LimitMaxDischargeGet written to
std::ostream& operator<<(std::ostream& os, const LimitMaxDischargeGet& k) {
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
    if (k.idTokenInfo) {
        j["idTokenInfo"] = k.idTokenInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given AuthorizationData \p k
void from_json(const json& j, AuthorizationData& k) {
    // the required parts of the message
    k.idToken = j.at("idToken");

    // the optional parts of the message
    if (j.contains("idTokenInfo")) {
        k.idTokenInfo.emplace(j.at("idTokenInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given APN \p k
void from_json(const json& j, APN& k) {
    // the required parts of the message
    k.apn = j.at("apn");
    k.apnAuthentication = conversions::string_to_apnauthentication_enum(j.at("apnAuthentication"));

    // the optional parts of the message
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
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.group) {
        j["group"] = k.group.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
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
    if (j.contains("group")) {
        k.group.emplace(j.at("group"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
        {"ocppInterface", conversions::ocppinterface_enum_to_string(k.ocppInterface)},
        {"ocppTransport", conversions::ocpptransport_enum_to_string(k.ocppTransport)},
        {"messageTimeout", k.messageTimeout},
        {"ocppCsmsUrl", k.ocppCsmsUrl},
        {"securityProfile", k.securityProfile},
    };
    // the optional parts of the message
    if (k.apn) {
        j["apn"] = k.apn.value();
    }
    if (k.ocppVersion) {
        j["ocppVersion"] = conversions::ocppversion_enum_to_string(k.ocppVersion.value());
    }
    if (k.identity) {
        j["identity"] = k.identity.value();
    }
    if (k.basicAuthPassword) {
        j["basicAuthPassword"] = k.basicAuthPassword.value();
    }
    if (k.vpn) {
        j["vpn"] = k.vpn.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given NetworkConnectionProfile \p k
void from_json(const json& j, NetworkConnectionProfile& k) {
    // the required parts of the message
    k.ocppInterface = conversions::string_to_ocppinterface_enum(j.at("ocppInterface"));
    k.ocppTransport = conversions::string_to_ocpptransport_enum(j.at("ocppTransport"));
    k.messageTimeout = j.at("messageTimeout");
    k.ocppCsmsUrl = j.at("ocppCsmsUrl");
    k.securityProfile = j.at("securityProfile");

    // the optional parts of the message
    if (j.contains("apn")) {
        k.apn.emplace(j.at("apn"));
    }
    if (j.contains("ocppVersion")) {
        k.ocppVersion.emplace(conversions::string_to_ocppversion_enum(j.at("ocppVersion")));
    }
    if (j.contains("identity")) {
        k.identity.emplace(j.at("identity"));
    }
    if (j.contains("basicAuthPassword")) {
        k.basicAuthPassword.emplace(j.at("basicAuthPassword"));
    }
    if (j.contains("vpn")) {
        k.vpn.emplace(j.at("vpn"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.id) {
        j["id"] = k.id.value();
    }
    if (k.periodicEventStream) {
        j["periodicEventStream"] = k.periodicEventStream.value();
    }
    if (k.transaction) {
        j["transaction"] = k.transaction.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
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
    if (j.contains("id")) {
        k.id.emplace(j.at("id"));
    }
    if (j.contains("periodicEventStream")) {
        k.periodicEventStream.emplace(j.at("periodicEventStream"));
    }
    if (j.contains("transaction")) {
        k.transaction.emplace(j.at("transaction"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.id) {
        j["id"] = k.id.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
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
    if (j.contains("id")) {
        k.id.emplace(j.at("id"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.attributeType) {
        j["attributeType"] = conversions::attribute_enum_to_string(k.attributeType.value());
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given SetVariableData \p k
void from_json(const json& j, SetVariableData& k) {
    // the required parts of the message
    k.attributeValue = j.at("attributeValue");
    k.component = j.at("component");
    k.variable = j.at("variable");

    // the optional parts of the message
    if (j.contains("attributeType")) {
        k.attributeType.emplace(conversions::string_to_attribute_enum(j.at("attributeType")));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.attributeType) {
        j["attributeType"] = conversions::attribute_enum_to_string(k.attributeType.value());
    }
    if (k.attributeStatusInfo) {
        j["attributeStatusInfo"] = k.attributeStatusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given SetVariableResult \p k
void from_json(const json& j, SetVariableResult& k) {
    // the required parts of the message
    k.attributeStatus = conversions::string_to_set_variable_status_enum(j.at("attributeStatus"));
    k.component = j.at("component");
    k.variable = j.at("variable");

    // the optional parts of the message
    if (j.contains("attributeType")) {
        k.attributeType.emplace(conversions::string_to_attribute_enum(j.at("attributeType")));
    }
    if (j.contains("attributeStatusInfo")) {
        k.attributeStatusInfo.emplace(j.at("attributeStatusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given SetVariableResult \p k to the given output stream \p os
/// \returns an output stream with the SetVariableResult written to
std::ostream& operator<<(std::ostream& os, const SetVariableResult& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given CostDimension \p k to a given json object \p j
void to_json(json& j, const CostDimension& k) {
    // the required parts of the message
    j = json{
        {"type", conversions::cost_dimension_enum_to_string(k.type)},
        {"volume", k.volume},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given CostDimension \p k
void from_json(const json& j, CostDimension& k) {
    // the required parts of the message
    k.type = conversions::string_to_cost_dimension_enum(j.at("type"));
    k.volume = j.at("volume");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given CostDimension \p k to the given output stream \p os
/// \returns an output stream with the CostDimension written to
std::ostream& operator<<(std::ostream& os, const CostDimension& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given ChargingPeriod \p k to a given json object \p j
void to_json(json& j, const ChargingPeriod& k) {
    // the required parts of the message
    j = json{
        {"startPeriod", k.startPeriod.to_rfc3339()},
    };
    // the optional parts of the message
    if (k.dimensions) {
        j["dimensions"] = json::array();
        for (auto val : k.dimensions.value()) {
            j["dimensions"].push_back(val);
        }
    }
    if (k.tariffId) {
        j["tariffId"] = k.tariffId.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given ChargingPeriod \p k
void from_json(const json& j, ChargingPeriod& k) {
    // the required parts of the message
    k.startPeriod = ocpp::DateTime(std::string(j.at("startPeriod")));

    // the optional parts of the message
    if (j.contains("dimensions")) {
        json arr = j.at("dimensions");
        std::vector<CostDimension> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.dimensions.emplace(vec);
    }
    if (j.contains("tariffId")) {
        k.tariffId.emplace(j.at("tariffId"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given ChargingPeriod \p k to the given output stream \p os
/// \returns an output stream with the ChargingPeriod written to
std::ostream& operator<<(std::ostream& os, const ChargingPeriod& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TotalPrice \p k to a given json object \p j
void to_json(json& j, const TotalPrice& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.exclTax) {
        j["exclTax"] = k.exclTax.value();
    }
    if (k.inclTax) {
        j["inclTax"] = k.inclTax.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TotalPrice \p k
void from_json(const json& j, TotalPrice& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("exclTax")) {
        k.exclTax.emplace(j.at("exclTax"));
    }
    if (j.contains("inclTax")) {
        k.inclTax.emplace(j.at("inclTax"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TotalPrice \p k to the given output stream \p os
/// \returns an output stream with the TotalPrice written to
std::ostream& operator<<(std::ostream& os, const TotalPrice& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TotalCost \p k to a given json object \p j
void to_json(json& j, const TotalCost& k) {
    // the required parts of the message
    j = json{
        {"currency", k.currency},
        {"typeOfCost", conversions::tariff_cost_enum_to_string(k.typeOfCost)},
        {"total", k.total},
    };
    // the optional parts of the message
    if (k.fixed) {
        j["fixed"] = k.fixed.value();
    }
    if (k.energy) {
        j["energy"] = k.energy.value();
    }
    if (k.chargingTime) {
        j["chargingTime"] = k.chargingTime.value();
    }
    if (k.idleTime) {
        j["idleTime"] = k.idleTime.value();
    }
    if (k.reservationTime) {
        j["reservationTime"] = k.reservationTime.value();
    }
    if (k.reservationFixed) {
        j["reservationFixed"] = k.reservationFixed.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TotalCost \p k
void from_json(const json& j, TotalCost& k) {
    // the required parts of the message
    k.currency = j.at("currency");
    k.typeOfCost = conversions::string_to_tariff_cost_enum(j.at("typeOfCost"));
    k.total = j.at("total");

    // the optional parts of the message
    if (j.contains("fixed")) {
        k.fixed.emplace(j.at("fixed"));
    }
    if (j.contains("energy")) {
        k.energy.emplace(j.at("energy"));
    }
    if (j.contains("chargingTime")) {
        k.chargingTime.emplace(j.at("chargingTime"));
    }
    if (j.contains("idleTime")) {
        k.idleTime.emplace(j.at("idleTime"));
    }
    if (j.contains("reservationTime")) {
        k.reservationTime.emplace(j.at("reservationTime"));
    }
    if (j.contains("reservationFixed")) {
        k.reservationFixed.emplace(j.at("reservationFixed"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TotalCost \p k to the given output stream \p os
/// \returns an output stream with the TotalCost written to
std::ostream& operator<<(std::ostream& os, const TotalCost& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TotalUsage \p k to a given json object \p j
void to_json(json& j, const TotalUsage& k) {
    // the required parts of the message
    j = json{
        {"energy", k.energy},
        {"chargingTime", k.chargingTime},
        {"idleTime", k.idleTime},
    };
    // the optional parts of the message
    if (k.reservationTime) {
        j["reservationTime"] = k.reservationTime.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TotalUsage \p k
void from_json(const json& j, TotalUsage& k) {
    // the required parts of the message
    k.energy = j.at("energy");
    k.chargingTime = j.at("chargingTime");
    k.idleTime = j.at("idleTime");

    // the optional parts of the message
    if (j.contains("reservationTime")) {
        k.reservationTime.emplace(j.at("reservationTime"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TotalUsage \p k to the given output stream \p os
/// \returns an output stream with the TotalUsage written to
std::ostream& operator<<(std::ostream& os, const TotalUsage& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given CostDetails \p k to a given json object \p j
void to_json(json& j, const CostDetails& k) {
    // the required parts of the message
    j = json{
        {"totalCost", k.totalCost},
        {"totalUsage", k.totalUsage},
    };
    // the optional parts of the message
    if (k.chargingPeriods) {
        j["chargingPeriods"] = json::array();
        for (auto val : k.chargingPeriods.value()) {
            j["chargingPeriods"].push_back(val);
        }
    }
    if (k.failureToCalculate) {
        j["failureToCalculate"] = k.failureToCalculate.value();
    }
    if (k.failureReason) {
        j["failureReason"] = k.failureReason.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given CostDetails \p k
void from_json(const json& j, CostDetails& k) {
    // the required parts of the message
    k.totalCost = j.at("totalCost");
    k.totalUsage = j.at("totalUsage");

    // the optional parts of the message
    if (j.contains("chargingPeriods")) {
        json arr = j.at("chargingPeriods");
        std::vector<ChargingPeriod> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.chargingPeriods.emplace(vec);
    }
    if (j.contains("failureToCalculate")) {
        k.failureToCalculate.emplace(j.at("failureToCalculate"));
    }
    if (j.contains("failureReason")) {
        k.failureReason.emplace(j.at("failureReason"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given CostDetails \p k to the given output stream \p os
/// \returns an output stream with the CostDetails written to
std::ostream& operator<<(std::ostream& os, const CostDetails& k) {
    os << json(k).dump(4);
    return os;
}

/// \brief Conversion from a given TransactionLimit \p k to a given json object \p j
void to_json(json& j, const TransactionLimit& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.maxCost) {
        j["maxCost"] = k.maxCost.value();
    }
    if (k.maxEnergy) {
        j["maxEnergy"] = k.maxEnergy.value();
    }
    if (k.maxTime) {
        j["maxTime"] = k.maxTime.value();
    }
    if (k.maxSoC) {
        j["maxSoC"] = k.maxSoC.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given TransactionLimit \p k
void from_json(const json& j, TransactionLimit& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("maxCost")) {
        k.maxCost.emplace(j.at("maxCost"));
    }
    if (j.contains("maxEnergy")) {
        k.maxEnergy.emplace(j.at("maxEnergy"));
    }
    if (j.contains("maxTime")) {
        k.maxTime.emplace(j.at("maxTime"));
    }
    if (j.contains("maxSoC")) {
        k.maxSoC.emplace(j.at("maxSoC"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given TransactionLimit \p k to the given output stream \p os
/// \returns an output stream with the TransactionLimit written to
std::ostream& operator<<(std::ostream& os, const TransactionLimit& k) {
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
    if (k.operationMode) {
        j["operationMode"] = conversions::operation_mode_enum_to_string(k.operationMode.value());
    }
    if (k.tariffId) {
        j["tariffId"] = k.tariffId.value();
    }
    if (k.transactionLimit) {
        j["transactionLimit"] = k.transactionLimit.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Transaction \p k
void from_json(const json& j, Transaction& k) {
    // the required parts of the message
    k.transactionId = j.at("transactionId");

    // the optional parts of the message
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
    if (j.contains("operationMode")) {
        k.operationMode.emplace(conversions::string_to_operation_mode_enum(j.at("operationMode")));
    }
    if (j.contains("tariffId")) {
        k.tariffId.emplace(j.at("tariffId"));
    }
    if (j.contains("transactionLimit")) {
        k.transactionLimit.emplace(j.at("transactionLimit"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
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
    if (k.installDateTime) {
        j["installDateTime"] = k.installDateTime.value().to_rfc3339();
    }
    if (k.signingCertificate) {
        j["signingCertificate"] = k.signingCertificate.value();
    }
    if (k.signature) {
        j["signature"] = k.signature.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

/// \brief Conversion from a given json object \p j to a given Firmware \p k
void from_json(const json& j, Firmware& k) {
    // the required parts of the message
    k.location = j.at("location");
    k.retrieveDateTime = ocpp::DateTime(std::string(j.at("retrieveDateTime")));

    // the optional parts of the message
    if (j.contains("installDateTime")) {
        k.installDateTime.emplace(j.at("installDateTime").get<std::string>());
    }
    if (j.contains("signingCertificate")) {
        k.signingCertificate.emplace(j.at("signingCertificate"));
    }
    if (j.contains("signature")) {
        k.signature.emplace(j.at("signature"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

// \brief Writes the string representation of the given Firmware \p k to the given output stream \p os
/// \returns an output stream with the Firmware written to
std::ostream& operator<<(std::ostream& os, const Firmware& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
