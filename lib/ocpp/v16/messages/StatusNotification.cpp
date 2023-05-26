// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>
#include <optional>

#include <ocpp/v16/messages/StatusNotification.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string StatusNotificationRequest::get_type() const {
    return "StatusNotification";
}

void to_json(json& j, const StatusNotificationRequest& k) {
    // the required parts of the message
    j = json{
        {"connectorId", k.connectorId},
        {"errorCode", conversions::charge_point_error_code_to_string(k.errorCode)},
        {"status", conversions::charge_point_status_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.info) {
        j["info"] = k.info.value();
    }
    if (k.timestamp) {
        j["timestamp"] = k.timestamp.value().to_rfc3339();
    }
    if (k.vendorId) {
        j["vendorId"] = k.vendorId.value();
    }
    if (k.vendorErrorCode) {
        j["vendorErrorCode"] = k.vendorErrorCode.value();
    }
}

void from_json(const json& j, StatusNotificationRequest& k) {
    // the required parts of the message
    k.connectorId = j.at("connectorId");
    k.errorCode = conversions::string_to_charge_point_error_code(j.at("errorCode"));
    k.status = conversions::string_to_charge_point_status(j.at("status"));

    // the optional parts of the message
    if (j.contains("info")) {
        k.info.emplace(j.at("info"));
    }
    if (j.contains("timestamp")) {
        k.timestamp.emplace(ocpp::DateTime(std::string(j.at("timestamp"))));
    }
    if (j.contains("vendorId")) {
        k.vendorId.emplace(j.at("vendorId"));
    }
    if (j.contains("vendorErrorCode")) {
        k.vendorErrorCode.emplace(j.at("vendorErrorCode"));
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
    j = json({});
    // the optional parts of the message
}

void from_json(const json& j, StatusNotificationResponse& k) {
    // the required parts of the message

    // the optional parts of the message
}

/// \brief Writes the string representation of the given StatusNotificationResponse \p k to the given output stream \p
/// os \returns an output stream with the StatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const StatusNotificationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
