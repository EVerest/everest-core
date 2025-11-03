// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/message_handler.hpp>

#include <ocpp/v2/evse.hpp>

namespace ocpp::v2 {
struct FunctionalBlockContext;
class SmartChargingHandlerInterface;

typedef std::function<RequestStartStopStatusEnum(const int32_t evse_id, const ReasonEnum& stop_reason)>
    StopTransactionCallback;

struct LimitsSetpointsForOperationMode;

struct GetChargingProfilesRequest;
struct SetChargingProfileRequest;
struct SetChargingProfileResponse;
struct GetCompositeScheduleResponse;
struct GetCompositeScheduleRequest;
struct ClearChargingProfileResponse;
struct ClearChargingProfileRequest;
struct ReportChargingProfilesRequest;
struct NotifyEVChargingNeedsRequest;
struct NotifyEVChargingNeedsResponse;

/// \brief Different types of limits and setpoints, used in the limits_setpoints_per_operation_mode map.
enum LimitSetpointType {
    Limit,
    DischargeLimit,
    Setpoint,
    SetpointReactive
};

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
    ChargingProfileRateLimitExceeded,
    ChargingProfileIdSmallerThanMaxExternalConstraintsId,
    ChargingProfileUnsupportedPurpose,
    ChargingProfileUnsupportedKind,
    ChargingProfileNotDynamic,
    ChargingScheduleChargingRateUnitUnsupported,
    ChargingSchedulePriorityExtranousDuration,
    ChargingScheduleRandomizedDelay,
    ChargingScheduleUnsupportedLocalTime,
    ChargingScheduleUnsupportedRandomizedDelay,
    ChargingScheduleUnsupportedLimitAtSoC,
    ChargingScheduleUnsupportedEvseSleep,
    ChargingSchedulePeriodsOutOfOrder,
    ChargingSchedulePeriodInvalidPhaseToUse,
    ChargingSchedulePeriodUnsupportedNumberPhases,
    ChargingSchedulePeriodExtraneousPhaseValues,
    ChargingSchedulePeriodPhaseToUseACPhaseSwitchingUnsupported,
    ChargingSchedulePeriodPriorityChargingNotChargingOnly,
    ChargingSchedulePeriodUnsupportedOperationMode,
    ChargingSchedulePeriodUnsupportedLimitSetpoint,
    ChargingSchedulePeriodNoPhaseForDC,
    ChargingSchedulePeriodNoFreqWattCurve,
    ChargingSchedulePeriodSignDifference,
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
        CiString<20> charging_limit_source = ChargingLimitSourceEnumStringType::CSO,
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

    /// \brief Initiates a NotifyEvChargingNeeds.req message to the CSMS
    /// \param req the request to send
    virtual void notify_ev_charging_needs_req(const NotifyEVChargingNeedsRequest& req) = 0;
};

class SmartCharging : public SmartChargingInterface {
private: // Members
    const FunctionalBlockContext& context;
    std::function<void()> set_charging_profiles_callback;
    std::map<ChargingProfilePurposeEnum, DateTime> last_charging_profile_update;
    StopTransactionCallback stop_transaction_callback;

public:
    SmartCharging(const FunctionalBlockContext& functional_block_context,
                  std::function<void()> set_charging_profiles_callback,
                  StopTransactionCallback stop_transaction_callback);
    void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;
    GetCompositeScheduleResponse get_composite_schedule(const GetCompositeScheduleRequest& request) override;
    std::optional<CompositeSchedule> get_composite_schedule(int32_t evse_id, std::chrono::seconds duration,
                                                            ChargingRateUnitEnum unit) override;
    std::vector<CompositeSchedule> get_all_composite_schedules(const int32_t duration,
                                                               const ChargingRateUnitEnum& unit) override;

    void delete_transaction_tx_profiles(const std::string& transaction_id) override;

    SetChargingProfileResponse conform_validate_and_add_profile(
        ChargingProfile& profile, int32_t evse_id,
        CiString<20> charging_limit_source = ChargingLimitSourceEnumStringType::CSO,
        AddChargingProfileSource source_of_request = AddChargingProfileSource::SetChargingProfile) override;
    ProfileValidationResultEnum conform_and_validate_profile(
        ChargingProfile& profile, int32_t evse_id,
        AddChargingProfileSource source_of_request = AddChargingProfileSource::SetChargingProfile) override;
    void notify_ev_charging_needs_req(const NotifyEVChargingNeedsRequest& req) override;

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

    ///
    /// \brief Validates the given profile according to the specification.
    /// \param profile  Profile to validate.
    /// \param evse_id  Evse id this charging profile belongs to.
    /// \return ProfileValidationResultEnum::Valid if valid.
    ///
    ProfileValidationResultEnum validate_priority_charging_profile(const ChargingProfile& profile,
                                                                   int32_t evse_id) const;

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
    SetChargingProfileResponse add_profile(ChargingProfile& profile, int32_t evse_id,
                                           CiString<20> charging_limit_source = ChargingLimitSourceEnumStringType::CSO);

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
    void report_charging_profile_req(const int32_t request_id, const int32_t evse_id, const CiString<20> source,
                                     const std::vector<ChargingProfile>& profiles, const bool tbc);
    void report_charging_profile_req(const ReportChargingProfilesRequest& req);

    /* OCPP message handlers */
    void handle_set_charging_profile_req(Call<SetChargingProfileRequest> call);
    void handle_clear_charging_profile_req(Call<ClearChargingProfileRequest> call);
    void handle_get_charging_profiles_req(Call<GetChargingProfilesRequest> call);
    void handle_get_composite_schedule_req(Call<GetCompositeScheduleRequest> call);
    void handle_notify_ev_charging_needs_response(const EnhancedMessage<MessageType>& call_result);

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
    std::vector<ChargingProfile> get_charging_station_max_profiles() const;
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

    ///
    /// \brief Verify rate limit, only for OCPP 2.1
    ///
    /// When Charging Station receives a SetChargingProfileRequest for a ChargingProfileType with a
    /// chargingProfilePurpose that is to be stored persistently AND the previous SetChargingProfileRequest for this
    /// chargingProfilePurpose was less than ChargingProfileUpdate RateLimit seconds ago, Charging Station MAY respond
    /// with SetChargingProfileResponse with status = Rejected and reasonCode = "RateLimitExceeded" (K01.FR.56).
    ///
    /// \param profile  Charging profile
    /// \return ProfileValidationResultEnum::Valid when rate limit was not exceeded, or OCPP 2.0.1.
    ///         ProfileValidationResultEnum::ChargingProfileRateLimitExceeded when rate limit was exceeded.
    ///
    ProfileValidationResultEnum verify_rate_limit(const ChargingProfile& profile);

    ///
    /// \brief Check if DCInputPhaseControl is enabled for this evse id.
    ///
    /// \note This function can also be used for evse id 0, it will then check all existing evse's for this variable.
    ///
    /// \param evse_id  The evse id. Can also be 0.
    /// \return True if evse has DCInputPhaseControl enabled.
    ///
    bool has_dc_input_phase_control(const int32_t evse_id) const;

    ///
    /// \brief Check if DCInputPhaseControl is enabled for this evse id.
    /// \param evse_id  The evse id. Should not be 0.
    /// \return True if evse has DCInputPhaseControl enabled.
    ///
    bool evse_has_dc_input_phase_control(const int32_t evse_id) const;
};

///
/// \brief Check if limits and checkpoints of an operation mode are correct.
///
/// Check if all required limits and setpoints are set and if there are limits and / or setpoints that should not be
/// there.
///
/// \param limits_setpoints The information about required and optional limits and setpoints.
/// \param limit        Limit or setpoint (for phase 1 if L2 and L3 are set, otherwise for all phases).
/// \param limit_L2     Limit or setpoint phase 2.
/// \param limit_L3     Limit or setpoint phase 3.
/// \return True if all limits and setpoints are set / not set according to limits_setpoints struct.
///
bool are_limits_and_setpoints_of_operation_mode_correct(const LimitsSetpointsForOperationMode& limits_setpoints,
                                                        const ocpp::v2::LimitSetpointType& type,
                                                        const std::optional<float>& limit,
                                                        const std::optional<float>& limit_L2,
                                                        const std::optional<float>& limit_L3);

///
/// \brief Check if operation mode for the charging profile purpose is correct.
///
/// See 2.1 spec: Table 95. operationMode for various ChargingProfilePurposes
///
/// \param operation_mode   The operation mode.
/// \param purpose          The charging profile purpose.
/// \return True if this operation mode is allowed.
///
bool check_operation_modes_for_charging_profile_purposes(const OperationModeEnum& operation_mode,
                                                         const ChargingProfilePurposeEnum& purpose);

///
/// \brief Check if limits and checkpoints of an operation mode are correct.
///
/// Check if all required limits and setpoints are set and if there are limits and / or setpoints that should not be
/// there.
///
/// \param charging_schedule_period The charging schedule period.
/// \return True when all limits and limits are set / not set according to the spec.
///
bool check_limits_and_setpoints(const ChargingSchedulePeriod& charging_schedule_period);

///
/// \brief Check if all setpoint signs are equal (for all phases positive or for all phases negative).
///
/// This check assumes that if setpoint is not set, setpoint_L2 and setpoint_L3 are not set as well.
///
/// \param charging_schedule_period The schedule period to check the setpoints for.
/// \return true if all signs are equal.
///
bool all_setpoints_signs_equal(const ChargingSchedulePeriod& charging_schedule_period);
} // namespace ocpp::v2
