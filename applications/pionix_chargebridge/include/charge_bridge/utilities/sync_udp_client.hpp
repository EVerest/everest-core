// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include "everest/io/udp/udp_payload.hpp"
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/udp/udp_socket.hpp>
#include <optional>

namespace charge_bridge::utilities {

class sync_udp_client {
public:
    using udp_payload = everest::lib::io::udp::udp_payload;
    using reply = std::optional<udp_payload>;
    sync_udp_client(std::string const& remote, std::uint16_t port);
    sync_udp_client(std::string const& remote, std::uint16_t port, std::uint16_t retries, std::uint16_t timeout_ms);
    reply request_reply(udp_payload const& payload);
    reply request_reply(udp_payload const& payload, std::uint16_t timeout_ms, std::uint16_t retries);
    bool tx(udp_payload const& payload);
    reply rx();
    reply rx(std::uint16_t timeout_ms);
    bool is_open();

private:
    void init(std::string const& remote, std::uint16_t port);
    void clear_socket();

    std::uint16_t m_retries;
    std::uint16_t m_timeout_ms;
    everest::lib::io::udp::udp_client_socket m_udp;
    everest::lib::io::event::fd_event_handler m_handler;
};

} // namespace charge_bridge::utilities
