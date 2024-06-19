// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V16_SMART_CHARGING_HPP
#define OCPP_V16_SMART_CHARGING_HPP

#include <cstddef>
#include <limits>

#include <ocpp/v16/connector.hpp>
#include <ocpp/v16/database_handler.hpp>
#include <ocpp/v16/ocpp_types.hpp>
#include <ocpp/v16/transaction.hpp>

namespace ocpp {
namespace v16 {

const int DEFAULT_AND_MAX_NUMBER_PHASES = 3;
const int HOURS_PER_DAY = 24;
const int SECONDS_PER_HOUR = 3600;
const int SECONDS_PER_DAY = 86400;
const int DAYS_PER_WEEK = 7;

/// \brief Helper struct to calculate Composite Schedule
struct LimitStackLevelPair {
    int limit;
    int stack_level;
};

/// \brief Helper struct to calculate Composite Schedule
struct PeriodDateTimePair {
    std::optional<ChargingSchedulePeriod> period;
    ocpp::DateTime end_time;
};

/// \brief This class handles and maintains incoming ChargingProfiles and contains the logic
/// to calculate the composite schedules
class SmartChargingHandler {
private:
    std::map<int32_t, std::shared_ptr<Connector>> connectors;
    std::shared_ptr<ocpp::v16::DatabaseHandler> database_handler;
    std::map<int, ChargingProfile> stack_level_charge_point_max_profiles_map;
    std::mutex charge_point_max_profiles_map_mutex;
    std::mutex tx_default_profiles_map_mutex;
    std::mutex tx_profiles_map_mutex;
    bool allow_charging_profile_without_start_schedule;

    std::unique_ptr<Everest::SteadyTimer> clear_profiles_timer;

    bool clear_profiles(std::map<int32_t, ChargingProfile>& stack_level_profiles_map, std::optional<int> profile_id_opt,
                        std::optional<int> connector_id_opt, const int connector_id, std::optional<int> stack_level_opt,
                        std::optional<ChargingProfilePurposeType> charging_profile_purpose_opt, bool check_id_only);

    void clear_expired_profiles();
    int get_number_installed_profiles();

public:
    SmartChargingHandler(std::map<int32_t, std::shared_ptr<Connector>>& connectors,
                         std::shared_ptr<DatabaseHandler> database_handler,
                         const bool allow_charging_profile_without_start_schedule);

    ///
    /// \brief validates the given \p profile according to the specification
    ///
    bool validate_profile(ChargingProfile& profile, const int connector_id, bool ignore_no_transaction,
                          const int profile_max_stack_level, const int max_charging_profiles_installed,
                          const int charging_schedule_max_periods,
                          const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units);

    ///
    /// \brief Adds the given \p profile to the collection of stack_level_charge_point_max_profiles_map
    ///
    void add_charge_point_max_profile(const ChargingProfile& profile);

    ///
    /// \brief Adds the given \p profile to the collection of stack_level_tx_default_profiles_map of the respective
    /// connector or to all connectors if \p connector_id is 0
    ///
    void add_tx_default_profile(const ChargingProfile& profile, const int connector_id);

    ///
    /// \brief Adds the given \p profile to the collection of stack_level_tx_profiles_map of the respective
    /// connector
    ///
    void add_tx_profile(const ChargingProfile& profile, const int connector_id);

    ///
    /// \brief Clears all charging profiles using optional filters
    ///
    bool clear_all_profiles_with_filter(std::optional<int> profile_id, std::optional<int> connector_id,
                                        std::optional<int> stack_level,
                                        std::optional<ChargingProfilePurposeType> charging_profile_purpose,
                                        bool check_id_only);

    ///
    /// \brief Clears all present profiles from all collections
    ///
    void clear_all_profiles();

    ///
    /// \brief Gets all valid profiles within the given absoulte \p start_time and absolute \p end_time for the given \p
    /// connector_id
    ///
    std::vector<ChargingProfile> get_valid_profiles(const ocpp::DateTime& start_time, const ocpp::DateTime& end_time,
                                                    const int connector_id);
    ///
    /// \brief Calculates the enhanced composite schedule for the given \p valid_profiles and the given \p connector_id
    ///
    EnhancedChargingSchedule calculate_enhanced_composite_schedule(const std::vector<ChargingProfile>& valid_profiles,
                                                                   const ocpp::DateTime& start_time,
                                                                   const ocpp::DateTime& end_time,
                                                                   const int connector_id,
                                                                   std::optional<ChargingRateUnit> charging_rate_unit);
    ///
    /// \brief Calculates the composite schedule for the given \p valid_profiles and the given \p connector_id
    ///
    ChargingSchedule calculate_composite_schedule(const std::vector<ChargingProfile>& valid_profiles,
                                                  const ocpp::DateTime& start_time, const ocpp::DateTime& end_time,
                                                  const int connector_id,
                                                  std::optional<ChargingRateUnit> charging_rate_unit);
};

bool validate_schedule(const ChargingSchedule& schedule, const int charging_schedule_max_periods,
                       const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_SMART_CHARGING_HPP
