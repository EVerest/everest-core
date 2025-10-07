// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/**
 * @example test_serial.cpp Event based PTY handling for data and status updates.
 */

#include <cstdint>
#include <cstring>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/serial/event_pty.hpp>
#include <everest/io/serial/serial.hpp>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

using namespace everest::lib::io;
using namespace everest::lib::io::utilities;
using namespace everest::lib::io::event;
using namespace std::chrono_literals;

int main() {
    std::cout << "This is serial test" << std::endl;

    // Create PTY
    serial::event_pty handler;
    // Register callback for data events
    handler.set_data_handler([](auto const& pl, auto& dev) {
        auto msg = std::string(pl.begin(), pl.end());
        std::cout << " ##data update: " << msg << std::endl;

        // generate reply and send it to the PTY
        static auto counter = 0;
        auto str = std::to_string(++counter) + "\n";
        auto ptr = reinterpret_cast<uint8_t*>(str.data());
        serial::event_pty::ClientPayloadT repl(ptr, ptr + str.size());
        dev.tx(repl);
    });

    handler.set_status_handler([](auto const& status) {
        std::cout << " ##status update" << std::endl;
        // clang-format off
        std::cout << " - ixon   -> " << status.ixon << "\n"
                  << " - ixoff  -> " << status.ixoff << "\n"
                  << " - cstopb -> " << status.cstopb << "\n"
                  << " - baud   -> " << status.cbaud
                  << std::endl;
        // clang-format on
    });

    handler.set_error_handler([](int error, std::string const& err_msg) {
        std::cout << "ERRORHANDLER: " << err_msg << "(" << error << ")" << std::endl;
    });

    // register the client with the event handler
    fd_event_handler ev_handler;
    ev_handler.register_event_handler(&handler);

    while (true) {
        ev_handler.poll();
    }
}
