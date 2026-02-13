// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2026 Pionix GmbH and Contributors to EVerest
#include <ieee2030/charger/io/can_broker_charger.hpp>

#include <chrono>
#include <thread>

#include <iostream>

using namespace ieee2030::charger::io;

int main() {

    std::cout << "Start CanBroker Charger\n";

    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto broker = CanBrokerCharger("vcan0");

    std::cout << "Wait 2 seconds!\n";

    std::this_thread::sleep_for(std::chrono::seconds(2));

    broker.enable_tx_can();

    std::cout << "Send can message for 10 seconds!\n";
    std::this_thread::sleep_for(std::chrono::seconds(10));

    broker.disable_tx_can();

    std::cout << "Stop CanBroker Charger\n";

    return 0;
}
