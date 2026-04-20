// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
//
// Unit tests for the K16 renegotiation latch helper used by
// iso15118_extensionsImpl::handle_trigger_schedule_renegotiation.
//
// The helper sets v2g_ctx->evse_v2g_data.evse_notification to ReNegotiation
// when the notification is currently None, preserves StopCharging precedence,
// and is a no-op when a ReNegotiation latch is already pending.
#include <atomic>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

#include <cbv2g/iso_2/iso2_msgDefDatatypes.h>

#include "ISO15118_chargerImplStub.hpp"
#include "ModuleAdapterStub.hpp"
#include "evse_securityIntfStub.hpp"
#include "iso15118_extensionsImplStub.hpp"
#include "iso15118_vasIntfStub.hpp"
#include "utest_log.hpp"
#include "v2g.hpp"
#include "v2g_ctx.hpp"

#include <gtest/gtest.h>

// Helper under test lives in iso15118_extensionsImpl.cpp.
// It is in a non-anonymous testing_internal namespace so unit tests can link to it.
namespace module {
namespace extensions {
namespace testing_internal {
void trigger_renegotiation_if_idle(struct v2g_context* ctx);
} // namespace testing_internal
} // namespace extensions
} // namespace module

// The production module declares v2g_ctx as an extern global (iso15118_extensionsImpl.hpp).
// Provide a definition here so the impl translation unit links in the test binary.
// Tests construct their own v2g_context via v2g_ctx_create and pass it to the helper;
// they do not use this global.
struct v2g_context* v2g_ctx = nullptr;

// evse_notification is written from the command-handler thread (handle_stop_charging),
// the iso_server / din_server response builders, and the K16 renegotiation latch in the
// extension thread. Only the latch currently holds mqtt_lock, so the field must itself
// be atomic to avoid torn reads/writes across threads.
static_assert(
    std::is_same_v<decltype(std::declval<v2g_context&>().evse_v2g_data.evse_notification), std::atomic<std::uint8_t>>,
    "v2g_context::evse_v2g_data::evse_notification must be std::atomic<std::uint8_t>");

namespace {

struct v2g_contextDeleter {
    void operator()(v2g_context* ptr) const {
        v2g_ctx_free(ptr);
    }
};

class TriggerRenegotiationTest : public testing::Test {
protected:
    std::unique_ptr<v2g_context, v2g_contextDeleter> ctx;
    module::stub::QuietModuleAdapterStub adapter;
    module::stub::ISO15118_chargerImplStub charger;
    module::stub::evse_securityIntfStub security;
    module::stub::iso15118_extensionsImplStub extensions;
    module::stub::iso15118_vasIntfStub vas_item;

    TriggerRenegotiationTest() : charger(adapter), security(adapter), vas_item(adapter) {
    }

    void SetUp() override {
        auto ptr = v2g_ctx_create(&charger, &extensions, &security, {&vas_item});
        ctx = std::unique_ptr<v2g_context, v2g_contextDeleter>(ptr, v2g_contextDeleter());
        module::stub::clear_logs();
    }
};

TEST_F(TriggerRenegotiationTest, IdleNotificationIsLatchedToReNegotiation) {
    ctx->evse_v2g_data.evse_notification = iso2_EVSENotificationType_None;

    module::extensions::testing_internal::trigger_renegotiation_if_idle(ctx.get());

    EXPECT_EQ(ctx->evse_v2g_data.evse_notification, iso2_EVSENotificationType_ReNegotiation);
}

TEST_F(TriggerRenegotiationTest, ReNegotiationReentryIsNoOp) {
    ctx->evse_v2g_data.evse_notification = iso2_EVSENotificationType_ReNegotiation;

    module::extensions::testing_internal::trigger_renegotiation_if_idle(ctx.get());

    EXPECT_EQ(ctx->evse_v2g_data.evse_notification, iso2_EVSENotificationType_ReNegotiation);
}

TEST_F(TriggerRenegotiationTest, StopChargingPrecedenceIsHonored) {
    ctx->evse_v2g_data.evse_notification = iso2_EVSENotificationType_StopCharging;

    module::extensions::testing_internal::trigger_renegotiation_if_idle(ctx.get());

    // StopCharging wins; the renegotiation latch must not clobber it.
    EXPECT_EQ(ctx->evse_v2g_data.evse_notification, iso2_EVSENotificationType_StopCharging);
}

} // namespace
