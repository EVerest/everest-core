// Copyright Pionix GmbH
#include "mMWcarBSP.hpp"

namespace module {

void mMWcarBSP::init() {
    // initialize serial driver
    if (!serial.open_device(config.serial_port.c_str(), config.baud_rate)) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestConfigError, "Could not open serial port ", config.serial_port,
                                    " with baud rate ", config.baud_rate));
        return;
    }

    invoke_init(*p_board_support_extended);
    invoke_init(*p_simple_board_support);
}

void mMWcarBSP::ready() {
    serial.run();

    invoke_ready(*p_board_support_extended);
    invoke_ready(*p_simple_board_support);
}

} // namespace module
