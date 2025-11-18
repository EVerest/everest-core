// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <everest/io/event/fd_event_register_interface.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/tun_tap/tap_client.hpp>
#include <everest/io/udp/udp_client.hpp>

namespace charge_bridge {

struct plc_bridge_config {
    std::string cb;
    std::string item;
    uint16_t cb_port;
    std::string cb_remote;
    std::string plc_tap;
    std::string plc_ip;
    std::string plc_netmaks;
    int plc_mtu{1518};
};

class plc_bridge : public everest::lib::io::event::fd_event_register_interface {
public:
    plc_bridge(plc_bridge_config const& config);
    ~plc_bridge() = default;

    bool register_events(everest::lib::io::event::fd_event_handler& handler) override;
    bool unregister_events(everest::lib::io::event::fd_event_handler& handler) override;

private:
    void handle_timer_event();

    everest::lib::io::tun_tap::tap_client m_tap;
    everest::lib::io::udp::udp_client m_udp;
    everest::lib::io::event::timer_fd m_timer;
    bool m_udp_on_error{false};
    bool m_tap_on_error{false};
};

} // namespace charge_bridge
