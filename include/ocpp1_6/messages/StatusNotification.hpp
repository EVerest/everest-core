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
    std::string get_type() const;
};

/// \brief Conversion from a given StatusNotificationRequest \p k to a given json object \p j
void to_json(json& j, const StatusNotificationRequest& k);

/// \brief Conversion from a given json object \p j to a given StatusNotificationRequest \p k
void from_json(const json& j, StatusNotificationRequest& k);

/// \brief Writes the string representation of the given StatusNotificationRequest \p k to the given output stream \p os
/// \returns an output stream with the StatusNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const StatusNotificationRequest& k);

/// \brief Contains a OCPP 1.6 StatusNotificationResponse message
struct StatusNotificationResponse : public Message {

    /// \brief Provides the type of this StatusNotificationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given StatusNotificationResponse \p k to a given json object \p j
void to_json(json& j, const StatusNotificationResponse& k);

/// \brief Conversion from a given json object \p j to a given StatusNotificationResponse \p k
void from_json(const json& j, StatusNotificationResponse& k);

/// \brief Writes the string representation of the given StatusNotificationResponse \p k to the given output stream \p
/// os \returns an output stream with the StatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const StatusNotificationResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_STATUSNOTIFICATION_HPP
