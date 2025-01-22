// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V201_SMART_CHARGING_HPP
#define OCPP_V201_SMART_CHARGING_HPP

#include <limits>
#include <memory>

#include <ocpp/v201/database_handler.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/evse_manager.hpp>
#include <ocpp/v201/messages/ClearChargingProfile.hpp>
#include <ocpp/v201/messages/GetChargingProfiles.hpp>
#include <ocpp/v201/messages/SetChargingProfile.hpp>
#include <ocpp/v201/ocpp_enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/transaction.hpp>
#include <ocpp/v201/types.hpp>

namespace ocpp::v201 {

const int DEFAULT_AND_MAX_NUMBER_PHASES = 3;

enum class ProfileValidationResultEnum {
    Valid,
    EvseDoesNotExist,
    ExistingChargingStationExternalConstraints,
    InvalidProfileType,
    TxProfileMissingTransactionId,
    TxProfileEvseIdNotGreaterThanZero,
    TxProfileTransactionNotOnEvse,
    TxProfileEvseHasNoActiveTransaction,
    TxProfileConflictingStackLevel,
    ChargingProfileNoChargingSchedulePeriods,
    ChargingProfileFirstStartScheduleIsNotZero,
    ChargingProfileMissingRequiredStartSchedule,
    ChargingProfileExtraneousStartSchedule,
    ChargingScheduleChargingRateUnitUnsupported,
    ChargingSchedulePeriodsOutOfOrder,
    ChargingSchedulePeriodInvalidPhaseToUse,
    ChargingSchedulePeriodUnsupportedNumberPhases,
    ChargingSchedulePeriodExtraneousPhaseValues,
    ChargingSchedulePeriodPhaseToUseACPhaseSwitchingUnsupported,
    ChargingStationMaxProfileCannotBeRelative,
    ChargingStationMaxProfileEvseIdGreaterThanZero,
    DuplicateTxDefaultProfileFound,
    DuplicateProfileValidityPeriod,
    RequestStartTransactionNonTxProfile
};

/// \brief This is used to associate charging profiles with a source.
/// Based on the source a different validation path can be taken.
enum class AddChargingProfileSource {
    SetChargingProfile,
    RequestStartTransactionRequest
};

namespace conversions {
/// \brief Converts the given ProfileValidationResultEnum \p e to human readable string
/// \returns a string representation of the ProfileValidationResultEnum
std::string profile_validation_result_to_string(ProfileValidationResultEnum e);

/// \brief Converts the given ProfileValidationResultEnum \p e to a OCPP reasonCode.
/// \returns a reasonCode
std::string profile_validation_result_to_reason_code(ProfileValidationResultEnum e);
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const ProfileValidationResultEnum validation_result);

class SmartChargingHandlerInterface {
public:
    virtual ~SmartChargingHandlerInterface() = default;

    virtual SetChargingProfileResponse conform_validate_and_add_profile(
        ChargingProfile& profile, int32_t evse_id,
        ChargingLimitSourceEnum charging_limit_source = ChargingLimitSourceEnum::CSO,
        AddChargingProfileSource source_of_request = AddChargingProfileSource::SetChargingProfile) = 0;

    virtual void delete_transaction_tx_profiles(const std::string& transaction_id) = 0;

    virtual ProfileValidationResultEnum conform_and_validate_profile(
        ChargingProfile& profile, int32_t evse_id,
        AddChargingProfileSource source_of_request = AddChargingProfileSource::SetChargingProfile) = 0;

    virtual SetChargingProfileResponse
    add_profile(ChargingProfile& profile, int32_t evse_id,
                ChargingLimitSourceEnum charging_limit_source = ChargingLimitSourceEnum::CSO) = 0;

    virtual ClearChargingProfileResponse clear_profiles(const ClearChargingProfileRequest& request) = 0;

    virtual std::vector<ReportedChargingProfile>
    get_reported_profiles(const GetChargingProfilesRequest& request) const = 0;

    virtual CompositeSchedule calculate_composite_schedule(const ocpp::DateTime& start_time,
                                                           const ocpp::DateTime& end_time, const int32_t evse_id,
                                                           std::optional<ChargingRateUnitEnum> charging_rate_unit,
                                                           bool is_offline, bool simulate_transaction_active) = 0;
};

/// \brief This class handles and maintains incoming ChargingProfiles and contains the logic
/// to calculate the composite schedules
class SmartChargingHandler : public SmartChargingHandlerInterface {
private:
    EvseManagerInterface& evse_manager;
    std::shared_ptr<DeviceModel>& device_model;

    ocpp::v201::DatabaseHandlerInterface& database_handler;

public:
    SmartChargingHandler(EvseManagerInterface& evse_manager, std::shared_ptr<DeviceModel>& device_model,
                         ocpp::v201::DatabaseHandlerInterface& database_handler);

    ///
    /// \brief for the given \p transaction_id removes the associated charging profile.
    ///
    void delete_transaction_tx_profiles(const std::string& transaction_id) override;

    ///
    /// \brief validates the given \p profile according to the specification,
    /// adding it to our stored list of profiles if valid.
    ///
    SetChargingProfileResponse conform_validate_and_add_profile(
        ChargingProfile& profile, int32_t evse_id,
        ChargingLimitSourceEnum charging_limit_source = ChargingLimitSourceEnum::CSO,
        AddChargingProfileSource source_of_request = AddChargingProfileSource::SetChargingProfile) override;

    ///
    /// \brief validates the given \p profile according to the specification.
    /// If a profile does not have validFrom or validTo set, we conform the values
    /// to a representation that fits the spec.
    ///
    ProfileValidationResultEnum conform_and_validate_profile(
        ChargingProfile& profile, int32_t evse_id,
        AddChargingProfileSource source_of_request = AddChargingProfileSource::SetChargingProfile) override;

    ///
    /// \brief Adds a given \p profile and associated \p evse_id to our stored list of profiles
    ///
    SetChargingProfileResponse
    add_profile(ChargingProfile& profile, int32_t evse_id,
                ChargingLimitSourceEnum charging_limit_source = ChargingLimitSourceEnum::CSO) override;

    ///
    /// \brief Clears profiles from the system using the given \p request
    ///
    ClearChargingProfileResponse clear_profiles(const ClearChargingProfileRequest& request) override;

    ///
    /// \brief Gets the charging profiles for the given \p request
    ///
    std::vector<ReportedChargingProfile>
    get_reported_profiles(const GetChargingProfilesRequest& request) const override;

    ///
    /// \brief Calculates the composite schedule for the given \p valid_profiles and the given \p connector_id
    ///
    CompositeSchedule calculate_composite_schedule(const ocpp::DateTime& start_time, const ocpp::DateTime& end_time,
                                                   const int32_t evse_id,
                                                   std::optional<ChargingRateUnitEnum> charging_rate_unit,
                                                   bool is_offline, bool simulate_transaction_active) override;

protected:
    ///
    /// \brief validates the existence of the given \p evse_id according to the specification
    ///
    ProfileValidationResultEnum validate_evse_exists(int32_t evse_id) const;

    ///
    /// \brief validates requirements that apply only to the ChargingStationMaxProfile \p profile
    /// according to the specification
    ///
    ProfileValidationResultEnum validate_charging_station_max_profile(const ChargingProfile& profile,
                                                                      int32_t evse_id) const;

    ///
    /// \brief validates the given \p profile and associated \p evse_id according to the specification
    ///
    ProfileValidationResultEnum validate_tx_default_profile(const ChargingProfile& profile, int32_t evse_id) const;

    ///
    /// \brief validates the given \p profile according to the specification
    ///
    ProfileValidationResultEnum validate_tx_profile(
        const ChargingProfile& profile, int32_t evse_id,
        AddChargingProfileSource source_of_request = AddChargingProfileSource::SetChargingProfile) const;

    /// \brief validates that the given \p profile has valid charging schedules.
    /// If a profiles charging schedule period does not have a valid numberPhases,
    /// we set it to the default value (3).
    ProfileValidationResultEnum validate_profile_schedules(ChargingProfile& profile,
                                                           std::optional<EvseInterface*> evse_opt = std::nullopt) const;

    /// \brief validates that the given \p profile from a RequestStartTransactionRequest is of the correct type
    /// TxProfile
    ProfileValidationResultEnum validate_request_start_transaction_profile(const ChargingProfile& profile) const;

    ///
    /// \brief Checks a given \p profile and associated \p evse_id validFrom and validTo range
    /// This method assumes that the existing profile will have dates set for validFrom and validTo
    ///
    bool is_overlapping_validity_period(const ChargingProfile& profile, int32_t evse_id) const;

    ///
    /// \brief Checks a given \p profile does not have an id that conflicts with an existing profile
    /// of type ChargingStationExternalConstraints
    ///
    ProfileValidationResultEnum verify_no_conflicting_external_constraints_id(const ChargingProfile& profile) const;

private:
    std::vector<ChargingProfile> get_evse_specific_tx_default_profiles() const;
    std::vector<ChargingProfile> get_station_wide_tx_default_profiles() const;

    /// \brief Retrieves all profiles that should be considered for calculating the composite schedule. Only profiles
    /// that belong to the given \p evse_id and that are not contained in \p purposes_to_ignore are included in the
    /// response.
    ///
    std::vector<ChargingProfile>
    get_valid_profiles(int32_t evse_id, const std::vector<ChargingProfilePurposeEnum>& purposes_to_ignore = {});
    std::vector<ChargingProfile>
    get_valid_profiles_for_evse(int32_t evse_id,
                                const std::vector<ChargingProfilePurposeEnum>& purposes_to_ignore = {});
    void conform_schedule_number_phases(int32_t profile_id, ChargingSchedulePeriod& charging_schedule_period) const;
    void conform_validity_periods(ChargingProfile& profile) const;
    CurrentPhaseType get_current_phase_type(const std::optional<EvseInterface*> evse_opt) const;
};

} // namespace ocpp::v201

#endif // OCPP_V201_SMART_CHARGING_HPP
