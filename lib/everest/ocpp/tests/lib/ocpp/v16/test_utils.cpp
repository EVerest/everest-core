// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include <ocpp/common/utils.hpp>
#include <ocpp/v16/utils.hpp>

namespace {
using namespace ocpp::v16::utils;

// reverse the order of the arguments
inline auto common_split_string(char separator, const std::string& csl) {
    return ocpp::split_string(csl, separator);
}

TEST(CSL, ToCSL) {
    auto res = to_csl({});
    EXPECT_TRUE(res.empty());

    res = to_csl({"One"});
    EXPECT_FALSE(res.empty());
    EXPECT_EQ(res, "One");

    res = to_csl({"One", "Two"});
    EXPECT_FALSE(res.empty());
    EXPECT_EQ(res, "One,Two");

    // TODO(james-ctc): should this be detected?
    res = to_csl({"One", "Two", "Three,Four"});
    EXPECT_FALSE(res.empty());
    EXPECT_EQ(res, "One,Two,Three,Four");
}

TEST(CSL, FromCSL) {
    auto res = from_csl("");
    EXPECT_TRUE(res.empty());

    res = from_csl(",");
    EXPECT_TRUE(res.empty());

    res = from_csl(",One");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0], "One");

    res = from_csl(",One,");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0], "One");

    res = from_csl(",One,Two");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "Two");

    res = from_csl(",One,Two,");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "Two");

    res = from_csl("One");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0], "One");

    res = from_csl("One,");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0], "One");

    res = from_csl("One,Two");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "Two");

    res = from_csl("One,Two,");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "Two");

    res = from_csl("One,,Two");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "Two");

    res = from_csl("One,,,Two");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "Two");
}

TEST(CSL, SplitString) {
    auto res = split_string(',', "");
    EXPECT_TRUE(res.empty());

    res = split_string(',', ",");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "");
    EXPECT_EQ(res[1], "");

    res = split_string(',', ",One");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "");
    EXPECT_EQ(res[1], "One");

    res = split_string(',', ",One,");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 3);
    EXPECT_EQ(res[0], "");
    EXPECT_EQ(res[1], "One");
    EXPECT_EQ(res[2], "");

    res = split_string(',', ",One,Two");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 3);
    EXPECT_EQ(res[0], "");
    EXPECT_EQ(res[1], "One");
    EXPECT_EQ(res[2], "Two");

    res = split_string(',', ",One,Two,");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 4);
    EXPECT_EQ(res[0], "");
    EXPECT_EQ(res[1], "One");
    EXPECT_EQ(res[2], "Two");
    EXPECT_EQ(res[3], "");

    res = split_string(',', "One");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0], "One");

    res = split_string(',', "One,");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "");

    res = split_string(',', "One,Two");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "Two");

    res = split_string(',', "One,Two,");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 3);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "Two");
    EXPECT_EQ(res[2], "");

    res = split_string(',', "One,,Two");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 3);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "");
    EXPECT_EQ(res[2], "Two");

    res = split_string(',', "One,,,Two");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 4);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "");
    EXPECT_EQ(res[2], "");
    EXPECT_EQ(res[3], "Two");
}

TEST(CSL, SplitStringCommon) {
    // gives different results to v16::split_string

    auto res = common_split_string(',', "");
    EXPECT_TRUE(res.empty());

    res = common_split_string(',', ",");
    EXPECT_FALSE(res.empty());
    // ASSERT_EQ(res.size(), 2);
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0], "");

    res = common_split_string(',', ",One");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "");
    EXPECT_EQ(res[1], "One");

    res = common_split_string(',', ",One,");
    EXPECT_FALSE(res.empty());
    // ASSERT_EQ(res.size(), 3);
    // EXPECT_EQ(res[0], "");
    // EXPECT_EQ(res[1], "One");
    // EXPECT_EQ(res[2], "");
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "");
    EXPECT_EQ(res[1], "One");

    res = common_split_string(',', ",One,Two");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 3);
    EXPECT_EQ(res[0], "");
    EXPECT_EQ(res[1], "One");
    EXPECT_EQ(res[2], "Two");

    res = common_split_string(',', ",One,Two,");
    EXPECT_FALSE(res.empty());
    // ASSERT_EQ(res.size(), 4);
    // EXPECT_EQ(res[0], "");
    // EXPECT_EQ(res[1], "One");
    // EXPECT_EQ(res[2], "Two");
    // EXPECT_EQ(res[3], "");
    ASSERT_EQ(res.size(), 3);
    EXPECT_EQ(res[0], "");
    EXPECT_EQ(res[1], "One");
    EXPECT_EQ(res[2], "Two");

    res = common_split_string(',', "One");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0], "One");

    res = common_split_string(',', "One,");
    EXPECT_FALSE(res.empty());
    // ASSERT_EQ(res.size(), 2);
    // EXPECT_EQ(res[0], "One");
    // EXPECT_EQ(res[1], "");
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0], "One");

    res = common_split_string(',', "One,Two");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "Two");

    res = common_split_string(',', "One,Two,");
    EXPECT_FALSE(res.empty());
    // ASSERT_EQ(res.size(), 3);
    // EXPECT_EQ(res[0], "One");
    // EXPECT_EQ(res[1], "Two");
    // EXPECT_EQ(res[2], "");
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "Two");

    res = common_split_string(',', "One,,Two");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 3);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "");
    EXPECT_EQ(res[2], "Two");

    res = common_split_string(',', "One,,,Two");
    EXPECT_FALSE(res.empty());
    ASSERT_EQ(res.size(), 4);
    EXPECT_EQ(res[0], "One");
    EXPECT_EQ(res[1], "");
    EXPECT_EQ(res[2], "");
    EXPECT_EQ(res[3], "Two");
}

} // namespace
