// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "bsl_gpio.h"
#include <chrono>
#include <iostream>
#include <thread>

BSL_GPIO::BSL_GPIO(_gpio_def _bsl, _gpio_def _reset) : bsl_out(_bsl), reset_out(_reset) {
}

bool BSL_GPIO::hard_reset(uint16_t ms_reset_time) {
    bool status = set_pin(reset_out, true);
    if (!status) {
        printf("Could set reset active\n");
        return status;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(ms_reset_time));

    status = set_pin(reset_out, false);
    if (!status) {
        printf("Could set reset inactive\n");
        return status;
    }

    return status;
}

bool BSL_GPIO::enter_bsl() {
    bool status = set_pin(bsl_out, true);
    if (!status) {
        printf("Could not set BSL pin high\n");
        return status;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(ms_bsl_out_settle));
    status = hard_reset();
    if (!status) {
        printf("Could not reset\n");
        return status;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(ms_bsl_out_settle));
    status = set_pin(bsl_out, false);
    if (!status) {
        printf("Could not set BSL pin low\n");
        return status;
    }

    return status;
}

bool BSL_GPIO::set_pin(_gpio_def gpio, bool level) {
    char* cmd;
    int size = asprintf(&cmd, "gpioset %d %d=%d", gpio.bank, gpio.pin, (level ? 1 : 0));
    if ((size == -1) || (!cmd)) {
        return false;
    }
    // printf("%s\n", cmd);    // debug
    int status = system(cmd);
    free(cmd);

    if (status == 0) {
        return true;
    } else {
        return false;
    }
}
