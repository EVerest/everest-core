// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "slacImpl.hpp"

namespace module {
namespace evse {

using util::State;

void slacImpl::init() {
}

void slacImpl::ready() {
    publish_state(state_to_string(state));
}

void slacImpl::handle_reset(bool& enable) {
    set_state_to_unmatched();
}

void slacImpl::handle_enter_bcd() {
    set_state_to_matching();
}

void slacImpl::handle_leave_bcd() {
    set_state_to_unmatched();
}

void slacImpl::handle_dlink_terminate() {
    set_state_to_unmatched();
}

void slacImpl::handle_dlink_error() {
    set_state_to_unmatched();
}

void slacImpl::handle_dlink_pause() {
    // No action needed for D-LINK_PAUSE in simulation
}

void slacImpl::set_state_to_unmatched() {
    if (state != State::UNMATCHED) {
        state = State::UNMATCHED;
        publish_state(state_to_string(state));
        publish_dlink_ready(false);
    }
}

void slacImpl::set_state_to_matching() {
    state = State::MATCHING;
    mod->cntmatching = 0;
    publish_state(state_to_string(state));
}

State slacImpl::get_state() const {
    return state;
}

void slacImpl::set_state_matched() {
    state = State::MATCHED;
    publish_state(state_to_string(state));
    publish_dlink_ready(true);
};
} // namespace evse
} // namespace module
