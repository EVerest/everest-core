// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v16/messages/ExtendedTriggerMessage.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string ExtendedTriggerMessageRequest::get_type() const {
    return "ExtendedTriggerMessage";
}

void to_json(json& j, const ExtendedTriggerMessageRequest& k) {
    // the required parts of the message
    j = json{
        {"requestedMessage", conversions::message_trigger_enum_type_to_string(k.requestedMessage)},
    };
    // the optional parts of the message
    if (k.connectorId) {
        j["connectorId"] = k.connectorId.value();
    }
}

void from_json(const json& j, ExtendedTriggerMessageRequest& k) {
    // the required parts of the message
    k.requestedMessage = conversions::string_to_message_trigger_enum_type(j.at("requestedMessage"));

    // the optional parts of the message
    if (j.contains("connectorId")) {
        k.connectorId.emplace(j.at("connectorId"));
    }
}

/// \brief Writes the string representation of the given ExtendedTriggerMessageRequest \p k to the given output stream
/// \p os
/// \returns an output stream with the ExtendedTriggerMessageRequest written to
std::ostream& operator<<(std::ostream& os, const ExtendedTriggerMessageRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ExtendedTriggerMessageResponse::get_type() const {
    return "ExtendedTriggerMessageResponse";
}

void to_json(json& j, const ExtendedTriggerMessageResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::trigger_message_status_enum_type_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, ExtendedTriggerMessageResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_trigger_message_status_enum_type(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given ExtendedTriggerMessageResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the ExtendedTriggerMessageResponse written to
std::ostream& operator<<(std::ostream& os, const ExtendedTriggerMessageResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
