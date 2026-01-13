// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdint>
#include <functional>

namespace fusion_charger::modbus_driver {
namespace utils {

bool always_report();

template <typename T> void ignore_write(T) {
}

/**
 * @brief Wraps a function that returns a boolean into a function that returns a
 * uint16_t value, where true maps to 1 and false maps to 0.
 * @param func The function to wrap.
 * @return A function that calls \c func and returns uint16_t.
 */
std::function<std::uint16_t()> wrap_alarm_register_func(const std::function<bool()>& func);

} // namespace utils
} // namespace fusion_charger::modbus_driver
