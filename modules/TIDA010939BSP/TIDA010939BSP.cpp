// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "TIDA010939BSP.hpp"
#include <fmt/core.h>
#include <utils/date.hpp>

namespace module {

void TIDA010939BSP::init() {

    // initialize serial driver
    if (!serial.openDevice(config.serial_port.c_str(), config.baud_rate)) {
        EVLOG_error << "Could not open serial port " << config.serial_port << " with baud rate " << config.baud_rate;
        return;
    }

    invoke_init(*p_board_support);
    invoke_init(*p_connector_lock);
    invoke_init(*p_rcd);
}

void TIDA010939BSP::ready() {
    serial.run();

    if (!serial.reset(config.reset_gpio_chip, config.reset_gpio)) {
        EVLOG_error << "TIDA010939 reset not successful.";
    }

    serial.signalSpuriousReset.connect([this]() { EVLOG_error << "TIDA010939 uC spurious reset!"; });
    serial.signalConnectionTimeout.connect([this]() { EVLOG_error << "TIDA010939 UART timeout!"; });

    invoke_ready(*p_board_support);
    invoke_ready(*p_connector_lock);
    invoke_ready(*p_rcd);

    serial.signalErrorFlags.connect([this](ErrorFlags e) { error_handling(e); });

    if (not serial.is_open()) {
        auto err = p_board_support->error_factory->create_error("evse_board_support/CommunicationFault", "",
                                                                "Could not open serial port.");
        p_board_support->raise_error(err);
    }
}

void TIDA010939BSP::clear_errors_on_unplug() {
    if (error_MREC2GroundFailure) {
        p_board_support->clear_error("evse_board_support/MREC2GroundFailure");
    }
    error_MREC2GroundFailure = false;

    if (error_MREC1ConnectorLockFailure) {
        p_connector_lock->clear_error("connector_lock/MREC1ConnectorLockFailure");
    }
    error_MREC1ConnectorLockFailure = false;
}

void TIDA010939BSP::error_handling(ErrorFlags e) {

    if (e.diode_fault and not last_error_flags.diode_fault) {
        Everest::error::Error error_object = p_board_support->error_factory->create_error(
            "evse_board_support/DiodeFault", "", "Diode Fault", Everest::error::Severity::High);
        p_board_support->raise_error(error_object);
    } else if (not e.diode_fault and last_error_flags.diode_fault) {
        p_board_support->clear_error("evse_board_support/DiodeFault");
    }

    if (e.ventilation_not_available and not last_error_flags.ventilation_not_available) {
        Everest::error::Error error_object =
            p_board_support->error_factory->create_error("evse_board_support/VentilationNotAvailable", "",
                                                         "State D is not supported", Everest::error::Severity::High);
        p_board_support->raise_error(error_object);
    } else if (not e.ventilation_not_available and last_error_flags.ventilation_not_available) {
        p_board_support->clear_error("evse_board_support/VentilationNotAvailable");
    }

    if (e.connector_lock_failed and not last_error_flags.connector_lock_failed) {
        Everest::error::Error error_object = p_connector_lock->error_factory->create_error(
            "connector_lock/MREC1ConnectorLockFailure", "", "Lock motor failure", Everest::error::Severity::High);

        error_MREC1ConnectorLockFailure = true;
        p_connector_lock->raise_error(error_object);
    }

    if (e.cp_signal_fault and not last_error_flags.cp_signal_fault) {
        Everest::error::Error error_object = p_board_support->error_factory->create_error(
            "evse_board_support/MREC14PilotFault", "", "CP error", Everest::error::Severity::High);
        p_board_support->raise_error(error_object);
    } else if (not e.cp_signal_fault and last_error_flags.cp_signal_fault) {
        p_board_support->clear_error("evse_board_support/MREC14PilotFault");
    }

    last_error_flags = e;
}

} // namespace module
