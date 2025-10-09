// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <everest/io/event/fd_event_register_interface.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/serial/event_pty.hpp>
#include <everest/io/tcp/tcp_client.hpp>
#include <charge_bridge/utilities/symlink.hpp>

namespace charge_bridge {

struct serial_bridge_config {
    std::string cb;
    std::string item;
    uint16_t cb_port;
    std::string cb_remote;
    std::string serial_device;
};

class serial_bridge : public everest::lib::io::event::fd_event_register_interface {
public:
    serial_bridge(serial_bridge_config const& config);
    ~serial_bridge() = default;

    bool register_events(everest::lib::io::event::fd_event_handler& handler) override;
    bool unregister_events(everest::lib::io::event::fd_event_handler& handler) override;
    std::string get_slave_path();

private:
    void reset_tcp();

    everest::lib::io::serial::event_pty m_pty;
    everest::lib::io::tcp::tcp_client m_tcp;
    utilities::symlink m_symlink;
//    everest::lib::io::event::timer_fd m_timer;
};

} // namespace charge_bridge
