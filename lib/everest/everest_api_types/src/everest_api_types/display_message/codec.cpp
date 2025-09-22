// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "display_message/codec.hpp"
#include "display_message/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"

namespace everest::lib::API::V1_0::types::display_message {

std::string serialize(MessagePriorityEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(MessageStateEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(DisplayMessageStatusEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ClearMessageResponseEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(MessageFormat val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Identifier_type val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(MessageContent const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(DisplayMessage const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(SetDisplayMessageRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(SetDisplayMessageResponse const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(GetDisplayMessageRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(GetDisplayMessageResponse const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ClearDisplayMessageRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ClearDisplayMessageResponse const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, MessagePriorityEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, MessageStateEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, DisplayMessageStatusEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ClearMessageResponseEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, MessageFormat const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Identifier_type const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, MessageContent const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, DisplayMessage const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SetDisplayMessageRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SetDisplayMessageResponse const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, GetDisplayMessageRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, GetDisplayMessageResponse const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ClearDisplayMessageRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ClearDisplayMessageResponse const& val) {
    os << serialize(val);
    return os;
}

template <> MessagePriorityEnum deserialize(std::string const& val) {
    auto data = json::parse(val);
    MessagePriorityEnum obj = data;
    return obj;
}

template <> MessageStateEnum deserialize(std::string const& val) {
    auto data = json::parse(val);
    MessageStateEnum obj = data;
    return obj;
}

template <> DisplayMessageStatusEnum deserialize(std::string const& val) {
    auto data = json::parse(val);
    DisplayMessageStatusEnum obj = data;
    return obj;
}

template <> ClearMessageResponseEnum deserialize(std::string const& val) {
    auto data = json::parse(val);
    ClearMessageResponseEnum obj = data;
    return obj;
}

template <> MessageFormat deserialize(std::string const& val) {
    auto data = json::parse(val);
    MessageFormat obj = data;
    return obj;
}

template <> Identifier_type deserialize(std::string const& val) {
    auto data = json::parse(val);
    Identifier_type obj = data;
    return obj;
}

template <> MessageContent deserialize(std::string const& val) {
    auto data = json::parse(val);
    MessageContent obj = data;
    return obj;
}

template <> DisplayMessage deserialize(std::string const& val) {
    auto data = json::parse(val);
    DisplayMessage obj = data;
    return obj;
}

template <> SetDisplayMessageRequest deserialize(std::string const& val) {
    auto data = json::parse(val);
    SetDisplayMessageRequest obj = data;
    return obj;
}

template <> SetDisplayMessageResponse deserialize(std::string const& val) {
    auto data = json::parse(val);
    SetDisplayMessageResponse obj = data;
    return obj;
}

template <> GetDisplayMessageRequest deserialize(std::string const& val) {
    auto data = json::parse(val);
    GetDisplayMessageRequest obj = data;
    return obj;
}

template <> GetDisplayMessageResponse deserialize(std::string const& val) {
    auto data = json::parse(val);
    GetDisplayMessageResponse obj = data;
    return obj;
}

template <> ClearDisplayMessageRequest deserialize(std::string const& val) {
    auto data = json::parse(val);
    ClearDisplayMessageRequest obj = data;
    return obj;
}

template <> ClearDisplayMessageResponse deserialize(std::string const& val) {
    auto data = json::parse(val);
    ClearDisplayMessageResponse obj = data;
    return obj;
}

} // namespace everest::lib::API::V1_0::types::display_message
