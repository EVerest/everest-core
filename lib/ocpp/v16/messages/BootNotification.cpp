// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>
#include <optional>

#include <ocpp/v16/messages/BootNotification.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string BootNotificationRequest::get_type() const {
    return "BootNotification";
}

void to_json(json& j, const BootNotificationRequest& k) {
    // the required parts of the message
    j = json{
        {"chargePointVendor", k.chargePointVendor},
        {"chargePointModel", k.chargePointModel},
    };
    // the optional parts of the message
    if (k.chargePointSerialNumber) {
        j["chargePointSerialNumber"] = k.chargePointSerialNumber.value();
    }
    if (k.chargeBoxSerialNumber) {
        j["chargeBoxSerialNumber"] = k.chargeBoxSerialNumber.value();
    }
    if (k.firmwareVersion) {
        j["firmwareVersion"] = k.firmwareVersion.value();
    }
    if (k.iccid) {
        j["iccid"] = k.iccid.value();
    }
    if (k.imsi) {
        j["imsi"] = k.imsi.value();
    }
    if (k.meterType) {
        j["meterType"] = k.meterType.value();
    }
    if (k.meterSerialNumber) {
        j["meterSerialNumber"] = k.meterSerialNumber.value();
    }
}

void from_json(const json& j, BootNotificationRequest& k) {
    // the required parts of the message
    k.chargePointVendor = j.at("chargePointVendor");
    k.chargePointModel = j.at("chargePointModel");

    // the optional parts of the message
    if (j.contains("chargePointSerialNumber")) {
        k.chargePointSerialNumber.emplace(j.at("chargePointSerialNumber"));
    }
    if (j.contains("chargeBoxSerialNumber")) {
        k.chargeBoxSerialNumber.emplace(j.at("chargeBoxSerialNumber"));
    }
    if (j.contains("firmwareVersion")) {
        k.firmwareVersion.emplace(j.at("firmwareVersion"));
    }
    if (j.contains("iccid")) {
        k.iccid.emplace(j.at("iccid"));
    }
    if (j.contains("imsi")) {
        k.imsi.emplace(j.at("imsi"));
    }
    if (j.contains("meterType")) {
        k.meterType.emplace(j.at("meterType"));
    }
    if (j.contains("meterSerialNumber")) {
        k.meterSerialNumber.emplace(j.at("meterSerialNumber"));
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
        {"status", conversions::registration_status_to_string(k.status)},
        {"currentTime", k.currentTime.to_rfc3339()},
        {"interval", k.interval},
    };
    // the optional parts of the message
}

void from_json(const json& j, BootNotificationResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_registration_status(j.at("status"));
    k.currentTime = ocpp::DateTime(std::string(j.at("currentTime")));
    k.interval = j.at("interval");

    // the optional parts of the message
}

/// \brief Writes the string representation of the given BootNotificationResponse \p k to the given output stream \p os
/// \returns an output stream with the BootNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const BootNotificationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
