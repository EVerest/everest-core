// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "YetiEvDriver.hpp"

namespace module {

void YetiEvDriver::init() {

    // initialize serial driver
    if (!serial.openDevice(config.serial_port.c_str(), config.baud_rate)) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestConfigError, "Could not open serial port ", config.serial_port,
                                    " with baud rate ", config.baud_rate));
        return;
    }

    invoke_init(*p_ev_board_support);
}

void YetiEvDriver::ready() {
    serial.run();

    if (!serial.reset(config.reset_gpio)) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "EVYeti reset not successful."));
    }

    serial.signalSpuriousReset.connect(
        [this]() { EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "EVYeti uC spurious reset!")); });
    serial.signalConnectionTimeout.connect(
        [this]() { EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "EVYeti UART timeout!")); });

    serial.signalEvent.connect([this](Event e) {
        types::board_support_common::BspEvent bspe;
        bspe = types::board_support_common::BspEvent::Disconnected;
        switch (e.type) {
        case Event_InterfaceEvent::Event_InterfaceEvent_A:
            bspe = types::board_support_common::BspEvent::A;
            EVLOG_info << "BspEvent::A";
            break;
        case Event_InterfaceEvent::Event_InterfaceEvent_B:
            EVLOG_info << "BspEvent::B";
            bspe = types::board_support_common::BspEvent::B;
            break;
        case Event_InterfaceEvent::Event_InterfaceEvent_C:
            EVLOG_info << "BspEvent::C";
            bspe = types::board_support_common::BspEvent::C;
            break;
        case Event_InterfaceEvent::Event_InterfaceEvent_D:
            EVLOG_info << "BspEvent::D";
            bspe = types::board_support_common::BspEvent::D;
            break;
        case Event_InterfaceEvent::Event_InterfaceEvent_DISCONNECTED:
            EVLOG_info << "BspEvent::Disconnected";
            bspe = types::board_support_common::BspEvent::Disconnected;
            break;
        }

        p_ev_board_support->publish_bsp_event(bspe);

        types::board_support_common::BspMeasurement bspm;
        bspm.cp_pwm_duty_cycle = 5;

        p_ev_board_support->publish_bsp_measurement(bspm);

        // FIXME
        // publish event
        // FIXME in uC: only send events when they actually change
    });

    invoke_ready(*p_ev_board_support);
}

} // namespace module
