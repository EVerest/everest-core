// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <boost/optional/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp/v201/messages/BootNotification.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string BootNotificationRequest::get_type() const {
    return "BootNotification";
}

void to_json(json& j, const BootNotificationRequest& k) {
    // the required parts of the message
    j = json{
        {"chargingStation", k.chargingStation},
        {"reason", conversions::boot_reason_enum_to_string(k.reason)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, BootNotificationRequest& k) {
    // the required parts of the message
    k.chargingStation = j.at("chargingStation");
    k.reason = conversions::string_to_boot_reason_enum(j.at("reason"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given BootNotificationRequest \p k to the given output stream \p os
/// \returns an output stream with the BootNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const BootNotificationRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string BootNotificationResponse::get_type() const {
    return "BootNotificationResponse";
}

void to_json(json& j, const BootNotificationResponse& k) {
    // the required parts of the message
    j = json{
        {"currentTime", k.currentTime.to_rfc3339()},
        {"interval", k.interval},
        {"status", conversions::registration_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, BootNotificationResponse& k) {
    // the required parts of the message
    k.currentTime = ocpp::DateTime(std::string(j.at("currentTime")));
    k.interval = j.at("interval");
    k.status = conversions::string_to_registration_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given BootNotificationResponse \p k to the given output stream \p os
/// \returns an output stream with the BootNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const BootNotificationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
