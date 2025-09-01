// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include <everest/helpers/helpers.hpp>

using namespace everest::helpers;
using ::testing::StartsWith;

TEST(HelpersTest, redact_token) {
    std::string token = "secret token";

    auto redacted = redact(token);

    EXPECT_THAT(redacted, StartsWith("[redacted] hash: "));
}

TEST(HelpersTest, get_uuid) {
    auto uuid1 = get_uuid();
    auto uuid2 = get_uuid();

    EXPECT_GT(uuid1.length(), 0);
    EXPECT_GT(uuid2.length(), 0);
    EXPECT_EQ(uuid1.length(), uuid2.length());
    EXPECT_NE(uuid1, uuid2);
}

TEST(HelpersTest, get_base64_uuid) {
    auto id1 = get_base64_uuid();
    auto id2 = get_base64_uuid();

    EXPECT_EQ(id1.length(), 22);
    EXPECT_EQ(id2.length(), 22);
    EXPECT_NE(id1, id2);
}

TEST(HelpersTest, get_base64_id) {
    auto id1 = get_base64_id();
    auto id2 = get_base64_id();

    EXPECT_EQ(id1.length(), 16);
    EXPECT_EQ(id2.length(), 16);
    EXPECT_NE(id1, id2);
}