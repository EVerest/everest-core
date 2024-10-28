// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "led_driverImpl.hpp"

namespace module {
namespace led_driver_2 {

void led_driverImpl::init() {
}

void led_driverImpl::ready() {
}


void led_driverImpl::handle_set_led_state(types::led_state::LedState& led_state, int& brightness) {
    EVLOG_info << "Setting LED for connector 2 to R:" << led_state.red << " G:" << led_state.green << " B: " << led_state.blue;
    const auto message = LedStateMessage{led_state.red, led_state.green, led_state.blue, brightness};
    mod->serial.set_led_state(2, message);
}

} // namespace led_driver_2
} // namespace module
