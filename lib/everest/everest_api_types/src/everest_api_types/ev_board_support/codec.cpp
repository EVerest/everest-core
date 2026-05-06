// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "ev_board_support/codec.hpp"
#include "ev_board_support/API.hpp"
#include "ev_board_support/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <string>

namespace everest::lib::API::V1_0::types::ev_board_support {

std::string serialize(EvCpState val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(BspMeasurement const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, EvCpState const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const BspMeasurement& val) {
    os << serialize(val);
    return os;
}

template <> EvCpState deserialize(std::string const& val) {
    return json::parse(val);
}

template <> BspMeasurement deserialize(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::ev_board_support
