// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <chrono>
#include <goose/frame.hpp>
#include <thread>

using namespace goose::frame;

TEST(GooseTimestamp, from_ms_works) {
    auto timestamp = GooseTimestamp::from_ms(500);
    ASSERT_EQ(timestamp.seconds, 0);
    ASSERT_EQ(timestamp.fraction, 0x800000);
    ASSERT_EQ(timestamp.quality_of_time, 0x0a);

    timestamp = GooseTimestamp::from_ms(1000);
    ASSERT_EQ(timestamp.seconds, 1);
    ASSERT_EQ(timestamp.fraction, 0);
    ASSERT_EQ(timestamp.quality_of_time, 0x0a);
}

TEST(GooseTimestamp, to_ms_works) {
    auto timestamp = GooseTimestamp::from_ms(1500);
    ASSERT_EQ(timestamp.to_ms(), 1500);
}

TEST(GooseTimestamp, encode_works) {
    auto timestamp = GooseTimestamp::from_ms(1500);
    auto encoded = timestamp.encode();
    ASSERT_EQ(encoded.size(), 8);
    ASSERT_EQ(encoded[0], 0);
    ASSERT_EQ(encoded[1], 0);
    ASSERT_EQ(encoded[2], 0);
    ASSERT_EQ(encoded[3], 1);
    ASSERT_EQ(encoded[4], 0x80);
    ASSERT_EQ(encoded[5], 0);
    ASSERT_EQ(encoded[6], 0);
    ASSERT_EQ(encoded[7], 0x0a);
}

TEST(GooseTimestamp, parsing_works) {
    std::vector<std::uint8_t> raw = {0, 0, 0, 1, 0x80, 0, 0, 0x0a};
    GooseTimestamp timestamp(raw);
    ASSERT_EQ(timestamp.seconds, 1);
    ASSERT_EQ(timestamp.fraction, 0x800000);
    ASSERT_EQ(timestamp.quality_of_time, 0x0a);
}

TEST(GooseTimestamp, now_works) {
    auto timestamp = GooseTimestamp::now();

    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    auto diff = now_ms - timestamp.to_ms();

    ASSERT_LT(abs(diff), 10);
}

TEST(GooseTimestamp, parsing_invalid_size) {
    std::vector<std::uint8_t> raw = {0, 0, 0, 1, 0x80, 0, 0, 0x0a, 0xff};
    ASSERT_THROW(GooseTimestamp timestamp(raw), std::runtime_error);
}
