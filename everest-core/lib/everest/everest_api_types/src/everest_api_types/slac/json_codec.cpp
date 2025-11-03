// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "slac/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "slac/API.hpp"
#include "slac/codec.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::slac {

void from_json(json const& j, State& k) {
    std::string s = j;
    if (s == "UNMATCHED") {
        k = State::UNMATCHED;
        return;
    }
    if (s == "MATCHING") {
        k = State::MATCHING;
        return;
    }
    if (s == "MATCHED") {
        k = State::MATCHED;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type API_V1_0_TYPES_SLAC_STATE");
}

void to_json(json& j, State const& k) noexcept {
    switch (k) {
    case State::UNMATCHED:
        j = "UNMATCHED";
        return;
    case State::MATCHING:
        j = "MATCHING";
        return;
    case State::MATCHED:
        j = "MATCHED";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::SLAC::STATE";
}
} // namespace everest::lib::API::V1_0::types::slac
