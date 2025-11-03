// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "slac/codec.hpp"
#include "nlohmann/json.hpp"
#include "slac/API.hpp"
#include "slac/json_codec.hpp"
#include "utilities/constants.hpp"
#include <string>

namespace everest::lib::API::V1_0::types::slac {

std::string serialize(State val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, State const& val) {
    os << serialize(val);
    return os;
}

template <> State deserialize(std::string const& s) {
    auto data = json::parse(s);
    State result = data;
    return result;
}
} // namespace everest::lib::API::V1_0::types::slac
