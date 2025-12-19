// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <charge_bridge/everest_api/api_connector.hpp>
#include <everest/io/event/fd_event_register_interface.hpp>
#include <everest/io/udp/udp_client.hpp>
#include <everest_api_types/evse_board_support/API.hpp>

namespace charge_bridge {

struct bsp_bridge_config {
    std::string cb;
    std::string item;
    std::uint16_t cb_port;
    std::string cb_remote;
    evse_bsp::everest_api_config api;
};

class bsp_bridge : public everest::lib::io::event::fd_event_register_interface {
public:
    bsp_bridge(bsp_bridge_config const& config);
    ~bsp_bridge() = default;

    bool register_events(everest::lib::io::event::fd_event_handler& handler) override;
    bool unregister_events(everest::lib::io::event::fd_event_handler& handler) override;

private:
    void handle_timer_event();

    evse_bsp::api_connector m_api;
    everest::lib::io::udp::udp_client m_udp;
    everest::lib::io::event::timer_fd m_timer;
    bool m_udp_on_error{false};
};

} // namespace charge_bridge
