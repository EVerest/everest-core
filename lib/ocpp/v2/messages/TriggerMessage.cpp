// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/TriggerMessage.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string TriggerMessageRequest::get_type() const {
    return "TriggerMessage";
}

void to_json(json& j, const TriggerMessageRequest& k) {
    // the required parts of the message
    j = json{
        {"requestedMessage", conversions::message_trigger_enum_to_string(k.requestedMessage)},
    };
    // the optional parts of the message
    if (k.evse) {
        j["evse"] = k.evse.value();
    }
    if (k.customTrigger) {
        j["customTrigger"] = k.customTrigger.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, TriggerMessageRequest& k) {
    // the required parts of the message
    k.requestedMessage = conversions::string_to_message_trigger_enum(j.at("requestedMessage"));

    // the optional parts of the message
    if (j.contains("evse")) {
        k.evse.emplace(j.at("evse"));
    }
    if (j.contains("customTrigger")) {
        k.customTrigger.emplace(j.at("customTrigger"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given TriggerMessageRequest \p k to the given output stream \p os
/// \returns an output stream with the TriggerMessageRequest written to
std::ostream& operator<<(std::ostream& os, const TriggerMessageRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string TriggerMessageResponse::get_type() const {
    return "TriggerMessageResponse";
}

void to_json(json& j, const TriggerMessageResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::trigger_message_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, TriggerMessageResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_trigger_message_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given TriggerMessageResponse \p k to the given output stream \p os
/// \returns an output stream with the TriggerMessageResponse written to
std::ostream& operator<<(std::ostream& os, const TriggerMessageResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
