// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "c4/yml/node.hpp"
#include <charge_bridge/utilities/parse_config.hpp>
#include <charge_bridge/utilities/string.hpp>
#include <charge_bridge/utilities/type_converters.hpp>
#include <everest_api_types/evse_board_support/API.hpp>
#include <everest_api_types/evse_board_support/codec.hpp>
#include <iostream>

#include <filesystem>
// clang-format off
#include <ryml_std.hpp>
#include <ryml.hpp>
// clang-format on
using namespace everest::lib::API::V1_0::types;

namespace {
static const int g_cb_port_management = 6000;
static const int g_cb_port_evse_bsp = 6001;
static const int g_cb_port_plc = 6002;
static const int g_cb_port_can0 = 6003;
static const int g_cb_port_serial_1 = 6004;
static const int g_cb_port_serial_2 = 6005;
static const std::uint16_t default_mqtt_ping_interval_ms = 1000;

std::string print_yaml_location(ryml::Location const& loc) {
    std::stringstream error_msg;

    if (loc) {
        if (not loc.name.empty()) {
            auto tmp = std::string(loc.name.str, loc.name.len);
            if (charge_bridge::utilities::string_ends_with(tmp, ".hpp")) {
                return "";
            }
            error_msg << "\n  file ";
            error_msg << tmp;
        }
        error_msg << "\n  line " << loc.line;
        if (loc.col) {
            error_msg << " column " << loc.col;
        }
        if (loc.offset) {
            error_msg << " offset " << loc.offset << "B";
        }
        error_msg << "\n";
    }
    return error_msg.str();
}

void yaml_error_handler(const char* msg, std::size_t len, ryml::Location loc, void*) {
    std::stringstream error_msg;
    error_msg << "YAML parsing error: ";
    error_msg << print_yaml_location(loc);
    error_msg.write(msg, len);

    std::cerr << error_msg.str() << std::endl;
    throw std::runtime_error(error_msg.str());
}

void print_location(ryml::ConstNodeRef node, ryml::Parser& parser) {
    std::cerr << print_yaml_location(node.location(parser)) << std::endl;
}

void load_yaml_file(const std::string& filename, ryml::Parser* parser, ryml::Tree* t) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_content = buffer.str();

    parse_in_arena(parser, ryml::to_csubstr(filename), ryml::to_csubstr(file_content), t);
}

template <class T> c4::yml::ConstNodeRef decode(c4::yml::ConstNodeRef const& node, T& rhs) {
    using namespace charge_bridge::utilities;
    node >> rhs;
    return node;
}

std::pair<std::string, c4::yml::ConstNodeRef> find_node(c4::yml::NodeRef& config, std::string const& main,
                                                        std::string const& sub) {
    auto main_str = ryml::to_csubstr(main);
    auto node_str = main;
    c4::yml::ConstNodeRef node;
    if (not sub.empty()) {
        node_str = node_str + "::" + sub;
        auto sub_str = ryml::to_csubstr(sub);
        node = config.find_child(main_str);
        if (not node.invalid()) {
            node = config.find_child(main_str).find_child(sub_str);
        }
    } else {
        node = config[main_str];
    }
    return {node_str, node};
}

template <class DataT>
bool get_node_impl(c4::yml::ConstNodeRef node, ryml::Parser& parser, std::string const& node_str, DataT& data) {
    if (node.invalid()) {
        std::cerr << "Node not found: " << node_str << std::endl;
        throw std::runtime_error("");
    }
    try {
        decode(node, data);
        return true;
    } catch (std::exception const& e) {
        std::cerr << "Cannot parse config: " << node_str << std::endl;
        std::cerr << e.what() << std::endl;
    } catch (charge_bridge::utilities::yml_node_error const& e) {
        std::cerr << "Error source: \n"
                  << "  parent " << node_str << "\n"
                  << "  data " << e.m_msg << std::flush;
        print_location(e.m_node, parser);
    }
    throw std::runtime_error("");
}

struct RymlCallbackInitializer {
    RymlCallbackInitializer() {
        ryml::set_callbacks({nullptr, nullptr, nullptr, yaml_error_handler});
    }
};

} // namespace

namespace charge_bridge::utilities {

void parse_config_impl(c4::yml::NodeRef& config, charge_bridge_config& c, std::filesystem::path const& config_path,
                       ryml::Parser& parser) {
    auto get_node = [&config, &parser](auto& data, std::string const& main, std::string const& sub = "") {
        auto [node_str, node] = find_node(config, main, sub);
        get_node_impl(node, parser, node_str, data);
    };

    auto get_node_or_default = [&get_node, &config](auto& data, std::string const& main, std::string const& sub,
                                                    auto fallback) {
        auto [node_str, node] = find_node(config, main, sub);
        if (node.invalid()) {
            data = fallback;
            return;
        }
        try {
            get_node(data, main, sub);
        } catch (...) {
            data = fallback;
        }
    };

    auto get_block = [&config, &c](std::string const& block, auto& block_cfg, auto const& ftor) {
        bool enable = false;
        auto block_str = ryml::to_csubstr(block);
        if (not config.find_child(block_str).invalid()) {
            if (config[block_str].find_child("enable").invalid()) {
                enable = true;
            } else {
                decode(config[block_str]["enable"], enable);
            }
        }
        if (enable) {
            block_cfg.emplace();
            ftor(*block_cfg, block);
            block_cfg->cb = c.cb_name;
            block_cfg->item = block;
        }
    };

    get_node(c.cb_name, "charge_bridge", "name");

    get_node(c.cb_remote, "charge_bridge", "ip");

    c.cb_port = g_cb_port_management;

    get_block("can_0", c.can0, [&](auto& cfg, auto const& main) {
        get_node(cfg.can_device, main, "local");
        cfg.cb_port = g_cb_port_can0;
        cfg.cb_remote = c.cb_remote;
    });

    get_block("serial_1", c.serial1, [&](auto& cfg, auto const& main) {
        get_node(cfg.serial_device, main, "local");
        cfg.cb_port = g_cb_port_serial_1;
        cfg.cb_remote = c.cb_remote;
    });

    get_block("serial_2", c.serial2, [&](auto& cfg, auto const& main) {
        get_node(cfg.serial_device, main, "local");
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
        get_node(cfg.plc_tap, main, "tap");
        get_node(cfg.plc_ip, main, "ip");
        get_node(cfg.plc_netmaks, main, "netmask");
        get_node(cfg.plc_mtu, main, "mtu");
        cfg.cb_port = g_cb_port_plc;
        cfg.cb_remote = c.cb_remote;
    });

    {
        bool wants_ev = false;
        bool wants_evse = false;
        get_node_or_default(wants_ev, "ev_bsp", "enable", false);
        get_node_or_default(wants_evse, "evse_bsp", "enable", false);
        if (wants_ev && wants_evse) {
            std::cerr << "Configuration error: Cannot enable EVSE and EV BSP at the same time" << std::endl;
            throw std::exception();
        }
    }

    get_block("evse_bsp", c.bsp, [&](auto& cfg, auto const& main) {
        cfg.cb_port = g_cb_port_evse_bsp;
        cfg.api.evse.enabled = true;
        get_node(cfg.api.evse.module_id, main, "module_id");
        get_node(cfg.api.mqtt_remote, main, "mqtt_remote");
        get_node_or_default(cfg.api.mqtt_bind, main, "mqtt_bind", "");
        get_node(cfg.api.mqtt_port, main, "mqtt_port");
        get_node_or_default(cfg.api.mqtt_ping_interval_ms, main, "mqtt_ping_interval_ms",
                            default_mqtt_ping_interval_ms);
        cfg.cb_remote = c.cb_remote;
        get_node(cfg.api.evse.capabilities, main, "capabilities");
        get_node(cfg.api.ovm.enabled, main, "ovm_enabled");
        get_node(cfg.api.ovm.module_id, main, "ovm_module_id");
    });

    if (not c.bsp.has_value()) {
        get_block("ev_bsp", c.bsp, [&](auto& cfg, auto const& main) {
            cfg.cb_port = g_cb_port_evse_bsp;
            cfg.api.ev.enabled = true;
            get_node(cfg.api.ev.module_id, main, "module_id");
            get_node(cfg.api.mqtt_remote, main, "mqtt_remote");
            get_node_or_default(cfg.api.mqtt_bind, main, "mqtt_bind", "");
            get_node(cfg.api.mqtt_port, main, "mqtt_port");
            get_node_or_default(cfg.api.mqtt_ping_interval_ms, main, "mqtt_ping_interval_ms",
                                default_mqtt_ping_interval_ms);
            cfg.cb_remote = c.cb_remote;
            get_node(cfg.api.ovm.enabled, main, "ovm_enabled");
            get_node(cfg.api.ovm.module_id, main, "ovm_module_id");
        });
    }

    get_block("gpio", c.gpio, [&](auto& cfg, auto const& main) {
        get_node(cfg.interval_s, main, "interval_s");
        get_node(cfg.mqtt_remote, main, "mqtt_remote");
        get_node_or_default(cfg.mqtt_bind, main, "mqtt_bind", "");
        get_node(cfg.mqtt_port, main, "mqtt_port");
        get_node_or_default(cfg.mqtt_ping_interval_ms, main, "mqtt_ping_interval_ms", default_mqtt_ping_interval_ms);
        cfg.cb_remote = c.cb_remote;
        cfg.cb_port = c.cb_port;
    });

    get_block("heartbeat", c.heartbeat, [&](auto& cfg, auto const& main) {
        get_node_or_default(cfg.interval_s, main, "interval_s", 1);
        get_node_or_default(cfg.connection_to_s, main, "connection_to_s", 3 * cfg.interval_s);
        cfg.cb_remote = c.cb_remote;
        cfg.cb_port = c.cb_port;
        get_node(cfg.cb_config.network, "charge_bridge");
        get_node(cfg.cb_config.safety, "safety");

        std::memset(cfg.cb_config.gpios, 0, CB_NUMBER_OF_GPIOS * sizeof(CbGpioConfig));
        std::memset(cfg.cb_config.uarts, 0, CB_NUMBER_OF_UARTS * sizeof(CbUartConfig));
        if (c.serial1) {
            get_node(cfg.cb_config.uarts[0], "serial_1");
        }
        if (c.serial2) {
            get_node(cfg.cb_config.uarts[1], "serial_2");
        }
        // FIXME (JH) serial 3 not available in first release
        // if (c.serial3) {
        //     get_main_node("serial_3", cfg.cb_config.uarts[2]);
        // }
        if (c.gpio) {
            for (auto i = 0; i < CB_NUMBER_OF_GPIOS; ++i) {
                get_node(cfg.cb_config.gpios[i], "gpio", "gpio_" + std::to_string(i));
            }
        }
        if (c.can0) {
            get_node(cfg.cb_config.can, "can_0");
        }
        get_node(cfg.cb_config.plc_powersaving_mode, "plc", "powersaving_mode");
        cfg.cb_config.config_version = CB_CONFIG_VERSION;
    });

    get_node(c.firmware.fw_path, "charge_bridge", "fw_file");
    get_node(c.firmware.fw_update_on_start, "charge_bridge", "fw_update_on_start");

    // If the path to the firmware file is relative, make it relative to the config file
    std::filesystem::path fw_path = c.firmware.fw_path;
    if (fw_path.is_relative()) {
        c.firmware.fw_path = config_path.parent_path().append(c.firmware.fw_path);
    }

    c.firmware.cb_remote = c.cb_remote;
    c.firmware.cb_port = c.cb_port;
    c.firmware.cb = c.cb_name;
}

charge_bridge_config set_config_placeholders(charge_bridge_config const& src, charge_bridge_config& result,
                                             std::string const& ip, std::size_t index) {
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
    if (result.bsp.has_value()) {
        result.bsp->cb_remote = ip;
        result.bsp->cb = result.cb_name;
        replace(result.bsp->api.evse.module_id);
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
    const static RymlCallbackInitializer ryml_callback_initializer;

    try {
        ryml::EventHandlerTree evt_handler = {};
        ryml::Parser parser(&evt_handler, ryml::ParserOptions().locations(true));
        ryml::Tree config_tree;
        load_yaml_file(config_file, &parser, &config_tree);
        c4::yml::NodeRef config = config_tree.rootref();
        if (config.invalid()) {
            std::cerr << "Config file not found: " << config_file << std::endl;
            return {};
        }
        charge_bridge_config base_config;
        parse_config_impl(config, base_config, config_file, parser);

        auto ip_list_node = config.find_child("charge_bridge_ip_list");
        if (ip_list_node.invalid()) {
            return {base_config};
        }
        std::vector<std::string> ip_list;
        ip_list_node >> ip_list;
        std::vector<charge_bridge_config> cb_config_list(ip_list.size());

        for (std::size_t i = 0; i < ip_list.size(); ++i) {
            set_config_placeholders(base_config, cb_config_list[i], ip_list[i], i);
        }

        return cb_config_list;
    } catch (...) {
        std::cerr << "FAILED to parse configuration!" << std::endl;
    }
    return {};
}

} // namespace charge_bridge::utilities
