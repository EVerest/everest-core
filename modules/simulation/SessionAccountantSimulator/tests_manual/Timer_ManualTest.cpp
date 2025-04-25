/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "Timer/SimpleTimeout.hpp"

using namespace std::chrono_literals;

void start_timer(std::shared_ptr<Timeout::TimeoutBase> timer) {
    timer->set_callback([]() { std::cout << "Callback called!" << std::endl; });
    timer->start();

    std::cout << "Everything set up waiting ..." << std::endl;
}

void wait_for_s(int duration, int offset = 0) {
    for (auto i = offset * 10; i < (offset + duration) * 10; i++) {
        std::cout << "\r" << i / 10 << "." << i % 10 << std::flush;
        std::this_thread::sleep_for(100ms);
    }
}

int main() {
    std::cout << "Demonstrating SimpleTimeout with real delay (1s)" << std::endl;
    {
        std::shared_ptr<Timeout::TimeoutBase> timer = std::make_shared<Timeout::SimpleTimeout>(1s);

        start_timer(timer);
        wait_for_s(2);
    }
    std::cout << "Demonstrating SimpleTimeout with real delay (2s) but cancelled before timeout" << std::endl;
    {
        std::shared_ptr<Timeout::TimeoutBase> timer = std::make_shared<Timeout::SimpleTimeout>(2s);

        start_timer(timer);
        wait_for_s(1);
        timer->cancel();
        wait_for_s(2, 1);
    }
    std::cout << "\n\nDemonstrating SimpleTimeout with real delay (2s) and reset the timer 2 times after 1s."
              << std::endl;
    {
        std::shared_ptr<Timeout::TimeoutBase> timer = std::make_shared<Timeout::SimpleTimeout>(2s);

        start_timer(timer);
        wait_for_s(1);
        timer->reset();
        std::cout << "Reset the timer to 0" << std::endl;
        wait_for_s(1, 1);
        timer->reset();
        std::cout << "Reset the timer to 0" << std::endl;
        wait_for_s(3, 2);
    }
    std::cout << "\n\nDemonstrating SimpleTimeout with real delay (2s) and changing it to 3s after 1s." << std::endl;
    {
        std::shared_ptr<Timeout::TimeoutBase> timer = std::make_shared<Timeout::SimpleTimeout>(2s);

        start_timer(timer);
        wait_for_s(1);
        timer->set_duration(3s);
        std::cout << "Reset the timer and change duration to 3s" << std::endl;
        wait_for_s(4, 1);
    }
    std::cout << "\n\nDemonstrating MockTimeout with no real delay" << std::endl;
    {
        std::shared_ptr<Timeout::MockTimeout> mock_timer = std::make_shared<Timeout::MockTimeout>(1s);

        start_timer(mock_timer);
        mock_timer->elapse_time();
        wait_for_s(1);
    }

    return 0;
}