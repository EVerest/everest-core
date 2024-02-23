// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "WiFiSetup.hpp"
#include "RunApplication.hpp"

#include <filesystem>
#include <regex>
#include <sstream>
#include <thread>
#include <utility>

/**
 * @file
 * @brief wpa_cli command failure detection
 *
 * `wpa_cli` sets an exit code of 0 unless the command is malformed.
 * Failures are presented via text to stdout.
 * Hence checking for failure to remove a network would mean checking
 * the output for OK or FAIL.
 *
 * This is common across all calls to `wpa_cli`.
 */

namespace module {

constexpr const char* wpa_cli = "/usr/sbin/wpa_cli";
constexpr const int not_connected_rssi = -100; // -100 dBm is the minimum for wifi

bool WpaCliSetup::do_scan(const std::string& interface) {
    if (!is_wifi_interface(interface)) {
        return false;
    }

    auto output = run_application(wpa_cli, {"-i", interface, "scan"});
    return output.exit_code == 0;
}

WpaCliSetup::WifiScanList WpaCliSetup::do_scan_results(const std::string& interface) {
    WifiScanList result = {};
    auto output = run_application(wpa_cli, {"-i", interface, "scan_results"});
    if (output.exit_code == 0) {
        auto scan_results = output.split_output;
        if (scan_results.size() >= 2) {
            // skip header
            for (auto scan_results_it = std::next(scan_results.begin()); scan_results_it != scan_results.end();
                 ++scan_results_it) {

                std::vector<std::string> columns;
                std::istringstream stream(*scan_results_it);
                for (std::string value; std::getline(stream, value, '\t');) {
                    columns.push_back(std::move(value));
                }

                if (columns.size() >= 5) {
                    WifiScan info;
                    info.bssid = columns[0];
                    info.ssid = columns[4];
                    info.frequency = std::stoi(columns[1]);
                    info.signal_level = std::stoi(columns[2]);
                    info.flags = std::move(parse_flags(columns[3]));
                    result.push_back(std::move(info));
                }
            }
        }
    }
    return result;
}

WpaCliSetup::Status WpaCliSetup::do_status(const std::string& interface) {
    Status result = {};
    if (is_wifi_interface(interface)) {
        auto output = run_application(wpa_cli, {"-i", interface, "status"});
        if (output.exit_code == 0) {
            auto scan_results = output.split_output;
            for (auto& scan_result : scan_results) {
                std::vector<std::string> columns;
                std::istringstream ss(scan_result);
                for (std::string value; std::getline(ss, value, '=');) {
                    columns.push_back(std::move(value));
                }

                if (columns.size() == 2) {
                    result[columns[0]] = columns[1];
                }
            }
        }
    }
    return result;
}

WpaCliSetup::Poll WpaCliSetup::do_signal_poll(const std::string& interface) {
    Poll result = {};
    if (is_wifi_interface(interface)) {
        auto output = run_application(wpa_cli, {"-i", interface, "signal_poll"});
        if (output.exit_code == 0) {
            auto scan_results = output.split_output;
            for (auto& scan_result : scan_results) {
                std::vector<std::string> columns;
                std::istringstream ss(scan_result);
                for (std::string value; std::getline(ss, value, '=');) {
                    columns.push_back(std::move(value));
                }

                if (columns.size() == 2) {
                    result[columns[0]] = columns[1];
                }
            }
        }
    }
    return result;
}

WpaCliSetup::flags_t WpaCliSetup::parse_flags(const std::string& flags) {
    const std::regex flags_regex("\\[(.*?)\\]");

    flags_t parsed_flags;

    for (auto it = std::sregex_iterator(flags.begin(), flags.end(), flags_regex); it != std::sregex_iterator(); ++it) {
        parsed_flags.push_back((*it).str(1));
    }

    return parsed_flags;
}

int WpaCliSetup::add_network(const std::string& interface) {
    if (!is_wifi_interface(interface)) {
        return -1;
    }

    auto output = run_application(wpa_cli, {"-i", interface, "add_network"});

    if ((output.exit_code != 0) || (output.split_output.size() != 1)) {
        return -1;
    }

    return std::stoi(output.split_output.at(0));
}

bool WpaCliSetup::set_network(const std::string& interface, int network_id, const std::string& ssid,
                              const std::string& psk, network_security_t mode, bool hidden) {
    /*
     * configuring a network needs:
     * - ssid "<SSID>"
     * - psk "<Passphrase>" or ABCDEF0123456789... (for WPA2)
     * - sae_password "<Passphrase>" (for WPA3)
     * - key_mgmt NONE (for open networks)
     * - scan_ssid 1 (for hidden networks)
     *
     * Support for WPA3 requires:
     * - key_mgmt WPA-PSK WPA-PSK-SHA256 SAE or SAE if WPA3 only
     * - sae_password replaces psk, WPA3 doesn't support PreSharedKey (64 hex digits)
     *   the passphrase is required
     * - for interworking WPA2 and WPA3 the passphrase is needed
     * - psk with hex digits (PreSharedKey) doesn't work
     */

    /*
     * From wpa_supplicant/hostapd
     * ieee80211w: Whether management frame protection (MFP) is enabled
     * 0 = disabled (default)
     * 1 = optional
     * 2 = required
     * The most common configuration options for this based on the PMF (protected
     * management frames) certification program are:
     * PMF enabled: ieee80211w=1 and wpa_key_mgmt=WPA-EAP WPA-EAP-SHA256
     * PMF required: ieee80211w=2 and wpa_key_mgmt=WPA-EAP-SHA256
     * (and similarly for WPA-PSK and WPA-PSK-SHA256 if WPA2-Personal is used)
     * WPA3-Personal-only mode: ieee80211w=2 and wpa_key_mgmt=SAE
     */

    constexpr std::uint8_t wpa2_psk_size = 64U;

    if (!is_wifi_interface(interface)) {
        return false;
    }

    if (psk.empty()) {
        // force security mode to none
        mode = network_security_t::none;
    }

    const char* key_mgt = nullptr;
    const char* psk_name = nullptr;
    const char* ieee80211w = nullptr;

    switch (mode) {
    case network_security_t::none:
        key_mgt = "NONE";
        break;
    case network_security_t::wpa2_only:
        key_mgt = "WPA-PSK";
        psk_name = "psk";
        break;
    case network_security_t::wpa3_only:
        key_mgt = "SAE";
        psk_name = "sae_password";
        ieee80211w = "2";
        break;
    case network_security_t::wpa2_and_wpa3:
    default:
        if (psk.size() == wpa2_psk_size) {
            // WPA3 doesn't support PSK (hex digits), it needs a passphrase
            key_mgt = "WPA-PSK";
            psk_name = "psk";
        } else if (psk.size() > wpa2_psk_size) {
            // WPA2 doesn't support passphrases > 63 characters
            key_mgt = "SAE";
            psk_name = "sae_password";
            ieee80211w = "2";
        } else {
            key_mgt = "WPA-PSK WPA-PSK-SHA256 SAE";
            psk_name = "psk";
            ieee80211w = "1";
        }
        break;
    }

    auto network_id_string = std::to_string(network_id);
    auto ssid_parameter = "\"" + ssid + "\"";

    auto output = run_application(wpa_cli, {"-i", interface, "set_network", network_id_string, "ssid", ssid_parameter});

    if ((output.exit_code == 0) && (psk_name != nullptr)) {
        output = run_application(wpa_cli, {"-i", interface, "set_network", network_id_string, psk_name, psk});
    }

    if (output.exit_code == 0) {
        output = run_application(wpa_cli, {"-i", interface, "set_network", network_id_string, "key_mgmt", key_mgt});
    }

    if ((output.exit_code == 0) && (ieee80211w != nullptr)) {
        output =
            run_application(wpa_cli, {"-i", interface, "set_network", network_id_string, "ieee80211w", ieee80211w});
    }

    if (hidden && (output.exit_code == 0)) {
        output = run_application(wpa_cli, {"-i", interface, "set_network", network_id_string, "scan_ssid", "1"});
    }

    return output.exit_code == 0;
}

bool WpaCliSetup::enable_network(const std::string& interface, int network_id) {
    if (!is_wifi_interface(interface)) {
        return false;
    }

    auto network_id_string = std::to_string(network_id);
    auto output = run_application(wpa_cli, {"-i", interface, "enable_network", network_id_string});
    return output.exit_code == 0;
}

bool WpaCliSetup::disable_network(const std::string& interface, int network_id) {
    if (!is_wifi_interface(interface)) {
        return false;
    }

    auto network_id_string = std::to_string(network_id);
    auto output = run_application(wpa_cli, {"-i", interface, "disable_network", network_id_string});
    return output.exit_code == 0;
}

bool WpaCliSetup::select_network(const std::string& interface, int network_id) {
    if (!is_wifi_interface(interface)) {
        return false;
    }

    auto network_id_string = std::to_string(network_id);
    auto output = run_application(wpa_cli, {"-i", interface, "select_network", network_id_string});
    return output.exit_code == 0;
}

bool WpaCliSetup::remove_network(const std::string& interface, int network_id) {
    if (!is_wifi_interface(interface)) {
        return false;
    }

    auto network_id_string = std::to_string(network_id);
    auto output = run_application(wpa_cli, {"-i", interface, "remove_network", network_id_string});
    return output.exit_code == 0;
}

bool WpaCliSetup::save_config(const std::string& interface) {
    if (!is_wifi_interface(interface)) {
        return false;
    }

    auto output = run_application(wpa_cli, {"-i", interface, "save_config"});
    return output.exit_code == 0;
}

WpaCliSetup::WifiScanList WpaCliSetup::scan_wifi(const std::string& interface) {
    WifiScanList result = {};

    if (do_scan(interface)) {
        // FIXME: is there a proper signal to check if the scan is ready? Maybe in the socket based interface
        std::this_thread::sleep_for(std::chrono::seconds(3));
        result = std::move(do_scan_results(interface));
    }

    return result;
}

WpaCliSetup::WifiNetworkList WpaCliSetup::list_networks(const std::string& interface) {
    WifiNetworkList result = {};
    if (is_wifi_interface(interface)) {
        auto output = run_application(wpa_cli, {"-i", interface, "list_networks"});
        if (output.exit_code == 0) {
            auto scan_results = output.split_output;
            if (scan_results.size() >= 2) {
                // skip header
                for (auto scan_results_it = std::next(scan_results.begin()); scan_results_it != scan_results.end();
                     ++scan_results_it) {

                    std::vector<std::string> columns;
                    std::istringstream ss(*scan_results_it);
                    for (std::string value; std::getline(ss, value, '\t');) {
                        columns.push_back(std::move(value));
                    }

                    if (columns.size() >= 2) {
                        WifiNetwork info;
                        info.network_id = std::stoi(columns[0]);
                        info.ssid = columns[1];
                        result.push_back(std::move(info));
                    }
                }
            }
        }
    }
    return result;
}

WpaCliSetup::WifiNetworkStatusList WpaCliSetup::list_networks_status(const std::string& interface) {
    WifiNetworkStatusList result = {};
    if (is_wifi_interface(interface)) {
        auto network_list = list_networks(interface);
        auto status_map = do_status(interface);
        int connected_rssi = not_connected_rssi;

        // signal_poll raises errors when not connected
        if (status_map["wpa_state"] == "COMPLETED") {
            auto signal_map = do_signal_poll(interface);
            if (auto it = signal_map.find("RSSI"); it != signal_map.end()) {
                connected_rssi = std::stoi(it->second);
            }
        }

        for (auto& i : network_list) {
            WifiNetworkStatus net;
            net.interface = interface;
            net.network_id = i.network_id;
            net.ssid = i.ssid;
            net.connected = false;
            net.signal_level = not_connected_rssi;

            auto id_it = status_map.find("id");
            auto ssid_it = status_map.find("ssid");

            if ((id_it != status_map.end()) && (ssid_it != status_map.end()) &&
                (std::stoi(id_it->second) == i.network_id) && (ssid_it->second == i.ssid)) {
                net.connected = true;
                net.signal_level = connected_rssi;
            }
            result.push_back(net);
        }
    }
    return result;
}

bool WpaCliSetup::is_wifi_interface(const std::string& interface) {
    // check if /sys/class/net/<interface>/wireless exists

    auto path = std::filesystem::path("/sys/class/net");
    path /= interface;
    path /= "wireless";

    return std::filesystem::exists(path);
}

} // namespace module
