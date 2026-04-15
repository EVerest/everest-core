// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "iso15118_extensionsImpl.hpp"
#include "log.hpp"
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

void iso15118_extensionsImpl::handle_set_notify_ev_schedule_status(types::iso15118::NotifyEvScheduleStatus& status) {
    pthread_mutex_lock(&v2g_ctx->mqtt_lock);
    switch (status) {
    case types::iso15118::NotifyEvScheduleStatus::Accepted:
        // K15.FR.03: hold ChargeParameterDiscoveryRes at Ongoing until the CSMS schedule arrives.
        v2g_ctx->hlc_schedule_wait.store(true);
        v2g_ctx->evse_v2g_data.evse_processing[PHASE_PARAMETER] = iso2_EVSEProcessingType_Ongoing;
        break;
    case types::iso15118::NotifyEvScheduleStatus::Rejected:
    case types::iso15118::NotifyEvScheduleStatus::Processing:
    case types::iso15118::NotifyEvScheduleStatus::NoChargingProfile:
        // K15.FR.04/05: fall through to the default schedule. libocpp triggers K16 renegotiation
        // later if/when the CSMS sends a profile.
        v2g_ctx->hlc_schedule_wait.store(false);
        v2g_ctx->evse_v2g_data.evse_processing[PHASE_PARAMETER] = iso2_EVSEProcessingType_Finished;
        break;
    }
    pthread_cond_signal(&v2g_ctx->mqtt_cond);
    pthread_mutex_unlock(&v2g_ctx->mqtt_lock);
}

types::iso15118::SetChargingSchedulesResult
iso15118_extensionsImpl::handle_set_ev_charging_schedules(types::iso15118::OcppEvChargingSchedules& charging_schedules) {
    if (charging_schedules.schedules.empty()) {
        return {types::iso15118::SetChargingSchedulesStatus::Rejected, std::string("empty schedules")};
    }

    pthread_mutex_lock(&v2g_ctx->mqtt_lock);

    // Populate one SAScheduleTuple per OCPP composite schedule (up to 3 per K15.FR.22 / V2G2-773).
    const auto tuple_count = std::min<std::size_t>(charging_schedules.schedules.size(),
                                                    iso2_SAScheduleTupleType_3_ARRAY_SIZE);
    auto& schedule_list = v2g_ctx->evse_v2g_data.evse_sa_schedule_list;
    for (std::size_t k = 0; k < tuple_count; ++k) {
        const auto& source = charging_schedules.schedules[k];
        auto& tuple = schedule_list.SAScheduleTuple.array[k];

        // SAScheduleTupleID must be 1..255 per [V2G2-773]; bias the OCPP schedule id into range.
        const int raw_id = source.id == 0 ? static_cast<int>(k + 1) : source.id;
        tuple.SAScheduleTupleID = static_cast<uint8_t>(((raw_id - 1) % 255) + 1);

        // PMaxSchedule: translate each OCPP ChargingSchedulePeriod into a PMaxScheduleEntry.
        const std::size_t period_count = std::min<std::size_t>(source.charging_schedule_period.size(),
                                                                iso2_PMaxScheduleEntryType_12_ARRAY_SIZE);
        tuple.PMaxSchedule.PMaxScheduleEntry.arrayLen = static_cast<uint16_t>(period_count);
        const double assumed_voltage = v2g_ctx->basic_config.evse_ac_nominal_voltage > 0
                                           ? v2g_ctx->basic_config.evse_ac_nominal_voltage
                                           : 230.0;
        for (std::size_t p = 0; p < period_count; ++p) {
            const auto& period = source.charging_schedule_period[p];
            auto& entry = tuple.PMaxSchedule.PMaxScheduleEntry.array[p];

            set_pmax_watts(entry.PMax, period_limit_to_watts(period, source.charging_rate_unit, assumed_voltage));
            entry.RelativeTimeInterval.start = static_cast<uint32_t>(period.start_period);
            const auto next_start = (p + 1 < period_count)
                                        ? static_cast<uint32_t>(source.charging_schedule_period[p + 1].start_period)
                                        : static_cast<uint32_t>(source.duration.value_or(0));
            if (next_start > static_cast<uint32_t>(period.start_period)) {
                entry.RelativeTimeInterval.duration = next_start - static_cast<uint32_t>(period.start_period);
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
    v2g_ctx->evse_v2g_data.evse_processing[PHASE_PARAMETER] = iso2_EVSEProcessingType_Finished;

    pthread_cond_signal(&v2g_ctx->mqtt_cond);
    pthread_mutex_unlock(&v2g_ctx->mqtt_lock);

    return {types::iso15118::SetChargingSchedulesStatus::Accepted, std::nullopt};
}

} // namespace extensions
} // namespace module
