// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_TRIGGERMESSAGE_HPP
#define OCPP1_6_TRIGGERMESSAGE_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 TriggerMessage message
struct TriggerMessageRequest : public Message {
    MessageTrigger requestedMessage;
    boost::optional<int32_t> connectorId;

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

/// \brief Contains a OCPP 1.6 TriggerMessageResponse message
struct TriggerMessageResponse : public Message {
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

} // namespace ocpp1_6

#endif // OCPP1_6_TRIGGERMESSAGE_HPP
