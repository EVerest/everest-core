// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_STATUSNOTIFICATION_HPP
#define OCPP1_6_STATUSNOTIFICATION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 StatusNotification message
struct StatusNotificationRequest : public Message {
    int32_t connectorId;
    ChargePointErrorCode errorCode;
    ChargePointStatus status;
    boost::optional<CiString50Type> info;
    boost::optional<DateTime> timestamp;
    boost::optional<CiString255Type> vendorId;
    boost::optional<CiString50Type> vendorErrorCode;

    /// \brief Provides the type of this StatusNotification message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "StatusNotification";
    }

    /// \brief Conversion from a given StatusNotificationRequest \p k to a given json object \p j
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

    /// \brief Conversion from a given json object \p j to a given StatusNotificationRequest \p k
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

    /// \brief Writes the string representation of the given StatusNotificationRequest \p k to the given output stream
    /// \p os \returns an output stream with the StatusNotificationRequest written to
    friend std::ostream& operator<<(std::ostream& os, const StatusNotificationRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 StatusNotificationResponse message
struct StatusNotificationResponse : public Message {

    /// \brief Provides the type of this StatusNotificationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "StatusNotificationResponse";
    }

    /// \brief Conversion from a given StatusNotificationResponse \p k to a given json object \p j
    friend void to_json(json& j, const StatusNotificationResponse& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given StatusNotificationResponse \p k
    friend void from_json(const json& j, StatusNotificationResponse& k) {
        // the required parts of the message

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given StatusNotificationResponse \p k to the given output stream
    /// \p os \returns an output stream with the StatusNotificationResponse written to
    friend std::ostream& operator<<(std::ostream& os, const StatusNotificationResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_STATUSNOTIFICATION_HPP
