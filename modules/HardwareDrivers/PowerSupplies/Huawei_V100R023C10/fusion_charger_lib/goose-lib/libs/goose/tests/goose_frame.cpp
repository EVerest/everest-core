// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <goose/frame.hpp>

#include "hex_to_vec.hpp"

TEST(GooseFrame, encode_decode) {
    goose::frame::GooseFrame goose_frame;
    memset(goose_frame.destination_mac_address, 0, 2);
    memset(goose_frame.source_mac_address, 0, 2);

    goose_frame.vlan_id = 2;
    goose_frame.priority = 5;

    goose_frame.appid[0] = 0x00;
    goose_frame.appid[1] = 0x01;

    strcpy(goose_frame.pdu.go_cb_ref, "PDU");
    goose_frame.pdu.time_allowed_to_live = 10000;
    strcpy(goose_frame.pdu.dat_set, "DAT_SET");
    strcpy(goose_frame.pdu.go_id, "GO_ID");
    goose_frame.pdu.timestamp = goose::frame::GooseTimestamp::from_ms(1667349763000);
    goose_frame.pdu.st_num = 1;
    goose_frame.pdu.sq_num = 0;
    goose_frame.pdu.simulation = false;
    goose_frame.pdu.conf_rev = 0;
    goose_frame.pdu.ndsCom = 0;
    goose_frame.pdu.apdu_entries.resize(1);
    goose_frame.pdu.apdu_entries[0].tag = 0x86;
    goose_frame.pdu.apdu_entries[0].value = {0, 1};

    auto encoded = goose_frame.serialize();
    ASSERT_EQ(encoded.eth_802_1q_tag.value(), 0xA002);

    auto decoded = goose::frame::GooseFrame(encoded);

    ASSERT_EQ(decoded.vlan_id, 2);
    ASSERT_EQ(decoded.priority, 5);
    ASSERT_EQ(decoded.appid[0], 0x00);
    ASSERT_EQ(decoded.appid[1], 0x01);
    ASSERT_STREQ(decoded.pdu.go_cb_ref, goose_frame.pdu.go_cb_ref);
    ASSERT_EQ(decoded.pdu.time_allowed_to_live, goose_frame.pdu.time_allowed_to_live);
    ASSERT_STREQ(decoded.pdu.dat_set, goose_frame.pdu.dat_set);
    ASSERT_STREQ(decoded.pdu.go_id, goose_frame.pdu.go_id);
    ASSERT_EQ(decoded.pdu.timestamp, goose_frame.pdu.timestamp);
    ASSERT_EQ(decoded.pdu.st_num, goose_frame.pdu.st_num);
    ASSERT_EQ(decoded.pdu.sq_num, goose_frame.pdu.sq_num);
    ASSERT_EQ(decoded.pdu.simulation, goose_frame.pdu.simulation);
    ASSERT_EQ(decoded.pdu.conf_rev, goose_frame.pdu.conf_rev);
    ASSERT_EQ(decoded.pdu.ndsCom, goose_frame.pdu.ndsCom);
    ASSERT_EQ(decoded.pdu.apdu_entries.size(), 1);
    ASSERT_EQ(decoded.pdu.apdu_entries[0].tag, 0x86);
    ASSERT_EQ(decoded.pdu.apdu_entries[0].value.size(), 2);
    ASSERT_EQ(decoded.pdu.apdu_entries[0].value[0], 0);
    ASSERT_EQ(decoded.pdu.apdu_entries[0].value[1], 1);
}
