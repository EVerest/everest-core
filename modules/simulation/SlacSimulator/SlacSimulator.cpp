// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "SlacSimulator.hpp"

#include <chrono>
#include <mutex>

namespace module {

void SlacSimulator::init() {
    invoke_init(*p_evse);
    invoke_init(*p_ev);

    state_evse = STATE_UNMATCHED;
    state_ev = STATE_UNMATCHED;
    cntmatching = 0;

    // this->slac_simulator_thread_handle = std::thread(&SlacSimulator::slac_simulator_worker, this);
}

void SlacSimulator::ready() {
    invoke_ready(*p_evse);
    invoke_ready(*p_ev);

    p_ev->publish_state(state_to_string(state_ev));
    p_evse->publish_state(state_to_string(state_evse));
}

void SlacSimulator::set_unmatched_ev() {
    if (state_ev != STATE_UNMATCHED) {
        state_ev = STATE_UNMATCHED;
        p_ev->publish_state(state_to_string(state_ev));
        p_ev->publish_dlink_ready(false);
    }
}

void SlacSimulator::set_matching_ev() {
    state_ev = STATE_MATCHING;
    cntmatching = 0;
    p_ev->publish_state(state_to_string(state_ev));
}

void SlacSimulator::set_matched_ev() {
    state_ev = STATE_MATCHED;
    p_ev->publish_state(state_to_string(state_ev));
    p_ev->publish_dlink_ready(true);
}

void SlacSimulator::set_unmatched_evse() {
    state_evse = STATE_UNMATCHED;
    p_evse->publish_state(state_to_string(state_evse));
    p_evse->publish_dlink_ready(false);
}

void SlacSimulator::set_matching_evse() {
    state_evse = STATE_MATCHING;
    cntmatching = 0;
    p_evse->publish_state(state_to_string(state_evse));
}

void SlacSimulator::set_matched_evse() {
    state_evse = STATE_MATCHED;
    p_evse->publish_state(state_to_string(state_evse));
    p_evse->publish_dlink_ready(true);

}

void SlacSimulator::slac_simulator_worker(void) {
    while (true) {
        // if (this->slac_simulator_thread_handle.shouldExit()) {
        // break;
        // }

        // set interval for publishing
        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        // std::scoped_lock access_lock(this->slac_simulator_thread_handle);

        cntmatching += 1;
        if (state_ev == STATE_MATCHING && state_evse == STATE_MATCHING && cntmatching > 2 * 4) {
            set_matched_ev();
            set_matched_evse();
        }
    }
}

} // namespace module
