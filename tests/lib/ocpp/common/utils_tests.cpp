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

TEST(Utils, test_split_string) {
    std::vector<std::string> result = split_string("This is a test", ' ');
    ASSERT_EQ(result.size(), 4);
    EXPECT_EQ(result.at(0), "This");
    EXPECT_EQ(result.at(1), "is");
    EXPECT_EQ(result.at(2), "a");
    EXPECT_EQ(result.at(3), "test");

    result = split_string("This;is;a;test;", ' ');
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(0), "This;is;a;test;");

    result = split_string("Testing;with;google test;", ';');
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result.at(0), "Testing");
    EXPECT_EQ(result.at(1), "with");
    EXPECT_EQ(result.at(2), "google test");

    result = split_string(",", ',');
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.at(0), "");

    result = split_string("", '.');
    EXPECT_EQ(result.size(), 0);

    result = split_string("This is a test. It is performed using google test.", '.');
    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result.at(0), "This is a test");
    EXPECT_EQ(result.at(1), " It is performed using google test");
}

TEST(Utils, test_trim_string) {
    EXPECT_EQ(trim_string(""), "");
    EXPECT_EQ(trim_string(" trim this"), "trim this");
    EXPECT_EQ(trim_string("   trim this as well       "), "trim this as well");
    EXPECT_EQ(trim_string("only space at end  "), "only space at end");
}

} // namespace common
} // namespace ocpp
