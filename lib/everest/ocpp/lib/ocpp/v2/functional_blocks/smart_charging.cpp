// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/functional_blocks/smart_charging.hpp>

#include <cmath>
#include <optional>

#include <ocpp/common/constants.hpp>

#include <ocpp/v2/connectivity_manager.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/evse_manager.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/profile.hpp>
#include <ocpp/v2/utils.hpp>

#include <algorithm>
#include <ocpp/v2/messages/ClearChargingProfile.hpp>
#include <ocpp/v2/messages/GetChargingProfiles.hpp>
#include <ocpp/v2/messages/GetCompositeSchedule.hpp>

#include <ocpp/v2/messages/NotifyEVChargingNeeds.hpp>
#include <ocpp/v2/messages/NotifyEVChargingSchedule.hpp>
#include <ocpp/v2/messages/ReportChargingProfiles.hpp>
#include <ocpp/v2/messages/SetChargingProfile.hpp>

const std::int32_t STATION_WIDE_ID = 0;

using namespace std::chrono;
using namespace std::chrono_literals;

namespace ocpp::v2 {
namespace {
/// \brief validates that the given \p profile from a RequestStartTransactionRequest is of the correct type
/// TxProfile
ProfileValidationResultEnum validate_request_start_transaction_profile(const ChargingProfile& profile) {
    if (ChargingProfilePurposeEnum::TxProfile != profile.chargingProfilePurpose) {
        return ProfileValidationResultEnum::RequestStartTransactionNonTxProfile;
    }
    return ProfileValidationResultEnum::Valid;
}

/// \brief sets attributes of the given \p charging_schedule_period according to the specification.
/// 2.11. ChargingSchedulePeriodType if absent numberPhases set to 3
void conform_schedule_number_phases(std::int32_t profile_id, ChargingSchedulePeriod& charging_schedule_period) {
    // K01.FR.49 If no value for numberPhases received for AC, numberPhases is 3.
    if (!charging_schedule_period.numberPhases.has_value()) {
        EVLOG_debug << "Conforming profile: " << profile_id << " added number phase as "
                    << DEFAULT_AND_MAX_NUMBER_PHASES;
        charging_schedule_period.numberPhases.emplace(DEFAULT_AND_MAX_NUMBER_PHASES);
    }
}

///
/// \brief sets attributes of the given \p profile according to the specification.
/// 2.10. ChargingProfileType validFrom if absent set to current date
/// 2.10. ChargingProfileType validTo if absent set to max date
///
void conform_validity_periods(ChargingProfile& profile) {
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

/// \brief Check if a float value is non-finite (infinity or NaN)
template <typename T> bool non_finite(const T& value) = delete;

template <> constexpr bool non_finite<float>(const float& value) {
    return !std::isfinite(value);
}

template <> constexpr bool non_finite<std::optional<float>>(const std::optional<float>& value) {
    return !std::isfinite(value.value_or(0.0));
}

template <typename T, typename... Values> constexpr bool non_finite(const T& value, const Values&... values) {
    return non_finite(value) || non_finite(values...);
}

/// \brief Returns true if any float field in \p period is non-finite (infinity or NaN).
/// Such values originate from float overflow (e.g. a max-double sent by a CSMS) and would be
/// serialized as JSON null
bool has_non_finite_float(const ChargingSchedulePeriod& period) {
    if (non_finite(period.limit, period.limit_L2, period.limit_L3, period.dischargeLimit, period.dischargeLimit_L2,
                   period.dischargeLimit_L3, period.setpoint, period.setpoint_L2, period.setpoint_L3,
                   period.setpointReactive, period.setpointReactive_L2, period.setpointReactive_L3,
                   period.v2xBaseline)) {
        return true;
    }
    if (period.v2xFreqWattCurve.has_value()) {
        for (const auto& point : period.v2xFreqWattCurve.value()) {
            if (non_finite(point.frequency, point.power)) {
                return true;
            }
        }
    }
    if (period.v2xSignalWattCurve.has_value()) {
        for (const auto& point : period.v2xSignalWattCurve.value()) {
            if (non_finite(point.power)) {
                return true;
            }
        }
    }
    return false;
}

/// \brief Returns true if any float field in \p schedule is non-finite (infinity or NaN).
bool has_non_finite_float(const ChargingSchedule& schedule) {
    if (non_finite(schedule.minChargingRate, schedule.powerTolerance)) {
        return true;
    }
    return schedule.limitAtSoC.has_value() && non_finite(schedule.limitAtSoC.value().limit);
}

/// \brief Result of the K15.FR.09 / K16.FR.04 / K18.FR.09 boundary check: validity flag plus a
/// human-readable reason (empty on success) used only for EVLOG output.
struct BoundaryCheckResult {
    bool valid{true};
    std::string reason;
};

/// \brief Pick the CSMS-sent schedule to compare against when the EV returned an id that may or
/// may not match one of the cached schedules. Direct id equality first; if that misses, try the
/// SAScheduleTupleID bias that EvseV2G applies to cap OCPP schedule ids into 1..255 on the iso2
/// wire (`((id - 1) % 255) + 1`). Cached schedules whose original id > 255 get biased on the wire
/// and the EV echoes back the biased value — a raw compare would silently fall back to front()
/// and bounds-check the EV against the wrong tuple on multi-tuple handoffs. Final fallback is
/// front() so the bound is still enforced — the EV is not allowed to pick an id outside the CSMS
/// envelope.
/// Precondition: \p csms_schedules must be non-empty (the caller handles the empty case).
const ChargingSchedule& pick_csms_reference(const ChargingSchedule& ev_schedule,
                                            const std::vector<ChargingSchedule>& csms_schedules) {
    for (const auto& c : csms_schedules) {
        if (c.id == ev_schedule.id) {
            return c;
        }
    }
    // SAScheduleTupleID must be 1..255 per [V2G2-773]; EvseV2G maps OCPP ids into that range.
    auto biased = [](std::int32_t id) -> std::int32_t { return id > 0 ? ((id - 1) % 255) + 1 : id; };
    const auto ev_biased = biased(ev_schedule.id);
    for (const auto& c : csms_schedules) {
        if (biased(c.id) == ev_biased) {
            return c;
        }
    }
    return csms_schedules.front();
}

/// \brief Locate the CSMS schedule period covering the EV period starting at \p start_period.
/// Returns nullptr if no such period exists (start past the end of the CSMS schedule). The CSMS
/// periods are sorted by startPeriod (same convention as the rest of libocpp composite logic).
const ChargingSchedulePeriod* find_csms_period_at(std::int32_t start_period,
                                                  const std::vector<ChargingSchedulePeriod>& csms_periods) {
    const ChargingSchedulePeriod* match = nullptr;
    for (const auto& p : csms_periods) {
        if (p.startPeriod <= start_period) {
            match = &p;
        } else {
            break;
        }
    }
    return match;
}

/// \brief K15.FR.09 / K16.FR.04 / K18.FR.09: verify the EV-returned ChargingSchedule is within
/// the boundaries of the CSMS-sent composite schedule.
///
/// Empty \p csms_schedules (no cached HLC handoff on this EVSE) returns valid — no envelope to
/// enforce. Otherwise the check covers:
///   - chargingRateUnit must match
///   - each EV period's limit must not exceed the CSMS limit at the same time offset
///   - limit sign must match (no reverse direction when CSMS was unidirectional positive)
///   - numberPhases must match when both sides specify it
///   - the EV schedule must not extend past the CSMS schedule's duration window
BoundaryCheckResult verify_ev_profile_within_boundaries(const ChargingSchedule& ev_schedule,
                                                        const std::vector<ChargingSchedule>& csms_schedules) {
    if (csms_schedules.empty()) {
        return {true, {}};
    }

    const ChargingSchedule& csms = pick_csms_reference(ev_schedule, csms_schedules);

    if (ev_schedule.chargingRateUnit != csms.chargingRateUnit) {
        return {false, "chargingRateUnit mismatch"};
    }

    // Time window: if the CSMS schedule has an explicit duration, the EV schedule must fit.
    if (csms.duration.has_value()) {
        const auto csms_end = csms.duration.value();
        std::int32_t ev_end = 0;
        if (ev_schedule.duration.has_value()) {
            ev_end = ev_schedule.duration.value();
        } else if (not ev_schedule.chargingSchedulePeriod.empty()) {
            ev_end = ev_schedule.chargingSchedulePeriod.back().startPeriod;
        }
        if (ev_end > csms_end) {
            return {false, "schedule exceeds CSMS time window"};
        }
    }

    for (const auto& ev_period : ev_schedule.chargingSchedulePeriod) {
        const auto* csms_period = find_csms_period_at(ev_period.startPeriod, csms.chargingSchedulePeriod);
        if (csms_period == nullptr) {
            return {false, "no CSMS period covers EV startPeriod"};
        }

        if (ev_period.limit.has_value() and csms_period->limit.has_value()) {
            const auto ev_limit = ev_period.limit.value();
            const auto csms_limit = csms_period->limit.value();
            // V2X: a negative csms_limit is a discharge envelope. An EV flipping direction
            // (charging while CSMS said discharge-only or vice-versa) is a sign violation.
            const bool csms_allows_charge = csms_limit > 0.0f;
            const bool csms_allows_discharge = csms_limit < 0.0f;
            const bool ev_is_charge = ev_limit > 0.0f;
            const bool ev_is_discharge = ev_limit < 0.0f;
            if ((csms_allows_charge and ev_is_discharge) or (csms_allows_discharge and ev_is_charge)) {
                return {false, "limit sign invalid"};
            }
            // Magnitude check: regardless of sign, |ev| must not exceed |csms|. For the
            // discharge envelope (csms_limit=-16 A, ev_limit=-5 A) the EV's smaller-magnitude
            // discharge is inside the bound and MUST NOT trigger renegotiation.
            if (std::abs(ev_limit) > std::abs(csms_limit)) {
                return {false, "limit exceeds CSMS bound"};
            }
        }

        if (ev_period.numberPhases.has_value() and csms_period->numberPhases.has_value() and
            ev_period.numberPhases.value() != csms_period->numberPhases.value()) {
            return {false, "numberPhases mismatch"};
        }
    }

    return {true, {}};
}

/// \brief Materiality check for two ChargingSchedule vectors, used by K16 change detection.
/// Falls back to JSON serialization — which is already the wire-level equality for OCPP — so
/// every field that affects EV behavior (chargingRateUnit, periods, limits, phases, durations,
/// startSchedule, minChargingRate, salesTariff body, etc.) is compared without hand-rolling a
/// struct-walker that would need to be kept in sync with ocpp_types.hpp.
bool schedules_materially_equal(const std::vector<ChargingSchedule>& a, const std::vector<ChargingSchedule>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (json(a[i]) != json(b[i])) {
            return false;
        }
    }
    return true;
}
} // namespace
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
    case ProfileValidationResultEnum::ChargingScheduleNonFiniteValue:
        return "ChargingScheduleNonFiniteValue";
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
    case ProfileValidationResultEnum::ChargingSchedulePeriodNonFiniteValue:
        return "ChargingSchedulePeriodNonFiniteValue";
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
    case ProfileValidationResultEnum::ChargingScheduleNonFiniteValue:
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
    case ProfileValidationResultEnum::ChargingSchedulePeriodNonFiniteValue:
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

namespace {
constexpr std::size_t MAX_SA_SCHEDULE_TUPLES = 3;

// K15.FR.22 composition helper: higher stackLevel wins for overlapping effects, so we sort
// descending and pick the highest stack level available at each slot as the base schedule.
struct StackLevelDesc {
    bool operator()(const ChargingProfile* a, const ChargingProfile* b) const {
        return a->stackLevel > b->stackLevel;
    }
};
} // namespace

std::vector<SaScheduleSlot> compute_sa_schedule_list(const std::vector<ChargingProfile>& all_profiles,
                                                     const std::string& transaction_id) {
    std::vector<const ChargingProfile*> tx_profiles;
    for (const auto& profile : all_profiles) {
        if (profile.chargingProfilePurpose != ChargingProfilePurposeEnum::TxProfile) {
            continue;
        }
        if (not profile.transactionId.has_value() or profile.transactionId.value().get() != transaction_id) {
            continue;
        }
        tx_profiles.push_back(&profile);
    }
    if (tx_profiles.empty()) {
        return {};
    }

    std::sort(tx_profiles.begin(), tx_profiles.end(), StackLevelDesc{});

    std::vector<SaScheduleSlot> slots;
    slots.reserve(MAX_SA_SCHEDULE_TUPLES);
    for (std::size_t k = 0; k < MAX_SA_SCHEDULE_TUPLES; ++k) {
        std::optional<ChargingSchedule> base;
        std::optional<SalesTariff> tariff;
        for (const auto* profile : tx_profiles) {
            if (k >= profile->chargingSchedule.size()) {
                continue;
            }
            const auto& schedule_k = profile->chargingSchedule[k];
            if (not base.has_value()) {
                // Highest-stackLevel schedule at this slot becomes the base; preserve its
                // SalesTariff. Period merging across stack levels is deferred to the full
                // composite-schedule engine; for K15 handoff the highest-stackLevel body is
                // used as-is, matching K01.FR.01 precedence.
                base = schedule_k;
                tariff = schedule_k.salesTariff;
            }
        }
        if (base.has_value()) {
            SaScheduleSlot slot;
            slot.schedule = base.value();
            slot.sales_tariff = tariff;
            slot.signature_value_b64 = std::nullopt; // 2.1 signature is carried separately.
            slots.push_back(std::move(slot));
        }
    }

    return slots;
}

SmartCharging::SmartCharging(
    const FunctionalBlockContext& functional_block_context, std::function<void()> set_charging_profiles_callback,
    StopTransactionCallback stop_transaction_callback,
    std::optional<NotifyEVChargingNeedsResponseCallback> notify_ev_charging_needs_response_callback,
    std::optional<TransferEVChargingSchedulesCallback> transfer_ev_charging_schedules_callback,
    std::optional<ScheduleRenegotiationCallback> trigger_schedule_renegotiation_callback) :
    context(functional_block_context),
    set_charging_profiles_callback(set_charging_profiles_callback),
    stop_transaction_callback(stop_transaction_callback),
    notify_ev_charging_needs_response_callback(std::move(notify_ev_charging_needs_response_callback)),
    transfer_ev_charging_schedules_callback(std::move(transfer_ev_charging_schedules_callback)),
    trigger_schedule_renegotiation_callback(std::move(trigger_schedule_renegotiation_callback)) {
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

std::optional<CompositeSchedule>
SmartCharging::get_composite_schedule(std::int32_t evse_id, std::chrono::seconds duration, ChargingRateUnitEnum unit) {
    GetCompositeScheduleRequest request;
    request.duration = clamp_to<std::int32_t>(duration.count());
    request.evseId = evse_id;
    request.chargingRateUnit = unit;

    auto composite_schedule_response = this->get_composite_schedule_internal(request, false);
    if (composite_schedule_response.status == GenericStatusEnum::Accepted) {
        return composite_schedule_response.schedule;
    }
    return std::nullopt;
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

std::pair<bool, bool> SmartCharging::validate_profile_with_offline_time(const ChargingProfile& profile) {
    const auto time_disconnected = this->context.connectivity_manager.get_time_disconnected();
    // Being online means the profile is valid
    if (time_disconnected.time_since_epoch() == 0s) {
        return {true, false};
    }

    // Absent maxOfflineDuration means the profile is valid independent of the offline time
    if (!profile.maxOfflineDuration.has_value()) {
        return {true, false};
    }

    // Not being offline for long enough means profile is valid
    if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - time_disconnected)
            .count() <= profile.maxOfflineDuration.value()) {
        return {true, false};
    }

    // Profile must be cleared when we are offline for too long and invalidAfterOfflineDuration is set
    return {false, profile.invalidAfterOfflineDuration.value_or(false)};
}

bool SmartCharging::has_dc_input_phase_control(const std::int32_t evse_id) const {
    if (evse_id == 0) {
        for (EvseManagerInterface::EvseIterator it = context.evse_manager.begin(); it != context.evse_manager.end();
             ++it) {
            const std::int32_t id = (*it).get_id();
            if (!evse_has_dc_input_phase_control(id)) {
                return false;
            }
        }

        return true;
    }
    return evse_has_dc_input_phase_control(evse_id);
}

bool SmartCharging::evse_has_dc_input_phase_control(const std::int32_t evse_id) const {
    const ComponentVariable evse_variable =
        EvseComponentVariables::get_component_variable(evse_id, EvseComponentVariables::DCInputPhaseControl);
    return this->context.device_model.get_optional_value<bool>(evse_variable).value_or(false);
}

std::vector<CompositeSchedule> SmartCharging::get_all_composite_schedules(const std::int32_t duration_s,
                                                                          const ChargingRateUnitEnum& unit) {
    std::vector<CompositeSchedule> composite_schedules;

    const auto number_of_evses = this->context.evse_manager.get_number_of_evses();
    // get all composite schedules including the one for evse_id == 0
    for (std::int32_t evse_id = 0; evse_id <= number_of_evses; evse_id++) {
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
    // K16.FR.02 / FR.11: look up the evse that owned this tx BEFORE removing the profiles, so
    // we can invalidate the cached HLC handoff snapshot for that evse. If the tx is already
    // gone from the evse_manager (e.g. previously closed), fall back to clearing all entries
    // so a stale cache cannot outlive the session it came from.
    const auto evse_id_opt = this->context.evse_manager.get_transaction_evseid(transaction_id);
    this->context.database_handler.delete_charging_profile_by_transaction_id(transaction_id);
    {
        auto cache = this->last_handed_off_schedules.handle();
        if (evse_id_opt.has_value()) {
            cache->erase(evse_id_opt.value());
            EVLOG_debug << "K16: invalidated cached schedule for evse " << evse_id_opt.value() << " on tx end";
        } else {
            cache->clear();
            EVLOG_debug << "K16: cleared all cached schedules on tx end (evse lookup missed for " << transaction_id
                        << ")";
        }
    }

    // Evict the per-tx HLC coordination entry (and its armed 60 s timer) so a dangling timer
    // cannot fire into a later session on the same EVSE. Move the timer out under the lock,
    // then destroy it OUTSIDE the lock: ~SteadyTimer joins its worker thread, which may be
    // parked on ev_schedule_state.handle() in its own callback (smart_charging.cpp ~:1386).
    std::unique_ptr<Everest::SteadyTimer> expiring_timer;
    {
        auto handle = this->ev_schedule_state.handle();
        auto it = handle->find(transaction_id);
        if (it != handle->end()) {
            expiring_timer = std::move(it->second.timeout_timer);
            handle->erase(it);
        }
    }
    // expiring_timer (if any) goes out of scope here — ~Timer joins with no monitor held.
}

SetChargingProfileResponse SmartCharging::conform_validate_and_add_profile(ChargingProfile& profile,
                                                                           std::int32_t evse_id,
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

ProfileValidationResultEnum SmartCharging::conform_and_validate_profile(ChargingProfile& profile, std::int32_t evse_id,
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
        result = validate_charging_station_max_profile(profile, evse_id);
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
    std::int32_t default_number_phases{};
    float supply_voltage{};

    CompositeScheduleConfig(DeviceModelAbstract& device_model, bool is_offline) :
        purposes_to_ignore{utils::get_purposes_to_ignore(
            device_model.get_optional_value<std::string>(ControllerComponentVariables::IgnoredProfilePurposesOffline)
                .value_or(""),
            is_offline)} {

        this->current_limit = static_cast<float>(
            device_model.get_optional_value<int>(ControllerComponentVariables::CompositeScheduleDefaultLimitAmps)
                .value_or(DEFAULT_LIMIT_AMPS));

        this->power_limit = static_cast<float>(
            device_model.get_optional_value<int>(ControllerComponentVariables::CompositeScheduleDefaultLimitWatts)
                .value_or(DEFAULT_LIMIT_WATTS));

        this->default_number_phases =
            device_model.get_optional_value<int>(ControllerComponentVariables::CompositeScheduleDefaultNumberPhases)
                .value_or(DEFAULT_AND_MAX_NUMBER_PHASES);

        this->supply_voltage = static_cast<float>(
            device_model.get_optional_value<int>(ControllerComponentVariables::SupplyVoltage).value_or(LOW_VOLTAGE));
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

CompositeSchedule SmartCharging::calculate_composite_schedule(const ocpp::DateTime& start_t,
                                                              const ocpp::DateTime& end_time,
                                                              const std::int32_t evse_id,
                                                              ChargingRateUnitEnum charging_rate_unit, bool is_offline,
                                                              bool simulate_transaction_active) {

    // handle edge case where start_time > end_time
    auto start_time = start_t;
    if (start_time > end_time) {
        start_time = end_time;
    }

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

ProfileValidationResultEnum SmartCharging::validate_evse_exists(std::int32_t evse_id) const {
    return this->context.evse_manager.does_evse_exist(evse_id) ? ProfileValidationResultEnum::Valid
                                                               : ProfileValidationResultEnum::EvseDoesNotExist;
}

ProfileValidationResultEnum validate_charging_station_max_profile(const ChargingProfile& profile,
                                                                  std::int32_t evse_id) {
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
                                                                       std::int32_t evse_id) const {
    auto profiles = evse_id == 0 ? get_evse_specific_tx_default_profiles() : get_station_wide_tx_default_profiles();

    // K01.FR.53
    for (const auto& candidate : profiles) {
        if (candidate.stackLevel == profile.stackLevel) {
            if (candidate.id != profile.id) {
                return ProfileValidationResultEnum::DuplicateTxDefaultProfileFound;
            }
        }
    }

    return ProfileValidationResultEnum::Valid;
}

// FIXME: See OCPP2.1 spec: 3.8 Avoiding Phase Conflicts
ProfileValidationResultEnum SmartCharging::validate_tx_profile(const ChargingProfile& profile, std::int32_t evse_id,
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
                                                                              std::int32_t /*evse_id*/) const {
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
        this->context.device_model.get_value<std::int32_t>(ControllerComponentVariables::ChargingStationSupplyPhases);

    auto phase_type = this->get_current_phase_type(evse_opt);

    for (auto& schedule : profile.chargingSchedule) {
        // K01.FR.26; We currently need to do string conversions for this manually because our DeviceModel class
        // does not let us get a vector of ChargingScheduleChargingRateUnits.
        auto supported_charging_rate_units = this->context.device_model.get_value<std::string>(
            ControllerComponentVariables::ChargingScheduleChargingRateUnit);
        if (supported_charging_rate_units.find(
                conversions::charging_rate_unit_enum_to_string(schedule.chargingRateUnit)) == std::string::npos) {
            return ProfileValidationResultEnum::ChargingScheduleChargingRateUnitUnsupported;
        }

        // A schedule must have at least one chargingSchedulePeriod
        if (schedule.chargingSchedulePeriod.empty()) {
            return ProfileValidationResultEnum::ChargingProfileNoChargingSchedulePeriods;
        }

        if (has_non_finite_float(schedule)) {
            return ProfileValidationResultEnum::ChargingScheduleNonFiniteValue;
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

            if (has_non_finite_float(charging_schedule_period)) {
                return ProfileValidationResultEnum::ChargingSchedulePeriodNonFiniteValue;
            }

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
                }
                if (this->context.ocpp_version == OcppProtocolVersion::v21) {
                    const std::int32_t evse_id = evse_opt.has_value() ? evse_opt.value()->get_id() : 0;
                    if (!this->has_dc_input_phase_control(evse_id)) {
                        // If 2.1 and DCInputPhaseControl is false or does not exist, then send rejected with reason
                        // code noPhaseForDC
                        // K01.FR.44
                        return ProfileValidationResultEnum::ChargingSchedulePeriodNoPhaseForDC;
                    }
                    // K01.FR.54
                    // TODO(mlitre): How to notify that this should be used for AC grid connection?
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
        }
        if (profile.chargingProfileKind == ChargingProfileKindEnum::Relative && schedule.startSchedule.has_value()) {
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

SetChargingProfileResponse SmartCharging::add_profile(ChargingProfile& profile, std::int32_t evse_id,
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
SmartCharging::get_valid_profiles(std::int32_t evse_id,
                                  const std::vector<ChargingProfilePurposeEnum>& purposes_to_ignore) {
    std::vector<ChargingProfile> valid_profiles = get_valid_profiles_for_evse(evse_id, purposes_to_ignore);

    if (evse_id != STATION_WIDE_ID) {
        auto station_wide_profiles = get_valid_profiles_for_evse(STATION_WIDE_ID, purposes_to_ignore);
        valid_profiles.insert(valid_profiles.end(), station_wide_profiles.begin(), station_wide_profiles.end());
    }

    return valid_profiles;
}

void SmartCharging::report_charging_profile_req(const std::int32_t request_id, const std::int32_t evse_id,
                                                const CiString<20> source, const std::vector<ChargingProfile>& profiles,
                                                const bool tbc) {
    ReportChargingProfilesRequest req;
    req.requestId = request_id;
    req.evseId = evse_id;
    req.chargingLimitSource = source;
    req.chargingProfile = profiles;
    req.tbc = tbc;

    const ocpp::Call<ReportChargingProfilesRequest> call(req);
    this->context.message_dispatcher.dispatch_call(call);
}

void SmartCharging::report_charging_profile_req(const ReportChargingProfilesRequest& req) {
    const ocpp::Call<ReportChargingProfilesRequest> call(req);
    this->context.message_dispatcher.dispatch_call(call);
}

void SmartCharging::notify_ev_charging_needs_req(const NotifyEVChargingNeedsRequest& req) {
    NotifyEVChargingNeedsRequest request = req;
    if (this->context.ocpp_version != OcppProtocolVersion::v21) {
        request.timestamp = std::nullopt; // field is not present in OCPP2.0.1
    }

    const ocpp::Call<NotifyEVChargingNeedsRequest> call(request);
    this->context.message_dispatcher.dispatch_call(call);

    // Track the HLC session so we can gate the ISO 15118 stack on the CSMS decision
    // (K15.FR.03/04/05) and enforce the 60-second ChargeParameterDiscoveryRes budget (K15.FR.08).
    const auto& tx = this->context.evse_manager.get_evse(request.evseId).get_transaction();
    if (tx == nullptr) {
        return; // No active transaction on this EVSE; no scheduling handoff to track.
    }
    const std::string transaction_id = tx->transactionId.get();

    auto state = EvScheduleState{};
    state.evse_id = request.evseId;
    state.timeout_timer = std::make_unique<Everest::SteadyTimer>();

    // Destroy any prior entry's timer OUTSIDE the monitor lock: ~SteadyTimer joins its worker
    // thread, and that worker may be parked on this->ev_schedule_state.handle(). Destroying
    // under the lock would deadlock. Arm the new timer AFTER the insertion so its callback can
    // never fire into an empty map — safe at 60 s today, but a mandatory invariant for any
    // future timeout shortening.
    std::unique_ptr<Everest::SteadyTimer> expiring_timer;
    {
        auto handle = this->ev_schedule_state.handle();
        auto existing = handle->find(transaction_id);
        if (existing != handle->end()) {
            expiring_timer = std::move(existing->second.timeout_timer);
        }
        auto [it, inserted] = handle->insert_or_assign(transaction_id, std::move(state));
        it->second.timeout_timer->timeout(
            [this, transaction_id]() {
                // 60-second timeout: surface NoChargingProfile to the ISO 15118 stack so it falls
                // back to a default/unlimited schedule (K15.FR.04, K15.FR.08).
                std::int32_t evse_id = 0;
                bool fire = false;
                {
                    auto handle = this->ev_schedule_state.handle();
                    auto it = handle->find(transaction_id);
                    if (it != handle->end() and not it->second.response_received) {
                        evse_id = it->second.evse_id;
                        fire = true;
                        handle->erase(it);
                    }
                }
                if (fire and this->notify_ev_charging_needs_response_callback.has_value()) {
                    this->notify_ev_charging_needs_response_callback.value()(
                        evse_id, NotifyEVChargingNeedsStatusEnum::NoChargingProfile);
                }
            },
            EV_SCHEDULE_TIMEOUT);
    }
    // expiring_timer (if any) goes out of scope here — ~Timer joins with no monitor held.
}

void SmartCharging::notify_ev_charging_schedule_req(std::int32_t evse_id, std::int32_t sa_schedule_tuple_id,
                                                    std::optional<std::int32_t> selected_charging_schedule_id,
                                                    const std::optional<ChargingSchedule>& ev_charging_schedule) {
    NotifyEVChargingScheduleRequest request;
    request.evseId = evse_id;
    request.timeBase = ocpp::DateTime{};

    if (ev_charging_schedule.has_value()) {
        request.chargingSchedule = ev_charging_schedule.value();
    } else {
        // K15.FR.19: EV did not return a profile; the CS SHOULD echo a schedule that matches the
        // SAScheduleTuple the EV selected. Caller has no EV-side data, so synthesize a minimal
        // well-formed schedule: one period starting at offset 0 with a zero limit (the OCPP JSON
        // schema requires chargingSchedulePeriod minItems=1; an empty array would be rejected).
        // The warning surfaces the substitution for the CSMS operator.
        EVLOG_warning << "NotifyEVChargingSchedule: no EV-side schedule on evse " << evse_id
                      << "; synthesizing empty-period fallback";
        request.chargingSchedule.chargingRateUnit = ChargingRateUnitEnum::A;
        ChargingSchedulePeriod placeholder{};
        placeholder.startPeriod = 0;
        placeholder.limit = 0;
        request.chargingSchedule.chargingSchedulePeriod.push_back(placeholder);
    }
    // Tie the reported schedule id to the tuple the EV picked, as recommended by K15.FR.19.
    request.chargingSchedule.id = sa_schedule_tuple_id;

    if (this->context.ocpp_version == OcppProtocolVersion::v21) {
        // K15.FR.21 (OCPP 2.1 only).
        request.selectedChargingScheduleId = selected_charging_schedule_id;
    }

    // K15.FR.09 / K16.FR.04 / K18.FR.09: verify the EV-returned schedule is within the bounds of
    // the CSMS-sent composite schedule cached at handoff time. If it is not, fire the
    // renegotiation callback so the CS asks the EV to renegotiate against a compliant envelope.
    // The NotifyEVChargingSchedule message is still dispatched upstream — the CSMS needs to see
    // the EV proposal regardless.
    if (ev_charging_schedule.has_value()) {
        std::vector<ChargingSchedule> csms_cached;
        {
            auto cache = this->last_handed_off_schedules.handle();
            auto it = cache->find(evse_id);
            if (it != cache->end()) {
                csms_cached = it->second;
            }
        }
        const auto check = verify_ev_profile_within_boundaries(ev_charging_schedule.value(), csms_cached);
        if (not check.valid) {
            EVLOG_warning << "K15.FR.09: EV schedule out of bounds on evse " << evse_id << ": " << check.reason
                          << "; firing renegotiation";
            if (this->trigger_schedule_renegotiation_callback.has_value()) {
                this->trigger_schedule_renegotiation_callback.value()(evse_id);
            }
        }
    }

    const ocpp::Call<NotifyEVChargingScheduleRequest> call(request);
    this->context.message_dispatcher.dispatch_call(call);
}

void SmartCharging::handle_set_charging_profile_req(Call<SetChargingProfileRequest> call) {
    EVLOG_debug << "Received SetChargingProfileRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    auto msg = call.msg;
    SetChargingProfileResponse response;
    response.status = ChargingProfileStatusEnum::Rejected;

    // K01.FR.29: Respond with a CallError if SmartCharging is not available for this Charging Station
    const bool is_smart_charging_available =
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

        const ocpp::CallResult<SetChargingProfileResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);

        return;
    }

    // K15.FR.17: a TxProfile arriving before the CS has sent NotifyEVChargingNeeds for this
    // transaction SHOULD be rejected with InvalidMessageSeq so the CSMS resends after the
    // request. We only apply this rule on HLC-capable builds (callback wired) and only for a
    // genuinely fresh tx (no prior TxProfile accepted and stored for this transactionId), so
    // K16 renegotiations still pass through.
    if (this->notify_ev_charging_needs_response_callback.has_value() and
        msg.chargingProfile.chargingProfilePurpose == ChargingProfilePurposeEnum::TxProfile and
        msg.chargingProfile.transactionId.has_value()) {
        const std::string tx_id = msg.chargingProfile.transactionId.value().get();
        bool notify_already_sent = false;
        {
            auto handle = this->ev_schedule_state.handle();
            notify_already_sent = (handle->find(tx_id) != handle->end());
        }
        if (not notify_already_sent) {
            const auto existing = this->context.database_handler.get_charging_profiles_for_evse(msg.evseId);
            const bool has_prior_tx_profile = std::any_of(existing.begin(), existing.end(), [&tx_id](const auto& p) {
                return p.chargingProfilePurpose == ChargingProfilePurposeEnum::TxProfile and
                       p.transactionId.has_value() and p.transactionId.value().get() == tx_id;
            });
            if (not has_prior_tx_profile) {
                response.status = ChargingProfileStatusEnum::Rejected;
                response.statusInfo = StatusInfo();
                // K18.FR.17 and K19.FR.13 prescribe "InvalidMessageSequence" (22 chars) for the
                // 15118-20 rejection path, but StatusInfoType.reasonCode is CiString<20> per the
                // OCPP 2.1 Ed.2 JSON schema — the full word overflows at runtime. K15.FR.17's
                // "InvalidMessageSeq" is the only value that fits the schema, so we emit it for
                // both 15118-2 and 15118-20; the K18/K19 deviation is a documented spec erratum.
                response.statusInfo->reasonCode = "InvalidMessageSeq";
                const ocpp::CallResult<SetChargingProfileResponse> call_result(response, call.uniqueId);
                this->context.message_dispatcher.dispatch_call_result(call_result);
                return;
            }
        }
    }

    response = this->conform_validate_and_add_profile(msg.chargingProfile, msg.evseId);
    if (response.status == ChargingProfileStatusEnum::Accepted) {
        EVLOG_debug << "Accepting SetChargingProfileRequest";
        this->set_charging_profiles_callback();

        // K15.FR.07: if this TxProfile matches an HLC transaction awaiting a schedule, compute
        // composite schedule(s) and forward them to the ISO 15118 stack via callback.
        const bool is_tx_profile = msg.chargingProfile.chargingProfilePurpose == ChargingProfilePurposeEnum::TxProfile;
        if (is_tx_profile and msg.chargingProfile.transactionId.has_value() and
            this->transfer_ev_charging_schedules_callback.has_value()) {
            const std::string tx_id = msg.chargingProfile.transactionId.value().get();
            std::int32_t evse_id = 0;
            bool awaiting = false;
            // Move the timer out of the map under the lock, erase, release the handle,
            // then call stop() and let the unique_ptr destruct — ~SteadyTimer joins its
            // worker thread, which may itself be waiting on this monitor in the timeout
            // callback (smart_charging.cpp:1177-1194). Destroying under the lock deadlocks.
            std::unique_ptr<Everest::SteadyTimer> expiring_timer;
            {
                auto handle = this->ev_schedule_state.handle();
                auto it = handle->find(tx_id);
                if (it != handle->end()) {
                    awaiting = true;
                    evse_id = it->second.evse_id;
                    expiring_timer = std::move(it->second.timeout_timer);
                    handle->erase(it);
                }
            }
            if (expiring_timer) {
                expiring_timer->stop();
            }
            // expiring_timer destructs here — ~Timer joins with no monitor held.
            if (awaiting) {
                // K15.FR.22: compose up to three ChargingSchedules by combining schedules from
                // all TxProfiles installed for this transaction across stackLevels. For a single
                // TxProfile this degenerates to its own schedules (K15.FR.18 recommended shape).
                const auto all_profiles = this->context.database_handler.get_charging_profiles_for_evse(evse_id);
                const auto slots = compute_sa_schedule_list(all_profiles, tx_id);

                std::vector<ChargingSchedule> schedules;
                std::vector<std::optional<SalesTariff>> tariffs;
                std::vector<std::optional<std::string>> signature_value_b64;
                schedules.reserve(slots.size());
                tariffs.reserve(slots.size());
                signature_value_b64.reserve(slots.size());
                for (const auto& slot : slots) {
                    schedules.push_back(slot.schedule);
                    tariffs.push_back(slot.sales_tariff);
                    // OCPP 2.1 Ed.2 adds ChargingProfileType.signatureValue for signed
                    // SalesTariffs; libocpp does not model it yet, so passthrough is nullopt.
                    signature_value_b64.push_back(slot.signature_value_b64);
                }
                this->transfer_ev_charging_schedules_callback.value()(evse_id, tx_id, schedules, tariffs,
                                                                      signature_value_b64, std::nullopt);
                // K16.FR.02 / FR.11: cache the handed-off schedule so a subsequent
                // SetChargingProfile can detect whether it materially changes the composite and
                // trigger a renegotiation instead of a fresh handoff.
                auto cache = this->last_handed_off_schedules.handle();
                (*cache)[evse_id] = std::move(schedules);
                EVLOG_debug << "K16: cached HLC handoff schedule for evse " << evse_id;
            } else {
                // K16.FR.02 / FR.11: TxProfile for a tx not currently awaiting handoff — if this
                // EVSE had a prior handoff (cache entry present), recompute the composite and
                // fire the renegotiation callback when it differs materially from the cache.
                std::vector<ChargingSchedule> cached;
                bool have_prior = false;
                {
                    auto cache = this->last_handed_off_schedules.handle();
                    auto it = cache->find(msg.evseId);
                    if (it != cache->end()) {
                        cached = it->second;
                        have_prior = true;
                    }
                }
                if (have_prior and this->trigger_schedule_renegotiation_callback.has_value()) {
                    const auto all_profiles = this->context.database_handler.get_charging_profiles_for_evse(msg.evseId);
                    const auto slots = compute_sa_schedule_list(all_profiles, tx_id);
                    std::vector<ChargingSchedule> recomputed;
                    std::vector<std::optional<SalesTariff>> tariffs;
                    std::vector<std::optional<std::string>> signature_value_b64;
                    recomputed.reserve(slots.size());
                    tariffs.reserve(slots.size());
                    signature_value_b64.reserve(slots.size());
                    for (const auto& slot : slots) {
                        recomputed.push_back(slot.schedule);
                        tariffs.push_back(slot.sales_tariff);
                        signature_value_b64.push_back(slot.signature_value_b64);
                    }
                    if (not schedules_materially_equal(cached, recomputed)) {
                        // K16.FR.03: re-deliver the recomputed composite schedule to the ISO stack
                        // BEFORE firing the renegotiation trigger, so the EV sees the fresh bundle
                        // when it re-enters ChargeParameterDiscovery in response to the ReNegotiation
                        // latch. Without this, evse_sa_schedule_list still holds the prior tuples.
                        if (this->transfer_ev_charging_schedules_callback.has_value()) {
                            this->transfer_ev_charging_schedules_callback.value()(
                                msg.evseId, tx_id, recomputed, tariffs, signature_value_b64, std::nullopt);
                        }
                        {
                            auto cache = this->last_handed_off_schedules.handle();
                            (*cache)[msg.evseId] = recomputed;
                        }
                        EVLOG_info << "K16: firing schedule renegotiation for evse " << msg.evseId;
                        this->trigger_schedule_renegotiation_callback.value()(msg.evseId);
                    }
                }
            }
        }
    } else {
        std::string reason_code = "Unspecified";
        std::string additional_info = "Unknown";
        if (response.statusInfo.has_value()) {
            const auto& status_info = response.statusInfo.value();
            reason_code = status_info.reasonCode.get();
            if (status_info.additionalInfo.has_value()) {
                additional_info = status_info.additionalInfo.value().get();
            }
        }
        EVLOG_debug << "Rejecting SetChargingProfileRequest:\n reasonCode: " << reason_code
                    << "\nadditionalInfo: " << additional_info;
    }

    const ocpp::CallResult<SetChargingProfileResponse> call_result(response, call.uniqueId);
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
        // K16.FR.02 / FR.11: clearing profiles invalidates any cached HLC handoff snapshot so a
        // subsequent SetChargingProfile does not spuriously trigger a renegotiation against
        // stale schedules. If the request had no evseId criterion, clear all entries.
        const bool scoped_to_evse =
            msg.chargingProfileCriteria.has_value() and msg.chargingProfileCriteria.value().evseId.has_value();
        auto cache = this->last_handed_off_schedules.handle();
        if (scoped_to_evse) {
            const auto cleared_evse = msg.chargingProfileCriteria.value().evseId.value();
            cache->erase(cleared_evse);
            EVLOG_debug << "K16: invalidated cached schedule for evse " << cleared_evse << " on profile clear";
        } else {
            cache->clear();
            EVLOG_debug << "K16: cleared all cached schedules on unscoped profile clear";
        }
    }

    const ocpp::CallResult<ClearChargingProfileResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

void SmartCharging::handle_get_charging_profiles_req(Call<GetChargingProfilesRequest> call) {
    EVLOG_debug << "Received GetChargingProfilesRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    const auto msg = call.msg;
    GetChargingProfilesResponse response;

    const auto profiles_to_report = this->get_reported_profiles(msg);

    response.status =
        profiles_to_report.empty() ? GetChargingProfileStatusEnum::NoProfiles : GetChargingProfileStatusEnum::Accepted;

    const ocpp::CallResult<GetChargingProfilesResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);

    if (response.status == GetChargingProfileStatusEnum::NoProfiles) {
        return;
    }

    // There are profiles to report.
    // Prepare ReportChargingProfileRequest(s). The message defines the properties evseId and
    // ChargingLimitSourceEnumStringType as required, so we can not report all profiles in a single
    // ReportChargingProfilesRequest. We need to prepare a single ReportChargingProfilesRequest for each combination
    // of evseId and ChargingLimitSourceEnumStringType
    std::set<std::int32_t> evse_ids; // will contain all evse_ids of the profiles
    std::set<CiString<20>> sources;  // will contain all sources of the profiles

    // fill evse_ids and sources sets
    for (const auto& profile : profiles_to_report) {
        evse_ids.insert(profile.evse_id);
        sources.insert(profile.source);
    }

    std::vector<ReportChargingProfilesRequest> requests_to_send;

    for (const auto evse_id : evse_ids) {
        for (const auto& source : sources) {
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
    const Call<NotifyEVChargingNeedsRequest> request = call_result.call_message;
    EVLOG_debug << "Received NotifyEVChargingNeedsResponse: " << response.msg
                << "\nwith messageId: " << response.uniqueId;
    const bool is_15118_20 = request.msg.chargingNeeds.v2xChargingParameters.has_value();
    const auto status = response.msg.status;

    // Mark state as response-received so the 60s timeout won't double-fire the callback.
    // For non-Accepted statuses, clear the entry since the ISO stack will fall through.
    // Move the timer out before erasing and destruct it AFTER releasing the monitor: the
    // timer's worker thread may be waiting on the same monitor in the timeout callback
    // (smart_charging.cpp:1177-1194); destroying under the lock would deadlock ~Timer's join.
    std::unique_ptr<Everest::SteadyTimer> expiring_timer;
    {
        auto handle = this->ev_schedule_state.handle();
        for (auto it = handle->begin(); it != handle->end(); ++it) {
            if (it->second.evse_id == request.msg.evseId) {
                it->second.response_status = status;
                it->second.response_received = true;
                if (status != NotifyEVChargingNeedsStatusEnum::Accepted) {
                    expiring_timer = std::move(it->second.timeout_timer);
                    handle->erase(it);
                }
                break;
            }
        }
    }
    // expiring_timer (if any) destructs here — ~Timer joins with no monitor held.

    switch (status) {
    case NotifyEVChargingNeedsStatusEnum::Accepted:
        // K15.FR.03 - ISO15118-2; K18.FR.03 / K19.FR.03. ISO stack waits for SetChargingProfile.
        if (this->notify_ev_charging_needs_response_callback.has_value()) {
            this->notify_ev_charging_needs_response_callback.value()(request.msg.evseId, status);
        }
        break;
    case NotifyEVChargingNeedsStatusEnum::Rejected:
        // K18.FR.22 - Scheduled Mode; K19.FR.15 - Dynamic Mode.
        if (this->context.ocpp_version == OcppProtocolVersion::v201 or !is_15118_20) {
            // K15.FR.04 - ISO15118-2: start without waiting, equivalent to NoChargingProfile.
            [[fallthrough]];
        } else {
            // V2X under 15118-20: stop the transaction instead of falling through
            // (Q01.FR.06 / K18.FR.23 / K19.FR.16).
            stop_transaction_callback(request.msg.evseId, ReasonEnum::ReqEnergyTransferRejected);
            break;
        }
    case NotifyEVChargingNeedsStatusEnum::Processing:
        // K15.FR.05 - ISO15118-2; K18.FR.05 / K19.FR.05. Late profile triggers renegotiation (K16).
    case NotifyEVChargingNeedsStatusEnum::NoChargingProfile:
        // K18.FR.04 / K19.FR.04. ISO stack falls through to a default/unlimited schedule.
        if (this->notify_ev_charging_needs_response_callback.has_value()) {
            this->notify_ev_charging_needs_response_callback.value()(request.msg.evseId, status);
        }
        break;
    }
}

void SmartCharging::handle_get_composite_schedule_req(Call<GetCompositeScheduleRequest> call) {
    EVLOG_debug << "Received GetCompositeScheduleRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    const auto response = this->get_composite_schedule_internal(call.msg);

    const ocpp::CallResult<GetCompositeScheduleResponse> call_result(response, call.uniqueId);
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
        const bool unit_supported = std::any_of(
            supported_charging_rate_units.begin(), supported_charging_rate_units.end(), [&request](std::string item) {
                return conversions::string_to_charging_rate_unit_enum(item) == request.chargingRateUnit.value();
            });

        if (unit_supported) {
            charging_rate_unit = request.chargingRateUnit;
        }
    } else if (!supported_charging_rate_units.empty()) {
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

bool SmartCharging::is_overlapping_validity_period(const ChargingProfile& candidate_profile,
                                                   std::int32_t candidate_evse_id) const {
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
        const ChargingProfile existing_profile = json::parse(overlap_stmt->column_text(0));
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
        const ChargingProfile profile = json::parse(stmt->column_text(0));
        evse_specific_tx_default_profiles.push_back(profile);
    }

    return evse_specific_tx_default_profiles;
}

std::vector<ChargingProfile> SmartCharging::get_station_wide_tx_default_profiles() const {
    std::vector<ChargingProfile> station_wide_tx_default_profiles;

    auto stmt = this->context.database_handler.new_statement(
        "SELECT PROFILE FROM CHARGING_PROFILES WHERE EVSE_ID = 0 AND CHARGING_PROFILE_PURPOSE = 'TxDefaultProfile'");
    while (stmt->step() != SQLITE_DONE) {
        const ChargingProfile profile = json::parse(stmt->column_text(0));
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
        const ChargingProfile profile = json::parse(stmt->column_text(0));
        charging_station_max_profiles.push_back(profile);
    }

    return charging_station_max_profiles;
}

std::vector<ChargingProfile>
SmartCharging::get_valid_profiles_for_evse(std::int32_t evse_id,
                                           const std::vector<ChargingProfilePurposeEnum>& purposes_to_ignore) {
    std::vector<ChargingProfile> valid_profiles;

    auto evse_profiles = this->context.database_handler.get_charging_profiles_for_evse(evse_id);
    for (auto profile : evse_profiles) {
        // Q11
        if (const auto [valid, clear] = this->validate_profile_with_offline_time(profile); !valid) {
            if (clear) {
                // Q12
                EVLOG_debug << "Clearing profile with ID: " << profile.id
                            << ", because it is invalid after offline duration";
                this->context.database_handler.clear_charging_profiles_matching_criteria(profile.id, std::nullopt);
            }
            continue;
        }

        if (this->conform_and_validate_profile(profile, evse_id) == ProfileValidationResultEnum::Valid and
            std::find(std::begin(purposes_to_ignore), std::end(purposes_to_ignore), profile.chargingProfilePurpose) ==
                std::end(purposes_to_ignore)) {
            valid_profiles.push_back(profile);
        }
    }

    return valid_profiles;
}

CurrentPhaseType SmartCharging::get_current_phase_type(const std::optional<EvseInterface*> evse_opt) const {
    if (evse_opt.has_value()) {
        return evse_opt.value()->get_current_phase_type();
    }

    auto supply_phases =
        this->context.device_model.get_value<std::int32_t>(ControllerComponentVariables::ChargingStationSupplyPhases);
    if (supply_phases == 1 || supply_phases == 3) {
        return CurrentPhaseType::AC;
    }
    if (supply_phases == 0) {
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
        const LimitsSetpointsForOperationMode& limits_setpoints =
            limits_setpoints_per_operation_mode.at(operation_mode);
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
        if ((charging_schedule_period.setpoint.value() > 0.0F &&
             ((charging_schedule_period.setpoint_L2.has_value() &&
               charging_schedule_period.setpoint_L2.value() < 0.0F) ||
              (charging_schedule_period.setpoint_L3.has_value() &&
               charging_schedule_period.setpoint_L3.value() < 0.0F))) ||
            (charging_schedule_period.setpoint.value() < 0.0F &&
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
