// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "ev_board_supportImpl.hpp"

namespace module {
namespace simple_board_support {

void ev_board_supportImpl::init() {
}

void ev_board_supportImpl::ready() {
}

void ev_board_supportImpl::handle_enable(bool& value) {
    // your code for cmd enable goes here
}

void ev_board_supportImpl::handle_set_cp_state(types::ev_board_support::EvCpState& cp_state) {
    CpState state = everest_cp_state_to_mcu_cp_state(cp_state);

    if (!mod->serial.set_cp_state(state))
        EVLOG_error << "Could not send set_cp_state packet";
    //  else
    //     EVLOG_info << "Sent set_cp_state packet with state " << ev_cp_state_to_string(cp_state);
}

void ev_board_supportImpl::handle_allow_power_on(bool& value) {
    EVLOG_info << "handle_allow_power_on not implemented";
}

void ev_board_supportImpl::handle_diode_fail(bool& value) {
    // your code for cmd diode_fail goes here
}

void ev_board_supportImpl::handle_set_ac_max_current(double& current) {
    EVLOG_info << "handle_set_ac_max_current not implemented";
}

void ev_board_supportImpl::handle_set_three_phases(bool& three_phases) {
    EVLOG_info << "handle_set_three_phases not implemented";
}

void ev_board_supportImpl::handle_set_rcd_error(double& rcd_current_mA) {
    EVLOG_info << "handle_set_rcd_error not implemented";
}

} // namespace simple_board_support
} // namespace module
