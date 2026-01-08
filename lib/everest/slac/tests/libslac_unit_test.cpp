// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <slac/slac.hpp>
#include <stdexcept>

namespace libslac {
class LibSLACUnitTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(LibSLACUnitTest, test_generate_nmk_hs) {
    uint8_t nmk_hs[slac::defs::NMK_LEN] = {0};
    std::string plain_password = "EVerest";

    slac::utils::generate_nmk_hs(nmk_hs, plain_password.c_str(), plain_password.length());
    ASSERT_THAT(nmk_hs, testing::ElementsAre(0xf7, 0x0f, 0x4e, 0x02, 0x2b, 0xb1, 0x08, 0x1b, 0xcb, 0xa1, 0xa0, 0x9d,
                                             0xf6, 0xe0, 0x8f, 0x6b));
}

TEST_F(LibSLACUnitTest, test_generate_nid_from_nmk) {
    uint8_t nid[slac::defs::NID_LEN] = {0};
    const uint8_t sample_nmk[] = {0x34, 0x52, 0x23, 0x54, 0x45, 0xae, 0xf2, 0xd4,
                                  0x55, 0xfe, 0xff, 0x31, 0xa3, 0xb3, 0x03, 0xad};

    slac::utils::generate_nid_from_nmk(nid, sample_nmk);
    ASSERT_THAT(nid, testing::ElementsAre(0xb1, 0xc9, 0x13, 0xb5, 0xaa, 0x07, 0x02));
}

TEST_F(LibSLACUnitTest, test_generate_nid_from_nmk_check_security_bits) {
    uint8_t nid[slac::defs::NID_LEN] = {0};
    const uint8_t sample_nmk[] = {0x34, 0x52, 0x23, 0x54, 0x45, 0xae, 0xf2, 0xd4,
                                  0x55, 0xfe, 0xff, 0x31, 0xa3, 0xb3, 0x03, 0xad};

    slac::utils::generate_nid_from_nmk(nid, sample_nmk);
    ASSERT_TRUE((nid[6] >> 4) == 0x00);
}

TEST_F(LibSLACUnitTest, test_setup_payload) {
    slac::messages::cm_set_key_cnf set_key_cnf;
    slac::messages::HomeplugMessage message;
    ASSERT_THROW(message.setup_payload(&set_key_cnf, 2000, // this is way too large
                                       (slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_CNF),
                                       slac::defs::MMV::AV_1_1),
                 std::runtime_error);
}
} // namespace libslac
