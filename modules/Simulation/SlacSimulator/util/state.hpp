// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef UTIL_STATE_HPP
#define UTIL_STATE_HPP

#include <cstdint>
#include <string>

namespace module::util {

enum class State : uint8_t {
    UNMATCHED = 0,
    MATCHING = 1,
    MATCHED = 2,
};

std::string state_to_string(State state);

} // namespace module::util

#endif
