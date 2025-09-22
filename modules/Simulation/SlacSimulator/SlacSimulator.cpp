// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "SlacSimulator.hpp"
#include "ev/ev_slacImpl.hpp"
#include "evse/slacImpl.hpp"

namespace module {

using util::State;

void SlacSimulator::init() {
    invoke_init(*p_evse);
    invoke_init(*p_ev);
}

void SlacSimulator::ready() {
    invoke_ready(*p_evse);
    invoke_ready(*p_ev);

    std::thread(&SlacSimulator::run, this).detach();
}

void SlacSimulator::run() {
    auto& evse = dynamic_cast<evse::slacImpl&>(*p_evse);
    auto& ev = dynamic_cast<ev::ev_slacImpl&>(*p_ev);
    while (true) {
        cntmatching++;
        if (ev.get_state() == State::MATCHING && evse.get_state() == State::MATCHING && cntmatching > 2 * 4) {
            ev.set_state_matched();
            evse.set_state_matched();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(loop_interval_ms));
    }
};

} // namespace module
