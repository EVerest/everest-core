// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_LOGSTATUSNOTIFICATION_HPP
#define OCPP_V16_LOGSTATUSNOTIFICATION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v16/enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP LogStatusNotification message
struct LogStatusNotificationRequest : public ocpp::Message {
    UploadLogStatusEnumType status;
    std::optional<int32_t> requestId;

    /// \brief Provides the type of this LogStatusNotification message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given LogStatusNotificationRequest \p k to a given json object \p j
void to_json(json& j, const LogStatusNotificationRequest& k);

/// \brief Conversion from a given json object \p j to a given LogStatusNotificationRequest \p k
void from_json(const json& j, LogStatusNotificationRequest& k);

/// \brief Writes the string representation of the given LogStatusNotificationRequest \p k to the given output stream \p
/// os \returns an output stream with the LogStatusNotificationRequest written to
std::ostream& operator<<(std::ostream& os, const LogStatusNotificationRequest& k);

/// \brief Contains a OCPP LogStatusNotificationResponse message
struct LogStatusNotificationResponse : public ocpp::Message {

    /// \brief Provides the type of this LogStatusNotificationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given LogStatusNotificationResponse \p k to a given json object \p j
void to_json(json& j, const LogStatusNotificationResponse& k);

/// \brief Conversion from a given json object \p j to a given LogStatusNotificationResponse \p k
void from_json(const json& j, LogStatusNotificationResponse& k);

/// \brief Writes the string representation of the given LogStatusNotificationResponse \p k to the given output stream
/// \p os \returns an output stream with the LogStatusNotificationResponse written to
std::ostream& operator<<(std::ostream& os, const LogStatusNotificationResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_LOGSTATUSNOTIFICATION_HPP
