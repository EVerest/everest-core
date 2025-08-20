// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef PHYVERSO_GPIO_EV_GPIO_H
#define PHYVERSO_GPIO_EV_GPIO_H

#include "evConfig.h"
#include <atomic>
#include <chrono>
#include <gpio.hpp>
#include <sigslot/signal.hpp>
#include <stdexcept>
#include <stdint.h>
#include <utility>
#include <utils/thread.hpp>

class evGpio {

public:
    evGpio(evConfig& _verso_config);
    ~evGpio();

    void poll_thread();
    void run();
    bool init_gpios();

    // Read thread for serial port
    Everest::Thread poll_thread_handle;

    // Signals to communicate state changes to other modules
    sigslot::signal<int, bool> signal_stop_button_state;

    // List of used buttons/gpios
    enum PushButtonName {
        CONN1_PB_STOP,
        CONN2_PB_STOP,
        NUM_PB_NAMES,
    };

private:
    static constexpr uint32_t poll_time_ms = 10; //< time in ms between polled gpio input readings/debouncing

    // structure to encapsulate Everest::Gpio and corresponding debounce shift register
    // for now we will use 16 consecutive 0's or 1's to correspond to a stable input level (could be 8/32/64 aswell,
    // just needs adjusting in bitmasks and bitwidths)
    struct PushButton {
        void init_gpio(const Everest::GpioSettings& settings) {
            gpio.open(settings);
            gpio.set_input();
        };

        bool ready() {
            return gpio.is_ready();
        };

        void read() {
            debounce_shift_reg = (debounce_shift_reg << 1) | gpio.read();
            switch (debounce_shift_reg) {
            case 0xFFFF:
                state = true;
                break;
            case 0x0000:
                state = false;
                break;
            default:
                break;
            }

            state_changed = (last_state != state);

            last_state = state;
        };

        bool get_state() {
            return state;
        };

        bool get_state_changed() {
            return state_changed;
        };

        void set_enabled(bool _enabled) {
            enabled = _enabled;
        };

        bool get_enabled() {
            return enabled;
        };

    private:
        Everest::Gpio gpio;
        bool state, last_state, state_changed = false;
        bool enabled = false;
        uint16_t debounce_shift_reg = 0;
    };

    PushButton push_buttons[NUM_PB_NAMES];

    // config bridge (filled by json or everest module config)
    evConfig& verso_config;
};

#endif // PHYVERSO_GPIO_EV_GPIO_H
