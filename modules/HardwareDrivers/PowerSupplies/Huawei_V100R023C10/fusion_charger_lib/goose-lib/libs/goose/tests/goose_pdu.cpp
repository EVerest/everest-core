// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <goose/frame.hpp>

TEST(GoosePDU, decode_real_world_example_1) {
    const char hex_data[] = "618197801543432f3024474f24506f776572526571756573740081022710821543432f30"
                            "24474f24506f7765725265717565737400831543432f3024474f24506f77657252657175"
                            "6573740084086361bd030000000a85040000000186040000000087010088040000000089"
                            "01008a0400000008ab24860200018602ffff860200018602000087040000000087040000"
                            "00008602ffff8602ffff";
    uint8_t data[sizeof(hex_data) / 2];
    for (size_t i = 0; i < sizeof(hex_data) / 2; i++) {
        sscanf(&hex_data[i * 2], "%2hhx", &data[i]);
    }

    goose::frame::GoosePDU pdu(std::vector<uint8_t>(data, data + sizeof(data)));
    ASSERT_STREQ(pdu.go_cb_ref, "CC/0$GO$PowerRequest");
    ASSERT_EQ(pdu.time_allowed_to_live, 10000);
    ASSERT_STREQ(pdu.dat_set, "CC/0$GO$PowerRequest");
    ASSERT_STREQ(pdu.go_id, "CC/0$GO$PowerRequest");

    ASSERT_EQ(pdu.timestamp.to_ms(), 1667349763000);
    ASSERT_EQ(pdu.st_num, 1);
    ASSERT_EQ(pdu.sq_num, 0);
    ASSERT_FALSE(pdu.simulation);
    ASSERT_EQ(pdu.conf_rev, 0);
    ASSERT_EQ(pdu.ndsCom, 0);
    ASSERT_EQ(pdu.apdu_entries.size(), 8);

    ASSERT_EQ(pdu.apdu_entries[0].tag, 0x86);
    ASSERT_EQ(pdu.apdu_entries[0].value.size(), 2);
    ASSERT_EQ(pdu.apdu_entries[0].value[0], 0);
    ASSERT_EQ(pdu.apdu_entries[0].value[1], 1);

    ASSERT_EQ(pdu.apdu_entries[1].tag, 0x86);
    ASSERT_EQ(pdu.apdu_entries[1].value.size(), 2);
    ASSERT_EQ(pdu.apdu_entries[1].value[0], 0xff);
    ASSERT_EQ(pdu.apdu_entries[1].value[1], 0xff);

    ASSERT_EQ(pdu.apdu_entries[2].tag, 0x86);
    ASSERT_EQ(pdu.apdu_entries[2].value.size(), 2);
    ASSERT_EQ(pdu.apdu_entries[2].value[0], 0);
    ASSERT_EQ(pdu.apdu_entries[2].value[1], 1);

    ASSERT_EQ(pdu.apdu_entries[3].tag, 0x86);
    ASSERT_EQ(pdu.apdu_entries[3].value.size(), 2);
    ASSERT_EQ(pdu.apdu_entries[3].value[0], 0);
    ASSERT_EQ(pdu.apdu_entries[3].value[1], 0);

    ASSERT_EQ(pdu.apdu_entries[4].tag, 0x87);
    ASSERT_EQ(pdu.apdu_entries[4].value.size(), 4);
    ASSERT_EQ(pdu.apdu_entries[4].value[0], 0);
    ASSERT_EQ(pdu.apdu_entries[4].value[1], 0);
    ASSERT_EQ(pdu.apdu_entries[4].value[2], 0);
    ASSERT_EQ(pdu.apdu_entries[4].value[3], 0);

    // rest of the fields are similar, not very interesting to test
}

TEST(GoosePDU, decode_real_world_example_2) {
    const char hex_data[] = "618197801543432f3024474f24506f776572526571756573740081022710821543432f30"
                            "24474f24506f7765725265717565737400831543432f3024474f24506f77657252657175"
                            "6573740084086361bd160000000a85040000000186040000000087010088040000000089"
                            "01008a0400000008ab24860200018602ffff86020005860200008704439a800087044248"
                            "00008602ffff8602ffff";
    uint8_t data[sizeof(hex_data) / 2];
    for (size_t i = 0; i < sizeof(hex_data) / 2; i++) {
        sscanf(&hex_data[i * 2], "%2hhx", &data[i]);
    }

    goose::frame::GoosePDU pdu(std::vector<uint8_t>(data, data + sizeof(data)));
    ASSERT_STREQ(pdu.go_cb_ref, "CC/0$GO$PowerRequest");
    ASSERT_EQ(pdu.time_allowed_to_live, 10000);
    ASSERT_STREQ(pdu.dat_set, "CC/0$GO$PowerRequest");
    ASSERT_STREQ(pdu.go_id, "CC/0$GO$PowerRequest");

    ASSERT_EQ(pdu.timestamp.to_ms(), 1667349782000);
    ASSERT_EQ(pdu.st_num, 1);
    ASSERT_EQ(pdu.sq_num, 0);
    ASSERT_FALSE(pdu.simulation);
    ASSERT_EQ(pdu.conf_rev, 0);
    ASSERT_EQ(pdu.ndsCom, 0);
    ASSERT_EQ(pdu.apdu_entries.size(), 8);

    ASSERT_EQ(pdu.apdu_entries[0].tag, 0x86);
    ASSERT_EQ(pdu.apdu_entries[0].value.size(), 2);
    ASSERT_EQ(pdu.apdu_entries[0].value[0], 0);
    ASSERT_EQ(pdu.apdu_entries[0].value[1], 1);

    ASSERT_EQ(pdu.apdu_entries[1].tag, 0x86);
    ASSERT_EQ(pdu.apdu_entries[1].value.size(), 2);
    ASSERT_EQ(pdu.apdu_entries[1].value[0], 0xff);
    ASSERT_EQ(pdu.apdu_entries[1].value[1], 0xff);

    ASSERT_EQ(pdu.apdu_entries[2].tag, 0x86);
    ASSERT_EQ(pdu.apdu_entries[2].value.size(), 2);
    ASSERT_EQ(pdu.apdu_entries[2].value[0], 0);
    ASSERT_EQ(pdu.apdu_entries[2].value[1], 5);

    ASSERT_EQ(pdu.apdu_entries[3].tag, 0x86);
    ASSERT_EQ(pdu.apdu_entries[3].value.size(), 2);
    ASSERT_EQ(pdu.apdu_entries[3].value[0], 0);
    ASSERT_EQ(pdu.apdu_entries[3].value[1], 0);

    ASSERT_EQ(pdu.apdu_entries[4].tag, 0x87);
    ASSERT_EQ(pdu.apdu_entries[4].value.size(), 4);
    ASSERT_EQ(pdu.apdu_entries[4].value[0], 0x43);
    ASSERT_EQ(pdu.apdu_entries[4].value[1], 0x9a);
    ASSERT_EQ(pdu.apdu_entries[4].value[2], 0x80);
    ASSERT_EQ(pdu.apdu_entries[4].value[3], 0x00);

    ASSERT_EQ(pdu.apdu_entries[5].tag, 0x87);
    ASSERT_EQ(pdu.apdu_entries[5].value.size(), 4);
    ASSERT_EQ(pdu.apdu_entries[5].value[0], 0x42);
    ASSERT_EQ(pdu.apdu_entries[5].value[1], 0x48);
    ASSERT_EQ(pdu.apdu_entries[5].value[2], 0x00);
    ASSERT_EQ(pdu.apdu_entries[5].value[3], 0x00);

    // rest of the fields are similar, not very interesting to test
}

TEST(GoosePDU, encode_decode_test) {
    goose::frame::GoosePDU pdu;
    strcpy(pdu.go_cb_ref, "GO_CB_REF");
    pdu.time_allowed_to_live = 10000;
    strcpy(pdu.dat_set, "DAT_SET");
    strcpy(pdu.go_id, "GO_ID");
    pdu.timestamp = goose::frame::GooseTimestamp::from_ms(1667349763000);
    pdu.st_num = 1;
    pdu.sq_num = 0;
    pdu.simulation = false;
    pdu.conf_rev = 0;
    pdu.ndsCom = 0;
    pdu.apdu_entries.resize(2);
    pdu.apdu_entries[0].tag = 0x86;
    pdu.apdu_entries[0].value = {0, 1};
    pdu.apdu_entries[1].tag = 0x87;
    pdu.apdu_entries[1].value = {0, 0, 0, 0};

    auto encoded = pdu.serialize();
    ASSERT_EQ(encoded.size(), 90);

    goose::frame::GoosePDU decoded(encoded);
    ASSERT_STREQ(decoded.go_cb_ref, "GO_CB_REF");
    ASSERT_EQ(decoded.time_allowed_to_live, 10000);
    ASSERT_STREQ(decoded.dat_set, "DAT_SET");
    ASSERT_STREQ(decoded.go_id, "GO_ID");
    ASSERT_EQ(decoded.timestamp.to_ms(), 1667349763000);
    ASSERT_EQ(decoded.st_num, 1);
    ASSERT_EQ(decoded.sq_num, 0);
    ASSERT_FALSE(decoded.simulation);
    ASSERT_EQ(decoded.conf_rev, 0);
    ASSERT_EQ(decoded.ndsCom, 0);
    ASSERT_EQ(decoded.apdu_entries.size(), 2);

    ASSERT_EQ(decoded.apdu_entries[0].tag, 0x86);
    ASSERT_EQ(decoded.apdu_entries[0].value.size(), 2);
    ASSERT_EQ(decoded.apdu_entries[0].value[0], 0);
    ASSERT_EQ(decoded.apdu_entries[0].value[1], 1);

    ASSERT_EQ(decoded.apdu_entries[1].tag, 0x87);
    ASSERT_EQ(decoded.apdu_entries[1].value.size(), 4);
    ASSERT_EQ(decoded.apdu_entries[1].value[0], 0);
    ASSERT_EQ(decoded.apdu_entries[1].value[1], 0);
    ASSERT_EQ(decoded.apdu_entries[1].value[2], 0);
    ASSERT_EQ(decoded.apdu_entries[1].value[3], 0);
}
