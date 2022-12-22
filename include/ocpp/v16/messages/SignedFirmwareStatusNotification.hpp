// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_SIGNEDFIRMWARESTATUSNOTIFICATION_HPP
#define OCPP_V16_SIGNEDFIRMWARESTATUSNOTIFICATION_HPP

#include <boost/optional.hpp>

#include <ocpp/v16/enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP SignedFirmwareStatusNotification message
struct SignedFirmwareStatusNotificationRequest : public ocpp::Message {
    FirmwareStatusEnumType status;
    boost::optional<int32_t> requestId;

    /// \brief Provides the type of this SignedFirmwareStatusNotification message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SignedFirmwareStatusNotificationRequest \p k to a given json object \p j
void to_json(json& j, const SignedFirmwareStatusNotificationRequest& k);

/// \brief Conversion from a given json object \p j to a given SignedFirmwareStatusNotificationRequest \p k
void from_json(const json& j, SignedFirmwareStatusNotificationRequest& k);

/// \brief Writes the string representation of the given SignedFirmwareStatusNotificationRequest \p k to the given
/// output stream \p os \returns an output stream with the SignedFirmwareStatusNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const SignedFirmwareStatusNotificationRequest& k);

/// \brief Contains a OCPP SignedFirmwareStatusNotificationResponse message
struct SignedFirmwareStatusNotificationResponse : public ocpp::Message {

    /// \brief Provides the type of this SignedFirmwareStatusNotificationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SignedFirmwareStatusNotificationResponse \p k to a given json object \p j
void to_json(json& j, const SignedFirmwareStatusNotificationResponse& k);

/// \brief Conversion from a given json object \p j to a given SignedFirmwareStatusNotificationResponse \p k
void from_json(const json& j, SignedFirmwareStatusNotificationResponse& k);

/// \brief Writes the string representation of the given SignedFirmwareStatusNotificationResponse \p k to the given
/// output stream \p os \returns an output stream with the SignedFirmwareStatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const SignedFirmwareStatusNotificationResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_SIGNEDFIRMWARESTATUSNOTIFICATION_HPP
