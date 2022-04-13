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
    std::string get_type() const;
};

/// \brief Conversion from a given DiagnosticsStatusNotificationRequest \p k to a given json object \p j
void to_json(json& j, const DiagnosticsStatusNotificationRequest& k);

/// \brief Conversion from a given json object \p j to a given DiagnosticsStatusNotificationRequest \p k
void from_json(const json& j, DiagnosticsStatusNotificationRequest& k);

/// \brief Writes the string representation of the given DiagnosticsStatusNotificationRequest \p k to the given output
/// stream \p os \returns an output stream with the DiagnosticsStatusNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const DiagnosticsStatusNotificationRequest& k);

/// \brief Contains a OCPP 1.6 DiagnosticsStatusNotificationResponse message
struct DiagnosticsStatusNotificationResponse : public Message {

    /// \brief Provides the type of this DiagnosticsStatusNotificationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given DiagnosticsStatusNotificationResponse \p k to a given json object \p j
void to_json(json& j, const DiagnosticsStatusNotificationResponse& k);

/// \brief Conversion from a given json object \p j to a given DiagnosticsStatusNotificationResponse \p k
void from_json(const json& j, DiagnosticsStatusNotificationResponse& k);

/// \brief Writes the string representation of the given DiagnosticsStatusNotificationResponse \p k to the given output
/// stream \p os \returns an output stream with the DiagnosticsStatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const DiagnosticsStatusNotificationResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_DIAGNOSTICSSTATUSNOTIFICATION_HPP
