// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <chrono>
#include <optional>

namespace ieee2030::io {

using TimePoint = std::chrono::steady_clock::time_point;

inline TimePoint get_current_time_point() {
    return std::chrono::steady_clock::now();
}

inline TimePoint offset_time_point_by_ms(const TimePoint& time_point, int32_t offset) {
    return time_point + std::chrono::milliseconds(offset);
}

// Todo: How to handle parallel timeouts?
class Timeout {
public:
    Timeout(){};

    void start(float timeout_s); // Todo: Perhaps change that chrono::seconds
    void reset();

    std::optional<bool> timeout_reached();

private:
    std::optional<TimePoint> timeout_point;
};

} // namespace ieee2030::io
