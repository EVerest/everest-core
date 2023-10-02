// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v16/messages/SecurityEventNotification.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string SecurityEventNotificationRequest::get_type() const {
    return "SecurityEventNotification";
}

void to_json(json& j, const SecurityEventNotificationRequest& k) {
    // the required parts of the message
    j = json{
        {"type", conversions::security_event_to_string(k.type)},
        {"timestamp", k.timestamp.to_rfc3339()},
    };
    // the optional parts of the message
    if (k.techInfo) {
        j["techInfo"] = k.techInfo.value();
    }
}

void from_json(const json& j, SecurityEventNotificationRequest& k) {
    // the required parts of the message
    k.type = conversions::string_to_security_event(j.at("type"));
    k.timestamp = ocpp::DateTime(std::string(j.at("timestamp")));

    // the optional parts of the message
    if (j.contains("techInfo")) {
        k.techInfo.emplace(j.at("techInfo"));
    }
}

/// \brief Writes the string representation of the given SecurityEventNotificationRequest \p k to the given output
/// stream \p os \returns an output stream with the SecurityEventNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const SecurityEventNotificationRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string SecurityEventNotificationResponse::get_type() const {
    return "SecurityEventNotificationResponse";
}

void to_json(json& j, const SecurityEventNotificationResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    (void)k; // no elements to unpack, silence unused parameter warning
}

void from_json(const json& j, SecurityEventNotificationResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    // no elements to unpack, silence unused parameter warning
    (void)j;
    (void)k;
}

/// \brief Writes the string representation of the given SecurityEventNotificationResponse \p k to the given output
/// stream \p os \returns an output stream with the SecurityEventNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const SecurityEventNotificationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
