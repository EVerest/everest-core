// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <phDriver_Linux_Int.h>
#include <Board_Pi4Rc663.h>

// argument output as char, in order to avoid the bool from <stdbool.h> to be carried into the c++ user code
void GPIO_reconfigure_pin(size_t gpio, int output_int) {
    PiGpio_unexport(gpio);
    PiGpio_export(gpio);
    bool output = (output_int == 0 ? false : true);
    PiGpio_set_direction(gpio, output);
    if (PIN_IRQ_TRIGGER_TYPE == PH_DRIVER_INTERRUPT_RISINGEDGE) {
        PiGpio_set_edge(gpio, true, false);
    } else {
        PiGpio_set_edge(gpio, false, true);
    }
}

phStatus_t GPIO_read_pin(size_t gpio, uint8_t *pGpioVal) {
    return PiGpio_read(gpio, pGpioVal);
}

phStatus_t GPIO_poll_pin(size_t gpio, int timeOutms) {
    return PiGpio_poll(PHDRIVER_PIN_IRQ, 1, timeOutms);
}
