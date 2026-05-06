// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "uk_random_delay/codec.hpp"
#include "nlohmann/json.hpp"
#include "uk_random_delay/API.hpp"
#include "uk_random_delay/json_codec.hpp"
#include "utilities/constants.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::uk_random_delay {

std::string serialize(CountDown const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, CountDown const& val) {
    os << serialize(val);
    return os;
}

template <> CountDown deserialize(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::uk_random_delay
