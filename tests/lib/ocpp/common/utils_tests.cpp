// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <ocpp/common/utils.hpp>

namespace ocpp {
namespace common {

class UtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(UtilsTest, test_valid_datetime) {
    ASSERT_TRUE(is_rfc3339_datetime("2023-11-29T10:21:04Z"));
    ASSERT_TRUE(is_rfc3339_datetime("2019-04-12T23:20:50.5Z"));
    ASSERT_TRUE(is_rfc3339_datetime("2019-04-12T23:20:50.52Z"));
    ASSERT_TRUE(is_rfc3339_datetime("2019-04-12T23:20:50.523Z"));
    ASSERT_TRUE(is_rfc3339_datetime("2019-12-19T16:39:57+01:00"));
    ASSERT_TRUE(is_rfc3339_datetime("2019-12-19T16:39:57-01:00"));
}

TEST_F(UtilsTest, test_invalid_datetime) {
    ASSERT_FALSE(is_rfc3339_datetime("1"));
    ASSERT_FALSE(is_rfc3339_datetime("1.1"));
    ASSERT_FALSE(is_rfc3339_datetime("true"));
    ASSERT_FALSE(is_rfc3339_datetime("abc"));

    // more than 3 decimal digits are not allowed in OCPP
    ASSERT_FALSE(is_rfc3339_datetime("2023-11-29T10:21:04.0001Z"));
}

} // namespace common
} // namespace ocpp
