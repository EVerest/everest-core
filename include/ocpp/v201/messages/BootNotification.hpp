// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V201_BOOTNOTIFICATION_HPP
#define OCPP_V201_BOOTNOTIFICATION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/ocpp_enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP BootNotification message
struct BootNotificationRequest : public ocpp::Message {
    ChargingStation chargingStation;
    BootReasonEnum reason;
    std::optional<CustomData> customData;

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

/// \brief Contains a OCPP BootNotificationResponse message
struct BootNotificationResponse : public ocpp::Message {
    ocpp::DateTime currentTime;
    int32_t interval;
    RegistrationStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

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

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_BOOTNOTIFICATION_HPP
