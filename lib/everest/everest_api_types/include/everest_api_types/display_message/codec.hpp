// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once
#include <everest_api_types/display_message/API.hpp>

namespace everest::lib::API::V1_0::types::display_message {

std::string serialize(MessagePriorityEnum val) noexcept;
std::string serialize(MessageStateEnum val) noexcept;
std::string serialize(DisplayMessageStatusEnum val) noexcept;
std::string serialize(ClearMessageResponseEnum val) noexcept;
std::string serialize(MessageFormat val) noexcept;
std::string serialize(Identifier_type val) noexcept;
std::string serialize(MessageContent const& val) noexcept;
std::string serialize(DisplayMessage const& val) noexcept;
std::string serialize(SetDisplayMessageRequest const& val) noexcept;
std::string serialize(SetDisplayMessageResponse const& val) noexcept;
std::string serialize(GetDisplayMessageRequest const& val) noexcept;
std::string serialize(GetDisplayMessageResponse const& val) noexcept;
std::string serialize(ClearDisplayMessageRequest const& val) noexcept;
std::string serialize(ClearDisplayMessageResponse const& val) noexcept;

std::ostream& operator<<(std::ostream& os, MessagePriorityEnum const& val);
std::ostream& operator<<(std::ostream& os, MessageStateEnum const& val);
std::ostream& operator<<(std::ostream& os, DisplayMessageStatusEnum const& val);
std::ostream& operator<<(std::ostream& os, ClearMessageResponseEnum const& val);
std::ostream& operator<<(std::ostream& os, MessageFormat const& val);
std::ostream& operator<<(std::ostream& os, Identifier_type const& val);
std::ostream& operator<<(std::ostream& os, MessageContent const& val);
std::ostream& operator<<(std::ostream& os, DisplayMessage const& val);
std::ostream& operator<<(std::ostream& os, SetDisplayMessageRequest const& val);
std::ostream& operator<<(std::ostream& os, SetDisplayMessageResponse const& val);
std::ostream& operator<<(std::ostream& os, GetDisplayMessageRequest const& val);
std::ostream& operator<<(std::ostream& os, GetDisplayMessageResponse const& val);
std::ostream& operator<<(std::ostream& os, ClearDisplayMessageRequest const& val);
std::ostream& operator<<(std::ostream& os, ClearDisplayMessageResponse const& val);

template <class T> T deserialize(std::string const& val);
template <class T> std::optional<T> try_deserialize(std::string const& val) {
    try {
        return deserialize<T>(val);
    } catch (...) {
        return std::nullopt;
    }
}
template <class T> bool adl_deserialize(std::string const& json_data, T& obj) {
    auto opt = try_deserialize<T>(json_data);
    if (opt) {
        obj = opt.value();
        return true;
    }
    return false;
}

} // namespace everest::lib::API::V1_0::types::display_message
