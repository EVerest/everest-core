// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/SetMonitoringLevel.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string SetMonitoringLevelRequest::get_type() const {
    return "SetMonitoringLevel";
}

void to_json(json& j, const SetMonitoringLevelRequest& k) {
    // the required parts of the message
    j = json{
        {"severity", k.severity},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, SetMonitoringLevelRequest& k) {
    // the required parts of the message
    k.severity = j.at("severity");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given SetMonitoringLevelRequest \p k to the given output stream \p os
/// \returns an output stream with the SetMonitoringLevelRequest written to
std::ostream& operator<<(std::ostream& os, const SetMonitoringLevelRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string SetMonitoringLevelResponse::get_type() const {
    return "SetMonitoringLevelResponse";
}

void to_json(json& j, const SetMonitoringLevelResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::generic_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, SetMonitoringLevelResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_generic_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given SetMonitoringLevelResponse \p k to the given output stream \p
/// os \returns an output stream with the SetMonitoringLevelResponse written to
std::ostream& operator<<(std::ostream& os, const SetMonitoringLevelResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
