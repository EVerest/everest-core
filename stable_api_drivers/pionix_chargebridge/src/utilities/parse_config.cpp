// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "charge_bridge/utilities/string.hpp"
#include "protocol/cb_config.h"
#include "protocol/cb_management.h"
#include <charge_bridge/charge_bridge.hpp>
#include <charge_bridge/utilities/parse_config.hpp>
#include <charge_bridge/utilities/type_converters.hpp>
#include <everest_api_types/evse_board_support/API.hpp>
#include <everest_api_types/evse_board_support/codec.hpp>
#include <exception>
#include <iostream>

#include <stdexcept>
#include <string>
#include <yaml-cpp/yaml.h>

using namespace everest::lib::API::V1_0::types;

namespace {
static const int g_cb_port_management = 6000;
static const int g_cb_port_evse_bsp = 6001;
static const int g_cb_port_plc = 6002;
static const int g_cb_port_can0 = 6003;
static const int g_cb_port_serial_1 = 6004;
static const int g_cb_port_serial_2 = 6005;
} // namespace

namespace charge_bridge::utilities {

void parse_config_impl(YAML::Node& config, charge_bridge_config& c) {
    auto get_node = [&config](std::string const& main, std::string const& sub, auto& data) {
        if (config[main][sub]) {
            try {
                data = config[main][sub].as<typename std::remove_reference<decltype(data)>::type>();
                return true;
            } catch (std::exception const& e) {
                std::cerr << "Cannot parse config: " << main << "::" << sub << std::endl;
                std::cerr << e.what() << std::endl;
            }
        } else {
            std::cerr << "Node not found: " << main << "::" << sub << std::endl;
        }
        throw std::runtime_error("");
    };
    auto get_main_node = [&config](std::string const& main, auto& data) {
        if (config[main]) {
            try {
                data = config[main].as<typename std::remove_reference<decltype(data)>::type>();
                return true;
            } catch (std::exception const& e) {
                std::cerr << "Cannot parse config: " << main << std::endl;
                std::cerr << e.what() << std::endl;
            }
        } else {
            std::cerr << "Node not found: " << main << std::endl;
        }
        throw std::runtime_error("");
    };

    auto get_block = [&config, &get_node, &c](std::string const& block, auto& block_cfg, auto const& ftor) {
        bool enable = false;
        if (config[block]) {
            if (config[block]["enable"]) {
                enable = config[block]["enable"].as<bool>();
            } else {
                enable = true;
            }
        }
        if (enable) {
            block_cfg.emplace();
            ftor(*block_cfg, block);
            block_cfg->cb = c.cb_name;
            block_cfg->item = block;
        }
    };

    get_node("charge_bridge", "name", c.cb_name);

    get_node("charge_bridge", "ip", c.cb_remote);

    c.cb_port = g_cb_port_management;

    get_block("can_0", c.can0, [&](auto& cfg, auto const& main) {
        get_node(main, "local", cfg.can_device);
        cfg.cb_port = g_cb_port_can0;
        cfg.cb_remote = c.cb_remote;
    });

    get_block("serial_1", c.serial1, [&](auto& cfg, auto const& main) {
        get_node(main, "local", cfg.serial_device);
        cfg.cb_port = g_cb_port_serial_1;
        cfg.cb_remote = c.cb_remote;
    });

    get_block("serial_2", c.serial2, [&](auto& cfg, auto const& main) {
        get_node(main, "local", cfg.serial_device);
        cfg.cb_port = g_cb_port_serial_2;
        cfg.cb_remote = c.cb_remote;
    });

    // FIXME (JH) serial3 not availabe in first release
    // get_block("serial_3", c.serial3, [&](auto& cfg, auto const& main) {
    //     get_node(main, "local", cfg.serial_device);
    //     get_node(main, "port", cfg.cb_port);
    //     cfg.cb_remote = c.cb_remote;
    // });

    get_block("plc", c.plc, [&](auto& cfg, auto const& main) {
        get_node(main, "tap", cfg.plc_tap);
        get_node(main, "ip", cfg.plc_ip);
        get_node(main, "netmask", cfg.plc_netmaks);
        get_node(main, "mtu", cfg.plc_mtu);
        cfg.cb_port = g_cb_port_plc;
        cfg.cb_remote = c.cb_remote;
    });

    get_block("evse_bsp", c.evse, [&](auto& cfg, auto const& main) {
        cfg.cb_port = g_cb_port_evse_bsp;
        get_node(main, "module_id", cfg.api.bsp.module_id);
        get_node(main, "mqtt_remote", cfg.api.mqtt_remote);
        get_node(main, "mqtt_port", cfg.api.mqtt_port);
        get_node(main, "mqtt_ping_interval_ms", cfg.api.mqtt_ping_interval_ms);
        cfg.cb_remote = c.cb_remote;
        get_node(main, "capabilities", cfg.api.bsp.capabilities);
        get_node(main, "ovm_enabled", cfg.api.ovm.enabled);
        get_node(main, "ovm_module_id", cfg.api.ovm.module_id);
    });

    get_block("gpio", c.gpio, [&](auto& cfg, auto const& main) {
        get_node(main, "interval_s", cfg.interval_s);
        get_node(main, "mqtt_remote", cfg.mqtt_remote);
        get_node(main, "mqtt_port", cfg.mqtt_port);
        get_node(main, "mqtt_ping_interval_ms", cfg.mqtt_ping_interval_ms);
        cfg.cb_remote = c.cb_remote;
        cfg.cb_port = c.cb_port;
    });

    get_block("heartbeat", c.heartbeat, [&](auto& cfg, auto const& main) {
        get_node(main, "interval_s", cfg.interval_s);
        cfg.cb_remote = c.cb_remote;
        cfg.cb_port = c.cb_port;
        get_main_node("charge_bridge", cfg.cb_config.network);
        get_main_node("safety", cfg.cb_config.safety);
        std::memset(cfg.cb_config.gpios, 0, CB_NUMBER_OF_GPIOS * sizeof(CbGpioConfig));
        std::memset(cfg.cb_config.uarts, 0, CB_NUMBER_OF_UARTS * sizeof(CbUartConfig));
        if (c.serial1) {
            get_main_node("serial_1", cfg.cb_config.uarts[0]);
        }
        if (c.serial2) {
            get_main_node("serial_2", cfg.cb_config.uarts[1]);
        }
        // FIXME (JH) serial 3 not available in first release
        // if (c.serial3) {
        //     get_main_node("serial_3", cfg.cb_config.uarts[2]);
        // }
        if (c.gpio) {
            for (auto i = 0; i < CB_NUMBER_OF_GPIOS; ++i) {
                get_node("gpio", "gpio_" + std::to_string(i), cfg.cb_config.gpios[i]);
            }
        }
        if (c.can0) {
            get_main_node("can_0", cfg.cb_config.can);
        }
        get_node("plc", "powersaving_mode", cfg.cb_config.plc_powersaving_mode);
        cfg.cb_config.config_version = CB_CONFIG_VERSION;
    });

    get_node("charge_bridge", "fw_file", c.firmware.fw_path);
    get_node("charge_bridge", "fw_update_on_start", c.firmware.fw_update_on_start);

    c.firmware.cb_remote = c.cb_remote;
    c.firmware.cb_port = c.cb_port;
    c.firmware.cb = c.cb_name;
}

charge_bridge_config set_config_placeholders(charge_bridge_config const& src, charge_bridge_config& result,
                                             std::string const& ip, size_t index) {
    auto index_str = std::to_string(index);
    result = src;
    auto replace = [index_str](std::string& src) { replace_all_in_place(src, "##", index_str); };

    result.cb_remote = ip;
    result.firmware.cb_remote = ip;
    replace(result.cb_name);
    result.firmware.cb = result.cb_name;
    if (result.can0.has_value()) {
        result.can0->cb_remote = ip;
        result.can0->cb = result.cb_name;
        replace(result.can0->can_device);
    }
    if (result.serial1.has_value()) {
        result.serial1->cb_remote = ip;
        result.serial1->cb = result.cb_name;
        replace(result.serial1->serial_device);
    }
    if (result.serial2.has_value()) {
        result.serial2->cb_remote = ip;
        result.serial2->cb = result.cb_name;
        replace(result.serial2->serial_device);
    }
    if (result.serial3.has_value()) {
        result.serial3->cb_remote = ip;
        result.serial3->cb = result.cb_name;
        replace(result.serial3->serial_device);
    }
    if (result.plc.has_value()) {
        result.plc->cb_remote = ip;
        result.plc->cb = result.cb_name;
        replace(result.plc->plc_tap);
    }
    if (result.evse.has_value()) {
        result.evse->cb_remote = ip;
        result.evse->cb = result.cb_name;
        replace(result.evse->api.bsp.module_id);
    }
    if (result.heartbeat.has_value()) {
        result.heartbeat->cb = result.cb_name;
        result.heartbeat->cb_remote = ip;
    }
    if (result.gpio.has_value()) {
        result.gpio->cb = result.cb_name;
        result.gpio->cb_remote = ip;
    }

    if (result.heartbeat.has_value()) {
        auto& raw = result.heartbeat->cb_config.network.mdns_name;
        std::string item = raw;
        replace(item);
        auto limit = sizeof(raw);
        if (item.size() > limit) {
            item = "cb_" + index_str;
            std::cout << "WARNING: Replacement for mdns_name is too long. Fallback to '" + item + "'" << std::endl;
        }
        std::memset(raw, 0, limit);
        std::memcpy(raw, item.c_str(), std::min(item.size(), limit));

        result.heartbeat->cb_remote = ip;
        result.heartbeat->cb = result.cb_name;
    }

    return result;
}

std::vector<charge_bridge_config> parse_config_multi(std::string const& config_file) {
    try {
        YAML::Node config = YAML::LoadFile(config_file);
        if (not config) {
            std::cerr << "Config file not found: " << config_file << std::endl;
            return {};
        }
        charge_bridge_config base_config;
        parse_config_impl(config, base_config);

        if (not config["charge_bridge_ip_list"]) {
            return {base_config};
        }
        auto ip_list = config["charge_bridge_ip_list"].as<std::vector<std::string>>();
        std::vector<charge_bridge_config> cb_config_list(ip_list.size());

        for (size_t i = 0; i < ip_list.size(); ++i) {
            set_config_placeholders(base_config, cb_config_list[i], ip_list[i], i);
        }

        return cb_config_list;
    } catch (...) {
        std::cerr << "FAILED to parse configuration!" << std::endl;
    }
    return {};
}

bool parse_config(std::string const& config_file, charge_bridge_config& c) {
    try {
        YAML::Node config = YAML::LoadFile(config_file);
        if (not config) {
            std::cerr << "Config file not found: " << config_file << std::endl;
            return false;
        }

        parse_config_impl(config, c);
        return true;
    } catch (...) {
        std::cerr << "FAILED to parse configuration!" << std::endl;
    }
    return false;
}

} // namespace charge_bridge::utilities
