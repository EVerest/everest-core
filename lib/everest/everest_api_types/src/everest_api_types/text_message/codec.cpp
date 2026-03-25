// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "text_message/codec.hpp"
#include "nlohmann/json.hpp"
#include "text_message/API.hpp"
#include "text_message/json_codec.hpp"
#include "utilities/constants.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::text_message {

std::string serialize(MessageFormat val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(MessageContent const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, MessageFormat const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, MessageContent const& val) {
    os << serialize(val);
    return os;
}

template <> MessageFormat deserialize(std::string const& val) {
    return json::parse(val);
}

template <> MessageContent deserialize(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::text_message
