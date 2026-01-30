// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <chrono>
#include <cstdint>
#include <everest/io/event/fd_event_register_interface.hpp>
#include <everest/io/event/timer_fd.hpp>
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
namespace API_EVM = everest::lib::API::V1_0::types::evse_manager;

struct evse_bsp_config {
    std::string module_id;
    API_BSP::HardwareCapabilities capabilities;
};

class evse_bsp_api : public everest::lib::io::event::fd_event_register_interface {
    using tx_ftor = std::function<void(evse_bsp_host_to_cb const&)>;
    using rx_ftor = std::function<void(evse_bsp_cb_to_host const&)>;
    using mqtt_ftor = std::function<void(std::string const&, std::string const&)>;

public:
    evse_bsp_api(evse_bsp_config const& config, std::string const& cb_identifier, evse_bsp_host_to_cb& host_status);
    void set_cb_tx(tx_ftor const& handler);
    void set_cb_message(evse_bsp_cb_to_host const& msg);
    void set_mqtt_tx(mqtt_ftor const& tx);

    bool register_events(everest::lib::io::event::fd_event_handler& handler) override;
    bool unregister_events(everest::lib::io::event::fd_event_handler& handler) override;
    void dispatch(std::string const& operation, std::string const& payload);

    void raise_comm_fault();
    void clear_comm_fault();
    void sync(bool cb_connected);

private:
    void tx(evse_bsp_host_to_cb const& msg);

    void handle_event_cp(std::uint8_t cp);
    void handle_event_relay(std::uint8_t relay);
    void handle_error(const SafetyErrorFlags& data);
    void handle_pp_type1(std::uint8_t data);
    void handle_pp_type2(std::uint8_t data);
    void handle_stop_button(std::uint8_t data);

    void send_event(API_BSP::Event data);
    void send_ac_nr_of_phases(std::uint8_t data);
    void send_capabilities();
    void send_ac_pp_amapcity(API_BSP::Ampacity data);
    void send_request_stop_transaction(API_EVM::StopTransactionReason data);
    void send_rcd_current(std::uint8_t data);
    void send_raise_error(API_BSP::ErrorEnum error, std::string const& subtype, std::string const& msg);
    void send_clear_error(API_BSP::ErrorEnum error, std::string const& subtype, std::string const& msg);
    void send_communication_check();
    void send_reply_reset(std::string const& replyTo);

    void send_mqtt(std::string const& topic, std::string const& message);

    void receive_enable(std::string const& payload);
    void receive_pwm_on(std::string const& payload);
    void receive_cp_state_X1(std::string const& payload);
    void receive_cp_state_F(std::string const& payload);
    void receive_allow_power_on(std::string const& payload);
    void receive_ac_switch_three_phases_while_charging(std::string const& payload);
    void receive_evse_replug(std::string const& payload);
    void receive_ac_overcurrent_limit(std::string const& payload);
    void receive_lock();
    void receive_unlock();
    void receive_self_test(std::string const& payload);
    void receive_request_reset(std::string const& payload);
    void receive_heartbeat(std::string const& pl);

    bool check_everest_heartbeat();
    void handle_everest_connection_state();

    evse_bsp_host_to_cb& host_status;
    evse_bsp_cb_to_host cb_status;

    tx_ftor m_tx;
    everest::lib::io::event::timer_fd m_capabilities_timer;
    API_BSP::HardwareCapabilities m_capabilities;
    bool m_enabled{false};
    bool everest_connected{false};
    bool m_cb_connected{false};
    bool m_bc_initial_comm_check{true};
    std::string m_cb_identifier;
    std::chrono::steady_clock::time_point last_everest_heartbeat;

    mqtt_ftor m_mqtt_tx;
    std::size_t m_last_hb_id{0};
};

} // namespace charge_bridge::evse_bsp
