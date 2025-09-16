// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <everest/io/socket/socket.hpp>
#include <everest/io/tcp/tcp_socket.hpp>
#include <sys/socket.h>

namespace everest::lib::io::tcp {

bool tcp_socket::open(std::string const& remote, uint16_t port) {
    try {
        auto socket = socket::open_tcp_socket(remote, port);
        socket::set_non_blocking(socket);
        m_fd = std::move(socket);
        return socket::get_pending_error(socket) == 0;
    } catch (...) {
    }
    return false;
}

bool tcp_socket::tx(PayloadT& payload) {
    if (not is_open()) {
        return false;
    }

    auto status = ::send(m_fd, payload.data(), payload.size(), 0);
    if (status == -1) {
        return false;
    }
    if (status < static_cast<ssize_t>(payload.size())) {
        // We have a reference to the current data. Replace it with what is left to be written
        // and return false. This signals the current block cannot be removed from the buffer.
        payload = {payload.begin() + status, payload.end()};
        return false;
    }
    return true;
}

bool tcp_socket::rx(PayloadT& buffer) {
    if (not is_open()) {
        return false;
    }
    buffer.resize(default_buffer_size);
    auto status = ::recv(m_fd, buffer.data(), buffer.size(), 0);
    if (status <= 0) { // -1 is an error, 0 is a connection closed by the peer
        return false;
    }
    buffer.resize(status);
    return true;
}

int tcp_socket::get_fd() const {
    return m_fd;
}

int tcp_socket::get_error() const {
    if (socket::is_tcp_socket_alive(m_fd)) {
        return socket::get_pending_error(m_fd);
    } else if (is_open()) {
        return ECONNRESET;
    }
    return socket::get_pending_error(m_fd);
}

bool tcp_socket::is_open() const {
    return m_fd.is_fd();
}

void tcp_socket::close() {
    m_fd.close();
}

bool tcp_socket::set_keep_alive(uint32_t count, uint32_t idle_s, uint32_t intval_s) {
    try {
        socket::set_tcp_keepalive(m_fd, count, idle_s, intval_s);
        return true;
    } catch (...) {
    }
    return false;
}

bool tcp_socket::set_user_timeout(uint32_t to_ms) {
    try {
        socket::set_tcp_user_timeout(m_fd, to_ms);
        return true;
    } catch (...) {
    }
    return false;
}

} // namespace everest::lib::io::tcp
