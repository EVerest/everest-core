// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ev_board_supportImpl.hpp"
#include <everest/logging.hpp>

namespace module::ev_board_support {

namespace {
constexpr auto CP_VOLTAGE_A = 12.0;
constexpr auto CP_VOLTAGE_B = 9.0;
constexpr auto CP_VOLTAGE_C = 6.0;
constexpr auto CP_VOLTAGE_D = 3.0;
} // namespace

void ev_board_supportImpl::init() {
}

void ev_board_supportImpl::ready() {
}

void ev_board_supportImpl::handle_enable(bool& value) {
    if (mod->module_state->simulation_enabled and not value) {
        publish_bsp_event({types::board_support_common::Event::A});
        mod->reset_module_state();
    }
    mod->module_state->simulation_enabled = value;
}

void ev_board_supportImpl::handle_set_cp_state(types::ev_board_support::EvCpState& cp_state) {
    using types::ev_board_support::EvCpState;
    auto& simdata_setting = mod->module_state->simdata_setting;

    switch (cp_state) {
    case EvCpState::A:
        simdata_setting.cp_voltage = CP_VOLTAGE_A;
        break;
    case EvCpState::B:
        simdata_setting.cp_voltage = CP_VOLTAGE_B;
        break;
    case EvCpState::C:
        simdata_setting.cp_voltage = CP_VOLTAGE_C;
        break;
    case EvCpState::D:
        simdata_setting.cp_voltage = CP_VOLTAGE_D;
        break;
    case EvCpState::E:
        simdata_setting.error_e = true;
        break;
    default:
        break;
    }
}

void ev_board_supportImpl::handle_allow_power_on(bool& value) {
    EVLOG_debug << "EV Power On: " << value;
}

void ev_board_supportImpl::handle_diode_fail(bool& value) {
    mod->module_state->simdata_setting.diode_fail = value;
}

void ev_board_supportImpl::handle_set_ac_max_current(double& current) {
    mod->module_state->ev_max_current = current;
}

void ev_board_supportImpl::handle_set_three_phases(bool& three_phases) {
    if (three_phases) {
        mod->module_state->ev_three_phases = 3;
    } else {
        mod->module_state->ev_three_phases = 1;
    }
}

void ev_board_supportImpl::handle_set_rcd_error(double& rcd_current_mA) {
    mod->module_state->simdata_setting.rcd_current = rcd_current_mA;
}

} // namespace module::ev_board_support
