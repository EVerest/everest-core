// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/functional_blocks/smart_charging.hpp>

#include <optional>

#include <ocpp/common/constants.hpp>

#include <ocpp/v2/connectivity_manager.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/evse_manager.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/profile.hpp>
#include <ocpp/v2/utils.hpp>

#include <ocpp/v2/messages/ClearChargingProfile.hpp>
#include <ocpp/v2/messages/GetChargingProfiles.hpp>
#include <ocpp/v2/messages/GetCompositeSchedule.hpp>
#include <ocpp/v2/messages/NotifyEVChargingNeeds.hpp>
#include <ocpp/v2/messages/ReportChargingProfiles.hpp>
#include <ocpp/v2/messages/SetChargingProfile.hpp>

const int32_t STATION_WIDE_ID = 0;

using namespace std::chrono;

namespace ocpp::v2 {
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
    case ProfileValidationResultEnum::ChargingProfileRateLimitExceeded:
        return "ChargingProfileRateLimitExceeded";
    case ProfileValidationResultEnum::ChargingProfileIdSmallerThanMaxExternalConstraintsId:
        return "ChargingProfileIdSmallerThanMaxExternalConstraintsId";
    case ProfileValidationResultEnum::ChargingProfileUnsupportedPurpose:
        return "ChargingProfileUnsupportedPurpose";
    case ProfileValidationResultEnum::ChargingProfileUnsupportedKind:
        return "ChargingProfileUnsupportedKind";
    case ProfileValidationResultEnum::ChargingProfileNotDynamic:
        return "ChargingProfileNotDynamic";
    case ProfileValidationResultEnum::ChargingScheduleChargingRateUnitUnsupported:
        return "ChargingScheduleChargingRateUnitUnsupported";
    case ProfileValidationResultEnum::ChargingSchedulePriorityExtranousDuration:
        return "ChargingSchedulePriorityExtranousDuration";
    case ProfileValidationResultEnum::ChargingScheduleRandomizedDelay:
        return "ChargingScheduleRandomizedDelay";
    case ProfileValidationResultEnum::ChargingScheduleUnsupportedLocalTime:
        return "ChargingScheduleUnsupportedLocalTime";
    case ProfileValidationResultEnum::ChargingScheduleUnsupportedRandomizedDelay:
        return "ChargingScheduleUnsupportedRandomizedDelay";
    case ProfileValidationResultEnum::ChargingScheduleUnsupportedLimitAtSoC:
        return "ChargingScheduleUnsupportedLimitAtSoC";
    case ProfileValidationResultEnum::ChargingScheduleUnsupportedEvseSleep:
        return "ChargingScheduleUnsupportedEvseSleep";
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
    case ProfileValidationResultEnum::ChargingSchedulePeriodPriorityChargingNotChargingOnly:
        return "ChargingSchedulePeriodPriorityChargingNotChargingOnly";
    case ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedOperationMode:
        return "ChargingSchedulePeriodUnsupportedOperationMode";
    case ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedLimitSetpoint:
        return "ChargingSchedulePeriodUnsupportedLimitSetpoint";
    case ProfileValidationResultEnum::ChargingSchedulePeriodNoPhaseForDC:
        return "ChargingSchedulePeriodNoPhaseForDC";
    case ProfileValidationResultEnum::ChargingSchedulePeriodNoFreqWattCurve:
        return "ChargingSchedulePeriodNoFreqWattCurve";
    case ocpp::v2::ProfileValidationResultEnum::ChargingSchedulePeriodSignDifference:
        return "ChargingSchedulePeriodSignDifference";
    case ProfileValidationResultEnum::ChargingStationMaxProfileCannotBeRelative:
        return "ChargingStationMaxProfileCannotBeRelative";
    case ProfileValidationResultEnum::ChargingStationMaxProfileEvseIdGreaterThanZero:
        return "ChargingStationMaxProfileEvseIdGreaterThanZero";
    case ProfileValidationResultEnum::DuplicateTxDefaultProfileFound:
        return "DuplicateTxDefaultProfileFound";
    case ProfileValidationResultEnum::DuplicateProfileValidityPeriod:
        return "DuplicateProfileValidityPeriod";
    case ProfileValidationResultEnum::RequestStartTransactionNonTxProfile:
        return "RequestStartTransactionNonTxProfile";
    case ProfileValidationResultEnum::ChargingProfileEmptyChargingSchedules:
        return "ChargingProfileEmptyChargingSchedules";
    }

    throw EnumToStringException{e, "ProfileValidationResultEnum"};
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
    case ProfileValidationResultEnum::ChargingProfileRateLimitExceeded:
        return "RateLimitExceeded";
    case ProfileValidationResultEnum::ChargingProfileIdSmallerThanMaxExternalConstraintsId:
        return "InvalidProfileId";
    case ProfileValidationResultEnum::ChargingProfileUnsupportedPurpose:
        return "UnsupportedPurpose";
    case ProfileValidationResultEnum::ChargingProfileUnsupportedKind:
        return "UnsupportedKind";
    case ProfileValidationResultEnum::ChargingProfileNotDynamic:
        return "InvalidProfile";
    case ProfileValidationResultEnum::ChargingProfileNoChargingSchedulePeriods:
    case ProfileValidationResultEnum::ChargingProfileFirstStartScheduleIsNotZero:
    case ProfileValidationResultEnum::ChargingProfileMissingRequiredStartSchedule:
    case ProfileValidationResultEnum::ChargingProfileExtraneousStartSchedule:
    case ProfileValidationResultEnum::ChargingProfileEmptyChargingSchedules:
    case ProfileValidationResultEnum::ChargingSchedulePriorityExtranousDuration:
    case ProfileValidationResultEnum::ChargingScheduleRandomizedDelay:
    case ProfileValidationResultEnum::ChargingScheduleUnsupportedLocalTime:
    case ProfileValidationResultEnum::ChargingScheduleUnsupportedRandomizedDelay:
    case ProfileValidationResultEnum::ChargingScheduleUnsupportedLimitAtSoC:
    case ProfileValidationResultEnum::ChargingScheduleUnsupportedEvseSleep:
    case ProfileValidationResultEnum::ChargingSchedulePeriodsOutOfOrder:
    case ProfileValidationResultEnum::ChargingSchedulePeriodInvalidPhaseToUse:
    case ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedNumberPhases:
    case ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues:
    case ProfileValidationResultEnum::ChargingSchedulePeriodPhaseToUseACPhaseSwitchingUnsupported:
    case ProfileValidationResultEnum::ChargingSchedulePeriodPriorityChargingNotChargingOnly:
    case ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedOperationMode:
    case ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedLimitSetpoint:
    case ProfileValidationResultEnum::ChargingSchedulePeriodSignDifference:
        return "InvalidSchedule";
    case ProfileValidationResultEnum::ChargingSchedulePeriodNoPhaseForDC:
        return "NoPhaseForDC";
    case ProfileValidationResultEnum::ChargingSchedulePeriodNoFreqWattCurve:
        return "NoFreqWattCurve";
    case ProfileValidationResultEnum::TxProfileMissingTransactionId:
        return "MissingParam";
    case ProfileValidationResultEnum::EvseDoesNotExist:
    case ProfileValidationResultEnum::TxProfileEvseIdNotGreaterThanZero:
    case ProfileValidationResultEnum::ChargingStationMaxProfileCannotBeRelative:
    case ProfileValidationResultEnum::ChargingStationMaxProfileEvseIdGreaterThanZero:
    case ProfileValidationResultEnum::RequestStartTransactionNonTxProfile:
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

/// \brief Table 95 from OCPP 2.1 spec (part 2 specification): operationMode for various ChargingProfilePurposes
/// Those operation modes are allowed for the given charging profile purposes.
const std::map<ChargingProfilePurposeEnum, std::set<OperationModeEnum>> operation_modes_for_charging_profile_purposes{
    {ChargingProfilePurposeEnum::TxProfile,
     {OperationModeEnum::ChargingOnly, OperationModeEnum::CentralSetpoint, OperationModeEnum::ExternalSetpoint,
      OperationModeEnum::ExternalLimits, OperationModeEnum::CentralFrequency, OperationModeEnum::LocalFrequency,
      OperationModeEnum::LocalLoadBalancing, OperationModeEnum::Idle}},
    {ChargingProfilePurposeEnum::TxDefaultProfile,
     {OperationModeEnum::ChargingOnly, OperationModeEnum::CentralSetpoint, OperationModeEnum::ExternalSetpoint,
      OperationModeEnum::ExternalLimits, OperationModeEnum::CentralFrequency, OperationModeEnum::LocalFrequency,
      OperationModeEnum::LocalLoadBalancing, OperationModeEnum::Idle}},
    {ChargingProfilePurposeEnum::PriorityCharging, {OperationModeEnum::ChargingOnly}},
    {ChargingProfilePurposeEnum::ChargingStationMaxProfile, {OperationModeEnum::ChargingOnly}},
    {ChargingProfilePurposeEnum::ChargingStationExternalConstraints,
     {OperationModeEnum::ChargingOnly, OperationModeEnum::ExternalLimits, OperationModeEnum::ExternalSetpoint}},
    {ChargingProfilePurposeEnum::LocalGeneration,
     {OperationModeEnum::ChargingOnly, OperationModeEnum::ExternalLimits}}};

/// \brief Struct to define required and optional limits / setpoints (for an operation mode).
struct LimitsSetpointsForOperationMode {
    std::set<LimitSetpointType> required;
    std::set<LimitSetpointType> optional;
};

/// \brief Map with required and optional limits and setpoints per operation mode, see table
///        'Limits and setpoints per operation mode' in the 2.1 spec.
const std::map<OperationModeEnum, LimitsSetpointsForOperationMode> limits_setpoints_per_operation_mode = {
    {OperationModeEnum::ChargingOnly, {{LimitSetpointType::Limit}, {}}},
    {OperationModeEnum::CentralSetpoint,
     {{LimitSetpointType::Setpoint},
      {LimitSetpointType::Limit, LimitSetpointType::DischargeLimit, LimitSetpointType::SetpointReactive}}},
    {OperationModeEnum::CentralFrequency,
     {{LimitSetpointType::Setpoint}, {LimitSetpointType::Limit, LimitSetpointType::DischargeLimit}}},
    {OperationModeEnum::LocalFrequency, {{}, {}}},
    {OperationModeEnum::ExternalSetpoint, {{}, {LimitSetpointType::Limit, LimitSetpointType::DischargeLimit}}},
    {OperationModeEnum::ExternalLimits, {{}, {}}},
    {OperationModeEnum::LocalLoadBalancing, {{}, {}}},
    {OperationModeEnum::Idle, {{}, {}}}};

SmartCharging::SmartCharging(const FunctionalBlockContext& functional_block_context,
                             std::function<void()> set_charging_profiles_callback,
                             StopTransactionCallback stop_transaction_callback) :
    context(functional_block_context),
    set_charging_profiles_callback(set_charging_profiles_callback),
    stop_transaction_callback(stop_transaction_callback) {
}

void SmartCharging::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    const auto& json_message = message.message;

    if (message.messageType == MessageType::SetChargingProfile) {
        this->handle_set_charging_profile_req(json_message);
    } else if (message.messageType == MessageType::ClearChargingProfile) {
        this->handle_clear_charging_profile_req(json_message);
    } else if (message.messageType == MessageType::GetChargingProfiles) {
        this->handle_get_charging_profiles_req(json_message);
    } else if (message.messageType == MessageType::GetCompositeSchedule) {
        this->handle_get_composite_schedule_req(json_message);
    } else if (message.messageType == MessageType::NotifyEVChargingNeedsResponse) {
        this->handle_notify_ev_charging_needs_response(message);
    } else {
        throw MessageTypeNotImplementedException(message.messageType);
    }
}

GetCompositeScheduleResponse SmartCharging::get_composite_schedule(const GetCompositeScheduleRequest& request) {
    return this->get_composite_schedule_internal(request);
}

std::optional<CompositeSchedule> SmartCharging::get_composite_schedule(int32_t evse_id, std::chrono::seconds duration,
                                                                       ChargingRateUnitEnum unit) {
    GetCompositeScheduleRequest request;
    request.duration = duration.count();
    request.evseId = evse_id;
    request.chargingRateUnit = unit;

    auto composite_schedule_response = this->get_composite_schedule_internal(request, false);
    if (composite_schedule_response.status == GenericStatusEnum::Accepted and
        composite_schedule_response.schedule.has_value()) {
        return composite_schedule_response.schedule.value();
    } else {
        return std::nullopt;
    }
}

ProfileValidationResultEnum SmartCharging::verify_rate_limit(const ChargingProfile& profile) {
    auto result = ProfileValidationResultEnum::Valid;

    // K01.FR.56
    // Currently we store all charging profiles in the database. So here we will check if the previous charging profile
    // was stored long enough ago and if not, return 'RateLimitExceeded'.
    const ComponentVariable update_rate_limit = ControllerComponentVariables::ChargingProfileUpdateRateLimit;
    const std::optional<int> update_rate_limit_seconds =
        this->context.device_model.get_optional_value<int>(update_rate_limit);
    if (this->context.ocpp_version == OcppProtocolVersion::v21 && update_rate_limit_seconds.has_value()) {
        if (last_charging_profile_update.count(profile.chargingProfilePurpose) != 0) {
            const DateTime now = DateTime();
            const std::chrono::seconds seconds_since_previous_profile =
                std::chrono::duration_cast<std::chrono::seconds>(
                    now.to_time_point() - last_charging_profile_update[profile.chargingProfilePurpose].to_time_point());
            if (seconds_since_previous_profile.count() < update_rate_limit_seconds.value()) {
                result = ProfileValidationResultEnum::ChargingProfileRateLimitExceeded;
            }
        }

        last_charging_profile_update[profile.chargingProfilePurpose] = DateTime();
    }

    return result;
}

bool SmartCharging::has_dc_input_phase_control(const int32_t evse_id) const {
    if (evse_id == 0) {
        for (EvseManagerInterface::EvseIterator it = context.evse_manager.begin(); it != context.evse_manager.end();
             ++it) {
            const int32_t id = (*it).get_id();
            if (!evse_has_dc_input_phase_control(id)) {
                return false;
            }
        }

        return true;
    } else {
        return evse_has_dc_input_phase_control(evse_id);
    }
}

bool SmartCharging::evse_has_dc_input_phase_control(const int32_t evse_id) const {
    const ComponentVariable evse_variable =
        EvseComponentVariables::get_component_variable(evse_id, EvseComponentVariables::DCInputPhaseControl);
    return this->context.device_model.get_optional_value<bool>(evse_variable).value_or(false);
}

std::vector<CompositeSchedule> SmartCharging::get_all_composite_schedules(const int32_t duration_s,
                                                                          const ChargingRateUnitEnum& unit) {
    std::vector<CompositeSchedule> composite_schedules;

    const auto number_of_evses = this->context.evse_manager.get_number_of_evses();
    // get all composite schedules including the one for evse_id == 0
    for (int32_t evse_id = 0; evse_id <= number_of_evses; evse_id++) {
        GetCompositeScheduleRequest request;
        request.duration = duration_s;
        request.evseId = evse_id;
        request.chargingRateUnit = unit;
        auto composite_schedule_response = this->get_composite_schedule_internal(request);
        if (composite_schedule_response.status == GenericStatusEnum::Accepted and
            composite_schedule_response.schedule.has_value()) {
            composite_schedules.push_back(composite_schedule_response.schedule.value());
        } else {
            EVLOG_warning << "Could not internally retrieve composite schedule for evse id " << evse_id << ": "
                          << composite_schedule_response;
        }
    }

    return composite_schedules;
}

void SmartCharging::delete_transaction_tx_profiles(const std::string& transaction_id) {
    this->context.database_handler.delete_charging_profile_by_transaction_id(transaction_id);
}

SetChargingProfileResponse SmartCharging::conform_validate_and_add_profile(ChargingProfile& profile, int32_t evse_id,
                                                                           CiString<20> charging_limit_source,
                                                                           AddChargingProfileSource source_of_request) {
    SetChargingProfileResponse response;
    response.status = ChargingProfileStatusEnum::Rejected;

    auto result = this->conform_and_validate_profile(profile, evse_id, source_of_request);

    if (result == ProfileValidationResultEnum::Valid) {
        result = verify_rate_limit(profile);
    }

    if (result == ProfileValidationResultEnum::Valid) {
        response = this->add_profile(profile, evse_id, charging_limit_source);
    } else {
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = conversions::profile_validation_result_to_reason_code(result);
        response.statusInfo->additionalInfo = conversions::profile_validation_result_to_string(result);
    }

    return response;
}

ProfileValidationResultEnum SmartCharging::conform_and_validate_profile(ChargingProfile& profile, int32_t evse_id,
                                                                        AddChargingProfileSource source_of_request) {
    auto result = ProfileValidationResultEnum::Valid;

    if (source_of_request == AddChargingProfileSource::RequestStartTransactionRequest) {
        result = validate_request_start_transaction_profile(profile);
        if (result != ProfileValidationResultEnum::Valid) {
            return result;
        }
    }

    conform_validity_periods(profile);

    if (evse_id != STATION_WIDE_ID) {
        // K01.FR.28: The evse in the charging profile must exist
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
        auto& evse = this->context.evse_manager.get_evse(evse_id);
        result = this->validate_profile_schedules(profile, &evse);
    } else {
        result = this->validate_profile_schedules(profile);
    }

    if (result == ProfileValidationResultEnum::Valid) {
        if (is_overlapping_validity_period(profile, evse_id)) {
            result = ProfileValidationResultEnum::DuplicateProfileValidityPeriod;
        }
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
        result = this->validate_tx_profile(profile, evse_id, source_of_request);
        break;
    case ChargingProfilePurposeEnum::ChargingStationExternalConstraints:
        // TODO: How do we check this? We shouldn't set it in
        // `SetChargingProfileRequest`, but that doesn't mean they're always
        // invalid. K01.FR.05 is the only thing that seems relevant.
        result = ProfileValidationResultEnum::Valid;
        break;
    case ChargingProfilePurposeEnum::PriorityCharging:
        result = this->validate_priority_charging_profile(profile, evse_id);
        break;
    case ChargingProfilePurposeEnum::LocalGeneration:
        // FIXME: handle missing cases
        result = ProfileValidationResultEnum::InvalidProfileType;
        break;
    }

    return result;
}

namespace {
struct CompositeScheduleConfig {
    std::vector<ChargingProfilePurposeEnum> purposes_to_ignore;
    float current_limit{};
    float power_limit{};
    int32_t default_number_phases{};
    float supply_voltage{};

    CompositeScheduleConfig(DeviceModel& device_model, bool is_offline) :
        purposes_to_ignore{utils::get_purposes_to_ignore(
            device_model.get_optional_value<std::string>(ControllerComponentVariables::IgnoredProfilePurposesOffline)
                .value_or(""),
            is_offline)} {

        this->current_limit =
            device_model.get_optional_value<int>(ControllerComponentVariables::CompositeScheduleDefaultLimitAmps)
                .value_or(DEFAULT_LIMIT_AMPS);

        this->power_limit =
            device_model.get_optional_value<int>(ControllerComponentVariables::CompositeScheduleDefaultLimitWatts)
                .value_or(DEFAULT_LIMIT_WATTS);

        this->default_number_phases =
            device_model.get_optional_value<int>(ControllerComponentVariables::CompositeScheduleDefaultNumberPhases)
                .value_or(DEFAULT_AND_MAX_NUMBER_PHASES);

        this->supply_voltage =
            device_model.get_optional_value<int>(ControllerComponentVariables::SupplyVoltage).value_or(LOW_VOLTAGE);
    }
};

std::vector<IntermediateProfile> generate_evse_intermediates(std::vector<ChargingProfile>&& evse_profiles,
                                                             const std::vector<ChargingProfile>& station_wide_profiles,
                                                             const ocpp::DateTime& start_time,
                                                             const ocpp::DateTime& end_time,
                                                             std::optional<ocpp::DateTime> session_start,
                                                             bool simulate_transaction_active) {

    // Combine the profiles with those from the station
    evse_profiles.insert(evse_profiles.end(), station_wide_profiles.begin(), station_wide_profiles.end());

    auto external_constraints_periods =
        calculate_all_profiles(start_time, end_time, session_start, evse_profiles,
                               ChargingProfilePurposeEnum::ChargingStationExternalConstraints);

    std::vector<IntermediateProfile> output;
    output.push_back(generate_profile_from_periods(external_constraints_periods, start_time, end_time));

    // If there is a session active or we want to simulate, add the combined tx and tx_default to the output
    if (session_start.has_value() || simulate_transaction_active) {
        auto tx_default_periods = calculate_all_profiles(start_time, end_time, session_start, evse_profiles,
                                                         ChargingProfilePurposeEnum::TxDefaultProfile);
        auto tx_periods = calculate_all_profiles(start_time, end_time, session_start, evse_profiles,
                                                 ChargingProfilePurposeEnum::TxProfile);

        auto tx_default = generate_profile_from_periods(tx_default_periods, start_time, end_time);
        auto tx = generate_profile_from_periods(tx_periods, start_time, end_time);

        // Merges the TxProfile with the TxDefaultProfile, for every period preferring a tx period over a tx_default
        // period
        output.push_back(merge_tx_profile_with_tx_default_profile(tx, tx_default));
    }

    return output;
}
} // namespace

CompositeSchedule SmartCharging::calculate_composite_schedule(const ocpp::DateTime& start_time,
                                                              const ocpp::DateTime& end_time, const int32_t evse_id,
                                                              ChargingRateUnitEnum charging_rate_unit, bool is_offline,
                                                              bool simulate_transaction_active) {

    const CompositeScheduleConfig config{this->context.device_model, is_offline};

    std::optional<ocpp::DateTime> session_start{};
    if (this->context.evse_manager.does_evse_exist(evse_id) and evse_id != 0 and
        this->context.evse_manager.get_evse(evse_id).get_transaction() != nullptr) {
        const auto& transaction = this->context.evse_manager.get_evse(evse_id).get_transaction();
        session_start = transaction->start_time;
    }

    const auto station_wide_profiles = get_valid_profiles_for_evse(STATION_WIDE_ID, config.purposes_to_ignore);

    std::vector<IntermediateProfile> combined_profiles{};

    if (evse_id == STATION_WIDE_ID) {
        auto nr_of_evses = this->context.evse_manager.get_number_of_evses();

        // Get the ChargingStationExternalConstraints and Combined Tx(Default)Profiles per evse
        std::vector<IntermediateProfile> evse_schedules{};
        for (int evse = 1; evse <= nr_of_evses; evse++) {
            session_start.reset();
            auto& transaction = this->context.evse_manager.get_evse(evse).get_transaction();
            if (this->context.evse_manager.get_evse(evse).get_transaction() != nullptr) {
                session_start = this->context.evse_manager.get_evse(evse).get_transaction()->start_time;
            }

            auto intermediates = generate_evse_intermediates(
                get_valid_profiles_for_evse(evse, config.purposes_to_ignore), station_wide_profiles, start_time,
                end_time, session_start, simulate_transaction_active);

            // Determine the lowest limits per evse
            evse_schedules.push_back(merge_profiles_by_lowest_limit(intermediates, this->context.ocpp_version));
        }

        // Add all the limits of all the evse's together since that will be the max the whole charging station can
        // consume at any point in time
        combined_profiles.push_back(merge_profiles_by_summing_limits(evse_schedules, config.current_limit,
                                                                     config.power_limit, this->context.ocpp_version));

    } else {
        combined_profiles = generate_evse_intermediates(get_valid_profiles_for_evse(evse_id, config.purposes_to_ignore),
                                                        station_wide_profiles, start_time, end_time, session_start,
                                                        simulate_transaction_active);
    }

    // ChargingStationMaxProfile is always station wide
    auto charge_point_max_periods = calculate_all_profiles(start_time, end_time, session_start, station_wide_profiles,
                                                           ChargingProfilePurposeEnum::ChargingStationMaxProfile);
    auto charge_point_max = generate_profile_from_periods(charge_point_max_periods, start_time, end_time);

    // Add the ChargingStationMaxProfile limits to the other profiles
    combined_profiles.push_back(std::move(charge_point_max));

    // Calculate the final limit of all the combined profiles
    auto retval = merge_profiles_by_lowest_limit(combined_profiles, this->context.ocpp_version);

    CompositeSchedule composite{};
    composite.evseId = evse_id;
    composite.scheduleStart = floor_seconds(start_time);
    composite.duration = elapsed_seconds(floor_seconds(end_time), floor_seconds(start_time));
    composite.chargingRateUnit = charging_rate_unit;

    // Convert the intermediate result into a proper schedule. Will fill in the periods with no limits with the default
    // one
    const auto limit = charging_rate_unit == ChargingRateUnitEnum::A ? config.current_limit : config.power_limit;
    composite.chargingSchedulePeriod = convert_intermediate_into_schedule(
        retval, charging_rate_unit, limit, config.default_number_phases, config.supply_voltage);

    return composite;
}

ProfileValidationResultEnum SmartCharging::validate_evse_exists(int32_t evse_id) const {
    return this->context.evse_manager.does_evse_exist(evse_id) ? ProfileValidationResultEnum::Valid
                                                               : ProfileValidationResultEnum::EvseDoesNotExist;
}

ProfileValidationResultEnum SmartCharging::validate_charging_station_max_profile(const ChargingProfile& profile,
                                                                                 int32_t evse_id) const {
    if (profile.chargingProfilePurpose != ChargingProfilePurposeEnum::ChargingStationMaxProfile) {
        return ProfileValidationResultEnum::InvalidProfileType;
    }

    if (evse_id > 0) {
        return ProfileValidationResultEnum::ChargingStationMaxProfileEvseIdGreaterThanZero;
    }

    // K01.FR.38: For ChargingStationMaxProfile, chargingProfileKind shall not be Relative
    if (profile.chargingProfileKind == ChargingProfileKindEnum::Relative) {
        return ProfileValidationResultEnum::ChargingStationMaxProfileCannotBeRelative;
    }

    return ProfileValidationResultEnum::Valid;
}

ProfileValidationResultEnum SmartCharging::validate_tx_default_profile(const ChargingProfile& profile,
                                                                       int32_t evse_id) const {
    auto profiles = evse_id == 0 ? get_evse_specific_tx_default_profiles() : get_station_wide_tx_default_profiles();

    // K01.FR.53
    for (auto candidate : profiles) {
        if (candidate.stackLevel == profile.stackLevel) {
            if (candidate.id != profile.id) {
                return ProfileValidationResultEnum::DuplicateTxDefaultProfileFound;
            }
        }
    }

    return ProfileValidationResultEnum::Valid;
}

// FIXME: See OCPP2.1 spec: 3.8 Avoiding Phase Conflicts
ProfileValidationResultEnum SmartCharging::validate_tx_profile(const ChargingProfile& profile, int32_t evse_id,
                                                               AddChargingProfileSource source_of_request) const {
    // K01.FR.16: TxProfile shall only be used with evseId > 0.
    if (evse_id <= 0) {
        return ProfileValidationResultEnum::TxProfileEvseIdNotGreaterThanZero;
    }

    // We don't want to retrieve an EVSE that doesn't exist below this point.
    auto result = this->validate_evse_exists(evse_id);
    if (result != ProfileValidationResultEnum::Valid) {
        return result;
    }

    // we can return valid here since the following checks verify the transactionId which is not given if the source is
    // RequestStartTransactionRequest
    if (source_of_request == AddChargingProfileSource::RequestStartTransactionRequest) {
        return ProfileValidationResultEnum::Valid;
    }

    if (!profile.transactionId.has_value()) {
        // K01.FR.03: TxProfile must have a transaction id.
        return ProfileValidationResultEnum::TxProfileMissingTransactionId;
    }

    auto& evse = this->context.evse_manager.get_evse(evse_id);
    // K01.FR.09: There must be an active transaction when a TxProfile is received.
    if (!evse.has_active_transaction()) {
        return ProfileValidationResultEnum::TxProfileEvseHasNoActiveTransaction;
    }

    auto& transaction = evse.get_transaction();
    // K01.FR.33: TxProfile and given transactionId is not known: reject.
    if (transaction->transactionId != profile.transactionId.value()) {
        return ProfileValidationResultEnum::TxProfileTransactionNotOnEvse;
    }

    // K01.FR.39: There can not be a stackLevel - transactionId combination that already exists in another
    // ChargingProfile with different id.
    auto conflicts_stmt =
        this->context.database_handler.new_statement("SELECT PROFILE FROM CHARGING_PROFILES WHERE TRANSACTION_ID = "
                                                     "@transaction_id AND STACK_LEVEL = @stack_level AND ID != @id");
    conflicts_stmt->bind_int("@stack_level", profile.stackLevel);
    conflicts_stmt->bind_int("@id", profile.id);
    if (profile.transactionId.has_value()) {
        conflicts_stmt->bind_text("@transaction_id", profile.transactionId.value().get(),
                                  everest::db::sqlite::SQLiteString::Transient);
    } else {
        conflicts_stmt->bind_null("@transaction_id");
    }

    if (conflicts_stmt->step() == SQLITE_ROW) {
        return ProfileValidationResultEnum::TxProfileConflictingStackLevel;
    }

    return ProfileValidationResultEnum::Valid;
}

ProfileValidationResultEnum SmartCharging::validate_priority_charging_profile(const ChargingProfile& profile,
                                                                              int32_t evse_id) const {
    // Charging profile purpose PriorityCharging:
    // A charging profile with purpose PriorityCharging is used to overrule the currently active TxProfile or
    // TxDefaultProfile charging restrictions with a charging profile that provides the maximum possible power under the
    // circumstances, and avoids discharging operations. It has charging schedule periods with operationMode =
    // ChargingOnly and the charging schedule has no duration, since it remains valid until end of the transaction.
    if (profile.chargingProfilePurpose != ChargingProfilePurposeEnum::PriorityCharging) {
        return ProfileValidationResultEnum::InvalidProfileType;
    }

    // K01.FR.70 Priority charging should not have value for duration.
    for (const auto& schedule : profile.chargingSchedule) {
        if (schedule.duration.has_value()) {
            // Priority charging should not have a duration.
            return ProfileValidationResultEnum::ChargingSchedulePriorityExtranousDuration;
        }
    }

    return ProfileValidationResultEnum::Valid;
}

/* TODO: Implement the following functional requirements:
 * - K01.FR.34
 * - K01.FR.43
 * - K01.FR.48
 * - K01.FR.90
 */

ProfileValidationResultEnum SmartCharging::validate_profile_schedules(ChargingProfile& profile,
                                                                      std::optional<EvseInterface*> evse_opt) const {
    if (profile.chargingSchedule.empty()) {
        return ProfileValidationResultEnum::ChargingProfileEmptyChargingSchedules;
    }

    auto charging_station_supply_phases =
        this->context.device_model.get_value<int32_t>(ControllerComponentVariables::ChargingStationSupplyPhases);

    auto phase_type = this->get_current_phase_type(evse_opt);

    for (auto& schedule : profile.chargingSchedule) {
        // K01.FR.26; We currently need to do string conversions for this manually because our DeviceModel class
        // does not let us get a vector of ChargingScheduleChargingRateUnits.
        auto supported_charging_rate_units = this->context.device_model.get_value<std::string>(
            ControllerComponentVariables::ChargingScheduleChargingRateUnit);
        if (supported_charging_rate_units.find(conversions::charging_rate_unit_enum_to_string(
                schedule.chargingRateUnit)) == supported_charging_rate_units.npos) {
            return ProfileValidationResultEnum::ChargingScheduleChargingRateUnitUnsupported;
        }

        // A schedule must have at least one chargingSchedulePeriod
        if (schedule.chargingSchedulePeriod.empty()) {
            return ProfileValidationResultEnum::ChargingProfileNoChargingSchedulePeriods;
        }

        if (this->context.ocpp_version == OcppProtocolVersion::v21) {
            // K01.FR.95 Other profiles than TxProfle or TxDefaultProfile can not have a randomized delay.
            if (profile.chargingProfilePurpose != ChargingProfilePurposeEnum::TxProfile &&
                profile.chargingProfilePurpose != ChargingProfilePurposeEnum::TxDefaultProfile &&
                schedule.randomizedDelay.has_value() && schedule.randomizedDelay.value() > 0) {
                return ProfileValidationResultEnum::ChargingScheduleRandomizedDelay;
            }

            // K01.FR.120: Priority charging or local generation is not supported.
            const auto supported_additional_purposes = utils::get_charging_profile_purposes(
                this->context.device_model
                    .get_optional_value<std::string>(ControllerComponentVariables::SupportedAdditionalPurposes)
                    .value_or(""));
            auto it = std::find(supported_additional_purposes.begin(), supported_additional_purposes.end(),
                                profile.chargingProfilePurpose);
            if ((profile.chargingProfilePurpose == ChargingProfilePurposeEnum::PriorityCharging ||
                 profile.chargingProfilePurpose == ChargingProfilePurposeEnum::LocalGeneration) &&
                it == supported_additional_purposes.end()) {
                return ProfileValidationResultEnum::ChargingProfileUnsupportedPurpose;
            }

            // K01.FR.121: Charging profile kind is dynamic, but dynamic profiles are not supported.
            if (profile.chargingProfileKind == ChargingProfileKindEnum::Dynamic &&
                !this->context.device_model
                     .get_optional_value<bool>(ControllerComponentVariables::SupportsDynamicProfiles)
                     .value_or(false)) {
                return ProfileValidationResultEnum::ChargingProfileUnsupportedKind;
            }

            // K01.FR.122: Can not set dynamic update interval or time if charging profile kind is not dynamic.
            if ((profile.dynUpdateInterval.has_value() || profile.dynUpdateTime.has_value()) &&
                profile.chargingProfileKind != ChargingProfileKindEnum::Dynamic) {
                return ProfileValidationResultEnum::ChargingProfileNotDynamic;
            }

            // K01.FR.123 Local time is not supported
            if (schedule.useLocalTime.value_or(false) &&
                !this->context.device_model.get_optional_value<bool>(ControllerComponentVariables::SupportsUseLocalTime)
                     .value_or(false)) {
                return ProfileValidationResultEnum::ChargingScheduleUnsupportedLocalTime;
            }

            // K01.FR.124: Randomized delay is not supported
            if (schedule.randomizedDelay.has_value() &&
                !this->context.device_model
                     .get_optional_value<bool>(ControllerComponentVariables::SupportsRandomizedDelay)
                     .value_or(false)) {
                return ProfileValidationResultEnum::ChargingScheduleUnsupportedRandomizedDelay;
            }

            // K01.FR.125: Limit at soc is not supported
            if (schedule.limitAtSoC.has_value() &&
                !this->context.device_model.get_optional_value<bool>(ControllerComponentVariables::SupportsLimitAtSoC)
                     .value_or(false)) {
                return ProfileValidationResultEnum::ChargingScheduleUnsupportedLimitAtSoC;
            }
        }

        for (auto i = 0; i < schedule.chargingSchedulePeriod.size(); i++) {
            auto& charging_schedule_period = schedule.chargingSchedulePeriod[i];
            // K01.FR.48 and K01.FR.19
            if (charging_schedule_period.numberPhases != 1 && charging_schedule_period.phaseToUse.has_value()) {
                return ProfileValidationResultEnum::ChargingSchedulePeriodInvalidPhaseToUse;
            }

            // K01.FR.48 and K01.FR.20
            if (charging_schedule_period.phaseToUse.has_value() &&
                !this->context.device_model
                     .get_optional_value<bool>(ControllerComponentVariables::ACPhaseSwitchingSupported)
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

            // K01.FR.44; We reject profiles that provide invalid numberPhases/phaseToUse instead
            // of silently acccepting them.
            if (phase_type == CurrentPhaseType::DC && (charging_schedule_period.numberPhases.has_value() ||
                                                       charging_schedule_period.phaseToUse.has_value())) {
                if (this->context.ocpp_version == OcppProtocolVersion::v201) {
                    return ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues;
                } else if (this->context.ocpp_version == OcppProtocolVersion::v21) {
                    const int32_t evse_id = evse_opt.has_value() ? evse_opt.value()->get_id() : 0;
                    if (!this->has_dc_input_phase_control(evse_id)) {
                        // If 2.1 and DCInputPhaseControl is false or does not exist, then send rejected with reason
                        // code noPhaseForDC
                        // K01.FR.44
                        return ProfileValidationResultEnum::ChargingSchedulePeriodNoPhaseForDC;
                    } else {
                        // K01.FR.54
                        // TODO(mlitre): How to notify that this should be used for AC grid connection?
                    }
                }
            }

            if (phase_type == CurrentPhaseType::AC) {
                // K01.FR.45; Once again rejecting invalid values
                if (charging_schedule_period.numberPhases.has_value() &&
                    charging_schedule_period.numberPhases > charging_station_supply_phases) {
                    return ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedNumberPhases;
                }

                conform_schedule_number_phases(profile.id, charging_schedule_period);
            }

            if (this->context.ocpp_version == OcppProtocolVersion::v21) {
                const OperationModeEnum operation_mode =
                    charging_schedule_period.operationMode.value_or(OperationModeEnum::ChargingOnly);

                // K01.FR.71: Priority charging should not have operation mode that is different than 'ChargingOnly'
                if (profile.chargingProfilePurpose == ChargingProfilePurposeEnum::PriorityCharging &&
                    operation_mode != OperationModeEnum::ChargingOnly) {
                    return ProfileValidationResultEnum::ChargingSchedulePeriodPriorityChargingNotChargingOnly;
                }

                // Check all other operation modes.
                if (!check_operation_modes_for_charging_profile_purposes(operation_mode,
                                                                         profile.chargingProfilePurpose)) {
                    return ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedOperationMode;
                }

                // Q08.FR.05: LocalFrequency should have chargingRateUnit `W`.
                if (operation_mode == OperationModeEnum::LocalFrequency &&
                    schedule.chargingRateUnit == ChargingRateUnitEnum::A) {
                    return ProfileValidationResultEnum::ChargingScheduleChargingRateUnitUnsupported;
                }

                // K01.FR.126: EvseSleep is not supported.
                if (charging_schedule_period.evseSleep.value_or(false) &&
                    !this->context.device_model
                         .get_optional_value<bool>(ControllerComponentVariables::SupportsEvseSleep)
                         .value_or(false)) {
                    return ProfileValidationResultEnum::ChargingScheduleUnsupportedEvseSleep;
                }

                // Check limits and setpoints per operation mode (see table 'limits and setpoints per operation mode'
                // in the 2.1 spec).
                if (!check_limits_and_setpoints(charging_schedule_period)) {
                    return ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedLimitSetpoint;
                }

                // Q08.FR.02: v2xBaseline and v2xFreqWattCurve must be set when operation mode is LocalFrequency.
                if (operation_mode == OperationModeEnum::LocalFrequency &&
                    (!charging_schedule_period.v2xFreqWattCurve.has_value() ||
                     charging_schedule_period.v2xFreqWattCurve.value().size() < 2 ||
                     !charging_schedule_period.v2xBaseline.has_value())) {
                    return ProfileValidationResultEnum::ChargingSchedulePeriodNoFreqWattCurve;
                }

                if (!all_setpoints_signs_equal(charging_schedule_period)) {
                    // A different setpoint sign (negative / positive per phase) is (currently) not supported.
                    return ProfileValidationResultEnum::ChargingSchedulePeriodSignDifference;
                }
            }
        }

        // K01.FR.40 For Absolute and Recurring chargingProfileKind, a startSchedule shall exist.
        if ((profile.chargingProfileKind == ChargingProfileKindEnum::Absolute ||
             profile.chargingProfileKind == ChargingProfileKindEnum::Recurring) &&
            !schedule.startSchedule.has_value()) {
            return ProfileValidationResultEnum::ChargingProfileMissingRequiredStartSchedule;
            // K01.FR.41 For Relative chargingProfileKind, a startSchedule shall be absent.
        } else if (profile.chargingProfileKind == ChargingProfileKindEnum::Relative &&
                   schedule.startSchedule.has_value()) {
            return ProfileValidationResultEnum::ChargingProfileExtraneousStartSchedule;
        }
    }

    return ProfileValidationResultEnum::Valid;
}

ProfileValidationResultEnum
SmartCharging::verify_no_conflicting_external_constraints_id(const ChargingProfile& profile) const {
    // K01.FR.81: OCPP 2.1: When MaxExternalConstraintsId is set and the chargingProfile id is less or equal than
    // this value, return 'Rejected'.
    if (this->context.ocpp_version == OcppProtocolVersion::v21) {
        auto max_external_constraints_id =
            this->context.device_model.get_optional_value<int>(ControllerComponentVariables::MaxExternalConstraintsId);
        if (max_external_constraints_id.has_value() && profile.id <= max_external_constraints_id.value()) {
            return ProfileValidationResultEnum::ChargingProfileIdSmallerThanMaxExternalConstraintsId;
        }
    }

    auto result = ProfileValidationResultEnum::Valid;
    auto conflicts_stmt =
        this->context.database_handler.new_statement("SELECT PROFILE FROM CHARGING_PROFILES WHERE ID = @profile_id AND "
                                                     "CHARGING_PROFILE_PURPOSE = 'ChargingStationExternalConstraints'");

    conflicts_stmt->bind_int("@profile_id", profile.id);
    if (conflicts_stmt->step() == SQLITE_ROW) {
        result = ProfileValidationResultEnum::ExistingChargingStationExternalConstraints;
    }

    return result;
}

SetChargingProfileResponse SmartCharging::add_profile(ChargingProfile& profile, int32_t evse_id,
                                                      CiString<20> charging_limit_source) {
    SetChargingProfileResponse response;
    response.status = ChargingProfileStatusEnum::Accepted;

    try {
        // K01.FR.05 - replace non-ChargingStationExternalConstraints profiles if id exists.
        // K01.FR.27 - add profiles to database when valid. Currently we store all profiles. For 2.1 it is allowed to
        // only store ChargingStationMaxProfile, TxDefaultProfile and PriorityCharging, but currently we store
        // everything here.
        this->context.database_handler.insert_or_update_charging_profile(evse_id, profile, charging_limit_source);
    } catch (const everest::db::QueryExecutionException& e) {
        EVLOG_error << "Could not store ChargingProfile in the database: " << e.what();
        response.status = ChargingProfileStatusEnum::Rejected;
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = "InternalError";
    }

    return response;
}

ClearChargingProfileResponse SmartCharging::clear_profiles(const ClearChargingProfileRequest& request) {
    ClearChargingProfileResponse response;
    response.status = ClearChargingProfileStatusEnum::Unknown;

    if (this->context.database_handler.clear_charging_profiles_matching_criteria(request.chargingProfileId,
                                                                                 request.chargingProfileCriteria)) {
        response.status = ClearChargingProfileStatusEnum::Accepted;
    }

    return response;
}

std::vector<ReportedChargingProfile>
SmartCharging::get_reported_profiles(const GetChargingProfilesRequest& request) const {
    return this->context.database_handler.get_charging_profiles_matching_criteria(request.evseId,
                                                                                  request.chargingProfile);
}

std::vector<ChargingProfile>
SmartCharging::get_valid_profiles(int32_t evse_id, const std::vector<ChargingProfilePurposeEnum>& purposes_to_ignore) {
    std::vector<ChargingProfile> valid_profiles = get_valid_profiles_for_evse(evse_id, purposes_to_ignore);

    if (evse_id != STATION_WIDE_ID) {
        auto station_wide_profiles = get_valid_profiles_for_evse(STATION_WIDE_ID, purposes_to_ignore);
        valid_profiles.insert(valid_profiles.end(), station_wide_profiles.begin(), station_wide_profiles.end());
    }

    return valid_profiles;
}

void SmartCharging::report_charging_profile_req(const int32_t request_id, const int32_t evse_id,
                                                const CiString<20> source, const std::vector<ChargingProfile>& profiles,
                                                const bool tbc) {
    ReportChargingProfilesRequest req;
    req.requestId = request_id;
    req.evseId = evse_id;
    req.chargingLimitSource = source;
    req.chargingProfile = profiles;
    req.tbc = tbc;

    ocpp::Call<ReportChargingProfilesRequest> call(req);
    this->context.message_dispatcher.dispatch_call(call);
}

void SmartCharging::report_charging_profile_req(const ReportChargingProfilesRequest& req) {
    ocpp::Call<ReportChargingProfilesRequest> call(req);
    this->context.message_dispatcher.dispatch_call(call);
}

void SmartCharging::notify_ev_charging_needs_req(const NotifyEVChargingNeedsRequest& req) {
    NotifyEVChargingNeedsRequest request = req;
    if (this->context.ocpp_version != OcppProtocolVersion::v21) {
        request.timestamp = std::nullopt; // field is not present in OCPP2.0.1
    }

    ocpp::Call<NotifyEVChargingNeedsRequest> call(request);
    this->context.message_dispatcher.dispatch_call(call);
}

void SmartCharging::handle_set_charging_profile_req(Call<SetChargingProfileRequest> call) {
    EVLOG_debug << "Received SetChargingProfileRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    auto msg = call.msg;
    SetChargingProfileResponse response;
    response.status = ChargingProfileStatusEnum::Rejected;

    // K01.FR.29: Respond with a CallError if SmartCharging is not available for this Charging Station
    bool is_smart_charging_available =
        this->context.device_model.get_optional_value<bool>(ControllerComponentVariables::SmartChargingCtrlrAvailable)
            .value_or(false);

    if (!is_smart_charging_available) {
        EVLOG_warning << "SmartChargingCtrlrAvailable is not set for Charging Station. Returning NotSupported error";

        const auto call_error =
            CallError(call.uniqueId, "NotSupported", "Charging Station does not support smart charging", json({}));
        this->context.message_dispatcher.dispatch_call_error(call_error);

        return;
    }

    // K01.FR.22: Reject ChargingStationExternalConstraints profiles in SetChargingProfileRequest
    if (msg.chargingProfile.chargingProfilePurpose == ChargingProfilePurposeEnum::ChargingStationExternalConstraints) {
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = "InvalidValue";
        response.statusInfo->additionalInfo = "ChargingStationExternalConstraintsInSetChargingProfileRequest";
        EVLOG_debug << "Rejecting SetChargingProfileRequest:\n reasonCode: " << response.statusInfo->reasonCode.get()
                    << "\nadditionalInfo: " << response.statusInfo->additionalInfo->get();

        ocpp::CallResult<SetChargingProfileResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);

        return;
    }

    response = this->conform_validate_and_add_profile(msg.chargingProfile, msg.evseId);
    if (response.status == ChargingProfileStatusEnum::Accepted) {
        EVLOG_debug << "Accepting SetChargingProfileRequest";
        this->set_charging_profiles_callback();
    } else {
        EVLOG_debug << "Rejecting SetChargingProfileRequest:\n reasonCode: " << response.statusInfo->reasonCode.get()
                    << "\nadditionalInfo: " << response.statusInfo->additionalInfo->get();
    }

    ocpp::CallResult<SetChargingProfileResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

void SmartCharging::handle_clear_charging_profile_req(Call<ClearChargingProfileRequest> call) {
    EVLOG_debug << "Received ClearChargingProfileRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    const auto msg = call.msg;
    ClearChargingProfileResponse response;
    response.status = ClearChargingProfileStatusEnum::Unknown;

    // K10.FR.06
    if (msg.chargingProfileCriteria.has_value() and
        msg.chargingProfileCriteria.value().chargingProfilePurpose.has_value() and
        msg.chargingProfileCriteria.value().chargingProfilePurpose.value() ==
            ChargingProfilePurposeEnum::ChargingStationExternalConstraints) {
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = "InvalidValue";
        response.statusInfo->additionalInfo = "ChargingStationExternalConstraintsInClearChargingProfileRequest";
        EVLOG_debug << "Rejecting SetChargingProfileRequest:\n reasonCode: " << response.statusInfo->reasonCode.get()
                    << "\nadditionalInfo: " << response.statusInfo->additionalInfo->get();
    } else {
        response = this->clear_profiles(msg);
    }

    if (response.status == ClearChargingProfileStatusEnum::Accepted) {
        this->set_charging_profiles_callback();
    }

    ocpp::CallResult<ClearChargingProfileResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

void SmartCharging::handle_get_charging_profiles_req(Call<GetChargingProfilesRequest> call) {
    EVLOG_debug << "Received GetChargingProfilesRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    const auto msg = call.msg;
    GetChargingProfilesResponse response;

    const auto profiles_to_report = this->get_reported_profiles(msg);

    response.status =
        profiles_to_report.empty() ? GetChargingProfileStatusEnum::NoProfiles : GetChargingProfileStatusEnum::Accepted;

    ocpp::CallResult<GetChargingProfilesResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);

    if (response.status == GetChargingProfileStatusEnum::NoProfiles) {
        return;
    }

    // There are profiles to report.
    // Prepare ReportChargingProfileRequest(s). The message defines the properties evseId and
    // ChargingLimitSourceEnumStringType as required, so we can not report all profiles in a single
    // ReportChargingProfilesRequest. We need to prepare a single ReportChargingProfilesRequest for each combination
    // of evseId and ChargingLimitSourceEnumStringType
    std::set<int32_t> evse_ids;     // will contain all evse_ids of the profiles
    std::set<CiString<20>> sources; // will contain all sources of the profiles

    // fill evse_ids and sources sets
    for (const auto& profile : profiles_to_report) {
        evse_ids.insert(profile.evse_id);
        sources.insert(profile.source);
    }

    std::vector<ReportChargingProfilesRequest> requests_to_send;

    for (const auto evse_id : evse_ids) {
        for (const auto source : sources) {
            std::vector<ChargingProfile> original_profiles;
            for (const auto& reported_profile : profiles_to_report) {
                if (reported_profile.evse_id == evse_id and reported_profile.source == source) {
                    original_profiles.push_back(reported_profile.profile);
                }
            }
            if (not original_profiles.empty()) {
                // prepare a ReportChargingProfilesRequest
                ReportChargingProfilesRequest req;
                req.requestId = msg.requestId; // K09.FR.01
                req.evseId = evse_id;
                req.chargingLimitSource = source;
                req.chargingProfile = original_profiles;
                req.tbc = true;
                requests_to_send.push_back(req);
            }
        }
    }

    requests_to_send.back().tbc = false;

    // requests_to_send are ready, send them and define tbc property
    for (const auto& request_to_send : requests_to_send) {
        this->report_charging_profile_req(request_to_send);
    }
}

void SmartCharging::handle_notify_ev_charging_needs_response(const EnhancedMessage<MessageType>& call_result) {
    CallResult<NotifyEVChargingNeedsResponse> response = call_result.message;
    Call<NotifyEVChargingNeedsRequest> request = call_result.call_message;
    EVLOG_debug << "Received NotifyEVChargingNeedsResponse: " << response.msg
                << "\nwith messageId: " << response.uniqueId;
    const bool is_15118_20 = request.msg.chargingNeeds.v2xChargingParameters.has_value();
    switch (response.msg.status) {
    case NotifyEVChargingNeedsStatusEnum::Accepted:
        // K15.FR.03 - ISO15118-2
        // K18.FR.03 - Scheduled Mode
        // K19.FR.03 - Dynamic Mode
        // TODO(mlitre) Support HLC smart charging
        // Wait for schedule, aka SetChargingProfileRequest
        break;
    case NotifyEVChargingNeedsStatusEnum::Rejected:
        // K18.FR.22 - Scheduled Mode
        // K19.FR.15 - Dynamic Mode
        if (this->context.ocpp_version == OcppProtocolVersion::v201 or !is_15118_20) {
            // K15.FR.04 - ISO15118-2
            // TODO(mlitre): Support HLC smart charging
            // Start without waiting for schedule, equivalent to NoChargingProfile
            [[fallthrough]];
        } else {
            // Start service renegotiation or Stop transaction based on OCPP version
            // Also check ISO15118 version
            // Service renegotiation should not technically be possible so stop transaction
            // Q01.FR.06 - V2X Authorization
            // K18.FR.23 - Scheduled Mode
            // K19.FR.16 - Dynamic Mode
            stop_transaction_callback(request.msg.evseId, ReasonEnum::ReqEnergyTransferRejected);
            break;
        }
    case NotifyEVChargingNeedsStatusEnum::Processing:
        // Q01.FR.07 should receive profile soon, but since it is V2X should we wait or just start and do service
        // renegotiation after? It seems like we don't have to wait
        // K15.FR.05 - ISO15118-2
        // K18.FR.05 - Scheduled Mode
        // K19.FR.05 - Dynamic Mode
    case NotifyEVChargingNeedsStatusEnum::NoChargingProfile:
        // K18.FR.04 - Scheduled Mode
        // K19.FR.04 - Dynamic Mode
        // TODO(mlitre): Support HLC smart charging
        // Start without waiting for schedule, schedule renegotiation will be calculated as per use case K16
        break;
    }
}

void SmartCharging::handle_get_composite_schedule_req(Call<GetCompositeScheduleRequest> call) {
    EVLOG_debug << "Received GetCompositeScheduleRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    const auto response = this->get_composite_schedule_internal(call.msg);

    ocpp::CallResult<GetCompositeScheduleResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

GetCompositeScheduleResponse SmartCharging::get_composite_schedule_internal(const GetCompositeScheduleRequest& request,
                                                                            bool simulate_transaction_active) {
    GetCompositeScheduleResponse response;
    response.status = GenericStatusEnum::Rejected;

    std::vector<std::string> supported_charging_rate_units =
        ocpp::split_string(this->context.device_model.get_value<std::string>(
                               ControllerComponentVariables::ChargingScheduleChargingRateUnit),
                           ',', true);

    std::optional<ChargingRateUnitEnum> charging_rate_unit = std::nullopt;
    if (request.chargingRateUnit.has_value()) {
        bool unit_supported = std::any_of(
            supported_charging_rate_units.begin(), supported_charging_rate_units.end(), [&request](std::string item) {
                return conversions::string_to_charging_rate_unit_enum(item) == request.chargingRateUnit.value();
            });

        if (unit_supported) {
            charging_rate_unit = request.chargingRateUnit.value();
        }
    } else if (supported_charging_rate_units.size() > 0) {
        charging_rate_unit = conversions::string_to_charging_rate_unit_enum(supported_charging_rate_units.at(0));
    }

    // K01.FR.05 & K01.FR.07
    if (this->context.evse_manager.does_evse_exist(request.evseId) and charging_rate_unit.has_value()) {
        auto start_time = ocpp::DateTime();
        auto end_time = ocpp::DateTime(start_time.to_time_point() + std::chrono::seconds(request.duration));

        auto schedule = this->calculate_composite_schedule(
            start_time, end_time, request.evseId, charging_rate_unit.value(),
            !this->context.connectivity_manager.is_websocket_connected(), simulate_transaction_active);

        response.schedule = schedule;
        response.status = GenericStatusEnum::Accepted;
    } else {
        auto reason = charging_rate_unit.has_value()
                          ? ProfileValidationResultEnum::EvseDoesNotExist
                          : ProfileValidationResultEnum::ChargingScheduleChargingRateUnitUnsupported;
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = conversions::profile_validation_result_to_reason_code(reason);
        response.statusInfo->additionalInfo = conversions::profile_validation_result_to_string(reason);
        EVLOG_debug << "Rejecting SetChargingProfileRequest:\n reasonCode: " << response.statusInfo->reasonCode.get()
                    << "\nadditionalInfo: " << response.statusInfo->additionalInfo->get();
    }
    return response;
}

ProfileValidationResultEnum
SmartCharging::validate_request_start_transaction_profile(const ChargingProfile& profile) const {
    if (ChargingProfilePurposeEnum::TxProfile != profile.chargingProfilePurpose) {
        return ProfileValidationResultEnum::RequestStartTransactionNonTxProfile;
    }
    return ProfileValidationResultEnum::Valid;
}

bool SmartCharging::is_overlapping_validity_period(const ChargingProfile& candidate_profile,
                                                   int32_t candidate_evse_id) const {
    if (candidate_profile.chargingProfilePurpose == ChargingProfilePurposeEnum::TxProfile) {
        // This only applies to non TxProfile types.
        return false;
    }

    auto overlap_stmt = this->context.database_handler.new_statement(
        "SELECT PROFILE FROM CHARGING_PROFILES WHERE CHARGING_PROFILE_PURPOSE = @purpose AND EVSE_ID = "
        "@evse_id AND ID != @profile_id AND CHARGING_PROFILES.STACK_LEVEL = @stack_level");

    overlap_stmt->bind_int("@evse_id", candidate_evse_id);
    overlap_stmt->bind_int("@profile_id", candidate_profile.id);
    overlap_stmt->bind_int("@stack_level", candidate_profile.stackLevel);
    overlap_stmt->bind_text(
        "@purpose", conversions::charging_profile_purpose_enum_to_string(candidate_profile.chargingProfilePurpose),
        everest::db::sqlite::SQLiteString::Transient);
    while (overlap_stmt->step() != SQLITE_DONE) {
        ChargingProfile existing_profile = json::parse(overlap_stmt->column_text(0));
        if (candidate_profile.validFrom <= existing_profile.validTo &&
            candidate_profile.validTo >= existing_profile.validFrom) {
            return true;
        }
    }

    return false;
}

std::vector<ChargingProfile> SmartCharging::get_evse_specific_tx_default_profiles() const {
    std::vector<ChargingProfile> evse_specific_tx_default_profiles;

    auto stmt =
        this->context.database_handler.new_statement("SELECT PROFILE FROM CHARGING_PROFILES WHERE "
                                                     "EVSE_ID != 0 AND CHARGING_PROFILE_PURPOSE = 'TxDefaultProfile'");
    while (stmt->step() != SQLITE_DONE) {
        ChargingProfile profile = json::parse(stmt->column_text(0));
        evse_specific_tx_default_profiles.push_back(profile);
    }

    return evse_specific_tx_default_profiles;
}

std::vector<ChargingProfile> SmartCharging::get_station_wide_tx_default_profiles() const {
    std::vector<ChargingProfile> station_wide_tx_default_profiles;

    auto stmt = this->context.database_handler.new_statement(
        "SELECT PROFILE FROM CHARGING_PROFILES WHERE EVSE_ID = 0 AND CHARGING_PROFILE_PURPOSE = 'TxDefaultProfile'");
    while (stmt->step() != SQLITE_DONE) {
        ChargingProfile profile = json::parse(stmt->column_text(0));
        station_wide_tx_default_profiles.push_back(profile);
    }

    return station_wide_tx_default_profiles;
}

std::vector<ChargingProfile> SmartCharging::get_charging_station_max_profiles() const {
    std::vector<ChargingProfile> charging_station_max_profiles;
    auto stmt =
        this->context.database_handler.new_statement("SELECT PROFILE FROM CHARGING_PROFILES WHERE EVSE_ID = 0 AND "
                                                     "CHARGING_PROFILE_PURPOSE = 'ChargingStationMaxProfile'");
    while (stmt->step() != SQLITE_DONE) {
        ChargingProfile profile = json::parse(stmt->column_text(0));
        charging_station_max_profiles.push_back(profile);
    }

    return charging_station_max_profiles;
}

std::vector<ChargingProfile>
SmartCharging::get_valid_profiles_for_evse(int32_t evse_id,
                                           const std::vector<ChargingProfilePurposeEnum>& purposes_to_ignore) {
    std::vector<ChargingProfile> valid_profiles;

    auto evse_profiles = this->context.database_handler.get_charging_profiles_for_evse(evse_id);
    for (auto profile : evse_profiles) {
        if (this->conform_and_validate_profile(profile, evse_id) == ProfileValidationResultEnum::Valid and
            std::find(std::begin(purposes_to_ignore), std::end(purposes_to_ignore), profile.chargingProfilePurpose) ==
                std::end(purposes_to_ignore)) {
            valid_profiles.push_back(profile);
        }
    }

    return valid_profiles;
}

void SmartCharging::conform_schedule_number_phases(int32_t profile_id,
                                                   ChargingSchedulePeriod& charging_schedule_period) const {
    // K01.FR.49 If no value for numberPhases received for AC, numberPhases is 3.
    if (!charging_schedule_period.numberPhases.has_value()) {
        EVLOG_debug << "Conforming profile: " << profile_id << " added number phase as "
                    << DEFAULT_AND_MAX_NUMBER_PHASES;
        charging_schedule_period.numberPhases.emplace(DEFAULT_AND_MAX_NUMBER_PHASES);
    }
}

void SmartCharging::conform_validity_periods(ChargingProfile& profile) const {
    if (!profile.validFrom.has_value()) {
        auto validFrom = ocpp::DateTime("1970-01-01T00:00:00Z");
        EVLOG_debug << "Conforming profile: " << profile.id << " added validFrom as " << validFrom;
        profile.validFrom = validFrom;
    }

    if (!profile.validTo.has_value()) {
        auto validTo = ocpp::DateTime(date::utc_clock::time_point::max());
        EVLOG_debug << "Conforming profile: " << profile.id << " added validTo as " << validTo;
        profile.validTo = validTo;
    }
}

CurrentPhaseType SmartCharging::get_current_phase_type(const std::optional<EvseInterface*> evse_opt) const {
    if (evse_opt.has_value()) {
        return evse_opt.value()->get_current_phase_type();
    }

    auto supply_phases =
        this->context.device_model.get_value<int32_t>(ControllerComponentVariables::ChargingStationSupplyPhases);
    if (supply_phases == 1 || supply_phases == 3) {
        return CurrentPhaseType::AC;
    } else if (supply_phases == 0) {
        return CurrentPhaseType::DC;
    }

    return CurrentPhaseType::Unknown;
}

bool are_limits_and_setpoints_of_operation_mode_correct(const LimitsSetpointsForOperationMode& limits_setpoints,
                                                        const LimitSetpointType& type,
                                                        const std::optional<float>& limit,
                                                        const std::optional<float>& limit_L2,
                                                        const std::optional<float>& limit_L3) {
    if ((limits_setpoints.required.count(type) > 0 && !limit.has_value()) ||
        ((limit.has_value() || limit_L2.has_value() || limit_L3.has_value()) &&
         limits_setpoints.required.count(type) == 0 && limits_setpoints.optional.count(type) == 0)) {
        return false;
    }

    return true;
}

bool check_limits_and_setpoints(const ChargingSchedulePeriod& charging_schedule_period) {
    // Q08.FR.04, Q10.FR.01, Q10.FR.02 (among others?)
    const OperationModeEnum operation_mode =
        charging_schedule_period.operationMode.value_or(OperationModeEnum::ChargingOnly);
    try {
        const LimitsSetpointsForOperationMode limits_setpoints = limits_setpoints_per_operation_mode.at(operation_mode);
        return are_limits_and_setpoints_of_operation_mode_correct(
                   limits_setpoints, LimitSetpointType::Limit, charging_schedule_period.limit,
                   charging_schedule_period.limit_L2, charging_schedule_period.limit_L3) &&
               are_limits_and_setpoints_of_operation_mode_correct(
                   limits_setpoints, LimitSetpointType::DischargeLimit, charging_schedule_period.dischargeLimit,
                   charging_schedule_period.dischargeLimit_L2, charging_schedule_period.dischargeLimit_L3) &&
               are_limits_and_setpoints_of_operation_mode_correct(
                   limits_setpoints, LimitSetpointType::Setpoint, charging_schedule_period.setpoint,
                   charging_schedule_period.setpoint_L2, charging_schedule_period.setpoint_L3) &&
               are_limits_and_setpoints_of_operation_mode_correct(
                   limits_setpoints, LimitSetpointType::SetpointReactive, charging_schedule_period.setpointReactive,
                   charging_schedule_period.setpointReactive_L2, charging_schedule_period.setpointReactive_L3);
    } catch (const std::out_of_range& e) {
        EVLOG_warning << "Operation mode "
                      << conversions::operation_mode_enum_to_string(charging_schedule_period.operationMode.value())
                      << " not in list of valid limits and setpoints: can not check if limits and "
                         "setpoints are valid";
        return false;
    }
}

bool all_setpoints_signs_equal(const ChargingSchedulePeriod& charging_schedule_period) {
    if (charging_schedule_period.setpoint != std::nullopt && (charging_schedule_period.setpoint_L2 != std::nullopt ||
                                                              (charging_schedule_period.setpoint_L3 != std::nullopt))) {
        if ((charging_schedule_period.setpoint.value() > 0.0f &&
             ((charging_schedule_period.setpoint_L2.has_value() &&
               charging_schedule_period.setpoint_L2.value() < 0.0F) ||
              (charging_schedule_period.setpoint_L3.has_value() &&
               charging_schedule_period.setpoint_L3.value() < 0.0F))) ||
            (charging_schedule_period.setpoint.value() < 0.0f &&
             ((charging_schedule_period.setpoint_L2.has_value() &&
               charging_schedule_period.setpoint_L2.value() > 0.0F) ||
              (charging_schedule_period.setpoint_L3.has_value() &&
               charging_schedule_period.setpoint_L3.value() > 0.0F)))) {
            return false;
        }
    }

    return true;
}

bool check_operation_modes_for_charging_profile_purposes(const OperationModeEnum& operation_mode,
                                                         const ChargingProfilePurposeEnum& purpose) {
    try {
        if (operation_modes_for_charging_profile_purposes.at(purpose).count(operation_mode) == 0) {
            return false;
        }
    } catch (const std::out_of_range& e) {
        EVLOG_warning << "Charging profile purpose " << conversions::charging_profile_purpose_enum_to_string(purpose)
                      << " not in list of valid operation modes: can not check if operation mode is valid.";
    }

    return true;
}
} // namespace ocpp::v2
