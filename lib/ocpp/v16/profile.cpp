// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <chrono>
#include <limits>
#include <optional>

#include <ocpp/common/constants.hpp>
#include <ocpp/common/types.hpp>
#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>
#include <ocpp/v16/profile.hpp>
#include <ocpp/v16/smart_charging.hpp>

#include <everest/logging.hpp>

using std::chrono::duration_cast;
using std::chrono::seconds;

namespace {

using ocpp::v16::EnhancedChargingSchedulePeriod;

inline std::int32_t elapsed_seconds(const ocpp::DateTime& to, const ocpp::DateTime& from) {
    return duration_cast<seconds>(to.to_time_point() - from.to_time_point()).count();
}

inline ocpp::DateTime floor_seconds(const ocpp::DateTime& dt) {
    return ocpp::DateTime(std::chrono::floor<seconds>(dt.to_time_point()));
}

/// \brief update the iterator when the current period has elapsed
/// \param[in] schedule_duration the time in seconds from the start of the composite schedule
/// \param[inout] itt the iterator for the periods in the schedule
/// \param[in] end the item beyond the last period in the schedule
/// \param[out] period the details of the current period in the schedule
/// \param[out] period_duration how long this period lasts
///
/// \note period_duration is defined by the startPeriod of the next period or forever when
///       there is no next period.
void update_itt(const int schedule_duration, std::vector<EnhancedChargingSchedulePeriod>::const_iterator& itt,
                const std::vector<EnhancedChargingSchedulePeriod>::const_iterator& end,
                EnhancedChargingSchedulePeriod& period, int& period_duration) {
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

std::pair<float, std::int32_t> convert_limit(const ocpp::v16::period_entry_t* const entry,
                                             const ocpp::v16::ChargingRateUnit selected_unit,
                                             int32_t default_number_phases, int32_t supply_voltage) {
    assert(entry != nullptr);
    float limit = entry->limit;
    std::int32_t number_phases = entry->number_phases.value_or(default_number_phases);

    // if the units are the same - don't change the values
    if (selected_unit != entry->charging_rate_unit) {
        if (selected_unit == ocpp::v16::ChargingRateUnit::A) {
            limit = entry->limit / (supply_voltage * number_phases);
        } else {
            limit = entry->limit * (supply_voltage * number_phases);
        }
    }

    return {limit, number_phases};
}

} // namespace

namespace ocpp::v16 {

constexpr bool operator==(const ChargingSchedulePeriod& lhs, const ChargingSchedulePeriod& rhs) {
    return (lhs.startPeriod == rhs.startPeriod) && (lhs.limit == rhs.limit);
}

constexpr bool operator!=(const ChargingSchedulePeriod& lhs, const ChargingSchedulePeriod& rhs) {
    return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& os, const period_entry_t& entry) {
    os << entry.start << " " << entry.end << " S:" << entry.stack_level << " " << entry.limit
       << ((entry.charging_rate_unit == ChargingRateUnit::A) ? "A" : "W");
    return os;
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
    charging_rate_unit = in_profile.chargingSchedule.chargingRateUnit;
    min_charging_rate = in_profile.chargingSchedule.minChargingRate;
}

/// \brief validate and update entry based on profile
/// \param profile the profile this entry is part of
/// \param now the current date and time
/// \return true when the entry is valid
bool period_entry_t::validate(const ChargingProfile& profile, const DateTime& now) {
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
    case ChargingProfileKindType::Absolute:
        if (in_profile.chargingSchedule.startSchedule) {
            start = in_profile.chargingSchedule.startSchedule.value();
        } else {
            // Absolute should have a startSchedule
            EVLOG_warning << "Absolute charging profile (" << in_profile.chargingProfileId << ") without startSchedule";

            // use validFrom where available
            if (in_profile.validFrom) {
                start = in_profile.validFrom.value();
            }
        }
        start_times.push_back(floor_seconds(start));
        break;
    case ChargingProfileKindType::Recurring:
        if (in_profile.recurrencyKind && in_profile.chargingSchedule.startSchedule) {
            const auto start_schedule = floor_seconds(in_profile.chargingSchedule.startSchedule.value());
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
            case RecurrencyKindType::Daily:
                seconds_to_go_back = duration_cast<seconds>(now_tp - start_schedule.to_time_point()).count() %
                                     (HOURS_PER_DAY * SECONDS_PER_HOUR);
                if (seconds_to_go_back < 0) {
                    seconds_to_go_back += HOURS_PER_DAY * SECONDS_PER_HOUR;
                }
                seconds_to_go_forward = HOURS_PER_DAY * SECONDS_PER_HOUR;
                break;
            case RecurrencyKindType::Weekly:
                seconds_to_go_back = duration_cast<seconds>(now_tp - start_schedule.to_time_point()).count() %
                                     (SECONDS_PER_DAY * DAYS_PER_WEEK);
                if (seconds_to_go_back < 0) {
                    seconds_to_go_back += SECONDS_PER_DAY * DAYS_PER_WEEK;
                }
                seconds_to_go_forward = SECONDS_PER_DAY * DAYS_PER_WEEK;
                break;
            }

            start = std::move(DateTime(now_tp - seconds(seconds_to_go_back)));

            while (start <= end) {
                start_times.push_back(start);
                start = DateTime(start.to_time_point() + seconds(seconds_to_go_forward));
            }
        }
        break;
    case ChargingProfileKindType::Relative:
        // if there isn't a session start then assume the session starts now
        if (in_session_start) {
            start = floor_seconds(in_session_start.value());
        }
        start_times.push_back(start);
        break;
    default:
        EVLOG_error << "Invalid ChargingProfileKindType: " << static_cast<int>(in_profile.chargingProfileKind);
        break;
    }

    return start_times;
}

/// \brief calculate the start times for the schedule period
/// \param in_now the current date and time
/// \param in_end the end of the composite schedule
/// \param in_session_start optional when the charging session started
/// \param in_profile the charging profile
/// \param in_period_index the schedule period index
/// \return the list of start times
std::vector<period_entry_t> calculate_profile_entry(const DateTime& in_now, const DateTime& in_end,
                                                    const std::optional<DateTime>& in_session_start,
                                                    const ChargingProfile& in_profile, std::uint8_t in_period_index) {
    std::vector<period_entry_t> entries;

    if (in_period_index >= in_profile.chargingSchedule.chargingSchedulePeriod.size()) {
        EVLOG_error << "Invalid schedule period index [" << static_cast<int>(in_period_index)
                    << "] (too large) for profile " << in_profile.chargingProfileId;
    } else {
        const auto& this_period = in_profile.chargingSchedule.chargingSchedulePeriod[in_period_index];

        if ((in_period_index == 0) && (this_period.startPeriod != 0)) {
            // invalid profile - first period must be 0
            EVLOG_error << "Invalid schedule period index [0] startPeriod " << this_period.startPeriod
                        << " for profile " << in_profile.chargingProfileId;
        } else if ((in_period_index > 0) &&
                   (in_profile.chargingSchedule.chargingSchedulePeriod[in_period_index - 1].startPeriod >=
                    this_period.startPeriod)) {
            // invalid profile - periods must be in order and with increasing startPeriod values
            EVLOG_error << "Invalid schedule period index [" << static_cast<int>(in_period_index) << "] startPeriod "
                        << this_period.startPeriod << " for profile " << in_profile.chargingProfileId;
        } else {
            const bool has_next_period =
                (in_period_index + 1) < in_profile.chargingSchedule.chargingSchedulePeriod.size();

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
                    duration = in_profile.chargingSchedule.chargingSchedulePeriod[in_period_index + 1].startPeriod;
                }

                // check optional chargingSchedule duration field
                if (in_profile.chargingSchedule.duration && (in_profile.chargingSchedule.duration.value() < duration)) {
                    duration = in_profile.chargingSchedule.duration.value();
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

/// \brief generate an ordered list of valid schedule periods for the profile
/// \param now the current date and time
/// \param end ignore entries beyond this date and time (i.e. that start after end)
/// \param session_start optional when the charging session started
/// \param profile the charging profile
/// \return a list of profile periods with calculated date & time start and end times
/// \note it is valid for there to be gaps (for recurring profiles)
std::vector<period_entry_t> calculate_profile(const DateTime& now, const DateTime& end,
                                              const std::optional<DateTime>& session_start,
                                              const ChargingProfile& profile) {
    std::vector<period_entry_t> entries;

    for (std::uint8_t i = 0; i < profile.chargingSchedule.chargingSchedulePeriod.size(); i++) {
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

/// \brief calculate the composite schedule for the list of periods
/// \param in_combined_schedules the list of periods to build into the schedule
/// \param in_now the start of the composite schedule
/// \param in_end the end (i.e. duration) of the composite schedule
/// \param in_charging_rate_unit the units to use (defaults to Amps)
/// \return the calculated composite schedule
EnhancedChargingSchedule calculate_composite_schedule(std::vector<period_entry_t>& in_combined_schedules,
                                                      const DateTime& in_now, const DateTime& in_end,
                                                      std::optional<ChargingRateUnit> in_charging_rate_unit,
                                                      int32_t default_number_phases, int32_t supply_voltage) {
    const ChargingRateUnit selected_unit =
        (in_charging_rate_unit) ? in_charging_rate_unit.value() : ChargingRateUnit::A;
    const auto now = floor_seconds(in_now);
    const auto end = floor_seconds(in_end);
    EnhancedChargingSchedule composite{selected_unit, {}, elapsed_seconds(end, now), now, std::nullopt};

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
                {elapsed_seconds(current, now), no_limit_specified, std::nullopt, 0});
            current = earliest;
        } else {
            // there is a schedule to use
            const auto [limit, number_phases] =
                convert_limit(chosen, selected_unit, default_number_phases, supply_voltage);
            composite.chargingSchedulePeriod.push_back({elapsed_seconds(current, now), limit, number_phases,
                                                        chosen->stack_level,
                                                        selected_unit != chosen->charging_rate_unit});
            if (chosen->end < next_earliest) {
                current = chosen->end;
            } else {
                current = next_earliest;
            }
        }
    }

    return composite;
}

/// \brief calculate the composite combined schedule
/// \param charge_point_max the composite schedule for ChargePointMax profiles
/// \param tx_default the composite schedule for TxDefault profiles
/// \param tx the composite schedule for Tx profiles
/// \return the calculated combined composite schedule
/// \note all composite schedules must have the same units configured
EnhancedChargingSchedule calculate_composite_schedule(const EnhancedChargingSchedule& charge_point_max,
                                                      const EnhancedChargingSchedule& tx_default,
                                                      const EnhancedChargingSchedule& tx,
                                                      const CompositeScheduleDefaultLimits& default_limits,
                                                      int32_t supply_voltage) {
    EnhancedChargingSchedule combined{
        tx_default.chargingRateUnit, {}, tx_default.duration, tx_default.startSchedule, tx_default.minChargingRate};
    if (tx.minChargingRate) {
        combined.minChargingRate = tx.minChargingRate.value();
    }

    const auto default_limit = (tx_default.chargingRateUnit == ChargingRateUnit::A)
                                   ? static_cast<float>(default_limits.amps)
                                   : static_cast<float>(default_limits.watts);

    int current{0};
    const int end = std::max(std::max(charge_point_max.duration.value_or(0), tx_default.duration.value_or(0)),
                             tx.duration.value_or(0));

    auto charge_point_max_itt = charge_point_max.chargingSchedulePeriod.begin();
    auto tx_default_itt = tx_default.chargingSchedulePeriod.begin();
    auto tx_itt = tx.chargingSchedulePeriod.begin();

    int duration_charge_point_max{std::numeric_limits<int>::max()};
    int duration_tx_default{std::numeric_limits<int>::max()};
    int duration_tx{std::numeric_limits<int>::max()};

    EnhancedChargingSchedulePeriod period_charge_point_max{-1, -1.0, std::nullopt};
    EnhancedChargingSchedulePeriod period_tx_default{-1, -1.0, std::nullopt};
    EnhancedChargingSchedulePeriod period_tx{-1, -1.0, std::nullopt};

    update_itt(0, charge_point_max_itt, charge_point_max.chargingSchedulePeriod.end(), period_charge_point_max,
               duration_charge_point_max);
    update_itt(0, tx_default_itt, tx_default.chargingSchedulePeriod.end(), period_tx_default, duration_tx_default);
    update_itt(0, tx_itt, tx.chargingSchedulePeriod.end(), period_tx, duration_tx);

    EnhancedChargingSchedulePeriod last{-1, no_limit_specified, default_limits.number_phases, 0};

    while (current < end) {
        int duration = std::min(std::min(duration_charge_point_max, duration_tx_default), duration_tx);
        EnhancedChargingSchedulePeriod period{-1, no_limit_specified, default_limits.number_phases, 0};

        // use TxProfile when there is one
        if (period_tx.startPeriod != -1) {
            period = period_tx;
        }

        if (period_tx_default.startPeriod != -1) {
            period.startPeriod = current;
            // use TxDefaultProfile when a TxProfile doesn't exist
            if ((period.limit == no_limit_specified) && (period_tx_default.limit != no_limit_specified)) {
                period = period_tx_default;
            }
        }

        if (period_charge_point_max.startPeriod != -1) {
            period.startPeriod = current;

            if (period.limit == no_limit_specified) {
                // use ChargePointMaxProfile when TxProfile and TxDefaultProfile don't exist
                if (period_charge_point_max.limit != no_limit_specified) {
                    period = period_charge_point_max;
                }
            } else {
                // apply ChargePointMaxProfile limits
                // Note the actual ChargePointMaxProfile limit is controlled outside of libocpp

                if (period_charge_point_max.limit != no_limit_specified) {

                    const auto charge_point_max_phases =
                        period_charge_point_max.numberPhases.value_or(default_limits.number_phases);
                    const auto period_max_phases = period.numberPhases.value_or(default_limits.number_phases);
                    period.numberPhases = std::min(charge_point_max_phases, period_max_phases);

                    if (combined.chargingRateUnit == ChargingRateUnit::A) {
                        // limit is per phase
                        if (period_charge_point_max.limit < period.limit) {
                            // apply lower limit
                            period.limit = period_charge_point_max.limit;
                        }
                    } else {
                        // limit is total allowed power, changes in number of phases matter

                        // adjust limits taking into account a potential change in number of phases
                        const auto charge_point_limit_per_phase =
                            period_charge_point_max.limit / charge_point_max_phases;
                        const auto period_limit_per_phase = period.limit / period_max_phases;

                        // avoid fractional results - 1 decimal place is allowed but tricky to ensure with a float
                        period.limit = std::floor(std::min(charge_point_limit_per_phase, period_limit_per_phase) *
                                                  period.numberPhases.value());
                    }
                }
            }
        }

        update_itt(duration, charge_point_max_itt, charge_point_max.chargingSchedulePeriod.end(),
                   period_charge_point_max, duration_charge_point_max);
        update_itt(duration, tx_default_itt, tx_default.chargingSchedulePeriod.end(), period_tx_default,
                   duration_tx_default);
        update_itt(duration, tx_itt, tx.chargingSchedulePeriod.end(), period_tx, duration_tx);

        if (period.startPeriod != -1) {
            if (!period.numberPhases.has_value()) {
                period.numberPhases = default_limits.number_phases;
            }

            if (period.limit == no_limit_specified) {
                period.limit = default_limit;
            }
            period.startPeriod = current;

            // check this new period is a change from the previous one
            if ((period.limit != last.limit) || (period.numberPhases.value() != last.numberPhases.value())) {
                combined.chargingSchedulePeriod.push_back(period);
            }
            current = duration;
            last = period;
        } else {
            combined.chargingSchedulePeriod.push_back({current, default_limit, default_limits.number_phases, 0});
            current = end;
        }
    }

    return combined;
}

} // namespace ocpp::v16
