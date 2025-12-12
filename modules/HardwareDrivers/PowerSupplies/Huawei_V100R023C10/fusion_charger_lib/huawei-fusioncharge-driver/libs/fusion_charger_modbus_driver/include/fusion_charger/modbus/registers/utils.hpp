// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstddef>

namespace fusion_charger::modbus_driver {
namespace utils {

bool always_report();

template <typename T> void ignore_write(T) {
}

} // namespace utils
} // namespace fusion_charger::modbus_driver
