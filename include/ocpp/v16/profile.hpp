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
class EnhancedChargingSchedule;
class ChargingSchedulePeriod;

constexpr float no_limit_specified = -1.0;

using ocpp::DateTime;

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

std::vector<DateTime> calculate_start(const DateTime& now, const DateTime& end,
                                      const std::optional<DateTime>& session_start, const ChargingProfile& profile);
std::vector<period_entry_t> calculate_profile_entry(const DateTime& now, const DateTime& end,
                                                    const std::optional<DateTime>& session_start,
                                                    const ChargingProfile& profile, std::uint8_t period_index);
std::vector<period_entry_t> calculate_profile(const DateTime& now, const DateTime& end,
                                              const std::optional<DateTime>& session_start,
                                              const ChargingProfile& profile);

EnhancedChargingSchedule calculate_composite_schedule(std::vector<period_entry_t>& combined_schedules,
                                                      const DateTime& now, const DateTime& end,
                                                      std::optional<ChargingRateUnit> charging_rate_unit,
                                                      int32_t default_number_phases, int32_t supply_voltage);

EnhancedChargingSchedule calculate_composite_schedule(const EnhancedChargingSchedule& charge_point_max,
                                                      const EnhancedChargingSchedule& tx_default,
                                                      const EnhancedChargingSchedule& tx,
                                                      const CompositeScheduleDefaultLimits& default_limits,
                                                      int32_t supply_voltage);

} // namespace ocpp::v16

#endif // PROFILE_H
