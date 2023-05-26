// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>
#include <optional>

#include <ocpp/v16/messages/SignedFirmwareStatusNotification.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string SignedFirmwareStatusNotificationRequest::get_type() const {
    return "SignedFirmwareStatusNotification";
}

void to_json(json& j, const SignedFirmwareStatusNotificationRequest& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::firmware_status_enum_type_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.requestId) {
        j["requestId"] = k.requestId.value();
    }
}

void from_json(const json& j, SignedFirmwareStatusNotificationRequest& k) {
    // the required parts of the message
    k.status = conversions::string_to_firmware_status_enum_type(j.at("status"));

    // the optional parts of the message
    if (j.contains("requestId")) {
        k.requestId.emplace(j.at("requestId"));
    }
}

/// \brief Writes the string representation of the given SignedFirmwareStatusNotificationRequest \p k to the given
/// output stream \p os \returns an output stream with the SignedFirmwareStatusNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const SignedFirmwareStatusNotificationRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string SignedFirmwareStatusNotificationResponse::get_type() const {
    return "SignedFirmwareStatusNotificationResponse";
}

void to_json(json& j, const SignedFirmwareStatusNotificationResponse& k) {
    // the required parts of the message
    j = json({});
    // the optional parts of the message
}

void from_json(const json& j, SignedFirmwareStatusNotificationResponse& k) {
    // the required parts of the message

    // the optional parts of the message
}

/// \brief Writes the string representation of the given SignedFirmwareStatusNotificationResponse \p k to the given
/// output stream \p os \returns an output stream with the SignedFirmwareStatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const SignedFirmwareStatusNotificationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
