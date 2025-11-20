// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include <everest/slac/fsm/ev/context.hpp>

#include <cstring>

namespace slac::fsm::ev {

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

SessionParamaters::SessionParamaters(const uint8_t* run_id_, const uint8_t* evse_mac_) {
    memcpy(run_id, run_id_, sizeof(run_id));
    memcpy(evse_mac, evse_mac_, sizeof(evse_mac));
}

} // namespace slac::fsm::ev
