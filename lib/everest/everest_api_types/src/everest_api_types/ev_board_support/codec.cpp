// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "ev_board_support/codec.hpp"
#include "ev_board_support/API.hpp"
#include "ev_board_support/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <string>

namespace everest::lib::API::V1_0::types::ev_board_support {

std::string serialize(EvCpState val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(BspMeasurement const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, EvCpState const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const BspMeasurement& val) {
    os << serialize(val);
    return os;
}

template <> EvCpState deserialize(std::string const& s) {
    auto data = json::parse(s);
    EvCpState result = data;
    return result;
}

template <> BspMeasurement deserialize(std::string const& s) {
    auto data = json::parse(s);
    BspMeasurement result = data;
    return result;
}

} // namespace everest::lib::API::V1_0::types::ev_board_support
