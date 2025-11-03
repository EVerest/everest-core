// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <atomic>
#include <chrono>
#include <everest/io/can/socket_can.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

using namespace everest::lib::io;
using namespace std::chrono_literals;
using generic_socket_can = typename can::socket_can::interface;

const std::string current_date_time() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

std::atomic_bool on_error{false};

void rx(can::can_dataset const& data, generic_socket_can& dev) {
    auto can_id = data.get_can_id();
    auto& payload = data.payload;

    std::cout << "[ " << std::hex << can_id << " ] -> ";
    for (auto elem : payload) {
        std::cout << std::setw(2) << std::setfill('0') << (uint)elem << " ";
    }
    std::cout << std::endl;

    can::can_dataset msg;
    msg.set_can_id(002);
    msg.payload = {01, 02, 03, 04};
    dev.tx(msg);
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
    can::socket_can can_dev("can0");
    can_dev.set_rx_handler(&rx);
    can_dev.set_error_handler([&](auto err, auto msg) {
        error_fn(err, msg, logfile);
        if (err != 0) {
            can_dev.reset();
            on_error.store(can_dev.on_error());
            std::this_thread::sleep_for(1s);
        }
    });
    while (true) {
        can_dev.sync();
    }

    return 0;
}
