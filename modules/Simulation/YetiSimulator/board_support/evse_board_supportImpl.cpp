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
            false,                                                         // supports_cp_state_E
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
        } else {
            mod->module_state->republish_state = true;
        }
    } else {
        current_state = state::State::STATE_DISABLED;
    }
}

void evse_board_supportImpl::handle_pwm_on(double& value) {
    const auto dutycycle = value / 100.0;
    mod->pwm_on(dutycycle);
}

void evse_board_supportImpl::handle_cp_state_X1() {
    mod->cp_state_x1();
}

void evse_board_supportImpl::handle_cp_state_F() {
    mod->cp_state_f();
}

void evse_board_supportImpl::handle_cp_state_E() {
    EVLOG_warning << "Command cp_state_E is not supported. Ignoring command.";
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

void evse_board_supportImpl::handle_ac_set_overcurrent_limit_A(double& value) {
}

} // namespace module::board_support
