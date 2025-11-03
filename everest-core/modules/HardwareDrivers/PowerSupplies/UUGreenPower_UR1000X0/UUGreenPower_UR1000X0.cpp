// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "UUGreenPower_UR1000X0.hpp"
#include <regex>

namespace module {

static std::vector<std::string> split_by_delimeters(const std::string& s, const std::string& delimeters) {
    std::regex re("[" + delimeters + "]");
    std::sregex_token_iterator first{s.begin(), s.end(), re, -1}, last;
    return {first, last};
}

static std::vector<uint8_t> parse_module_addresses(const std::string a) {
    std::vector<uint8_t> addresses;
    auto adr = split_by_delimeters(a, ",");
    for (const auto& ad : adr) {
        addresses.push_back(std::stoi(ad));
    }
    return addresses;
}

void UUGreenPower_UR1000X0::init() {

    // open DCDC CAN device
    if (!acdc.open_device(config.can_device.c_str())) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestConfigError, "Could not open CAN interface ", config.can_device));
    }

    // configure module address
    auto module_addresses = parse_module_addresses(config.module_addresses);
    EVLOG_info << "Amount of modules: " << module_addresses.size();
    acdc.set_module_address(module_addresses);

    acdc.set_voltage_mode(config.voltage_mode);

    acdc.switch_on(false);

    invoke_init(*p_main);

    acdc.request_module_info();
}

void UUGreenPower_UR1000X0::ready() {
    invoke_ready(*p_main);
}

} // namespace module
