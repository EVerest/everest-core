// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_DIAGNOSTICSSTATUSNOTIFICATION_HPP
#define OCPP1_6_DIAGNOSTICSSTATUSNOTIFICATION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 DiagnosticsStatusNotification message
struct DiagnosticsStatusNotificationRequest : public Message {
    DiagnosticsStatus status;

    /// \brief Provides the type of this DiagnosticsStatusNotification message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "DiagnosticsStatusNotification";
    }

    /// \brief Conversion from a given DiagnosticsStatusNotificationRequest \p k to a given json object \p j
    friend void to_json(json& j, const DiagnosticsStatusNotificationRequest& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::diagnostics_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given DiagnosticsStatusNotificationRequest \p k
    friend void from_json(const json& j, DiagnosticsStatusNotificationRequest& k) {
        // the required parts of the message
        k.status = conversions::string_to_diagnostics_status(j.at("status"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given DiagnosticsStatusNotificationRequest \p k to the given
    /// output stream \p os \returns an output stream with the DiagnosticsStatusNotificationRequest written to
    friend std::ostream& operator<<(std::ostream& os, const DiagnosticsStatusNotificationRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 DiagnosticsStatusNotificationResponse message
struct DiagnosticsStatusNotificationResponse : public Message {

    /// \brief Provides the type of this DiagnosticsStatusNotificationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "DiagnosticsStatusNotificationResponse";
    }

    /// \brief Conversion from a given DiagnosticsStatusNotificationResponse \p k to a given json object \p j
    friend void to_json(json& j, const DiagnosticsStatusNotificationResponse& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given DiagnosticsStatusNotificationResponse \p k
    friend void from_json(const json& j, DiagnosticsStatusNotificationResponse& k) {
        // the required parts of the message

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given DiagnosticsStatusNotificationResponse \p k to the given
    /// output stream \p os \returns an output stream with the DiagnosticsStatusNotificationResponse written to
    friend std::ostream& operator<<(std::ostream& os, const DiagnosticsStatusNotificationResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_DIAGNOSTICSSTATUSNOTIFICATION_HPP
