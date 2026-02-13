// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <ieee2030/common/io/time.hpp>

#include <cmath>

namespace ieee2030::io {

void Timeout::start(float timeout_s) {
    timeout_point =
        std::chrono::steady_clock::now() + std::chrono::seconds(static_cast<int64_t>(std::round(timeout_s)));
}

void Timeout::reset() {
    timeout_point.reset();
}

std::optional<bool> Timeout::timeout_reached() {

    if (!timeout_point.has_value()) {
        return std::nullopt;
    }

    if (get_current_time_point() >= timeout_point.value()) {
        return true;
    }
    return false;
}

} // namespace ieee2030::io
