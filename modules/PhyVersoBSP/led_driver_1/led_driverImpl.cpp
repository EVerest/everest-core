// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "led_driverImpl.hpp"

namespace module {
namespace led_driver_1 {

void led_driverImpl::init() {
}

void led_driverImpl::ready() {
}

void led_driverImpl::handle_set_led_state(std::string& led_state, int& brightness) {
    //TODO: Implement me
    EVLOG_info << "Setting LED for connector 1 to " << led_state;
    mod->serial.set_led_state(1, led_state, brightness);
}

} // namespace led_driver
}
