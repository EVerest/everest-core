// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/ClearedChargingLimit.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string ClearedChargingLimitRequest::get_type() const {
    return "ClearedChargingLimit";
}

void to_json(json& j, const ClearedChargingLimitRequest& k) {
    // the required parts of the message
    j = json{
        {"chargingLimitSource", k.chargingLimitSource},
    };
    // the optional parts of the message
    if (k.evseId) {
        j["evseId"] = k.evseId.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ClearedChargingLimitRequest& k) {
    // the required parts of the message
    k.chargingLimitSource = j.at("chargingLimitSource");

    // the optional parts of the message
    if (j.contains("evseId")) {
        k.evseId.emplace(j.at("evseId"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ClearedChargingLimitRequest \p k to the given output stream \p
/// os
/// \returns an output stream with the ClearedChargingLimitRequest written to
std::ostream& operator<<(std::ostream& os, const ClearedChargingLimitRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ClearedChargingLimitResponse::get_type() const {
    return "ClearedChargingLimitResponse";
}

void to_json(json& j, const ClearedChargingLimitResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ClearedChargingLimitResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ClearedChargingLimitResponse \p k to the given output stream \p
/// os
/// \returns an output stream with the ClearedChargingLimitResponse written to
std::ostream& operator<<(std::ostream& os, const ClearedChargingLimitResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
