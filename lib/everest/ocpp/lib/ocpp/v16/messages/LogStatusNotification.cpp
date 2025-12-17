// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v16/messages/LogStatusNotification.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string LogStatusNotificationRequest::get_type() const {
    return "LogStatusNotification";
}

void to_json(json& j, const LogStatusNotificationRequest& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::upload_log_status_enum_type_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.requestId) {
        j["requestId"] = k.requestId.value();
    }
}

void from_json(const json& j, LogStatusNotificationRequest& k) {
    // the required parts of the message
    k.status = conversions::string_to_upload_log_status_enum_type(j.at("status"));

    // the optional parts of the message
    if (j.contains("requestId")) {
        k.requestId.emplace(j.at("requestId"));
    }
}

/// \brief Writes the string representation of the given LogStatusNotificationRequest \p k to the given output stream \p
/// os
/// \returns an output stream with the LogStatusNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const LogStatusNotificationRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string LogStatusNotificationResponse::get_type() const {
    return "LogStatusNotificationResponse";
}

void to_json(json& j, const LogStatusNotificationResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    (void)k; // no elements to unpack, silence unused parameter warning
}

void from_json(const json& j, LogStatusNotificationResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    // no elements to unpack, silence unused parameter warning
    (void)j;
    (void)k;
}

/// \brief Writes the string representation of the given LogStatusNotificationResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the LogStatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const LogStatusNotificationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
