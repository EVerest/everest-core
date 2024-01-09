// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evse_board_supportImpl.hpp"

namespace module {
namespace connector_1 {

void evse_board_supportImpl::init() {
    {
        std::scoped_lock lock(caps_mutex);

        caps.min_current_A_import = mod->config.caps_min_current_A;
        caps.max_current_A_import = 16;
        caps.min_phase_count_import = 1;
        caps.max_phase_count_import = 3;
        caps.supports_changing_phases_during_charging = false;

        caps.min_current_A_export = mod->config.caps_min_current_A;
        caps.max_current_A_export = 6;
        caps.min_phase_count_export = 1;
        caps.max_phase_count_export = 3;
        caps.supports_changing_phases_during_charging = false;
    }

    mod->serial.signal_cp_state.connect([this](int connector, CpState s) {
        if (connector == 1 and s not_eq last_cp_state) {
            publish_event(to_bsp_event(s));
            EVLOG_info << "[1] CP State changed: " << to_bsp_event(s);
            last_cp_state = s;
        }
    });
    mod->serial.signal_relais_state.connect([this](int connector, bool s) {
        if (connector == 1) {
            EVLOG_info << "[1] Relais: " << (s ? "ON" : "OFF");
            publish_event(to_bsp_event(s));
        }
    });

    mod->serial.signal_telemetry.connect([this](int connector, Telemetry t) {
        if (connector == 1) {
            EVLOG_info << "[1] CP Voltage: " << t.cp_voltage_hi << " " << t.cp_voltage_lo;
        }
    });
}

void evse_board_supportImpl::ready() {
}

void evse_board_supportImpl::handle_setup(bool& three_phases, bool& has_ventilation, std::string& country_code) {
    // your code for cmd setup goes here
}

types::evse_board_support::HardwareCapabilities evse_board_supportImpl::handle_get_hw_capabilities() {
    std::scoped_lock lock(caps_mutex);
    return caps;
}

void evse_board_supportImpl::handle_enable(bool& value) {
    if (value) {
        mod->serial.set_pwm(1, 10000);
    } else {
        mod->serial.set_pwm(1, 0);
    }
}

void evse_board_supportImpl::handle_pwm_on(double& value) {
    if (value >= 0 && value <= 100.) {
        mod->serial.set_pwm(1, value * 100);
    }
}

void evse_board_supportImpl::handle_pwm_off() {
    mod->serial.set_pwm(1, 10000);
}

void evse_board_supportImpl::handle_pwm_F() {
    mod->serial.set_pwm(1, 0);
}

void evse_board_supportImpl::handle_allow_power_on(types::evse_board_support::PowerOnOff& value) {
    mod->serial.allow_power_on(1, value.allow_power_on);
}

void evse_board_supportImpl::handle_ac_switch_three_phases_while_charging(bool& value) {
    // your code for cmd ac_switch_three_phases_while_charging goes here
}

void evse_board_supportImpl::handle_evse_replug(int& value) {
    // your code for cmd evse_replug goes here
}

types::board_support_common::ProximityPilot evse_board_supportImpl::handle_ac_read_pp_ampacity() {
    // IMPLEMENT ME
    return {types::board_support_common::Ampacity::A_32};
}

void evse_board_supportImpl::handle_ac_set_overcurrent_limit_A(double& value) {
    // your code for cmd ac_set_overcurrent_limit_A goes here
}

} // namespace connector_1
} // namespace module
