// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include <everest/slac/fsm/evse/context.hpp>

#include <random>

#include "misc.hpp"

namespace slac::fsm::evse {

void EvseSlacConfig::generate_nmk() {
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    for (std::size_t i = 0; i < slac::defs::NMK_LEN; ++i) {
        session_nmk[i] = (uint8_t)CHARACTERS[distribution(generator)];
    }
}

void Context::signal_cm_slac_parm_req(const uint8_t* mac) {
    if (callbacks.signal_ev_mac_address_parm_req) {
        const auto mac_string = format_mac_addr(mac);
        callbacks.signal_ev_mac_address_parm_req(mac_string);
    }
}

void Context::signal_cm_slac_match_cnf(const uint8_t* mac) {
    if (callbacks.signal_ev_mac_address_match_cnf) {
        const auto mac_string = format_mac_addr(mac);
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

void Context::log_debug(const std::string& text) {
    if (callbacks.log_debug) {
        callbacks.log_debug(text);
    }
}

void Context::log_info(const std::string& text) {
    if (callbacks.log_info) {
        callbacks.log_info(text);
    }
}

void Context::log_warn(const std::string& text) {
    if (callbacks.log_warn) {
        callbacks.log_warn(text);
    }
}

void Context::log_error(const std::string& text) {
    if (callbacks.log_error) {
        callbacks.log_error(text);
    }
}

} // namespace slac::fsm::evse
