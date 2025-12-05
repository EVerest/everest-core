// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <charge_bridge/everest_api/evse_bsp_api.hpp>
#include <charge_bridge/everest_api/ovm_api.hpp>
#include <chrono>
#include <cstdint>
#include <everest/io/event/fd_event_register_interface.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/mqtt/mqtt_client.hpp>
#include <everest_api_types/evse_board_support/API.hpp>
#include <everest_api_types/evse_manager/API.hpp>
#include <everest_api_types/utilities/Topics.hpp>
#include <functional>
#include <protocol/cb_common.h>
#include <protocol/evse_bsp_cb_to_host.h>
#include <protocol/evse_bsp_host_to_cb.h>
#include <string>

namespace charge_bridge::evse_bsp {

namespace API_BSP = everest::lib::API::V1_0::types::evse_board_support;

struct everest_api_config {
    std::string mqtt_remote;
    uint16_t mqtt_port;
    uint32_t mqtt_ping_interval_ms;
    evse_bsp_config bsp;
    evse_ovm_config ovm;
};

class api_connector : public everest::lib::io::event::fd_event_register_interface {
    using tx_ftor = std::function<void(evse_bsp_host_to_cb const&)>;
    using rx_ftor = std::function<void(evse_bsp_cb_to_host const&)>;

public:
    api_connector(everest_api_config const& config, std::string const& cb_identifier);
    void set_cb_tx(tx_ftor const& handler);
    void set_cb_message(evse_bsp_cb_to_host const& msg);

    bool register_events(everest::lib::io::event::fd_event_handler& handler) override;
    bool unregister_events(everest::lib::io::event::fd_event_handler& handler) override;

private:
    void handle_mqtt_connect();
    void handle_cb_connection_state();
    bool check_cb_heartbeat();

    std::string m_cb_identifier;
    everest::lib::io::mqtt::mqtt_client m_mqtt;
    tx_ftor m_tx;
    std::chrono::steady_clock::time_point m_last_cb_heartbeat;
    everest::lib::io::event::timer_fd m_sync_timer;

    std::string m_bsp_receive_topic;
    std::string m_bsp_send_topic;
    std::string m_ovm_receive_topic;
    std::string m_ovm_send_topic;
    bool m_ovm_enabled{false};
    bool m_cb_initial_comm_check{true};
    bool m_cb_connected{false};
    evse_bsp_host_to_cb m_host_status;

    evse_bsp_api m_bsp;
    ovm_api m_ovm;
};
} // namespace charge_bridge::evse_bsp
