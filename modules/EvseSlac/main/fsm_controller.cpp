// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include "fsm_controller.hpp"

#include "states/others.hpp"

FSMController::FSMController(Context& context) : ctx(context){};

void FSMController::signal_new_slac_message(slac::messages::HomeplugMessage& msg) {
    if (running == false) {
        return;
    }
    {
        const std::lock_guard<std::mutex> feed_lck(feed_mtx);
        ctx.slac_message_payload = msg;
        feed_result = fsm.feed_event(Event::SLAC_MESSAGE);
    }

    new_event = true;
    new_event_cv.notify_all();
}

void FSMController::signal_reset() {
    signal_simple_event(Event::RESET);
}

bool FSMController::signal_enter_bcd() {
    return (signal_simple_event(Event::ENTER_BCD) != fsm::EVENT_UNHANDLED);
}

bool FSMController::signal_leave_bcd() {
    return (signal_simple_event(Event::LEAVE_BCD) != fsm::EVENT_UNHANDLED);
}

int FSMController::signal_simple_event(Event ev) {
    int feed_result_copy;

    {
        const std::lock_guard<std::mutex> feed_lck(feed_mtx);
        feed_result = fsm.feed_event(ev);
        feed_result_copy = feed_result;
    }

    new_event = true;
    new_event_cv.notify_all();

    return feed_result_copy;
}

void FSMController::run() {
    ctx.log_info("Starting the SLAC state machine");

    feed_result = fsm.reset<ResetState>(ctx);

    std::unique_lock<std::mutex> feed_lck(feed_mtx);

    running = true;

    while (true) {
        if (feed_result == 0 || feed_result == fsm::EVENT_HANDLED_INTERNALLY) {
            // call immediately again
        } else if (feed_result == fsm::EVENT_UNHANDLED) {
            // FIXME (aw): what to do here?
            // ENABLE_ME DEBUG
        } else if (feed_result == fsm::DO_NOT_CALL_ME_AGAIN) {
            new_event_cv.wait(feed_lck, [this] { return new_event; });
        } else {
            new_event_cv.wait_for(feed_lck, std::chrono::milliseconds(feed_result), [this] { return new_event; });
        }

        if (new_event) {
            // we got an event and the feed result was just set
            new_event = false;
            continue;
        }

        // no new event, so either timeout or all other conditions to call feed again
        feed_result = fsm.feed();
    }
}
