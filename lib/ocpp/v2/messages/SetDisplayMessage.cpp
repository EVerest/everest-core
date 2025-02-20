// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/SetDisplayMessage.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string SetDisplayMessageRequest::get_type() const {
    return "SetDisplayMessage";
}

void to_json(json& j, const SetDisplayMessageRequest& k) {
    // the required parts of the message
    j = json{
        {"message", k.message},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, SetDisplayMessageRequest& k) {
    // the required parts of the message
    k.message = j.at("message");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given SetDisplayMessageRequest \p k to the given output stream \p os
/// \returns an output stream with the SetDisplayMessageRequest written to
std::ostream& operator<<(std::ostream& os, const SetDisplayMessageRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string SetDisplayMessageResponse::get_type() const {
    return "SetDisplayMessageResponse";
}

void to_json(json& j, const SetDisplayMessageResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::display_message_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, SetDisplayMessageResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_display_message_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given SetDisplayMessageResponse \p k to the given output stream \p os
/// \returns an output stream with the SetDisplayMessageResponse written to
std::ostream& operator<<(std::ostream& os, const SetDisplayMessageResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
