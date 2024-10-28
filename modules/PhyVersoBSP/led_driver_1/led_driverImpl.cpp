// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "led_driverImpl.hpp"

namespace {
LedState led_state_to_proto(const types::led_state::LedState& led_state) {
    if (led_state == types::led_state::LedState::Red)
        return LedState_RED;
    if (led_state == types::led_state::LedState::Blue)
        return LedState_BLUE;
    if (led_state == types::led_state::LedState::Green)
        return LedState_GREEN;
    return LedState_RED;
}
} // namespace

namespace module {
namespace led_driver_1 {

void led_driverImpl::init() {
}

void led_driverImpl::ready() {
}


void led_driverImpl::handle_set_led_state(types::led_state::LedState& led_state, int& brightness) {
    EVLOG_info << "Setting LED for connector 1 to " << led_state;
    mod->serial.set_led_state(1, led_state_to_proto(led_state), brightness);
}

} // namespace led_driver_1
} // namespace module
