// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "SLIPCommHub.hpp"

namespace module {

void SLIPCommHub::init() {
    invoke_init(*p_main);
    initHW();
}

void SLIPCommHub::ready() {
    invoke_ready(*p_main);
    slip.txrx(0xFF, 0x4114,0);
}

void SLIPCommHub::initHW(){
    Everest::GpioSettings rxtx_gpio_settings;

    /*rxtx_gpio_settings.chip_name = config.rxtx_gpio_chip;
    rxtx_gpio_settings.line_number = config.rxtx_gpio_line;
    rxtx_gpio_settings.inverted = config.rxtx_gpio_tx_high;*/

    rxtx_gpio_settings.chip_name = "";
    rxtx_gpio_settings.line_number = 0;
    rxtx_gpio_settings.inverted = false;

    if (!slip.open_device(config.serial_port, config.baudrate, /*config.ignore_echo*/false, rxtx_gpio_settings,
                          static_cast<tiny_slip::Parity>(config.parity))) {
        EVLOG_AND_THROW(Everest::EverestConfigError(fmt::format("Cannot open serial port {}.", config.serial_port)));
    }
}

} // namespace module
