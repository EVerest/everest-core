// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <fusion_charger/modbus/registers/utils.hpp>

namespace fusion_charger::modbus_driver {
namespace utils {

bool always_report() {
    return true;
}
std::function<std::uint16_t()> wrap_alarm_register_func(const std::function<bool()>& func) {
    return [func]() { return static_cast<std::uint16_t>(func() ? 1 : 0); };
}

}; // namespace utils
}; // namespace fusion_charger::modbus_driver
