// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/ClearTariffs.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string ClearTariffsRequest::get_type() const {
    return "ClearTariffs";
}

void to_json(json& j, const ClearTariffsRequest& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.tariffIds) {
        if (j.size() == 0) {
            j = json{{"tariffIds", json::array()}};
        } else {
            j["tariffIds"] = json::array();
        }
        for (auto val : k.tariffIds.value()) {
            j["tariffIds"].push_back(val);
        }
    }
    if (k.evseId) {
        j["evseId"] = k.evseId.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ClearTariffsRequest& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("tariffIds")) {
        json arr = j.at("tariffIds");
        std::vector<CiString<60>> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.tariffIds.emplace(vec);
    }
    if (j.contains("evseId")) {
        k.evseId.emplace(j.at("evseId"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ClearTariffsRequest \p k to the given output stream \p os
/// \returns an output stream with the ClearTariffsRequest written to
std::ostream& operator<<(std::ostream& os, const ClearTariffsRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ClearTariffsResponse::get_type() const {
    return "ClearTariffsResponse";
}

void to_json(json& j, const ClearTariffsResponse& k) {
    // the required parts of the message
    j = json{
        {"clearTariffsResult", k.clearTariffsResult},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ClearTariffsResponse& k) {
    // the required parts of the message
    for (auto val : j.at("clearTariffsResult")) {
        k.clearTariffsResult.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ClearTariffsResponse \p k to the given output stream \p os
/// \returns an output stream with the ClearTariffsResponse written to
std::ostream& operator<<(std::ostream& os, const ClearTariffsResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
