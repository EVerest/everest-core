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
    std::string get_type() const {
        return "TriggerMessage";
    }

    /// \brief Conversion from a given TriggerMessageRequest \p k to a given json object \p j
    friend void to_json(json& j, const TriggerMessageRequest& k) {
        // the required parts of the message
        j = json{
            {"requestedMessage", conversions::message_trigger_to_string(k.requestedMessage)},
        };
        // the optional parts of the message
        if (k.connectorId) {
            j["connectorId"] = k.connectorId.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given TriggerMessageRequest \p k
    friend void from_json(const json& j, TriggerMessageRequest& k) {
        // the required parts of the message
        k.requestedMessage = conversions::string_to_message_trigger(j.at("requestedMessage"));

        // the optional parts of the message
        if (j.contains("connectorId")) {
            k.connectorId.emplace(j.at("connectorId"));
        }
    }

    /// \brief Writes the string representation of the given TriggerMessageRequest \p k to the given output stream \p os
    /// \returns an output stream with the TriggerMessageRequest written to
    friend std::ostream& operator<<(std::ostream& os, const TriggerMessageRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 TriggerMessageResponse message
struct TriggerMessageResponse : public Message {
    TriggerMessageStatus status;

    /// \brief Provides the type of this TriggerMessageResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "TriggerMessageResponse";
    }

    /// \brief Conversion from a given TriggerMessageResponse \p k to a given json object \p j
    friend void to_json(json& j, const TriggerMessageResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::trigger_message_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given TriggerMessageResponse \p k
    friend void from_json(const json& j, TriggerMessageResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_trigger_message_status(j.at("status"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given TriggerMessageResponse \p k to the given output stream \p
    /// os \returns an output stream with the TriggerMessageResponse written to
    friend std::ostream& operator<<(std::ostream& os, const TriggerMessageResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_TRIGGERMESSAGE_HPP
