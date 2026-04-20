// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "iso15118_extensionsImpl.hpp"

#include <everest/util/async/monitor.hpp>

namespace module {
namespace extensions {

namespace {
// Per-module HLC smart charging coordination state. The bundle and status flags live here until
// libiso15118's d20 session state machine is extended to consult them during ScheduleExchangeReq
// handling (K15 / K18 / K19). Guarded by a monitor so the iso thread and the cmd handler thread
// can read/write without races once the integration lands.
struct ScheduleCoordination {
    bool wait_for_schedule{false};
    bool bundle_ready{false};
    bool fallback_requested{false};
    // K16 renegotiation latch — mirrors EvseV2G's v2g_ctx->evse_v2g_data.evse_notification latch.
    // Set by handle_trigger_schedule_renegotiation when OCPP201 observes a composite schedule
    // change for an active HLC session (K16.FR.02 / K16.FR.11). Consumed by the d20 state
    // machine to emit EvseNotification::ScheduleRenegotiation on the next ChargeLoopRes.
    // The consumer is blocked on libiso15118 exposing a mutator on d20::Session (or a new
    // ControlEvent variant); see the "Blocked follow-ups" section in
    // docs/superpowers/plans/2026-04-17-k16-renegotiation-and-hlc-handoff-followups.md.
    bool renegotiation_pending{false};
    types::iso15118::OcppEvChargingSchedules bundle;
};
everest::lib::util::monitor<ScheduleCoordination> g_schedule_coordination;
} // namespace

void iso15118_extensionsImpl::init() {
}

void iso15118_extensionsImpl::ready() {
}

void iso15118_extensionsImpl::handle_set_get_certificate_response(
    types::iso15118::ResponseExiStreamStatus& certificate_response) {
    // your code for cmd set_get_certificate_response goes here
}

void iso15118_extensionsImpl::handle_set_hlc_schedule_wait(bool& wait) {
    auto handle = g_schedule_coordination.handle();
    if (wait) {
        // K15.FR.03: hold ScheduleExchangeRes at Ongoing until the CSMS schedule arrives.
        handle->wait_for_schedule = true;
        handle->bundle_ready = false;
        handle->fallback_requested = false;
    } else {
        handle->wait_for_schedule = false;
        handle->fallback_requested = true;
    }
    EVLOG_info << "ISO 15118-20 HLC schedule wait = " << (wait ? "true" : "false");
}

types::iso15118::SetChargingSchedulesResult iso15118_extensionsImpl::handle_set_ev_charging_schedules(
    types::iso15118::OcppEvChargingSchedules& charging_schedules) {
    if (charging_schedules.schedules.empty()) {
        return {types::iso15118::SetChargingSchedulesStatus::Rejected, std::string("empty schedules")};
    }

    auto handle = g_schedule_coordination.handle();
    handle->bundle = charging_schedules;
    handle->bundle_ready = true;
    handle->wait_for_schedule = false;
    EVLOG_info << "ISO 15118-20 HLC schedule bundle stored for transaction " << charging_schedules.transaction_id
               << " (" << charging_schedules.schedules.size() << " schedule(s))";

    // Integration with libiso15118's d20::Session (translating the bundle into ScheduleTuple
    // entries on the ScheduleExchangeRes wire) is deferred to a follow-up — it requires
    // plumbing this module-local coordination state into the library.
    return {types::iso15118::SetChargingSchedulesStatus::Accepted, std::nullopt};
}

void iso15118_extensionsImpl::handle_trigger_schedule_renegotiation(int& evse_id) {
    // Warning (not info): the EV will NOT see EvseNotification::ScheduleRenegotiation on the
    // next ChargeLoopRes — libiso15118's d20::Session has no mutator yet, so the latch below
    // has no consumer (see handle_request in
    // lib/everest/iso15118/src/iso15118/d20/state/dc_charge_loop.cpp, which only writes
    // res.status on stop/pause). OCPP201 has no way to distinguish this from a working path,
    // so surface the drop here. Tracked in
    // docs/superpowers/plans/2026-04-17-k16-renegotiation-and-hlc-handoff-followups.md.
    EVLOG_warning << "K16 renegotiation requested for evse " << evse_id
                  << " on ISO 15118-20 session, but the EV will NOT be notified: "
                  << "libiso15118 d20 does not yet emit EvseNotification::ScheduleRenegotiation";
    auto handle = g_schedule_coordination.handle();
    handle->renegotiation_pending = true;
}

} // namespace extensions
} // namespace module
