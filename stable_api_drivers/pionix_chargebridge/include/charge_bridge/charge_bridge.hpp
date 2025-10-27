// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <charge_bridge/can_bridge.hpp>
#include <charge_bridge/evse_bridge.hpp>
#include <charge_bridge/firmware_update/sync_fw_updater.hpp>
#include <charge_bridge/gpio_bridge.hpp>
#include <charge_bridge/heartbeat_service.hpp>
#include <charge_bridge/plc_bridge.hpp>
#include <charge_bridge/serial_bridge.hpp>
#include <charge_bridge/utilities/symlink.hpp>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/serial/event_pty.hpp>
#include <everest/io/tun_tap/tap_client.hpp>
#include <memory>
#include <optional>

namespace charge_bridge {

struct charge_bridge_config {
    std::string cb_name;
    uint16_t cb_port;
    std::string cb_remote;
    std::optional<can_bridge_config> can0;
    std::optional<serial_bridge_config> serial1;
    std::optional<serial_bridge_config> serial2;
    std::optional<serial_bridge_config> serial3;
    std::optional<plc_bridge_config> plc;
    std::optional<evse_bridge_config> evse;
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

private:
    std::unique_ptr<can_bridge> can_0_client;
    std::unique_ptr<serial_bridge> pty_1;
    std::unique_ptr<serial_bridge> pty_2;
    std::unique_ptr<serial_bridge> pty_3;
    std::unique_ptr<evse_bridge> bsp;
    std::unique_ptr<plc_bridge> plc;
    std::unique_ptr<heartbeat_service> heartbeat;
    std::unique_ptr<gpio_bridge> gpio;

    const charge_bridge_config m_config;
};

} // namespace charge_bridge
