// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "display_message/codec.hpp"
#include "display_message/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"

namespace everest::lib::API::V1_0::types::display_message {

std::string serialize(MessagePriorityEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(MessageStateEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(DisplayMessageStatusEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ClearMessageResponseEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Identifier_type val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(DisplayMessage const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SetDisplayMessageRequest const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SetDisplayMessageResponse const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(GetDisplayMessageRequest const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(GetDisplayMessageResponse const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ClearDisplayMessageRequest const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ClearDisplayMessageResponse const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
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

std::ostream& operator<<(std::ostream& os, Identifier_type const& val) {
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
    return json::parse(val);
}

template <> MessageStateEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> DisplayMessageStatusEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ClearMessageResponseEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Identifier_type deserialize(std::string const& val) {
    return json::parse(val);
}

template <> DisplayMessage deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SetDisplayMessageRequest deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SetDisplayMessageResponse deserialize(std::string const& val) {
    return json::parse(val);
}

template <> GetDisplayMessageRequest deserialize(std::string const& val) {
    return json::parse(val);
}

template <> GetDisplayMessageResponse deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ClearDisplayMessageRequest deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ClearDisplayMessageResponse deserialize(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::display_message
