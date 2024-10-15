// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <fusion_charger/goose/power_request.hpp>
#include <fusion_charger/goose/stop_charge_request.hpp>
#include <thread>

TEST(PowerRequirementRequest, from_pdu_real_world_test_1) {
    const char raw_data[] = "618197801543432f3024474f24506f77657252657175657374008102"
                            "2710821543432f3024474f24506f7765725265717565737400831543432f3024474f2450"
                            "6f776572526571756573740084086361bd060000000a8504000000018604000000008701"
                            "008804000000008901008a0400000008ab24860200018602ffff86020005860200008704"
                            "4479c000870440a000008602ffff8602ffff";
    std::uint8_t data[sizeof(raw_data) / 2];
    for (size_t i = 0; i < sizeof(data); ++i) {
        sscanf(&raw_data[2 * i], "%2hhx", &data[i]);
    }

    ::goose::frame::GoosePDU pdu(std::vector<std::uint8_t>(data, data + sizeof(data)));
    fusion_charger::goose::PowerRequirementRequest request;
    ASSERT_NO_THROW(request.from_pdu(pdu));

    EXPECT_EQ(request.charging_connector_no, 1);
    EXPECT_EQ(request.charging_sn, 0xffff);
    EXPECT_EQ(request.requirement_type, fusion_charger::goose::RequirementType::Charging);
    EXPECT_EQ(request.mode, fusion_charger::goose::Mode::None);
    EXPECT_FLOAT_EQ(request.voltage, 999.0f);
    EXPECT_FLOAT_EQ(request.current, 5.0f);
}

TEST(PowerRequirementRequest, from_pdu_real_world_test_2) {
    const char raw_data[] = "618197801543432f3024474f24506f77657252657175657374008102"
                            "2710821543432f3024474f24506f7765725265717565737400831543432f3024474f2450"
                            "6f776572526571756573740084086361bd1f0000000a8504000000018604000000008701"
                            "008804000000008901008a0400000008ab24860200018602ffff86020005860200008704"
                            "439a80008704424800008602ffff8602ffff";
    std::uint8_t data[sizeof(raw_data) / 2];
    for (size_t i = 0; i < sizeof(data); ++i) {
        sscanf(&raw_data[2 * i], "%2hhx", &data[i]);
    }

    ::goose::frame::GoosePDU pdu(std::vector<std::uint8_t>(data, data + sizeof(data)));
    fusion_charger::goose::PowerRequirementRequest request;
    ASSERT_NO_THROW(request.from_pdu(pdu));

    EXPECT_EQ(request.charging_connector_no, 1);
    EXPECT_EQ(request.charging_sn, 0xffff);
    EXPECT_EQ(request.requirement_type, fusion_charger::goose::RequirementType::Charging);
    EXPECT_EQ(request.mode, fusion_charger::goose::Mode::None);
    EXPECT_FLOAT_EQ(request.voltage, 309.0f);
    EXPECT_FLOAT_EQ(request.current, 50.0f);
}

TEST(PowerRequirementRequest, to_pdu_positive_test) {
    fusion_charger::goose::PowerRequirementRequest request;

    request.charging_connector_no = 0xdead;
    request.charging_sn = 0xbeef;
    request.requirement_type = fusion_charger::goose::RequirementType::Charging;
    request.mode = fusion_charger::goose::Mode::ConstantVoltage;
    request.voltage = 123.456f;
    request.current = 789.012f;

    auto time = ::goose::frame::GooseTimestamp::now();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    ::goose::frame::GoosePDU pdu = request.to_pdu(time);
    ASSERT_STREQ(pdu.go_cb_ref, "CC/0$GO$PowerRequest");
    EXPECT_EQ(pdu.time_allowed_to_live, 10000);
    ASSERT_STREQ(pdu.dat_set, "CC/0$GO$PowerRequest");
    ASSERT_STREQ(pdu.go_id, "CC/0$GO$PowerRequest");
    EXPECT_FALSE(pdu.simulation);
    EXPECT_EQ(pdu.conf_rev, 1);
    EXPECT_FALSE(pdu.ndsCom);
    EXPECT_EQ(pdu.timestamp, time);
    ASSERT_EQ(pdu.apdu_entries.size(), 8);
    EXPECT_EQ(pdu.apdu_entries[0].tag, 0x86);
    EXPECT_EQ(pdu.apdu_entries[0].value.size(), 2);
    EXPECT_EQ(pdu.apdu_entries[0].value[0], 0xde);
    EXPECT_EQ(pdu.apdu_entries[0].value[1], 0xad);
    EXPECT_EQ(pdu.apdu_entries[1].tag, 0x86);
    EXPECT_EQ(pdu.apdu_entries[1].value.size(), 2);
    EXPECT_EQ(pdu.apdu_entries[1].value[0], 0xbe);
    EXPECT_EQ(pdu.apdu_entries[1].value[1], 0xef);
    EXPECT_EQ(pdu.apdu_entries[2].tag, 0x86);
    EXPECT_EQ(pdu.apdu_entries[2].value.size(), 2);
    EXPECT_EQ(pdu.apdu_entries[2].value[0], 0x00);
    EXPECT_EQ(pdu.apdu_entries[2].value[1], 0x05);
    EXPECT_EQ(pdu.apdu_entries[3].tag, 0x86);
    EXPECT_EQ(pdu.apdu_entries[3].value.size(), 2);
    EXPECT_EQ(pdu.apdu_entries[3].value[0], 0x00);
    EXPECT_EQ(pdu.apdu_entries[3].value[1], 0x01);
    EXPECT_EQ(pdu.apdu_entries[4].tag, 0x87);
    EXPECT_EQ(pdu.apdu_entries[4].value.size(), 4);
    EXPECT_EQ(pdu.apdu_entries[4].value[0], 0x42);
    EXPECT_EQ(pdu.apdu_entries[4].value[1], 0xf6);
    EXPECT_EQ(pdu.apdu_entries[4].value[2], 0xe9);
    EXPECT_EQ(pdu.apdu_entries[4].value[3], 0x79);
    EXPECT_EQ(pdu.apdu_entries[5].tag, 0x87);
    EXPECT_EQ(pdu.apdu_entries[5].value.size(), 4);
    EXPECT_EQ(pdu.apdu_entries[5].value[0], 0x44);
    EXPECT_EQ(pdu.apdu_entries[5].value[1], 0x45);
    EXPECT_EQ(pdu.apdu_entries[5].value[2], 0x40);
    EXPECT_EQ(pdu.apdu_entries[5].value[3], 0xc5);
    EXPECT_EQ(pdu.apdu_entries[6].tag, 0x86);
    EXPECT_EQ(pdu.apdu_entries[6].value.size(), 2);
    EXPECT_EQ(pdu.apdu_entries[6].value[0], 0xff);
    EXPECT_EQ(pdu.apdu_entries[6].value[1], 0xff);
    EXPECT_EQ(pdu.apdu_entries[7].tag, 0x86);
    EXPECT_EQ(pdu.apdu_entries[7].value.size(), 2);
    EXPECT_EQ(pdu.apdu_entries[7].value[0], 0xff);
    EXPECT_EQ(pdu.apdu_entries[7].value[1], 0xff);
}

TEST(StopChargeRequest, to_pdu_positive_test) {
    fusion_charger::goose::StopChargeRequest request;
    request.charging_connector_no = 0xbeef;
    request.charging_sn = 0xdead;
    request.reason = fusion_charger::goose::StopChargeRequest::Reason::EPO_FAULT;

    auto time = ::goose::frame::GooseTimestamp::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    ::goose::frame::GoosePDU pdu = request.to_pdu(time);
    ASSERT_STREQ(pdu.go_cb_ref, "CC/0$GO$ShutdownRequest");
    ASSERT_EQ(pdu.time_allowed_to_live, 10000);
    ASSERT_STREQ(pdu.dat_set, "CC/0$GO$ShutdownRequest");
    ASSERT_STREQ(pdu.go_id, "CC/0$GO$ShutdownRequest");
    ASSERT_FALSE(pdu.simulation);
    ASSERT_EQ(pdu.conf_rev, 1);
    ASSERT_EQ(pdu.ndsCom, false);
    ASSERT_EQ(pdu.timestamp, time);
    ASSERT_EQ(pdu.apdu_entries.size(), 5);
    ASSERT_EQ(pdu.apdu_entries[0].tag, 0x86);
    ASSERT_EQ(pdu.apdu_entries[0].value.size(), 2);
    ASSERT_EQ(pdu.apdu_entries[0].value[0], 0xbe);
    ASSERT_EQ(pdu.apdu_entries[0].value[1], 0xef);
    ASSERT_EQ(pdu.apdu_entries[1].tag, 0x86);
    ASSERT_EQ(pdu.apdu_entries[1].value.size(), 2);
    ASSERT_EQ(pdu.apdu_entries[1].value[0], 0xde);
    ASSERT_EQ(pdu.apdu_entries[1].value[1], 0xad);
    ASSERT_EQ(pdu.apdu_entries[2].tag, 0x86);
    ASSERT_EQ(pdu.apdu_entries[2].value.size(), 2);
    ASSERT_EQ(pdu.apdu_entries[2].value[0], 0x10);
    ASSERT_EQ(pdu.apdu_entries[2].value[1], 0x04);
    ASSERT_EQ(pdu.apdu_entries[3].tag, 0x86);
    ASSERT_EQ(pdu.apdu_entries[3].value.size(), 2);
    ASSERT_EQ(pdu.apdu_entries[3].value[0], 0xff);
    ASSERT_EQ(pdu.apdu_entries[3].value[1], 0xff);
    ASSERT_EQ(pdu.apdu_entries[4].tag, 0x86);
    ASSERT_EQ(pdu.apdu_entries[4].value.size(), 2);
    ASSERT_EQ(pdu.apdu_entries[4].value[0], 0xff);
    ASSERT_EQ(pdu.apdu_entries[4].value[1], 0xff);
}
