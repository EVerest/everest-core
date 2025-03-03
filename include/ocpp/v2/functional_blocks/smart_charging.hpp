// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/message_handler.hpp>

#include <ocpp/v2/evse.hpp>

namespace ocpp::v2 {
struct FunctionalBlockContext;
class SmartChargingHandlerInterface;

struct GetChargingProfilesRequest;
struct SetChargingProfileRequest;
struct SetChargingProfileResponse;
struct GetCompositeScheduleResponse;
struct GetCompositeScheduleRequest;
struct ClearChargingProfileResponse;
struct ClearChargingProfileRequest;
struct ReportChargingProfilesRequest;

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
    RequestStartTransactionNonTxProfile,
    ChargingProfileEmptyChargingSchedules
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

class SmartChargingInterface : public MessageHandlerInterface {
public:
    virtual ~SmartChargingInterface() = default;

    /// \brief Gets composite schedules for all evse_ids (including 0) for the given \p duration and \p unit . If no
    /// valid profiles are given for an evse for the specified period, the composite schedule will be empty for this
    /// evse.
    /// \param duration of the request from. Composite schedules will be retrieved from now to (now + duration)
    /// \param unit of the period entries of the composite schedules
    /// \return vector of composite schedules, one for each evse_id including 0.
    virtual std::vector<CompositeSchedule> get_all_composite_schedules(const int32_t duration,
                                                                       const ChargingRateUnitEnum& unit) = 0;

    ///
    /// \brief for the given \p transaction_id removes the associated charging profile.
    ///
    virtual void delete_transaction_tx_profiles(const std::string& transaction_id) = 0;

    ///
    /// \brief validates the given \p profile according to the specification,
    /// adding it to our stored list of profiles if valid.
    ///
    virtual SetChargingProfileResponse conform_validate_and_add_profile(
        ChargingProfile& profile, int32_t evse_id,
        ChargingLimitSourceEnum charging_limit_source = ChargingLimitSourceEnum::CSO,
        AddChargingProfileSource source_of_request = AddChargingProfileSource::SetChargingProfile) = 0;

    ///
    /// \brief validates the given \p profile according to the specification.
    /// If a profile does not have validFrom or validTo set, we conform the values
    /// to a representation that fits the spec.
    ///
    virtual ProfileValidationResultEnum conform_and_validate_profile(
        ChargingProfile& profile, int32_t evse_id,
        AddChargingProfileSource source_of_request = AddChargingProfileSource::SetChargingProfile) = 0;

    /// \brief Gets a composite schedule based on the given \p request
    /// \param request specifies different options for the request
    /// \return GetCompositeScheduleResponse containing the status of the operation and the composite schedule if the
    /// operation was successful
    virtual GetCompositeScheduleResponse get_composite_schedule(const GetCompositeScheduleRequest& request) = 0;

    /// \brief Gets a composite schedule based on the given parameters.
    /// \note This will ignore TxDefaultProfiles and TxProfiles if no transaction is active on \p evse_id
    /// \param evse_id Evse to get the schedule for
    /// \param duration How long the schedule should be
    /// \param unit ChargingRateUnit to thet the schedule for
    /// \return the composite schedule if the operation was successful, otherwise nullopt
    virtual std::optional<CompositeSchedule> get_composite_schedule(int32_t evse_id, std::chrono::seconds duration,
                                                                    ChargingRateUnitEnum unit) = 0;
};

class SmartCharging : public SmartChargingInterface {
private: // Members
    const FunctionalBlockContext& context;
    std::function<void()> set_charging_profiles_callback;

public:
    SmartCharging(const FunctionalBlockContext& functional_block_context,
                  std::function<void()> set_charging_profiles_callback);
    void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;
    GetCompositeScheduleResponse get_composite_schedule(const GetCompositeScheduleRequest& request) override;
    std::optional<CompositeSchedule> get_composite_schedule(int32_t evse_id, std::chrono::seconds duration,
                                                            ChargingRateUnitEnum unit) override;
    std::vector<CompositeSchedule> get_all_composite_schedules(const int32_t duration,
                                                               const ChargingRateUnitEnum& unit) override;

    void delete_transaction_tx_profiles(const std::string& transaction_id) override;

    SetChargingProfileResponse conform_validate_and_add_profile(
        ChargingProfile& profile, int32_t evse_id,
        ChargingLimitSourceEnum charging_limit_source = ChargingLimitSourceEnum::CSO,
        AddChargingProfileSource source_of_request = AddChargingProfileSource::SetChargingProfile) override;
    ProfileValidationResultEnum conform_and_validate_profile(
        ChargingProfile& profile, int32_t evse_id,
        AddChargingProfileSource source_of_request = AddChargingProfileSource::SetChargingProfile) override;

protected:
    ///
    /// \brief Calculates the composite schedule for the given \p valid_profiles and the given \p connector_id
    ///
    CompositeSchedule calculate_composite_schedule(const ocpp::DateTime& start_time, const ocpp::DateTime& end_time,
                                                   const int32_t evse_id, ChargingRateUnitEnum charging_rate_unit,
                                                   bool is_offline, bool simulate_transaction_active);

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

    ///
    /// \brief Checks a given \p profile does not have an id that conflicts with an existing profile
    /// of type ChargingStationExternalConstraints
    ///
    ProfileValidationResultEnum verify_no_conflicting_external_constraints_id(const ChargingProfile& profile) const;

    ///
    /// \brief Adds a given \p profile and associated \p evse_id to our stored list of profiles
    ///
    SetChargingProfileResponse
    add_profile(ChargingProfile& profile, int32_t evse_id,
                ChargingLimitSourceEnum charging_limit_source = ChargingLimitSourceEnum::CSO);

    ///
    /// \brief Clears profiles from the system using the given \p request
    ///
    ClearChargingProfileResponse clear_profiles(const ClearChargingProfileRequest& request);

    ///
    /// \brief Gets the charging profiles for the given \p request
    ///
    std::vector<ReportedChargingProfile> get_reported_profiles(const GetChargingProfilesRequest& request) const;

    /// \brief Retrieves all profiles that should be considered for calculating the composite schedule. Only profiles
    /// that belong to the given \p evse_id and that are not contained in \p purposes_to_ignore are included in the
    /// response.
    ///
    std::vector<ChargingProfile>
    get_valid_profiles(int32_t evse_id, const std::vector<ChargingProfilePurposeEnum>& purposes_to_ignore = {});

private: // Functions
    /* OCPP message requests */
    void report_charging_profile_req(const int32_t request_id, const int32_t evse_id,
                                     const ChargingLimitSourceEnum source, const std::vector<ChargingProfile>& profiles,
                                     const bool tbc);
    void report_charging_profile_req(const ReportChargingProfilesRequest& req);

    /* OCPP message handlers */
    void handle_set_charging_profile_req(Call<SetChargingProfileRequest> call);
    void handle_clear_charging_profile_req(Call<ClearChargingProfileRequest> call);
    void handle_get_charging_profiles_req(Call<GetChargingProfilesRequest> call);
    void handle_get_composite_schedule_req(Call<GetCompositeScheduleRequest> call);

    GetCompositeScheduleResponse get_composite_schedule_internal(const GetCompositeScheduleRequest& request,
                                                                 bool simulate_transaction_active = true);

    /// \brief validates that the given \p profile from a RequestStartTransactionRequest is of the correct type
    /// TxProfile
    ProfileValidationResultEnum validate_request_start_transaction_profile(const ChargingProfile& profile) const;

    ///
    /// \brief Checks a given \p candidate_profile and associated \p evse_id validFrom and validTo range
    /// This method assumes that the existing candidate_profile will have dates set for validFrom and validTo
    ///
    bool is_overlapping_validity_period(const ChargingProfile& candidate_profile, int32_t candidate_evse_id) const;

    std::vector<ChargingProfile> get_evse_specific_tx_default_profiles() const;
    std::vector<ChargingProfile> get_station_wide_tx_default_profiles() const;
    std::vector<ChargingProfile>
    get_valid_profiles_for_evse(int32_t evse_id,
                                const std::vector<ChargingProfilePurposeEnum>& purposes_to_ignore = {});
    /// \brief sets attributes of the given \p charging_schedule_period according to the specification.
    /// 2.11. ChargingSchedulePeriodType if absent numberPhases set to 3
    void conform_schedule_number_phases(int32_t profile_id, ChargingSchedulePeriod& charging_schedule_period) const;
    ///
    /// \brief sets attributes of the given \p profile according to the specification.
    /// 2.10. ChargingProfileType validFrom if absent set to current date
    /// 2.10. ChargingProfileType validTo if absent set to max date
    ///
    void conform_validity_periods(ChargingProfile& profile) const;
    CurrentPhaseType get_current_phase_type(const std::optional<EvseInterface*> evse_opt) const;
};
} // namespace ocpp::v2
