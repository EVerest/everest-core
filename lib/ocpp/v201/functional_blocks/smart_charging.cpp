// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/functional_blocks/smart_charging.hpp>

#include <ocpp/v201/connectivity_manager.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/evse_manager.hpp>
#include <ocpp/v201/profile.hpp>

#include <ocpp/v201/utils.hpp>

#include <ocpp/common/constants.hpp>

const int32_t STATION_WIDE_ID = 0;

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
    case ProfileValidationResultEnum::RequestStartTransactionNonTxProfile:
        return "RequestStartTransactionNonTxProfile";
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

SmartCharging::SmartCharging(DeviceModel& device_model, EvseManagerInterface& evse_manager,
                             ConnectivityManagerInterface& connectivity_manager,
                             MessageDispatcherInterface<MessageType>& message_dispatcher,
                             DatabaseHandlerInterface& database_handler,
                             std::function<void()> set_charging_profiles_callback) :
    device_model(device_model),
    evse_manager(evse_manager),
    connectivity_manager(connectivity_manager),
    message_dispatcher(message_dispatcher),
    database_handler(database_handler),
    set_charging_profiles_callback(set_charging_profiles_callback) {
}

void SmartCharging::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    const auto& json_message = message.message;
    switch (message.messageType) {
    case MessageType::SetChargingProfile:
        this->handle_set_charging_profile_req(json_message);
        break;
    case MessageType::ClearChargingProfile:
        this->handle_clear_charging_profile_req(json_message);
        break;
    case MessageType::GetChargingProfiles:
        this->handle_get_charging_profiles_req(json_message);
        break;
    case MessageType::GetCompositeSchedule:
        this->handle_get_composite_schedule_req(json_message);
        break;
    default:
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

std::vector<CompositeSchedule> SmartCharging::get_all_composite_schedules(const int32_t duration_s,
                                                                          const ChargingRateUnitEnum& unit) {
    std::vector<CompositeSchedule> composite_schedules;

    const auto number_of_evses = this->evse_manager.get_number_of_evses();
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
    this->database_handler.delete_charging_profile_by_transaction_id(transaction_id);
}

SetChargingProfileResponse
SmartCharging::conform_validate_and_add_profile(ChargingProfile& profile, int32_t evse_id,
                                                ChargingLimitSourceEnum charging_limit_source,
                                                AddChargingProfileSource source_of_request) {
    SetChargingProfileResponse response;
    response.status = ChargingProfileStatusEnum::Rejected;

    auto result = this->conform_and_validate_profile(profile, evse_id, source_of_request);
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
        result = this->validate_tx_profile(profile, evse_id, source_of_request);
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
                                                             bool simulate_transaction_active

) {

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

    const CompositeScheduleConfig config{this->device_model, is_offline};

    std::optional<ocpp::DateTime> session_start{};
    if (this->evse_manager.does_evse_exist(evse_id) and evse_id != 0 and
        this->evse_manager.get_evse(evse_id).get_transaction() != nullptr) {
        const auto& transaction = this->evse_manager.get_evse(evse_id).get_transaction();
        session_start = transaction->start_time;
    }

    const auto station_wide_profiles = get_valid_profiles_for_evse(STATION_WIDE_ID, config.purposes_to_ignore);

    std::vector<IntermediateProfile> combined_profiles{};

    if (evse_id == STATION_WIDE_ID) {
        auto nr_of_evses = this->evse_manager.get_number_of_evses();

        // Get the ChargingStationExternalConstraints and Combined Tx(Default)Profiles per evse
        std::vector<IntermediateProfile> evse_schedules{};
        for (int evse = 1; evse <= nr_of_evses; evse++) {
            auto intermediates = generate_evse_intermediates(
                get_valid_profiles_for_evse(evse, config.purposes_to_ignore), station_wide_profiles, start_time,
                end_time, session_start, simulate_transaction_active);

            // Determine the lowest limits per evse
            evse_schedules.push_back(merge_profiles_by_lowest_limit(intermediates));
        }

        // Add all the limits of all the evse's together since that will be the max the whole charging station can
        // consume at any point in time
        combined_profiles.push_back(
            merge_profiles_by_summing_limits(evse_schedules, config.current_limit, config.power_limit));

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
    auto retval = merge_profiles_by_lowest_limit(combined_profiles);

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
    return evse_manager.does_evse_exist(evse_id) ? ProfileValidationResultEnum::Valid
                                                 : ProfileValidationResultEnum::EvseDoesNotExist;
}

ProfileValidationResultEnum SmartCharging::validate_charging_station_max_profile(const ChargingProfile& profile,
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

ProfileValidationResultEnum SmartCharging::validate_tx_default_profile(const ChargingProfile& profile,
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

ProfileValidationResultEnum SmartCharging::validate_tx_profile(const ChargingProfile& profile, int32_t evse_id,
                                                               AddChargingProfileSource source_of_request) const {
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
        return ProfileValidationResultEnum::TxProfileMissingTransactionId;
    }

    auto& evse = evse_manager.get_evse(evse_id);
    if (!evse.has_active_transaction()) {
        return ProfileValidationResultEnum::TxProfileEvseHasNoActiveTransaction;
    }

    auto& transaction = evse.get_transaction();
    if (transaction->transactionId != profile.transactionId.value()) {
        return ProfileValidationResultEnum::TxProfileTransactionNotOnEvse;
    }

    auto conflicts_stmt =
        this->database_handler.new_statement("SELECT PROFILE FROM CHARGING_PROFILES WHERE TRANSACTION_ID = "
                                             "@transaction_id AND STACK_LEVEL = @stack_level AND ID != @id");
    conflicts_stmt->bind_int("@stack_level", profile.stackLevel);
    conflicts_stmt->bind_int("@id", profile.id);
    if (profile.transactionId.has_value()) {
        conflicts_stmt->bind_text("@transaction_id", profile.transactionId.value().get(),
                                  common::SQLiteString::Transient);
    } else {
        conflicts_stmt->bind_null("@transaction_id");
    }

    if (conflicts_stmt->step() == SQLITE_ROW) {
        return ProfileValidationResultEnum::TxProfileConflictingStackLevel;
    }

    return ProfileValidationResultEnum::Valid;
}

/* TODO: Implement the following functional requirements:
 * - K01.FR.34
 * - K01.FR.43
 * - K01.FR.48
 */

ProfileValidationResultEnum SmartCharging::validate_profile_schedules(ChargingProfile& profile,
                                                                      std::optional<EvseInterface*> evse_opt) const {
    auto charging_station_supply_phases =
        this->device_model.get_value<int32_t>(ControllerComponentVariables::ChargingStationSupplyPhases);

    auto phase_type = this->get_current_phase_type(evse_opt);

    for (auto& schedule : profile.chargingSchedule) {
        // K01.FR.26; We currently need to do string conversions for this manually because our DeviceModel class
        // does not let us get a vector of ChargingScheduleChargingRateUnits.
        auto supported_charging_rate_units =
            this->device_model.get_value<std::string>(ControllerComponentVariables::ChargingScheduleChargingRateUnit);
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
            // K01.FR.48 and K01.FR.19
            if (charging_schedule_period.numberPhases != 1 && charging_schedule_period.phaseToUse.has_value()) {
                return ProfileValidationResultEnum::ChargingSchedulePeriodInvalidPhaseToUse;
            }

            // K01.FR.48 and K01.FR.20
            if (charging_schedule_period.phaseToUse.has_value() &&
                !device_model.get_optional_value<bool>(ControllerComponentVariables::ACPhaseSwitchingSupported)
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
                return ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues;
            }

            if (phase_type == CurrentPhaseType::AC) {
                // K01.FR.45; Once again rejecting invalid values
                if (charging_schedule_period.numberPhases.has_value() &&
                    charging_schedule_period.numberPhases > charging_station_supply_phases) {
                    return ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedNumberPhases;
                }

                conform_schedule_number_phases(profile.id, charging_schedule_period);
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

ProfileValidationResultEnum
SmartCharging::verify_no_conflicting_external_constraints_id(const ChargingProfile& profile) const {
    auto result = ProfileValidationResultEnum::Valid;
    auto conflicts_stmt =
        this->database_handler.new_statement("SELECT PROFILE FROM CHARGING_PROFILES WHERE ID = @profile_id AND "
                                             "CHARGING_PROFILE_PURPOSE = 'ChargingStationExternalConstraints'");

    conflicts_stmt->bind_int("@profile_id", profile.id);
    if (conflicts_stmt->step() == SQLITE_ROW) {
        result = ProfileValidationResultEnum::ExistingChargingStationExternalConstraints;
    }

    return result;
}

SetChargingProfileResponse SmartCharging::add_profile(ChargingProfile& profile, int32_t evse_id,
                                                      ChargingLimitSourceEnum charging_limit_source) {
    SetChargingProfileResponse response;
    response.status = ChargingProfileStatusEnum::Accepted;

    try {
        // K01.FR05 - replace non-ChargingStationExternalConstraints profiles if id exists.
        // K01.FR27 - add profiles to database when valid
        this->database_handler.insert_or_update_charging_profile(evse_id, profile, charging_limit_source);
    } catch (const QueryExecutionException& e) {
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

    if (this->database_handler.clear_charging_profiles_matching_criteria(request.chargingProfileId,
                                                                         request.chargingProfileCriteria)) {
        response.status = ClearChargingProfileStatusEnum::Accepted;
    }

    return response;
}

std::vector<ReportedChargingProfile>
SmartCharging::get_reported_profiles(const GetChargingProfilesRequest& request) const {
    return this->database_handler.get_charging_profiles_matching_criteria(request.evseId, request.chargingProfile);
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
                                                const ChargingLimitSourceEnum source,
                                                const std::vector<ChargingProfile>& profiles, const bool tbc) {
    ReportChargingProfilesRequest req;
    req.requestId = request_id;
    req.evseId = evse_id;
    req.chargingLimitSource = source;
    req.chargingProfile = profiles;
    req.tbc = tbc;

    ocpp::Call<ReportChargingProfilesRequest> call(req);
    this->message_dispatcher.dispatch_call(call);
}

void SmartCharging::report_charging_profile_req(const ReportChargingProfilesRequest& req) {
    ocpp::Call<ReportChargingProfilesRequest> call(req);
    this->message_dispatcher.dispatch_call(call);
}

void SmartCharging::handle_set_charging_profile_req(Call<SetChargingProfileRequest> call) {
    EVLOG_debug << "Received SetChargingProfileRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    auto msg = call.msg;
    SetChargingProfileResponse response;
    response.status = ChargingProfileStatusEnum::Rejected;

    // K01.FR.29: Respond with a CallError if SmartCharging is not available for this Charging Station
    bool is_smart_charging_available =
        this->device_model.get_optional_value<bool>(ControllerComponentVariables::SmartChargingCtrlrAvailable)
            .value_or(false);

    if (!is_smart_charging_available) {
        EVLOG_warning << "SmartChargingCtrlrAvailable is not set for Charging Station. Returning NotSupported error";

        const auto call_error =
            CallError(call.uniqueId, "NotSupported", "Charging Station does not support smart charging", json({}));
        this->message_dispatcher.dispatch_call_error(call_error);

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
        this->message_dispatcher.dispatch_call_result(call_result);

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
    this->message_dispatcher.dispatch_call_result(call_result);
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
    this->message_dispatcher.dispatch_call_result(call_result);
}

void SmartCharging::handle_get_charging_profiles_req(Call<GetChargingProfilesRequest> call) {
    EVLOG_debug << "Received GetChargingProfilesRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    const auto msg = call.msg;
    GetChargingProfilesResponse response;

    const auto profiles_to_report = this->get_reported_profiles(msg);

    response.status =
        profiles_to_report.empty() ? GetChargingProfileStatusEnum::NoProfiles : GetChargingProfileStatusEnum::Accepted;

    ocpp::CallResult<GetChargingProfilesResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);

    if (response.status == GetChargingProfileStatusEnum::NoProfiles) {
        return;
    }

    // There are profiles to report.
    // Prepare ReportChargingProfileRequest(s). The message defines the properties evseId and
    // chargingLimitSource as required, so we can not report all profiles in a single
    // ReportChargingProfilesRequest. We need to prepare a single ReportChargingProfilesRequest for each
    // combination of evseId and chargingLimitSource
    std::set<int32_t> evse_ids;                // will contain all evse_ids of the profiles
    std::set<ChargingLimitSourceEnum> sources; // will contain all sources of the profiles

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

void SmartCharging::handle_get_composite_schedule_req(Call<GetCompositeScheduleRequest> call) {
    EVLOG_debug << "Received GetCompositeScheduleRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    const auto response = this->get_composite_schedule_internal(call.msg);

    ocpp::CallResult<GetCompositeScheduleResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);
}

GetCompositeScheduleResponse SmartCharging::get_composite_schedule_internal(const GetCompositeScheduleRequest& request,
                                                                            bool simulate_transaction_active) {
    GetCompositeScheduleResponse response;
    response.status = GenericStatusEnum::Rejected;

    std::vector<std::string> supported_charging_rate_units = ocpp::split_string(
        this->device_model.get_value<std::string>(ControllerComponentVariables::ChargingScheduleChargingRateUnit), ',',
        true);

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
    if (this->evse_manager.does_evse_exist(request.evseId) and charging_rate_unit.has_value()) {
        auto start_time = ocpp::DateTime();
        auto end_time = ocpp::DateTime(start_time.to_time_point() + std::chrono::seconds(request.duration));

        auto schedule = this->calculate_composite_schedule(
            start_time, end_time, request.evseId, charging_rate_unit.value(),
            !this->connectivity_manager.is_websocket_connected(), simulate_transaction_active);

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

    auto overlap_stmt = this->database_handler.new_statement(
        "SELECT PROFILE FROM CHARGING_PROFILES WHERE CHARGING_PROFILE_PURPOSE = @purpose AND EVSE_ID = "
        "@evse_id AND ID != @profile_id AND CHARGING_PROFILES.STACK_LEVEL = @stack_level");

    overlap_stmt->bind_int("@evse_id", candidate_evse_id);
    overlap_stmt->bind_int("@profile_id", candidate_profile.id);
    overlap_stmt->bind_int("@stack_level", candidate_profile.stackLevel);
    overlap_stmt->bind_text(
        "@purpose", conversions::charging_profile_purpose_enum_to_string(candidate_profile.chargingProfilePurpose),
        common::SQLiteString::Transient);
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

    auto stmt = this->database_handler.new_statement("SELECT PROFILE FROM CHARGING_PROFILES WHERE "
                                                     "EVSE_ID != 0 AND CHARGING_PROFILE_PURPOSE = 'TxDefaultProfile'");
    while (stmt->step() != SQLITE_DONE) {
        ChargingProfile profile = json::parse(stmt->column_text(0));
        evse_specific_tx_default_profiles.push_back(profile);
    }

    return evse_specific_tx_default_profiles;
}

std::vector<ChargingProfile> SmartCharging::get_station_wide_tx_default_profiles() const {
    std::vector<ChargingProfile> station_wide_tx_default_profiles;

    auto stmt = this->database_handler.new_statement(
        "SELECT PROFILE FROM CHARGING_PROFILES WHERE EVSE_ID = 0 AND CHARGING_PROFILE_PURPOSE = 'TxDefaultProfile'");
    while (stmt->step() != SQLITE_DONE) {
        ChargingProfile profile = json::parse(stmt->column_text(0));
        station_wide_tx_default_profiles.push_back(profile);
    }

    return station_wide_tx_default_profiles;
}

std::vector<ChargingProfile>
SmartCharging::get_valid_profiles_for_evse(int32_t evse_id,
                                           const std::vector<ChargingProfilePurposeEnum>& purposes_to_ignore) {
    std::vector<ChargingProfile> valid_profiles;

    auto evse_profiles = this->database_handler.get_charging_profiles_for_evse(evse_id);
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
    // K01.FR.49
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
        this->device_model.get_value<int32_t>(ControllerComponentVariables::ChargingStationSupplyPhases);
    if (supply_phases == 1 || supply_phases == 3) {
        return CurrentPhaseType::AC;
    } else if (supply_phases == 0) {
        return CurrentPhaseType::DC;
    }

    return CurrentPhaseType::Unknown;
}
} // namespace ocpp::v201
