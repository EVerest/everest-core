// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include "fsm_controller.hpp"

#include <everest/io/event/fd_event_handler.hpp>
#include <everest/slac/fsm/evse/states/others.hpp>
#include <everest/util/misc/bind.hpp>

FSMController::FSMController(slac::fsm::evse::Context& context) : ctx(context) {
};

void FSMController::init() {
    ctx.log_info("Starting the SLAC state machine");
    fsm.reset<slac::fsm::evse::InitState>(ctx);
    run();
}

void FSMController::signal_new_slac_message(slac::messages::HomeplugMessage const& msg) {
    ctx.slac_message_payload = msg;
    fsm.handle_event(slac::fsm::evse::Event::SLAC_MESSAGE);
    run();
}

void FSMController::signal_reset() {
    m_reset.notify();
}

bool FSMController::signal_enter_bcd() {
    m_enter_bcd.notify();
    return true;
}

bool FSMController::signal_leave_bcd() {
    m_leave_bcd.notify();
    return true;
}

void FSMController::handle_retrigger() {
    run();
    m_retrigger.set_timeout_ms(0);
}

void FSMController::handle_reset() {
    fsm.handle_event(slac::fsm::evse::Event::RESET);
    run();
}

void FSMController::handle_enter_bcd() {
    fsm.handle_event(slac::fsm::evse::Event::ENTER_BCD);
    run();
}

void FSMController::handle_leave_bcd() {
    fsm.handle_event(slac::fsm::evse::Event::LEAVE_BCD);
    run();
}

bool FSMController::register_events(everest::lib::io::event::fd_event_handler& handler) {
    using everest::lib::util::bind_obj;
    using T = FSMController;
    auto result = true;
    result &= handler.register_event_handler(&m_reset, bind_obj(&T::handle_reset, this));
    result &= handler.register_event_handler(&m_enter_bcd, bind_obj(&T::handle_enter_bcd, this));
    result &= handler.register_event_handler(&m_leave_bcd, bind_obj(&T::handle_leave_bcd, this));
    result &= handler.register_event_handler(&m_feed, bind_obj(&T::run, this));
    result &= handler.register_event_handler(&m_retrigger, bind_obj(&T::handle_retrigger, this));
    return result;
}

bool FSMController::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    auto result = true;
    result &= handler.unregister_event_handler(&m_reset);
    result &= handler.unregister_event_handler(&m_enter_bcd);
    result &= handler.unregister_event_handler(&m_leave_bcd);
    result &= handler.unregister_event_handler(&m_feed);
    result &= handler.unregister_event_handler(&m_retrigger);
    return result;
}

void FSMController::run() {
    using namespace std::chrono_literals;
    auto feed_result = fsm.feed();
    if (feed_result.transition()) {
        m_feed.notify();
    } else if (feed_result.internal_error() || feed_result.unhandled_event()) {
        return;
    } else if (feed_result.has_value() == true) {
        const auto timeout = *feed_result;
        if (timeout == 0) {
            m_feed.notify();
            return;
        }
        m_retrigger.set_timeout_ms(timeout);
    }
}
