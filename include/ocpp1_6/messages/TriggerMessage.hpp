// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_TRIGGERMESSAGE_HPP
#define OCPP1_6_TRIGGERMESSAGE_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct TriggerMessageRequest : public Message {
    MessageTrigger requestedMessage;
    boost::optional<int32_t> connectorId;

    std::string get_type() const {
        return "TriggerMessage";
    }

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

    friend void from_json(const json& j, TriggerMessageRequest& k) {
        // the required parts of the message
        k.requestedMessage = conversions::string_to_message_trigger(j.at("requestedMessage"));

        // the optional parts of the message
        if (j.contains("connectorId")) {
            k.connectorId.emplace(j.at("connectorId"));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const TriggerMessageRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct TriggerMessageResponse : public Message {
    TriggerMessageStatus status;

    std::string get_type() const {
        return "TriggerMessageResponse";
    }

    friend void to_json(json& j, const TriggerMessageResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::trigger_message_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, TriggerMessageResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_trigger_message_status(j.at("status"));

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const TriggerMessageResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_TRIGGERMESSAGE_HPP
