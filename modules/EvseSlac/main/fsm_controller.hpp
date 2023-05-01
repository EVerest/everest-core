// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVSE_SLAC_FSM_CONTROLLER_HPP
#define EVSE_SLAC_FSM_CONTROLLER_HPP

#include "fsm.hpp"

#include <condition_variable>
#include <mutex>

class FSMController {
public:
    explicit FSMController(Context& ctx);

    void signal_new_slac_message(slac::messages::HomeplugMessage&);
    void signal_reset();
    bool signal_enter_bcd();
    bool signal_leave_bcd();
    void run();

private:
    bool signal_simple_event(Event ev);
    Context& ctx;
    FSM fsm;

    bool running{false};

    std::mutex feed_mtx;
    std::condition_variable new_event_cv;
    bool new_event{false};
};

#endif // EVSE_SLAC_FSM_CONTROLLER_HPP
