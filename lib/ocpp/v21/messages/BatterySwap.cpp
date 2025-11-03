// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/BatterySwap.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string BatterySwapRequest::get_type() const {
    return "BatterySwap";
}

void to_json(json& j, const BatterySwapRequest& k) {
    // the required parts of the message
    j = json{
        {"batteryData", k.batteryData},
        {"eventType", ocpp::v2::conversions::battery_swap_event_enum_to_string(k.eventType)},
        {"idToken", k.idToken},
        {"requestId", k.requestId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, BatterySwapRequest& k) {
    // the required parts of the message
    for (auto val : j.at("batteryData")) {
        k.batteryData.push_back(val);
    }
    k.eventType = ocpp::v2::conversions::string_to_battery_swap_event_enum(j.at("eventType"));
    k.idToken = j.at("idToken");
    k.requestId = j.at("requestId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given BatterySwapRequest \p k to the given output stream \p os
/// \returns an output stream with the BatterySwapRequest written to
std::ostream& operator<<(std::ostream& os, const BatterySwapRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string BatterySwapResponse::get_type() const {
    return "BatterySwapResponse";
}

void to_json(json& j, const BatterySwapResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, BatterySwapResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given BatterySwapResponse \p k to the given output stream \p os
/// \returns an output stream with the BatterySwapResponse written to
std::ostream& operator<<(std::ostream& os, const BatterySwapResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
