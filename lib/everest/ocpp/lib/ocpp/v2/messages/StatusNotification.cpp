// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/StatusNotification.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string StatusNotificationRequest::get_type() const {
    return "StatusNotification";
}

void to_json(json& j, const StatusNotificationRequest& k) {
    // the required parts of the message
    j = json{
        {"timestamp", k.timestamp.to_rfc3339()},
        {"connectorStatus", conversions::connector_status_enum_to_string(k.connectorStatus)},
        {"evseId", k.evseId},
        {"connectorId", k.connectorId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, StatusNotificationRequest& k) {
    // the required parts of the message
    k.timestamp = ocpp::DateTime(std::string(j.at("timestamp")));
    k.connectorStatus = conversions::string_to_connector_status_enum(j.at("connectorStatus"));
    k.evseId = j.at("evseId");
    k.connectorId = j.at("connectorId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given StatusNotificationRequest \p k to the given output stream \p os
/// \returns an output stream with the StatusNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const StatusNotificationRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string StatusNotificationResponse::get_type() const {
    return "StatusNotificationResponse";
}

void to_json(json& j, const StatusNotificationResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, StatusNotificationResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given StatusNotificationResponse \p k to the given output stream \p
/// os
/// \returns an output stream with the StatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const StatusNotificationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
