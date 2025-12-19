// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <chrono>
#include <cstdint>
#include <everest/io/event/fd_event_register_interface.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest_api_types/ev_board_support/API.hpp>
#include <everest_api_types/evse_board_support/API.hpp>
#include <everest_api_types/evse_manager/API.hpp>
#include <everest_api_types/generic/API.hpp>
#include <everest_api_types/utilities/Topics.hpp>
#include <functional>
#include <protocol/cb_common.h>
#include <protocol/evse_bsp_cb_to_host.h>
#include <protocol/evse_bsp_host_to_cb.h>
#include <string>

namespace charge_bridge::evse_bsp {
namespace API_EVSE_BSP = everest::lib::API::V1_0::types::evse_board_support;
namespace API_EV_BSP = everest::lib::API::V1_0::types::ev_board_support;
namespace API_EVM = everest::lib::API::V1_0::types::evse_manager;
namespace API_GENERIC = everest::lib::API::V1_0::types::generic;
// namespace API_OVM = everest::lib::API::V1_0::types::over_voltage_monitor;

struct evse_ev_bsp_config {
    bool enabled{false};
    std::string module_id;
};

class ev_bsp_api : public everest::lib::io::event::fd_event_register_interface {
    using tx_ftor = std::function<void(evse_bsp_host_to_cb const&)>;
    using rx_ftor = std::function<void(evse_bsp_cb_to_host const&)>;
    using mqtt_ftor = std::function<void(std::string const&, std::string const&)>;

public:
    ev_bsp_api(evse_ev_bsp_config const& config, std::string const& cb_identifier, evse_bsp_host_to_cb& host_status);
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

    void send_bsp_event(API_EVSE_BSP::Event data);
    void send_bsp_measurement(API_EV_BSP::BspMeasurement data);
    void send_ev_info(API_EVM::EVInfo data);

    void send_raise_error(API_GENERIC::ErrorEnum error, std::string const& subtype, std::string const& msg);
    void send_clear_error(API_GENERIC::ErrorEnum error, std::string const& subtype);

    void send_communication_check();

    void send_mqtt(std::string const& topic, std::string const& message);

    void send_event(API_EVSE_BSP::Event data);

    void receive_enable(std::string const& payload);
    void receive_set_cp_state(std::string const& payload);
    void receive_allow_power_on(std::string const& payload);
    void receive_diode_fail(std::string const& payload);
    void receive_set_ac_max_current(std::string const& payload);
    void receive_set_three_phases(std::string const& payload);
    void receive_set_rcd_error(std::string const& payload);
    void receive_heartbeat(std::string const& pl);

    void handle_error(const SafetyErrorFlags& data);
    void handle_event_cp(std::uint8_t cp);
    void handle_event_relay(std::uint8_t relay);
    void handle_bsp_measurement(uint16_t cp, uint8_t pp_1, uint8_t pp2);

    bool check_everest_heartbeat();
    void handle_everest_connection_state();

    evse_bsp_host_to_cb& host_status;
    evse_bsp_cb_to_host m_cb_status;

    tx_ftor m_tx;
    bool m_everest_connected{false};
    bool m_cb_connected{false};
    bool m_cb_initial_comm_check{true};
    bool m_bc_initial_comm_check{true};
    std::string m_cb_identifier;
    std::chrono::steady_clock::time_point last_everest_heartbeat;

    mqtt_ftor m_mqtt_tx;
    std::size_t m_last_hb_id{0};
    everest::lib::API::V1_0::types::evse_board_support::Event last_cp_event{
        everest::lib::API::V1_0::types::evse_board_support::Event::Disconnected};
};

} // namespace charge_bridge::evse_bsp
