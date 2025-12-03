// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_PUBLISHFIRMWARESTATUSNOTIFICATION_HPP
#define OCPP_V2_PUBLISHFIRMWARESTATUSNOTIFICATION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP PublishFirmwareStatusNotification message
struct PublishFirmwareStatusNotificationRequest : public ocpp::Message {
    PublishFirmwareStatusEnum status;
    std::optional<std::vector<CiString<2000>>> location;
    std::optional<int32_t> requestId;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this PublishFirmwareStatusNotification message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given PublishFirmwareStatusNotificationRequest \p k to a given json object \p j
void to_json(json& j, const PublishFirmwareStatusNotificationRequest& k);

/// \brief Conversion from a given json object \p j to a given PublishFirmwareStatusNotificationRequest \p k
void from_json(const json& j, PublishFirmwareStatusNotificationRequest& k);

/// \brief Writes the string representation of the given PublishFirmwareStatusNotificationRequest \p k to the given
/// output stream \p os \returns an output stream with the PublishFirmwareStatusNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const PublishFirmwareStatusNotificationRequest& k);

/// \brief Contains a OCPP PublishFirmwareStatusNotificationResponse message
struct PublishFirmwareStatusNotificationResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this PublishFirmwareStatusNotificationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given PublishFirmwareStatusNotificationResponse \p k to a given json object \p j
void to_json(json& j, const PublishFirmwareStatusNotificationResponse& k);

/// \brief Conversion from a given json object \p j to a given PublishFirmwareStatusNotificationResponse \p k
void from_json(const json& j, PublishFirmwareStatusNotificationResponse& k);

/// \brief Writes the string representation of the given PublishFirmwareStatusNotificationResponse \p k to the given
/// output stream \p os \returns an output stream with the PublishFirmwareStatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const PublishFirmwareStatusNotificationResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_PUBLISHFIRMWARESTATUSNOTIFICATION_HPP
