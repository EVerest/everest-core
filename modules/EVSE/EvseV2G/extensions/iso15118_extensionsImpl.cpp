// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "iso15118_extensionsImpl.hpp"
#include "iso_server.hpp" // cpd_handoff_should_drop_bundle
#include "log.hpp"
#include "tools.hpp" // getmonotonictime
#include "v2g_ctx.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace module {
namespace extensions {

namespace {
// Convert a watt value into an iso2 PhysicalValue (value + exponent, base unit W).
void set_pmax_watts(iso2_PhysicalValueType& dst, double watts) {
    int8_t multiplier = 0;
    double scaled = watts;
    while (std::abs(scaled) > std::numeric_limits<int16_t>::max() and multiplier < 3) {
        scaled /= 10.0;
        ++multiplier;
    }
    dst.Multiplier = multiplier;
    dst.Unit = iso2_unitSymbolType_W;
    dst.Value = static_cast<int16_t>(scaled);
}

// Best-effort conversion of an OCPP ChargingSchedulePeriod limit (in W or A) to a wattage number
// when the schedule unit is "W". For "A" we fall back to assuming EVSE nominal voltage so the
// limit is translated into a sensible PMax; refinement is a follow-up.
double period_limit_to_watts(const types::ocpp::ChargingSchedulePeriod& period, const std::string& rate_unit,
                             double assumed_voltage) {
    if (rate_unit == "W") {
        return period.limit;
    }
    return period.limit * assumed_voltage;
}
} // namespace

namespace testing_internal {
// K16 renegotiation latch: set evse_notification to ReNegotiation when currently None,
// so the next DC/AC response builder emits EVSENotification=ReNegotiation and the EV
// renegotiates (K16.FR.02/11). StopCharging has precedence and is never clobbered.
// A re-entry while ReNegotiation is already pending is a no-op. Holds mqtt_lock to
// serialize with the ISO state machine and signals mqtt_cond so any waiter observes
// the change immediately.
void trigger_renegotiation_if_idle(struct v2g_context* ctx) {
    if (ctx == nullptr) {
        return;
    }
    pthread_mutex_lock(&ctx->mqtt_lock);
    switch (ctx->evse_v2g_data.evse_notification) {
    case iso2_EVSENotificationType_None:
        ctx->evse_v2g_data.evse_notification = iso2_EVSENotificationType_ReNegotiation;
        break;
    case iso2_EVSENotificationType_StopCharging:
        EVLOG_debug << "K16 renegotiation trigger ignored: StopCharging latched";
        break;
    case iso2_EVSENotificationType_ReNegotiation:
        // Latch already pending; no-op.
        break;
    default:
        break;
    }
    pthread_cond_signal(&ctx->mqtt_cond);
    pthread_mutex_unlock(&ctx->mqtt_lock);
}
} // namespace testing_internal

void iso15118_extensionsImpl::init() {
    if (!v2g_ctx) {
        dlog(DLOG_LEVEL_ERROR, "v2g_ctx not created");
        return;
    }
    // Give the ISO state machine a way to publish the EV's selected schedule back to OCPP.
    v2g_ctx->publish_ev_selected_schedule_cb = [this](int32_t sa_schedule_tuple_id,
                                                      const std::optional<int32_t>& selected_schedule_id) {
        types::iso15118::SelectedEvChargingSchedule selected;
        selected.sa_schedule_tuple_id = sa_schedule_tuple_id;
        selected.selected_charging_schedule_id = selected_schedule_id;
        // ev_charging_schedule (15118-2 ChargingProfile → OCPP ChargingSchedule) is not yet
        // translated here; libocpp falls back to an empty schedule when absent.
        publish_ev_selected_schedule(selected);
    };
}

void iso15118_extensionsImpl::ready() {
}

void iso15118_extensionsImpl::handle_set_get_certificate_response(
    types::iso15118::ResponseExiStreamStatus& certificate_response) {
    pthread_mutex_lock(&v2g_ctx->mqtt_lock);
    if (certificate_response.exi_response.has_value() and not certificate_response.exi_response.value().empty()) {
        v2g_ctx->evse_v2g_data.cert_install_res_b64_buffer = std::string(certificate_response.exi_response.value());
    }
    v2g_ctx->evse_v2g_data.cert_install_status =
        (certificate_response.status == types::iso15118::Status::Accepted) ? true : false;
    pthread_cond_signal(&v2g_ctx->mqtt_cond);
    /* unlock */
    pthread_mutex_unlock(&v2g_ctx->mqtt_lock);
}

void iso15118_extensionsImpl::handle_set_hlc_schedule_wait(bool& wait) {
    pthread_mutex_lock(&v2g_ctx->mqtt_lock);
    if (wait) {
        // K15.FR.03: hold ChargeParameterDiscoveryRes at Ongoing until the CSMS schedule arrives.
        v2g_ctx->hlc_schedule_wait.store(true);
        v2g_ctx->hlc_schedule_deadline_ms = getmonotonictime() + v2g_ctx->basic_config.cpd_timeout_ms;
        v2g_ctx->evse_v2g_data.evse_processing[PHASE_PARAMETER] = iso2_EVSEProcessingType_Ongoing;
    } else {
        // K15.FR.04/05: fall through to the default schedule. libocpp triggers K16 renegotiation
        // later if/when the CSMS sends a profile.
        v2g_ctx->hlc_schedule_wait.store(false);
        v2g_ctx->hlc_schedule_deadline_ms = 0;
        v2g_ctx->evse_v2g_data.evse_processing[PHASE_PARAMETER] = iso2_EVSEProcessingType_Finished;
    }
    pthread_cond_signal(&v2g_ctx->mqtt_cond);
    pthread_mutex_unlock(&v2g_ctx->mqtt_lock);
}

types::iso15118::SetChargingSchedulesResult iso15118_extensionsImpl::handle_set_ev_charging_schedules(
    types::iso15118::OcppEvChargingSchedules& charging_schedules) {
    if (charging_schedules.schedules.empty()) {
        return {types::iso15118::SetChargingSchedulesStatus::Rejected, std::string("empty schedules")};
    }

    if (cpd_handoff_should_drop_bundle(v2g_ctx)) {
        dlog(DLOG_LEVEL_WARNING, "schedule bundle arrived after CPD exit; dropping");
        return {types::iso15118::SetChargingSchedulesStatus::Rejected, std::string("CPD phase already exited")};
    }

    pthread_mutex_lock(&v2g_ctx->mqtt_lock);

    // Populate one SAScheduleTuple per OCPP composite schedule (up to 3 per K15.FR.22 / V2G2-773).
    const auto tuple_count =
        std::min<std::size_t>(charging_schedules.schedules.size(), iso2_SAScheduleTupleType_3_ARRAY_SIZE);
    auto& schedule_list = v2g_ctx->evse_v2g_data.evse_sa_schedule_list;
    for (std::size_t k = 0; k < tuple_count; ++k) {
        const auto& source = charging_schedules.schedules[k];
        auto& tuple = schedule_list.SAScheduleTuple.array[k];

        // SAScheduleTupleID must be 1..255 per [V2G2-773]; bias the OCPP schedule id into range.
        const int raw_id = source.id == 0 ? static_cast<int>(k + 1) : source.id;
        tuple.SAScheduleTupleID = static_cast<uint8_t>(((raw_id - 1) % 255) + 1);

        // PMaxSchedule: translate each OCPP ChargingSchedulePeriod into a PMaxScheduleEntry.
        const std::size_t period_count =
            std::min<std::size_t>(source.charging_schedule_period.size(), iso2_PMaxScheduleEntryType_12_ARRAY_SIZE);
        tuple.PMaxSchedule.PMaxScheduleEntry.arrayLen = static_cast<uint16_t>(period_count);
        const double assumed_voltage =
            v2g_ctx->basic_config.evse_ac_nominal_voltage > 0 ? v2g_ctx->basic_config.evse_ac_nominal_voltage : 230.0;
        // OCPP ChargingSchedulePeriod::start_period is signed (per spec a non-negative offset
        // in seconds). Clamp rather than wrap on unsigned cast so a malformed negative value
        // cannot produce a multi-gigasecond interval on the wire.
        auto clamped_start = [](std::int32_t raw) -> std::uint32_t {
            if (raw < 0) {
                EVLOG_warning << "ChargingSchedulePeriod start_period=" << raw << " is negative; clamping to 0";
                return 0U;
            }
            return static_cast<std::uint32_t>(raw);
        };
        for (std::size_t p = 0; p < period_count; ++p) {
            const auto& period = source.charging_schedule_period[p];
            auto& entry = tuple.PMaxSchedule.PMaxScheduleEntry.array[p];

            set_pmax_watts(entry.PMax, period_limit_to_watts(period, source.charging_rate_unit, assumed_voltage));
            const auto start = clamped_start(period.start_period);
            entry.RelativeTimeInterval.start = start;
            const auto next_start = (p + 1 < period_count)
                                        ? clamped_start(source.charging_schedule_period[p + 1].start_period)
                                        : static_cast<std::uint32_t>(std::max(0, source.duration.value_or(0)));
            if (next_start > start) {
                entry.RelativeTimeInterval.duration = next_start - start;
                entry.RelativeTimeInterval.duration_isUsed = 1;
            } else {
                entry.RelativeTimeInterval.duration_isUsed = 0;
            }
            entry.RelativeTimeInterval_isUsed = 1;
            entry.TimeInterval_isUsed = 0;
        }

        // SalesTariff passthrough is deferred — the iso2 SalesTariff wire format requires
        // mapping every SalesTariffEntry (RelativeTimeInterval + ePriceLevel + optional
        // ConsumptionCost) and signature blocks, which is out of scope for the first cut.
        tuple.SalesTariff_isUsed = 0;
    }
    schedule_list.SAScheduleTuple.arrayLen = static_cast<uint16_t>(tuple_count);
    v2g_ctx->evse_v2g_data.evse_sa_schedule_list_is_used = true;

    // Release the ISO state machine now that a bundle is ready (K15.FR.07).
    v2g_ctx->hlc_schedule_wait.store(false);
    v2g_ctx->hlc_schedule_deadline_ms = 0;
    v2g_ctx->evse_v2g_data.evse_processing[PHASE_PARAMETER] = iso2_EVSEProcessingType_Finished;

    pthread_cond_signal(&v2g_ctx->mqtt_cond);
    pthread_mutex_unlock(&v2g_ctx->mqtt_lock);

    return {types::iso15118::SetChargingSchedulesStatus::Accepted, std::nullopt};
}

void iso15118_extensionsImpl::handle_trigger_schedule_renegotiation(int& evse_id) {
    EVLOG_info << "trigger_schedule_renegotiation received for evse " << evse_id;
    testing_internal::trigger_renegotiation_if_idle(v2g_ctx);
}

} // namespace extensions
} // namespace module
