// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <goose/ber.hpp>

TEST(BEREntry, encode_small_length) {
    goose::frame::ber::BEREntry entry;
    entry.tag = 0x01;
    entry.value = {0x03, 0x04};

    std::vector<uint8_t> encoded = entry.encode();
    ASSERT_EQ(encoded.size(), 4);
    ASSERT_EQ(encoded[0], 0x01);
    ASSERT_EQ(encoded[1], 0x02);
    ASSERT_EQ(encoded[2], 0x03);
    ASSERT_EQ(encoded[3], 0x04);
}

TEST(BEREntry, encode_mid_length) {
    goose::frame::ber::BEREntry entry;
    entry.tag = 0x01;
    entry.value.resize(200);
    entry.value[0] = 0xde;
    entry.value[1] = 0xad;

    std::vector<uint8_t> encoded = entry.encode();
    ASSERT_EQ(encoded.size(), 203);
    ASSERT_EQ(encoded[0], 0x01);
    ASSERT_EQ(encoded[1], 0x81);
    ASSERT_EQ(encoded[2], 200);
    ASSERT_EQ(encoded[3], 0xde);
    ASSERT_EQ(encoded[4], 0xad);
}

TEST(BEREntry, encode_large_length) {
    goose::frame::ber::BEREntry entry;
    entry.tag = 0x01;
    entry.value.resize(0x100);
    entry.value[0] = 0xde;
    entry.value[1] = 0xad;

    std::vector<uint8_t> encoded = entry.encode();
    ASSERT_EQ(encoded.size(), 4 + 0x100);
    ASSERT_EQ(encoded[0], 0x01);
    ASSERT_EQ(encoded[1], 0x82);
    ASSERT_EQ(encoded[2], 0x01);
    ASSERT_EQ(encoded[3], 0x00);
    ASSERT_EQ(encoded[4], 0xde);
    ASSERT_EQ(encoded[5], 0xad);
}

TEST(BEREntry, decode_small_frame) {
    std::vector<uint8_t> input = {0xde, 0x05, 0xca, 0xfe, 0xba, 0xbe, 0xef};
    goose::frame::ber::BEREntry entry(&input);

    ASSERT_EQ(entry.tag, 0xde);
    ASSERT_EQ(entry.value.size(), 5);
    ASSERT_EQ(entry.value[0], 0xca);
    ASSERT_EQ(entry.value[1], 0xfe);
    ASSERT_EQ(entry.value[2], 0xba);
    ASSERT_EQ(entry.value[3], 0xbe);
    ASSERT_EQ(entry.value[4], 0xef);

    ASSERT_EQ(input.size(), 0);
}

TEST(BEREntry, decode_multiple_frames) {
    std::vector<uint8_t> input = {
        0xde, 0x05, 0xca, 0xfe, 0xba, 0xbe, 0xef,      // First frame
        0xad, 0x02, 0xbe, 0xef,                        // Second frame
        0xca, 0x06, 0xde, 0xad, 0xbe, 0xef, 0xca, 0xfe // Third frame
    };

    goose::frame::ber::BEREntry entry1(&input);
    ASSERT_EQ(entry1.tag, 0xde);
    ASSERT_EQ(entry1.value.size(), 5);

    goose::frame::ber::BEREntry entry2(&input);
    ASSERT_EQ(entry2.tag, 0xad);
    ASSERT_EQ(entry2.value.size(), 2);

    goose::frame::ber::BEREntry entry3(&input);
    ASSERT_EQ(entry3.tag, 0xca);
    ASSERT_EQ(entry3.value.size(), 6);

    ASSERT_EQ(input.size(), 0);
}

TEST(BEREntry, decode_long_frame) {
    std::vector<uint8_t> input = {
        0xde,             // tag
        0x82, 0x01, 0x00, // length: 0x100
        0xca, 0xfe,       // first 2 bytes of payload
    };
    input.resize(0x100 + 4);

    goose::frame::ber::BEREntry entry(&input);

    ASSERT_EQ(entry.tag, 0xde);
    ASSERT_EQ(entry.value.size(), 0x100);
    ASSERT_EQ(entry.value[0], 0xca);
    ASSERT_EQ(entry.value[1], 0xfe);

    ASSERT_EQ(input.size(), 0);
}

TEST(BEREntry, decode_mid_length_frame) {
    std::vector<uint8_t> input = {
        0xde,       // tag
        0x81, 200,  // length: 200
        0xca, 0xfe, // first 2 bytes of payload
    };
    input.resize(3 + 200);

    goose::frame::ber::BEREntry entry(&input);

    ASSERT_EQ(entry.tag, 0xde);
    ASSERT_EQ(entry.value.size(), 200);
    ASSERT_EQ(entry.value[0], 0xca);
    ASSERT_EQ(entry.value[1], 0xfe);

    ASSERT_EQ(input.size(), 0);
}

// todo: PrimitiveBEREntry
