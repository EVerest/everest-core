// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v201/messages/PublishFirmwareStatusNotification.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string PublishFirmwareStatusNotificationRequest::get_type() const {
    return "PublishFirmwareStatusNotification";
}

void to_json(json& j, const PublishFirmwareStatusNotificationRequest& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::publish_firmware_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.location) {
        j["location"] = json::array();
        for (auto val : k.location.value()) {
            j["location"].push_back(val);
        }
    }
    if (k.requestId) {
        j["requestId"] = k.requestId.value();
    }
}

void from_json(const json& j, PublishFirmwareStatusNotificationRequest& k) {
    // the required parts of the message
    k.status = conversions::string_to_publish_firmware_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("location")) {
        json arr = j.at("location");
        std::vector<CiString<512>> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.location.emplace(vec);
    }
    if (j.contains("requestId")) {
        k.requestId.emplace(j.at("requestId"));
    }
}

/// \brief Writes the string representation of the given PublishFirmwareStatusNotificationRequest \p k to the given
/// output stream \p os \returns an output stream with the PublishFirmwareStatusNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const PublishFirmwareStatusNotificationRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string PublishFirmwareStatusNotificationResponse::get_type() const {
    return "PublishFirmwareStatusNotificationResponse";
}

void to_json(json& j, const PublishFirmwareStatusNotificationResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, PublishFirmwareStatusNotificationResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given PublishFirmwareStatusNotificationResponse \p k to the given
/// output stream \p os \returns an output stream with the PublishFirmwareStatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const PublishFirmwareStatusNotificationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
