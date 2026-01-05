// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_SECURITYEVENTNOTIFICATION_HPP
#define OCPP_V2_SECURITYEVENTNOTIFICATION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP SecurityEventNotification message
struct SecurityEventNotificationRequest : public ocpp::Message {
    CiString<50> type;
    ocpp::DateTime timestamp;
    std::optional<CiString<255>> techInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SecurityEventNotification message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SecurityEventNotificationRequest \p k to a given json object \p j
void to_json(json& j, const SecurityEventNotificationRequest& k);

/// \brief Conversion from a given json object \p j to a given SecurityEventNotificationRequest \p k
void from_json(const json& j, SecurityEventNotificationRequest& k);

/// \brief Writes the string representation of the given SecurityEventNotificationRequest \p k to the given output
/// stream \p os
/// \returns an output stream with the SecurityEventNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const SecurityEventNotificationRequest& k);

/// \brief Contains a OCPP SecurityEventNotificationResponse message
struct SecurityEventNotificationResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SecurityEventNotificationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SecurityEventNotificationResponse \p k to a given json object \p j
void to_json(json& j, const SecurityEventNotificationResponse& k);

/// \brief Conversion from a given json object \p j to a given SecurityEventNotificationResponse \p k
void from_json(const json& j, SecurityEventNotificationResponse& k);

/// \brief Writes the string representation of the given SecurityEventNotificationResponse \p k to the given output
/// stream \p os
/// \returns an output stream with the SecurityEventNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const SecurityEventNotificationResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_SECURITYEVENTNOTIFICATION_HPP
