// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ev_board_supportImpl.hpp"

namespace module {
namespace ev_board_support {

void ev_board_supportImpl::init() {
}

void ev_board_supportImpl::ready() {
}

void ev_board_supportImpl::handle_enable(bool& value) {
    auto& module_state = mod->get_module_state();
    if (module_state.simulation_enabled && !value) {
        publish_bsp_event({types::board_support_common::Event::A});
        mod->clear_data();
    }
    module_state.simulation_enabled = value;
}

void ev_board_supportImpl::handle_set_cp_state(types::ev_board_support::EvCpState& cp_state) {
    using types::ev_board_support::EvCpState;
    auto& simdata_setting = mod->get_module_state().simdata_setting;

    switch (cp_state) {
    case EvCpState::A:
        simdata_setting.cp_voltage = 12.0;
        break;
    case EvCpState::B:
        simdata_setting.cp_voltage = 9.0;
        break;
    case EvCpState::C:
        simdata_setting.cp_voltage = 6.0;
        break;
    case EvCpState::D:
        simdata_setting.cp_voltage = 3.0;
        break;
    case EvCpState::E:
        simdata_setting.error_e = true;
        break;
    default:
        break;
    };
}

void ev_board_supportImpl::handle_allow_power_on(bool& value) {
    EVLOG_debug << "EV Power On: " << value;
}

void ev_board_supportImpl::handle_diode_fail(bool& value) {
    auto& simdata_setting = mod->get_module_state().simdata_setting;

    simdata_setting.diode_fail = value;
}

void ev_board_supportImpl::handle_set_ac_max_current(double& current) {
    auto& module_state = mod->get_module_state();

    module_state.ev_max_current = current;
}

void ev_board_supportImpl::handle_set_three_phases(bool& three_phases) {
    auto& module_state = mod->get_module_state();

    if (three_phases) {
        module_state.ev_three_phases = 3;
    } else {
        module_state.ev_three_phases = 1;
    }
}

void ev_board_supportImpl::handle_set_rcd_error(double& rcd_current_mA) {
    auto& simdata_setting = mod->get_module_state().simdata_setting;

    simdata_setting.rcd_current = rcd_current_mA;
}

} // namespace ev_board_support
} // namespace module
