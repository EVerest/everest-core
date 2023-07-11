// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef TIMEOUT_HPP
#define TIMEOUT_HPP

#include <chrono>
#include <sigslot/signal.hpp>

#include "utils/thread.hpp"

using namespace std::chrono;

/*
 Simple helper class for a polling timeout
*/

class Timeout {
public:
    void start(milliseconds _t) {
        t = _t;
        start_time = steady_clock::now();
        running = true;
    }

    void stop() {
        running = false;
    }

    bool is_running() {
        return running;
    }

    bool reached() {
        if (!running)
            return false;
        if ((steady_clock::now() - start_time) > t) {
            running = false;
            return true;
        } else {
            return false;
        }
    }

private:
    milliseconds t;
    time_point<steady_clock> start_time;
    bool running{false};
};

/* Simple helper class for a timeout with internal thread */
class AsyncTimeout {
public:
    void start(milliseconds _t) {
        if (running) {
            stop();
        }
        t = _t;
        start_time = steady_clock::now();
        running = true;
        // start waiting thread
        wait_thread = std::thread([this]() {
            while (not wait_thread.shouldExit()) {
                std::this_thread::sleep_for(resolution);
                if (reached()) {
                    // Note the order is important here.
                    // We first signal reached which will call all callbacks.
                    // The timer is still running in this in those callbacks, so they can also call reached() and get a
                    // true as return value.
                    signal_reached();
                    // After all signal handlers are called, we stop the timer.
                    running = false;
                    return;
                }
            }
        });
    }

    // Note that stopping the timer may take up to "resolution" amount of time to return
    void stop() {
        wait_thread.stop();
        running = false;
    }

    bool is_running() {
        return running;
    }

    bool reached() {
        if (!running) {
            return false;
        }
        if ((steady_clock::now() - start_time) > t) {
            return true;
        } else {
            return false;
        }
    }

    sigslot::signal<> signal_reached;

private:
    constexpr static auto resolution = 500ms;
    milliseconds t;
    time_point<steady_clock> start_time;
    std::atomic_bool running{false};
    Everest::Thread wait_thread;
};

#endif
