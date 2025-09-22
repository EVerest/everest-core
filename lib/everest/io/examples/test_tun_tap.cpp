// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <everest/io/tun_tap/tap_client.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

using namespace everest::lib::io;
using namespace std::chrono_literals;
using generic_tun_tap = typename tun_tap::tap_client::interface;

const std::string current_date_time() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

std::atomic_bool on_error{false};

void rx(std::vector<uint8_t> const& data, generic_tun_tap& dev) {
    std::cout << "[ tap ] -> ";
    for (auto elem : data) {
        std::cout << std::setw(2) << std::setfill('0') << (uint)elem << " ";
    }
    std::cout << std::endl;
}

void error_fn(int err, std::string const& message, std::ostream& logger) {
    if (err == 0) {
        std::cerr << "ERROR: ( " << err << " ): " << message << std::endl;
        on_error.store(false);
    } else {
        std::stringstream ss;
        ss << "ERROR: ( " << err << " ): " << message << std::endl;

        on_error.store(true);
        std::cerr << ss.str() << std::endl;
        logger << current_date_time() << "  " << ss.str() << std::endl;
    }
}

int main() {
    std::ofstream logfile("logfile.txt", std::ofstream::out);
    std::cout << "hello socket_can test" << std::endl;
    tun_tap::tap_client tap("test_tap", "172.1.1.1", "255.255.255.0", 1518);
    tap.set_rx_handler(&rx);
    tap.set_error_handler([&](auto err, auto msg) {
        error_fn(err, msg, logfile);
        if (err != 0) {
            tap.reset();
            on_error.store(tap.on_error());
            std::this_thread::sleep_for(1s);
        }
    });
    while (true) {
        tap.sync();
    }

    return 0;
}
