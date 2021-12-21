// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include <fsm/specialization/sync/posix.hpp>

#include "sync_fsm.hpp"

void set_brightness(int value) {
    std::cout << "Set brightness to value: " << value << "\n";
}

class FSMDriver {
public:
    FSMDriver() {
        fsm_ctrl.reset(fsm_desc.sd_light_off);
        running = true;
        loop_thread = std::thread(&FSMDriver::loop, this);
    };

    void on() {
        std::lock_guard<std::mutex> lck(drive_mutex);
        fsm_ctrl.submit_event(EventPressedOn());
        notify();
    };

    void off() {
        std::lock_guard<std::mutex> lck(drive_mutex);
        fsm_ctrl.submit_event(EventPressedOff());
        notify();
    };

    void mot() {
        std::lock_guard<std::mutex> lck(drive_mutex);
        fsm_ctrl.submit_event(EventPressedMot());
        notify();
    };

    void motion_detect() {
        std::lock_guard<std::mutex> lck(drive_mutex);
        fsm_ctrl.submit_event(EventMotionDetect());
        notify();
    };

    ~FSMDriver() {
        running = false;
        notify();

        loop_thread.join();
    }

private:
    void notify() {
        new_event = true;
        cv_new_event.notify_one();
    }

    void loop() {
        std::unique_lock<std::mutex> feed_lck(drive_mutex);
        int next_timeout_in_ms;

        while (running) {
            do {
                next_timeout_in_ms = fsm_ctrl.feed();
            } while (next_timeout_in_ms == 0);

            if (next_timeout_in_ms < 0) {
                cv_new_event.wait(feed_lck, [this] { return new_event; });
            } else {
                cv_new_event.wait_for(feed_lck, std::chrono::milliseconds(next_timeout_in_ms),
                                      [this] { return new_event; });
            }

            if (new_event) {
                new_event = false;
            }
        }
    }

    std::thread loop_thread;

    std::mutex drive_mutex;
    std::condition_variable cv_new_event;
    bool new_event{false};

    bool running;

    FSM fsm_desc;
    fsm::sync::PosixController<FSM::StateHandleType> fsm_ctrl;
};

int main(int argc, char* argv[]) {

    FSMDriver driver;
    driver.on();
    driver.on();
    driver.on();
    driver.on();
    driver.on();
    driver.off();
    driver.mot();
    driver.motion_detect();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // fsm_ctrl.stop();
    return 0;
}
