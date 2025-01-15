// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evse_board_supportImpl.hpp"
#include "util/state.hpp"

namespace module::board_support {

void evse_board_supportImpl::init() {
}

void evse_board_supportImpl::ready() {
    const auto default_capabilities = set_default_capabilities();
    publish_capabilities(default_capabilities);
}

void evse_board_supportImpl::handle_enable(bool& value) {
    auto& current_state = mod->get_module_state().current_state;
    if (value) {
        if (current_state == state::State::STATE_DISABLED) {
            current_state = state::State::STATE_A;
        }
    } else {
        current_state = state::State::STATE_DISABLED;
    }
}

void evse_board_supportImpl::handle_pwm_on(double& value) {
    const auto dutycycle = value / 100.0;
    pwm_on(dutycycle);
}

void evse_board_supportImpl::handle_pwm_off() {
    pwm_off();
}

void evse_board_supportImpl::handle_pwm_F() {
    pwm_f();
}

void evse_board_supportImpl::handle_allow_power_on(types::evse_board_support::PowerOnOff& value) {
    auto& module_state = mod->get_module_state();

    module_state.power_on_allowed = value.allow_power_on;
}

void evse_board_supportImpl::handle_ac_switch_three_phases_while_charging(bool& value) {
    auto& module_state = mod->get_module_state();

    module_state.use_three_phases = value;
    module_state.use_three_phases_confirmed = value;
}

void evse_board_supportImpl::handle_evse_replug(int& _) {
    EVLOG_error << "Replugging not supported";
}

types::board_support_common::ProximityPilot evse_board_supportImpl::handle_ac_read_pp_ampacity() {
    using types::board_support_common::Ampacity;
    const auto pp_resistor = mod->get_module_state().simulation_data.pp_resistor;

    if (pp_resistor < 80 or pp_resistor > 2460) {
        EVLOG_error << "PP resistor value " << pp_resistor << " Ohm seems to be outside the allowed range.";
        return {.ampacity = Ampacity::None};
    }
    if (pp_resistor > 936 && pp_resistor <= 2460) {
        return {.ampacity = Ampacity::A_13};
    }
    if (pp_resistor > 308 && pp_resistor <= 936) {
        return {.ampacity = Ampacity::A_20};
    }
    if (pp_resistor > 140 && pp_resistor <= 308) {
        return {.ampacity = Ampacity::A_32};
    }
    if (pp_resistor > 80 && pp_resistor <= 140) {
        return {.ampacity = Ampacity::A_63_3ph_70_1ph};
    }
    return {.ampacity = Ampacity::None};
}

void evse_board_supportImpl::handle_ac_set_overcurrent_limit_A(double& value) {
    // TODO: intentional?
}

void evse_board_supportImpl::pwm_on(const double dutycycle) const {
    auto& module_state = mod->get_module_state();

    if (dutycycle > 0.0) {
        module_state.pwm_duty_cycle = dutycycle;
        module_state.pwm_running = true;
        module_state.pwm_error_f = false;
    } else {
        pwm_off();
    }
}

void evse_board_supportImpl::pwm_off() const {
    auto& module_state = mod->get_module_state();

    module_state.pwm_duty_cycle = 1.0;
    module_state.pwm_running = false;
    module_state.pwm_error_f = false;
}

void evse_board_supportImpl::pwm_f() const {
    auto& module_state = mod->get_module_state();

    module_state.pwm_duty_cycle = 1.0;
    module_state.pwm_running = false;
    module_state.pwm_error_f = true;
}

types::evse_board_support::HardwareCapabilities set_default_capabilities() {
    return {.max_current_A_import = 32.0,
            .min_current_A_import = 6.0,
            .max_phase_count_import = 3,
            .min_phase_count_import = 1,
            .max_current_A_export = 16.0,
            .min_current_A_export = 0.0,
            .max_phase_count_export = 3,
            .min_phase_count_export = 1,
            .supports_changing_phases_during_charging = true,
            .connector_type = types::evse_board_support::Connector_type::IEC62196Type2Cable};
}
} // namespace module::board_support