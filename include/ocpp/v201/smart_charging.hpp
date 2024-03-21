// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V201_SMART_CHARGING_HPP
#define OCPP_V201_SMART_CHARGING_HPP

#include "ocpp/v201/enums.hpp"
#include <limits>

#include <ocpp/v201/database_handler.hpp>
#include <ocpp/v201/evse.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/transaction.hpp>

namespace ocpp::v201 {

enum class ProfileValidationResultEnum {
    Valid,
    EvseDoesNotExist,
    TxProfileMissingTransactionId,
    TxProfileEvseIdNotGreaterThanZero,
    TxProfileTransactionNotOnEvse,
    TxProfileEvseHasNoActiveTransaction,
    TxProfileConflictingStackLevel,
    ChargingProfileNoChargingSchedulePeriods,
    ChargingProfileFirstStartScheduleIsNotZero,
    ChargingProfileMissingRequiredStartSchedule,
    ChargingProfileExtraneousStartSchedule,
    ChargingSchedulePeriodsOutOfOrder,
    ChargingSchedulePeriodInvalidPhaseToUse,
};

/// \brief This class handles and maintains incoming ChargingProfiles and contains the logic
/// to calculate the composite schedules
class SmartChargingHandler {
private:
    std::map<int32_t, std::unique_ptr<Evse>>& evses;

    std::shared_ptr<ocpp::v201::DatabaseHandler> database_handler;
    // cppcheck-suppress unusedStructMember
    std::vector<ChargingProfile> charging_profiles;

public:
    explicit SmartChargingHandler(std::map<int32_t, std::unique_ptr<Evse>>& evses);

    ///
    /// \brief validates the existence of the given \p evse_id according to the specification
    ///
    ProfileValidationResultEnum validate_evse_exists(int32_t evse_id) const;

    ///
    /// \brief validates the given \p profile according to the specification
    ///
    ProfileValidationResultEnum validate_tx_profile(const ChargingProfile& profile, Evse& evse) const;

    /// \brief validates that the given \p profile has valid charging schedules
    ProfileValidationResultEnum validate_profile_schedules(const ChargingProfile& profile) const;

    /// \brief Adds a given \p profile to our stored list of profiles
    void add_profile(const ChargingProfile& profile);
};

} // namespace ocpp::v201

#endif // OCPP_V201_SMART_CHARGING_HPP
