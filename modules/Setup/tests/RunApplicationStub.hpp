// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef RUNAPPLICATIONSTUB_HPP
#define RUNAPPLICATIONSTUB_HPP

#include <RunApplication.hpp>

#include <map>
#include <string>

namespace stub {

class RunApplication {
public:
    static RunApplication* active_p;

    std::map<std::string, module::CmdOutput> results;
    bool signal_poll_called;
    bool psk_called;
    bool sae_password_called;
    bool key_mgmt_called;
    bool scan_ssid_called;
    bool ieee80211w_called;
    std::string key_mgmt_value;
    std::string ieee80211w_value;

    RunApplication();
    virtual ~RunApplication();

    virtual module::CmdOutput run_application(const std::string& name, std::vector<std::string> args);
};

} // namespace stub

#endif // RUNAPPLICATIONSTUB_HPP
