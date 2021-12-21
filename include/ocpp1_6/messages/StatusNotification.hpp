// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_STATUSNOTIFICATION_HPP
#define OCPP1_6_STATUSNOTIFICATION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct StatusNotificationRequest : public Message {
    int32_t connectorId;
    ChargePointErrorCode errorCode;
    ChargePointStatus status;
    boost::optional<CiString50Type> info;
    boost::optional<DateTime> timestamp;
    boost::optional<CiString255Type> vendorId;
    boost::optional<CiString50Type> vendorErrorCode;

    std::string get_type() const {
        return "StatusNotification";
    }

    friend void to_json(json& j, const StatusNotificationRequest& k) {
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

    friend void from_json(const json& j, StatusNotificationRequest& k) {
        // the required parts of the message
        k.connectorId = j.at("connectorId");
        k.errorCode = conversions::string_to_charge_point_error_code(j.at("errorCode"));
        k.status = conversions::string_to_charge_point_status(j.at("status"));

        // the optional parts of the message
        if (j.contains("info")) {
            k.info.emplace(j.at("info"));
        }
        if (j.contains("timestamp")) {
            k.timestamp.emplace(DateTime(std::string(j.at("timestamp"))));
        }
        if (j.contains("vendorId")) {
            k.vendorId.emplace(j.at("vendorId"));
        }
        if (j.contains("vendorErrorCode")) {
            k.vendorErrorCode.emplace(j.at("vendorErrorCode"));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const StatusNotificationRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct StatusNotificationResponse : public Message {

    std::string get_type() const {
        return "StatusNotificationResponse";
    }

    friend void to_json(json& j, const StatusNotificationResponse& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
    }

    friend void from_json(const json& j, StatusNotificationResponse& k) {
        // the required parts of the message

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const StatusNotificationResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_STATUSNOTIFICATION_HPP
