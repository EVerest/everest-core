// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "state.hpp"

namespace module::util {

std::string state_to_string(State state) {
    switch (state) {
    case State::UNMATCHED:
        return "UNMATCHED";
    case State::MATCHING:
        return "MATCHING";
    case State::MATCHED:
        return "MATCHED";
    default:
        return "";
    }
}

} // namespace module::util
