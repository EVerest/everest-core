// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "YetiDriver.hpp"
#include <fmt/core.h>
#include <utils/date.hpp>

namespace module {

void YetiDriver::init() {

    // initialize serial driver
    if (!serial.openDevice(config.serial_port.c_str(), config.baud_rate)) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestConfigError, "Could not open serial port ", config.serial_port,
                                    " with baud rate ", config.baud_rate));
        return;
    }

    telemetry_power_path_controller_version = {{"timestamp", ""},
                                               {"type", "power_path_controller_version"},
                                               {"hardware_version", 1},
                                               {"software_version", "1.00"},
                                               {"date_manufactured", "N/A"},
                                               {"operating_time_h", 5},
                                               {"operating_time_h_warning", 5000},
                                               {"operating_time_h_error", 6000},
                                               {"software_version", "1.00"},
                                               {"error", false}};

    telemetry_power_path_controller = {{"timestamp", ""},
                                       {"type", "power_path_controller"},
                                       {"cp_voltage_high", 0.0},
                                       {"cp_voltage_low", 0.0},
                                       {"cp_pwm_duty_cycle", 0.0},
                                       {"cp_state", "A1"},
                                       {"pp_ohm", 0.0},
                                       {"supply_voltage_12V", 0.0},
                                       {"supply_voltage_minus_12V", 0.0},
                                       {"temperature_controller", 0.0},
                                       {"temperature_car_connector", 0.0},
                                       {"watchdog_reset_count", 0.0},
                                       {"error", false}};

    telemetry_power_switch = {{"timestamp", ""},
                              {"type", "power_switch"},
                              {"switching_count", 0},
                              {"switching_count_warning", 30000},
                              {"switching_count_error", 50000},
                              {"is_on", false},
                              {"time_to_switch_on_ms", 100},
                              {"time_to_switch_off_ms", 100},
                              {"temperature_C", 0.0},
                              {"error", false},
                              {"error_over_current", false}};

    telemetry_rcd = {{"timestamp", ""},    //
                     {"type", "rcd"},      //
                     {"enabled", true},    //
                     {"current_mA", 0.0},  //
                     {"triggered", false}, //
                     {"error", false}};    //

    invoke_init(*p_powermeter);
    invoke_init(*p_board_support);
    invoke_init(*p_connector_lock);
    invoke_init(*p_rcd);
}

void YetiDriver::ready() {
    serial.run();

    if (!serial.reset(config.reset_gpio_chip, config.reset_gpio)) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "Yeti reset not successful."));
    }

    serial.signalSpuriousReset.connect(
        [this]() { EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "Yeti uC spurious reset!")); });
    serial.signalConnectionTimeout.connect(
        [this]() { EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "Yeti UART timeout!")); });

    invoke_ready(*p_powermeter);
    invoke_ready(*p_board_support);
    invoke_ready(*p_connector_lock);
    invoke_ready(*p_rcd);

    telemetryThreadHandle = std::thread([this]() {
        while (!telemetryThreadHandle.shouldExit()) {
            sleep(10);
            {
                std::scoped_lock lock(telemetry_mutex);
                publish_external_telemetry_livedata("power_path_controller", telemetry_power_path_controller);
                publish_external_telemetry_livedata("rcd", telemetry_rcd);
                publish_external_telemetry_livedata("power_path_controller_version",
                                                    telemetry_power_path_controller_version);
            }
        }
    });

    serial.signalErrorFlags.connect([this](ErrorFlags e) { error_handling(e); });
}

void YetiDriver::publish_external_telemetry_livedata(const std::string& topic, const Everest::TelemetryMap& data) {
    if (info.telemetry_enabled) {
        telemetry.publish("livedata", topic, data);
    }
}

bool rcd_selftest_failed;

bool connector_lock_failed;
bool cp_signal_fault;

void YetiDriver::clear_errors_on_unplug() {
    if (error_MREC2GroundFailure) {
        p_board_support->request_clear_all_evse_board_support_MREC2GroundFailure();
    }
    error_MREC2GroundFailure = false;

    if (error_MREC1ConnectorLockFailure) {
        p_connector_lock->request_clear_all_connector_lock_MREC1ConnectorLockFailure();
    }
    error_MREC1ConnectorLockFailure = false;
}

void YetiDriver::error_handling(ErrorFlags e) {

    if (e.diode_fault and not last_error_flags.diode_fault) {
        p_board_support->raise_evse_board_support_DiodeFault("Diode Fault", Everest::error::Severity::High);
    } else if (not e.diode_fault and last_error_flags.diode_fault) {
        p_board_support->request_clear_all_evse_board_support_DiodeFault();
    }

    if (e.rcd_triggered and not last_error_flags.rcd_triggered) {
        p_board_support->raise_evse_board_support_MREC2GroundFailure("Onboard RCD triggered",
                                                                     Everest::error::Severity::High);
        error_MREC2GroundFailure = true;
    }

    if (e.ventilation_not_available and not last_error_flags.ventilation_not_available) {
        p_board_support->raise_evse_board_support_VentilationNotAvailable("State D is not supported",
                                                                          Everest::error::Severity::High);
    } else if (not e.ventilation_not_available and last_error_flags.ventilation_not_available) {
        p_board_support->request_clear_all_evse_board_support_VentilationNotAvailable();
    }

    if (e.connector_lock_failed and not last_error_flags.connector_lock_failed) {
        p_connector_lock->raise_connector_lock_MREC1ConnectorLockFailure("Lock motor failure",
                                                                         Everest::error::Severity::High);

        error_MREC1ConnectorLockFailure = true;
    }

    if (e.cp_signal_fault and not last_error_flags.cp_signal_fault) {
        p_board_support->raise_evse_board_support_MREC14PilotFault("CP error", Everest::error::Severity::High);
    } else if (not e.cp_signal_fault and last_error_flags.cp_signal_fault) {
        p_board_support->request_clear_all_evse_board_support_MREC14PilotFault();
    }

    last_error_flags = e;
}

} // namespace module
