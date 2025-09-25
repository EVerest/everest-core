/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "Winline.hpp"

namespace module {

void Winline::init() {
    acdc = std::make_unique<WinlineCanDevice>();
    acdc->set_can_device(config.can_device);
    acdc->set_config_values(config.module_addresses, config.group_address, config.device_connection_timeout_s,
                            config.controller_address, config.power_state_grace_period_ms, config.altitude_setting_m,
                            config.input_mode);

    // Set altitude on all modules
    acdc->set_altitude_all_modules();

    // Set input mode on all modules
    acdc->set_input_mode_all_modules();

    invoke_init(*p_main);
}

void Winline::ready() {
    invoke_ready(*p_main);
}

} // namespace module
