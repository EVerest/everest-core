// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include "context.hpp"

#include <random>

#include <fmt/format.h>

void Context::signal_cm_slac_parm_req(const uint8_t* mac) {
    if (callbacks.signal_ev_mac_address_parm_req) {
        auto mac_string = fmt::format("{:02X}", fmt::join(mac, mac + ETH_ALEN, ":"));
        callbacks.signal_ev_mac_address_parm_req(mac_string);
    }
}

void Context::signal_cm_slac_match_cnf(const uint8_t* mac) {
    if (callbacks.signal_ev_mac_address_match_cnf) {
        auto mac_string = fmt::format("{:02X}", fmt::join(mac, mac + ETH_ALEN, ":"));
        callbacks.signal_ev_mac_address_match_cnf(mac_string);
    }
}

void Context::signal_dlink_ready(bool value) {
    if (callbacks.signal_dlink_ready) {
        callbacks.signal_dlink_ready(value);
    }
}

void Context::signal_error_routine_request() {
    if (callbacks.signal_error_routine_request) {
        callbacks.signal_error_routine_request();
    }
}

void Context::signal_state(const std::string& state) {
    if (callbacks.signal_state) {
        callbacks.signal_state(state);
    }
}

void Context::generate_nmk() {
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    for (std::size_t i = 0; i < slac::defs::NMK_LEN; ++i) {
        session_nmk[i] = (uint8_t)CHARACTERS[distribution(generator)];
    }
}

void Context::log_info(const std::string& text) {
    if (callbacks.log) {
        callbacks.log(text);
    }
}
