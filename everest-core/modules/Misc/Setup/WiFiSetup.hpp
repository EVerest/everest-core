// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef WIFISETUP_HPP
#define WIFISETUP_HPP

#include <cstdint>
#include <map>
#include <sstream>
#include <string>
#include <vector>

/**
 * SSID encoding
 * From Wikipedia:
 * "SSIDs can be zero to 32 octets long, and are, for convenience, usually
 *  in a natural language, such as English"
 *
 * wpa-cli escapes SSID strings in scan results; character values 32..126 are
 * not converted. The following conversions are applied:
 * - \"   double quote
 * - \\   backslash
 * - \e   escape (\033)
 * - \n   newline
 * - \r   return
 * - \t   tab
 * - \xnn for other values
 *
 * JSON strings are UTF-8 with \ converted to \\
 */

namespace module {

class Ssid {
private:
    using fn_p = int (Ssid::*)(std::uint8_t c);
    fn_p handler{nullptr};
    std::uint8_t tmp{0};
    bool error{false};

    int standard(std::uint8_t c);
    int backslash(std::uint8_t c);
    int hex_1(std::uint8_t c);
    int hex_2(std::uint8_t c);

    void encode(int c, std::ostringstream& ss);

public:
    std::string to_hex(const std::string& ssid);
    std::string from_hex(const std::string& hex);
};

class WpaCliSetup {
public:
    using flags_t = std::vector<std::string>;

    enum class network_security_t : std::uint8_t {
        none,
        wpa2_only,
        wpa3_only,
        wpa2_and_wpa3,
    };

    struct WifiScan {
        std::string bssid;
        std::string ssid;
        flags_t flags;
        int frequency;
        int signal_level;
    };
    using WifiScanList = std::vector<WifiScan>;

    struct WifiNetworkStatus {
        std::string interface;
        std::string ssid;
        int network_id;
        int signal_level;
        bool connected;
    };
    using WifiNetworkStatusList = std::vector<WifiNetworkStatus>;

    struct WifiNetwork {
        std::string ssid;
        int network_id;
    };
    using WifiNetworkList = std::vector<WifiNetwork>;

    using Status = std::map<std::string, std::string>;
    using Poll = std::map<std::string, std::string>;

protected:
    virtual bool do_scan(const std::string& interface);
    virtual WifiScanList do_scan_results(const std::string& interface);
    virtual Status do_status(const std::string& interface);
    virtual Poll do_signal_poll(const std::string& interface);
    virtual flags_t parse_flags(const std::string& flags);

public:
    virtual ~WpaCliSetup() = default;
    virtual int add_network(const std::string& interface);
    virtual bool set_network(const std::string& interface, int network_id, const std::string& ssid,
                             const std::string& psk, network_security_t mode, bool hidden);
    virtual bool set_network(const std::string& interface, int network_id, const std::string& ssid,
                             const std::string& psk, bool hidden) {
        return set_network(interface, network_id, ssid, psk, network_security_t::wpa2_and_wpa3, hidden);
    }
    virtual bool enable_network(const std::string& interface, int network_id);
    virtual bool disable_network(const std::string& interface, int network_id);
    virtual bool select_network(const std::string& interface, int network_id);
    virtual bool remove_network(const std::string& interface, int network_id);
    virtual bool save_config(const std::string& interface);
    virtual WifiScanList scan_wifi(const std::string& interface);
    virtual WifiNetworkList list_networks(const std::string& interface);
    virtual WifiNetworkStatusList list_networks_status(const std::string& interface);
    virtual bool is_wifi_interface(const std::string& interface);

    static std::string hex_to_ssid(const std::string& hex);
    static std::string ssid_to_hex(const std::string& ssid);
};

} // namespace module

#endif // WIFISETUP_HPP
