// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include <everest/staging/helpers/helpers.hpp>

using namespace everest::staging::helpers;
using ::testing::StartsWith;

TEST(HelpersTest, redact_token) {
    std::string token = "secret token";

    auto redacted = redact(token);

    EXPECT_THAT(redacted, StartsWith("[redacted] hash: "));
}
