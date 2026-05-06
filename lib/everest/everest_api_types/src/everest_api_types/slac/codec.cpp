// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "slac/codec.hpp"
#include "nlohmann/json.hpp"
#include "slac/API.hpp"
#include "slac/json_codec.hpp"
#include "utilities/constants.hpp"
#include <string>

namespace everest::lib::API::V1_0::types::slac {

std::string serialize(State val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, State const& val) {
    os << serialize(val);
    return os;
}

template <> State deserialize(std::string const& val) {
    return json::parse(val);
}
} // namespace everest::lib::API::V1_0::types::slac
