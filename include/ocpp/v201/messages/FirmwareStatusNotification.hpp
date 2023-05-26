// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_FIRMWARESTATUSNOTIFICATION_HPP
#define OCPP_V201_FIRMWARESTATUSNOTIFICATION_HPP

#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP FirmwareStatusNotification message
struct FirmwareStatusNotificationRequest : public ocpp::Message {
    FirmwareStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<int32_t> requestId;

    /// \brief Provides the type of this FirmwareStatusNotification message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given FirmwareStatusNotificationRequest \p k to a given json object \p j
void to_json(json& j, const FirmwareStatusNotificationRequest& k);

/// \brief Conversion from a given json object \p j to a given FirmwareStatusNotificationRequest \p k
void from_json(const json& j, FirmwareStatusNotificationRequest& k);

/// \brief Writes the string representation of the given FirmwareStatusNotificationRequest \p k to the given output
/// stream \p os \returns an output stream with the FirmwareStatusNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const FirmwareStatusNotificationRequest& k);

/// \brief Contains a OCPP FirmwareStatusNotificationResponse message
struct FirmwareStatusNotificationResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this FirmwareStatusNotificationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given FirmwareStatusNotificationResponse \p k to a given json object \p j
void to_json(json& j, const FirmwareStatusNotificationResponse& k);

/// \brief Conversion from a given json object \p j to a given FirmwareStatusNotificationResponse \p k
void from_json(const json& j, FirmwareStatusNotificationResponse& k);

/// \brief Writes the string representation of the given FirmwareStatusNotificationResponse \p k to the given output
/// stream \p os \returns an output stream with the FirmwareStatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const FirmwareStatusNotificationResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_FIRMWARESTATUSNOTIFICATION_HPP
