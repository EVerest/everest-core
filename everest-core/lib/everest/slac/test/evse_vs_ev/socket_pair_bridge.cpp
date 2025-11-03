// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include "socket_pair_bridge.hpp"

#include <cstdio>
#include <cstring>

#include <errno.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <unistd.h>

// FIXME (aw): this helper doesn't really belong here
static void exit_with_error(const char* msg) {
    fprintf(stderr, "%s (%s)\n", msg, strerror(errno));
    exit(-EXIT_FAILURE);
}

SocketPairBridge::SocketPairBridge(const SocketInputHandler& ev_input_handler,
                                   const SocketInputHandler& evse_input_handler) :
    handle_ev_input(ev_input_handler), handle_evse_input(evse_input_handler) {
    std::array<int, 2> fd_pair;

    auto ret = socketpair(AF_UNIX, SOCK_DGRAM, 0, fd_pair.data());
    if (ret) {
        exit_with_error("ev socketpair creation failed");
    }

    sockets.ev_fd = fd_pair.at(0);
    sockets.ev_bridge_fd = fd_pair.at(1);

    ret = socketpair(AF_UNIX, SOCK_DGRAM, 0, fd_pair.data());
    if (ret) {
        exit_with_error("evse socketpair creation failed");
    }

    sockets.evse_fd = fd_pair.at(0);
    sockets.evse_bridge_fd = fd_pair.at(1);

    event_fd = eventfd(0, 0);

    if (event_fd == -1) {
        exit_with_error("eventfd failed");
    }

    // NOTE (aw): using 'this' here is safe
    loop_thread = std::thread(&SocketPairBridge::loop, this);
}

SocketPairBridge::~SocketPairBridge() {
    uint64_t event_value = 0x1;
    write(event_fd, &event_value, sizeof(event_value));

    loop_thread.join();

    close(event_fd);
    close(sockets.evse_bridge_fd);
    close(sockets.evse_fd);
    close(sockets.ev_bridge_fd);
    close(sockets.ev_fd);
}

void SocketPairBridge::loop() {
    constexpr static auto EV_INDEX = 0;
    constexpr static auto EVSE_INDEX = 1;
    constexpr static auto ABORT_INDEX = 2;

    struct pollfd pollfds[] = {
        {sockets.ev_bridge_fd, POLLIN, 0},
        {sockets.evse_bridge_fd, POLLIN, 0},
        {event_fd, POLLIN, 0},
    };

    static constexpr auto num_fds = sizeof(pollfds) / sizeof(struct pollfd);

    while (true) {
        const auto status = poll(pollfds, num_fds, -1);

        if (status == -1) {
            exit_with_error("bridge poll");
        }

        if (pollfds[ABORT_INDEX].revents & POLLIN) {
            uint64_t tmp;
            read(event_fd, &tmp, sizeof(tmp));
            printf("Received shutdown event, exiting\n");
            return;
        }

        if (pollfds[EV_INDEX].revents & POLLIN) {
            handle_ev_input(sockets.ev_bridge_fd, sockets.evse_bridge_fd);
        }

        if (pollfds[EVSE_INDEX].revents & POLLIN) {
            handle_evse_input(sockets.evse_bridge_fd, sockets.ev_bridge_fd);
        }
    }
}
