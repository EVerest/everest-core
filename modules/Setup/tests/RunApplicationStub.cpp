// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "RunApplicationStub.hpp"
#include <gtest/gtest.h>

#include <utility>

namespace stub {

RunApplication* RunApplication::active_p = nullptr;

RunApplication::RunApplication() :
    results({
        {"add_network", {{}, {{"0"}}, 0}},
        {"set_network", {{}, {{"OK"}}, 0}},
        {"enable_network", {{}, {{"OK"}}, 0}},
        {"disable_network", {{}, {{"OK"}}, 0}},
        {"select_network", {{}, {{"OK"}}, 0}},
        {"remove_network", {{}, {{"OK"}}, 0}},
        {"save_config", {{}, {{"OK"}}, 0}},
        // scan_wifi uses scan and scan_results
        {"scan", {{}, {{"OK"}}, 0}},
        {"scan_results",
         {{},
          {
              {"bssid / frequency / signal level / flags / ssid"},
          },
          0}},
        {"list_networks",
         {{},
          {
              {"network id / ssid / bssid / flags"},
          },
          0}},
        // list_networks_status uses list_networks status signal_poll
        {"status",
         {{},
          {
              {"wpa_state=INACTIVE"},
              {"p2p_device_address=c2:ee:40:b0:57:b8"},
              {"address=c0:ee:40:b0:57:b8"},
              {"uuid=7dd9abf8-53f0-532b-a763-2f43537e4234"},
          },
          0}},
        {"signal_poll", {{}, {{"FAIL"}}, 0}},
    }),
    signal_poll_called(false),
    psk_called(false),
    sae_password_called(false),
    key_mgmt_called(false),
    scan_ssid_called(false),
    ieee80211w_called(false),
    key_mgmt_value(),
    ieee80211w_value() {
    active_p = this;
}

RunApplication::~RunApplication() {
    active_p = nullptr;
}

module::CmdOutput RunApplication::run_application(const std::string& name, std::vector<std::string> args) {
    module::CmdOutput result = {{}, {}, -1};
    EXPECT_EQ(name, "/usr/sbin/wpa_cli");
    EXPECT_EQ(args[0], "-i");
    if (args[2] == "signal_poll") {
        signal_poll_called = true;
    } else if (args[2] == "set_network") {
        if (args[4] == "psk") {
            psk_called = true;
        } else if (args[4] == "sae_password") {
            sae_password_called = true;
        } else if (args[4] == "key_mgmt") {
            key_mgmt_called = true;
            key_mgmt_value = args[5];
        } else if (args[4] == "ieee80211w") {
            ieee80211w_called = true;
            ieee80211w_value = args[5];
        } else if (args[4] == "scan_ssid") {
            scan_ssid_called = true;
        }
    }
    auto it = results.find(args[2]);
    if (it != results.end()) {
        result = it->second;
        if (!result.split_output.empty() && result.output.empty()) {
            for (auto& line : result.output) {
                result.output += line + "\n";
            }
        }
    }
    return result;
}

} // namespace stub

namespace module {
CmdOutput run_application(const std::string& name, std::vector<std::string> args) {
    CmdOutput result = {{}, {}, -1};
    if (stub::RunApplication::active_p != nullptr) {
        result = std::move(stub::RunApplication::active_p->run_application(name, args));
    }
    return result;
}
} // namespace module
