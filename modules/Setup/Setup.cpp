// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "Setup.hpp"

#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <cstdlib>
#include <fstream>

namespace module {

namespace fs = boost::filesystem;

void to_json(json& j, const NetworkDeviceInfo& k) {
    j = json::object({{"interface", k.interface},
                      {"wireless", k.wireless},
                      {"blocked", k.blocked},
                      {"rfkill_id", k.rfkill_id},
                      {"ipv4", k.ipv4},
                      {"ipv6", k.ipv6}});
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
    j = json::object({{"interface", k.interface}, {"network_id", k.network_id}, {"ssid", k.ssid}});
}

void Setup::init() {
    invoke_init(*p_main);
}

void Setup::ready() {
    invoke_ready(*p_main);

    this->discover_network_thread = std::thread([this]() {
        while (true) {
            this->discover_network();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    });

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
        this->publish_configured_networks();
    });

    std::string remove_all_networks_cmd = this->cmd_base + "remove_all_networks";
    this->mqtt.subscribe(remove_all_networks_cmd, [this](const std::string& data) {
        this->remove_all_networks();
        this->publish_configured_networks();
    });
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

        std::ifstream ifs(type_path.c_str());
        std::string type_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        // trim newlines
        type_file.erase(std::remove(type_file.begin(), type_file.end(), '\n'), type_file.end());

        // check if type is ethernet:
        if (type_file == "1") {
            auto interface = net_path.filename();

            auto virtual_interface = sys_virtual_net_path / interface;
            if (fs::exists(virtual_interface)) {
                continue;
            }

            auto device = NetworkDeviceInfo();
            device.interface = interface.string();

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
        }
    }

    return device_info;
}

void Setup::populate_rfkill_status(std::vector<NetworkDeviceInfo>& device_info) {
    auto rfkill_output = this->run_application(fs::path("/usr/sbin/rfkill"), {"--json"});
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

    auto rfkill_output = this->run_application(fs::path("/usr/sbin/rfkill"), {"unblock", rfkill_id});
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

    auto rfkill_output = this->run_application(fs::path("/usr/sbin/rfkill"), {"block", rfkill_id});
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
    auto wpa_cli_list_networks_output = this->run_application("/usr/sbin/wpa_cli", {"-i", interface, "list_networks"});
    if (wpa_cli_list_networks_output.exit_code != 0) {
        return {};
    }

    std::vector<WifiList> configured_networks;

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

    auto wpa_cli_output = this->run_application("/usr/sbin/wpa_cli", {"-i", interface, "add_network"});
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
        EVLOG(warning) << "Attempting to add a network without an interface, attempting to use the first one";
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

    auto wpa_cli_set_ssid_output = this->run_application(
        "/usr/sbin/wpa_cli", {"-i", interface, "set_network", network_id_string, "ssid", ssid_parameter});
    if (wpa_cli_set_ssid_output.exit_code != 0) {
        return false;
    }

    auto wpa_cli_set_psk_output =
        this->run_application("/usr/sbin/wpa_cli", {"-i", interface, "set_network", network_id_string, "psk", psk});

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
        this->run_application("/usr/sbin/wpa_cli", {"-i", interface, "enable_network", network_id_string});
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
        this->run_application("/usr/sbin/wpa_cli", {"-i", interface, "disable_network", network_id_string});
    if (wpa_cli_disable_network_output.exit_code != 0) {
        return false;
    }
}

bool Setup::remove_network(std::string interface, int network_id) {
    if (!this->is_wifi_interface(interface)) {
        return false;
    }

    auto network_id_string = std::to_string(network_id);

    auto wpa_cli_remove_network_output =
        this->run_application("/usr/sbin/wpa_cli", {"-i", interface, "remove_network", network_id_string});
    if (wpa_cli_remove_network_output.exit_code != 0) {
        return false;
    }
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
            this->run_application("/usr/sbin/wpa_cli", {"-i", interface, "remove_network", network_id_string});
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
    }

    return success;
}

void Setup::populate_ip_addresses(std::vector<NetworkDeviceInfo>& device_info) {
    auto ip_output = this->run_application(fs::path("/usr/bin/ip"), {"--json", "address", "show"});
    if (ip_output.exit_code != 0) {
        return;
    }
    auto ip_json = json::parse(ip_output.output);
    for (auto ip_object : ip_json) {
        for (auto& device : device_info) {
            if (ip_object.at("ifname") == device.interface) {
                for (auto addr_info : ip_object.at("addr_info")) {
                    if (addr_info.at("family") == "inet") {
                        device.ipv4 = addr_info.at("local");
                    }
                    // FIXME: add ipv6 info
                }
                break;
            }
        }
    }
}

std::vector<WifiInfo> Setup::scan_wifi(const std::vector<NetworkDeviceInfo>& device_info) {
    std::vector<WifiInfo> wifi_info;

    for (auto device : device_info) {
        if (!device.wireless) {
            continue;
        }
        // use wpa_cli to query wifi info
        auto wpa_cli_scan_output = this->run_application("/usr/sbin/wpa_cli", {"-i", device.interface, "scan"});
        if (wpa_cli_scan_output.exit_code != 0) {
            continue;
        }

        // FIXME: is there a proper signal to check if the scan is ready? Maybe in the socket based interface
        std::this_thread::sleep_for(std::chrono::seconds(3));
        auto wpa_cli_scan_results_output =
            this->run_application("/usr/sbin/wpa_cli", {"-i", device.interface, "scan_results"});
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

CmdOutput Setup::run_application(boost::filesystem::path path, std::vector<std::string> args) {
    if (!path.is_absolute()) {
        EVLOG(error) << "Provided path MUST be absolute, but is not: " << path.string();
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
