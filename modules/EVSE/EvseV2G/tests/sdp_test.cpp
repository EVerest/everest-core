// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>
#include <sdp.hpp>

namespace {

class SdpTest : public testing::Test {
protected:
    SdpTest() {
    }
};

TEST_F(SdpTest, sdp_write_header) {
    uint8_t buffer[20];
    uint16_t payload_type = 0x9001;
    uint32_t length = 367;

    EXPECT_EQ(sdp_write_header(buffer, payload_type, length), 8);

    EXPECT_EQ(buffer[0], 0x01);
    EXPECT_EQ(buffer[1], 0xFE);
    EXPECT_EQ(buffer[2], 0x90);
    EXPECT_EQ(buffer[3], 0x01);
    EXPECT_EQ(buffer[4], 0x00);
    EXPECT_EQ(buffer[5], 0x00);
    EXPECT_EQ(buffer[6], 0x01);
    EXPECT_EQ(buffer[7], 0x6F);
}

} // namespace
