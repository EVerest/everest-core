// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "everest/io/event/fd_event_handler.hpp"
#include <charge_bridge/utilities/sync_udp_client.hpp>
#include <chrono>
#include <optional>

namespace charge_bridge::utilities {
using reply = sync_udp_client::reply;
using namespace std::chrono_literals;

sync_udp_client::sync_udp_client(std::string const& remote, std::uint16_t port) : m_retries(0), m_timeout_ms(1000) {
    init(remote, port);
}

sync_udp_client::sync_udp_client(std::string const& remote, std::uint16_t port, std::uint16_t retries,
                                 std::uint16_t timeout_ms) :
    m_retries(retries), m_timeout_ms(timeout_ms) {
    init(remote, port);
}

void sync_udp_client::init(std::string const& remote, std::uint16_t port) {
    m_udp.open_as_client(remote, port);
    m_handler.register_event_handler(m_udp.get_fd(), [this](auto) {}, everest::lib::io::event::poll_events::read);
}

reply sync_udp_client::request_reply(udp_payload const& payload) {
    return request_reply(payload, m_timeout_ms, m_retries);
}

reply sync_udp_client::request_reply(udp_payload const& payload, std::uint16_t timeout_ms, std::uint16_t retries) {
    udp_payload result;
    clear_socket();
    if (not m_udp.tx(payload)) {
        return std::nullopt;
    }
    for (std::uint16_t i = 0; i < retries; ++i) {
        if (not m_handler.poll(std::chrono::milliseconds(timeout_ms))) {
            if (not m_udp.tx(payload)) {
                return std::nullopt;
            }
            continue;
        }
        if (not m_udp.rx(result)) {
            return std::nullopt;
        }
        return result;
    }
    return std::nullopt;
}

bool sync_udp_client::tx(udp_payload const& payload) {
    return m_udp.tx(payload);
}

reply sync_udp_client::rx() {
    return rx(m_timeout_ms);
}

reply sync_udp_client::rx(std::uint16_t timeout_ms) {
    udp_payload result;
    if (not m_handler.poll(std::chrono::milliseconds(timeout_ms))) {
        return std::nullopt;
    }
    if (not m_udp.rx(result)) {
        return std::nullopt;
    }
    return result;
}

bool sync_udp_client::is_open() {
    return m_udp.is_open();
}

void sync_udp_client::clear_socket() {
    udp_payload tmp;
    while (m_handler.poll(0ms)) {
        m_udp.rx(tmp);
    };
}

} // namespace charge_bridge::utilities
