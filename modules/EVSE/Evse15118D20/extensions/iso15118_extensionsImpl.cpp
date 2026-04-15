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

void iso15118_extensionsImpl::handle_set_notify_ev_schedule_status(types::iso15118::NotifyEvScheduleStatus& status) {
    auto handle = g_schedule_coordination.handle();
    switch (status) {
    case types::iso15118::NotifyEvScheduleStatus::Accepted:
        // K15.FR.03: hold ScheduleExchangeRes at Ongoing until the CSMS schedule arrives.
        handle->wait_for_schedule = true;
        handle->bundle_ready = false;
        handle->fallback_requested = false;
        break;
    case types::iso15118::NotifyEvScheduleStatus::Rejected:
    case types::iso15118::NotifyEvScheduleStatus::Processing:
    case types::iso15118::NotifyEvScheduleStatus::NoChargingProfile:
        handle->wait_for_schedule = false;
        handle->fallback_requested = true;
        break;
    }
    EVLOG_info << "ISO 15118-20 HLC schedule status = " << types::iso15118::notify_ev_schedule_status_to_string(status);
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

} // namespace extensions
} // namespace module
