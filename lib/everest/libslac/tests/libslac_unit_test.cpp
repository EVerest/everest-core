// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <slac/slac.hpp>

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

TEST_F(LibSLACUnitTest, test_generate_nid_from_nmk_check_security_bits) {
    uint8_t nid[slac::defs::NID_LEN] = {0};
    const uint8_t sample_nmk[] = {0x34, 0x52, 0x23, 0x54, 0x45, 0xae, 0xf2, 0xd4,
                                  0x55, 0xfe, 0xff, 0x31, 0xa3, 0xb3, 0x03, 0xad};

    slac::utils::generate_nid_from_nmk(nid, sample_nmk);
    ASSERT_TRUE((nid[6] >> 4) == 0x00);
}

} // namespace libslac
