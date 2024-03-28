// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include "plc_emu.hpp"

#include <cstdio>
#include <cstring>
#include <random>

#include <unistd.h>

#include <slac/slac.hpp>

// NOTE (aw): this is only intended to be used for this test, if multiple instances use these handle functions, they
// will race the buffer
static slac::messages::HomeplugMessage homeplug_message;

constexpr static uint8_t EV_MAC_ADDR[ETH_ALEN] = {0x00, 0x7d, 0xfa, 0x09, 0xfe, 0x76};
constexpr static uint8_t EVSE_MAC_ADDR[ETH_ALEN] = {0x6e, 0x3f, 0x46, 0x32, 0xbf, 0xc6};

constexpr static uint8_t PLC_SRC_MAC_ADDR[ETH_ALEN] = {0x00, 0x01, 0x87, 0x0e, 0xa3, 0x55};

static void handle_set_key_req(int origin_fd) {
    slac::messages::cm_set_key_cnf set_key_cnf;
    // FIXME (aw): proper message and mac header setup!
    homeplug_message.setup_payload(&set_key_cnf, sizeof(set_key_cnf),
                                   (slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_CNF),
                                   slac::defs::MMV::AV_1_1);

    auto raw = homeplug_message.get_raw_message_ptr();
    memcpy(raw->ethernet_header.ether_dhost, raw->ethernet_header.ether_shost,
           sizeof(raw->ethernet_header.ether_dhost));
    memcpy(raw->ethernet_header.ether_shost, PLC_SRC_MAC_ADDR, sizeof(PLC_SRC_MAC_ADDR));

    write(origin_fd, raw, homeplug_message.get_raw_msg_len());
}

static void attach_atten_profile(int evse_bridge_fd) {
    slac::messages::cm_atten_profile_ind atten_profile;

    memcpy(atten_profile.pev_mac, homeplug_message.get_src_mac(), sizeof(atten_profile.pev_mac));
    atten_profile.num_groups = slac::defs::AAG_LIST_LEN;

    std::random_device rnd_dev;
    std::mt19937 rng(rnd_dev());
    std::uniform_int_distribution<std::mt19937::result_type> db_dist(20, 27);

    for (auto i = 0; i < atten_profile.num_groups; ++i) {
        atten_profile.aag[i] = db_dist(rng);
    }

    homeplug_message.setup_payload(&atten_profile, sizeof(atten_profile),
                                   (slac::defs::MMTYPE_CM_ATTEN_PROFILE | slac::defs::MMTYPE_MODE_IND),
                                   slac::defs::MMV::AV_1_1);

    auto raw = homeplug_message.get_raw_message_ptr();

    memcpy(raw->ethernet_header.ether_shost, PLC_SRC_MAC_ADDR, sizeof(PLC_SRC_MAC_ADDR));

    write(evse_bridge_fd, raw, homeplug_message.get_raw_msg_len());
}

void handle_ev_input(int ev_bridge_fd, int evse_bridge_fd) {
    auto raw_hp_message = homeplug_message.get_raw_message_ptr();
    auto bytes_read = read(ev_bridge_fd, raw_hp_message, sizeof(slac::messages::homeplug_message));

    const auto mmtype = homeplug_message.get_mmtype();

    // printf("EV send message of size %d and type 0x%hx\n", bytes_read, mmtype);

    // patch in "our" mac address
    memcpy(raw_hp_message->ethernet_header.ether_shost, EV_MAC_ADDR, sizeof(EV_MAC_ADDR));

    if (mmtype == (slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_REQ)) {
        handle_set_key_req(ev_bridge_fd);
    } else {
        // default: forward message
        write(evse_bridge_fd, raw_hp_message, bytes_read);
    }

    if (mmtype == (slac::defs::MMTYPE_CM_MNBC_SOUND | slac::defs::MMTYPE_MODE_IND)) {
        // also attach CM_ATTEN_PROFILE.IND
        attach_atten_profile(evse_bridge_fd);
    }
}

void handle_evse_input(int evse_bridge_fd, int ev_bridge_fd) {
    auto raw_hp_message = homeplug_message.get_raw_message_ptr();
    auto bytes_read = read(evse_bridge_fd, raw_hp_message, sizeof(slac::messages::homeplug_message));

    const auto mmtype = homeplug_message.get_mmtype();

    // patch in "our" mac address
    memcpy(raw_hp_message->ethernet_header.ether_shost, EVSE_MAC_ADDR, sizeof(EVSE_MAC_ADDR));

    if (mmtype == (slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_REQ)) {
        handle_set_key_req(evse_bridge_fd);
    } else {
        // default: forward message
        write(ev_bridge_fd, raw_hp_message, bytes_read);
    }
}
