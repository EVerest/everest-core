// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "display_message/json_codec.hpp"
#include "display_message/API.hpp"
#include "display_message/codec.hpp"

#include "nlohmann/json.hpp"
#include "text_message/json_codec.hpp"

namespace everest::lib::API::V1_0::types::display_message {

void to_json(json& j, MessagePriorityEnum const& k) noexcept {
    switch (k) {
    case MessagePriorityEnum::AlwaysFront:
        j = "AlwaysFront";
        return;
    case MessagePriorityEnum::InFront:
        j = "InFront";
        return;
    case MessagePriorityEnum::NormalCycle:
        j = "NormalCycle";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::display_message::MessagePriorityEnum";
}

void from_json(const json& j, MessagePriorityEnum& k) {
    std::string s = j;
    if (s == "AlwaysFront") {
        k = MessagePriorityEnum::AlwaysFront;
        return;
    }
    if (s == "InFront") {
        k = MessagePriorityEnum::InFront;
        return;
    }
    if (s == "NormalCycle") {
        k = MessagePriorityEnum::NormalCycle;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::auth::MessagePriorityEnum");
}

void to_json(json& j, MessageStateEnum const& k) noexcept {
    switch (k) {
    case MessageStateEnum::Charging:
        j = "Charging";
        return;
    case MessageStateEnum::Faulted:
        j = "Faulted";
        return;
    case MessageStateEnum::Idle:
        j = "Idle";
        return;
    case MessageStateEnum::Unavailable:
        j = "Unavailable";
        return;
    case MessageStateEnum::Suspending:
        j = "Suspending";
        return;
    case MessageStateEnum::Discharging:
        j = "Discharging";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::display_message::MessageStateEnum";
}

void from_json(const json& j, MessageStateEnum& k) {
    std::string s = j;
    if (s == "Charging") {
        k = MessageStateEnum::Charging;
        return;
    }
    if (s == "Faulted") {
        k = MessageStateEnum::Faulted;
        return;
    }
    if (s == "Idle") {
        k = MessageStateEnum::Idle;
        return;
    }
    if (s == "Unavailable") {
        k = MessageStateEnum::Unavailable;
        return;
    }
    if (s == "Suspending") {
        k = MessageStateEnum::Suspending;
        return;
    }
    if (s == "Discharging") {
        k = MessageStateEnum::Discharging;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::auth::MessageStateEnum");
}

void to_json(json& j, DisplayMessageStatusEnum const& k) noexcept {
    switch (k) {
    case DisplayMessageStatusEnum::Accepted:
        j = "Accepted";
        return;
    case DisplayMessageStatusEnum::NotSupportedMessageFormat:
        j = "NotSupportedMessageFormat";
        return;
    case DisplayMessageStatusEnum::Rejected:
        j = "Rejected";
        return;
    case DisplayMessageStatusEnum::NotSupportedPriority:
        j = "NotSupportedPriority";
        return;
    case DisplayMessageStatusEnum::NotSupportedState:
        j = "NotSupportedState";
        return;
    case DisplayMessageStatusEnum::UnknownTransaction:
        j = "UnknownTransaction";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::display_message::MessageStateEnum";
}

void from_json(const json& j, DisplayMessageStatusEnum& k) {
    std::string s = j;
    if (s == "Accepted") {
        k = DisplayMessageStatusEnum::Accepted;
        return;
    }
    if (s == "NotSupportedMessageFormat") {
        k = DisplayMessageStatusEnum::NotSupportedMessageFormat;
        return;
    }
    if (s == "Rejected") {
        k = DisplayMessageStatusEnum::Rejected;
        return;
    }
    if (s == "NotSupportedPriority") {
        k = DisplayMessageStatusEnum::NotSupportedPriority;
        return;
    }
    if (s == "NotSupportedState") {
        k = DisplayMessageStatusEnum::NotSupportedState;
        return;
    }
    if (s == "UnknownTransaction") {
        k = DisplayMessageStatusEnum::UnknownTransaction;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::auth::MessageStateEnum");
}

void to_json(json& j, ClearMessageResponseEnum const& k) noexcept {
    switch (k) {
    case ClearMessageResponseEnum::Accepted:
        j = "Accepted";
        return;
    case ClearMessageResponseEnum::Unknown:
        j = "Unknown";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::display_message::ClearMessageResponseEnum";
}

void from_json(const json& j, ClearMessageResponseEnum& k) {
    std::string s = j;
    if (s == "Accepted") {
        k = ClearMessageResponseEnum::Accepted;
        return;
    }
    if (s == "Unknown") {
        k = ClearMessageResponseEnum::Unknown;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::auth::ClearMessageResponseEnum");
}

void to_json(json& j, Identifier_type const& k) noexcept {
    switch (k) {
    case Identifier_type::IdToken:
        j = "IdToken";
        return;
    case Identifier_type::SessionId:
        j = "SessionId";
        return;
    case Identifier_type::TransactionId:
        j = "TransactionId";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::display_message::Identifier_type";
}

void from_json(const json& j, Identifier_type& k) {
    std::string s = j;
    if (s == "IdToken") {
        k = Identifier_type::IdToken;
        return;
    }
    if (s == "SessionId") {
        k = Identifier_type::SessionId;
        return;
    }
    if (s == "TransactionId") {
        k = Identifier_type::TransactionId;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::dispaly_messages::Identifier_type");
}

void to_json(json& j, DisplayMessage const& k) noexcept {
    j = json{
        {"message", k.message},
    };
    if (k.id) {
        j["id"] = k.id.value();
    }
    if (k.priority) {
        j["priority"] = k.priority.value();
    }
    if (k.state) {
        j["state"] = k.state.value();
    }
    if (k.timestamp_from) {
        j["timestamp_from"] = k.timestamp_from.value();
    }
    if (k.timestamp_to) {
        j["timestamp_to"] = k.timestamp_to.value();
    }
    if (k.identifier_id) {
        j["identifier_id"] = k.identifier_id.value();
    }
    if (k.identifier_type) {
        j["identifier_type"] = k.identifier_type.value();
    }
    if (k.qr_code) {
        j["qr_code"] = k.qr_code.value();
    }
}

void from_json(const json& j, DisplayMessage& k) {
    k.message = j.at("message");
    if (j.contains("id")) {
        k.id.emplace(j.at("id"));
    }
    if (j.contains("priority")) {
        k.priority.emplace(j.at("priority"));
    }
    if (j.contains("state")) {
        k.state.emplace(j.at("state"));
    }
    if (j.contains("timestamp_from")) {
        k.timestamp_from.emplace(j.at("timestamp_from"));
    }
    if (j.contains("timestamp_to")) {
        k.timestamp_to.emplace(j.at("timestamp_to"));
    }
    if (j.contains("identifier_id")) {
        k.identifier_id.emplace(j.at("identifier_id"));
    }
    if (j.contains("identifier_type")) {
        k.identifier_type.emplace(j.at("identifier_type"));
    }
    if (j.contains("qr_code")) {
        k.qr_code.emplace(j.at("qr_code"));
    }
}

void to_json(json& j, SetDisplayMessageRequest const& k) noexcept {
    j = json::array();
    for (auto val : k.display_messages) {
        j.push_back(val);
    }
}

void from_json(const json& j, SetDisplayMessageRequest& k) {
    k.display_messages.clear();
    for (auto val : j) {
        k.display_messages.push_back(val);
    }
}

void to_json(json& j, SetDisplayMessageResponse const& k) noexcept {
    j = json{
        {"status", k.status},
    };
    if (k.status_info) {
        j["status_info"] = k.status_info.value();
    }
}

void from_json(const json& j, SetDisplayMessageResponse& k) {
    k.status = j.at("status");
    if (j.contains("status_info")) {
        k.status_info.emplace(j.at("status_info"));
    }
}

void to_json(json& j, GetDisplayMessageRequest const& k) noexcept {
    j = json({});
    if (k.id) {
        if (j.size() == 0) {
            j = json{{"id", json::array()}};
        } else {
            j["id"] = json::array();
        }
        for (auto val : k.id.value()) {
            j["id"].push_back(val);
        }
    }
    if (k.priority) {
        j["priority"] = k.priority.value();
    }
    if (k.state) {
        j["state"] = k.state.value();
    }
}

void from_json(const json& j, GetDisplayMessageRequest& k) {
    if (j.contains("id")) {
        json arr = j.at("id");
        std::vector<int32_t> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.id.emplace(vec);
    }
    if (j.contains("priority")) {
        k.priority.emplace(j.at("priority"));
    }
    if (j.contains("state")) {
        k.state.emplace(j.at("state"));
    }
}

void to_json(json& j, GetDisplayMessageResponse const& k) noexcept {
    j = json({});
    if (k.status_info) {
        j["status_info"] = k.status_info.value();
    }
    if (k.messages) {
        if (j.size() == 0) {
            j = json{{"messages", json::array()}};
        } else {
            j["messages"] = json::array();
        }
        for (auto val : k.messages.value()) {
            j["messages"].push_back(val);
        }
    }
}

void from_json(const json& j, GetDisplayMessageResponse& k) {
    if (j.contains("status_info")) {
        k.status_info.emplace(j.at("status_info"));
    }
    if (j.contains("messages")) {
        json arr = j.at("messages");
        std::vector<DisplayMessage> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.messages.emplace(vec);
    }
}

void to_json(json& j, ClearDisplayMessageRequest const& k) noexcept {
    j = json{
        {"id", k.id},
    };
}

void from_json(const json& j, ClearDisplayMessageRequest& k) {
    k.id = j.at("id");
}

void to_json(json& j, ClearDisplayMessageResponse const& k) noexcept {
    j = json{
        {"status", k.status},
    };
    if (k.status_info) {
        j["status_info"] = k.status_info.value();
    }
}

void from_json(const json& j, ClearDisplayMessageResponse& k) {
    k.status = j.at("status");
    if (j.contains("status_info")) {
        k.status_info.emplace(j.at("status_info"));
    }
}

} // namespace everest::lib::API::V1_0::types::display_message
