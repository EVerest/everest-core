// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EV_SLAC_STATES_TIMING_HELPER_HPP
#define EV_SLAC_STATES_TIMING_HELPER_HPP

#include <chrono>

template <typename TimepointType> auto milliseconds_left(const TimepointType& from, const TimepointType& to) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(to - from).count();
}

#endif // EV_SLAC_STATES_TIMING_HELPER_HPP
