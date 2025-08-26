// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <memory>
#include <v2g_ctx.hpp>

#include "ISO15118_chargerImplStub.hpp"
#include "ModuleAdapterStub.hpp"
#include "evse_securityIntfStub.hpp"
#include "iso15118_extensionsImplStub.hpp"
#include "utest_log.hpp"
#include "v2g.hpp"

#include <gtest/gtest.h>

namespace {

struct v2g_contextDeleter {
    void operator()(v2g_context* ptr) const {
        v2g_ctx_free(ptr);
    };
};

class V2gCtxTest : public testing::Test {
protected:
    std::unique_ptr<v2g_context, v2g_contextDeleter> ctx;
    module::stub::QuietModuleAdapterStub adapter;
    module::stub::ISO15118_chargerImplStub charger;
    module::stub::evse_securityIntfStub security;
    module::stub::iso15118_extensionsImplStub extensions;

    V2gCtxTest() : charger(adapter), security(adapter) {
    }

    void v2g_ctx_init_charging_state_cleared() {
        // checks try to match the order is v2g.hpp

        EXPECT_EQ(ctx->last_v2g_msg, V2G_UNKNOWN_MSG);
        EXPECT_EQ(ctx->current_v2g_msg, V2G_UNKNOWN_MSG);
        EXPECT_EQ(ctx->state, 0);

        // not changed
        // is_dc_charger
        // debugMode
        // supported_protocols

        EXPECT_EQ(ctx->selected_protocol, V2G_UNKNOWN_PROTOCOL);
        EXPECT_FALSE(ctx->intl_emergency_shutdown);
        EXPECT_FALSE(ctx->stop_hlc);

        // ctx->is_connection_terminated is updated rather than cleared

        // not changed
        // terminate_connection_on_failed_response
        // contactor_is_closed

        // many items in session not reset
        EXPECT_FALSE(ctx->session.renegotiation_required);
        EXPECT_FALSE(ctx->session.is_charging);
    }

    void SetUp() override {
        auto ptr = v2g_ctx_create(&charger, &extensions, &security);
        ctx = std::unique_ptr<v2g_context, v2g_contextDeleter>(ptr, v2g_contextDeleter());
        module::stub::clear_logs();
    }

    void TearDown() override {
    }
};

TEST(RunFirst, v2g_ctx_init_charging_values) {
    // must not be part of V2gCtxTest
    // V2gCtxTest::SetUp() creates the v2g_context which would be the 1st
    // call to v2g_ctx_init_charging_values()

    // only called from v2g_ctx_init_charging_session()
    // which is called from v2g_ctx_create()

    // note v2g_ctx_init_charging_values() has a static bool so it
    // performs different tidyup after the first time it is called

    v2g_context ctx;
    ctx.evse_v2g_data.charge_service.FreeService = 9;
    v2g_ctx_init_charging_values(&ctx);
    EXPECT_EQ(ctx.evse_v2g_data.charge_service.FreeService, 0);
    ctx.evse_v2g_data.charge_service.FreeService = 10;
    v2g_ctx_init_charging_values(&ctx);
    EXPECT_EQ(ctx.evse_v2g_data.charge_service.FreeService, 10);

    // reset back to a valid value as it will never be reset
    ctx.evse_v2g_data.charge_service.FreeService = 0;
}

TEST_F(V2gCtxTest, v2g_ctx_init_charging_stateTrue) {
    // called on session start in v2g_handle_connection()

    ctx->last_v2g_msg = V2G_CABLE_CHECK_MSG;
    ctx->current_v2g_msg = V2G_CHARGE_PARAMETER_DISCOVERY_MSG;
    ctx->state = 10;
    ctx->selected_protocol = V2G_PROTO_DIN70121;
    ctx->intl_emergency_shutdown = true;
    ctx->stop_hlc = true;
    ctx->session.renegotiation_required = true;
    ctx->session.is_charging = true;

    v2g_ctx_init_charging_state(ctx.get(), true);

    v2g_ctx_init_charging_state_cleared();
    EXPECT_TRUE(ctx->is_connection_terminated);
}

TEST_F(V2gCtxTest, v2g_ctx_init_charging_stateFalse) {
    // called on session end in v2g_handle_connection()

    ctx->last_v2g_msg = V2G_CABLE_CHECK_MSG;
    ctx->current_v2g_msg = V2G_CHARGE_PARAMETER_DISCOVERY_MSG;
    ctx->state = 10;
    ctx->selected_protocol = V2G_PROTO_DIN70121;
    ctx->intl_emergency_shutdown = true;
    ctx->stop_hlc = true;
    ctx->session.renegotiation_required = true;
    ctx->session.is_charging = true;

    v2g_ctx_init_charging_state(ctx.get(), false);

    v2g_ctx_init_charging_state_cleared();
    EXPECT_FALSE(ctx->is_connection_terminated);
}

#if 0
// v2g_ctx_init_charging_session() is a trivial implementation
TEST_F(V2gCtxTest, v2g_ctx_init_charging_sessionTrue) {
    // called in connection_teardown()
    // calls v2g_ctx_init_charging_state
    // calls v2g_ctx_init_charging_values
}

TEST_F(V2gCtxTest, v2g_ctx_init_charging_sessionFalse) {
    // called in connection_teardown()
    // calls v2g_ctx_init_charging_state
    // calls v2g_ctx_init_charging_values
}
#endif

TEST(valgrind, memcheck) {
    GTEST_SKIP() << "pthreads result in valgrind reporting errors";
    /*
     * v2g_ctx_free() doesn't stop or wait for threads to finish (no join)
     * hence there is access to free'd memory reported.
     *
     * ==2136== LEAK SUMMARY:
     * ==2136==    definitely lost: 0 bytes in 0 blocks
     * ==2136==    indirectly lost: 0 bytes in 0 blocks
     * ==2136==      possibly lost: 304 bytes in 1 blocks
     * ==2136==    still reachable: 80 bytes in 2 blocks
     * ==2136==         suppressed: 0 bytes in 0 blocks
     */

    // run via valgrind to ensure that malloc/free are working
    module::stub::QuietModuleAdapterStub adapter;
    module::stub::ISO15118_chargerImplStub charger(adapter);
    module::stub::evse_securityIntfStub security(adapter);
    module::stub::iso15118_extensionsImplStub extensions;
    auto ptr = v2g_ctx_create(&charger, &extensions, &security);
    v2g_ctx_free(ptr);
}

} // namespace
