// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "RunApplicationStub.hpp"
#include <WiFiSetup.hpp>
#include <gtest/gtest.h>

namespace {
using namespace module;
constexpr const char* example_psk = "e3003974af901976485f3e655b455791dcc20a5380f42a7839de3bfdc9d70d71";
constexpr const char* example_password = "LetMeIn2";
constexpr const char* example_long_password = "e3003974af901976485f3e655b455791dcc20a5380f42a7839de3bfdc9d70d71X";

class WpaCliSetupTest : public WpaCliSetup {
public:
    // override to support testing
    virtual bool is_wifi_interface(const std::string& interface) override {
        if (interface == "ap0") {
            return false;
        } else if (interface == "eth0") {
            return false;
        }
        return true;
    };
};

//-----------------------------------------------------------------------------
// add_network()
TEST(add_network, wired) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_EQ(obj.add_network("eth0"), -1);
}

TEST(add_network, wireless) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_NE(obj.add_network("wlan0"), -1);
}

TEST(add_network, access_point) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_EQ(obj.add_network("ap0"), -1);
}

//-----------------------------------------------------------------------------
// set_network()
TEST(set_network, none_no_psk) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.set_network("wlan0", 0, "PlusnetWireless", "", WpaCliSetup::network_security_t::none, false));
    ASSERT_FALSE(ra.psk_called);
    ASSERT_FALSE(ra.sae_password_called);
    ASSERT_FALSE(ra.ieee80211w_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_EQ(ra.key_mgmt_value, "NONE");
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, none_psk) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(
        obj.set_network("wlan0", 0, "PlusnetWireless", example_psk, WpaCliSetup::network_security_t::none, false));
    ASSERT_FALSE(ra.psk_called);
    ASSERT_FALSE(ra.sae_password_called);
    ASSERT_FALSE(ra.ieee80211w_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_EQ(ra.key_mgmt_value, "NONE");
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, none_password) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(
        obj.set_network("wlan0", 0, "PlusnetWireless", example_password, WpaCliSetup::network_security_t::none, false));
    ASSERT_FALSE(ra.psk_called);
    ASSERT_FALSE(ra.sae_password_called);
    ASSERT_FALSE(ra.ieee80211w_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_EQ(ra.key_mgmt_value, "NONE");
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, wpa2_no_psk) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.set_network("wlan0", 0, "PlusnetWireless", "", WpaCliSetup::network_security_t::wpa2_only, false));
    ASSERT_FALSE(ra.psk_called);
    ASSERT_FALSE(ra.sae_password_called);
    ASSERT_FALSE(ra.ieee80211w_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_EQ(ra.key_mgmt_value, "NONE");
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, wpa2_psk) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(
        obj.set_network("wlan0", 0, "PlusnetWireless", example_psk, WpaCliSetup::network_security_t::wpa2_only, false));
    ASSERT_TRUE(ra.psk_called);
    ASSERT_FALSE(ra.sae_password_called);
    ASSERT_FALSE(ra.ieee80211w_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_EQ(ra.key_mgmt_value, "WPA-PSK");
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, wpa2_password) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.set_network("wlan0", 0, "PlusnetWireless", example_password,
                                WpaCliSetup::network_security_t::wpa2_only, false));
    ASSERT_TRUE(ra.psk_called);
    ASSERT_FALSE(ra.sae_password_called);
    ASSERT_FALSE(ra.ieee80211w_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_EQ(ra.key_mgmt_value, "WPA-PSK");
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, wpa3_no_psk) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.set_network("wlan0", 0, "PlusnetWireless", "", WpaCliSetup::network_security_t::wpa3_only, false));
    ASSERT_FALSE(ra.psk_called);
    ASSERT_FALSE(ra.sae_password_called);
    ASSERT_FALSE(ra.ieee80211w_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_EQ(ra.key_mgmt_value, "NONE");
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, wpa3_psk) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(
        obj.set_network("wlan0", 0, "PlusnetWireless", example_psk, WpaCliSetup::network_security_t::wpa3_only, false));
    ASSERT_FALSE(ra.psk_called);
    ASSERT_TRUE(ra.sae_password_called);
    ASSERT_TRUE(ra.ieee80211w_called);
    ASSERT_EQ(ra.ieee80211w_value, "2");
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_EQ(ra.key_mgmt_value, "SAE");
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, wpa3_password) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.set_network("wlan0", 0, "PlusnetWireless", example_password,
                                WpaCliSetup::network_security_t::wpa3_only, false));
    ASSERT_FALSE(ra.psk_called);
    ASSERT_TRUE(ra.sae_password_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_TRUE(ra.ieee80211w_called);
    ASSERT_EQ(ra.ieee80211w_value, "2");
    ASSERT_EQ(ra.key_mgmt_value, "SAE");
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, wpa2_and_wpa3_no_psk) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(
        obj.set_network("wlan0", 0, "PlusnetWireless", "", WpaCliSetup::network_security_t::wpa2_and_wpa3, false));
    ASSERT_FALSE(ra.psk_called);
    ASSERT_FALSE(ra.sae_password_called);
    ASSERT_FALSE(ra.ieee80211w_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_EQ(ra.key_mgmt_value, "NONE");
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, wpa2_and_wpa3_psk) {
    // with a PSK result is same as wpa2_only
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.set_network("wlan0", 0, "PlusnetWireless", example_psk,
                                WpaCliSetup::network_security_t::wpa2_and_wpa3, false));
    ASSERT_TRUE(ra.psk_called);
    ASSERT_FALSE(ra.sae_password_called);
    ASSERT_FALSE(ra.ieee80211w_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_EQ(ra.key_mgmt_value, "WPA-PSK");
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, wpa2_and_wpa3_password) {
    // configure for both
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.set_network("wlan0", 0, "PlusnetWireless", example_password,
                                WpaCliSetup::network_security_t::wpa2_and_wpa3, false));
    ASSERT_TRUE(ra.psk_called);
    ASSERT_FALSE(ra.sae_password_called);
    ASSERT_TRUE(ra.ieee80211w_called);
    ASSERT_EQ(ra.ieee80211w_value, "1");
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_EQ(ra.key_mgmt_value, "WPA-PSK WPA-PSK-SHA256 SAE");
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, wpa2_and_wpa3_password_long) {
    // configure for WPA3 only
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.set_network("wlan0", 0, "PlusnetWireless", example_long_password,
                                WpaCliSetup::network_security_t::wpa2_and_wpa3, false));
    ASSERT_FALSE(ra.psk_called);
    ASSERT_TRUE(ra.sae_password_called);
    ASSERT_TRUE(ra.ieee80211w_called);
    ASSERT_EQ(ra.ieee80211w_value, "2");
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_EQ(ra.key_mgmt_value, "SAE");
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, wpa2) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.set_network("wlan0", 0, "PlusnetWireless", "LetMeIn2", false));
    ASSERT_TRUE(ra.psk_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, open) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.set_network("wlan0", 0, "OpenNet", "", false));
    ASSERT_FALSE(ra.psk_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_FALSE(ra.scan_ssid_called);
}

TEST(set_network, hidden_wpa2) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.set_network("wlan0", 0, "Hidden", "LetMeIn3", true));
    ASSERT_TRUE(ra.psk_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_TRUE(ra.scan_ssid_called);
}

TEST(set_network, hidden_open) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.set_network("wlan0", 0, "Hidden", "", true));
    ASSERT_FALSE(ra.psk_called);
    ASSERT_TRUE(ra.key_mgmt_called);
    ASSERT_TRUE(ra.scan_ssid_called);
}

//-----------------------------------------------------------------------------
// enable_network()
TEST(enable_network, exists) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.enable_network("wlan0", 0));
}

TEST(enable_network, doesnt_exist) {
    stub::RunApplication ra;
    ra.results["enable_network"] = {{}, {{"FAIL"}}, 0};
    WpaCliSetupTest obj;
    // still returns an exit code of 0
    ASSERT_TRUE(obj.enable_network("wlan0", 1));
}

//-----------------------------------------------------------------------------
// disable_network()
TEST(disable_network, exists) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.disable_network("wlan0", 0));
}

TEST(disable_network, doesnt_exist) {
    stub::RunApplication ra;
    ra.results["disable_network"] = {{}, {{"FAIL"}}, 0};
    WpaCliSetupTest obj;
    // still returns an exit code of 0
    ASSERT_TRUE(obj.disable_network("wlan0", 1));
}

//-----------------------------------------------------------------------------
// select_network()
TEST(select_network, exists) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.select_network("wlan0", 0));
}

TEST(select_network, doesnt_exist) {
    stub::RunApplication ra;
    ra.results["select_network"] = {{}, {{"FAIL"}}, 0};
    WpaCliSetupTest obj;
    // still returns an exit code of 0
    ASSERT_TRUE(obj.select_network("wlan0", 1));
}

//-----------------------------------------------------------------------------
// remove_network()
TEST(remove_network, exists) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.remove_network("wlan0", 0));
}

TEST(remove_network, doesnt_exist) {
    stub::RunApplication ra;
    ra.results["remove_network"] = {{}, {{"FAIL"}}, 0};
    WpaCliSetupTest obj;
    // still returns an exit code of 0
    ASSERT_TRUE(obj.remove_network("wlan0", 1));
}

TEST(remove_network, fail) {
    stub::RunApplication ra;
    ra.results["remove_network"] = {{}, {{"Invalid REMOVE_NETWORK command - at least 1 argument is required."}}, 255};
    WpaCliSetupTest obj;
    ASSERT_FALSE(obj.remove_network("wlan0", -99));
}

//-----------------------------------------------------------------------------
// save_config()
TEST(save_config, success) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_TRUE(obj.save_config("wlan0"));
}

TEST(save_config, fail) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    ASSERT_FALSE(obj.save_config("ap0"));
}

//-----------------------------------------------------------------------------
// scan_wifi()
TEST(scan_wifi, none) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    auto res = obj.scan_wifi("wlan0");
    ASSERT_TRUE(res.empty());
}

TEST(scan_wifi, some) {
    stub::RunApplication ra;
    ra.results["scan_results"] = {
        {},
        {
            {"bssid / frequency / signal level / flags / ssid"},
            {"14:49:bc:06:81:19\t2412\t-72\t[WPA2-PSK-CCMP][ESS]\tPlusnetWireless"},
            {"6a:82:8c:38:b2:a1\t2412\t-93\t[WPA2-PSK-CCMP][ESS]\t\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00"},
        },
        0};

    WpaCliSetupTest obj;
    auto res = obj.scan_wifi("wlan0");
    ASSERT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);

    module::WpaCliSetup::flags_t expected = {"WPA2-PSK-CCMP", "ESS"};

    EXPECT_EQ(res[0].bssid, "14:49:bc:06:81:19");
    EXPECT_EQ(res[0].ssid, "PlusnetWireless");
    EXPECT_EQ(res[0].frequency, 2412);
    EXPECT_EQ(res[0].signal_level, -72);
    EXPECT_EQ(res[0].flags, expected);

    EXPECT_EQ(res[1].bssid, "6a:82:8c:38:b2:a1");
    EXPECT_EQ(res[1].ssid, "\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00");
    EXPECT_EQ(res[1].frequency, 2412);
    EXPECT_EQ(res[1].signal_level, -93);
    EXPECT_EQ(res[1].flags, expected);
}

TEST(scan_wifi, more) {
    stub::RunApplication ra;
    ra.results["scan_results"] = {
        {},
        {
            {"bssid / frequency / signal level / flags / ssid"},
            {"14:49:bc:06:81:19\t2412\t-71\t[WPA2-PSK-CCMP][ESS]\tPlusnetWireless"},
            {"14:49:bc:06:81:1b\t2412\t-71\t[WPA2-PSK-CCMP][ESS]\t\\x00\\x00\\x00\\x00\\x00\\x00\\x00"},
            {"00:1e:42:33:62:07\t2462\t-89\t[WPA2-PSK-CCMP+TKIP][ESS][UTF-8]\tRUT950_6207"},
            {"b4:ba:9d:16:e2:ba\t2437\t-92\t[WPA2-PSK-CCMP][WPS][ESS]\tSKYLZMEY"},
            {"14:49:bc:06:81:1c\t2412\t-72\t[ESS]\tTesting123"},
            {"36:49:5b:f8:e1:07\t2412\t-92\t[ESS]\tEE WiFi"},
            {"14:49:bc:06:81:18\t2412\t-73\t[WPA2-PSK-CCMP][ESS]\t\\x00\\x00\\x00\\x00\\x00\\x00\\x00"},
            {"6a:82:8c:38:b2:a1\t2412\t-88\t[WPA2-PSK-CCMP][ESS]\t\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00"},
            {"18:82:8c:38:b2:a5\t2412\t-92\t[WPA2-PSK-CCMP][WPS][ESS]\tBT-3GAG3M"},
            {"6a:82:8c:38:b2:a6\t2412\t-92\t[ESS]\tEE WiFi"},
        },
        0};

    WpaCliSetupTest obj;
    auto res = obj.scan_wifi("wlan0");
    ASSERT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 10);

    module::WpaCliSetup::flags_t expected1 = {"WPA2-PSK-CCMP", "ESS"};
    module::WpaCliSetup::flags_t expected2 = {"WPA2-PSK-CCMP+TKIP", "ESS", "UTF-8"};
    module::WpaCliSetup::flags_t expected3 = {"WPA2-PSK-CCMP", "WPS", "ESS"};
    module::WpaCliSetup::flags_t expected4 = {"ESS"};

    EXPECT_EQ(res[0].bssid, "14:49:bc:06:81:19");
    EXPECT_EQ(res[0].ssid, "PlusnetWireless");
    EXPECT_EQ(res[0].frequency, 2412);
    EXPECT_EQ(res[0].signal_level, -71);
    EXPECT_EQ(res[0].flags, expected1);

    EXPECT_EQ(res[1].bssid, "14:49:bc:06:81:1b");
    EXPECT_EQ(res[1].ssid, "\\x00\\x00\\x00\\x00\\x00\\x00\\x00");
    EXPECT_EQ(res[1].frequency, 2412);
    EXPECT_EQ(res[1].signal_level, -71);
    EXPECT_EQ(res[1].flags, expected1);

    EXPECT_EQ(res[2].bssid, "00:1e:42:33:62:07");
    EXPECT_EQ(res[2].ssid, "RUT950_6207");
    EXPECT_EQ(res[2].frequency, 2462);
    EXPECT_EQ(res[2].signal_level, -89);
    EXPECT_EQ(res[2].flags, expected2);

    EXPECT_EQ(res[3].bssid, "b4:ba:9d:16:e2:ba");
    EXPECT_EQ(res[3].ssid, "SKYLZMEY");
    EXPECT_EQ(res[3].frequency, 2437);
    EXPECT_EQ(res[3].signal_level, -92);
    EXPECT_EQ(res[3].flags, expected3);

    EXPECT_EQ(res[4].bssid, "14:49:bc:06:81:1c");
    EXPECT_EQ(res[4].ssid, "Testing123");
    EXPECT_EQ(res[4].frequency, 2412);
    EXPECT_EQ(res[4].signal_level, -72);
    EXPECT_EQ(res[4].flags, expected4);

    EXPECT_EQ(res[5].bssid, "36:49:5b:f8:e1:07");
    EXPECT_EQ(res[5].ssid, "EE WiFi");
    EXPECT_EQ(res[5].frequency, 2412);
    EXPECT_EQ(res[5].signal_level, -92);
    EXPECT_EQ(res[5].flags, expected4);

    EXPECT_EQ(res[6].bssid, "14:49:bc:06:81:18");
    EXPECT_EQ(res[6].ssid, "\\x00\\x00\\x00\\x00\\x00\\x00\\x00");
    EXPECT_EQ(res[6].frequency, 2412);
    EXPECT_EQ(res[6].signal_level, -73);
    EXPECT_EQ(res[6].flags, expected1);

    EXPECT_EQ(res[7].bssid, "6a:82:8c:38:b2:a1");
    EXPECT_EQ(res[7].ssid, "\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00");
    EXPECT_EQ(res[7].frequency, 2412);
    EXPECT_EQ(res[7].signal_level, -88);
    EXPECT_EQ(res[7].flags, expected1);

    EXPECT_EQ(res[8].bssid, "18:82:8c:38:b2:a5");
    EXPECT_EQ(res[8].ssid, "BT-3GAG3M");
    EXPECT_EQ(res[8].frequency, 2412);
    EXPECT_EQ(res[8].signal_level, -92);
    EXPECT_EQ(res[8].flags, expected3);

    EXPECT_EQ(res[9].bssid, "6a:82:8c:38:b2:a6");
    EXPECT_EQ(res[9].ssid, "EE WiFi");
    EXPECT_EQ(res[9].frequency, 2412);
    EXPECT_EQ(res[9].signal_level, -92);
    EXPECT_EQ(res[9].flags, expected4);
}

//-----------------------------------------------------------------------------
// list_networks()
TEST(list_networks, none) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    auto res = obj.list_networks("wlan0");
    ASSERT_TRUE(res.empty());
}

TEST(list_networks, one) {
    stub::RunApplication ra;
    ra.results["list_networks"] = {{},
                                   {
                                       {"network id / ssid / bssid / flags"},
                                       {"0\t\tany\t[DISABLED]"},
                                   },
                                   0};

    WpaCliSetupTest obj;
    auto res = obj.list_networks("wlan0");
    ASSERT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 1);

    module::WpaCliSetup::flags_t expected = {"WPA2-PSK-CCMP", "ESS"};

    EXPECT_EQ(res[0].network_id, 0);
    EXPECT_EQ(res[0].ssid, "");
}

TEST(list_networks, two) {
    stub::RunApplication ra;
    ra.results["list_networks"] = {{},
                                   {
                                       {"network id / ssid / bssid / flags"},
                                       {"0\t\tany\t[DISABLED]"},
                                       {"1\tPlusnetWireless\tany\t[CURRENT]"},
                                   },
                                   0};

    WpaCliSetupTest obj;
    auto res = obj.list_networks("wlan0");
    ASSERT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);

    module::WpaCliSetup::flags_t expected = {"WPA2-PSK-CCMP", "ESS"};

    EXPECT_EQ(res[0].network_id, 0);
    EXPECT_EQ(res[0].ssid, "");

    EXPECT_EQ(res[1].network_id, 1);
    EXPECT_EQ(res[1].ssid, "PlusnetWireless");
}

//-----------------------------------------------------------------------------
// list_networks_status()
TEST(list_networks, not_connected) {
    stub::RunApplication ra;
    WpaCliSetupTest obj;
    auto res = obj.list_networks_status("wlan0");
    ASSERT_TRUE(res.empty());
    ASSERT_FALSE(ra.signal_poll_called);
}

TEST(list_networks, connected) {
    stub::RunApplication ra;
    ra.results["list_networks"] = {{},
                                   {
                                       {"network id / ssid / bssid / flags"},
                                       {"0\t\tany\t[DISABLED]"},
                                       {"1\tPlusnetWireless\tany\t[CURRENT]"},
                                       {"2\tHiddenNet\tany\t[DISABLED]"},
                                   },
                                   0};
    ra.results["status"] = {{},
                            {
                                {"bssid=14:49:bc:06:81:19"},
                                {"freq=2412"},
                                {"ssid=PlusnetWireless"},
                                {"id=1"},
                                {"mode=station"},
                                {"wifi_generation=4"},
                                {"pairwise_cipher=CCMP"},
                                {"group_cipher=CCMP"},
                                {"key_mgmt=WPA2-PSK"},
                                {"wpa_state=COMPLETED"},
                                {"ip_address=172.25.1.11"},
                                {"p2p_device_address=c2:ee:40:b0:57:b8"},
                                {"address=c0:ee:40:b0:57:b8"},
                                {"uuid=7dd9abf8-53f0-532b-a763-2f43537e4234"},
                            },
                            0};
    ra.results["signal_poll"] = {{},
                                 {
                                     {"RSSI=-73"},
                                     {"LINKSPEED=54"},
                                     {"NOISE=9999"},
                                     {"FREQUENCY=2412"},
                                     {"WIDTH=20 MHz"},
                                     {"CENTER_FRQ1=2412"},
                                 },
                                 0};
    WpaCliSetupTest obj;
    auto res = obj.list_networks_status("wlan0");
    ASSERT_FALSE(res.empty());
    ASSERT_TRUE(ra.signal_poll_called);
    ASSERT_EQ(res.size(), 3);

    EXPECT_EQ(res[0].interface, "wlan0");
    EXPECT_EQ(res[0].network_id, 0);
    EXPECT_EQ(res[0].ssid, "");
    EXPECT_FALSE(res[0].connected);
    EXPECT_EQ(res[0].signal_level, -100);

    EXPECT_EQ(res[1].interface, "wlan0");
    EXPECT_EQ(res[1].network_id, 1);
    EXPECT_EQ(res[1].ssid, "PlusnetWireless");
    EXPECT_TRUE(res[1].connected);
    EXPECT_EQ(res[1].signal_level, -73);

    EXPECT_EQ(res[2].interface, "wlan0");
    EXPECT_EQ(res[2].network_id, 2);
    EXPECT_EQ(res[2].ssid, "HiddenNet");
    EXPECT_FALSE(res[2].connected);
    EXPECT_EQ(res[2].signal_level, -100);
}

//-----------------------------------------------------------------------------
// is_wifi_interface()
// not tested as it is checking for files in /proc so depends on the
// machine the tests are running on

} // namespace
