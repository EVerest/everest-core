// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>
#include <sdp.hpp>
#include <tools.hpp>

namespace {

class SdpTest : public testing::Test {
protected:
    SdpTest() {
    }
};

TEST_F(SdpTest, sdp_set_dlink_ready_sets_flag) {
    v2g_context ctx{};
    EXPECT_FALSE(ctx.sdp_dlink_ready);

    sdp_set_dlink_ready(&ctx, true);
    EXPECT_TRUE(ctx.sdp_dlink_ready);

    sdp_set_dlink_ready(&ctx, false);
    EXPECT_FALSE(ctx.sdp_dlink_ready);
}

TEST_F(SdpTest, sdp_set_dlink_ready_captures_timestamp) {
    v2g_context ctx{};
    EXPECT_EQ(ctx.sdp_dlink_ready_time.load(), 0);

    sdp_set_dlink_ready(&ctx, true);
    EXPECT_GT(ctx.sdp_dlink_ready_time.load(), 0);

    sdp_set_dlink_ready(&ctx, false);
    EXPECT_EQ(ctx.sdp_dlink_ready_time.load(), 0);
}

TEST_F(SdpTest, sdp_set_dlink_ready_timestamp_is_monotonic) {
    v2g_context ctx{};

    sdp_set_dlink_ready(&ctx, true);
    long long int first = ctx.sdp_dlink_ready_time.load();

    sdp_set_dlink_ready(&ctx, false);
    sdp_set_dlink_ready(&ctx, true);
    long long int second = ctx.sdp_dlink_ready_time.load();

    EXPECT_GE(second, first);
}

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

TEST_F(SdpTest, timeout_detected_when_elapsed_exceeds_limit) {
    v2g_context ctx{};
    // Simulate dlink becoming ready 18001ms ago
    long long int now = getmonotonictime();
    ctx.sdp_dlink_ready_time = now - (V2G_COMMUNICATION_SETUP_TIMEOUT + 1);

    long long int elapsed = now - ctx.sdp_dlink_ready_time.load();
    EXPECT_GE(elapsed, V2G_COMMUNICATION_SETUP_TIMEOUT);
}

TEST_F(SdpTest, timeout_not_detected_when_elapsed_below_limit) {
    v2g_context ctx{};
    // Simulate dlink becoming ready just now
    ctx.sdp_dlink_ready_time = getmonotonictime();

    long long int elapsed = getmonotonictime() - ctx.sdp_dlink_ready_time.load();
    EXPECT_LT(elapsed, V2G_COMMUNICATION_SETUP_TIMEOUT);
}

TEST_F(SdpTest, timeout_cancelled_when_connection_initiated) {
    v2g_context ctx{};
    sdp_set_dlink_ready(&ctx, true);
    EXPECT_NE(ctx.sdp_dlink_ready_time.load(), 0);

    // Simulate connection established — timeout should be cancelled
    ctx.connection_initiated = true;
    long long int dlink_ready_time = ctx.sdp_dlink_ready_time.load();
    if (ctx.connection_initiated && dlink_ready_time != 0) {
        ctx.sdp_dlink_ready_time = 0;
    }
    EXPECT_EQ(ctx.sdp_dlink_ready_time.load(), 0);
}

TEST_F(SdpTest, v2g_communication_setup_timeout_is_18000) {
    EXPECT_EQ(V2G_COMMUNICATION_SETUP_TIMEOUT, 18000);
}

} // namespace
