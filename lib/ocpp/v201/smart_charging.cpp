// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include "everest/logging.hpp"
#include "ocpp/common/types.hpp"
#include "ocpp/v201/enums.hpp"
#include "ocpp/v201/evse.hpp"
#include "ocpp/v201/ocpp_types.hpp"
#include "ocpp/v201/transaction.hpp"
#include <iterator>
#include <memory>
#include <ocpp/v201/smart_charging.hpp>

using namespace std::chrono;

namespace ocpp::v201 {

namespace conversions {
std::string profile_validation_result_to_string(ProfileValidationResultEnum e) {
    switch (e) {
    case ProfileValidationResultEnum::Valid:
        return "Valid";
    case ProfileValidationResultEnum::EvseDoesNotExist:
        return "EvseDoesNotExist";
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
    case ProfileValidationResultEnum::ChargingSchedulePeriodsOutOfOrder:
        return "ChargingSchedulePeriodsOutOfOrder";
    case ProfileValidationResultEnum::ChargingSchedulePeriodInvalidPhaseToUse:
        return "ChargingSchedulePeriodInvalidPhaseToUse";
    case ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedNumberPhases:
        return "ChargingSchedulePeriodUnsupportedNumberPhases";
    case ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues:
        return "ChargingSchedulePeriodExtraneousPhaseValues";
    case ProfileValidationResultEnum::DuplicateTxDefaultProfileFound:
        return "DuplicateTxDefaultProfileFound";
    }

    throw std::out_of_range("No known string conversion for provided enum of type ProfileValidationResultEnum");
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ProfileValidationResultEnum validation_result) {
    os << conversions::profile_validation_result_to_string(validation_result);
    return os;
}

const int32_t STATION_WIDE_ID = 0;

SmartChargingHandler::SmartChargingHandler(std::map<int32_t, std::unique_ptr<EvseInterface>>& evses) : evses(evses) {
}

ProfileValidationResultEnum SmartChargingHandler::validate_evse_exists(int32_t evse_id) const {
    return evses.find(evse_id) == evses.end() ? ProfileValidationResultEnum::EvseDoesNotExist
                                              : ProfileValidationResultEnum::Valid;
}

ProfileValidationResultEnum SmartChargingHandler::validate_tx_default_profile(const ChargingProfile& profile,
                                                                              int32_t evse_id) const {
    auto profiles = evse_id == 0 ? get_evse_specific_tx_default_profiles() : get_station_wide_tx_default_profiles();
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
                                                                      EvseInterface& evse) const {
    if (!profile.transactionId.has_value()) {
        return ProfileValidationResultEnum::TxProfileMissingTransactionId;
    }

    int32_t evseId = evse.get_evse_info().id;
    if (evseId <= 0) {
        return ProfileValidationResultEnum::TxProfileEvseIdNotGreaterThanZero;
    }

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
 * - K01.FR.20
 * - K01.FR.34
 * - K01.FR.43
 * - K01.FR.48
 */
ProfileValidationResultEnum
SmartChargingHandler::validate_profile_schedules(ChargingProfile& profile,
                                                 std::optional<EvseInterface*> evse_opt) const {
    for (ChargingSchedule& schedule : profile.chargingSchedule) {
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

            if (evse_opt.has_value()) {
                auto evse = evse_opt.value();
                // K01.FR.44 for EVSEs; We reject profiles that provide invalid numberPhases/phaseToUse instead
                // of silently acccepting them.
                if (evse->get_current_phase_type() == CurrentPhaseType::DC &&
                    (charging_schedule_period.numberPhases.has_value() ||
                     charging_schedule_period.phaseToUse.has_value())) {
                    return ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues;
                }

                if (evse->get_current_phase_type() == CurrentPhaseType::AC) {
                    // K01.FR.45; Once again rejecting invalid values
                    if (charging_schedule_period.numberPhases.has_value() &&
                        charging_schedule_period.numberPhases > DEFAULT_AND_MAX_NUMBER_PHASES) {
                        return ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedNumberPhases;
                    }

                    // K01.FR.49
                    if (!charging_schedule_period.numberPhases.has_value()) {
                        charging_schedule_period.numberPhases.emplace(DEFAULT_AND_MAX_NUMBER_PHASES);
                    }
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

void SmartChargingHandler::add_profile(int32_t evse_id, ChargingProfile& profile) {
    if (STATION_WIDE_ID == evse_id) {
        station_wide_charging_profiles.push_back(profile);
    } else {
        charging_profiles[evse_id].push_back(profile);
    }
}

std::vector<ChargingProfile> SmartChargingHandler::get_evse_specific_tx_default_profiles() const {
    std::vector<ChargingProfile> evse_specific_tx_default_profiles;

    for (auto evse_profile_pair : charging_profiles) {
        for (auto profile : evse_profile_pair.second)
            if (profile.chargingProfilePurpose == ChargingProfilePurposeEnum::TxDefaultProfile) {
                evse_specific_tx_default_profiles.push_back(profile);
            }
    }

    return evse_specific_tx_default_profiles;
}

std::vector<ChargingProfile> SmartChargingHandler::get_station_wide_tx_default_profiles() const {
    std::vector<ChargingProfile> station_wide_tx_default_profiles;
    for (auto profile : station_wide_charging_profiles) {
        if (profile.chargingProfilePurpose == ChargingProfilePurposeEnum::TxDefaultProfile) {
            station_wide_tx_default_profiles.push_back(profile);
        }
    }

    return station_wide_tx_default_profiles;
}

} // namespace ocpp::v201
