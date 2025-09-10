// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <goose-ethernet/frame.hpp>

using namespace goose_ethernet;

TEST(EthernetFrame, deserialization_positive_test) {
    uint8_t raw_data[60] = {
        0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, // destination
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, // source
        0x08, 0x00,                         // ethertype
        0xca, 0xfe, 0xba, 0xbe, 0x00, 0x01, // payload (rest is padding)
    };

    EthernetFrame frame = EthernetFrame(raw_data, sizeof(raw_data));

    EXPECT_EQ(frame.destination[0], 0xde);
    EXPECT_EQ(frame.destination[1], 0xad);
    EXPECT_EQ(frame.destination[2], 0xbe);
    EXPECT_EQ(frame.destination[3], 0xef);
    EXPECT_EQ(frame.destination[4], 0xfe);
    EXPECT_EQ(frame.destination[5], 0xed);

    EXPECT_EQ(frame.source[0], 0x01);
    EXPECT_EQ(frame.source[1], 0x23);
    EXPECT_EQ(frame.source[2], 0x45);
    EXPECT_EQ(frame.source[3], 0x67);
    EXPECT_EQ(frame.source[4], 0x89);
    EXPECT_EQ(frame.source[5], 0xab);

    EXPECT_EQ(frame.ethertype, 0x0800);
    EXPECT_FALSE(frame.eth_802_1q_tag.has_value());
    EXPECT_EQ(frame.payload.size(), 46);
    EXPECT_EQ(frame.payload[0], 0xca);
    EXPECT_EQ(frame.payload[1], 0xfe);
    EXPECT_EQ(frame.payload[2], 0xba);
    EXPECT_EQ(frame.payload[3], 0xbe);
    EXPECT_EQ(frame.payload[4], 0x00);
    EXPECT_EQ(frame.payload[5], 0x01);

    EXPECT_FALSE(frame.ethertype_is_length());
}

TEST(EthernetFrame, deserialization_802_1Q_positive_test) {
    uint8_t raw_data[60] = {
        0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, // destination
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, // source
        0x81, 0x00,                         // 802.1Q ID
        0xfe, 0xdc,                         // 802.1Q tag
        0x08, 0x00,                         // ethertype
        0xca, 0xfe, 0xba, 0xbe, 0x00, 0x01, // payload (rest is padding)
    };

    EthernetFrame frame = EthernetFrame(raw_data, sizeof(raw_data));

    EXPECT_EQ(frame.destination[0], 0xde);
    EXPECT_EQ(frame.destination[1], 0xad);
    EXPECT_EQ(frame.destination[2], 0xbe);
    EXPECT_EQ(frame.destination[3], 0xef);
    EXPECT_EQ(frame.destination[4], 0xfe);
    EXPECT_EQ(frame.destination[5], 0xed);

    EXPECT_EQ(frame.source[0], 0x01);
    EXPECT_EQ(frame.source[1], 0x23);
    EXPECT_EQ(frame.source[2], 0x45);
    EXPECT_EQ(frame.source[3], 0x67);
    EXPECT_EQ(frame.source[4], 0x89);
    EXPECT_EQ(frame.source[5], 0xab);

    EXPECT_TRUE(frame.eth_802_1q_tag.has_value());
    EXPECT_EQ(frame.eth_802_1q_tag.value(), 0xfedc);
    EXPECT_EQ(frame.ethertype, 0x0800);

    EXPECT_EQ(frame.payload.size(), 42);
    EXPECT_EQ(frame.payload[0], 0xca);
    EXPECT_EQ(frame.payload[1], 0xfe);
    EXPECT_EQ(frame.payload[2], 0xba);
    EXPECT_EQ(frame.payload[3], 0xbe);
    EXPECT_EQ(frame.payload[4], 0x00);
    EXPECT_EQ(frame.payload[5], 0x01);

    EXPECT_FALSE(frame.ethertype_is_length());
}

TEST(EthernetFrame, deserialization_frame_too_short) {
    uint8_t raw_data[59] = {
        0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, // destination
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, // source
        0x08, 0x00,                         // ethertype
        0xca, 0xfe, 0xba, 0xbe, 0x00, 0x01, // payload (rest is padding)
    };

    EXPECT_THROW(EthernetFrame(raw_data, 59), DeserializeError);
}

TEST(EthernetFrame, deserialization_frame_802_1q_too_short) {
    uint8_t raw_data[59] = {
        0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, // destination
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, // source
        0x81, 0x00,                         // 802.1Q ID
        0xfe, 0xdc,                         // 802.1Q tag
        0x08, 0x00,                         // ethertype
        0xca, 0xfe, 0xba, 0xbe, 0x00, 0x01, // payload (rest is padding)
    };

    EXPECT_THROW(EthernetFrame(raw_data, sizeof(raw_data)), DeserializeError);
}

TEST(EthernetFrame, serialize_positive_test) {
    EthernetFrame frame;
    frame.destination[0] = 0xde;
    frame.destination[1] = 0xad;
    frame.destination[2] = 0xbe;
    frame.destination[3] = 0xef;
    frame.destination[4] = 0xfe;
    frame.destination[5] = 0xed;

    frame.source[0] = 0x01;
    frame.source[1] = 0x23;
    frame.source[2] = 0x45;
    frame.source[3] = 0x67;
    frame.source[4] = 0x89;
    frame.source[5] = 0xab;

    frame.eth_802_1q_tag = std::nullopt;
    frame.ethertype = 0x0800;

    frame.payload.resize(46);
    frame.payload[0] = 0xca;
    frame.payload[1] = 0xfe;

    auto serialized = frame.serialize();

    EXPECT_EQ(serialized.size(), 60);
    EXPECT_EQ(serialized[0], 0xde);
    EXPECT_EQ(serialized[1], 0xad);
    EXPECT_EQ(serialized[2], 0xbe);
    EXPECT_EQ(serialized[3], 0xef);
    EXPECT_EQ(serialized[4], 0xfe);
    EXPECT_EQ(serialized[5], 0xed);

    EXPECT_EQ(serialized[6], 0x01);
    EXPECT_EQ(serialized[7], 0x23);
    EXPECT_EQ(serialized[8], 0x45);
    EXPECT_EQ(serialized[9], 0x67);
    EXPECT_EQ(serialized[10], 0x89);
    EXPECT_EQ(serialized[11], 0xab);

    EXPECT_EQ(serialized[12], 0x08);
    EXPECT_EQ(serialized[13], 0x00);

    EXPECT_EQ(serialized[14], 0xca);
    EXPECT_EQ(serialized[15], 0xfe);
}

TEST(EthernetFrame, serialize_positive_test_802_1q) {
    EthernetFrame frame;
    frame.destination[0] = 0xde;
    frame.destination[1] = 0xad;
    frame.destination[2] = 0xbe;
    frame.destination[3] = 0xef;
    frame.destination[4] = 0xfe;
    frame.destination[5] = 0xed;

    frame.source[0] = 0x01;
    frame.source[1] = 0x23;
    frame.source[2] = 0x45;
    frame.source[3] = 0x67;
    frame.source[4] = 0x89;
    frame.source[5] = 0xab;

    frame.eth_802_1q_tag = 0xfedc;
    frame.ethertype = 0x0800;

    frame.payload.resize(42);
    frame.payload[0] = 0xca;
    frame.payload[1] = 0xfe;

    auto serialized = frame.serialize();

    EXPECT_EQ(serialized.size(), 60);
    EXPECT_EQ(serialized[0], 0xde);
    EXPECT_EQ(serialized[1], 0xad);
    EXPECT_EQ(serialized[2], 0xbe);
    EXPECT_EQ(serialized[3], 0xef);
    EXPECT_EQ(serialized[4], 0xfe);
    EXPECT_EQ(serialized[5], 0xed);

    EXPECT_EQ(serialized[6], 0x01);
    EXPECT_EQ(serialized[7], 0x23);
    EXPECT_EQ(serialized[8], 0x45);
    EXPECT_EQ(serialized[9], 0x67);
    EXPECT_EQ(serialized[10], 0x89);
    EXPECT_EQ(serialized[11], 0xab);

    EXPECT_EQ(serialized[12], 0x81);
    EXPECT_EQ(serialized[13], 0x00);
    EXPECT_EQ(serialized[14], 0xfe);
    EXPECT_EQ(serialized[15], 0xdc);

    EXPECT_EQ(serialized[16], 0x08);
    EXPECT_EQ(serialized[17], 0x00);

    EXPECT_EQ(serialized[18], 0xca);
    EXPECT_EQ(serialized[19], 0xfe);
}

TEST(EthernetFrame, serialize_too_short) {
    EthernetFrame frame;
    frame.eth_802_1q_tag = std::nullopt;
    frame.payload.resize(45);
    ASSERT_THROW(frame.serialize(), SerializeError);
    frame.payload.resize(46);
    ASSERT_NO_THROW(frame.serialize());

    frame.eth_802_1q_tag = 0xfedc;
    frame.payload.resize(41);
    ASSERT_THROW(frame.serialize(), SerializeError);
    frame.payload.resize(42);
    ASSERT_NO_THROW(frame.serialize());
}

TEST(EthernetFrame, serialize_too_long) {
    EthernetFrame frame;
    frame.eth_802_1q_tag = std::nullopt;
    frame.payload.resize(1500);
    ASSERT_NO_THROW(frame.serialize());
    frame.payload.resize(1501);
    ASSERT_THROW(frame.serialize(), SerializeError);

    frame.eth_802_1q_tag = 0xfedc;
    frame.payload.resize(1500);
    ASSERT_NO_THROW(frame.serialize());
    frame.payload.resize(1501);
    ASSERT_THROW(frame.serialize(), SerializeError);
}

TEST(EthernetFrame, ethertype_is_length) {
    EthernetFrame frame;
    frame.ethertype = 1500;
    EXPECT_TRUE(frame.ethertype_is_length());
    frame.ethertype = 0x0800;
    EXPECT_FALSE(frame.ethertype_is_length());
}

TEST(EthernetFrame, serialize_deserialize) {
    EthernetFrame frame;
    frame.destination[0] = 0xde;
    frame.destination[1] = 0xad;
    frame.destination[2] = 0xbe;
    frame.destination[3] = 0xef;
    frame.destination[4] = 0xfe;
    frame.destination[5] = 0xed;

    frame.source[0] = 0x01;
    frame.source[1] = 0x23;
    frame.source[2] = 0x45;
    frame.source[3] = 0x67;
    frame.source[4] = 0x89;
    frame.source[5] = 0xab;

    frame.eth_802_1q_tag = 0xfedc;
    frame.ethertype = 0x0800;

    frame.payload.resize(42);
    frame.payload[0] = 0xca;
    frame.payload[1] = 0xfe;

    auto serialized = frame.serialize();

    EthernetFrame deserialized = EthernetFrame(serialized);
    ASSERT_EQ(deserialized.destination[0], 0xde);
    ASSERT_EQ(deserialized.destination[1], 0xad);
    ASSERT_EQ(deserialized.destination[2], 0xbe);
    ASSERT_EQ(deserialized.destination[3], 0xef);
    ASSERT_EQ(deserialized.destination[4], 0xfe);
    ASSERT_EQ(deserialized.destination[5], 0xed);

    ASSERT_EQ(deserialized.source[0], 0x01);
    ASSERT_EQ(deserialized.source[1], 0x23);
    ASSERT_EQ(deserialized.source[2], 0x45);
    ASSERT_EQ(deserialized.source[3], 0x67);
    ASSERT_EQ(deserialized.source[4], 0x89);
    ASSERT_EQ(deserialized.source[5], 0xab);

    ASSERT_TRUE(deserialized.eth_802_1q_tag.has_value());
    ASSERT_EQ(deserialized.eth_802_1q_tag.value(), 0xfedc);
    ASSERT_EQ(deserialized.ethertype, 0x0800);

    ASSERT_EQ(deserialized.payload.size(), 42);
    ASSERT_EQ(deserialized.payload[0], 0xca);
    ASSERT_EQ(deserialized.payload[1], 0xfe);
}
