// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/v16/smart_charging.hpp>

using namespace std::chrono;

using QueryExecutionException = ocpp::common::QueryExecutionException;

namespace ocpp {
namespace v16 {

bool validate_schedule(const ChargingSchedule& schedule, const int charging_schedule_max_periods,
                       const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units) {

    if (schedule.chargingSchedulePeriod.size() > (size_t)charging_schedule_max_periods) {
        EVLOG_warning << "INVALID SCHEDULE - Number of chargingSchedulePeriod(s) is greater than configured "
                         "ChargingScheduleMaxPeriods of "
                      << charging_schedule_max_periods;
        return false;
    }

    if (std::find(charging_schedule_allowed_charging_rate_units.begin(),
                  charging_schedule_allowed_charging_rate_units.end(),
                  schedule.chargingRateUnit) == charging_schedule_allowed_charging_rate_units.end()) {
        EVLOG_warning << "INVALID SCHEDULE - ChargingRateUnit not supported: " << schedule.chargingRateUnit;
        return false;
    }

    for (const auto& period : schedule.chargingSchedulePeriod) {
        if (period.numberPhases &&
            (period.numberPhases.value() <= 0 or period.numberPhases.value() > DEFAULT_AND_MAX_NUMBER_PHASES)) {
            EVLOG_warning << "INVALID SCHEDULE - Invalid number of phases: " << period.numberPhases.value();
            return false;
        } else if (period.limit < 0) {
            EVLOG_warning << "INVALID SCHEDULE - Invalid limit: " << period.limit;
            return false;
        }
    }
    return true;
}

bool overlap(const ocpp::DateTime& start_time, const ocpp::DateTime& end_time, const ChargingProfile profile) {
    ocpp::DateTime latest_start(start_time);
    ocpp::DateTime earliest_end(end_time);
    if (profile.validFrom && profile.validFrom.value() > start_time) {
        latest_start = profile.validFrom.value();
    }
    if (profile.validTo && profile.validTo.value() < end_time) {
        earliest_end = profile.validTo.value();
    }
    const auto delta = earliest_end.to_time_point() - latest_start.to_time_point();
    return delta.count() > 0;
}

int get_requested_limit(const int limit, const int nr_phases, const ChargingRateUnit& requested_unit) {
    if (requested_unit == ChargingRateUnit::A) {
        return limit / (LOW_VOLTAGE * nr_phases);
    } else {
        return limit;
    }
}

int get_power_limit(const int limit, const int nr_phases, const ChargingRateUnit& unit_of_limit) {
    if (unit_of_limit == ChargingRateUnit::W) {
        return limit;
    } else {
        return limit * LOW_VOLTAGE * nr_phases;
    }
}

ocpp::DateTime get_period_end_time(const int period_index, const ocpp::DateTime& period_start_time,
                                   const ChargingSchedule& schedule,
                                   const std::vector<ChargingSchedulePeriod>& periods) {

    std::optional<ocpp::DateTime> period_end_time;

    int period_diff_in_seconds;
    if ((size_t)period_index + 1 < periods.size()) {
        int duration;
        if (schedule.duration) {
            duration = schedule.duration.value();
        } else {
            duration = std::numeric_limits<int>::max();
        }

        if (periods.at(period_index + 1).startPeriod < duration) {
            period_diff_in_seconds = periods.at(period_index + 1).startPeriod - periods.at(period_index).startPeriod;
        } else {
            period_diff_in_seconds = duration - periods.at(period_index).startPeriod;
        }
        const auto dt = ocpp::DateTime(period_start_time.to_time_point() + seconds(period_diff_in_seconds));
        return dt;
    } else if (schedule.duration) {
        period_diff_in_seconds = schedule.duration.value() - periods.at(period_index).startPeriod;
        return ocpp::DateTime(period_start_time.to_time_point() + seconds(period_diff_in_seconds));
    } else {
        return ocpp::DateTime(date::utc_clock::now() + hours(std::numeric_limits<int>::max()));
    }
}

std::map<ChargingProfilePurposeType, LimitStackLevelPair> get_initial_purpose_and_stack_limits() {
    std::map<ChargingProfilePurposeType, LimitStackLevelPair> map;
    map[ChargingProfilePurposeType::ChargePointMaxProfile] = {std::numeric_limits<int>::max(), -1};
    map[ChargingProfilePurposeType::TxDefaultProfile] = {std::numeric_limits<int>::max(), -1};
    map[ChargingProfilePurposeType::TxProfile] = {std::numeric_limits<int>::max(), -1};
    return map;
}

SmartChargingHandler::SmartChargingHandler(std::map<int32_t, std::shared_ptr<Connector>>& connectors,
                                           std::shared_ptr<DatabaseHandler> database_handler,
                                           const bool allow_charging_profile_without_start_schedule) :
    connectors(connectors),
    database_handler(database_handler),
    allow_charging_profile_without_start_schedule(allow_charging_profile_without_start_schedule) {
    this->clear_profiles_timer = std::make_unique<Everest::SteadyTimer>();
    this->clear_profiles_timer->interval([this]() { this->clear_expired_profiles(); }, hours(HOURS_PER_DAY));
}

PeriodDateTimePair SmartChargingHandler::find_period_at(const ocpp::DateTime& time, const ChargingProfile& profile,
                                                        const int connector_id) {

    auto period_start_time = this->get_profile_start_time(profile, time, connector_id);
    const auto schedule = profile.chargingSchedule;

    if (period_start_time) {
        const auto periods = schedule.chargingSchedulePeriod;
        time_point<date::utc_clock> period_end_time;
        for (size_t i = 0; i < periods.size(); i++) {
            const auto period_end_time = get_period_end_time(i, period_start_time.value(), schedule, periods);
            if (time >= period_start_time.value() && time < period_end_time) {
                return {periods.at(i), ocpp::DateTime(period_end_time)};
            }
            period_start_time.emplace(ocpp::DateTime(period_end_time));
        }
    }

    return {std::nullopt, ocpp::DateTime(date::utc_clock::now() + hours(std::numeric_limits<int>::max()))};
}

void SmartChargingHandler::clear_expired_profiles() {
    EVLOG_debug << "Scanning all installed profiles and clearing expired profiles";

    const auto now = date::utc_clock::now();
    std::lock_guard<std::mutex> lk(this->charge_point_max_profiles_map_mutex);
    for (auto it = this->stack_level_charge_point_max_profiles_map.cbegin();
         it != this->stack_level_charge_point_max_profiles_map.cend();) {
        const auto& validTo = it->second.validTo;
        if (validTo.has_value() and validTo.value().to_time_point() < now) {
            this->stack_level_charge_point_max_profiles_map.erase(it++);
            this->database_handler->delete_charging_profile(it->second.chargingProfileId);
        } else {
            ++it;
        }
    }
}

int SmartChargingHandler::get_number_installed_profiles() {
    int number = 0;

    std::lock_guard<std::mutex> lk_cp(this->charge_point_max_profiles_map_mutex);
    std::lock_guard<std::mutex> lk_txd(this->tx_default_profiles_map_mutex);
    std::lock_guard<std::mutex> lk_tx(this->tx_profiles_map_mutex);

    number += this->stack_level_charge_point_max_profiles_map.size();
    for (const auto& [connector_id, connector] : this->connectors) {
        number += connector->stack_level_tx_default_profiles_map.size();
        number += connector->stack_level_tx_profiles_map.size();
    }

    return number;
}

ChargingSchedule SmartChargingHandler::calculate_composite_schedule(
    std::vector<ChargingProfile> valid_profiles, const ocpp::DateTime& start_time, const ocpp::DateTime& end_time,
    const int connector_id, std::optional<ChargingRateUnit> charging_rate_unit) {
    const auto enhanced_composite_schedule = this->calculate_enhanced_composite_schedule(
        valid_profiles, start_time, end_time, connector_id, charging_rate_unit);
    ChargingSchedule composite_schedule;
    composite_schedule.chargingRateUnit = enhanced_composite_schedule.chargingRateUnit;
    composite_schedule.duration = enhanced_composite_schedule.duration;
    composite_schedule.startSchedule = enhanced_composite_schedule.startSchedule;
    composite_schedule.minChargingRate = enhanced_composite_schedule.minChargingRate;
    for (const auto enhanced_period : enhanced_composite_schedule.chargingSchedulePeriod) {
        ChargingSchedulePeriod period;
        period.startPeriod = enhanced_period.startPeriod;
        period.limit = enhanced_period.limit;
        period.numberPhases = enhanced_period.numberPhases;
        composite_schedule.chargingSchedulePeriod.push_back(period);
    }
    return composite_schedule;
}

EnhancedChargingSchedule SmartChargingHandler::calculate_enhanced_composite_schedule(
    std::vector<ChargingProfile> valid_profiles, const ocpp::DateTime& start_time, const ocpp::DateTime& end_time,
    const int connector_id, std::optional<ChargingRateUnit> charging_rate_unit) {
    // return in amps if not given
    if (!charging_rate_unit) {
        charging_rate_unit.emplace(ChargingRateUnit::A);
    }

    EnhancedChargingSchedule composite_schedule; // the schedule that will be returned
    composite_schedule.chargingRateUnit = charging_rate_unit.value();
    composite_schedule.duration.emplace(
        duration_cast<seconds>(end_time.to_time_point() - start_time.to_time_point()).count());

    std::vector<EnhancedChargingSchedulePeriod> periods;

    ocpp::DateTime temp_time(start_time);
    ocpp::DateTime last_period_end_time(end_time);
    auto current_period_limit = std::numeric_limits<int>::max();
    LimitStackLevelPair significant_limit_stack_level_pair = {std::numeric_limits<int>::max(), -1};

    // calculate every ChargingSchedulePeriod of result within this while loop
    while (duration_cast<seconds>(end_time.to_time_point() - temp_time.to_time_point()).count() > 0) {
        auto current_purpose_and_stack_limits =
            get_initial_purpose_and_stack_limits(); // this data structure holds the current lowest limit and stack
                                                    // level for every purpose
        ocpp::DateTime temp_period_end_time;
        int temp_number_phases;
        for (const auto& profile : valid_profiles) {
            if (profile.stackLevel > current_purpose_and_stack_limits.at(profile.chargingProfilePurpose).stack_level) {
                const auto period_period_end_time_pair = this->find_period_at(
                    temp_time, profile, connector_id); // this data structure holds the respective period and period end
                                                       // time for temp_time point in time
                const auto period_opt = period_period_end_time_pair.period;
                const auto period_end_time = period_period_end_time_pair.end_time;
                if (period_opt) {
                    const auto period = period_opt.value();
                    temp_period_end_time = period_end_time;
                    if (period.numberPhases) {
                        temp_number_phases = period.numberPhases.value();
                    } else {
                        temp_number_phases = DEFAULT_AND_MAX_NUMBER_PHASES;
                    }

                    // update data structure with limit and stack level for this profile
                    current_purpose_and_stack_limits.at(profile.chargingProfilePurpose).limit =
                        get_power_limit(period.limit, temp_number_phases, profile.chargingSchedule.chargingRateUnit);
                    current_purpose_and_stack_limits.at(profile.chargingProfilePurpose).stack_level =
                        profile.stackLevel;
                } else {
                    // skip profiles with a lower stack level if a higher stack level already has a limit for this point
                    // in time
                    continue;
                }
            }
        }

        // if there is a limit with purpose TxProfile it overrules the limit of purpose TxDefaultProfile
        if (current_purpose_and_stack_limits.at(ChargingProfilePurposeType::TxProfile).limit !=
            std::numeric_limits<int>::max()) {
            significant_limit_stack_level_pair =
                current_purpose_and_stack_limits.at(ChargingProfilePurposeType::TxProfile);
        } else {
            significant_limit_stack_level_pair =
                current_purpose_and_stack_limits.at(ChargingProfilePurposeType::TxDefaultProfile);
        }

        if (current_purpose_and_stack_limits.at(ChargingProfilePurposeType::ChargePointMaxProfile).limit <
            significant_limit_stack_level_pair.limit) {
            significant_limit_stack_level_pair =
                current_purpose_and_stack_limits.at(ChargingProfilePurposeType::ChargePointMaxProfile);
        }

        // insert new period to result only if limit changed or period was found
        if (significant_limit_stack_level_pair.limit != current_period_limit and
            significant_limit_stack_level_pair.limit != std::numeric_limits<int>::max()) {

            EnhancedChargingSchedulePeriod new_period;
            const auto start_period =
                duration_cast<seconds>(temp_time.to_time_point() - start_time.to_time_point()).count();
            new_period.startPeriod = start_period;
            new_period.limit = get_requested_limit(significant_limit_stack_level_pair.limit, temp_number_phases,
                                                   charging_rate_unit.value());
            new_period.numberPhases = temp_number_phases;
            new_period.stackLevel = significant_limit_stack_level_pair.stack_level;
            periods.push_back(new_period);

            last_period_end_time = temp_period_end_time;
            current_period_limit = significant_limit_stack_level_pair.limit;
        }
        temp_time = this->get_next_temp_time(temp_time, valid_profiles, connector_id);
    }

    // update duration if end time of last period is smaller than requested end time
    if (last_period_end_time.to_time_point() - start_time.to_time_point() <
        (end_time.to_time_point() - start_time.to_time_point())) {
        composite_schedule.duration =
            duration_cast<seconds>(last_period_end_time.to_time_point() - start_time.to_time_point()).count();
    }
    composite_schedule.chargingSchedulePeriod = periods;

    return composite_schedule;
}

bool SmartChargingHandler::validate_profile(
    ChargingProfile& profile, const int connector_id, bool ignore_no_transaction, const int profile_max_stack_level,
    const int max_charging_profiles_installed, const int charging_schedule_max_periods,
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units) {
    if ((size_t)connector_id > this->connectors.size() or connector_id < 0 or profile.stackLevel < 0 or
        profile.stackLevel > profile_max_stack_level) {
        EVLOG_warning << "INVALID PROFILE - connector_id invalid or invalid stack level";
        return false;
    }

    if (profile.chargingProfileKind == ChargingProfileKindType::Absolute && !profile.chargingSchedule.startSchedule) {
        EVLOG_warning << "INVALID PROFILE - Absolute Profile Kind with no given startSchedule";
        if (this->allow_charging_profile_without_start_schedule) {
            EVLOG_warning << "Allowing profile because AllowChargingProfileWithoutStartSchedule is configured";
            profile.chargingSchedule.startSchedule = ocpp::DateTime();
        } else {
            return false;
        }
    }

    if (this->get_number_installed_profiles() >= max_charging_profiles_installed) {
        EVLOG_warning << "Maximum amount of profiles are installed at the charging point";
        return false;
    }

    if (!validate_schedule(profile.chargingSchedule, charging_schedule_max_periods,
                           charging_schedule_allowed_charging_rate_units)) {
        return false;
    }

    if (profile.chargingProfileKind == ChargingProfileKindType::Recurring) {
        if (!profile.recurrencyKind) {
            EVLOG_warning << "INVALID PROFILE - Recurring Profile Kind with no given RecurrencyKind";
            return false;
        }
        if (!profile.chargingSchedule.startSchedule) {
            EVLOG_warning << "INVALID PROFILE - Recurring Profile Kind with no startSchedule";
            if (this->allow_charging_profile_without_start_schedule) {
                EVLOG_warning << "Allowing profile because AllowChargingProfileWithoutStartSchedule is configured";
                profile.chargingSchedule.startSchedule = ocpp::DateTime();
            } else {
                return false;
            }
        }
        if (profile.chargingSchedule.duration) {
            int max_recurrency_duration;
            if (profile.recurrencyKind == RecurrencyKindType::Daily) {
                max_recurrency_duration = SECONDS_PER_DAY;
            } else {
                max_recurrency_duration = SECONDS_PER_DAY * DAYS_PER_WEEK;
            }

            if (profile.chargingSchedule.duration > max_recurrency_duration) {
                EVLOG_warning
                    << "Given duration of Recurring profile was > than max_recurrency_duration. Setting duration of "
                       "schedule to max_currency_duration";
                profile.chargingSchedule.duration = max_recurrency_duration;
            }
        }
    }

    if (profile.chargingProfilePurpose == ChargingProfilePurposeType::ChargePointMaxProfile) {
        if (connector_id == 0 and profile.chargingProfileKind != ChargingProfileKindType::Relative) {
            return true;
        } else {
            EVLOG_warning
                << "INVALID PROFILE - connector_id != 0 with purpose ChargePointMaxProfile or kind is Relative";
            return false;
        }
    } else if (profile.chargingProfilePurpose == ChargingProfilePurposeType::TxDefaultProfile) {
        return true;
    } else if (profile.chargingProfilePurpose == ChargingProfilePurposeType::TxProfile) {
        if (connector_id == 0) {
            EVLOG_warning << "INVALID PROFILE - connector_id is 0";
            return false;
        }

        const auto& connector = this->connectors.at(connector_id);

        if (connector->transaction == nullptr && !ignore_no_transaction) {
            EVLOG_warning << "INVALID PROFILE - No active transaction at this connector";
            return false;
        }

        if (profile.transactionId.has_value()) {
            if (connector->transaction == nullptr) {
                EVLOG_warning << "INVALID PROFILE - profile.transaction_id is present but no transaction is active at "
                                 "this connector";
                return false;
            }

            if (connector->transaction->get_transaction_id() != profile.transactionId) {
                EVLOG_warning << "INVALID PROFILE - transaction_id doesn't match for purpose TxProfile";
                return false;
            }
        }
    }
    return true;
}

void SmartChargingHandler::add_charge_point_max_profile(const ChargingProfile& profile) {
    std::lock_guard<std::mutex> lk(this->charge_point_max_profiles_map_mutex);
    this->stack_level_charge_point_max_profiles_map[profile.stackLevel] = profile;
    try {
        this->database_handler->insert_or_update_charging_profile(0, profile);
    } catch (const QueryExecutionException& e) {
        EVLOG_warning << "Could not store ChargePointMaxProfile in the database: " << e.what();
    }
}

void SmartChargingHandler::add_tx_default_profile(const ChargingProfile& profile, const int connector_id) {
    std::lock_guard<std::mutex> lk(this->tx_default_profiles_map_mutex);
    if (connector_id == 0) {
        for (size_t id = 1; id <= this->connectors.size() - 1; id++) {
            this->connectors.at(id)->stack_level_tx_default_profiles_map[profile.stackLevel] = profile;
            try {
                this->database_handler->insert_or_update_charging_profile(connector_id, profile);
            } catch (const QueryExecutionException& e) {
                EVLOG_warning << "Could not store TxDefaultProfile in the database: " << e.what();
            }
        }
    } else {
        this->connectors.at(connector_id)->stack_level_tx_default_profiles_map[profile.stackLevel] = profile;
        try {
            this->database_handler->insert_or_update_charging_profile(connector_id, profile);
        } catch (const QueryExecutionException& e) {
            EVLOG_warning << "Could not store TxDefaultProfile in the database: " << e.what();
        }
    }
}

void SmartChargingHandler::add_tx_profile(const ChargingProfile& profile, const int connector_id) {
    std::lock_guard<std::mutex> lk(this->tx_profiles_map_mutex);
    this->connectors.at(connector_id)->stack_level_tx_profiles_map[profile.stackLevel] = profile;
    try {
        this->database_handler->insert_or_update_charging_profile(connector_id, profile);
    } catch (const QueryExecutionException& e) {
        EVLOG_warning << "Could not store TxProfile in the database: " << e.what();
    }
}

bool SmartChargingHandler::clear_profiles(std::map<int32_t, ChargingProfile>& stack_level_profiles_map,
                                          std::optional<int> profile_id_opt, std::optional<int> connector_id_opt,
                                          const int connector_id, std::optional<int> stack_level_opt,
                                          std::optional<ChargingProfilePurposeType> charging_profile_purpose_opt,
                                          bool check_id_only) {
    bool erased_at_least_one = false;

    for (auto it = stack_level_profiles_map.cbegin(); it != stack_level_profiles_map.cend();) {
        if (profile_id_opt && it->second.chargingProfileId == profile_id_opt.value()) {
            EVLOG_info << "Clearing ChargingProfile with id: " << it->second.chargingProfileId;
            try {
                this->database_handler->delete_charging_profile(it->second.chargingProfileId);
            } catch (const QueryExecutionException& e) {
                EVLOG_warning << "Could not delete ChargingProfile from the database: " << e.what();
            }
            stack_level_profiles_map.erase(it++);
            erased_at_least_one = true;
        } else if (!check_id_only and (!connector_id_opt or connector_id_opt.value() == connector_id) and
                   (!stack_level_opt or stack_level_opt.value() == it->first) and
                   (!charging_profile_purpose_opt or
                    charging_profile_purpose_opt.value() == it->second.chargingProfilePurpose)) {
            EVLOG_info << "Clearing ChargingProfile with id: " << it->second.chargingProfileId;
            try {
                this->database_handler->delete_charging_profile(it->second.chargingProfileId);
            } catch (const QueryExecutionException& e) {
                EVLOG_warning << "Could not delete ChargingProfile from the database: " << e.what();
            }
            stack_level_profiles_map.erase(it++);
            erased_at_least_one = true;
        } else {
            ++it;
        }
    }
    return erased_at_least_one;
}

bool SmartChargingHandler::clear_all_profiles_with_filter(
    std::optional<int> profile_id_opt, std::optional<int> connector_id_opt, std::optional<int> stack_level_opt,
    std::optional<ChargingProfilePurposeType> charging_profile_purpose_opt, bool check_id_only) {

    // for ChargePointMaxProfile
    auto erased_charge_point_max_profile =
        this->clear_profiles(this->stack_level_charge_point_max_profiles_map, profile_id_opt, connector_id_opt, 0,
                             stack_level_opt, charging_profile_purpose_opt, check_id_only);

    bool erased_at_least_one_tx_profile = false;

    for (auto& [connector_id, connector] : this->connectors) {
        // for TxDefaultProfiles
        auto tmp_erased_tx_default_profile =
            this->clear_profiles(connector->stack_level_tx_default_profiles_map, profile_id_opt, connector_id_opt,
                                 connector_id, stack_level_opt, charging_profile_purpose_opt, check_id_only);
        // for TxProfiles
        auto tmp_erased_tx_profile =
            this->clear_profiles(connector->stack_level_tx_profiles_map, profile_id_opt, connector_id_opt, connector_id,
                                 stack_level_opt, charging_profile_purpose_opt, check_id_only);

        if (!erased_at_least_one_tx_profile and (tmp_erased_tx_profile or tmp_erased_tx_default_profile)) {
            erased_at_least_one_tx_profile = true;
        }
    }
    return erased_charge_point_max_profile or erased_at_least_one_tx_profile;
}

void SmartChargingHandler::clear_all_profiles() {
    EVLOG_info << "Clearing all charging profiles";
    std::lock_guard<std::mutex> lk_cp(this->charge_point_max_profiles_map_mutex);
    std::lock_guard<std::mutex> lk_txd(this->tx_default_profiles_map_mutex);
    std::lock_guard<std::mutex> lk_tx(this->tx_profiles_map_mutex);
    this->stack_level_charge_point_max_profiles_map.clear();

    for (auto& [connector_id, connector] : this->connectors) {
        connector->stack_level_tx_default_profiles_map.clear();
        connector->stack_level_tx_profiles_map.clear();
    }

    try {
        this->database_handler->delete_charging_profiles();
    } catch (const QueryExecutionException& e) {
        EVLOG_warning << "Could not delete ChargingProfile from the database: " << e.what();
    }
}

std::vector<ChargingProfile> SmartChargingHandler::get_valid_profiles(const ocpp::DateTime& start_time,
                                                                      const ocpp::DateTime& end_time,
                                                                      const int connector_id) {
    std::vector<ChargingProfile> valid_profiles;

    {
        std::lock_guard<std::mutex> lk(this->charge_point_max_profiles_map_mutex);

        for (const auto& [stack_level, profile] : this->stack_level_charge_point_max_profiles_map) {
            if (overlap(start_time, end_time, profile)) {
                valid_profiles.push_back(profile);
            }
        }
    }

    if (connector_id > 0 and this->connectors.at(connector_id)->transaction != nullptr) {
        std::lock_guard<std::mutex> lk_txd(this->tx_default_profiles_map_mutex);
        std::lock_guard<std::mutex> lk_tx(this->tx_profiles_map_mutex);
        for (const auto& [stack_level, profile] : this->connectors.at(connector_id)->stack_level_tx_profiles_map) {
            if (overlap(start_time, end_time, profile)) {
                valid_profiles.push_back(profile);
            }
        }
        for (const auto& [stack_level, profile] :
             this->connectors.at(connector_id)->stack_level_tx_default_profiles_map) {
            if (overlap(start_time, end_time, profile)) {
                valid_profiles.push_back(profile);
            }
        }
    }

    return valid_profiles;
}

std::optional<ocpp::DateTime> SmartChargingHandler::get_profile_start_time(const ChargingProfile& profile,
                                                                           const ocpp::DateTime& time,
                                                                           const int connector_id) {

    const auto schedule = profile.chargingSchedule;
    std::optional<ocpp::DateTime> period_start_time;
    if (profile.chargingProfileKind == ChargingProfileKindType::Absolute) {
        if (schedule.startSchedule) {
            period_start_time.emplace(ocpp::DateTime(floor<seconds>(schedule.startSchedule.value().to_time_point())));
        } else {
            EVLOG_warning << "Absolute profile with no startSchedule, this should not be possible";
        }
    } else if (profile.chargingProfileKind == ChargingProfileKindType::Relative) {
        if (this->connectors.at(connector_id)->transaction != nullptr) {
            period_start_time.emplace(ocpp::DateTime(floor<seconds>(
                this->connectors.at(connector_id)->transaction->get_start_energy_wh()->timestamp.to_time_point())));
        }
    } else if (profile.chargingProfileKind == ChargingProfileKindType::Recurring) {
        const auto start_schedule = ocpp::DateTime(floor<seconds>(schedule.startSchedule.value().to_time_point()));
        const auto recurrency_kind = profile.recurrencyKind.value();
        int seconds_to_go_back;
        if (recurrency_kind == RecurrencyKindType::Daily) {
            seconds_to_go_back = duration_cast<seconds>(time.to_time_point() - start_schedule.to_time_point()).count() %
                                 (HOURS_PER_DAY * SECONDS_PER_HOUR);
        } else {
            seconds_to_go_back = duration_cast<seconds>(time.to_time_point() - start_schedule.to_time_point()).count() %
                                 (SECONDS_PER_DAY * DAYS_PER_WEEK);
        }
        period_start_time.emplace(ocpp::DateTime(time.to_time_point() - seconds(static_cast<int>(seconds_to_go_back))));
    }
    return period_start_time;
}

ocpp::DateTime SmartChargingHandler::get_next_temp_time(const ocpp::DateTime temp_time,
                                                        const std::vector<ChargingProfile>& valid_profiles,
                                                        const int connector_id) {
    auto lowest_next_time = ocpp::DateTime(date::utc_clock::now() + hours(std::numeric_limits<int>::max()));
    for (const auto& profile : valid_profiles) {
        const auto schedule = profile.chargingSchedule;
        const auto periods = schedule.chargingSchedulePeriod;
        const auto period_start_time_opt = this->get_profile_start_time(profile, temp_time, connector_id);
        if (period_start_time_opt) {
            auto period_start_time = period_start_time_opt.value();
            for (size_t i = 0; i < periods.size(); i++) {
                auto period_end_time = get_period_end_time(i, period_start_time, schedule, periods);
                if (temp_time >= period_start_time && temp_time < period_end_time &&
                    period_end_time < lowest_next_time) {
                    lowest_next_time = period_end_time;
                }
                period_start_time = period_end_time;
            }
        }
    }
    return lowest_next_time;
}

} // namespace v16
} // namespace ocpp
