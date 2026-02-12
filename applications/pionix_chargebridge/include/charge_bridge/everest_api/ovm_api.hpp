// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <chrono>
#include <cstdint>
#include <everest/io/event/fd_event_register_interface.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest_api_types/evse_manager/API.hpp>
#include <everest_api_types/over_voltage_monitor/API.hpp>
#include <everest_api_types/utilities/Topics.hpp>
#include <functional>
#include <protocol/cb_common.h>
#include <protocol/evse_bsp_cb_to_host.h>
#include <protocol/evse_bsp_host_to_cb.h>
#include <string>

namespace charge_bridge::evse_bsp {
namespace API_OVM = everest::lib::API::V1_0::types::over_voltage_monitor;

struct evse_ovm_config {
    bool enabled{false};
    std::string module_id;
};

class ovm_api : public everest::lib::io::event::fd_event_register_interface {
    using tx_ftor = std::function<void(evse_bsp_host_to_cb const&)>;
    using rx_ftor = std::function<void(evse_bsp_cb_to_host const&)>;
    using mqtt_ftor = std::function<void(std::string const&, std::string const&)>;

public:
    ovm_api(evse_ovm_config const& config, std::string const& cb_identifier, evse_bsp_host_to_cb& host_status);
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

    void send_voltage_measurement_V(double data);
    void send_raise_error(API_OVM::ErrorEnum error, std::string const& subtype, std::string const& msg,
                          API_OVM::ErrorSeverityEnum severity);
    void send_clear_error(API_OVM::ErrorEnum error, std::string const& subtype);
    void send_communication_check();

    void send_mqtt(std::string const& topic, std::string const& message);

    void handle_dc_hv_ov_emergency(bool high);
    void handle_dc_hv_ov_error(bool high);
    void handle_cp_state(CpState state);

    void receive_set_limits(std::string const& payload);
    void receive_start();
    void receive_stop();
    void receive_reset_over_voltage_error();
    void receive_heartbeat(std::string const& pl);

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

    API_OVM::OverVoltageLimits m_limits{0, 0};
    mqtt_ftor m_mqtt_tx;
    std::size_t m_last_hb_id{0};
};

} // namespace charge_bridge::evse_bsp
