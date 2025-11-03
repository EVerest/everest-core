// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "charge_bridge/gpio_bridge.hpp"
#include "charge_bridge/heartbeat_service.hpp"
#include "protocol/cb_config.h"
#include <charge_bridge/charge_bridge.hpp>
#include <charge_bridge/firmware_update/sync_fw_updater.hpp>
#include <charge_bridge/utilities/print_config.hpp>
#include <charge_bridge/utilities/sync_udp_client.hpp>
#include <everest/io/event/fd_event_sync_interface.hpp>
#include <everest/io/netlink/vcan_netlink_manager.hpp>

#include <iostream>

namespace charge_bridge {

charge_bridge::charge_bridge(charge_bridge_config const& config) : m_config(config) {
    if (config.can0.has_value()) {
        can_0_client = std::make_unique<can_bridge>(config.can0.value());
    }
    if (config.serial1.has_value()) {
        pty_1 = std::make_unique<serial_bridge>(config.serial1.value());
    }
    if (config.serial2.has_value()) {
        pty_2 = std::make_unique<serial_bridge>(config.serial2.value());
    }
    if (config.serial3.has_value()) {
        pty_3 = std::make_unique<serial_bridge>(config.serial3.value());
    }
    if (config.plc.has_value()) {
        plc = std::make_unique<plc_bridge>(config.plc.value());
    }
    if (config.evse.has_value()) {
        bsp = std::make_unique<evse_bridge>(config.evse.value());
    }
    if (config.heartbeat.has_value()) {
        heartbeat = std::make_unique<heartbeat_service>(config.heartbeat.value());
    }
    if (config.gpio.has_value()) {
        gpio = std::make_unique<gpio_bridge>(config.gpio.value());
    }
}

charge_bridge::~charge_bridge() {
}

bool charge_bridge::update_firmware(bool force) {
    firmware_update::sync_fw_updater updater(m_config.firmware);
    auto is_connected = updater.quick_check_connection();
    if (not is_connected) {
        return false;
    }
    updater.print_fw_version();

    auto do_update = force or (m_config.firmware.fw_update_on_start and not updater.check_if_correct_fw_installed());

    if (not do_update) {
        return true;
    }
    auto result = updater.upload_fw() && updater.check_connection();
    if (not result) {
        std::cout << "Error: could not install correct firmware version" << std::endl;
    }
    return result;
}

std::string charge_bridge::get_pty_1_slave_path() {
    if (pty_1) {
        return pty_1->get_slave_path();
    }
    return "";
}

std::string charge_bridge::get_pty_2_slave_path() {
    if (pty_2) {
        return pty_2->get_slave_path();
    }
    return "";
}

std::string charge_bridge::get_pty_3_slave_path() {
    if (pty_3) {
        return pty_3->get_slave_path();
    }
    return "";
}

bool charge_bridge::register_events(everest::lib::io::event::fd_event_handler& handler) {
    auto result = true;
    if (can_0_client) {
        result = handler.register_event_handler(can_0_client.get()) && result;
    }
    if (pty_1) {
        result = handler.register_event_handler(pty_1.get()) && result;
    }
    if (pty_2) {
        result = handler.register_event_handler(pty_2.get()) && result;
    }
    if (pty_3) {
        result = handler.register_event_handler(pty_3.get()) && result;
    }
    if (bsp) {
        result = handler.register_event_handler(bsp.get()) && result;
    }
    if (plc) {
        result = handler.register_event_handler(plc.get()) && result;
    }
    if (heartbeat) {
        result = handler.register_event_handler(heartbeat.get()) && result;
    }
    if (gpio) {
        result = handler.register_event_handler(gpio.get()) && result;
    }
    return result;
}
bool charge_bridge::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    auto result = true;
    if (can_0_client) {
        result = handler.unregister_event_handler(can_0_client.get()) && result;
    }
    if (pty_1) {
        result = handler.unregister_event_handler(pty_1.get()) && result;
    }
    if (pty_2) {
        result = handler.unregister_event_handler(pty_2.get()) && result;
    }
    if (pty_3) {
        result = handler.unregister_event_handler(pty_3.get()) && result;
    }
    if (bsp) {
        result = handler.unregister_event_handler(bsp.get()) && result;
    }
    if (plc) {
        result = handler.unregister_event_handler(plc.get()) && result;
    }
    if (heartbeat) {
        result = handler.unregister_event_handler(heartbeat.get()) && result;
    }
    if (gpio) {
        result = handler.unregister_event_handler(gpio.get()) && result;
    }
    return result;
}

void charge_bridge::print_config() {
    print_charge_bridge_config(m_config);
}

void print_charge_bridge_config(charge_bridge_config const& c) {
    using namespace utilities;
    std::cout << "ChargeBridge: " << c.cb_name << std::endl;
    std::cout << " * remote:    " << c.cb_remote << std::endl;
    if (c.serial1) {
        std::cout << " * serial 1:  " << c.serial1->serial_device;
        if (c.heartbeat.has_value() && CB_NUMBER_OF_UARTS >= 1) {
            std::cout << " " << to_string(c.heartbeat->cb_config.uarts[0]);
        }
        std::cout << std::endl;
    }
    if (c.serial2) {
        std::cout << " * serial 2:  " << c.serial2->serial_device;
        if (c.heartbeat.has_value() && CB_NUMBER_OF_UARTS >= 2) {
            std::cout << " " << to_string(c.heartbeat->cb_config.uarts[1]);
        }
        std::cout << std::endl;
    }
    if (c.serial3) {
        std::cout << " * serial 3:  " << c.serial3->serial_device;
        if (c.heartbeat.has_value() && CB_NUMBER_OF_UARTS >= 3) {
            std::cout << " " << to_string(c.heartbeat->cb_config.uarts[2]);
        }
        std::cout << std::endl;
    }
    if (c.can0) {
        std::cout << " * can 0:     " << c.can0->can_device;
        if (c.heartbeat.has_value()) {
            std::cout << " " << to_string(c.heartbeat->cb_config.can.baudrate) << "bps" << std::endl;
        }
    }
    if (c.plc) {
        std::cout << " * plc:       " << c.plc->plc_tap << std::flush;
        std::cout << " " << c.cb_remote << ":" << c.plc->cb_port;
        std::cout << " adress " << c.plc->plc_ip;
        std::cout << " netmask " << c.plc->plc_netmaks;
        std::cout << " MTU " << c.plc->plc_mtu << std::endl;
    }
    if (c.evse) {
        std::cout << " * evse_bsp:  " << c.evse->cb_remote << ":" << c.evse->cb_port;
        std::cout << " module " << c.evse->api.bsp.module_id;
        std::cout << " MQTT " << c.evse->api.mqtt_remote << ":" << c.evse->api.mqtt_port;
        std::cout << " ping " << c.evse->api.mqtt_ping_interval_ms << "ms";
        if (c.evse->api.ovm.enabled) {
            std::cout << " OVM module " << c.evse->api.ovm.module_id;
        }
        std::cout << std::endl;
    }
    if (c.heartbeat) {
        std::cout << " * heartbeat: " << c.cb_remote << ":" << c.cb_port;
        std::cout << " heartbeat interval " << c.heartbeat->interval_s << "s" << std::endl;
    }
    if (c.gpio) {
        std::cout << " * gpio:      " << c.cb_remote << ":" << c.cb_port;
        std::cout << " send interval " << c.gpio->interval_s << "s" << std::endl;
    }

    std::cout << "\n" << std::endl;
}

} // namespace charge_bridge
