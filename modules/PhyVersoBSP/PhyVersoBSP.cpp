// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "PhyVersoBSP.hpp"

namespace module {

void PhyVersoBSP::init() {
    // initialize serial driver
    if (!serial.open_device(config.serial_port.c_str(), config.baud_rate)) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestConfigError, "Could not open serial port ", config.serial_port,
                                    " with baud rate ", config.baud_rate));
        return;
    }

    invoke_init(*p_connector_1);
    invoke_init(*p_connector_2);
    invoke_init(*p_rcd_1);
    invoke_init(*p_rcd_2);
    invoke_init(*p_connector_lock_1);
    invoke_init(*p_connector_lock_2);
    invoke_init(*p_temperature);
    invoke_init(*p_system_specific_data_1);
    invoke_init(*p_system_specific_data_2);
}

void PhyVersoBSP::ready() {
    serial.run();

    /*
    FIXME will need to be implemented
    if (!serial.reset(config.reset_gpio)) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "Yeti reset not successful."));
    }

    serial.signalSpuriousReset.connect(
        [this]() { EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "Yeti uC spurious reset!")); });
    serial.signalConnectionTimeout.connect(
        [this]() { EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "Yeti UART timeout!")); });
        */

    invoke_ready(*p_connector_1);
    invoke_ready(*p_connector_2);
    invoke_ready(*p_rcd_1);
    invoke_ready(*p_rcd_2);
    invoke_ready(*p_connector_lock_1);
    invoke_ready(*p_connector_lock_2);
    invoke_ready(*p_temperature);
    invoke_ready(*p_system_specific_data_1);
    invoke_ready(*p_system_specific_data_2);
}

} // namespace module
