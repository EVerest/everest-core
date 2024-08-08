// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evGpio.h"
#include <everest/logging.hpp>
#include <fmt/format.h>

evGpio::evGpio(evConfig& _verso_config) : verso_config(_verso_config) {
}

evGpio::~evGpio() {
    // TODO: deinit gpios?
}

bool evGpio::init_gpios() {
    push_buttons[CONN1_PB_STOP].set_enabled(verso_config.conf.conn1_gpio_stop_button_enabled);
    push_buttons[CONN2_PB_STOP].set_enabled(verso_config.conf.conn2_gpio_stop_button_enabled);

    if (push_buttons[CONN1_PB_STOP].get_enabled()) {
        Everest::GpioSettings settings;
        settings.chip_name = fmt::format("gpiochip{}", verso_config.conf.conn1_gpio_stop_button_bank);
        settings.line_number = verso_config.conf.conn1_gpio_stop_button_pin;
        settings.inverted = verso_config.conf.conn1_gpio_stop_button_invert;
        push_buttons[CONN1_PB_STOP].init_gpio(settings);
        if (not push_buttons[CONN1_PB_STOP].ready()) {
            EVLOG_error << "Could not initialize Connector 1 push button";
            return false;
        }
    }

    if (push_buttons[CONN2_PB_STOP].get_enabled()) {
        Everest::GpioSettings settings;
        settings.chip_name = fmt::format("gpiochip{}", verso_config.conf.conn2_gpio_stop_button_bank);
        settings.line_number = verso_config.conf.conn2_gpio_stop_button_pin;
        settings.inverted = verso_config.conf.conn2_gpio_stop_button_invert;
        push_buttons[CONN2_PB_STOP].init_gpio(settings);
        if (not push_buttons[CONN2_PB_STOP].ready()) {
            EVLOG_error << "Could not initialize Connector 2 push button";
            return false;
        }
    }

    return true;
}

void evGpio::run() {
    poll_thread_handle = std::thread(&evGpio::poll_thread, this);
}

void evGpio::poll_thread() {
    while (true) {
        if (poll_thread_handle.shouldExit())
            break;

        // iterate over button list
        for (int i = 0; i < NUM_PB_NAMES; i++) {
            PushButton& button = push_buttons[i];
            if (not button.get_enabled())
                continue;

            // check if GPIO is still usable
            if (not button.ready()) {
                EVLOG_error << fmt::format("Push button {} not ready. Stopping thread.", i + 1);
                goto cleanup; // break out of polling loop immediatly and terminate thread
            }

            button.read();

            if (button.get_state_changed()) {
                // select which signal to send depending on button name
                switch (i) {
                case CONN1_PB_STOP:
                    signal_stop_button_state(1, button.get_state());
                    break;
                case CONN2_PB_STOP:
                    signal_stop_button_state(2, button.get_state());
                    break;
                default:
                    break;
                }
            }
        }

        // sleep the nominal polling interval time
        std::this_thread::sleep_for(std::chrono::milliseconds(poll_time_ms));
    }

cleanup : {}
}