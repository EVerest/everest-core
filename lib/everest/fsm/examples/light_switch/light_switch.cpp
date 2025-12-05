// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include <cstddef>
#include <cstdio>

#include <chrono>
#include <thread>
#include <vector>

#include "fsm.hpp"
#include "states.hpp"

auto delayed_timepoint(int delay_ms) {
    return std::chrono::steady_clock::now() + std::chrono::milliseconds(delay_ms);
}

void feed_machine_until(FSM& machine, int delay_ms) {
    auto next_event_tp = delayed_timepoint(delay_ms);

    while (true) {

        auto feed_result = machine.feed();
        if (feed_result.transition()) {
            continue;
        } else if (feed_result.unhandled_event()) {
            break;
        } else if (feed_result.has_value() == false) {
            // returning no value means don't do anything
            break;
        }

        // fall-through: got a value
        if (*feed_result == 0) {
            continue;
        }

        auto next_feed_tp = delayed_timepoint(*feed_result);
        if (next_feed_tp < next_event_tp) {
            std::this_thread::sleep_until(next_feed_tp);
        } else {
            break;
        }
    }

    std::this_thread::sleep_until(next_event_tp);
}

int main(int argc, char* argv[]) {
    Context ctx;

    const int DEFAULT_DELAY = 200;

    struct EventTodo {
        Event event;
        int delay;
    };

    auto events = std::vector<EventTodo>({
        {Event::PRESSED_ON, DEFAULT_DELAY},
        {Event::PRESSED_ON, DEFAULT_DELAY},
        {Event::PRESSED_ON, DEFAULT_DELAY},
        {Event::ENTER_MOTION_MODE, DEFAULT_DELAY},
        {Event::PRESSED_ON, DEFAULT_DELAY},
        {Event::PRESSED_OFF, DEFAULT_DELAY},
        {Event::ENTER_MOTION_MODE, DEFAULT_DELAY},
        {Event::MOTION_DETECT, 6000},
        {Event::MOTION_TIMEOUT, DEFAULT_DELAY},
        {Event::MOTION_DETECT, DEFAULT_DELAY},
        {Event::PRESSED_ON, DEFAULT_DELAY},
        {Event::PRESSED_ON, DEFAULT_DELAY},
    });

#ifdef HEAP_FREE_MODE
    BufferType static_buffer{};
    FSM machine(static_buffer);
#else
    FSM machine{};
#endif

    machine.reset<LightOff>(ctx);

    for (const auto& todo : events) {
        machine.handle_event(todo.event);

        feed_machine_until(machine, todo.delay);
    }

    return 0;
}
