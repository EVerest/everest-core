// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "Setup.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <locale>

#include <boost/process.hpp>

#include <fmt/core.h>

namespace module {

void to_json(json& j, const NetworkDeviceInfo& k) {
    j = json::object({{"interface", k.interface},
                      {"wireless", k.wireless},
                      {"blocked", k.blocked},
                      {"rfkill_id", k.rfkill_id},
                      {"ipv4", k.ipv4},
                      {"ipv6", k.ipv6},
                      {"mac", k.mac},
                      {"link_type", k.link_type}});
}

void to_json(json& j, const WifiInfo& k) {
    auto flags_array = json::array();
    flags_array = k.flags;
    j = json::object({{"bssid", k.bssid},
                      {"ssid", k.ssid},
                      {"frequency", k.frequency},
                      {"signal_level", k.signal_level},
                      {"flags", flags_array}});
}

void to_json(json& j, const WifiCredentials& k) {
    j = json::object({{"interface", k.interface}, {"ssid", k.ssid}, {"psk", k.psk}});
}

void from_json(const json& j, WifiCredentials& k) {
    k.interface = j.at("interface");
    k.ssid = j.at("ssid");
    k.psk = j.at("psk");
}

void to_json(json& j, const WifiList& k) {
    j = json::object({{"interface", k.interface},
                      {"network_id", k.network_id},
                      {"ssid", k.ssid},
                      {"connected", k.connected},
                      {"signal_level", k.signal_level}});
}

void to_json(json& j, const InterfaceAndNetworkId& k) {
    j = json::object({{"interface", k.interface}, {"network_id", k.network_id}});
}
void from_json(const json& j, InterfaceAndNetworkId& k) {
    k.interface = j.at("interface");
    k.network_id = j.at("network_id");
}

void to_json(json& j, const SupportedSetupFeatures& k) {
    j = json::object(
        {{"setup_wifi", k.setup_wifi}, {"localization", k.localization}, {"setup_simulation", k.setup_simulation}});
}

void to_json(json& j, const ApplicationInfo& k) {
    j = json::object({{"initialized", k.initialized},
                      {"mode", k.mode},
                      {"default_language", k.default_language},
                      {"current_language", k.current_language},
                      {"release_metadata_file", k.release_metadata_file}});
}

void Setup::init() {
    invoke_init(*p_main);

    // Set default locale "C" when no locale is set at all
    try {
        std::locale loc("");
    } catch (const std::runtime_error& e) {
        setenv("LC_ALL", "C", 1);
    }
}

void Setup::ready() {
    invoke_ready(*p_main);

    this->discover_network_thread = std::thread([this]() {
        while (true) {
            if (wifi_scan_enabled) {
                this->discover_network();
            }
            this->publish_hostname();
            this->publish_configured_networks();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    });

    this->publish_application_info_thread = std::thread([this]() {
        while (true) {
            this->publish_supported_features();
            this->publish_application_info();
            this->publish_ap_state();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::string set_mode_cmd = this->cmd_base + "set_mode";
    this->mqtt.subscribe(set_mode_cmd, [this](const std::string& data) { this->set_mode(data); });

    std::string set_initialized_cmd = this->cmd_base + "set_initialized";
    this->mqtt.subscribe(set_initialized_cmd, [this](const std::string& data) { this->set_initialized(true); });

    std::string reset_initialized_cmd = this->cmd_base + "reset_initialized";
    this->mqtt.subscribe(reset_initialized_cmd, [this](const std::string& data) { this->set_initialized(false); });

    std::string change_default_language_cmd = this->cmd_base + "change_default_language";
    this->mqtt.subscribe(change_default_language_cmd,
                         [this](const std::string& data) { this->set_default_language(data); });

    std::string change_current_language_cmd = this->cmd_base + "change_current_language";
    this->mqtt.subscribe(change_current_language_cmd,
                         [this](const std::string& data) { this->set_current_language(data); });

    std::string get_application_info_cmd = this->cmd_base + "get_application_info";
    this->mqtt.subscribe(get_application_info_cmd,
                         [this](const std::string& data) { this->publish_application_info(); });

    if (this->config.setup_wifi) {
        std::string rfkill_unblock_cmd = this->cmd_base + "rfkill_unblock";
        this->mqtt.subscribe(rfkill_unblock_cmd, [this](const std::string& data) { this->rfkill_unblock(data); });

        std::string rfkill_block_cmd = this->cmd_base + "rfkill_block";
        this->mqtt.subscribe(rfkill_block_cmd, [this](const std::string& data) { this->rfkill_block(data); });

        std::string list_configured_networks_cmd = this->cmd_base + "list_configured_networks";
        this->mqtt.subscribe(list_configured_networks_cmd,
                             [this](const std::string& data) { this->publish_configured_networks(); });

        std::string add_network_cmd = this->cmd_base + "add_network";
        this->mqtt.subscribe(add_network_cmd, [this](const std::string& data) {
            WifiCredentials wifi_credentials = json::parse(data);
            this->add_and_enable_network(wifi_credentials);
            this->save_config(wifi_credentials.interface);
            this->publish_configured_networks();
        });

        std::string enable_network_cmd = this->cmd_base + "enable_network";
        this->mqtt.subscribe(enable_network_cmd, [this](const std::string& data) {
            InterfaceAndNetworkId wifi = json::parse(data);
            this->enable_network(wifi.interface, wifi.network_id);
            this->publish_configured_networks();
        });

        std::string disable_network_cmd = this->cmd_base + "disable_network";
        this->mqtt.subscribe(disable_network_cmd, [this](const std::string& data) {
            InterfaceAndNetworkId wifi = json::parse(data);
            this->disable_network(wifi.interface, wifi.network_id);
            this->publish_configured_networks();
        });

        std::string select_network_cmd = this->cmd_base + "select_network";
        this->mqtt.subscribe(select_network_cmd, [this](const std::string& data) {
            InterfaceAndNetworkId wifi = json::parse(data);
            this->select_network(wifi.interface, wifi.network_id);
            this->publish_configured_networks();
        });

        std::string remove_network_cmd = this->cmd_base + "remove_network";
        this->mqtt.subscribe(remove_network_cmd, [this](const std::string& data) {
            InterfaceAndNetworkId wifi = json::parse(data);
            this->remove_network(wifi.interface, wifi.network_id);
            this->save_config(wifi.interface);
            this->publish_configured_networks();
        });

        std::string scan_wifi_cmd = this->cmd_base + "scan_wifi";
        this->mqtt.subscribe(scan_wifi_cmd, [this](const std::string& data) { this->discover_network(); });

        std::string enable_wifi_scanning_cmd = this->cmd_base + "enable_wifi_scanning";
        this->mqtt.subscribe(enable_wifi_scanning_cmd,
                             [this](const std::string& data) { this->wifi_scan_enabled = true; });

        std::string disable_wifi_scanning_cmd = this->cmd_base + "disable_wifi_scanning";
        this->mqtt.subscribe(disable_wifi_scanning_cmd,
                             [this](const std::string& data) { this->wifi_scan_enabled = false; });

        std::string remove_all_networks_cmd = this->cmd_base + "remove_all_networks";
        this->mqtt.subscribe(remove_all_networks_cmd, [this](const std::string& data) {
            this->remove_all_networks();
            this->publish_configured_networks();
        });

        std::string reboot_cmd = this->cmd_base + "reboot";
        this->mqtt.subscribe(reboot_cmd, [this](const std::string& data) { this->reboot(); });

        std::string check_online_status_cmd = this->cmd_base + "check_online_status";
        this->mqtt.subscribe(check_online_status_cmd, [this](const std::string& data) { this->check_online_status(); });

        std::string enable_ap_cmd = this->cmd_base + "enable_ap";
        this->mqtt.subscribe(enable_ap_cmd, [this](const std::string& data) { enable_ap(); });

        std::string disable_ap_cmd = this->cmd_base + "disable_ap";
        this->mqtt.subscribe(disable_ap_cmd, [this](const std::string& data) { disable_ap(); });
    }
}

void Setup::publish_supported_features() {
    SupportedSetupFeatures supported_setup_features;
    supported_setup_features.setup_wifi = this->config.setup_wifi;
    supported_setup_features.localization = this->config.localization;
    supported_setup_features.setup_simulation = this->config.setup_simulation;

    std::string supported_setup_features_var = this->var_base + "supported_setup_features";

    json supported_setup_features_json = supported_setup_features;

    this->mqtt.publish(supported_setup_features_var, supported_setup_features_json.dump());
}

void Setup::publish_application_info() {
    ApplicationInfo application_info;
    application_info.initialized = this->get_initialized();
    application_info.mode = this->get_mode();
    application_info.default_language = this->get_default_language();
    application_info.current_language = this->get_current_language();
    application_info.release_metadata_file = this->info.paths.etc / this->config.release_metadata_file;

    std::string application_info_var = this->var_base + "application_info";

    json application_info_json = application_info;

    this->mqtt.publish(application_info_var, application_info_json.dump());
}

void Setup::publish_hostname() {
    std::string hostname_var = this->var_base + "hostname";

    this->mqtt.publish(hostname_var, this->get_hostname());
}

void Setup::publish_ap_state() {
    std::string ap_state_var = this->var_base + "ap_state";

    auto hostapd_enabled_output = this->run_application("systemctl", {"is-active", "--quiet", "hostapd"});
    if (hostapd_enabled_output.exit_code == 0) {
        this->ap_state = "enabled";
    } else {
        this->ap_state = "disabled";
    }

    this->mqtt.publish(ap_state_var, this->ap_state);
}

void Setup::set_default_language(std::string language) {
    this->r_store->call_store("everest_localization_default_language", language);
}

std::string Setup::get_default_language() {
    auto language = this->r_store->call_load("everest_localization_default_language");
    if (!std::holds_alternative<std::string>(language)) {
        return "unknown";
    }
    return std::get<std::string>(language);
}

void Setup::set_current_language(const std::string& language) {
    this->current_language = language;
}

std::string Setup::get_current_language() {
    if (this->current_language.empty()) {
        this->current_language = this->get_default_language();
    }

    return this->current_language;
}

void Setup::set_mode(std::string mode) {
    this->r_store->call_store("everest_mode", mode);
}

std::string Setup::get_mode() {
    auto mode = this->r_store->call_load("everest_mode");
    if (!std::holds_alternative<std::string>(mode)) {
        return "unknown";
    }
    return std::get<std::string>(mode);
}

void Setup::set_initialized(bool initialized) {
    this->r_store->call_store("everest_initialized", initialized);
}

bool Setup::get_initialized() {
    if (this->config.initialized_by_default) {
        return true;
    }
    auto initialized = this->r_store->call_load("everest_initialized");
    if (!std::holds_alternative<bool>(initialized)) {
        return false;
    }
    return std::get<bool>(initialized);
}

void Setup::discover_network() {
    std::vector<NetworkDeviceInfo> device_info = this->get_network_devices();

    this->populate_rfkill_status(device_info);
    this->populate_ip_addresses(device_info);

    std::string network_device_info_var = this->var_base + "network_device_info";
    json device_info_json = json::array();
    device_info_json = device_info;
    this->mqtt.publish(network_device_info_var, device_info_json.dump());

    auto wifi_info = this->scan_wifi(device_info);

    std::string wifi_info_var = this->var_base + "wifi_info";
    json wifi_info_json = json::array();
    wifi_info_json = wifi_info;
    this->mqtt.publish(wifi_info_var, wifi_info_json.dump());

    this->publish_configured_networks();
}

std::string Setup::read_type_file(const fs::path& type_path) {
    if (!fs::exists(type_path)) {
        return "";
    }

    std::ifstream ifs(type_path.c_str());
    std::string type_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    // trim newlines
    type_file.erase(std::remove(type_file.begin(), type_file.end(), '\n'), type_file.end());

    return type_file;
}

std::vector<NetworkDeviceInfo> Setup::get_network_devices() {
    auto sys_net_path = fs::path("/sys/class/net");
    auto sys_virtual_net_path = fs::path("/sys/devices/virtual/net");

    std::vector<NetworkDeviceInfo> device_info;

    for (auto&& net_it : fs::directory_iterator(sys_net_path)) {
        auto net_path = net_it.path();
        auto type_path = net_path / "type";
        if (!fs::exists(type_path)) {
            continue;
        }

        std::string type_file = this->read_type_file(type_path);

        auto interface = net_path.filename();
        auto virtual_interface = sys_virtual_net_path / interface;

        // check if type is ethernet:
        if (type_file == "1") {
            if (fs::exists(virtual_interface)) {
                continue;
            }

            auto device = NetworkDeviceInfo();
            device.interface = interface.string();
            device.link_type = "ether";

            // check if its wireless or not:
            auto wireless_path = net_path / "wireless";
            if (fs::exists(wireless_path)) {
                device.wireless = true;
                auto phy80211_path = net_path / "phy80211";
                for (auto&& rfkill_it : fs::directory_iterator(phy80211_path)) {
                    auto phy_file_path = rfkill_it.path().filename().string();
                    std::string rfkill = "rfkill";
                    if (phy_file_path.find(rfkill) == 0) {
                        phy_file_path.erase(0, rfkill.size());
                        device.rfkill_id = phy_file_path;
                        break;
                    }
                }
            }

            device_info.push_back(device);
        } else if (type_file == "65534") {
            if (!fs::exists(virtual_interface)) {
                continue;
            }

            auto virtual_type_path = virtual_interface / "type";

            if (!fs::exists(virtual_type_path)) {
                continue;
            }

            std::string virtual_type_file = this->read_type_file(virtual_type_path);
            if (virtual_type_file == type_file) {
                // assume it's a vpn, but check ip link
                auto ip_output = this->run_application("ip", {"--json", "-details", "link", "show", interface});
                if (ip_output.exit_code != 0) {
                    continue;
                }
                const auto ip_json = json::parse(ip_output.output);
                if (ip_json.size() < 1) {
                    continue;
                }
                const auto& entry = ip_json.at(0);
                if (entry.contains("linkinfo") and entry.at("linkinfo").contains("info_kind")) {
                    auto device = NetworkDeviceInfo();
                    device.interface = interface.string();
                    device.link_type = entry.at("linkinfo").at("info_kind");
                    device_info.push_back(device);
                }
            }
        }
    }

    return device_info;
}

void Setup::populate_rfkill_status(std::vector<NetworkDeviceInfo>& device_info) {
    auto rfkill_output = this->run_application("rfkill", {"--json"});
    if (rfkill_output.exit_code != 0) {
        return;
    }
    auto rfkill_json = json::parse(rfkill_output.output);
    for (auto rfkill_object : rfkill_json.items()) {
        for (auto rfkill_device : rfkill_object.value()) {
            for (auto& device : device_info) {
                int device_id = rfkill_device.at("id");
                if (!device.rfkill_id.empty() && std::to_string(device_id) == device.rfkill_id) {
                    if (rfkill_device.at("soft") == "blocked") {
                        device.blocked = true;
                    }
                    break;
                }
            }
        }
    }
}

bool Setup::rfkill_unblock(std::string rfkill_id) {
    auto network_devices = this->get_network_devices();
    this->populate_rfkill_status(network_devices);
    bool found = false;
    for (auto device : network_devices) {
        if (device.rfkill_id == rfkill_id) {
            if (!device.blocked) {
                return true;
            }
            found = true;
            break;
        }
    }

    if (!found) {
        return false;
    }

    auto rfkill_output = this->run_application("rfkill", {"unblock", rfkill_id});
    if (rfkill_output.exit_code != 0) {
        return false;
    }

    return true;
}

bool Setup::rfkill_block(std::string rfkill_id) {
    auto network_devices = this->get_network_devices();
    this->populate_rfkill_status(network_devices);
    bool found = false;
    for (auto device : network_devices) {
        if (device.rfkill_id == rfkill_id) {
            if (device.blocked) {
                return true;
            }
            found = true;
            break;
        }
    }

    if (!found) {
        return false;
    }

    auto rfkill_output = this->run_application("rfkill", {"block", rfkill_id});
    if (rfkill_output.exit_code != 0) {
        return false;
    }

    return true;
}

bool Setup::is_wifi_interface(std::string interface) {
    // TODO: maybe cache these results for some time?
    auto network_devices = this->get_network_devices();

    for (auto device : network_devices) {
        if (device.interface == interface && device.wireless) {
            return true;
        }
    }

    return false;
}

std::vector<WifiList> Setup::list_configured_networks(std::string interface) {
    if (!this->is_wifi_interface(interface)) {
        return {};
    }

    // use wpa_cli to query wifi info
    auto wpa_cli_list_networks_output = this->run_application("wpa_cli", {"-i", interface, "list_networks"});
    if (wpa_cli_list_networks_output.exit_code != 0) {
        return {};
    }

    std::vector<WifiList> configured_networks;
    std::map<std::string, std::string> wpa_cli_status_map;

    auto wpa_cli_status_output = this->run_application("wpa_cli", {"-i", interface, "status"});
    if (wpa_cli_status_output.exit_code == 0) {
        for (auto wpa_cli_status_it = wpa_cli_status_output.split_output.begin();
             wpa_cli_status_it != wpa_cli_status_output.split_output.end(); ++wpa_cli_status_it) {
            std::vector<std::string> wpa_cli_status_columns;
            // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
            boost::split(wpa_cli_status_columns, *wpa_cli_status_it, boost::is_any_of("="));
            if (wpa_cli_status_columns.size() == 2) {
                wpa_cli_status_map[wpa_cli_status_columns.at(0)] = wpa_cli_status_columns.at(1);
            }
        }
    }
    std::map<std::string, std::string> wpa_cli_signal_poll_map;
    auto wpa_cli_signal_poll_output = this->run_application("wpa_cli", {"-i", interface, "signal_poll"});
    if (wpa_cli_signal_poll_output.exit_code == 0) {

        for (auto wpa_cli_signal_poll_it = wpa_cli_signal_poll_output.split_output.begin();
             wpa_cli_signal_poll_it != wpa_cli_signal_poll_output.split_output.end(); ++wpa_cli_signal_poll_it) {
            std::vector<std::string> wpa_cli_signal_poll_columns;
            // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
            boost::split(wpa_cli_signal_poll_columns, *wpa_cli_signal_poll_it, boost::is_any_of("="));
            if (wpa_cli_signal_poll_columns.size() == 2) {
                wpa_cli_signal_poll_map[wpa_cli_signal_poll_columns.at(0)] = wpa_cli_signal_poll_columns.at(1);
            }
        }
    }

    auto list_networks_results = wpa_cli_list_networks_output.split_output;
    if (list_networks_results.size() >= 2) {
        // skip header
        for (auto list_networks_results_it = std::next(list_networks_results.begin());
             list_networks_results_it != list_networks_results.end(); ++list_networks_results_it) {
            std::vector<std::string> list_networks_results_columns;
            // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
            boost::split(list_networks_results_columns, *list_networks_results_it, boost::is_any_of("\t"));

            WifiList wifi_list;
            wifi_list.interface = interface;
            wifi_list.network_id = std::stoi(list_networks_results_columns.at(0));
            wifi_list.ssid = list_networks_results_columns.at(1);
            wifi_list.connected = false;
            wifi_list.signal_level = -100; // -100 dBm is the minimum for wifi

            if (wpa_cli_status_map.count("id") && wpa_cli_status_map.count("ssid") &&
                wpa_cli_status_map.count("wpa_state")) {
                if (wpa_cli_status_map.at("id") == list_networks_results_columns.at(0) &&
                    wpa_cli_status_map.at("ssid") == wifi_list.ssid &&
                    wpa_cli_status_map.at("wpa_state") == "COMPLETED") {
                    wifi_list.connected = true;
                    if (wpa_cli_signal_poll_map.count("RSSI")) {
                        wifi_list.signal_level = std::stoi(wpa_cli_signal_poll_map.at("RSSI"));
                    }
                }
            }

            configured_networks.push_back(wifi_list);
        }
    }

    return configured_networks;
}

void Setup::publish_configured_networks() {
    auto network_devices = this->get_network_devices();
    for (auto device : network_devices) {
        if (!device.wireless) {
            continue;
        }
        auto network_list = this->list_configured_networks(device.interface);
        std::string network_list_var = this->var_base + "configured_networks";
        json configured_networks_json = json::array();
        configured_networks_json = network_list;
        this->mqtt.publish(network_list_var, configured_networks_json.dump());
    }
}

int Setup::add_network(std::string interface) {
    if (!this->is_wifi_interface(interface)) {
        return -1;
    }

    auto wpa_cli_output = this->run_application("wpa_cli", {"-i", interface, "add_network"});
    if (wpa_cli_output.exit_code != 0) {
        return -1;
    }

    if (wpa_cli_output.split_output.size() != 1) {
        return -1;
    }
    return std::stoi(wpa_cli_output.split_output.at(0));
}

bool Setup::add_and_enable_network(WifiCredentials wifi_credentials) {
    return this->add_and_enable_network(wifi_credentials.interface, wifi_credentials.ssid, wifi_credentials.psk);
}

bool Setup::add_and_enable_network(std::string interface, std::string ssid, std::string psk) {
    if (interface.empty()) {
        EVLOG_warning << "Attempting to add a network without an interface, attempting to use the first one";
        auto network_devices = this->get_network_devices();
        for (auto device : network_devices) {
            if (device.wireless) {
                interface = device.interface;
                break;
            }
        }
    }

    if (!this->is_wifi_interface(interface)) {
        return false;
    }

    auto network_id = this->add_network(interface);
    if (network_id == -1) {
        return false;
    }

    if (!this->set_network(interface, network_id, ssid, psk)) {
        return false;
    }

    return this->enable_network(interface, network_id);
}

bool Setup::set_network(std::string interface, int network_id, std::string ssid, std::string psk) {
    if (!this->is_wifi_interface(interface)) {
        return false;
    }

    auto network_id_string = std::to_string(network_id);

    auto ssid_parameter = "\"" + ssid + "\"";

    auto wpa_cli_set_ssid_output =
        this->run_application("wpa_cli", {"-i", interface, "set_network", network_id_string, "ssid", ssid_parameter});
    if (wpa_cli_set_ssid_output.exit_code != 0) {
        return false;
    }

    auto wpa_cli_set_psk_output =
        this->run_application("wpa_cli", {"-i", interface, "set_network", network_id_string, "psk", psk});

    if (wpa_cli_set_psk_output.exit_code != 0) {
        return false;
    }

    return true;
}

bool Setup::enable_network(std::string interface, int network_id) {
    if (!this->is_wifi_interface(interface)) {
        return false;
    }

    auto network_id_string = std::to_string(network_id);

    auto wpa_cli_enable_network_output =
        this->run_application("wpa_cli", {"-i", interface, "enable_network", network_id_string});
    if (wpa_cli_enable_network_output.exit_code != 0) {
        return false;
    }

    return true;
}

bool Setup::disable_network(std::string interface, int network_id) {
    if (!this->is_wifi_interface(interface)) {
        return false;
    }

    auto network_id_string = std::to_string(network_id);

    auto wpa_cli_disable_network_output =
        this->run_application("wpa_cli", {"-i", interface, "disable_network", network_id_string});
    if (wpa_cli_disable_network_output.exit_code != 0) {
        return false;
    }

    return true;
}

bool Setup::select_network(std::string interface, int network_id) {
    if (!this->is_wifi_interface(interface)) {
        return false;
    }

    auto network_id_string = std::to_string(network_id);

    auto wpa_cli_disable_network_output =
        this->run_application("wpa_cli", {"-i", interface, "select_network", network_id_string});
    if (wpa_cli_disable_network_output.exit_code != 0) {
        return false;
    }

    return true;
}

bool Setup::remove_network(std::string interface, int network_id) {
    if (!this->is_wifi_interface(interface)) {
        return false;
    }

    auto network_id_string = std::to_string(network_id);

    auto wpa_cli_remove_network_output =
        this->run_application("wpa_cli", {"-i", interface, "remove_network", network_id_string});
    if (wpa_cli_remove_network_output.exit_code != 0) {
        return false;
    }

    return true;
}

bool Setup::remove_networks(std::string interface) {
    if (!this->is_wifi_interface(interface)) {
        return false;
    }

    auto networks = this->list_configured_networks(interface);

    bool success = true;
    for (auto network : networks) {
        auto network_id_string = std::to_string(network.network_id);

        auto wpa_cli_remove_network_output =
            this->run_application("wpa_cli", {"-i", interface, "remove_network", network_id_string});
        if (wpa_cli_remove_network_output.exit_code != 0) {
            success = false;
        }
    }

    return success;
}

bool Setup::remove_all_networks() {
    auto network_devices = this->get_network_devices();
    bool success = true;
    for (auto device : network_devices) {
        if (!device.wireless) {
            continue;
        }

        if (!this->remove_networks(device.interface)) {
            success = false;
        }
        this->save_config(device.interface);
    }

    return success;
}

bool Setup::save_config(std::string interface) {
    bool success = true;
    auto wpa_cli_save_config_output = this->run_application("wpa_cli", {"-i", interface, "save_config"});
    if (wpa_cli_save_config_output.exit_code != 0) {
        success = false;
    }
    return success;
}

bool Setup::reboot() {
    bool success = true;
    auto reboot_output = this->run_application("systemctl", {"reboot"});
    if (reboot_output.exit_code != 0) {
        success = false;
    }
    return success;
}

bool Setup::is_online() {
    bool success = true;
    auto reboot_output = this->run_application("ping", {"-c", "1", this->config.online_check_host});
    if (reboot_output.exit_code != 0) {
        success = false;
    }
    return success;
}

void Setup::check_online_status() {
    std::string online_status_var = this->var_base + "online_status";

    if (this->is_online()) {
        this->mqtt.publish(online_status_var, "online");
    } else {
        this->mqtt.publish(online_status_var, "offline");
    }
}

void Setup::enable_ap() {
    bool success = true;
    auto wpa_cli_output = this->run_application("wpa_cli", {"-i", this->config.ap_interface, "disconnect"});
    if (wpa_cli_output.exit_code != 0) {
        EVLOG_error << "Could not disconnect from wireless LAN";
    }
    auto start_hostapd_output = this->run_application("systemctl", {"start", "hostapd"});
    if (start_hostapd_output.exit_code != 0) {
        EVLOG_error << "Could not start hostapd";
    }
    auto start_dnsmasq_output = this->run_application("systemctl", {"start", "dnsmasq"});
    if (start_dnsmasq_output.exit_code != 0) {
        EVLOG_error << "Could not start dnsmasq";
    }
    auto add_static_ip_output =
        this->run_application("ip", {"addr", "add", this->config.ap_ipv4, "dev", this->config.ap_interface});
    if (add_static_ip_output.exit_code != 0) {
        EVLOG_error << "Could not add static ip to interface " << this->config.ap_interface;
    }
}

void Setup::disable_ap() {
    auto del_static_ip_output =
        this->run_application("ip", {"addr", "del", this->config.ap_ipv4, "dev", this->config.ap_interface});
    if (del_static_ip_output.exit_code != 0) {
        EVLOG_error << "Could not del static ip " << this->config.ap_ipv4 << " from interface "
                    << this->config.ap_interface;
    }
    auto stop_dnsmasq_output = this->run_application("systemctl", {"stop", "dnsmasq"});
    if (stop_dnsmasq_output.exit_code != 0) {
        EVLOG_error << "Could not stop dnsmasq";
    }
    auto stop_hostapd_output = this->run_application("systemctl", {"stop", "hostapd"});
    if (stop_hostapd_output.exit_code != 0) {
        EVLOG_error << "Could not stop hostapd";
    }

    auto wpa_cli_output = this->run_application("wpa_cli", {"-i", this->config.ap_interface, "reconnect"});
    if (wpa_cli_output.exit_code != 0) {
        EVLOG_error << "Could not reconnect to wireless LAN";
    }
}

static void add_addr_infos_to_device(const json& addr_infos, NetworkDeviceInfo& device) {
    for (const auto& addr_info : addr_infos) {
        if (addr_info.at("family") == "inet") {
            device.ipv4.push_back(addr_info.at("local"));
        } else if (addr_info.at("family") == "inet6") {
            device.ipv6.push_back(addr_info.at("local"));
        }
    }
}

void Setup::populate_ip_addresses(std::vector<NetworkDeviceInfo>& device_info) {
    auto ip_output = this->run_application("ip", {"--json", "address", "show"});
    if (ip_output.exit_code != 0) {
        return;
    }
    const auto ip_json = json::parse(ip_output.output);
    for (const auto& ip_object : ip_json) {
        const std::string ifname = ip_object.at("ifname");
        auto device = std::find_if(device_info.begin(), device_info.end(),
                                   [&ifname](NetworkDeviceInfo& device) { return device.interface == ifname; });
        if (device == device_info.end()) {
            continue;
        }
        if (ip_object.contains("address")) {
            device->mac = ip_object.at("address");
        }
        add_addr_infos_to_device(ip_object.at("addr_info"), *device);
    }
}

std::vector<WifiInfo> Setup::scan_wifi(const std::vector<NetworkDeviceInfo>& device_info) {
    std::vector<WifiInfo> wifi_info;

    for (auto device : device_info) {
        if (!device.wireless) {
            continue;
        }
        // use wpa_cli to query wifi info
        auto wpa_cli_scan_output = this->run_application("wpa_cli", {"-i", device.interface, "scan"});
        if (wpa_cli_scan_output.exit_code != 0) {
            continue;
        }

        // FIXME: is there a proper signal to check if the scan is ready? Maybe in the socket based interface
        std::this_thread::sleep_for(std::chrono::seconds(3));
        auto wpa_cli_scan_results_output = this->run_application("wpa_cli", {"-i", device.interface, "scan_results"});
        if (wpa_cli_scan_results_output.exit_code != 0) {
            continue;
        }

        auto scan_results = wpa_cli_scan_results_output.split_output;
        if (scan_results.size() >= 2) {
            // skip header
            for (auto scan_results_it = std::next(scan_results.begin()); scan_results_it != scan_results.end();
                 ++scan_results_it) {
                std::vector<std::string> scan_results_columns;
                // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
                boost::split(scan_results_columns, *scan_results_it, boost::is_any_of("\t"));

                WifiInfo info;
                info.bssid = scan_results_columns.at(0);
                info.ssid = scan_results_columns.at(4);
                info.frequency = std::stoi(scan_results_columns.at(1));
                info.signal_level = std::stoi(scan_results_columns.at(2));
                info.flags = this->parse_wpa_cli_flags(scan_results_columns.at(3));

                wifi_info.push_back(info);
            }
        }
    }

    return wifi_info;
}

std::string Setup::get_hostname() {
    auto hostname_output = this->run_application("hostname", {});
    if (hostname_output.exit_code == 0 && hostname_output.split_output.size() > 0) {
        return hostname_output.split_output.at(0);
    }
    return "";
}

CmdOutput Setup::run_application(const std::string& name, std::vector<std::string> args) {
    const auto path = boost::process::search_path(name);

    if (path.empty()) {
        EVLOG_debug << fmt::format("The application '{}' could not be found", name);
        return CmdOutput{"", {}, 1};
    }

    boost::process::ipstream stream;
    boost::process::child cmd(path, boost::process::args(args), boost::process::std_out > stream);
    std::string output;
    std::vector<std::string> split_output;
    std::string temp;
    while (std::getline(stream, temp)) {
        output += temp + "\n";
        split_output.push_back(temp);
    }
    cmd.wait();
    return CmdOutput{output, split_output, cmd.exit_code()};
}

std::vector<std::string> Setup::parse_wpa_cli_flags(std::string flags) {
    const std::regex wpa_cli_flags_regex("\\[(.*?)\\]");

    std::vector<std::string> parsed_flags;

    for (auto regex_it = std::sregex_iterator(flags.begin(), flags.end(), wpa_cli_flags_regex);
         regex_it != std::sregex_iterator(); ++regex_it) {
        parsed_flags.push_back((*regex_it).str(1));
    }

    return parsed_flags;
}

} // namespace module
