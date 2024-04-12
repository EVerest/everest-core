// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/NotifyDisplayMessages.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string NotifyDisplayMessagesRequest::get_type() const {
    return "NotifyDisplayMessages";
}

void to_json(json& j, const NotifyDisplayMessagesRequest& k) {
    // the required parts of the message
    j = json{
        {"requestId", k.requestId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.messageInfo) {
        j["messageInfo"] = json::array();
        for (auto val : k.messageInfo.value()) {
            j["messageInfo"].push_back(val);
        }
    }
    if (k.tbc) {
        j["tbc"] = k.tbc.value();
    }
}

void from_json(const json& j, NotifyDisplayMessagesRequest& k) {
    // the required parts of the message
    k.requestId = j.at("requestId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("messageInfo")) {
        json arr = j.at("messageInfo");
        std::vector<MessageInfo> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.messageInfo.emplace(vec);
    }
    if (j.contains("tbc")) {
        k.tbc.emplace(j.at("tbc"));
    }
}

/// \brief Writes the string representation of the given NotifyDisplayMessagesRequest \p k to the given output stream \p
/// os \returns an output stream with the NotifyDisplayMessagesRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyDisplayMessagesRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyDisplayMessagesResponse::get_type() const {
    return "NotifyDisplayMessagesResponse";
}

void to_json(json& j, const NotifyDisplayMessagesResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyDisplayMessagesResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyDisplayMessagesResponse \p k to the given output stream
/// \p os \returns an output stream with the NotifyDisplayMessagesResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyDisplayMessagesResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
