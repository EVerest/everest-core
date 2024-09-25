// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ev_slacImpl.hpp"

namespace module {
namespace ev {

using util::State;

void ev_slacImpl::init() {
}

void ev_slacImpl::ready() {
    publish_state(state_to_string(state));
}

void ev_slacImpl::handle_reset() {
    if (state != State::UNMATCHED) {
        state = State::UNMATCHED;
        publish_state(state_to_string(state));
        publish_dlink_ready(false);
    }
}

bool ev_slacImpl::handle_trigger_matching() {
    state = State::MATCHING;
    mod->cntmatching = 0;
    publish_state(state_to_string(state));
    return true;
}

State ev_slacImpl::get_state() const {
    return state;
}

void ev_slacImpl::set_state_matched() {
    state = State::MATCHED;
    publish_state(state_to_string(state));
    publish_dlink_ready(true);
}

} // namespace ev
} // namespace module
