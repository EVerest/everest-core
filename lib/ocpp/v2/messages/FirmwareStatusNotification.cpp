// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/FirmwareStatusNotification.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string FirmwareStatusNotificationRequest::get_type() const {
    return "FirmwareStatusNotification";
}

void to_json(json& j, const FirmwareStatusNotificationRequest& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::firmware_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.requestId) {
        j["requestId"] = k.requestId.value();
    }
}

void from_json(const json& j, FirmwareStatusNotificationRequest& k) {
    // the required parts of the message
    k.status = conversions::string_to_firmware_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("requestId")) {
        k.requestId.emplace(j.at("requestId"));
    }
}

/// \brief Writes the string representation of the given FirmwareStatusNotificationRequest \p k to the given output
/// stream \p os \returns an output stream with the FirmwareStatusNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const FirmwareStatusNotificationRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string FirmwareStatusNotificationResponse::get_type() const {
    return "FirmwareStatusNotificationResponse";
}

void to_json(json& j, const FirmwareStatusNotificationResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, FirmwareStatusNotificationResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given FirmwareStatusNotificationResponse \p k to the given output
/// stream \p os \returns an output stream with the FirmwareStatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const FirmwareStatusNotificationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
