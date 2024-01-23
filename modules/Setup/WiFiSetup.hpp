// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef WIFISETUP_HPP
#define WIFISETUP_HPP

#include <map>
#include <string>
#include <vector>

namespace module {

class WpaCliSetup {
public:
    typedef std::vector<std::string> flags_t;

    struct WifiScan {
        std::string bssid;
        std::string ssid;
        int frequency;
        int signal_level;
        flags_t flags;
    };
    typedef std::vector<WifiScan> WifiScanList;

    struct WifiNetworkStatus {
        std::string interface;
        int network_id;
        std::string ssid;
        bool connected;
        int signal_level;
    };
    typedef std::vector<WifiNetworkStatus> WifiNetworkStatusList;

    struct WifiNetwork {
        int network_id;
        std::string ssid;
    };
    typedef std::vector<WifiNetwork> WifiNetworkList;

    typedef std::map<std::string, std::string> Status;
    typedef std::map<std::string, std::string> Poll;

protected:
    virtual bool do_scan(const std::string& interface);
    virtual WifiScanList do_scan_results(const std::string& interface);
    virtual Status do_status(const std::string& interface);
    virtual Poll do_signal_poll(const std::string& interface);
    virtual flags_t parse_flags(const std::string& flags);

public:
    virtual ~WpaCliSetup() {
    }
    virtual int add_network(const std::string& interface);
    virtual bool set_network(const std::string& interface, int network_id, const std::string& ssid,
                             const std::string& psk, bool hidden = false);
    virtual bool enable_network(const std::string& interface, int network_id);
    virtual bool disable_network(const std::string& interface, int network_id);
    virtual bool select_network(const std::string& interface, int network_id);
    virtual bool remove_network(const std::string& interface, int network_id);
    virtual bool save_config(const std::string& interface);
    virtual WifiScanList scan_wifi(const std::string& interface);
    virtual WifiNetworkList list_networks(const std::string& interface);
    virtual WifiNetworkStatusList list_networks_status(const std::string& interface);
    virtual bool is_wifi_interface(const std::string& interface);
};

} // namespace module

#endif // WIFISETUP_HPP
