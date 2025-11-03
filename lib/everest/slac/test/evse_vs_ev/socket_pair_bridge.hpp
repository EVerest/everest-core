// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef TESTS_EVSE_VS_EV_SOCKET_PAIR_BRIDGE_HPP
#define TESTS_EVSE_VS_EV_SOCKET_PAIR_BRIDGE_HPP

#include <cstdint>
#include <functional>
#include <thread>

class SocketPairBridge {
public:
    struct Sockets {
        int ev_fd;
        int ev_bridge_fd;

        int evse_fd;
        int evse_bridge_fd;
    };
    using SocketInputHandler = std::function<void(int, int)>;

    SocketPairBridge(const SocketInputHandler& ev_input_handler, const SocketInputHandler& evse_input_handler);
    // FIXME (aw): disable copy constructors!

    int get_ev_socket() {
        return sockets.ev_fd;
    };

    int get_evse_socket() {
        return sockets.evse_fd;
    }

    ~SocketPairBridge();

private:
    SocketInputHandler handle_ev_input;
    SocketInputHandler handle_evse_input;

    void loop();

    Sockets sockets{};

    int event_fd;

    std::thread loop_thread;

    uint8_t transfer_buffer[1024];
};

#endif // TESTS_EVSE_VS_EV_SOCKET_PAIR_BRIDGE_HPP
