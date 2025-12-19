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
#include <thread>

namespace charge_bridge {

charge_bridge::charge_bridge(charge_bridge_config const& config) : m_config(config) {
    if (config.can0.has_value()) {
        m_can_0_client = std::make_unique<can_bridge>(config.can0.value());
    }
    if (config.serial1.has_value()) {
        m_pty_1 = std::make_unique<serial_bridge>(config.serial1.value());
    }
    if (config.serial2.has_value()) {
        m_pty_2 = std::make_unique<serial_bridge>(config.serial2.value());
    }
    if (config.serial3.has_value()) {
        m_pty_3 = std::make_unique<serial_bridge>(config.serial3.value());
    }
    if (config.plc.has_value()) {
        m_plc = std::make_unique<plc_bridge>(config.plc.value());
    }
    if (config.bsp.has_value()) {
        m_bsp = std::make_unique<bsp_bridge>(config.bsp.value());
    }
    if (config.heartbeat.has_value()) {
        m_heartbeat = std::make_unique<heartbeat_service>(config.heartbeat.value(), [this](bool connected) {
            {
                auto handle = m_is_connected.handle();
                *handle = connected;
            }
            m_is_connected.notify_one();
        });
    }
    if (config.gpio.has_value()) {
        m_gpio = std::make_unique<gpio_bridge>(config.gpio.value());
    }
}

charge_bridge::~charge_bridge() {
    m_is_connected.notify_one();
}

void charge_bridge::manage(everest::lib::io::event::fd_event_handler& handler, std::atomic_bool const& run,
                           bool force_update) {
    using namespace std::chrono_literals;
    m_event_handler = &handler;
    m_force_firmware_update = force_update;

    auto action = [this](bool is_connected, int& error_count) {
        if (m_was_connected and not is_connected) {
            if (error_count > 1) {
                m_event_handler->add_action([this]() { unregister_events(*m_event_handler); });
                m_was_connected = false;
            } else {
                error_count++;
            }
        }
        if (not m_was_connected) {
            if (update_firmware(m_force_firmware_update)) {
                m_event_handler->add_action([this]() { register_events(*m_event_handler); });
                m_was_connected = true;
                error_count = 0;
            }
        }
    };

    std::thread manager([&run, action, this]() {
        auto handle = m_is_connected.handle();
        bool last_is_connected = *handle;
        int error_count = 0;
        while (run.load()) {
            action(*handle, error_count);
            handle.wait_for([&] { return (*handle not_eq last_is_connected) or not run.load(); }, 10s);
            last_is_connected = *handle;
        }
    });
    manager.detach();
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
    if (m_pty_1) {
        return m_pty_1->get_slave_path();
    }
    return "";
}

std::string charge_bridge::get_pty_2_slave_path() {
    if (m_pty_2) {
        return m_pty_2->get_slave_path();
    }
    return "";
}

std::string charge_bridge::get_pty_3_slave_path() {
    if (m_pty_3) {
        return m_pty_3->get_slave_path();
    }
    return "";
}

bool charge_bridge::register_events(everest::lib::io::event::fd_event_handler& handler) {
    auto result = true;
    if (m_can_0_client) {
        result = handler.register_event_handler(m_can_0_client.get()) && result;
    }
    if (m_pty_1) {
        result = handler.register_event_handler(m_pty_1.get()) && result;
    }
    if (m_pty_2) {
        result = handler.register_event_handler(m_pty_2.get()) && result;
    }
    if (m_pty_3) {
        result = handler.register_event_handler(m_pty_3.get()) && result;
    }
    if (m_bsp) {
        result = handler.register_event_handler(m_bsp.get()) && result;
    }
    if (m_plc) {
        result = handler.register_event_handler(m_plc.get()) && result;
    }
    if (m_heartbeat) {
        result = handler.register_event_handler(m_heartbeat.get()) && result;
    }
    if (m_gpio) {
        result = handler.register_event_handler(m_gpio.get()) && result;
    }
    return result;
}
bool charge_bridge::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    auto result = true;
    if (m_can_0_client) {
        result = handler.unregister_event_handler(m_can_0_client.get()) && result;
    }
    if (m_pty_1) {
        result = handler.unregister_event_handler(m_pty_1.get()) && result;
    }
    if (m_pty_2) {
        result = handler.unregister_event_handler(m_pty_2.get()) && result;
    }
    if (m_pty_3) {
        result = handler.unregister_event_handler(m_pty_3.get()) && result;
    }
    if (m_bsp) {
        result = handler.unregister_event_handler(m_bsp.get()) && result;
    }
    if (m_plc) {
        result = handler.unregister_event_handler(m_plc.get()) && result;
    }
    if (m_heartbeat) {
        result = handler.unregister_event_handler(m_heartbeat.get()) && result;
    }
    if (m_gpio) {
        result = handler.unregister_event_handler(m_gpio.get()) && result;
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
    if (c.bsp) {
        if (c.bsp->api.evse.enabled) {
            std::cout << " * evse_bsp:  ";
        } else if (c.bsp->api.ev.enabled) {
            std::cout << " * ev_bsp:    ";
        }
        std::cout << c.bsp->cb_remote << ":" << c.bsp->cb_port;
        std::cout << " module " << c.bsp->api.evse.module_id;
        std::cout << " MQTT " << c.bsp->api.mqtt_remote << ":" << c.bsp->api.mqtt_port;
        if (not c.bsp->api.mqtt_bind.empty()) {
            std::cout << " on " << c.bsp->api.mqtt_bind;
        }
        std::cout << " ping " << c.bsp->api.mqtt_ping_interval_ms << "ms";
        if (c.bsp->api.ovm.enabled) {
            std::cout << " OVM module " << c.bsp->api.ovm.module_id;
        }
        std::cout << std::endl;
    }
    if (c.heartbeat) {
        std::cout << " * heartbeat: " << c.cb_remote << ":" << c.cb_port;
        std::cout << " heartbeat interval " << c.heartbeat->interval_s << "s" << std::endl;
    }
    if (c.gpio) {
        std::cout << " * gpio:      " << c.cb_remote << ":" << c.cb_port;
        std::cout << " MQTT " << c.gpio->mqtt_remote << ":" << c.gpio->mqtt_port;
        if (not c.gpio->mqtt_bind.empty()) {
            std::cout << " on " << c.gpio->mqtt_bind;
        }
        std::cout << " send interval " << c.gpio->interval_s << "s" << std::endl;
    }

    std::cout << "\n" << std::endl;
}

} // namespace charge_bridge
