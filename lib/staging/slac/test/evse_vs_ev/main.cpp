// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include <array>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <thread>

#include <poll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <slac/fsm/ev/fsm.hpp>
#include <slac/fsm/ev/states/others.hpp>
#include <slac/fsm/evse/fsm.hpp>
#include <slac/fsm/evse/states/others.hpp>

#include "plc_emu.hpp"
#include "socket_pair_bridge.hpp"

class EvseFsmController {
public:
    explicit EvseFsmController(int evse_fd);

    void trigger_enter_bcd();

    void run();

private:
    static constexpr uint64_t ENTER_BCD_EVENT_CODE = 1;
    slac::fsm::evse::ContextCallbacks callbacks;
    slac::fsm::evse::Context ctx{callbacks};
    slac::fsm::evse::FSM fsm;

    std::array<struct pollfd, 2> pollfds;
};

EvseFsmController::EvseFsmController(int evse_fd) :
    pollfds({{
        {evse_fd, POLLIN, 0},
        {eventfd(0, 0), POLLIN, 0},
    }}) {

    ctx.slac_config.chip_reset.enabled = false;

    callbacks.log = [](const std::string& msg) { printf("EVSE log: %s\n", msg.c_str()); };
    callbacks.send_raw_slac = [evse_fd](slac::messages::HomeplugMessage& msg) {
        write(evse_fd, msg.get_raw_message_ptr(), msg.get_raw_msg_len());
    };

    fsm.reset<slac::fsm::evse::ResetState>(ctx);
}

void EvseFsmController::trigger_enter_bcd() {
    write(pollfds[1].fd, &ENTER_BCD_EVENT_CODE, sizeof(ENTER_BCD_EVENT_CODE));
}

void EvseFsmController::run() {
    while (true) {
        auto feed_result = fsm.feed();

        if (feed_result.transition()) {
            // call immediately again
            continue;
        } else if (feed_result.internal_error() || feed_result.unhandled_event()) {
            throw std::runtime_error("Evse fsm: internal error / unhandled event");
            // FIXME (aw): would need to log here!
        }
        const auto timeout = (feed_result.has_value()) ? *feed_result : -1;
        if (timeout == 0) {
            continue;
        }

        const auto poll_result = poll(pollfds.data(), pollfds.size(), timeout);

        if (poll_result == 0) {
            // timeout
            continue;
        }

        // check for fds
        if (pollfds[0].revents & POLLIN) {
            auto raw_msg = ctx.slac_message_payload.get_raw_message_ptr();
            read(pollfds[0].fd, raw_msg, sizeof(slac::messages::homeplug_message));
            fsm.handle_event(slac::fsm::evse::Event::SLAC_MESSAGE);
        }

        if (pollfds[1].revents & POLLIN) {
            uint64_t tmp;
            read(pollfds[1].fd, &tmp, sizeof(tmp));

            if (tmp == ENTER_BCD_EVENT_CODE) {
                fsm.handle_event(slac::fsm::evse::Event::ENTER_BCD);
            }
            // new event, for now, we do not care, later on we could check, if it is an exit event code
        }
    }
}

class EvFsmController {
public:
    explicit EvFsmController(int ev_fd);

    void run();

private:
    slac::fsm::ev::ContextCallbacks callbacks;
    slac::fsm::ev::Context ctx{callbacks};
    slac::fsm::ev::FSM fsm;

    std::array<struct pollfd, 2> pollfds;
};

EvFsmController::EvFsmController(int ev_fd) {
    pollfds = {{
        {ev_fd, POLLIN, 0},
        {eventfd(0, 0), POLLIN, 0},
    }};

    callbacks.log = [](const std::string& msg) { printf("EV log: %s\n", msg.c_str()); };
    callbacks.send_raw_slac = [ev_fd](slac::messages::HomeplugMessage& msg) {
        write(ev_fd, msg.get_raw_message_ptr(), msg.get_raw_msg_len());
    };

    fsm.reset<slac::fsm::ev::ResetState>(ctx);
}

void EvFsmController::run() {
    // start connecting
    fsm.handle_event(slac::fsm::ev::Event::TRIGGER_MATCHING);
    while (true) {
        auto feed_result = fsm.feed();

        if (feed_result.transition()) {
            // call immediately again
            continue;
        } else if (feed_result.internal_error() || feed_result.unhandled_event()) {
            throw std::runtime_error("Evse fsm: internal error / unhandled event");
            // FIXME (aw): would need to log here!
        }
        const auto timeout = (feed_result.has_value()) ? *feed_result : -1;

        if (timeout == 0) {
            continue;
        }

        const auto poll_result = poll(pollfds.data(), pollfds.size(), timeout);

        if (poll_result == 0) {
            // timeout
            continue;
        }

        if (pollfds[0].revents & POLLIN) {
            auto raw_msg = ctx.slac_message.get_raw_message_ptr();
            read(pollfds[0].fd, raw_msg, sizeof(slac::messages::homeplug_message));
            fsm.handle_event(slac::fsm::ev::Event::SLAC_MESSAGE);
        }

        if (pollfds[1].revents & POLLIN) {
            uint64_t tmp;
            read(pollfds[1].fd, &tmp, sizeof(tmp));
            // new event, for now, we do not care, later on we could check, if it is an exit event code
        }
    }
}

auto main(int argc, char* argv[]) -> int {
    SocketPairBridge spb{handle_ev_input, handle_evse_input};

    EvseFsmController evse_ctrl{spb.get_evse_socket()};

    EvFsmController ev_ctrl{spb.get_ev_socket()};

    std::thread evse_thread(&EvseFsmController::run, &evse_ctrl);

    // give the EVSE some time to get ready
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    evse_ctrl.trigger_enter_bcd();
    std::thread ev_thread(&EvFsmController::run, &ev_ctrl);

    // the fsm controller could poll on its fd by itself and look for new slac messages

    evse_thread.join();
    ev_thread.join();

    // each controller needs to have a feeding thread, and an async input function for the slac message

    return 0;
}
