// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "everest/io/udp/udp_payload.hpp"
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/unique_fd.hpp>
#include <everest/io/socket/socket.hpp>
#include <everest/io/udp/udp_socket.hpp>
#include <iostream>
#include <net/if.h>
#include <netinet/in.h>
#include <optional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace everest::lib::io::udp {

bool udp_socket_base::open_as_client(std::string const& remote, uint16_t port) {
    try {
        auto socket = socket::open_udp_client_socket(remote, port);
        socket::set_non_blocking(socket);
        m_owned_udp_fd = std::move(socket);
        return socket::get_pending_error(socket) == 0;
    } catch (...) {
    }
    return false;
}

bool udp_socket_base::open_as_server(uint16_t port) {
    auto socket = socket::open_udp_server_socket(port);
    socket::set_non_blocking(socket);
    m_owned_udp_fd = std::move(socket);
    return socket::get_pending_error(socket) == 0;
}

bool udp_socket_base::is_open() {
    return m_owned_udp_fd.is_fd();
}

void udp_socket_base::close() {
    m_owned_udp_fd.close();
}

int udp_socket_base::get_fd() const {
    return m_owned_udp_fd;
}

int udp_socket_base::get_error() const {
    return socket::get_pending_error(m_owned_udp_fd);
}

bool udp_socket_base::tx_impl(void const* payload, size_t size) {
    if (not is_open()) {
        return ENETDOWN;
    }
    size_t nbytes = ::send(m_owned_udp_fd, payload, size, 0);

    return nbytes == size;
}

bool udp_socket_base::tx_impl(void const* payload, size_t size, udp_info const& destination) {
    if (not is_open()) {
        return false;
    }
    socklen_t peer_addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in peer_addr;
    peer_addr.sin_port = destination.port;
    peer_addr.sin_addr.s_addr = destination.addr;
    peer_addr.sin_family = destination.family;
    size_t nbytes = ::sendto(m_owned_udp_fd, payload, size, 0, (struct sockaddr*)&peer_addr, peer_addr_len);

    return nbytes == size;
}

std::optional<udp_info> udp_socket_base::rx_impl(void* buffer, size_t buffer_size, ssize_t& payload_size) {
    socklen_t peer_addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in peer_addr;
    if (is_open()) {
        payload_size = ::recvfrom(m_owned_udp_fd, buffer, buffer_size, 0, (struct sockaddr*)&peer_addr, &peer_addr_len);
        if (payload_size >= 0) {
            return udp_info{peer_addr.sin_addr.s_addr, peer_addr.sin_port, peer_addr.sin_family};
        }
    }
    return std::nullopt;
}

bool udp_client_socket::open(std::string const& remote, uint16_t port) {
    return open_as_client(remote, port);
}

bool udp_client_socket::tx(udp_payload const& payload) {
    return tx_impl(payload.buffer.data(), payload.size());
}

bool udp_client_socket::rx(udp_payload& payload) {
    ssize_t msg_size = 0;
    auto result = rx_impl(rx_buffer.data(), rx_buffer.size(), msg_size);
    if (result) {
        payload.set_message(rx_buffer.data(), msg_size);
    }
    return result.has_value();
}

} // namespace everest::lib::io::udp
