// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include "ocpp/v2/profile.hpp"
#include "everest/logging.hpp"
#include <ocpp/common/constants.hpp>
#include <ocpp/v2/ocpp_types.hpp>

using std::chrono::duration_cast;
using std::chrono::seconds;

namespace ocpp {
namespace v2 {

// Forward declarations of helper functions
void set_setpoint_limit_phase_values(PeriodLimit& current_limit, PeriodLimit& power_limit,
                                     const float& no_limit_specified, const std::optional<int32_t> number_phases,
                                     const OcppProtocolVersion ocpp_version);
float get_max_limit(const float limit1, const float limit2, const std::optional<float> cap_max = std::nullopt);
PeriodLimit get_max_limit(const PeriodLimit& limit1, const PeriodLimit& limit2,
                          const std::optional<PeriodLimit>& cap = std::nullopt);
float get_min_limit(const float limit1, const float limit2, const std::optional<float> cap_min = std::nullopt);
PeriodLimit get_min_limit(const PeriodLimit& limit1, const PeriodLimit& limit2,
                          const std::optional<PeriodLimit>& cap = std::nullopt);
void get_set_setpoint_limit(PeriodLimit& setpoint1, const PeriodLimit& setpoint2, const PeriodLimit& cap_limit,
                            const PeriodLimit& cap_discharge_limit);
bool uses_three_phase_values(const IntermediatePeriod& period);
void store_limit_to_phase_limits(const PeriodLimit& input_limit, const float& not_specified,
                                 std::optional<float>& value, std::optional<float>& value_L2,
                                 std::optional<float>& value_L3);
void convert_and_transform_limit_value(const float& input, const float& not_specified, const float& transform_value,
                                       std::optional<float>& value, const bool use_min, const bool use_divide);
void convert_and_transform_limit_to_period_schedule(const PeriodLimit& input_limit, const float& not_specified,
                                                    const float& transform_value, const int32_t number_phases,
                                                    std::optional<float>& value, std::optional<float>& value_L2,
                                                    std::optional<float>& value_L3, const bool use_min,
                                                    const bool use_divide);

int32_t elapsed_seconds(const ocpp::DateTime& to, const ocpp::DateTime& from) {
    return duration_cast<seconds>(to.to_time_point() - from.to_time_point()).count();
}

ocpp::DateTime floor_seconds(const ocpp::DateTime& dt) {
    return ocpp::DateTime(std::chrono::floor<seconds>(dt.to_time_point()));
}

IntermediatePeriod default_intermediate_period() {
    IntermediatePeriod empty;
    empty.startPeriod = 0;
    empty.current_limit = {NO_LIMIT_SPECIFIED, NO_LIMIT_SPECIFIED, NO_LIMIT_SPECIFIED};
    empty.power_limit = {NO_LIMIT_SPECIFIED, NO_LIMIT_SPECIFIED, NO_LIMIT_SPECIFIED};
    empty.current_discharge_limit = {NO_DISCHARGE_LIMIT_SPECIFIED, NO_DISCHARGE_LIMIT_SPECIFIED,
                                     NO_DISCHARGE_LIMIT_SPECIFIED};
    empty.power_discharge_limit = {NO_DISCHARGE_LIMIT_SPECIFIED, NO_DISCHARGE_LIMIT_SPECIFIED,
                                   NO_DISCHARGE_LIMIT_SPECIFIED};
    empty.current_setpoint = {NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED};
    empty.power_setpoint = {NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED};
    empty.numberPhases = std::nullopt;
    empty.phaseToUse = std::nullopt;
    return empty;
}

IntermediatePeriod default_intermediate_period(const int32_t start_period) {
    IntermediatePeriod empty = default_intermediate_period();
    empty.startPeriod = start_period;
    return empty;
}

bool ocpp::v2::PeriodLimit::operator==(const PeriodLimit& other) const {
    return is_equal(limit, other.limit) && is_equal(limit_L2, other.limit_L2) && is_equal(limit_L3, other.limit_L3);
}

bool PeriodLimit::operator!=(const PeriodLimit& other) const {
    return !(*this == other);
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
    number_phases = in_period.numberPhases;
    stack_level = in_profile.stackLevel;
    charging_rate_unit = in_profile.chargingSchedule.front().chargingRateUnit;
    limit.limit = in_period.limit.value_or(NO_LIMIT_SPECIFIED);
    limit.limit_L2 = in_period.limit_L2.value_or(NO_LIMIT_SPECIFIED);
    limit.limit_L3 = in_period.limit_L3.value_or(NO_LIMIT_SPECIFIED);
    if (in_period.limit == std::nullopt &&
        (!in_period.operationMode.has_value() || in_period.operationMode.value() == OperationModeEnum::ChargingOnly)) {
        if (charging_rate_unit == ChargingRateUnitEnum::A) {
            limit.limit = DEFAULT_LIMIT_AMPS;
        } else {
            limit.limit = DEFAULT_LIMIT_WATTS;
        }
    }

    discharge_limit.limit = in_period.dischargeLimit.value_or(NO_DISCHARGE_LIMIT_SPECIFIED);
    discharge_limit.limit_L2 = in_period.dischargeLimit_L2.value_or(NO_DISCHARGE_LIMIT_SPECIFIED);
    discharge_limit.limit_L3 = in_period.dischargeLimit_L3.value_or(NO_DISCHARGE_LIMIT_SPECIFIED);

    setpoint.limit = in_period.setpoint.value_or(NO_SETPOINT_SPECIFIED);       // FIXME
    setpoint.limit_L2 = in_period.setpoint_L2.value_or(NO_SETPOINT_SPECIFIED); // FIXME
    setpoint.limit_L3 = in_period.setpoint_L3.value_or(NO_SETPOINT_SPECIFIED); // FIXME

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
    case ChargingProfileKindEnum::Dynamic:
        // FIXME: check if other requirements for dynamic exist
        start_times.push_back(floor_seconds(start));
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

namespace {
std::vector<period_entry_t> calculate_profile_unsorted(const DateTime& now, const DateTime& end,
                                                       const std::optional<DateTime>& session_start,
                                                       const ChargingProfile& profile) {
    std::vector<period_entry_t> entries;

    const auto nr_of_entries = profile.chargingSchedule.front().chargingSchedulePeriod.size();
    for (uint8_t i = 0; i < nr_of_entries; i++) {
        const auto results = calculate_profile_entry(now, end, session_start, profile, i);
        for (const auto& entry : results) {
            if (entry.start <= end) {
                entries.push_back(entry);
            }
        }
    }

    return entries;
}

void sort_periods_into_date_order(std::vector<period_entry_t>& periods) {
    // sort into date order
    struct {
        bool operator()(const period_entry_t& a, const period_entry_t& b) const {
            // earliest first
            return a.start < b.start;
        }
    } less_than;
    std::sort(periods.begin(), periods.end(), less_than);
}
} // namespace

std::vector<period_entry_t> calculate_profile(const DateTime& now, const DateTime& end,
                                              const std::optional<DateTime>& session_start,
                                              const ChargingProfile& profile) {
    std::vector<period_entry_t> entries = calculate_profile_unsorted(now, end, session_start, profile);

    sort_periods_into_date_order(entries);
    return entries;
}

std::vector<period_entry_t> calculate_all_profiles(const DateTime& now, const DateTime& end,
                                                   const std::optional<DateTime>& session_start,
                                                   const std::vector<ChargingProfile>& profiles,
                                                   ChargingProfilePurposeEnum purpose) {
    std::vector<period_entry_t> output;
    for (const auto& profile : profiles) {
        if (profile.chargingProfilePurpose == purpose) {
            std::vector<period_entry_t> periods = calculate_profile_unsorted(now, end, session_start, profile);
            output.insert(output.end(), periods.begin(), periods.end());
        }
    }

    sort_periods_into_date_order(output);
    return output;
}

IntermediateProfile generate_profile_from_periods(std::vector<period_entry_t>& periods, const DateTime& in_now,
                                                  const DateTime& in_end) {

    const auto now = floor_seconds(in_now);
    const auto end = floor_seconds(in_end);

    if (periods.empty()) {
        return {default_intermediate_period()};
    }

    // sort the combined_schedules in stack priority order
    struct {
        bool operator()(const period_entry_t& a, const period_entry_t& b) const {
            // highest stack level first
            return a.stack_level > b.stack_level;
        }
    } less_than;
    std::sort(periods.begin(), periods.end(), less_than);

    IntermediateProfile combined{};
    DateTime current = now;

    while (current < end) {
        // find schedule to use for time: current
        DateTime earliest = end;
        DateTime next_earliest = end;
        const period_entry_t* chosen{nullptr};

        for (const auto& schedule : periods) {
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
            combined.push_back({default_intermediate_period(elapsed_seconds(current, now))});
            current = earliest;
        } else {
            // there is a schedule to use
            PeriodLimit current_limit{NO_LIMIT_SPECIFIED, NO_LIMIT_SPECIFIED, NO_LIMIT_SPECIFIED};
            PeriodLimit power_limit = {NO_LIMIT_SPECIFIED, NO_LIMIT_SPECIFIED, NO_LIMIT_SPECIFIED};
            PeriodLimit current_discharge_limit = {NO_DISCHARGE_LIMIT_SPECIFIED, NO_DISCHARGE_LIMIT_SPECIFIED,
                                                   NO_DISCHARGE_LIMIT_SPECIFIED};
            PeriodLimit power_discharge_limit = {NO_DISCHARGE_LIMIT_SPECIFIED, NO_DISCHARGE_LIMIT_SPECIFIED,
                                                 NO_DISCHARGE_LIMIT_SPECIFIED};
            PeriodLimit current_setpoint = {NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED};
            PeriodLimit power_setpoint = {NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED};

            if (chosen->charging_rate_unit == ChargingRateUnitEnum::A) {
                current_limit = chosen->limit;
                current_setpoint = chosen->setpoint;
                current_discharge_limit = chosen->discharge_limit;
            } else {
                power_limit = chosen->limit;
                power_setpoint = chosen->setpoint;
                power_discharge_limit = chosen->discharge_limit;
            }

            IntermediatePeriod charging_schedule_period;
            charging_schedule_period.startPeriod = elapsed_seconds(current, now);
            charging_schedule_period.current_limit = current_limit;
            charging_schedule_period.power_limit = power_limit;
            charging_schedule_period.current_setpoint = current_setpoint;
            charging_schedule_period.power_setpoint = power_setpoint;
            charging_schedule_period.current_discharge_limit = current_discharge_limit;
            charging_schedule_period.power_discharge_limit = power_discharge_limit;
            charging_schedule_period.numberPhases = chosen->number_phases;
            charging_schedule_period.phaseToUse = std::nullopt;

            // If the new ChargingSchedulePeriod.phaseToUse field is set, pass it on
            // Profile validation has already ensured that the values have been properly set.
            if (chosen->phase_to_use.has_value()) {
                charging_schedule_period.phaseToUse = chosen->phase_to_use.value();
            }

            combined.push_back(charging_schedule_period);
            if (chosen->end < next_earliest) {
                current = chosen->end;
            } else {
                current = next_earliest;
            }
        }
    }

    return combined;
}

namespace {

using period_iterator = IntermediateProfile::const_iterator;
using period_pair_vector = std::vector<std::pair<period_iterator, period_iterator>>;
using IntermediateProfileRef = std::reference_wrapper<const IntermediateProfile>;

inline std::vector<IntermediateProfileRef> convert_to_ref_vector(const std::vector<IntermediateProfile>& profiles) {
    std::vector<IntermediateProfileRef> references{};
    for (auto& profile : profiles) {
        references.push_back(profile);
    }
    return references;
}

IntermediateProfile combine_list_of_profiles(const std::vector<IntermediateProfileRef>& profiles,
                                             std::function<IntermediatePeriod(const period_pair_vector&)> combinator) {
    if (profiles.empty()) {
        // We should never get here as there are always profiles, otherwise there is a mistake in the calling
        // function Return an empty profile to be safe
        return {default_intermediate_period()};
    }

    IntermediateProfile combined{};

    period_pair_vector profile_iterators{};
    for (const auto& wrapped_profile : profiles) {
        auto& profile = wrapped_profile.get();
        if (!profile.empty()) {
            profile_iterators.push_back(std::make_pair(profile.begin(), profile.end()));
        }
    }

    int32_t current_period = 0;
    while (std::any_of(profile_iterators.begin(), profile_iterators.end(),
                       [](const std::pair<period_iterator, period_iterator>& it) { return it.first != it.second; })) {

        IntermediatePeriod period = combinator(profile_iterators);
        period.startPeriod = current_period;

        if (combined.empty() || (period.current_limit != combined.back().current_limit) ||
            (period.power_limit != combined.back().power_limit) ||
            (period.current_discharge_limit != combined.back().current_discharge_limit) ||
            (period.power_discharge_limit != combined.back().power_discharge_limit) ||
            (period.current_setpoint != combined.back().current_setpoint) ||
            (period.power_setpoint != combined.back().power_setpoint) ||
            (period.numberPhases != combined.back().numberPhases)) {
            combined.push_back(period);
        }

        // Determine the next earliest period
        int32_t next_lowest_period = std::numeric_limits<int32_t>::max();

        for (const auto& [it, end] : profile_iterators) {
            auto next = it + 1;
            if (next != end && next->startPeriod > current_period && next->startPeriod < next_lowest_period) {
                next_lowest_period = next->startPeriod;
            }
        }

        // If there is none, we are done
        if (next_lowest_period == std::numeric_limits<int32_t>::max()) {
            break;
        }

        // Otherwise update to next earliest period
        for (auto& [it, end] : profile_iterators) {
            auto next = it + 1;
            if (next != end && next->startPeriod == next_lowest_period) {
                it++;
            }
        }
        current_period = next_lowest_period;
    }

    if (combined.empty()) {
        combined.push_back(default_intermediate_period());
    }

    return combined;
}

} // namespace

IntermediateProfile merge_tx_profile_with_tx_default_profile(const IntermediateProfile& tx_profile,
                                                             const IntermediateProfile& tx_default_profile) {
    auto combinator = [](const period_pair_vector& periods) {
        const IntermediatePeriod default_period = default_intermediate_period();
        IntermediatePeriod period{};
        period.current_limit = {NO_LIMIT_SPECIFIED, NO_LIMIT_SPECIFIED, NO_LIMIT_SPECIFIED};
        period.power_limit = {NO_LIMIT_SPECIFIED, NO_LIMIT_SPECIFIED, NO_LIMIT_SPECIFIED};
        period.current_discharge_limit = {NO_DISCHARGE_LIMIT_SPECIFIED, NO_DISCHARGE_LIMIT_SPECIFIED,
                                          NO_DISCHARGE_LIMIT_SPECIFIED};
        period.power_discharge_limit = {NO_DISCHARGE_LIMIT_SPECIFIED, NO_DISCHARGE_LIMIT_SPECIFIED,
                                        NO_DISCHARGE_LIMIT_SPECIFIED};
        period.current_setpoint = {NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED};
        period.power_setpoint = {NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED};

        for (const auto& [it, end] : periods) {
            if (it->current_limit != default_period.current_limit || it->power_limit != default_period.power_limit ||
                it->current_discharge_limit != default_period.current_discharge_limit ||
                it->power_discharge_limit != default_period.power_discharge_limit ||
                it->current_setpoint != default_period.current_setpoint ||
                it->power_setpoint != default_period.power_setpoint) {
                period.current_limit = it->current_limit;
                period.power_limit = it->power_limit;
                period.current_discharge_limit = it->current_discharge_limit;
                period.power_discharge_limit = it->power_discharge_limit;
                period.current_setpoint = it->current_setpoint;
                period.power_setpoint = it->power_setpoint;
                period.numberPhases = it->numberPhases;
                break;
            }
        }

        return period;
    };

    // This ordering together with the combinator will prefer the tx_profile above the default profile
    std::vector<IntermediateProfileRef> profiles{tx_profile, tx_default_profile};

    return combine_list_of_profiles(profiles, combinator);
}

IntermediateProfile merge_profiles_by_lowest_limit(const std::vector<IntermediateProfile>& profiles,
                                                   const OcppProtocolVersion ocpp_version) {
    auto combinator = [ocpp_version](const period_pair_vector& periods) {
        IntermediatePeriod period;
        period.current_limit = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                                std::numeric_limits<float>::max()};
        period.power_limit = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                              std::numeric_limits<float>::max()};
        period.current_discharge_limit = {NO_DISCHARGE_LIMIT_SPECIFIED, NO_DISCHARGE_LIMIT_SPECIFIED,
                                          NO_DISCHARGE_LIMIT_SPECIFIED};
        period.power_discharge_limit = {NO_DISCHARGE_LIMIT_SPECIFIED, NO_DISCHARGE_LIMIT_SPECIFIED,
                                        NO_DISCHARGE_LIMIT_SPECIFIED};
        period.current_setpoint = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                                   std::numeric_limits<float>::max()};
        period.power_setpoint = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                                 std::numeric_limits<float>::max()};

        bool three_phases_used = false;
        // Get number of phases for this period (the lowest of the number of phases).
        for (const auto& [it, end] : periods) {
            if (!period.numberPhases || (it->numberPhases && it->numberPhases.value() < period.numberPhases.value())) {
                period.numberPhases = it->numberPhases;
            }

            if (ocpp_version == OcppProtocolVersion::v21 && uses_three_phase_values(*it)) {
                three_phases_used = true;
            }
        }

        for (const auto& [it, end] : periods) {
            IntermediatePeriod new_period = *it;
            if (three_phases_used) {
                set_setpoint_limit_phase_values(new_period.current_limit, new_period.power_limit, NO_LIMIT_SPECIFIED,
                                                period.numberPhases, ocpp_version);
                set_setpoint_limit_phase_values(new_period.current_discharge_limit, new_period.power_discharge_limit,
                                                NO_DISCHARGE_LIMIT_SPECIFIED, period.numberPhases, ocpp_version);
                set_setpoint_limit_phase_values(new_period.current_setpoint, new_period.power_setpoint,
                                                NO_SETPOINT_SPECIFIED, period.numberPhases, ocpp_version);
            }

            period.current_limit = get_min_limit(period.current_limit, new_period.current_limit);
            period.power_limit = get_min_limit(period.power_limit, new_period.power_limit);
            period.current_discharge_limit =
                get_max_limit(period.current_discharge_limit, new_period.current_discharge_limit);
            period.power_discharge_limit =
                get_max_limit(period.power_discharge_limit, new_period.power_discharge_limit);

            // Only check value of first phase, as this one should be set as first.
            get_set_setpoint_limit(period.current_setpoint, new_period.current_setpoint, period.current_limit,
                                   period.current_discharge_limit);
            get_set_setpoint_limit(period.power_setpoint, new_period.power_setpoint, period.power_limit,
                                   period.power_discharge_limit);
        }

        auto replace_max_with_no_limit = [](PeriodLimit& value, float max_value, float replacement) {
            if (is_equal(value.limit, max_value)) {
                value.limit = replacement;
            }
            if (is_equal(value.limit_L2, max_value)) {
                value.limit_L2 = replacement;
            }
            if (is_equal(value.limit_L3, max_value)) {
                value.limit_L3 = replacement;
            }
        };

        replace_max_with_no_limit(period.current_limit, std::numeric_limits<float>::max(), NO_LIMIT_SPECIFIED);
        replace_max_with_no_limit(period.power_limit, std::numeric_limits<float>::max(), NO_LIMIT_SPECIFIED);
        replace_max_with_no_limit(period.current_setpoint, std::numeric_limits<float>::max(), NO_SETPOINT_SPECIFIED);
        replace_max_with_no_limit(period.power_setpoint, std::numeric_limits<float>::max(), NO_SETPOINT_SPECIFIED);
        replace_max_with_no_limit(period.current_discharge_limit, NO_DISCHARGE_LIMIT_SPECIFIED,
                                  NO_DISCHARGE_LIMIT_SPECIFIED);
        replace_max_with_no_limit(period.power_discharge_limit, NO_DISCHARGE_LIMIT_SPECIFIED,
                                  NO_DISCHARGE_LIMIT_SPECIFIED);

        return period;
    };

    return combine_list_of_profiles(convert_to_ref_vector(profiles), combinator);
}

IntermediateProfile merge_profiles_by_summing_limits(const std::vector<IntermediateProfile>& profiles,
                                                     float current_default, float power_default,
                                                     const OcppProtocolVersion ocpp_version) {
    auto combinator = [current_default, power_default, ocpp_version](const period_pair_vector& periods) {
        IntermediatePeriod period{};

        // summing limits dont have a setpoint, so set to default values
        period.current_setpoint = {NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED};
        period.power_setpoint = {NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED, NO_SETPOINT_SPECIFIED};

        bool three_phases_used = false;
        // Get number of phases for this period (the lowest of the number of phases).
        for (const auto& [it, end] : periods) {
            // Copy number of phases if higher
            if (!period.numberPhases.has_value() ||
                (it->numberPhases.has_value() && it->numberPhases.value() > period.numberPhases.value())) {
                period.numberPhases = it->numberPhases;
            }

            if (ocpp_version == OcppProtocolVersion::v21 && uses_three_phase_values(*it)) {
                three_phases_used = true;
            }
        }

        for (const auto& [it, end] : periods) {
            IntermediatePeriod new_period = *it;
            if (three_phases_used) {
                set_setpoint_limit_phase_values(new_period.current_limit, new_period.power_limit, NO_LIMIT_SPECIFIED,
                                                period.numberPhases, ocpp_version);
                set_setpoint_limit_phase_values(new_period.current_discharge_limit, new_period.power_discharge_limit,
                                                NO_DISCHARGE_LIMIT_SPECIFIED, period.numberPhases, ocpp_version);
                set_setpoint_limit_phase_values(new_period.current_setpoint, new_period.power_setpoint,
                                                NO_SETPOINT_SPECIFIED, period.numberPhases, ocpp_version);
            }

            period.current_limit.limit +=
                new_period.current_limit.limit >= 0.0F ? new_period.current_limit.limit : current_default;
            if (three_phases_used) {
                period.current_limit.limit_L2 +=
                    new_period.current_limit.limit_L2 >= 0.0F ? new_period.current_limit.limit_L2 : current_default;
                period.current_limit.limit_L3 +=
                    new_period.current_limit.limit_L3 >= 0.0F ? new_period.current_limit.limit_L3 : current_default;
            } else {
                period.current_limit.limit_L2 = NO_LIMIT_SPECIFIED;
                period.current_limit.limit_L3 = NO_LIMIT_SPECIFIED;
            }

            period.power_limit.limit +=
                new_period.power_limit.limit >= 0.0F ? new_period.power_limit.limit : power_default;

            if (three_phases_used) {
                period.power_limit.limit_L2 +=
                    new_period.power_limit.limit_L2 >= 0.0F ? new_period.power_limit.limit_L2 : power_default;
                period.power_limit.limit_L3 +=
                    new_period.power_limit.limit_L3 >= 0.0F ? new_period.power_limit.limit_L3 : power_default;
            } else {
                period.power_limit.limit_L2 = NO_LIMIT_SPECIFIED;
                period.power_limit.limit_L3 = NO_LIMIT_SPECIFIED;
            }
        }
        return period;
    };

    return combine_list_of_profiles(convert_to_ref_vector(profiles), combinator);
}

std::vector<ChargingSchedulePeriod>
convert_intermediate_into_schedule(const IntermediateProfile& profile, ChargingRateUnitEnum charging_rate_unit,
                                   float default_limit, int32_t default_number_phases, float supply_voltage) {

    std::vector<ChargingSchedulePeriod> output{};

    for (const auto& period : profile) {
        ChargingSchedulePeriod period_out{};
        period_out.startPeriod = period.startPeriod;
        period_out.numberPhases = period.numberPhases;

        if (is_equal(period.current_limit.limit, NO_LIMIT_SPECIFIED) &&
            is_equal(period.power_limit.limit, NO_LIMIT_SPECIFIED)) {
            period_out.limit = default_limit;
        } else {
            period_out.limit = std::numeric_limits<float>::max();
        }

        const int32_t number_phases = period_out.numberPhases.value_or(default_number_phases);
        const float transform_value = supply_voltage;
        if (charging_rate_unit == ChargingRateUnitEnum::A) {
            store_limit_to_phase_limits(period.current_limit, NO_LIMIT_SPECIFIED, period_out.limit, period_out.limit_L2,
                                        period_out.limit_L3);

            convert_and_transform_limit_to_period_schedule(period.power_limit, NO_LIMIT_SPECIFIED, transform_value,
                                                           number_phases, period_out.limit, period_out.limit_L2,
                                                           period_out.limit_L3, true, true);

            store_limit_to_phase_limits(period.current_discharge_limit, NO_DISCHARGE_LIMIT_SPECIFIED,
                                        period_out.dischargeLimit, period_out.dischargeLimit_L2,
                                        period_out.dischargeLimit_L3);

            convert_and_transform_limit_to_period_schedule(
                period.power_discharge_limit, NO_DISCHARGE_LIMIT_SPECIFIED, transform_value, number_phases,
                period_out.dischargeLimit, period_out.dischargeLimit_L2, period_out.dischargeLimit_L3, false, true);

            store_limit_to_phase_limits(period.current_setpoint, NO_SETPOINT_SPECIFIED, period_out.setpoint,
                                        period_out.setpoint_L2, period_out.setpoint_L3);

            convert_and_transform_limit_to_period_schedule(period.power_setpoint, NO_SETPOINT_SPECIFIED,
                                                           transform_value, number_phases, period_out.setpoint,
                                                           period_out.setpoint_L2, period_out.setpoint_L3, true, true);
        } else {
            store_limit_to_phase_limits(period.power_limit, NO_LIMIT_SPECIFIED, period_out.limit, period_out.limit_L2,
                                        period_out.limit_L3);

            convert_and_transform_limit_to_period_schedule(period.current_limit, NO_LIMIT_SPECIFIED, transform_value,
                                                           number_phases, period_out.limit, period_out.limit_L2,
                                                           period_out.limit_L3, true, false);

            store_limit_to_phase_limits(period.power_discharge_limit, NO_DISCHARGE_LIMIT_SPECIFIED,
                                        period_out.dischargeLimit, period_out.dischargeLimit_L2,
                                        period_out.dischargeLimit_L3);

            convert_and_transform_limit_to_period_schedule(
                period.current_discharge_limit, NO_DISCHARGE_LIMIT_SPECIFIED, transform_value, number_phases,
                period_out.dischargeLimit, period_out.dischargeLimit_L2, period_out.dischargeLimit_L3, false, false);

            store_limit_to_phase_limits(period.power_setpoint, NO_SETPOINT_SPECIFIED, period_out.setpoint,
                                        period_out.setpoint_L2, period_out.setpoint_L3);

            convert_and_transform_limit_to_period_schedule(period.current_setpoint, NO_SETPOINT_SPECIFIED,
                                                           transform_value, number_phases, period_out.setpoint,
                                                           period_out.setpoint_L2, period_out.setpoint_L3, true, false);
        }

        if (output.empty() || (period_out.limit != output.back().limit) ||
            (period_out.numberPhases != output.back().numberPhases) || period_out.setpoint != output.back().setpoint ||
            period_out.dischargeLimit != output.back().dischargeLimit) {
            output.push_back(period_out);
        }
    }

    return output;
}

// Helper functions

///
/// \brief Set the values of the phases so both `PeriodLimit`s have the same values for number of phases, which is used
///        later when combining the two.
///
/// This is only done for OCPP version 2.1.
///
/// If number of phases is two or three, and `limit` has a value and `limit_L2` or `limit_L3` do not have values:
/// In case of `current_limit`: the `current_limit.limit` value is copied to `current_limit.limit_L2` and
/// `current_limit.limit_L3`.
/// In case of `power_limit`: The `power_limit.limit` value is divided by 3 and that value is
/// copied to `power_limit.limit_L2` and `power_limit.limit_L3`.
///
/// If number of phases is one, the minimum of `limit`, `limit_L2` and `limit_L3` is copied to `limit` and `limit_L2`
/// and `limit_L3` are set to `no_limit_specified`.
///
/// \param current_limit        A PeriodLimit for `current` (possible containing limits for three phases).
/// \param power_limit          A PeriodLimit for `power` (possible containing limits for three phases).
/// \param no_limit_specified   The value when there is no limit specified.
/// \param number_phases        The number of phases used.
/// \param ocpp_version         The ocpp version.
///
void set_setpoint_limit_phase_values(PeriodLimit& current_limit, PeriodLimit& power_limit,
                                     const float& no_limit_specified, const std::optional<int32_t> number_phases,
                                     const OcppProtocolVersion ocpp_version) {
    // This can't be done for all limits and setpoints, because OCPP 2.0.1 does not support setpoints for L2 and L3.
    if (ocpp_version != OcppProtocolVersion::v21) {
        return;
    }

    // Set the values of the phases so both `PeriodLimit`s have the same values for number of phases, which is used
    // later when combining the two.
    if (number_phases.has_value() && number_phases.value() == 1) {
        // When you have a single phase, but next to limit the limit_L2 and limit_L3 are also defined, then the limit
        // will be the lowest of the three.
        if (!is_equal(current_limit.limit, no_limit_specified)) {
            if (!is_equal(current_limit.limit_L2, no_limit_specified)) {
                current_limit.limit = std::min(current_limit.limit, current_limit.limit_L2);
                current_limit.limit_L2 = no_limit_specified;
            }

            if (!is_equal(current_limit.limit_L3, no_limit_specified)) {
                current_limit.limit = std::min(current_limit.limit, current_limit.limit_L3);
                current_limit.limit_L3 = no_limit_specified;
            }
        }

        if (!is_equal(power_limit.limit, no_limit_specified)) {
            if (!is_equal(power_limit.limit_L2, no_limit_specified)) {
                power_limit.limit = std::min(power_limit.limit, power_limit.limit_L2);
                power_limit.limit_L2 = no_limit_specified;
            }

            if (!is_equal(power_limit.limit_L3, no_limit_specified)) {
                power_limit.limit = std::min(power_limit.limit, power_limit.limit_L3);
                power_limit.limit_L3 = no_limit_specified;
            }
        }
        return;
    }

    if (!number_phases.has_value() || number_phases.value() < 1) {
        return;
    }

    // Three phases: One of this group has one phase specified and the other three, so let's specify the same number of
    // phases for each.
    if (!is_equal(current_limit.limit, no_limit_specified) && (is_equal(current_limit.limit_L2, no_limit_specified) ||
                                                               is_equal(current_limit.limit_L3, no_limit_specified))) {
        current_limit.limit_L2 = current_limit.limit;
        current_limit.limit_L3 = current_limit.limit;
    }

    if (!is_equal(power_limit.limit, no_limit_specified) &&
        (is_equal(power_limit.limit_L2, no_limit_specified) || is_equal(power_limit.limit_L3, no_limit_specified))) {
        power_limit.limit_L2 = power_limit.limit / static_cast<float>(number_phases.value());
        power_limit.limit_L3 = power_limit.limit / static_cast<float>(number_phases.value());
        power_limit.limit = power_limit.limit / static_cast<float>(number_phases.value());
    }
}

///
/// \brief Get max limit of two floats. Capped on cap_max.
/// \param limit1
/// \param limit2
/// \param cap_max
/// \return The maximum of two floats and cap_max.
///
float get_max_limit(const float limit1, const float limit2, const std::optional<float> cap_max) {
    float limit = limit1;
    if (limit2 < 0.0F) {
        limit = std::max(limit1, limit2);
    }

    if (cap_max.has_value()) {
        return std::max(limit, cap_max.value());
    }

    return limit;
}

///
/// \brief Get max limit of each phase in PeriodLimit. Capped on 'cap'.
/// \param limit1
/// \param limit2
/// \param cap
/// \return For each limit in the PeriodLimit struct: the max of the two and cap.
///
PeriodLimit get_max_limit(const PeriodLimit& limit1, const PeriodLimit& limit2, const std::optional<PeriodLimit>& cap) {
    PeriodLimit result;
    result.limit = get_max_limit(limit1.limit, limit2.limit,
                                 (cap == std::nullopt ? std::nullopt : std::optional<float>(cap.value().limit)));
    result.limit_L2 = get_max_limit(limit1.limit_L2, limit2.limit_L2,
                                    (cap == std::nullopt ? std::nullopt : std::optional<float>(cap.value().limit_L2)));
    result.limit_L3 = get_max_limit(limit1.limit_L3, limit2.limit_L3,
                                    (cap == std::nullopt ? std::nullopt : std::optional<float>(cap.value().limit_L3)));
    return result;
}

///
/// \brief Get minimum limit of two floats. Capped on cap_min.
/// \param limit1
/// \param limit2
/// \param cap_min
/// \return The minimum of two floats and cap_min.
///
float get_min_limit(const float limit1, const float limit2, const std::optional<float> cap_min) {
    float limit = limit1;
    if (limit2 >= 0.0F) {
        limit = std::min(limit1, limit2);
    }

    if (cap_min.has_value()) {
        return std::min(limit, cap_min.value());
    }

    return limit;
}

///
/// \brief Get min limit of each phase in PeriodLimit. Capped on 'cap'.
/// \param limit1
/// \param limit2
/// \param cap
/// \return For each limit in the PeriodLimit struct: the min of the two and cap.
///
PeriodLimit get_min_limit(const PeriodLimit& limit1, const PeriodLimit& limit2, const std::optional<PeriodLimit>& cap) {
    PeriodLimit result;
    result.limit = get_min_limit(limit1.limit, limit2.limit,
                                 (cap == std::nullopt ? std::nullopt : std::optional<float>(cap.value().limit)));
    result.limit_L2 = get_min_limit(limit1.limit_L2, limit2.limit_L2,
                                    (cap == std::nullopt ? std::nullopt : std::optional<float>(cap.value().limit_L2)));
    result.limit_L3 = get_min_limit(limit1.limit_L3, limit2.limit_L3,
                                    (cap == std::nullopt ? std::nullopt : std::optional<float>(cap.value().limit_L3)));
    return result;
}

///
/// \brief Get setpoint limit and set to the output variable.
///
/// This will set the limit of a setpoint to the lowest or highest of the two. With a minimum of cap_limit or maximum
/// of cap_discharge_limit.
///
/// If min or max is used depends on the value of setpoint1.limit. If it is > 0, the minimum is used (charging),
/// otherwise the maximum (discharging).
///
/// \param[in/out] setpoint1        First setpoint to set the limits for. Limits will be stored in this setpoint.
/// \param[in] setpoint2            Second setpoint with limits.
/// \param[in] cap_limit            Setpoints are capped to this limit in case they are positive.
/// \param[in] cap_discharge_limit  Setpoints are capped to this limit in case they are negative.
///
void get_set_setpoint_limit(PeriodLimit& setpoint1, const PeriodLimit& setpoint2, const PeriodLimit& cap_limit,
                            const PeriodLimit& cap_discharge_limit) {
    // Only check value of first phase, as this one should be set as first.
    // If the limit of setpoint1 or setpoint2 is set...
    if (!is_equal(setpoint2.limit, NO_SETPOINT_SPECIFIED) or !is_equal(setpoint1.limit, NO_SETPOINT_SPECIFIED)) {
        // ... check if both limits are set ...
        if (!is_equal(setpoint2.limit, NO_SETPOINT_SPECIFIED) && !is_equal(setpoint1.limit, NO_SETPOINT_SPECIFIED)) {
            // Both limits are set, get the min of both in case of charging and the max in case if discharging.
            if (setpoint1.limit < 0.0F) {
                setpoint1.limit = std::max(setpoint1.limit, setpoint2.limit);
            } else {
                setpoint1.limit = std::min(setpoint1.limit, setpoint2.limit);
            }
        }
        // ... check if only setpoint2 limit is set, if so, store setpoint2 limit to setpoint1 limit. We always want a
        // setpoint to be set for setpoint1.
        else if (!is_equal(setpoint2.limit, NO_SETPOINT_SPECIFIED)) {
            setpoint1.limit = setpoint2.limit;
        }

        // If limit is negative, that means discharging, get max limit and cap on discharge limit.
        if (setpoint1.limit < 0.0F) {
            // V2X.04
            setpoint1 = get_max_limit(setpoint1, setpoint2, cap_discharge_limit);
        }
        // And thus positive means charging, get min limit and cap on limit.
        else {
            // V2X.03
            setpoint1 = get_min_limit(setpoint1, setpoint2, cap_limit);
        }
    }
}

///
/// \brief Check if this period uses the second and third phase limit or setpoint values.
/// \param period   The period to check.
/// \return True if at least one L2 or L3 limit is specified.
///
bool uses_three_phase_values(const IntermediatePeriod& period) {
    return !is_equal(period.current_limit.limit_L2, NO_LIMIT_SPECIFIED) ||
           !is_equal(period.current_limit.limit_L3, NO_LIMIT_SPECIFIED) ||
           !is_equal(period.power_limit.limit_L2, NO_LIMIT_SPECIFIED) ||
           !is_equal(period.power_limit.limit_L3, NO_LIMIT_SPECIFIED) ||
           !is_equal(period.current_discharge_limit.limit_L2, NO_DISCHARGE_LIMIT_SPECIFIED) ||
           !is_equal(period.current_discharge_limit.limit_L3, NO_DISCHARGE_LIMIT_SPECIFIED) ||
           !is_equal(period.power_discharge_limit.limit_L2, NO_DISCHARGE_LIMIT_SPECIFIED) ||
           !is_equal(period.power_discharge_limit.limit_L3, NO_DISCHARGE_LIMIT_SPECIFIED) ||
           !is_equal(period.current_setpoint.limit_L2, NO_SETPOINT_SPECIFIED) ||
           !is_equal(period.current_setpoint.limit_L3, NO_SETPOINT_SPECIFIED) ||
           !is_equal(period.power_setpoint.limit_L2, NO_SETPOINT_SPECIFIED) ||
           !is_equal(period.power_setpoint.limit_L3, NO_SETPOINT_SPECIFIED);
}

///
/// \brief Store limit values to values for each phase, but only if input limit values are set.
/// \param input_limit      Input limits
/// \param not_specified    Value to check if limit is set or not. If limit is equal to not_specified, value is not set.
/// \param value            Output value for phase 1.
/// \param value_L2         Output value for phase 2.
/// \param value_L3         Output value for phase 3.
///
void store_limit_to_phase_limits(const PeriodLimit& input_limit, const float& not_specified,
                                 std::optional<float>& value, std::optional<float>& value_L2,
                                 std::optional<float>& value_L3) {
    if (!is_equal(input_limit.limit, not_specified)) {
        value = input_limit.limit;
    }

    if (!is_equal(input_limit.limit_L2, not_specified)) {
        value_L2 = input_limit.limit_L2;
    }

    if (!is_equal(input_limit.limit_L3, not_specified)) {
        value_L3 = input_limit.limit_L3;
    }
}

///
/// \brief If input value is specified, transform the value by 'transform_value' and get min or max of transformed
///        value.
/// \param[in] input                Input value.
/// \param[in] not_specified        If input value has this value, it is not specified, output value 'value' is not
///                                 changed in that case.
/// \param[in] transform_value      The value for transformation of the input value.
/// \param[in/out] value            Value to compare with 'transformed_value', min or max (depending on use_min) of
///                                 `transform_value` and `value` is stored in this variable.
/// \param[in] use_min              Whether to use std::min when comparing the values. True means to use std::min, false
///                                 use std::max.
/// \param[in] use_divide           Whether to divide with transform_value (true) or multiply (false).
///
void convert_and_transform_limit_value(const float& input, const float& not_specified, const float& transform_value,
                                       std::optional<float>& value, const bool use_min, const bool use_divide) {
    if (!is_equal(input, not_specified)) {
        float transformed_value;
        if (use_divide) {
            transformed_value = input / transform_value;
        } else {
            transformed_value = input * transform_value;
        }

        if (value.has_value()) {
            if (use_min) {
                value = std::min(value.value(), transformed_value);
            } else {
                value = std::max(value.value(), transformed_value);
            }

        } else {
            value = transformed_value;
        }
    }
}

///
/// \brief If input_limit is specified, transform the value by `transform_value` (eventually multiplied by
///        `number_phases`) and get min or max of transformed value.
///
/// Transform value is multiplied by number_phases if limit_L2 or limit_L3 is not set.
///
/// \param[in] input_limit          Input value.
/// \param[in] not_specified        If limits of `input_limit` has this value, it is not specified, output value 'value'
///                                 is not changed in that case.
/// \param[in] transform_value      The value for transformation of the input value.
/// \param[in] number_phases        The number of phases of this period.
/// \param[in/out] value            Value to compare with 'transformed_value', min or max (depending on use_min) of
///                                 `transform_value` and `value` is stored in this variable.
/// \param[in/out] value_L2         Same as `value`, but for phase 2.
/// \param[in/out] value_L3         Same as `value`, but for phase 3.
/// \param[in] use_min              Whether to use std::min when comparing the values. True means to use std::min, false
///                                 use std::max.
/// \param[in] use_divide           Whether to divide with transform_value (true) or multiply (false).
///
void convert_and_transform_limit_to_period_schedule(const PeriodLimit& input_limit, const float& not_specified,
                                                    const float& transform_value, const int32_t number_phases,
                                                    std::optional<float>& value, std::optional<float>& value_L2,
                                                    std::optional<float>& value_L3, const bool use_min,
                                                    const bool use_divide) {
    float t = transform_value;

    if (is_equal(input_limit.limit_L2, not_specified) || is_equal(input_limit.limit_L3, not_specified)) {
        t *= static_cast<float>(number_phases);
    }

    convert_and_transform_limit_value(input_limit.limit, not_specified, t, value, use_min, use_divide);

    convert_and_transform_limit_value(input_limit.limit_L2, not_specified, t, value_L2, use_min, use_divide);
    convert_and_transform_limit_value(input_limit.limit_L3, not_specified, t, value_L3, use_min, use_divide);
}

} // namespace v2
} // namespace ocpp
