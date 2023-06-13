// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_TRIGGERMESSAGE_HPP
#define OCPP_V16_TRIGGERMESSAGE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v16/enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP TriggerMessage message
struct TriggerMessageRequest : public ocpp::Message {
    MessageTrigger requestedMessage;
    std::optional<int32_t> connectorId;

    /// \brief Provides the type of this TriggerMessage message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given TriggerMessageRequest \p k to a given json object \p j
void to_json(json& j, const TriggerMessageRequest& k);

/// \brief Conversion from a given json object \p j to a given TriggerMessageRequest \p k
void from_json(const json& j, TriggerMessageRequest& k);

/// \brief Writes the string representation of the given TriggerMessageRequest \p k to the given output stream \p os
/// \returns an output stream with the TriggerMessageRequest written to
std::ostream& operator<<(std::ostream& os, const TriggerMessageRequest& k);

/// \brief Contains a OCPP TriggerMessageResponse message
struct TriggerMessageResponse : public ocpp::Message {
    TriggerMessageStatus status;

    /// \brief Provides the type of this TriggerMessageResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given TriggerMessageResponse \p k to a given json object \p j
void to_json(json& j, const TriggerMessageResponse& k);

/// \brief Conversion from a given json object \p j to a given TriggerMessageResponse \p k
void from_json(const json& j, TriggerMessageResponse& k);

/// \brief Writes the string representation of the given TriggerMessageResponse \p k to the given output stream \p os
/// \returns an output stream with the TriggerMessageResponse written to
std::ostream& operator<<(std::ostream& os, const TriggerMessageResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_TRIGGERMESSAGE_HPP
