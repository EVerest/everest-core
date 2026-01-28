// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evse_board_supportImpl.hpp"

namespace module {
namespace connector_2 {

void evse_board_supportImpl::init() {
    {
        std::scoped_lock lock(caps_mutex);

        caps.min_current_A_import = mod->config.conn2_min_current_A_import;
        caps.max_current_A_import = mod->config.conn2_max_current_A_import;
        caps.min_phase_count_import = mod->config.conn2_min_phase_count_import;
        caps.max_phase_count_import = mod->config.conn2_max_phase_count_import;
        caps.supports_changing_phases_during_charging = false;
        caps.supports_cp_state_E = false;

        caps.min_current_A_export = mod->config.conn2_min_current_A_export;
        caps.max_current_A_export = mod->config.conn2_max_current_A_export;
        caps.min_phase_count_export = mod->config.conn2_min_phase_count_export;
        caps.max_phase_count_export = mod->config.conn2_max_phase_count_export;

        if (mod->config.conn2_has_socket) {
            caps.connector_type = types::evse_board_support::Connector_type::IEC62196Type2Socket;
        } else {
            caps.connector_type = types::evse_board_support::Connector_type::IEC62196Type2Cable;
        }
    }

    mod->serial.signal_cp_state.connect([this](int connector, CpState s) {
        if (connector == 2 && s != last_cp_state) {
            publish_event(to_bsp_event(s));
            EVLOG_info << "[2] CP State changed: " << to_bsp_event(s);
            last_cp_state = s;
        }
    });
    mod->serial.signal_set_coil_state_response.connect([this](int connector, CoilState s) {
        if (connector == 2) {
            EVLOG_info << "[2] Relais: " << (s.coil_state ? "ON" : "OFF");
            publish_event(to_bsp_event(s));
        }
    });

    mod->serial.signal_telemetry.connect([this](int connector, Telemetry t) {
        if (connector == 2) {
            EVLOG_info << "[2] CP Voltage: " << t.cp_voltage_hi << " " << t.cp_voltage_lo;
        }
    });

    mod->serial.signal_pp_state.connect([this](int connector, PpState s) {
        if (connector == 2) {
            if (last_pp_state != s) {
                EVLOG_info << "[2] PpState " << s;
                publish_ac_pp_ampacity(to_pp_ampacity(s));
            }
            last_pp_state = s;
        }
    });

    mod->gpio.signal_stop_button_state.connect([this](int connector, bool state) {
        if (connector == 2 && (state != last_stop_button_state)) {
            types::evse_manager::StopTransactionRequest request;
            request.reason = types::evse_manager::StopTransactionReason::Local;
            this->publish_request_stop_transaction(request);
            EVLOG_info << "[2] Request stop button state: " << (state ? "PUSHED" : "RELEASED");
            last_stop_button_state = state;
        }
    });

    mod->serial.signal_error_flags.connect([this](int connector, ErrorFlags error_flags) {
        if (connector == 2) {
            // Contactor feedback divergence
            if (error_flags.coil_feedback_diverges != last_error_flags.coil_feedback_diverges) {
                if (error_flags.coil_feedback_diverges) {
                    Everest::error::Error error_object = this->error_factory->create_error(
                        "evse_board_support/MREC17EVSEContactorFault", "",
                        "Port 2 contactor feedback diverges from target state", Everest::error::Severity::High);
                    this->raise_error(error_object);
                } else {
                    this->clear_error("evse_board_support/MREC17EVSEContactorFault");
                }
            }

            // Diode fault
            if (error_flags.diode_fault != last_error_flags.diode_fault) {
                if (error_flags.diode_fault) {
                    Everest::error::Error error_object = this->error_factory->create_error(
                        "evse_board_support/DiodeFault", "", "Port 2 diode fault", Everest::error::Severity::High);
                    this->raise_error(error_object);
                } else {
                    this->clear_error("evse_board_support/DiodeFault");
                }
            }

            // PP fault
            if (error_flags.pp_signal_fault != last_error_flags.pp_signal_fault) {
                if (error_flags.pp_signal_fault) {
                    Everest::error::Error error_object =
                        this->error_factory->create_error("evse_board_support/MREC23ProximityFault", "",
                                                          "Port 2 PP signal fault", Everest::error::Severity::High);
                    this->raise_error(error_object);
                } else {
                    this->clear_error("evse_board_support/MREC23ProximityFault");
                }
            }

            last_error_flags = error_flags;
        }
    });
}

void evse_board_supportImpl::ready() {
    {
        std::scoped_lock lock(caps_mutex);
        publish_capabilities(caps);
    }
}

void evse_board_supportImpl::handle_enable(bool& value) {
    if (value) {
        mod->serial.set_pwm(2, 10000);
    } else {
        mod->serial.set_pwm(2, 0);
    }
}

void evse_board_supportImpl::handle_pwm_on(double& value) {
    if (value >= 0 && value <= 100.) {
        mod->serial.set_pwm(2, value * 100);
    } else {
        EVLOG_warning << "Invalid pwm value " << value;
    }
}

void evse_board_supportImpl::handle_cp_state_X1() {
    mod->serial.set_pwm(2, 10000);
}

void evse_board_supportImpl::handle_cp_state_F() {
    mod->serial.set_pwm(2, 0);
}

void evse_board_supportImpl::handle_cp_state_E() {
    EVLOG_warning << "Command cp_state_E is not supported. Ignoring command.";
}

void evse_board_supportImpl::handle_allow_power_on(types::evse_board_support::PowerOnOff& value) {
    if (mod->config.conn2_disable_port) {
        EVLOG_error << "[2] Port disabled; Cannot set power_on!";
        return;
    }

    if (mod->config.conn2_dc) {
        mod->serial.set_coil_state_request(2, CoilType_COIL_DC1, value.allow_power_on);
    } else {
        mod->serial.set_coil_state_request(2, CoilType_COIL_AC, value.allow_power_on);
    }
}

void evse_board_supportImpl::handle_ac_switch_three_phases_while_charging(bool& value) {
    // your code for cmd ac_switch_three_phases_while_charging goes here
}

void evse_board_supportImpl::handle_evse_replug(int& value) {
    // your code for cmd evse_replug goes here
}

void evse_board_supportImpl::handle_ac_set_overcurrent_limit_A(double& value) {
    // your code for cmd ac_set_overcurrent_limit_A goes here
}

} // namespace connector_2
} // namespace module
