// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/timeout.hpp>

#include <map>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d20 {

void Timeouts::start_timeout(TimeoutType type, uint32_t timeout_ms) {
    const auto type_u8 = to_underlying_value(type);
    if (timeouts.at(type_u8).has_value()) {
        logf_warning("Timeout %u already started", type_u8);
        return;
    }
    timeouts.at(type_u8).emplace(Timeout(timeout_ms));
}

void Timeouts::stop_timeout(TimeoutType type) {
    const auto type_u8 = to_underlying_value(type);
    if (not timeouts.at(type_u8).has_value()) {
        logf_warning("Timeout %u is not started", type_u8);
    }
    timeouts.at(type_u8).reset();
}

void Timeouts::reset_timeout(TimeoutType type) {
    const auto type_u8 = to_underlying_value(type);
    timeouts.at(type_u8).reset();
}

std::optional<std::vector<TimeoutType>> Timeouts::check() {
    bool reached{false};

    std::map<TimePoint, TimeoutType> active_timeouts_map;

    for (uint8_t i = 0; i < TIMEOUT_TYPE_SIZE; i++) {
        auto timeout = timeouts.at(i);
        if (timeout.has_value() and timeout.value().is_reached()) {
            active_timeouts_map.insert({timeout.value().get_timeout_point(), static_cast<TimeoutType>(i)});
            reached = true;
        }
    }

    if (reached) {
        std::vector<TimeoutType> active_timeouts{};
        for (const auto& [_, value] : active_timeouts_map) {
            active_timeouts.push_back(value);
        }
        return active_timeouts;
    }

    return std::nullopt;
}

} // namespace iso15118::d20
