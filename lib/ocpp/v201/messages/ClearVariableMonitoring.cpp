// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>
#include <optional>

#include <ocpp/v201/messages/ClearVariableMonitoring.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string ClearVariableMonitoringRequest::get_type() const {
    return "ClearVariableMonitoring";
}

void to_json(json& j, const ClearVariableMonitoringRequest& k) {
    // the required parts of the message
    j = json{
        {"id", k.id},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ClearVariableMonitoringRequest& k) {
    // the required parts of the message
    for (auto val : j.at("id")) {
        k.id.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ClearVariableMonitoringRequest \p k to the given output stream
/// \p os \returns an output stream with the ClearVariableMonitoringRequest written to
std::ostream& operator<<(std::ostream& os, const ClearVariableMonitoringRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ClearVariableMonitoringResponse::get_type() const {
    return "ClearVariableMonitoringResponse";
}

void to_json(json& j, const ClearVariableMonitoringResponse& k) {
    // the required parts of the message
    j = json{
        {"clearMonitoringResult", k.clearMonitoringResult},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ClearVariableMonitoringResponse& k) {
    // the required parts of the message
    for (auto val : j.at("clearMonitoringResult")) {
        k.clearMonitoringResult.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ClearVariableMonitoringResponse \p k to the given output stream
/// \p os \returns an output stream with the ClearVariableMonitoringResponse written to
std::ostream& operator<<(std::ostream& os, const ClearVariableMonitoringResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
