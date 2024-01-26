// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef SETUP_HPP
#define SETUP_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/kvs/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "WiFiSetup.hpp"
#include <regex>

namespace module {
namespace fs = std::filesystem;

struct WifiCredentials {
    std::string interface;
    std::string ssid;
    std::string psk;
    bool hidden;

    operator std::string() {

        json wifi_credentials = *this;

        return wifi_credentials.dump();
    }
};
void to_json(json& j, const WifiCredentials& k);
void from_json(const json& j, WifiCredentials& k);

struct InterfaceAndNetworkId {
    std::string interface;
    int network_id;

    operator std::string() {
        json remove_wifi = *this;

        return remove_wifi.dump();
    }
};
void to_json(json& j, const InterfaceAndNetworkId& k);
void from_json(const json& j, InterfaceAndNetworkId& k);

struct NetworkDeviceInfo {
    std::string interface;
    bool wireless = false;
    bool blocked = false;
    std::string rfkill_id;
    std::vector<std::string> ipv4;
    std::vector<std::string> ipv6;
    std::string mac;
    std::string link_type;

    operator std::string() {
        json device_info = *this;

        return device_info.dump();
    }
};
void to_json(json& j, const NetworkDeviceInfo& k);

struct SupportedSetupFeatures {
    bool setup_wifi;
    bool localization;
    bool setup_simulation;

    operator std::string() {
        json supported_setup_features = *this;

        return supported_setup_features.dump();
    }
};
void to_json(json& j, const SupportedSetupFeatures& k);

struct ApplicationInfo {
    bool initialized;
    std::string mode;
    std::string default_language;
    std::string current_language;
    std::string release_metadata_file;

    operator std::string() {
        json application_info = *this;

        return application_info.dump();
    }
};
void to_json(json& j, const ApplicationInfo& k);
} // namespace module
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    bool setup_wifi;
    bool localization;
    bool setup_simulation;
    std::string online_check_host;
    bool initialized_by_default;
    std::string release_metadata_file;
    std::string ap_interface;
    std::string ap_ipv4;
};

class Setup : public Everest::ModuleBase {
public:
    Setup() = delete;
    Setup(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider, std::unique_ptr<emptyImplBase> p_main,
          std::unique_ptr<kvsIntf> r_store, Conf& config) :
        ModuleBase(info), mqtt(mqtt_provider), p_main(std::move(p_main)), r_store(std::move(r_store)), config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<emptyImplBase> p_main;
    const std::unique_ptr<kvsIntf> r_store;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1

protected:
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1
    // insert your protected definitions here
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1

private:
    friend class LdEverest;
    void init();
    void ready();

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
    // insert your private definitions here
    std::string api_base = "everest_api/setup/";
    std::string var_base = api_base + "var/";
    std::string cmd_base = api_base + "cmd/";
    std::thread discover_network_thread;
    std::thread publish_application_info_thread;
    bool wifi_scan_enabled = false;
    std::string ap_state = "unknown";
    void publish_supported_features();
    void publish_application_info();
    void publish_hostname();
    void publish_ap_state();
    void set_default_language(std::string language);
    std::string get_default_language();
    std::string current_language;
    void set_current_language(const std::string& language);
    std::string get_current_language();
    void set_mode(std::string mode);
    std::string get_mode();
    void set_initialized(bool initialized);
    bool get_initialized();
    void discover_network();
    std::string read_type_file(const fs::path& type_path);
    std::vector<NetworkDeviceInfo> get_network_devices();
    void populate_rfkill_status(std::vector<NetworkDeviceInfo>& device_info);
    bool rfkill_unblock(std::string rfkill_id);
    bool rfkill_block(std::string rfkill_id);

    void publish_configured_networks();
    bool add_and_enable_network(const std::string& interface, const std::string& ssid, const std::string& psk,
                                bool hidden = false);
    bool remove_all_networks();
    bool reboot();
    bool is_online();
    void check_online_status();
    void enable_ap();
    void disable_ap();
    void populate_ip_addresses(std::vector<NetworkDeviceInfo>& device_info);
    WpaCliSetup::WifiScanList scan_wifi(const std::vector<NetworkDeviceInfo>& device_info);
    std::string get_hostname();
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // SETUP_HPP
