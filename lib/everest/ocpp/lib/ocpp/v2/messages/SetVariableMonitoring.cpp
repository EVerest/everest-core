// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/SetVariableMonitoring.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string SetVariableMonitoringRequest::get_type() const {
    return "SetVariableMonitoring";
}

void to_json(json& j, const SetVariableMonitoringRequest& k) {
    // the required parts of the message
    j = json{
        {"setMonitoringData", k.setMonitoringData},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, SetVariableMonitoringRequest& k) {
    // the required parts of the message
    for (const auto& val : j.at("setMonitoringData")) {
        k.setMonitoringData.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given SetVariableMonitoringRequest \p k to the given output stream \p
/// os
/// \returns an output stream with the SetVariableMonitoringRequest written to
std::ostream& operator<<(std::ostream& os, const SetVariableMonitoringRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string SetVariableMonitoringResponse::get_type() const {
    return "SetVariableMonitoringResponse";
}

void to_json(json& j, const SetVariableMonitoringResponse& k) {
    // the required parts of the message
    j = json{
        {"setMonitoringResult", k.setMonitoringResult},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, SetVariableMonitoringResponse& k) {
    // the required parts of the message
    for (const auto& val : j.at("setMonitoringResult")) {
        k.setMonitoringResult.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given SetVariableMonitoringResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the SetVariableMonitoringResponse written to
std::ostream& operator<<(std::ostream& os, const SetVariableMonitoringResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
