// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "din_server.hpp"
#include "iso_server.hpp"
#include "log.hpp"
#include "tools.hpp"

void cpd_handoff_self_heal(struct v2g_context* ctx) {
    if (!ctx->hlc_schedule_wait.load()) {
        return;
    }
    if (ctx->hlc_schedule_deadline_ms <= 0) {
        return;
    }
    if (getmonotonictime() < ctx->hlc_schedule_deadline_ms) {
        return;
    }
    dlog(DLOG_LEVEL_WARNING, "CPD handoff deadline elapsed; serving default SAScheduleList");
    ctx->hlc_schedule_wait.store(false);
    ctx->hlc_schedule_deadline_ms = 0;
    ctx->evse_v2g_data.evse_processing[PHASE_PARAMETER] = iso2_EVSEProcessingType_Finished;
}

bool cpd_handoff_should_drop_bundle(const struct v2g_context* ctx) {
    // ctx->state is indexed into a state enum that depends on the selected
    // protocol: ISO 15118-2 AC and DC both place WAIT_FOR_CHARGEPARAMETERDISCOVERY
    // at ordinal 6, but DIN 70121 places it at ordinal 4. Comparing against the
    // AC ordinal for a DIN session leaves late DIN bundles (state 5 = CableCheck)
    // undetected, so the right enum must be selected from ctx->selected_protocol.
    constexpr int ISO2_CPD_STATE = static_cast<int>(iso_ac_state_id::WAIT_FOR_CHARGEPARAMETERDISCOVERY);
    static_assert(static_cast<int>(iso_dc_state_id::WAIT_FOR_CHARGEPARAMETERDISCOVERY) == ISO2_CPD_STATE,
                  "AC and DC CPD state ordinals must match for the late-drop guard");
    constexpr int DIN_CPD_STATE = static_cast<int>(din_state_id::WAIT_FOR_CHARGEPARAMETERDISCOVERY);

    const int cpd_ordinal = (ctx->selected_protocol == V2G_PROTO_DIN70121) ? DIN_CPD_STATE : ISO2_CPD_STATE;
    return ctx->state > cpd_ordinal;
}
