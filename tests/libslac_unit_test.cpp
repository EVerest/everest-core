// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
namespace libslac {
class LibSLACUnitTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(LibSLACUnitTest, test_invalid_datetime) {
    ASSERT_TRUE(1 == 1);
}
} // namespace libslac
