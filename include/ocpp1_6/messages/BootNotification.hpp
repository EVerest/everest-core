// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_BOOTNOTIFICATION_HPP
#define OCPP1_6_BOOTNOTIFICATION_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 BootNotification message
struct BootNotificationRequest : public Message {
    CiString20Type chargePointVendor;
    CiString20Type chargePointModel;
    boost::optional<CiString25Type> chargePointSerialNumber;
    boost::optional<CiString25Type> chargeBoxSerialNumber;
    boost::optional<CiString50Type> firmwareVersion;
    boost::optional<CiString20Type> iccid;
    boost::optional<CiString20Type> imsi;
    boost::optional<CiString25Type> meterType;
    boost::optional<CiString25Type> meterSerialNumber;

    /// \brief Provides the type of this BootNotification message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given BootNotificationRequest \p k to a given json object \p j
void to_json(json& j, const BootNotificationRequest& k);

/// \brief Conversion from a given json object \p j to a given BootNotificationRequest \p k
void from_json(const json& j, BootNotificationRequest& k);

/// \brief Writes the string representation of the given BootNotificationRequest \p k to the given output stream \p os
/// \returns an output stream with the BootNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const BootNotificationRequest& k);

/// \brief Contains a OCPP 1.6 BootNotificationResponse message
struct BootNotificationResponse : public Message {
    RegistrationStatus status;
    DateTime currentTime;
    int32_t interval;

    /// \brief Provides the type of this BootNotificationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given BootNotificationResponse \p k to a given json object \p j
void to_json(json& j, const BootNotificationResponse& k);

/// \brief Conversion from a given json object \p j to a given BootNotificationResponse \p k
void from_json(const json& j, BootNotificationResponse& k);

/// \brief Writes the string representation of the given BootNotificationResponse \p k to the given output stream \p os
/// \returns an output stream with the BootNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const BootNotificationResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_BOOTNOTIFICATION_HPP
