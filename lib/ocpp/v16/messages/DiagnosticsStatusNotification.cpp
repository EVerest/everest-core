// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <ocpp/v16/messages/DiagnosticsStatusNotification.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string DiagnosticsStatusNotificationRequest::get_type() const {
    return "DiagnosticsStatusNotification";
}

void to_json(json& j, const DiagnosticsStatusNotificationRequest& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::diagnostics_status_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, DiagnosticsStatusNotificationRequest& k) {
    // the required parts of the message
    k.status = conversions::string_to_diagnostics_status(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given DiagnosticsStatusNotificationRequest \p k to the given output
/// stream \p os \returns an output stream with the DiagnosticsStatusNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const DiagnosticsStatusNotificationRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string DiagnosticsStatusNotificationResponse::get_type() const {
    return "DiagnosticsStatusNotificationResponse";
}

void to_json(json& j, const DiagnosticsStatusNotificationResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    (void)k; // no elements to unpack, silence unused parameter warning
}

void from_json(const json& j, DiagnosticsStatusNotificationResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    // no elements to unpack, silence unused parameter warning
    (void)j;
    (void)k;
}

/// \brief Writes the string representation of the given DiagnosticsStatusNotificationResponse \p k to the given output
/// stream \p os \returns an output stream with the DiagnosticsStatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const DiagnosticsStatusNotificationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
