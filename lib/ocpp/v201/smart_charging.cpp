// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include "date/tz.h"
#include "everest/logging.hpp"
#include "ocpp/common/message_queue.hpp"
#include "ocpp/common/types.hpp"
#include "ocpp/v201/ctrlr_component_variables.hpp"
#include "ocpp/v201/device_model.hpp"
#include "ocpp/v201/enums.hpp"
#include "ocpp/v201/evse.hpp"
#include "ocpp/v201/messages/SetChargingProfile.hpp"
#include "ocpp/v201/ocpp_types.hpp"
#include "ocpp/v201/transaction.hpp"
#include <algorithm>
#include <iterator>
#include <ocpp/v201/smart_charging.hpp>
#include <optional>

using namespace std::chrono;

namespace ocpp::v201 {

namespace conversions {
std::string profile_validation_result_to_string(ProfileValidationResultEnum e) {
    switch (e) {
    case ProfileValidationResultEnum::Valid:
        return "Valid";
    case ProfileValidationResultEnum::EvseDoesNotExist:
        return "EvseDoesNotExist";
    case ProfileValidationResultEnum::ExistingChargingStationExternalConstraints:
        return "ExstingChargingStationExternalConstraints";
    case ProfileValidationResultEnum::InvalidProfileType:
        return "InvalidProfileType";
    case ProfileValidationResultEnum::TxProfileMissingTransactionId:
        return "TxProfileMissingTransactionId";
    case ProfileValidationResultEnum::TxProfileEvseIdNotGreaterThanZero:
        return "TxProfileEvseIdNotGreaterThanZero";
    case ProfileValidationResultEnum::TxProfileTransactionNotOnEvse:
        return "TxProfileTransactionNotOnEvse";
    case ProfileValidationResultEnum::TxProfileEvseHasNoActiveTransaction:
        return "TxProfileEvseHasNoActiveTransaction";
    case ProfileValidationResultEnum::TxProfileConflictingStackLevel:
        return "TxProfileConflictingStackLevel";
    case ProfileValidationResultEnum::ChargingProfileNoChargingSchedulePeriods:
        return "ChargingProfileNoChargingSchedulePeriods";
    case ProfileValidationResultEnum::ChargingProfileFirstStartScheduleIsNotZero:
        return "ChargingProfileFirstStartScheduleIsNotZero";
    case ProfileValidationResultEnum::ChargingProfileMissingRequiredStartSchedule:
        return "ChargingProfileMissingRequiredStartSchedule";
    case ProfileValidationResultEnum::ChargingProfileExtraneousStartSchedule:
        return "ChargingProfileExtraneousStartSchedule";
    case ProfileValidationResultEnum::ChargingScheduleChargingRateUnitUnsupported:
        return "ChargingScheduleChargingRateUnitUnsupported";
    case ProfileValidationResultEnum::ChargingSchedulePeriodsOutOfOrder:
        return "ChargingSchedulePeriodsOutOfOrder";
    case ProfileValidationResultEnum::ChargingSchedulePeriodInvalidPhaseToUse:
        return "ChargingSchedulePeriodInvalidPhaseToUse";
    case ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedNumberPhases:
        return "ChargingSchedulePeriodUnsupportedNumberPhases";
    case ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues:
        return "ChargingSchedulePeriodExtraneousPhaseValues";
    case ProfileValidationResultEnum::ChargingSchedulePeriodPhaseToUseACPhaseSwitchingUnsupported:
        return "ChargingSchedulePeriodPhaseToUseACPhaseSwitchingUnsupported";
    case ProfileValidationResultEnum::ChargingStationMaxProfileCannotBeRelative:
        return "ChargingStationMaxProfileCannotBeRelative";
    case ProfileValidationResultEnum::ChargingStationMaxProfileEvseIdGreaterThanZero:
        return "ChargingStationMaxProfileEvseIdGreaterThanZero";
    case ProfileValidationResultEnum::DuplicateTxDefaultProfileFound:
        return "DuplicateTxDefaultProfileFound";
    case ProfileValidationResultEnum::DuplicateProfileValidityPeriod:
        return "DuplicateProfileValidityPeriod";
    }

    throw std::out_of_range("No known string conversion for provided enum of type ProfileValidationResultEnum");
}

std::string profile_validation_result_to_reason_code(ProfileValidationResultEnum e) {
    switch (e) {
    case ProfileValidationResultEnum::Valid:
        return "NoError";
    case ProfileValidationResultEnum::DuplicateProfileValidityPeriod:
    case ProfileValidationResultEnum::DuplicateTxDefaultProfileFound:
    case ProfileValidationResultEnum::ExistingChargingStationExternalConstraints:
        return "DuplicateProfile";
    case ProfileValidationResultEnum::TxProfileTransactionNotOnEvse:
    case ProfileValidationResultEnum::TxProfileEvseHasNoActiveTransaction:
        return "TxNotFound";
    case ProfileValidationResultEnum::TxProfileConflictingStackLevel:
        return "InvalidStackLevel";
    case ProfileValidationResultEnum::ChargingScheduleChargingRateUnitUnsupported:
        return "UnsupportedRateUnit";
    case ProfileValidationResultEnum::ChargingProfileNoChargingSchedulePeriods:
    case ProfileValidationResultEnum::ChargingProfileFirstStartScheduleIsNotZero:
    case ProfileValidationResultEnum::ChargingProfileMissingRequiredStartSchedule:
    case ProfileValidationResultEnum::ChargingProfileExtraneousStartSchedule:
    case ProfileValidationResultEnum::ChargingSchedulePeriodsOutOfOrder:
    case ProfileValidationResultEnum::ChargingSchedulePeriodInvalidPhaseToUse:
    case ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedNumberPhases:
    case ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues:
    case ProfileValidationResultEnum::ChargingSchedulePeriodPhaseToUseACPhaseSwitchingUnsupported:
        return "InvalidSchedule";
    case ProfileValidationResultEnum::TxProfileMissingTransactionId:
        return "MissingParam";
    case ProfileValidationResultEnum::EvseDoesNotExist:
    case ProfileValidationResultEnum::TxProfileEvseIdNotGreaterThanZero:
    case ProfileValidationResultEnum::ChargingStationMaxProfileCannotBeRelative:
    case ProfileValidationResultEnum::ChargingStationMaxProfileEvseIdGreaterThanZero:
        return "InvalidValue";
    case ProfileValidationResultEnum::InvalidProfileType:
        return "InternalError";
    }

    throw std::out_of_range("No applicable reason code for provided enum of type ProfileValidationResultEnum");
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ProfileValidationResultEnum validation_result) {
    os << conversions::profile_validation_result_to_string(validation_result);
    return os;
}

const int32_t STATION_WIDE_ID = 0;

CurrentPhaseType SmartChargingHandler::get_current_phase_type(const std::optional<EvseInterface*> evse_opt) const {
    if (evse_opt.has_value()) {
        return evse_opt.value()->get_current_phase_type();
    }

    auto supply_phases =
        this->device_model->get_value<int32_t>(ControllerComponentVariables::ChargingStationSupplyPhases);
    if (supply_phases == 1 || supply_phases == 3) {
        return CurrentPhaseType::AC;
    } else if (supply_phases == 0) {
        return CurrentPhaseType::DC;
    }

    return CurrentPhaseType::Unknown;
}

SmartChargingHandler::SmartChargingHandler(EvseManagerInterface& evse_manager,
                                           std::shared_ptr<DeviceModel>& device_model,
                                           std::shared_ptr<ocpp::v201::DatabaseHandler> database_handler) :
    evse_manager(evse_manager), device_model(device_model), database_handler(database_handler) {
}

SetChargingProfileResponse SmartChargingHandler::validate_and_add_profile(ChargingProfile& profile, int32_t evse_id) {
    SetChargingProfileResponse response;
    response.status = ChargingProfileStatusEnum::Rejected;

    auto result = this->validate_profile(profile, evse_id);
    if (result == ProfileValidationResultEnum::Valid) {
        response = this->add_profile(profile, evse_id);
    } else {
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = conversions::profile_validation_result_to_reason_code(result);
        response.statusInfo->additionalInfo = conversions::profile_validation_result_to_string(result);
    }

    return response;
}

ProfileValidationResultEnum SmartChargingHandler::validate_profile(ChargingProfile& profile, int32_t evse_id) {
    conform_validity_periods(profile);

    auto result = ProfileValidationResultEnum::Valid;
    if (evse_id != STATION_WIDE_ID) {
        result = this->validate_evse_exists(evse_id);
        if (result != ProfileValidationResultEnum::Valid) {
            return result;
        }
    }

    result = verify_no_conflicting_external_constraints_id(profile);
    if (result != ProfileValidationResultEnum::Valid) {
        return result;
    }

    if (evse_id != STATION_WIDE_ID) {
        auto& evse = evse_manager.get_evse(evse_id);
        result = this->validate_profile_schedules(profile, &evse);
    } else {
        result = this->validate_profile_schedules(profile);
    }
    if (result != ProfileValidationResultEnum::Valid) {
        return result;
    }

    switch (profile.chargingProfilePurpose) {
    case ChargingProfilePurposeEnum::ChargingStationMaxProfile:
        result = this->validate_charging_station_max_profile(profile, evse_id);
        break;
    case ChargingProfilePurposeEnum::TxDefaultProfile:
        result = this->validate_tx_default_profile(profile, evse_id);
        break;
    case ChargingProfilePurposeEnum::TxProfile:
        result = this->validate_tx_profile(profile, evse_id);
        break;
    case ChargingProfilePurposeEnum::ChargingStationExternalConstraints:
        // TODO: How do we check this? We shouldn't set it in
        // `SetChargingProfileRequest`, but that doesn't mean they're always
        // invalid. K01.FR.05 is the only thing that seems relevant.
        result = ProfileValidationResultEnum::Valid;
        break;
    }

    return result;
}

ProfileValidationResultEnum SmartChargingHandler::validate_evse_exists(int32_t evse_id) const {
    return evse_manager.does_evse_exist(evse_id) ? ProfileValidationResultEnum::Valid
                                                 : ProfileValidationResultEnum::EvseDoesNotExist;
}

ProfileValidationResultEnum SmartChargingHandler::validate_charging_station_max_profile(const ChargingProfile& profile,
                                                                                        int32_t evse_id) const {
    if (profile.chargingProfilePurpose != ChargingProfilePurposeEnum::ChargingStationMaxProfile) {
        return ProfileValidationResultEnum::InvalidProfileType;
    }

    if (is_overlapping_validity_period(profile, evse_id)) {
        return ProfileValidationResultEnum::DuplicateProfileValidityPeriod;
    }

    if (evse_id > 0) {
        return ProfileValidationResultEnum::ChargingStationMaxProfileEvseIdGreaterThanZero;
    }

    if (profile.chargingProfileKind == ChargingProfileKindEnum::Relative) {
        return ProfileValidationResultEnum::ChargingStationMaxProfileCannotBeRelative;
    }

    return ProfileValidationResultEnum::Valid;
}

ProfileValidationResultEnum SmartChargingHandler::validate_tx_default_profile(ChargingProfile& profile,
                                                                              int32_t evse_id) const {
    auto profiles = evse_id == 0 ? get_evse_specific_tx_default_profiles() : get_station_wide_tx_default_profiles();

    if (is_overlapping_validity_period(profile, evse_id)) {
        return ProfileValidationResultEnum::DuplicateProfileValidityPeriod;
    }

    for (auto candidate : profiles) {
        if (candidate.stackLevel == profile.stackLevel) {
            if (candidate.id != profile.id) {
                return ProfileValidationResultEnum::DuplicateTxDefaultProfileFound;
            }
        }
    }

    return ProfileValidationResultEnum::Valid;
}

ProfileValidationResultEnum SmartChargingHandler::validate_tx_profile(const ChargingProfile& profile,
                                                                      int32_t evse_id) const {
    if (!profile.transactionId.has_value()) {
        return ProfileValidationResultEnum::TxProfileMissingTransactionId;
    }

    if (evse_id <= 0) {
        return ProfileValidationResultEnum::TxProfileEvseIdNotGreaterThanZero;
    }

    // We don't want to retrieve an EVSE that doesn't exist below this point.
    auto result = this->validate_evse_exists(evse_id);
    if (result != ProfileValidationResultEnum::Valid) {
        return result;
    }

    auto& evse = evse_manager.get_evse(evse_id);
    if (!evse.has_active_transaction()) {
        return ProfileValidationResultEnum::TxProfileEvseHasNoActiveTransaction;
    }

    auto& transaction = evse.get_transaction();
    if (transaction->transactionId != profile.transactionId.value()) {
        return ProfileValidationResultEnum::TxProfileTransactionNotOnEvse;
    }

    auto conflicts_with = [&profile](const std::pair<int32_t, std::vector<ChargingProfile>>& candidate) {
        return std::any_of(candidate.second.begin(), candidate.second.end(),
                           [&profile](const ChargingProfile& candidateProfile) {
                               return candidateProfile.transactionId == profile.transactionId &&
                                      candidateProfile.stackLevel == profile.stackLevel;
                           });
    };

    if (std::any_of(charging_profiles.begin(), charging_profiles.end(), conflicts_with)) {
        return ProfileValidationResultEnum::TxProfileConflictingStackLevel;
    }

    return ProfileValidationResultEnum::Valid;
}

/* TODO: Implement the following functional requirements:
 * - K01.FR.34
 * - K01.FR.43
 * - K01.FR.48
 */

ProfileValidationResultEnum
SmartChargingHandler::validate_profile_schedules(ChargingProfile& profile,
                                                 std::optional<EvseInterface*> evse_opt) const {
    auto charging_station_supply_phases =
        this->device_model->get_value<int32_t>(ControllerComponentVariables::ChargingStationSupplyPhases);

    for (auto& schedule : profile.chargingSchedule) {
        // K01.FR.26; We currently need to do string conversions for this manually because our DeviceModel class does
        // not let us get a vector of ChargingScheduleChargingRateUnits.
        auto supported_charging_rate_units =
            this->device_model->get_value<std::string>(ControllerComponentVariables::ChargingScheduleChargingRateUnit);
        if (supported_charging_rate_units.find(conversions::charging_rate_unit_enum_to_string(
                schedule.chargingRateUnit)) == supported_charging_rate_units.npos) {
            return ProfileValidationResultEnum::ChargingScheduleChargingRateUnitUnsupported;
        }

        // A schedule must have at least one chargingSchedulePeriod
        if (schedule.chargingSchedulePeriod.empty()) {
            return ProfileValidationResultEnum::ChargingProfileNoChargingSchedulePeriods;
        }

        for (auto i = 0; i < schedule.chargingSchedulePeriod.size(); i++) {
            auto& charging_schedule_period = schedule.chargingSchedulePeriod[i];
            // K01.FR.19
            if (charging_schedule_period.numberPhases != 1 && charging_schedule_period.phaseToUse.has_value()) {
                return ProfileValidationResultEnum::ChargingSchedulePeriodInvalidPhaseToUse;
            }

            // K01.FR.20
            if (charging_schedule_period.phaseToUse.has_value() &&
                !device_model->get_optional_value<bool>(ControllerComponentVariables::ACPhaseSwitchingSupported)
                     .value_or(false)) {
                return ProfileValidationResultEnum::ChargingSchedulePeriodPhaseToUseACPhaseSwitchingUnsupported;
            }

            // K01.FR.31
            if (i == 0 && charging_schedule_period.startPeriod != 0) {
                return ProfileValidationResultEnum::ChargingProfileFirstStartScheduleIsNotZero;
            }

            // K01.FR.35
            if (i + 1 < schedule.chargingSchedulePeriod.size()) {
                auto next_charging_schedule_period = schedule.chargingSchedulePeriod[i + 1];
                if (next_charging_schedule_period.startPeriod <= charging_schedule_period.startPeriod) {
                    return ProfileValidationResultEnum::ChargingSchedulePeriodsOutOfOrder;
                }
            }

            auto phase_type = this->get_current_phase_type(evse_opt);
            // K01.FR.44; We reject profiles that provide invalid numberPhases/phaseToUse instead
            // of silently acccepting them.
            if (phase_type == CurrentPhaseType::DC && (charging_schedule_period.numberPhases.has_value() ||
                                                       charging_schedule_period.phaseToUse.has_value())) {
                return ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues;
            }

            if (phase_type == CurrentPhaseType::AC) {
                // K01.FR.45; Once again rejecting invalid values
                if (charging_schedule_period.numberPhases.has_value() &&
                    charging_schedule_period.numberPhases > charging_station_supply_phases) {
                    return ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedNumberPhases;
                }

                // K01.FR.49
                if (!charging_schedule_period.numberPhases.has_value()) {
                    charging_schedule_period.numberPhases.emplace(DEFAULT_AND_MAX_NUMBER_PHASES);
                }
            }
        }

        // K01.FR.40
        if (profile.chargingProfileKind != ChargingProfileKindEnum::Relative && !schedule.startSchedule.has_value()) {
            return ProfileValidationResultEnum::ChargingProfileMissingRequiredStartSchedule;
            // K01.FR.41
        } else if (profile.chargingProfileKind == ChargingProfileKindEnum::Relative &&
                   schedule.startSchedule.has_value()) {
            return ProfileValidationResultEnum::ChargingProfileExtraneousStartSchedule;
        }
    }

    return ProfileValidationResultEnum::Valid;
}

SetChargingProfileResponse SmartChargingHandler::add_profile(ChargingProfile& profile, int32_t evse_id) {
    SetChargingProfileResponse response;
    response.status = ChargingProfileStatusEnum::Accepted;

    // K01.FR05 - replace non-ChargingStationExternalConstraints profiles if id exists.
    try {
        // K01.FR27 - add profiles to database when valid
        this->database_handler->insert_or_update_charging_profile(evse_id, profile);

        auto found_profile = false;
        for (auto& [existing_evse_id, evse_profiles] : charging_profiles) {
            for (auto it = evse_profiles.begin(); it != evse_profiles.end(); it++) {
                if (profile.id == it->id) {
                    evse_profiles.erase(it);
                    found_profile = true;
                    break;
                }
            }

            if (found_profile) {
                break;
            }
        }
        charging_profiles[evse_id].push_back(profile);

    } catch (const QueryExecutionException& e) {
        EVLOG_error << "Could not store ChargingProfile in the database: " << e.what();
        response.status = ChargingProfileStatusEnum::Rejected;
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = "InternalError";
    }

    return response;
}

std::vector<ChargingProfile> SmartChargingHandler::get_station_wide_profiles() const {
    std::vector<ChargingProfile> station_wide_profiles;
    if (charging_profiles.count(STATION_WIDE_ID) > 0) {
        station_wide_profiles = charging_profiles.at(STATION_WIDE_ID);
    } else {
        station_wide_profiles = {};
    }

    return station_wide_profiles;
}

std::vector<ChargingProfile> SmartChargingHandler::get_profiles() const {
    std::vector<ChargingProfile> all_profiles;
    for (auto evse_profile_pair : charging_profiles) {
        all_profiles.insert(all_profiles.end(), evse_profile_pair.second.begin(), evse_profile_pair.second.end());
    }
    return all_profiles;
}

std::vector<ChargingProfile> SmartChargingHandler::get_evse_specific_tx_default_profiles() const {
    std::vector<ChargingProfile> evse_specific_tx_default_profiles;

    for (auto& [evse_id, profiles] : charging_profiles) {
        if (evse_id != STATION_WIDE_ID) {
            for (auto profile : profiles) {
                if (profile.chargingProfilePurpose == ChargingProfilePurposeEnum::TxDefaultProfile) {
                    evse_specific_tx_default_profiles.push_back(profile);
                }
            }
        }
    }

    return evse_specific_tx_default_profiles;
}

std::vector<ChargingProfile> SmartChargingHandler::get_station_wide_tx_default_profiles() const {
    std::vector<ChargingProfile> station_wide_tx_default_profiles;
    for (auto profile : this->get_station_wide_profiles()) {
        if (profile.chargingProfilePurpose == ChargingProfilePurposeEnum::TxDefaultProfile) {
            station_wide_tx_default_profiles.push_back(profile);
        }
    }

    return station_wide_tx_default_profiles;
}

bool SmartChargingHandler::is_overlapping_validity_period(const ChargingProfile& candidate_profile,
                                                          int candidate_evse_id) const {

    if (candidate_profile.chargingProfilePurpose == ChargingProfilePurposeEnum::TxProfile) {
        // This only applies to non TxProfile types.
        return false;
    }

    auto conflicts_with = [candidate_evse_id, &candidate_profile](
                              const std::pair<int32_t, std::vector<ChargingProfile>>& existing_profiles) {
        auto existing_evse_id = existing_profiles.first;
        if (existing_evse_id == candidate_evse_id) {
            return std::any_of(existing_profiles.second.begin(), existing_profiles.second.end(),
                               [&candidate_profile](const ChargingProfile& existing_profile) {
                                   if (existing_profile.stackLevel == candidate_profile.stackLevel &&
                                       existing_profile.chargingProfileKind == candidate_profile.chargingProfileKind &&
                                       existing_profile.id != candidate_profile.id) {

                                       return candidate_profile.validFrom <= existing_profile.validTo &&
                                              candidate_profile.validTo >= existing_profile.validFrom; // reject
                                   }
                                   return false;
                               });
        }
        return false;
    };

    return std::any_of(charging_profiles.begin(), charging_profiles.end(), conflicts_with);
}

void SmartChargingHandler::conform_validity_periods(ChargingProfile& profile) const {
    profile.validFrom = profile.validFrom.value_or(ocpp::DateTime());
    profile.validTo = profile.validTo.value_or(ocpp::DateTime(date::utc_clock::time_point::max()));
}

ProfileValidationResultEnum
SmartChargingHandler::verify_no_conflicting_external_constraints_id(const ChargingProfile& profile) const {
    auto result = ProfileValidationResultEnum::Valid;
    for (auto existing_profile : this->get_profiles()) {
        if (existing_profile.id == profile.id &&
            existing_profile.chargingProfilePurpose == ChargingProfilePurposeEnum::ChargingStationExternalConstraints) {
            result = ProfileValidationResultEnum::ExistingChargingStationExternalConstraints;
        }
    }

    return result;
}

} // namespace ocpp::v201
