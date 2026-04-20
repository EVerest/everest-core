// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
//
// Unit tests for the CPD handoff self-heal helpers:
//   cpd_handoff_self_heal        — deadline-based transition to Finished
//   cpd_handoff_should_drop_bundle — late-bundle rejection after CPD exit

#include <atomic>
#include <gtest/gtest.h>
#include <limits>
#include <type_traits>

#include "din_server.hpp"
#include "iso_server.hpp"
#include "v2g.hpp"
#include "v2g_ctx.hpp"

namespace {

// v2g_context::state is read from the extension thread (cpd_handoff_should_drop_bundle)
// while the iso_server thread writes it from inside request handlers. Enforce atomicity
// so a plain int can never be reintroduced without breaking this compile-time guard.
static_assert(std::is_same_v<decltype(v2g_context::state), std::atomic<int>>,
              "v2g_context::state must be std::atomic<int> to avoid the CPD drop-guard data race");

struct HandoffFixture : public ::testing::Test {
    v2g_context ctx{};

    void SetUp() override {
        ctx.basic_config.cpd_timeout_ms = 60000;
        ctx.evse_v2g_data.evse_sa_schedule_list_is_used = false;
        ctx.evse_v2g_data.evse_processing[PHASE_PARAMETER] = iso2_EVSEProcessingType_Finished;
        ctx.state = static_cast<int>(iso_ac_state_id::WAIT_FOR_CHARGEPARAMETERDISCOVERY);
        ctx.selected_protocol = V2G_PROTO_ISO15118_2013;
    }
};

TEST_F(HandoffFixture, DeadlineElapsedTransitionsToFinished) {
    ctx.hlc_schedule_wait.store(true);
    ctx.evse_v2g_data.evse_processing[PHASE_PARAMETER] = iso2_EVSEProcessingType_Ongoing;
    ctx.hlc_schedule_deadline_ms = 1; // already in the past

    cpd_handoff_self_heal(&ctx);

    EXPECT_FALSE(ctx.hlc_schedule_wait.load());
    EXPECT_EQ(ctx.evse_v2g_data.evse_processing[PHASE_PARAMETER], iso2_EVSEProcessingType_Finished);
    EXPECT_EQ(ctx.hlc_schedule_deadline_ms, 0);
}

TEST_F(HandoffFixture, DeadlineInFutureLeavesWaitArmed) {
    ctx.hlc_schedule_wait.store(true);
    ctx.evse_v2g_data.evse_processing[PHASE_PARAMETER] = iso2_EVSEProcessingType_Ongoing;
    ctx.hlc_schedule_deadline_ms = std::numeric_limits<long long int>::max();

    cpd_handoff_self_heal(&ctx);

    EXPECT_TRUE(ctx.hlc_schedule_wait.load());
    EXPECT_EQ(ctx.evse_v2g_data.evse_processing[PHASE_PARAMETER], iso2_EVSEProcessingType_Ongoing);
}

TEST_F(HandoffFixture, DisarmedDeadlineIsNoOp) {
    ctx.hlc_schedule_wait.store(false);
    ctx.hlc_schedule_deadline_ms = 0;

    cpd_handoff_self_heal(&ctx);

    EXPECT_FALSE(ctx.hlc_schedule_wait.load());
    EXPECT_EQ(ctx.hlc_schedule_deadline_ms, 0);
}

TEST_F(HandoffFixture, LateBundleDroppedAfterCpdExit) {
    // Session past CPD — wait may still be true, but bundle should be rejected.
    ctx.state = static_cast<int>(iso_ac_state_id::WAIT_FOR_POWERDELIVERY);
    ctx.hlc_schedule_wait.store(true);

    EXPECT_TRUE(cpd_handoff_should_drop_bundle(&ctx));
}

TEST_F(HandoffFixture, BundleDuringCpdAccepted) {
    ctx.state = static_cast<int>(iso_ac_state_id::WAIT_FOR_CHARGEPARAMETERDISCOVERY);
    ctx.hlc_schedule_wait.store(true);

    EXPECT_FALSE(cpd_handoff_should_drop_bundle(&ctx));
}

// DIN uses a narrower state enum where WAIT_FOR_CHARGEPARAMETERDISCOVERY is at
// ordinal 4, not 6 as in ISO-2 AC/DC. The drop-guard must consult the active
// protocol — comparing against the AC ordinal would leave late DIN bundles
// (state 5 = WAIT_FOR_CABLECHECK) undetected.
TEST_F(HandoffFixture, DinLateBundleDroppedAfterCpdExit) {
    ctx.selected_protocol = V2G_PROTO_DIN70121;
    ctx.state = static_cast<int>(din_state_id::WAIT_FOR_CABLECHECK);
    ctx.hlc_schedule_wait.store(true);

    EXPECT_TRUE(cpd_handoff_should_drop_bundle(&ctx));
}

TEST_F(HandoffFixture, DinBundleDuringCpdAccepted) {
    ctx.selected_protocol = V2G_PROTO_DIN70121;
    ctx.state = static_cast<int>(din_state_id::WAIT_FOR_CHARGEPARAMETERDISCOVERY);
    ctx.hlc_schedule_wait.store(true);

    EXPECT_FALSE(cpd_handoff_should_drop_bundle(&ctx));
}

} // namespace
