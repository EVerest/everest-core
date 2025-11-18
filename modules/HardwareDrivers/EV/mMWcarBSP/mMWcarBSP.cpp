/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

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

    if (not config.reset_gpio_chip.empty()) {
        EVLOG_info << "Perform HW reset with gpio chip " << config.reset_gpio_chip << " line "
                   << config.reset_gpio_line;
        if (!serial.hard_reset(config.reset_gpio_chip, config.reset_gpio_line)) {
            EVLOG_error << "mMWcar reset not successful.";
        } else {
            EVLOG_info << "HW hard reset successful.";
        }
    }

    invoke_ready(*p_board_support_extended);
    invoke_ready(*p_simple_board_support);

    serial.signal_cp_pp_meas.connect([this](BasicChargingCommMeasInstant m) {
        types::board_support_common::BspMeasurement bspm;
        bspm.cp_pwm_duty_cycle = m.pwm_duty;
        // FIXME(joro): This is not implemented on MCU side yet
        bspm.proximity_pilot = {types::board_support_common::Ampacity::None};
        p_simple_board_support->publish_bsp_measurement(bspm);

        // DEBUG
        if (last_bspm.cp_pwm_duty_cycle != bspm.cp_pwm_duty_cycle) {
            EVLOG_info << bspm;
        }
        last_bspm = bspm;

        types::board_support_common::BspEvent bspe;
        bspe.event = types::board_support_common::Event::Disconnected;

        switch (m.cp_state) {
        case 0:
            bspe.event = types::board_support_common::Event::A;
            break;
        case 1:
            bspe.event = types::board_support_common::Event::B;
            break;
        case 2:
            bspe.event = types::board_support_common::Event::C;
            break;
        case 3:
            bspe.event = types::board_support_common::Event::D;
            break;
        case 4:
            bspe.event = types::board_support_common::Event::E;
            break;
        case 5:
            bspe.event = types::board_support_common::Event::F;
        case 6:
            EVLOG_info << "BspEvent::DF (NOT IMPLEMENTED)";
            // bspe.event = types::board_support_common::Event::DF;
        default:
            bspe.event = types::board_support_common::Event::Disconnected;
            break;
        }

        if (bspe.event != last_bsp_event.event) {
            EVLOG_info << "BspEvent::" << event_to_string(bspe.event);
        }

        last_bsp_event = bspe;

        p_simple_board_support->publish_bsp_event(bspe);
    });
}

} // namespace module
