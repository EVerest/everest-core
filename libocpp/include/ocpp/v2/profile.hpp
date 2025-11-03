// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/constants.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

struct PeriodLimit {
    float limit;
    float limit_L2;
    float limit_L3;
    bool operator==(const PeriodLimit& other) const;
    bool operator!=(const PeriodLimit& other) const;
};

struct IntermediatePeriod {
    int32_t startPeriod;
    PeriodLimit current_limit;
    PeriodLimit power_limit;
    PeriodLimit current_discharge_limit;
    PeriodLimit power_discharge_limit;
    PeriodLimit current_setpoint;
    PeriodLimit power_setpoint;
    std::optional<int32_t> numberPhases;
    std::optional<int32_t> phaseToUse;
};

using IntermediateProfile = std::vector<IntermediatePeriod>;

/// \brief Returns elements from a specific ChargingProfile and ChargingSchedulePeriod
///        for use in the calculation of the CompositeSchedule within a specific slice
///        of time. These are aggregated by Profile.
/// \param in_start The starting time
/// \param in_duration The duration for the specific slice of time
/// \param in_period the details of this period
/// \param in_profile the charging profile
/// \return an entry with smart charging information for a specific period in time
struct period_entry_t {
    void init(const ocpp::DateTime& in_start, int in_duration, const ChargingSchedulePeriod& in_period,
              const ChargingProfile& in_profile);
    bool validate(const ChargingProfile& profile, const ocpp::DateTime& now);

    ocpp::DateTime start;
    ocpp::DateTime end;
    PeriodLimit limit;
    PeriodLimit discharge_limit;
    PeriodLimit setpoint;
    std::optional<std::int32_t> number_phases;
    std::optional<std::int32_t> phase_to_use;
    std::int32_t stack_level;
    ChargingRateUnitEnum charging_rate_unit;
    std::optional<float> min_charging_rate;

    bool equals(const period_entry_t& other) const {
        return (start == other.start) && (end == other.end) && (limit == other.limit) &&
               (discharge_limit == other.discharge_limit) && (setpoint == other.setpoint) &&
               (number_phases == other.number_phases) && (stack_level == other.stack_level) &&
               (charging_rate_unit == other.charging_rate_unit) && (min_charging_rate == other.min_charging_rate);
    }
};

/// \brief Calculate the number of seconds elapsed between \param to and \param from
int32_t elapsed_seconds(const ocpp::DateTime& to, const ocpp::DateTime& from);

/// \brief Rounds down the \param dt to the nearest second
ocpp::DateTime floor_seconds(const ocpp::DateTime& dt);

/// \brief calculate the start times for the profile
/// \param now the current date and time
/// \param end the end of the composite schedule
/// \param session_start optional when the charging session started
/// \param profile the charging profile
/// \return a list of the start times of the profile
std::vector<DateTime> calculate_start(const DateTime& now, const DateTime& end,
                                      const std::optional<DateTime>& session_start, const ChargingProfile& profile);

/// \brief Calculates the period entries based upon the indicated time window for every profile passed in.
/// \param now the current date and time
/// \param end the end of the composite schedule
/// \param session_start optional when the charging session started
/// \param profile the charging profile
/// \param period_index the schedule period index
/// \note  used by calculate_profile
/// \return the list of start times
std::vector<period_entry_t> calculate_profile_entry(const DateTime& now, const DateTime& end,
                                                    const std::optional<DateTime>& session_start,
                                                    const ChargingProfile& profile, std::uint8_t period_index);

/// \brief generate an ordered list of valid schedule periods for the profile
/// \param now the current date and time
/// \param end ignore entries beyond this date and time (i.e. that start after end)
/// \param session_start optional when the charging session started
/// \param profile the charging profile
/// \return a list of profile periods with calculated date & time start and end times
/// \note it is valid for there to be gaps (for recurring profiles)
std::vector<period_entry_t> calculate_profile(const DateTime& now, const DateTime& end,
                                              const std::optional<DateTime>& session_start,
                                              const ChargingProfile& profile);

/// \brief generate an ordered list of valid schedule periods for the profiles
/// \param now the current date and time
/// \param end ignore entries beyond this date and time (i.e. that start after end)
/// \param session_start optional when the charging session started
/// \param profiles A vector of charging profiles
/// \param purpose The purpose to generate the list for. Only profiles with this purpose are included.
/// \return a list of profile periods with calculated date & time start and end times
/// \note it is valid for there to be gaps (for recurring profiles)
std::vector<period_entry_t> calculate_all_profiles(const DateTime& now, const DateTime& end,
                                                   const std::optional<DateTime>& session_start,
                                                   const std::vector<ChargingProfile>& profiles,
                                                   ChargingProfilePurposeEnum purpose);

/// \brief calculate the profile for the list of periods
/// \param periods the list of periods to build into the profile
/// \param now the start of the composite schedule
/// \param end the end (i.e. duration) of the composite schedule
/// \param charging_rate_unit the units to use (defaults to Amps)
/// \param default_number_phases default number of phases if no existing period limit applies
/// \param supply_voltage Supply voltage of the grid. This value is only used in case a conversion between smart
/// charging amp and watt limits is required \return the calculated composite schedule
IntermediateProfile generate_profile_from_periods(std::vector<period_entry_t>& periods, const DateTime& now,
                                                  const DateTime& end);

/// \brief Generates a new profile by combining a \param tx_profile and a \param tx_default_profile.
/// The tx_profile will be preferred over the tx_default_profile whenever it has a value
/// \return A combined profile
IntermediateProfile merge_tx_profile_with_tx_default_profile(const IntermediateProfile& tx_profile,
                                                             const IntermediateProfile& tx_default_profile);

/// \brief Generates a new profile by taking the lowest limit of all the provided \param profiles
IntermediateProfile merge_profiles_by_lowest_limit(const std::vector<IntermediateProfile>& profiles,
                                                   const OcppProtocolVersion ocpp_version);

/// \brief Generates a new profile by summing the limits of all the provided \param profiles, filling in defaults
/// wherever a profile has no limit
IntermediateProfile merge_profiles_by_summing_limits(const std::vector<IntermediateProfile>& profiles,
                                                     float current_default, float power_default,
                                                     const OcppProtocolVersion ocpp_version);

/// \brief Fills all the periods without a limit or a number of phases with the defaults provided
void fill_gaps_with_defaults(IntermediateProfile& schedule, float default_limit, int32_t default_number_phases);

/// \brief Convert an intermediate profile into a final charging schedule.
/// This will fill in defaults and convert merge the current and power limits into the final \p charging_rate_unit based
/// limit
std::vector<ChargingSchedulePeriod>
convert_intermediate_into_schedule(const IntermediateProfile& profile, ChargingRateUnitEnum charging_rate_unit,
                                   float default_limit, int32_t default_number_phases, float supply_voltage);

} // namespace v2
} // namespace ocpp
