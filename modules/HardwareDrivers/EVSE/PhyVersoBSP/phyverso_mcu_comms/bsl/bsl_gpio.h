// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef PHYVERSO_MCU_COMMS_BSL_BSL_GPIO_H
#define PHYVERSO_MCU_COMMS_BSL_BSL_GPIO_H

#include <cstdint>

class BSL_GPIO {
public:
    struct _gpio_def {
        uint8_t bank;
        uint8_t pin;
    };

    BSL_GPIO(_gpio_def _bsl = {.bank = 1, .pin = 12}, _gpio_def _reset = {.bank = 1, .pin = 23});
    bool hard_reset(uint16_t ms_reset_time = default_ms_reset_time);
    bool enter_bsl();

private:
    bool set_pin(_gpio_def gpio, bool level);

    static constexpr uint16_t default_ms_reset_time = 10;
    static constexpr uint16_t ms_bsl_out_settle = 10;

    // should be changeable later by loading a conf file
    _gpio_def bsl_out;
    _gpio_def reset_out;
};

#endif // PHYVERSO_MCU_COMMS_BSL_BSL_GPIO_H
