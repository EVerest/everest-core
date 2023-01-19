// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V16_SMART_CHARGING_HPP
#define OCPP_V16_SMART_CHARGING_HPP

#include <limits>

#include <ocpp/v16/database_handler.hpp>
#include <ocpp/v16/connector.hpp>
#include <ocpp/v16/ocpp_types.hpp>
#include <ocpp/v16/transaction.hpp>

namespace ocpp {
namespace v16 {

const int LOW_VOLTAGE = 230;
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
    boost::optional<ChargingSchedulePeriod> period;
    ocpp::DateTime end_time;
};

/// \brief This class handles and maintains incoming ChargingProfiles and contains the logic
/// to calculate the composite schedules
class SmartChargingHandler {
private:
    std::shared_ptr<DatabaseHandler> database_handler;
    std::map<int32_t, std::shared_ptr<Connector>> connectors;
    std::map<int, ChargingProfile> stack_level_charge_point_max_profiles_map;
    std::mutex charge_point_max_profiles_map_mutex;
    std::mutex tx_default_profiles_map_mutex;
    std::mutex tx_profiles_map_mutex;

    std::unique_ptr<Everest::SteadyTimer> clear_profiles_timer;

    bool clear_profiles(std::map<int32_t, ChargingProfile>& stack_level_profiles_map,
                        boost::optional<int> profile_id_opt, boost::optional<int> connector_id_opt,
                        const int connector_id, boost::optional<int> stack_level_opt,
                        boost::optional<ChargingProfilePurposeType> charging_profile_purpose_opt, bool check_id_only);

    ///
    /// \brief Iterates over the periods of the given \p profile and returns a struct that contains the period and the
    /// absolute end time of the period that refers to the given absoulte \p time as a pair.
    ///
    PeriodDateTimePair find_period_at(const ocpp::DateTime& time, const ChargingProfile& profile,
                                      const int connector_id);

    void clear_expired_profiles();
    int get_number_installed_profiles();

    ///
    /// \brief Gets the absolute start time of the given \p profile for the given \p connector_id for different profile
    /// purposes
    ///
    boost::optional<ocpp::DateTime> get_profile_start_time(const ChargingProfile& profile, const ocpp::DateTime& time,
                                                           const int connector_id);

    ///
    /// \brief Iterates over the periods of the given \p valid_profiles and determines the earliest next absolute period
    /// end time later than \p temp_time
    ///
    ocpp::DateTime get_next_temp_time(const ocpp::DateTime temp_time,
                                      const std::vector<ChargingProfile>& valid_profiles, const int connector_id);

public:
    SmartChargingHandler(std::map<int32_t, std::shared_ptr<Connector>>& connectors,
                         std::shared_ptr<DatabaseHandler> database_handler);

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
    bool clear_all_profiles_with_filter(boost::optional<int> profile_id, boost::optional<int> connector_id,
                                        boost::optional<int> stack_level,
                                        boost::optional<ChargingProfilePurposeType> charging_profile_purpose,
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
    /// \brief Calculates the composite schedule for the given \p valid_profiles and the given \p connector_id .
    ///
    ChargingSchedule calculate_composite_schedule(std::vector<ChargingProfile> valid_profiles,
                                                  const ocpp::DateTime& start_time, const ocpp::DateTime& end_time,
                                                  const int connector_id,
                                                  boost::optional<ChargingRateUnit> charging_rate_unit);
};

bool validate_schedule(const ChargingSchedule& schedule, const int charging_schedule_max_periods,
                       const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units);
bool overlap(const ocpp::DateTime& start_time, const ocpp::DateTime& end_time, const ChargingProfile profile);
int get_requested_limit(const int limit, const int nr_phases, const ChargingRateUnit& requested_unit);
int get_power_limit(const int limit, const int nr_phases, const ChargingRateUnit& unit_of_limit);
ocpp::DateTime get_period_end_time(const int period_index, const ocpp::DateTime& period_start_time,
                                   const ChargingSchedule& schedule,
                                   const std::vector<ChargingSchedulePeriod>& periods);

std::map<ChargingProfilePurposeType, LimitStackLevelPair> get_initial_purpose_and_stack_limits();

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_SMART_CHARGING_HPP
