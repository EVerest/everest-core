// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "MicroMegaWattBSP.hpp"

namespace module {

void MicroMegaWattBSP::init() {
    // initialize serial driver
    if (!serial.openDevice(config.serial_port.c_str(), config.baud_rate)) {
        EVLOG_error << "Could not open serial port " << config.serial_port << " with baud rate " << config.baud_rate;
        return;
    }

    invoke_init(*p_powermeter);
    invoke_init(*p_board_support);
    invoke_init(*p_dc_supply);
}

void MicroMegaWattBSP::ready() {
    serial.run();

    if (!serial.reset(config.reset_gpio)) {
        EVLOG_error << "uMWC reset not successful.";
    }

    serial.signalSpuriousReset.connect([this]() { EVLOG_error << "uMWC uC spurious reset!"; });
    serial.signalConnectionTimeout.connect([this]() { EVLOG_error << "uMWC UART timeout!"; });

    serial.signalTelemetry.connect([this](Telemetry t) {
        mqtt.publish("everest_external/umwc/cp_hi", t.cp_hi);
        mqtt.publish("everest_external/umwc/cp_lo", t.cp_lo);
        mqtt.publish("everest_external/umwc/pwm_dc", t.pwm_dc);
        mqtt.publish("everest_external/umwc/relais_on", t.relais_on);
    });
    serial.signalPowerMeter.connect(
        [this](PowerMeter p) { mqtt.publish("everest_external/umwc/output_voltage", p.voltage); });

    invoke_ready(*p_powermeter);
    invoke_ready(*p_board_support);
    invoke_ready(*p_dc_supply);

    if (not serial.is_open()) {
        auto err = p_board_support->error_factory->create_error("evse_board_support/CommunicationFault", "",
                                                                "Could not open serial port.");
        p_board_support->raise_error(err);
    }
}

} // namespace module
