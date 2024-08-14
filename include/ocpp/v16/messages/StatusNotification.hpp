// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V16_STATUSNOTIFICATION_HPP
#define OCPP_V16_STATUSNOTIFICATION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP StatusNotification message
struct StatusNotificationRequest : public ocpp::Message {
    int32_t connectorId;
    ChargePointErrorCode errorCode;
    ChargePointStatus status;
    std::optional<CiString<50>> info;
    std::optional<ocpp::DateTime> timestamp;
    std::optional<CiString<255>> vendorId;
    std::optional<CiString<50>> vendorErrorCode;

    /// \brief Provides the type of this StatusNotification message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given StatusNotificationRequest \p k to a given json object \p j
void to_json(json& j, const StatusNotificationRequest& k);

/// \brief Conversion from a given json object \p j to a given StatusNotificationRequest \p k
void from_json(const json& j, StatusNotificationRequest& k);

/// \brief Writes the string representation of the given StatusNotificationRequest \p k to the given output stream \p os
/// \returns an output stream with the StatusNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const StatusNotificationRequest& k);

/// \brief Contains a OCPP StatusNotificationResponse message
struct StatusNotificationResponse : public ocpp::Message {

    /// \brief Provides the type of this StatusNotificationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given StatusNotificationResponse \p k to a given json object \p j
void to_json(json& j, const StatusNotificationResponse& k);

/// \brief Conversion from a given json object \p j to a given StatusNotificationResponse \p k
void from_json(const json& j, StatusNotificationResponse& k);

/// \brief Writes the string representation of the given StatusNotificationResponse \p k to the given output stream \p
/// os \returns an output stream with the StatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const StatusNotificationResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_STATUSNOTIFICATION_HPP
