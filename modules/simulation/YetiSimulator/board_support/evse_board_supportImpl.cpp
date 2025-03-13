// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evse_board_supportImpl.hpp"
#include "util/state.hpp"

namespace module::board_support {

namespace {
types::evse_board_support::HardwareCapabilities set_default_capabilities() {
    return {32.0,                                                          // max_current_A_import
            6.0,                                                           // min_current_A_import
            3,                                                             // max_phase_count_import
            1,                                                             // min_phase_count_import
            16.0,                                                          // max_current_A_export
            0.0,                                                           // min_current_A_export
            3,                                                             // max_phase_count_export
            1,                                                             // min_phase_count_export
            true,                                                          // supports_changing_phases_during_charging
            types::evse_board_support::Connector_type::IEC62196Type2Cable, // connector_type
            std::nullopt};                                                 // max_plug_temperature_C
}
} // namespace

void evse_board_supportImpl::init() {
}

void evse_board_supportImpl::ready() {
    const auto default_capabilities = set_default_capabilities();
    publish_capabilities(default_capabilities);
}

void evse_board_supportImpl::handle_enable(bool& value) {
    auto& current_state = mod->module_state->current_state;
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
    mod->pwm_on(dutycycle);
}

void evse_board_supportImpl::handle_pwm_off() {
    mod->pwm_off();
}

void evse_board_supportImpl::handle_pwm_F() {
    mod->pwm_f();
}

void evse_board_supportImpl::handle_allow_power_on(types::evse_board_support::PowerOnOff& value) {
    mod->module_state->power_on_allowed = value.allow_power_on;
}

void evse_board_supportImpl::handle_ac_switch_three_phases_while_charging(bool& value) {
    mod->module_state->use_three_phases = value;
    mod->module_state->use_three_phases_confirmed = value;
}

void evse_board_supportImpl::handle_evse_replug(int& _) {
    EVLOG_error << "Replugging not supported";
}

types::board_support_common::ProximityPilot evse_board_supportImpl::handle_ac_read_pp_ampacity() {
    using types::board_support_common::Ampacity;
    const auto pp_resistor = mod->module_state->simulation_data.pp_resistor;

    if (pp_resistor < 80 or pp_resistor > 2460) {
        EVLOG_error << "PP resistor value " << pp_resistor << " Ohm seems to be outside the allowed range.";
        return {Ampacity::None};
    }
    if (pp_resistor > 936 && pp_resistor <= 2460) {
        return {Ampacity::A_13};
    }
    if (pp_resistor > 308 && pp_resistor <= 936) {
        return {Ampacity::A_20};
    }
    if (pp_resistor > 140 && pp_resistor <= 308) {
        return {Ampacity::A_32};
    }
    if (pp_resistor > 80 && pp_resistor <= 140) {
        return {Ampacity::A_63_3ph_70_1ph};
    }
    return {Ampacity::None};
}

void evse_board_supportImpl::handle_ac_set_overcurrent_limit_A(double& value) {
}

} // namespace module::board_support