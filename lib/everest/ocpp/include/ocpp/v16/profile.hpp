// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef PROFILE_H
#define PROFILE_H

#include <cstdint>
#include <optional>
#include <vector>

#include <ocpp/common/types.hpp>

namespace ocpp::v16 {
class ChargingProfile;
class EnhancedChargingSchedulePeriod;
class ChargingSchedulePeriod;

constexpr float no_limit_specified = -1.0;

using ocpp::DateTime;

struct IntermediatePeriod {
    int32_t startPeriod;
    float current_limit;
    float power_limit;
    int32_t stack_level_current;
    int32_t stack_level_power;
    std::optional<int32_t> numberPhases;
    std::optional<int32_t> phaseToUse;
};

using IntermediateProfile = std::vector<IntermediatePeriod>;

struct period_entry_t {
    void init(const DateTime& in_start, int in_duration, const ChargingSchedulePeriod& in_period,
              const ChargingProfile& in_profile);
    bool validate(const ChargingProfile& profile, const DateTime& now);

    DateTime start;
    DateTime end;
    float limit;
    std::optional<std::int32_t> number_phases;
    std::int32_t stack_level;
    ChargingRateUnit charging_rate_unit;
    std::optional<float> min_charging_rate;
};

/// \brief Calculate the number of seconds elapsed between \param to and \param from
int32_t elapsed_seconds(const ocpp::DateTime& to, const ocpp::DateTime& from);

/// \brief Rounds down the \param dt to the nearest second
ocpp::DateTime floor_seconds(const ocpp::DateTime& dt);

std::vector<DateTime> calculate_start(const DateTime& now, const DateTime& end,
                                      const std::optional<DateTime>& session_start, const ChargingProfile& profile);
std::vector<period_entry_t> calculate_profile_entry(const DateTime& now, const DateTime& end,
                                                    const std::optional<DateTime>& session_start,
                                                    const ChargingProfile& profile, std::uint8_t period_index);
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
                                                   ChargingProfilePurposeType purpose);

/// \brief generates an IntermediateProfile by sorting and processing a list of period_entry_t intervals within a given
/// time range, resolving overlaps based on stack priority and filling any time gaps with default values.
/// \param periods the list of periods to build into the profile
/// \param now the start of the composite schedule
/// \param end the end (i.e. duration) of the composite schedule \return a list of profile periods with calculated date
/// & time start and end times
IntermediateProfile generate_profile_from_periods(std::vector<period_entry_t>& periods, const DateTime& now,
                                                  const DateTime& end);

/// \brief Generates a new profile by combining a \param tx_profile and a \param tx_default_profile.
/// The tx_profile will be preferred over the tx_default_profile whenever it has a value
/// \return A combined profile
IntermediateProfile merge_tx_profile_with_tx_default_profile(const IntermediateProfile& tx_profile,
                                                             const IntermediateProfile& tx_default_profile);

/// \brief Generates a new profile by taking the lowest limit of all the provided \param profiles
IntermediateProfile merge_profiles_by_lowest_limit(const std::vector<IntermediateProfile>& profiles);

/// \brief Generates a new profile by summing the limits of all the provided \param profiles, filling in defaults
/// wherever a profile has no limit
IntermediateProfile merge_profiles_by_summing_limits(const std::vector<IntermediateProfile>& profiles,
                                                     float current_default, float power_default);

/// \brief Convert an intermediate profile into a final charging schedule.
/// This will fill in defaults and convert merge the current and power limits into the final \p charging_rate_unit based
/// limit
std::vector<EnhancedChargingSchedulePeriod>
convert_intermediate_into_schedule(const IntermediateProfile& profile, ChargingRateUnit charging_rate_unit,
                                   float default_limit, int32_t default_number_phases, float supply_voltage);

} // namespace ocpp::v16

#endif // PROFILE_H
