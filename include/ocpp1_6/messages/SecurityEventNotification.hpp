// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_SECURITYEVENTNOTIFICATION_HPP
#define OCPP1_6_SECURITYEVENTNOTIFICATION_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 SecurityEventNotification message
struct SecurityEventNotificationRequest : public Message {
    CiString50Type type;
    DateTime timestamp;
    boost::optional<CiString255Type> techInfo;

    /// \brief Provides the type of this SecurityEventNotification message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SecurityEventNotificationRequest \p k to a given json object \p j
void to_json(json& j, const SecurityEventNotificationRequest& k);

/// \brief Conversion from a given json object \p j to a given SecurityEventNotificationRequest \p k
void from_json(const json& j, SecurityEventNotificationRequest& k);

/// \brief Writes the string representation of the given SecurityEventNotificationRequest \p k to the given output
/// stream \p os \returns an output stream with the SecurityEventNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const SecurityEventNotificationRequest& k);

/// \brief Contains a OCPP 1.6 SecurityEventNotificationResponse message
struct SecurityEventNotificationResponse : public Message {

    /// \brief Provides the type of this SecurityEventNotificationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SecurityEventNotificationResponse \p k to a given json object \p j
void to_json(json& j, const SecurityEventNotificationResponse& k);

/// \brief Conversion from a given json object \p j to a given SecurityEventNotificationResponse \p k
void from_json(const json& j, SecurityEventNotificationResponse& k);

/// \brief Writes the string representation of the given SecurityEventNotificationResponse \p k to the given output
/// stream \p os \returns an output stream with the SecurityEventNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const SecurityEventNotificationResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_SECURITYEVENTNOTIFICATION_HPP
