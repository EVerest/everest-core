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

        bspe.event = types::board_support_common::Event::Disconnected;
        switch (e.type) {
        case Event_InterfaceEvent::Event_InterfaceEvent_A:
            bspe.event = types::board_support_common::Event::A;
            EVLOG_info << "BspEvent::A";
            break;
        case Event_InterfaceEvent::Event_InterfaceEvent_B:
            EVLOG_info << "BspEvent::B";
            bspe.event = types::board_support_common::Event::B;
            break;
        case Event_InterfaceEvent::Event_InterfaceEvent_C:
            EVLOG_info << "BspEvent::C";
            bspe.event = types::board_support_common::Event::C;
            break;
        case Event_InterfaceEvent::Event_InterfaceEvent_D:
            EVLOG_info << "BspEvent::D";
            bspe.event = types::board_support_common::Event::D;
            break;
        case Event_InterfaceEvent::Event_InterfaceEvent_DISCONNECTED:
            EVLOG_info << "BspEvent::Disconnected";
            bspe.event = types::board_support_common::Event::Disconnected;
            break;
        }

        p_ev_board_support->publish_bsp_event(bspe);
    });

    serial.signalMeasurements.connect([this](Measurements m) {
        types::board_support_common::BspMeasurement bspm;
        bspm.cp_pwm_duty_cycle = m.pwmDutyCycle * 100.;
        // FIXME(cc): This is not implemented on MCU side yet
        bspm.proximity_pilot = {types::board_support_common::Ampacity::None};
        p_ev_board_support->publish_bsp_measurement(bspm);
    });

    invoke_ready(*p_ev_board_support);
}

} // namespace module
