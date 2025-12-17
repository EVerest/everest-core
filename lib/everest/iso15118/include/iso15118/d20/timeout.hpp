// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <array>
#include <optional>
#include <vector>

#include <iso15118/io/time.hpp>

namespace iso15118::d20 {

template <typename T> constexpr auto to_underlying_value(T t) {
    return static_cast<std::underlying_type_t<T>>(t);
}

enum class TimeoutType : uint8_t {
    SEQUENCE = 0,
    PERFORMANCE,
    ONGOING,
    CONTACTOR,
};

constexpr uint8_t TIMEOUT_TYPE_SIZE = 4;

static_assert(TIMEOUT_TYPE_SIZE == to_underlying_value(TimeoutType::CONTACTOR) + 1,
              "TIMEOUT_TYPE_SIZE should be in sync with the TimeoutType enum definition");

constexpr auto TIMEOUT_ONGOING = 1000 * 55;
constexpr auto TIMEOUT_SEQUENCE = 1000 * 60;
constexpr auto TIMEOUT_EIM_ONGOING = 1000 * 60 * 3;

class Timeouts {
public:
    explicit Timeouts() = default;
    ~Timeouts() = default;

    void start_timeout(TimeoutType type, uint32_t timeout_ms);
    void stop_timeout(TimeoutType type);
    void reset_timeout(TimeoutType type);
    std::optional<std::vector<TimeoutType>> check();

private:
    std::array<std::optional<Timeout>, TIMEOUT_TYPE_SIZE> timeouts;
};

} // namespace iso15118::d20
