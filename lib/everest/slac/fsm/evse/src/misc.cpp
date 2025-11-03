// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include "misc.hpp"

#include <net/ethernet.h>

#include <slac/slac.hpp>

std::string format_mac_addr(const uint8_t* mac) {
    char string_buffer[ETH_ALEN * 2 + (ETH_ALEN - 1) + 1];
    snprintf(string_buffer, sizeof(string_buffer), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3],
             mac[4], mac[5]);
    return string_buffer;
}

std::string format_nmk(const uint8_t* nmk) {
    char string_buffer[slac::defs::NMK_LEN * 3];
    for (int i = 0; i < slac::defs::NMK_LEN; i++) {
        snprintf(string_buffer + (i * 3), sizeof(string_buffer) - (i * 3), "%02X:", nmk[i]);
    }
    return string_buffer;
}

std::string format_run_id(const uint8_t* run_id) {
    char string_buffer[2 * slac::defs::RUN_ID_LEN + 1];
    snprintf(string_buffer, sizeof(string_buffer), "%02X%02X%02X%02X%02X%02X%02X%02X", run_id[0], run_id[1], run_id[2],
             run_id[3], run_id[4], run_id[5], run_id[6], run_id[7]);
    return string_buffer;
}

std::string format_mmtype(const uint16_t mmtype) {
    char string_buffer[2 + 2 * 2 + 1];
    snprintf(string_buffer, sizeof(string_buffer), "0x%04hX", mmtype);
    return string_buffer;
}
