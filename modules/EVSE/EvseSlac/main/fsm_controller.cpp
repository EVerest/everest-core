// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include "fsm_controller.hpp"

#include <slac/fsm/evse/states/others.hpp>

FSMController::FSMController(slac::fsm::evse::Context& context) : ctx(context){};

void FSMController::signal_new_slac_message(slac::messages::HomeplugMessage& msg) {
    if (running == false) {
        return;
    }
    {
        const std::lock_guard<std::mutex> feed_lck(feed_mtx);
        ctx.slac_message_payload = msg;
        fsm.handle_event(slac::fsm::evse::Event::SLAC_MESSAGE);
    }

    new_event = true;
    new_event_cv.notify_all();
}

void FSMController::signal_reset() {
    signal_simple_event(slac::fsm::evse::Event::RESET);
}

bool FSMController::signal_enter_bcd() {
    return signal_simple_event(slac::fsm::evse::Event::ENTER_BCD);
}

bool FSMController::signal_leave_bcd() {
    return signal_simple_event(slac::fsm::evse::Event::LEAVE_BCD);
}

bool FSMController::signal_simple_event(slac::fsm::evse::Event ev) {
    const std::lock_guard<std::mutex> feed_lck(feed_mtx);
    auto event_result = fsm.handle_event(ev);

    new_event = true;
    new_event_cv.notify_all();

    return event_result == fsm::HandleEventResult::SUCCESS;
}

void FSMController::run() {
    ctx.log_info("Starting the SLAC state machine");

    fsm.reset<slac::fsm::evse::InitState>(ctx);

    std::unique_lock<std::mutex> feed_lck(feed_mtx);

    running = true;

    while (true) {
        auto feed_result = fsm.feed();

        if (feed_result.transition()) {
            // call immediately again
            continue;
        } else if (feed_result.internal_error() || feed_result.unhandled_event()) {
            // FIXME (aw): would need to log here!
        } else if (feed_result.has_value() == true) {
            const auto timeout = *feed_result;
            if (timeout == 0) {
                // call feed directly again
                continue;
            }
            new_event_cv.wait_for(feed_lck, std::chrono::milliseconds(timeout), [this] { return new_event; });
        } else {
            // nothing happened, no return value -> wait for new event
            new_event_cv.wait(feed_lck, [this] { return new_event; });
        }

        if (new_event) {
            // we got a new event, reset it and let run feed again
            new_event = false;
        }
    }
}
