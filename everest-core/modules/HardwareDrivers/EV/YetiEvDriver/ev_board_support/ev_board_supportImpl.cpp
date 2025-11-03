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
    // your code for cmd enable goes here
}

void ev_board_supportImpl::handle_set_cp_state(types::ev_board_support::EvCpState& cp_state) {
    // your code for cmd set_cp_state goes here
    switch (cp_state) {
    case types::ev_board_support::EvCpState::A:
        mod->serial.setBCDE(0);
        break;
    case types::ev_board_support::EvCpState::B:
        mod->serial.setBCDE(1);
        break;
    case types::ev_board_support::EvCpState::C:
        mod->serial.setBCDE(2);
        break;
    case types::ev_board_support::EvCpState::D:
        mod->serial.setBCDE(3);
        break;
    case types::ev_board_support::EvCpState::E:
        mod->serial.setBCDE(4);
        break;
    }
}

void ev_board_supportImpl::handle_allow_power_on(bool& value) {
    mod->serial.allowPowerOn(value);
}

void ev_board_supportImpl::handle_diode_fail(bool& value) {
    // your code for cmd diode_fail goes here
}

void ev_board_supportImpl::handle_set_ac_max_current(double& current) {
    // your code for cmd set_ac_max_current goes here
}

void ev_board_supportImpl::handle_set_three_phases(bool& three_phases) {
    // your code for cmd set_three_phases goes here
}

void ev_board_supportImpl::handle_set_rcd_error(double& rcd_current_mA) {
    // your code for cmd set_rcd_error goes here
}

} // namespace ev_board_support
} // namespace module
