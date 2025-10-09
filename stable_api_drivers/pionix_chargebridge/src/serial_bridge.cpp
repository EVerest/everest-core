// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "everest/io/serial/event_pty.hpp"
#include <charge_bridge/serial_bridge.hpp>
#include <charge_bridge/utilities/logging.hpp>
#include <cstring>
#include <everest/io/event/fd_event_handler.hpp>
#include <protocol/cb_can_message.h>

namespace charge_bridge {

serial_bridge::serial_bridge(serial_bridge_config const& config) :
    m_pty(), m_tcp(config.cb_remote, config.cb_port, 1000) {
    using namespace std::chrono_literals;

    auto link_ok = m_symlink.set_link(m_pty.get_slave_path(), config.serial_device);
    if (not link_ok) {
        throw std::runtime_error("Failed to setup symbolic links for serial ports");
    }

    m_tcp.set_on_ready_action([this]() {
        m_tcp.get_raw_handler()->set_keep_alive(3, 1, 1);
        m_tcp.get_raw_handler()->set_user_timeout(4000);
    });

    m_pty.set_data_handler([this](auto const& data, auto&) { m_tcp.tx(data); });

    m_tcp.set_rx_handler([this](auto const& data, auto&) { m_pty.tx(data); });

    auto identifier = config.cb + "/" + config.item;
    m_pty.set_error_handler([this, identifier](auto id, auto const& msg) {
        utilities::print_error(identifier, "SERIAL/PTY", id) << msg << std::endl;
        if (id not_eq 0) {
            m_pty.reset();
        }
    });

    m_tcp.set_error_handler([this, identifier](auto id, auto const& msg) {
        if (m_tcp_last_error_id not_eq id) {
            utilities::print_error(identifier, "SERIAL/TCP", id) << msg << std::endl;
            m_tcp_last_error_id = id;
        }
        if (id not_eq 0) {
            m_tcp.reset();
        }
    });
}

void serial_bridge::reset_tcp() {
    m_tcp.reset();
}

std::string serial_bridge::get_slave_path() {
    return m_pty.get_slave_path();
}

bool serial_bridge::register_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    auto result = true;
    result = result && handler.register_event_handler(&m_pty);
    result = result && handler.register_event_handler(&m_tcp);
    return result;
    // clang-format on
}

bool serial_bridge::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return
        handler.unregister_event_handler(&m_pty) &&
        handler.unregister_event_handler(&m_tcp);
    // clang-format on
}

} // namespace charge_bridge
