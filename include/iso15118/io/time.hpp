// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <chrono>

namespace iso15118 {

using TimePoint = std::chrono::steady_clock::time_point;

inline TimePoint get_current_time_point() {
    return std::chrono::steady_clock::now();
}

inline TimePoint offset_time_point_by_ms(const TimePoint& time_point, int32_t offset) {
    return time_point + std::chrono::milliseconds(offset);
}

inline int32_t get_timeout_ms_until(const TimePoint& until, int32_t max_timeout_ms) {
    const auto duration =
        std::chrono::duration_cast<std::chrono::duration<int32_t, std::milli>>(until - get_current_time_point());
    const auto timeout_ms = duration.count();

    if (timeout_ms < max_timeout_ms) {
        return timeout_ms;
    } else {
        return max_timeout_ms;
    }
}

} // namespace iso15118
