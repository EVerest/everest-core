// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_FIRMWARESTATUSNOTIFICATION_HPP
#define OCPP1_6_FIRMWARESTATUSNOTIFICATION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 FirmwareStatusNotification message
struct FirmwareStatusNotificationRequest : public Message {
    FirmwareStatus status;

    /// \brief Provides the type of this FirmwareStatusNotification message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "FirmwareStatusNotification";
    }

    /// \brief Conversion from a given FirmwareStatusNotificationRequest \p k to a given json object \p j
    friend void to_json(json& j, const FirmwareStatusNotificationRequest& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::firmware_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given FirmwareStatusNotificationRequest \p k
    friend void from_json(const json& j, FirmwareStatusNotificationRequest& k) {
        // the required parts of the message
        k.status = conversions::string_to_firmware_status(j.at("status"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given FirmwareStatusNotificationRequest \p k to the given output
    /// stream \p os \returns an output stream with the FirmwareStatusNotificationRequest written to
    friend std::ostream& operator<<(std::ostream& os, const FirmwareStatusNotificationRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 FirmwareStatusNotificationResponse message
struct FirmwareStatusNotificationResponse : public Message {

    /// \brief Provides the type of this FirmwareStatusNotificationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "FirmwareStatusNotificationResponse";
    }

    /// \brief Conversion from a given FirmwareStatusNotificationResponse \p k to a given json object \p j
    friend void to_json(json& j, const FirmwareStatusNotificationResponse& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given FirmwareStatusNotificationResponse \p k
    friend void from_json(const json& j, FirmwareStatusNotificationResponse& k) {
        // the required parts of the message

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given FirmwareStatusNotificationResponse \p k to the given output
    /// stream \p os \returns an output stream with the FirmwareStatusNotificationResponse written to
    friend std::ostream& operator<<(std::ostream& os, const FirmwareStatusNotificationResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_FIRMWARESTATUSNOTIFICATION_HPP
