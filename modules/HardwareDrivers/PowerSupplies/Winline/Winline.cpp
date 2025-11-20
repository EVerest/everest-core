// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "Winline.hpp"

namespace module {

void Winline::init() {
    acdc = std::make_unique<WinlineCanDevice>();
    acdc->set_can_device(config.can_device);
    acdc->set_config_values(config.module_addresses, config.group_address, config.device_connection_timeout_s,
                            config.controller_address, config.power_state_grace_period_ms, config.altitude_setting_m,
                            config.input_mode, config.module_current_limit_point);

    // Set altitude on all modules
    acdc->set_altitude_all_modules();

    // Set input mode on all modules
    acdc->set_input_mode_all_modules();

    // Set current limit point on all modules
    acdc->set_current_limit_point_all_modules();

    invoke_init(*p_main);
}

void Winline::ready() {
    invoke_ready(*p_main);
}

} // namespace module
