// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/NotifyDERAlarm.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string NotifyDERAlarmRequest::get_type() const {
    return "NotifyDERAlarm";
}

void to_json(json& j, const NotifyDERAlarmRequest& k) {
    // the required parts of the message
    j = json{
        {"controlType", ocpp::v2::conversions::dercontrol_enum_to_string(k.controlType)},
        {"timestamp", k.timestamp.to_rfc3339()},
    };
    // the optional parts of the message
    if (k.gridEventFault) {
        j["gridEventFault"] = ocpp::v2::conversions::grid_event_fault_enum_to_string(k.gridEventFault.value());
    }
    if (k.alarmEnded) {
        j["alarmEnded"] = k.alarmEnded.value();
    }
    if (k.extraInfo) {
        j["extraInfo"] = k.extraInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyDERAlarmRequest& k) {
    // the required parts of the message
    k.controlType = ocpp::v2::conversions::string_to_dercontrol_enum(j.at("controlType"));
    k.timestamp = ocpp::DateTime(std::string(j.at("timestamp")));

    // the optional parts of the message
    if (j.contains("gridEventFault")) {
        k.gridEventFault.emplace(ocpp::v2::conversions::string_to_grid_event_fault_enum(j.at("gridEventFault")));
    }
    if (j.contains("alarmEnded")) {
        k.alarmEnded.emplace(j.at("alarmEnded"));
    }
    if (j.contains("extraInfo")) {
        k.extraInfo.emplace(j.at("extraInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyDERAlarmRequest \p k to the given output stream \p os
/// \returns an output stream with the NotifyDERAlarmRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyDERAlarmRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyDERAlarmResponse::get_type() const {
    return "NotifyDERAlarmResponse";
}

void to_json(json& j, const NotifyDERAlarmResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyDERAlarmResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyDERAlarmResponse \p k to the given output stream \p os
/// \returns an output stream with the NotifyDERAlarmResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyDERAlarmResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
