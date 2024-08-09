// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V16_DIAGNOSTICSSTATUSNOTIFICATION_HPP
#define OCPP_V16_DIAGNOSTICSSTATUSNOTIFICATION_HPP

#include <nlohmann/json_fwd.hpp>

#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP DiagnosticsStatusNotification message
struct DiagnosticsStatusNotificationRequest : public ocpp::Message {
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

/// \brief Contains a OCPP DiagnosticsStatusNotificationResponse message
struct DiagnosticsStatusNotificationResponse : public ocpp::Message {

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

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_DIAGNOSTICSSTATUSNOTIFICATION_HPP
