// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include "ocpp/common/constants.hpp"
#include "ocpp/common/types.hpp"
#include "ocpp/v16/ocpp_enums.hpp"
#include <ocpp/v16/profile.hpp>
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

SmartChargingHandler::SmartChargingHandler(std::map<int32_t, std::shared_ptr<Connector>>& connectors,
                                           std::shared_ptr<DatabaseHandler> database_handler,
                                           ChargePointConfiguration& configuration) :
    connectors(connectors), database_handler(database_handler), configuration(configuration) {
    this->clear_profiles_timer = std::make_unique<Everest::SteadyTimer>();
    this->clear_profiles_timer->interval([this]() { this->clear_expired_profiles(); }, hours(HOURS_PER_DAY));
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
    const std::vector<ChargingProfile>& valid_profiles, const ocpp::DateTime& start_time,
    const ocpp::DateTime& end_time, const int connector_id, std::optional<ChargingRateUnit> charging_rate_unit) {
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
    const std::vector<ChargingProfile>& valid_profiles, const ocpp::DateTime& start_time,
    const ocpp::DateTime& end_time, const int connector_id, std::optional<ChargingRateUnit> charging_rate_unit) {

    std::optional<ocpp::DateTime> session_start{};

    if (const auto& itt = connectors.find(connector_id); itt != connectors.end()) {
        // connector exists!
        if (itt->second->transaction) {
            session_start.emplace(ocpp::DateTime(
                floor<seconds>(itt->second->transaction->get_start_energy_wh()->timestamp.to_time_point())));
        }
    }

    std::vector<period_entry_t> charge_point_max{};
    std::vector<period_entry_t> tx_default{};
    std::vector<period_entry_t> tx{};

    for (const auto& profile : valid_profiles) {
        std::vector<period_entry_t> periods{};
        periods = ocpp::v16::calculate_profile(start_time, end_time, session_start, profile);

        switch (profile.chargingProfilePurpose) {
        case ChargingProfilePurposeType::ChargePointMaxProfile:
            charge_point_max.insert(charge_point_max.end(), periods.begin(), periods.end());
            break;
        case ChargingProfilePurposeType::TxDefaultProfile:
            tx_default.insert(tx_default.end(), periods.begin(), periods.end());
            break;
        case ChargingProfilePurposeType::TxProfile:
            tx.insert(tx.end(), periods.begin(), periods.end());
            break;
        default:
            break;
        }
    }

    const auto default_amps_limit =
        this->configuration.getCompositeScheduleDefaultLimitAmps().value_or(DEFAULT_LIMIT_AMPS);
    const auto default_watts_limit =
        this->configuration.getCompositeScheduleDefaultLimitWatts().value_or(DEFAULT_LIMIT_WATTS);
    const auto default_number_phases =
        this->configuration.getCompositeScheduleDefaultNumberPhases().value_or(DEFAULT_AND_MAX_NUMBER_PHASES);
    const auto supply_voltage = this->configuration.getSupplyVoltage().value_or(LOW_VOLTAGE);

    CompositeScheduleDefaultLimits default_limits = {default_amps_limit, default_watts_limit, default_number_phases};

    auto composite_charge_point_max = ocpp::v16::calculate_composite_schedule(
        charge_point_max, start_time, end_time, charging_rate_unit, default_number_phases, supply_voltage);
    auto composite_tx_default = ocpp::v16::calculate_composite_schedule(
        tx_default, start_time, end_time, charging_rate_unit, default_number_phases, supply_voltage);
    auto composite_tx = ocpp::v16::calculate_composite_schedule(tx, start_time, end_time, charging_rate_unit,
                                                                default_number_phases, supply_voltage);
    return ocpp::v16::calculate_composite_schedule(composite_charge_point_max, composite_tx_default, composite_tx,
                                                   default_limits, supply_voltage);
}

bool SmartChargingHandler::validate_profile(
    ChargingProfile& profile, const int connector_id, bool ignore_no_transaction, const int profile_max_stack_level,
    const int max_charging_profiles_installed, const int charging_schedule_max_periods,
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units) {
    if ((size_t)connector_id >= this->connectors.size() or connector_id < 0 or profile.stackLevel < 0 or
        profile.stackLevel > profile_max_stack_level) {
        EVLOG_warning << "INVALID PROFILE - connector_id invalid or invalid stack level";
        return false;
    }

    if (profile.chargingProfileKind == ChargingProfileKindType::Absolute && !profile.chargingSchedule.startSchedule) {
        EVLOG_warning << "INVALID PROFILE - Absolute Profile Kind with no given startSchedule";
        if (this->configuration.getAllowChargingProfileWithoutStartSchedule().value_or(false)) {
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
            if (this->configuration.getAllowChargingProfileWithoutStartSchedule().value_or(false)) {
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

std::vector<ChargingProfile>
SmartChargingHandler::get_valid_profiles(const ocpp::DateTime& start_time, const ocpp::DateTime& end_time,
                                         const int connector_id,
                                         const std::set<ChargingProfilePurposeType>& purposes_to_ignore) {
    std::vector<ChargingProfile> valid_profiles;

    {
        std::lock_guard<std::mutex> lk(charge_point_max_profiles_map_mutex);

        if (std::find(std::begin(purposes_to_ignore), std::end(purposes_to_ignore),
                      ChargingProfilePurposeType::ChargePointMaxProfile) == std::end(purposes_to_ignore)) {
            for (const auto& [stack_level, profile] : stack_level_charge_point_max_profiles_map) {
                valid_profiles.push_back(profile);
            }
        }
    }

    if (connector_id > 0) {
        // tx_default profiles added to connector 0 are copied to every connector

        std::optional<int32_t> transactionId;
        const auto& itt = connectors.find(connector_id);
        if (itt != connectors.end()) {
            // connector exists!

            if (itt->second->transaction) {
                transactionId = itt->second->transaction->get_transaction_id();
            }

            std::lock_guard<std::mutex> lk_txd(tx_default_profiles_map_mutex);
            std::lock_guard<std::mutex> lk_tx(tx_profiles_map_mutex);

            if (std::find(std::begin(purposes_to_ignore), std::end(purposes_to_ignore),
                          ChargingProfilePurposeType::TxProfile) == std::end(purposes_to_ignore)) {
                for (const auto& [stack_level, profile] : itt->second->stack_level_tx_profiles_map) {
                    // only include profiles that match the transactionId (when there is one)
                    bool b_add{false};

                    if (profile.transactionId) {
                        if ((transactionId) && (transactionId.value() == profile.transactionId.value())) {
                            // there is a session/transaction in progress and the ID matches the profile
                            b_add = true;
                        }
                    } else {
                        // profile doesn't have a transaction ID specified
                        b_add = true;
                    }

                    if (b_add) {
                        valid_profiles.push_back(profile);
                    }
                }
            }
            if (std::find(std::begin(purposes_to_ignore), std::end(purposes_to_ignore),
                          ChargingProfilePurposeType::TxDefaultProfile) == std::end(purposes_to_ignore)) {
                for (const auto& [stack_level, profile] : itt->second->stack_level_tx_default_profiles_map) {
                    valid_profiles.push_back(profile);
                }
            }
        }
    }

    return valid_profiles;
}

} // namespace v16
} // namespace ocpp
