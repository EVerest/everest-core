// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/GetTariffs.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string GetTariffsRequest::get_type() const {
    return "GetTariffs";
}

void to_json(json& j, const GetTariffsRequest& k) {
    // the required parts of the message
    j = json{
        {"evseId", k.evseId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetTariffsRequest& k) {
    // the required parts of the message
    k.evseId = j.at("evseId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetTariffsRequest \p k to the given output stream \p os
/// \returns an output stream with the GetTariffsRequest written to
std::ostream& operator<<(std::ostream& os, const GetTariffsRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetTariffsResponse::get_type() const {
    return "GetTariffsResponse";
}

void to_json(json& j, const GetTariffsResponse& k) {
    // the required parts of the message
    j = json{
        {"status", ocpp::v2::conversions::tariff_get_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.tariffAssignments) {
        j["tariffAssignments"] = json::array();
        for (const auto& val : k.tariffAssignments.value()) {
            j["tariffAssignments"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetTariffsResponse& k) {
    // the required parts of the message
    k.status = ocpp::v2::conversions::string_to_tariff_get_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("tariffAssignments")) {
        const json& arr = j.at("tariffAssignments");
        std::vector<TariffAssignment> vec;
        for (const auto& val : arr) {
            vec.push_back(val);
        }
        k.tariffAssignments.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetTariffsResponse \p k to the given output stream \p os
/// \returns an output stream with the GetTariffsResponse written to
std::ostream& operator<<(std::ostream& os, const GetTariffsResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
