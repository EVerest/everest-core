// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <boost/optional/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp1_6/messages/SecurityEventNotification.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

using json = nlohmann::json;

namespace ocpp1_6 {

std::string SecurityEventNotificationRequest::get_type() const {
    return "SecurityEventNotification";
}

void to_json(json& j, const SecurityEventNotificationRequest& k) {
    // the required parts of the message
    j = json{
        {"type", k.type},
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
    k.timestamp = DateTime(std::string(j.at("timestamp")));
    ;

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
    j = json({});
    // the optional parts of the message
}

void from_json(const json& j, SecurityEventNotificationResponse& k) {
    // the required parts of the message

    // the optional parts of the message
}

/// \brief Writes the string representation of the given SecurityEventNotificationResponse \p k to the given output
/// stream \p os \returns an output stream with the SecurityEventNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const SecurityEventNotificationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace ocpp1_6
