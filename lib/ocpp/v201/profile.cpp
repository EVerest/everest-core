// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include "ocpp/v201/profile.hpp"
#include "everest/logging.hpp"
#include <ocpp/common/constants.hpp>
#include <ocpp/v201/ocpp_types.hpp>

using std::chrono::duration_cast;
using std::chrono::seconds;

namespace {

/// \brief update the iterator when the current period has elapsed
/// \param[in] schedule_duration the time in seconds from the start of the composite schedule
/// \param[inout] itt the iterator for the periods in the schedule
/// \param[in] end the item beyond the last period in the schedule
/// \param[out] period the details of the current period in the schedule
/// \param[out] period_duration how long this period lasts
///
/// \note period_duration is defined by the startPeriod of the next period or forever when
///       there is no next period.
void update_itt(const int schedule_duration, std::vector<ocpp::v201::ChargingSchedulePeriod>::const_iterator& itt,
                const std::vector<ocpp::v201::ChargingSchedulePeriod>::const_iterator& end,
                ocpp::v201::ChargingSchedulePeriod& period, int& period_duration) {
    if (itt != end) {
        // default is to remain in the current period
        period = *itt;

        /*
         * calculate the duration of this period:
         * - the startPeriod of the next period in the vector, or
         * - forever where there is no next period
         */
        auto next = std::next(itt);
        period_duration = (next != end) ? next->startPeriod : std::numeric_limits<int>::max();

        if (schedule_duration >= period_duration) {
            /*
             * when the current duration is beyond the duration of this period
             * move to the next period in the vector and recalculate the period duration
             * (the handling for being at the last element is below)
             */
            itt++;
            if (itt != end) {
                period = *itt;
                next = std::next(itt);
                period_duration = (next != end) ? next->startPeriod : std::numeric_limits<int>::max();
            }
        }
    }

    /*
     * all periods in the schedule have been used
     * i.e. there are no future periods to consider in this schedule
     */
    if (itt == end) {
        period.startPeriod = -1;
        period_duration = std::numeric_limits<int>::max();
    }
}

std::pair<float, std::int32_t> convert_limit(const ocpp::v201::period_entry_t* const entry,
                                             const ocpp::v201::ChargingRateUnitEnum selected_unit,
                                             int32_t default_number_phases, int32_t supply_voltage) {
    assert(entry != nullptr);
    float limit = entry->limit;
    std::int32_t number_phases = entry->number_phases.value_or(default_number_phases);

    // if the units are the same - don't change the values
    if (selected_unit != entry->charging_rate_unit) {
        if (selected_unit == ocpp::v201::ChargingRateUnitEnum::A) {
            limit = entry->limit / (supply_voltage * number_phases);
        } else {
            limit = entry->limit * (supply_voltage * number_phases);
        }
    }

    return {limit, number_phases};
}
} // namespace

namespace ocpp {
namespace v201 {

inline std::int32_t elapsed_seconds(const ocpp::DateTime& to, const ocpp::DateTime& from) {
    return duration_cast<seconds>(to.to_time_point() - from.to_time_point()).count();
}

inline ocpp::DateTime floor_seconds(const ocpp::DateTime& dt) {
    return ocpp::DateTime(std::chrono::floor<seconds>(dt.to_time_point()));
}

/// \brief populate a schedule period
/// \param in_start the start time of the profile
/// \param in_duration the time in seconds from the start of the profile to the end of this period
/// \param in_period the details of this period
void period_entry_t::init(const DateTime& in_start, int in_duration, const ChargingSchedulePeriod& in_period,
                          const ChargingProfile& in_profile) {
    // note duration can be negative and hence end time is before start time
    // see period_entry_t::validate()
    const auto start_tp = std::chrono::floor<seconds>(in_start.to_time_point());
    start = std::move(DateTime(start_tp + seconds(in_period.startPeriod)));
    end = std::move(DateTime(start_tp + seconds(in_duration)));
    limit = in_period.limit;
    number_phases = in_period.numberPhases;
    stack_level = in_profile.stackLevel;
    charging_rate_unit = in_profile.chargingSchedule.front().chargingRateUnit;
    min_charging_rate = in_profile.chargingSchedule.front().minChargingRate;
}

bool period_entry_t::validate(const ChargingProfile& profile, const ocpp::DateTime& now) {
    bool b_valid{true};

    if (profile.validFrom) {
        const auto valid_from = floor_seconds(profile.validFrom.value());
        if (valid_from > start) {
            // the calculated start is before the profile is valid
            if (valid_from >= end) {
                // the whole period isn't valid
                b_valid = false;
            } else {
                // adjust start to match validFrom
                start = valid_from;
            }
        }
    }

    b_valid = b_valid && end > start; // check end time is after the start time
    b_valid = b_valid && end > now;   // ignore expired periods
    return b_valid;
}

/// \brief calculate the start times for the profile
/// \param in_now the current date and time
/// \param in_end the end of the composite schedule
/// \param in_session_start optional when the charging session started
/// \param in_profile the charging profile
/// \return a list of the start times of the profile
std::vector<DateTime> calculate_start(const DateTime& in_now, const DateTime& in_end,
                                      const std::optional<DateTime>& in_session_start,
                                      const ChargingProfile& in_profile) {
    /*
     * Absolute schedules start at the defined startSchedule
     * Relative schedules start at session start
     * Recurring schedules start based on startSchedule and the current date/time
     * start can be affected by the profile validFrom. See period_entry_t::validate()
     */
    std::vector<DateTime> start_times;
    DateTime start = floor_seconds(in_now); // fallback when a better value can't be found

    switch (in_profile.chargingProfileKind) {
    case ChargingProfileKindEnum::Absolute:
        // TODO how to deal with multible ChargingSchedules? Currently only handling one.
        if (in_profile.chargingSchedule.front().startSchedule) {
            start = in_profile.chargingSchedule.front().startSchedule.value();
        } else {
            // Absolute should have a startSchedule
            EVLOG_warning << "Absolute charging profile (" << in_profile.id << ") without startSchedule";

            // use validFrom where available
            if (in_profile.validFrom) {
                start = in_profile.validFrom.value();
            }
        }
        start_times.push_back(floor_seconds(start));
        break;
    case ChargingProfileKindEnum::Recurring:
        // TODO how to deal with multible ChargingSchedules?
        if (in_profile.recurrencyKind && in_profile.chargingSchedule.front().startSchedule) {
            const auto start_schedule = floor_seconds(in_profile.chargingSchedule.front().startSchedule.value());
            const auto end = floor_seconds(in_end);
            const auto now_tp = start.to_time_point();
            int seconds_to_go_back{0};
            int seconds_to_go_forward{0};

            /*
             example problem case:
             - allow daily charging 08:00 to 18:00
               at 07:00 and 19:00 what should the start time be?
             a) profile could have 1 period (32A) at 0s with a duration of 36000s (10 hours)
                relying on a lower stack level to deny charging
             b) profile could have 2 periods (32A) at 0s and (0A) at 36000s (10 hours)
                i.e. the profile covers the full 24 hours
             at 07:00 is the start time in 1 hour, or 23 hours ago?
             23 hours ago is the chosen result - however the profile code needs to consider that
             a new daily profile is about to start hence the next start time is provided.
             Weekly has a similar problem
            */

            switch (in_profile.recurrencyKind.value()) {
            case RecurrencyKindEnum::Daily:
                seconds_to_go_back = duration_cast<seconds>(now_tp - start_schedule.to_time_point()).count() %
                                     (HOURS_PER_DAY * SECONDS_PER_HOUR);
                if (seconds_to_go_back < 0) {
                    seconds_to_go_back += HOURS_PER_DAY * SECONDS_PER_HOUR;
                }
                seconds_to_go_forward = HOURS_PER_DAY * SECONDS_PER_HOUR;
                break;
            case RecurrencyKindEnum::Weekly:
                seconds_to_go_back = duration_cast<seconds>(now_tp - start_schedule.to_time_point()).count() %
                                     (SECONDS_PER_DAY * DAYS_PER_WEEK);
                if (seconds_to_go_back < 0) {
                    seconds_to_go_back += SECONDS_PER_DAY * DAYS_PER_WEEK;
                }
                seconds_to_go_forward = SECONDS_PER_DAY * DAYS_PER_WEEK;
                break;
            default:
                EVLOG_error << "Invalid RecurrencyKindType: " << static_cast<int>(in_profile.recurrencyKind.value());
                break;
            }

            start = std::move(DateTime(now_tp - seconds(seconds_to_go_back)));

            while (start <= end) {
                start_times.push_back(start);
                start = DateTime(start.to_time_point() + seconds(seconds_to_go_forward));
            }
        }
        break;
    case ChargingProfileKindEnum::Relative:
        // if there isn't a session start then assume the session starts now
        if (in_session_start) {
            start = floor_seconds(in_session_start.value());
        }
        start_times.push_back(start);
        break;
    default:
        EVLOG_error << "Invalid ChargingProfileKindEnum: " << static_cast<int>(in_profile.chargingProfileKind);
        break;
    }
    return start_times;
}

std::vector<period_entry_t> calculate_profile_entry(const DateTime& in_now, const DateTime& in_end,
                                                    const std::optional<DateTime>& in_session_start,
                                                    const ChargingProfile& in_profile, std::uint8_t in_period_index) {
    std::vector<period_entry_t> entries;

    if (in_period_index >= in_profile.chargingSchedule.front().chargingSchedulePeriod.size()) {
        EVLOG_error << "Invalid schedule period index [" << static_cast<int>(in_period_index)
                    << "] (too large) for profile " << in_profile.id;
    } else {
        const auto& this_period = in_profile.chargingSchedule.front().chargingSchedulePeriod[in_period_index];

        if ((in_period_index == 0) && (this_period.startPeriod != 0)) {
            // invalid profile - first period must be 0
            EVLOG_error << "Invalid schedule period index [0] startPeriod " << this_period.startPeriod
                        << " for profile " << in_profile.id;
        } else if ((in_period_index > 0) &&
                   (in_profile.chargingSchedule.front().chargingSchedulePeriod[in_period_index - 1].startPeriod >=
                    this_period.startPeriod)) {
            // invalid profile - periods must be in order and with increasing startPeriod values
            EVLOG_error << "Invalid schedule period index [" << static_cast<int>(in_period_index) << "] startPeriod "
                        << this_period.startPeriod << " for profile " << in_profile.id;
        } else {
            const bool has_next_period =
                (in_period_index + 1) < in_profile.chargingSchedule.front().chargingSchedulePeriod.size();

            // start time(s) of the schedule
            // the start time of this period is calculated in period_entry_t::init()
            const auto schedule_start = calculate_start(in_now, in_end, in_session_start, in_profile);

            for (std::uint8_t i = 0; i < schedule_start.size(); i++) {
                const bool has_next_occurrance = (i + 1) < schedule_start.size();
                const auto& entry_start = schedule_start[i];

                /*
                 * The duration of this period (from the start of the schedule) is the sooner of
                 * - forever
                 * - next period start time
                 * - optional duration
                 * - the start of the next recurrence
                 * - optional validTo
                 */

                int duration = std::numeric_limits<int>::max(); // forever

                if (has_next_period) {
                    duration =
                        in_profile.chargingSchedule.front().chargingSchedulePeriod[in_period_index + 1].startPeriod;
                }

                // check optional chargingSchedule duration field
                if (in_profile.chargingSchedule.front().duration &&
                    (in_profile.chargingSchedule.front().duration.value() < duration)) {
                    duration = in_profile.chargingSchedule.front().duration.value();
                }

                // check duration doesn't extend into the next recurrence
                if (has_next_occurrance) {
                    const auto next_start =
                        duration_cast<seconds>(schedule_start[i + 1].to_time_point() - entry_start.to_time_point())
                            .count();
                    if (next_start < duration) {
                        duration = next_start;
                    }
                }

                // check duration doesn't extend beyond profile validity
                if (in_profile.validTo) {
                    // note can be negative
                    const auto valid_to = floor_seconds(in_profile.validTo.value());
                    const auto valid_to_seconds =
                        duration_cast<seconds>(valid_to.to_time_point() - entry_start.to_time_point()).count();
                    if (valid_to_seconds < duration) {
                        duration = valid_to_seconds;
                    }
                }

                period_entry_t entry;
                entry.init(entry_start, duration, this_period, in_profile);
                const auto now = floor_seconds(in_now);

                if (entry.validate(in_profile, now)) {
                    entries.push_back(std::move(entry));
                }
            }
        }
    }

    return entries;
}

std::vector<period_entry_t> calculate_profile(const DateTime& now, const DateTime& end,
                                              const std::optional<DateTime>& session_start,
                                              const ChargingProfile& profile) {
    std::vector<period_entry_t> entries;

    for (std::uint8_t i = 0; i < profile.chargingSchedule.front().chargingSchedulePeriod.size(); i++) {
        const auto results = calculate_profile_entry(now, end, session_start, profile, i);
        for (const auto& entry : results) {
            if (entry.start <= end) {
                entries.push_back(entry);
            }
        }
    }

    // sort into date order
    struct {
        bool operator()(const period_entry_t& a, const period_entry_t& b) const {
            // earliest first
            return a.start < b.start;
        }
    } less_than;
    std::sort(entries.begin(), entries.end(), less_than);
    return entries;
}

CompositeSchedule calculate_composite_schedule(std::vector<period_entry_t>& in_combined_schedules,
                                               const DateTime& in_now, const DateTime& in_end,
                                               std::optional<ChargingRateUnitEnum> charging_rate_unit,
                                               int32_t default_number_phases, int32_t supply_voltage) {

    // Defaults to ChargingRateUnitEnum::A if not set.
    const ChargingRateUnitEnum selected_unit =
        (charging_rate_unit) ? charging_rate_unit.value() : ChargingRateUnitEnum::A;

    const auto now = floor_seconds(in_now);
    const auto end = floor_seconds(in_end);

    CompositeSchedule composite{
        .chargingSchedulePeriod = {},
        .evseId = EVSEID_NOT_SET,
        .duration = elapsed_seconds(end, now),
        .scheduleStart = now,
        .chargingRateUnit = selected_unit,
    };

    // sort the combined_schedules in stack priority order
    struct {
        bool operator()(const period_entry_t& a, const period_entry_t& b) const {
            // highest stack level first
            return a.stack_level > b.stack_level;
        }
    } less_than;
    std::sort(in_combined_schedules.begin(), in_combined_schedules.end(), less_than);

    DateTime current = now;

    while (current < end) {
        // find schedule to use for time: current
        DateTime earliest = end;
        DateTime next_earliest = end;
        const period_entry_t* chosen{nullptr};

        for (const auto& schedule : in_combined_schedules) {
            if (schedule.start <= earliest) {
                // ensure the earlier schedule is valid at the current time
                if (schedule.end > current) {
                    next_earliest = earliest;
                    earliest = schedule.start;
                    chosen = &schedule;
                    if (earliest <= current) {
                        break;
                    }
                }
            }
        }

        if (earliest > current) {
            // there is a gap to fill
            composite.chargingSchedulePeriod.push_back(
                {elapsed_seconds(current, now), NO_LIMIT_SPECIFIED, std::nullopt, std::nullopt, std::nullopt});
            current = earliest;
        } else {
            // there is a schedule to use
            const auto [limit, number_phases] =
                convert_limit(chosen, selected_unit, default_number_phases, supply_voltage);

            ChargingSchedulePeriod charging_schedule_period{elapsed_seconds(current, now), limit, std::nullopt,
                                                            number_phases};

            // If the new ChargingSchedulePeriod.phaseToUse field is set, pass it on
            // Profile validation has already ensured that the values have been properly set.
            if (chosen->phase_to_use.has_value()) {
                charging_schedule_period.phaseToUse = chosen->phase_to_use.value();
            }

            composite.chargingSchedulePeriod.push_back(charging_schedule_period);
            if (chosen->end < next_earliest) {
                current = chosen->end;
            } else {
                current = next_earliest;
            }
        }
    }

    return composite;
}

/// \brief update the period based on the power limit
/// \param[in] current the current startPeriod based on duration
/// \param[inout] prevailing_period the details of the current period in the schedule.
/// \param[in] candidate_period the period that is being compared to period.
/// \param[in] current_charging_rate_unit_enum details of the current composite schedule charging_rate_unit for
/// conversion.
///
ChargingSchedulePeriod minimize_charging_schedule_period_by_limit(
    const int current, const ChargingSchedulePeriod& prevailing_period, const ChargingSchedulePeriod& candidate_period,
    const ChargingRateUnitEnum current_charging_rate_unit_enum, const int32_t default_number_phases) {

    auto adjusted_period = prevailing_period;

    if (candidate_period.startPeriod == NO_START_PERIOD) {
        return adjusted_period;
    }

    if (prevailing_period.limit == NO_LIMIT_SPECIFIED && candidate_period.limit != NO_LIMIT_SPECIFIED) {
        adjusted_period = candidate_period;
    } else if (candidate_period.limit != NO_LIMIT_SPECIFIED) {
        const auto charge_point_max_phases = candidate_period.numberPhases.value_or(default_number_phases);

        const auto period_max_phases = prevailing_period.numberPhases.value_or(default_number_phases);
        adjusted_period.numberPhases = std::min(charge_point_max_phases, period_max_phases);

        if (current_charging_rate_unit_enum == ChargingRateUnitEnum::A) {
            if (candidate_period.limit < prevailing_period.limit) {
                adjusted_period.limit = candidate_period.limit;
            }
        } else {
            const auto charge_point_limit_per_phase = candidate_period.limit / charge_point_max_phases;
            const auto period_limit_per_phase = prevailing_period.limit / period_max_phases;

            adjusted_period.limit = std::floor(std::min(charge_point_limit_per_phase, period_limit_per_phase) *
                                               adjusted_period.numberPhases.value());
        }
    }

    return adjusted_period;
}

CompositeSchedule calculate_composite_schedule(const CompositeSchedule& charging_station_external_constraints,
                                               const CompositeSchedule& charging_station_max,
                                               const CompositeSchedule& tx_default, const CompositeSchedule& tx,
                                               const CompositeScheduleDefaultLimits& default_limits,
                                               int32_t supply_voltage) {

    CompositeSchedule combined{
        .chargingSchedulePeriod = {},
        .evseId = EVSEID_NOT_SET,
        .duration = tx_default.duration,
        .scheduleStart = tx_default.scheduleStart,
        .chargingRateUnit = tx_default.chargingRateUnit,

    };

    const float default_limit = (tx_default.chargingRateUnit == ChargingRateUnitEnum::A)
                                    ? static_cast<float>(default_limits.amps)
                                    : static_cast<float>(default_limits.watts);

    int current_period = 0;

    const int end = std::max(charging_station_external_constraints.duration,
                             std::max(std::max(charging_station_max.duration, tx_default.duration), tx.duration));

    auto charging_station_external_constraints_itt =
        charging_station_external_constraints.chargingSchedulePeriod.begin();
    auto charging_station_max_itt = charging_station_max.chargingSchedulePeriod.begin();
    auto tx_default_itt = tx_default.chargingSchedulePeriod.begin();
    auto tx_itt = tx.chargingSchedulePeriod.begin();

    int duration_charging_station_external_constraints{std::numeric_limits<int>::max()};
    int duration_charging_station_max{std::numeric_limits<int>::max()};
    int duration_tx_default{std::numeric_limits<int>::max()};
    int duration_tx{std::numeric_limits<int>::max()};

    ChargingSchedulePeriod period_charging_station_external_constraints{NO_START_PERIOD, NO_LIMIT_SPECIFIED,
                                                                        std::nullopt, std::nullopt};
    ChargingSchedulePeriod period_charging_station_max{NO_START_PERIOD, NO_LIMIT_SPECIFIED, std::nullopt, std::nullopt};
    ChargingSchedulePeriod period_tx_default{NO_START_PERIOD, NO_LIMIT_SPECIFIED, std::nullopt, std::nullopt};
    ChargingSchedulePeriod period_tx{NO_START_PERIOD, NO_LIMIT_SPECIFIED, std::nullopt, std::nullopt};

    update_itt(0, charging_station_external_constraints_itt,
               charging_station_external_constraints.chargingSchedulePeriod.end(),
               period_charging_station_external_constraints, duration_charging_station_external_constraints);

    update_itt(0, charging_station_max_itt, charging_station_max.chargingSchedulePeriod.end(),
               period_charging_station_max, duration_charging_station_max);

    update_itt(0, tx_default_itt, tx_default.chargingSchedulePeriod.end(), period_tx_default, duration_tx_default);

    update_itt(0, tx_itt, tx.chargingSchedulePeriod.end(), period_tx, duration_tx);

    ChargingSchedulePeriod last{1, NO_LIMIT_SPECIFIED, std::nullopt, default_limits.number_phases};

    while (current_period < end) {

        // get the duration that covers the smallest amount of time.
        int duration = std::min(duration_charging_station_external_constraints,
                                std::min(std::min(duration_charging_station_max, duration_tx_default), duration_tx));

        // create an unset period to override as needed.
        ChargingSchedulePeriod period{NO_START_PERIOD, NO_LIMIT_SPECIFIED, std::nullopt, default_limits.number_phases};

        if (period_tx.startPeriod != NO_START_PERIOD) {
            period = period_tx;
        }

        if (period_tx_default.startPeriod != NO_START_PERIOD) {
            if ((period.limit == NO_LIMIT_SPECIFIED) && (period_tx_default.limit != NO_LIMIT_SPECIFIED)) {
                period = period_tx_default;
            }
        }

        period = minimize_charging_schedule_period_by_limit(current_period, period, period_charging_station_max,
                                                            combined.chargingRateUnit, default_limits.number_phases);
        period = minimize_charging_schedule_period_by_limit(current_period, period,
                                                            period_charging_station_external_constraints,
                                                            combined.chargingRateUnit, default_limits.number_phases);

        update_itt(duration, charging_station_external_constraints_itt,
                   charging_station_external_constraints.chargingSchedulePeriod.end(),
                   period_charging_station_external_constraints, duration_charging_station_external_constraints);
        update_itt(duration, charging_station_max_itt, charging_station_max.chargingSchedulePeriod.end(),
                   period_charging_station_max, duration_charging_station_max);
        update_itt(duration, tx_default_itt, tx_default.chargingSchedulePeriod.end(), period_tx_default,
                   duration_tx_default);
        update_itt(duration, tx_itt, tx.chargingSchedulePeriod.end(), period_tx, duration_tx);

        if (period.startPeriod != NO_START_PERIOD) {
            if (!period.numberPhases.has_value()) {
                period.numberPhases = default_limits.number_phases;
            }

            if (period.limit == NO_LIMIT_SPECIFIED) {
                period.limit = default_limit;
            }
            period.startPeriod = current_period;

            // check this new period is a change from the previous one
            if ((period.limit != last.limit) || (period.numberPhases.value() != last.numberPhases.value())) {
                combined.chargingSchedulePeriod.push_back(period);
            }
            current_period = duration;
            last = period;
        } else {
            combined.chargingSchedulePeriod.push_back(
                {current_period, default_limit, std::nullopt, default_limits.number_phases});
            current_period = end;
        }
    }

    return combined;
}

} // namespace v201
} // namespace ocpp