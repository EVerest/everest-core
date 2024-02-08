// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
namespace libtimer {
class LibTimerUnitTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(LibTimerUnitTest, just_an_example) {
    ASSERT_TRUE(1 == 1);
}
} // namespace libtimer
