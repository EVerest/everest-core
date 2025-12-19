// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <atomic>
#include <charge_bridge/bsp_bridge.hpp>
#include <charge_bridge/can_bridge.hpp>
#include <charge_bridge/discovery.hpp>
#include <charge_bridge/firmware_update/sync_fw_updater.hpp>
#include <charge_bridge/gpio_bridge.hpp>
#include <charge_bridge/heartbeat_service.hpp>
#include <charge_bridge/plc_bridge.hpp>
#include <charge_bridge/serial_bridge.hpp>
#include <charge_bridge/utilities/symlink.hpp>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/serial/event_pty.hpp>
#include <everest/io/tun_tap/tap_client.hpp>
#include <everest/util/async/monitor.hpp>

#include <memory>
#include <optional>

namespace charge_bridge {

struct charge_bridge_status {
    bool is_connected{false};
    bool discovery_pending{false};
};

struct charge_bridge_config {
    std::string cb_name;
    std::uint16_t cb_port;
    std::string cb_remote;
    std::optional<can_bridge_config> can0;
    std::optional<serial_bridge_config> serial1;
    std::optional<serial_bridge_config> serial2;
    std::optional<serial_bridge_config> serial3;
    std::optional<plc_bridge_config> plc;
    std::optional<bsp_bridge_config> bsp;
    std::optional<heartbeat_config> heartbeat;
    std::optional<gpio_config> gpio;
    firmware_update::fw_update_config firmware;
};

void print_charge_bridge_config(charge_bridge_config const& config);

class charge_bridge : public everest::lib::io::event::fd_event_register_interface {
public:
    charge_bridge(charge_bridge_config const& config);
    ~charge_bridge();

    bool update_firmware(bool force);

    std::string get_pty_1_slave_path();
    std::string get_pty_2_slave_path();
    std::string get_pty_3_slave_path();

    void print_config();

    bool register_events(everest::lib::io::event::fd_event_handler& handler) override;
    bool unregister_events(everest::lib::io::event::fd_event_handler& handler) override;

    void manage(everest::lib::io::event::fd_event_handler& handler, std::atomic_bool const& exit, bool force_update);

private:
    void init();
    void init_discovery(discovery_device_type type, std::set<std::string> const& interfaces, bool excluding);
    void handle_discovery(std::string const& ip);

private:
    std::unique_ptr<can_bridge> m_can_0_client;
    std::unique_ptr<serial_bridge> m_pty_1;
    std::unique_ptr<serial_bridge> m_pty_2;
    std::unique_ptr<serial_bridge> m_pty_3;
    std::unique_ptr<bsp_bridge> m_bsp;
    std::unique_ptr<plc_bridge> m_plc;
    std::unique_ptr<heartbeat_service> m_heartbeat;
    std::unique_ptr<gpio_bridge> m_gpio;
    std::unique_ptr<discovery> m_discovery;

    everest::lib::io::event::fd_event_handler* m_event_handler{nullptr};
    bool m_force_firmware_update{false};
    everest::lib::util::monitor<charge_bridge_status> m_cb_status;
    bool m_was_connected{false};
    bool m_discovery_active{false};

    charge_bridge_config m_config;
};

} // namespace charge_bridge
