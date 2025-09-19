// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

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
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(MessageContent const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
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
    auto data = json::parse(val);
    MessageFormat obj = data;
    return obj;
}

template <> MessageContent deserialize(std::string const& val) {
    auto data = json::parse(val);
    MessageContent obj = data;
    return obj;
}

} // namespace everest::lib::API::V1_0::types::text_message
