// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#include "watchdog.hpp"

namespace Everest {

WatchdogSupervisor::WatchdogSupervisor() {
    // Thread that monitors all registered watchdogs
    timeout_detection_thread = std::thread([this]() {
        while (not should_stop) {
            std::this_thread::sleep_for(check_interval);

            // check if any watchdog timed out
            for (const auto& dog : dogs) {
                std::chrono::steady_clock::time_point last_seen = dog.last_seen;
                if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_seen) >
                    dog.timeout) {
                    // Signal to callback handler
                    timeout(dog.description);
                }
            }
        }
    });
}

WatchdogSupervisor::~WatchdogSupervisor() {
    should_stop = true;
    if (timeout_detection_thread.joinable()) {
        timeout_detection_thread.join();
    }
}

WatchdogSupervisor::WatchdogData::WatchdogData(const std::string& _description, std::chrono::seconds _timeout) :
    description(_description), timeout(_timeout) {
    last_seen = std::chrono::steady_clock::now();
}

void WatchdogSupervisor::timeout(const std::string& description) {
    printf("---------WATCHDOG expired ------------ \n");
    exit(255);
}

} // namespace Everest
