// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>
#include <tools.hpp>

namespace {

template <typename T, std::size_t S> constexpr auto macStr(const T (&arg)[S]) {
    return to_mac_address_str(reinterpret_cast<const uint8_t*>(arg), S - 1);
}

TEST(to_mac_address_str, various) {
    auto result = to_mac_address_str(nullptr, 0);
    EXPECT_TRUE(result.empty());
    const char* txt = "123";
    result = to_mac_address_str(reinterpret_cast<const uint8_t*>(txt), 0);
    EXPECT_TRUE(result.empty());

    result = macStr("");
    EXPECT_TRUE(result.empty());
    result = macStr("12345678901234567"); // too long
    EXPECT_TRUE(result.empty());

    result = macStr("A");
    EXPECT_EQ(result, "41");
    result = macStr("AB");
    EXPECT_EQ(result, "41:42");
    result = macStr("ABM");
    EXPECT_EQ(result, "41:42:4D");
    result = macStr("\xac\x91\xa1\x56\x5f\x46");
    EXPECT_EQ(result, "AC:91:A1:56:5F:46");
    result = macStr("1234567890123456"); // max length
    EXPECT_EQ(result, "31:32:33:34:35:36:37:38:39:30:31:32:33:34:35:36");
}

} // namespace
