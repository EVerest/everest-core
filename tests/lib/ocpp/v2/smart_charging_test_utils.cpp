// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <smart_charging_test_utils.hpp>

#include "ocpp/v2/ctrlr_component_variables.hpp"

namespace ocpp::v2 {

bool operator==(const V2XFreqWattPoint& a, const V2XFreqWattPoint& b) {
    return a.frequency == b.frequency && a.power == b.power;
}

bool operator==(const V2XSignalWattPoint& a, const V2XSignalWattPoint& b) {
    return a.power == b.power and a.signal == b.signal;
}

bool operator==(const ChargingSchedulePeriod& a, const ChargingSchedulePeriod& b) {
    auto diff = std::abs(a.startPeriod - b.startPeriod);
    bool bRes = diff < 10; // allow for a small difference
    bRes = bRes && (a.limit == b.limit);
    bRes = bRes && (a.numberPhases == b.numberPhases);
    bRes = bRes && (a.phaseToUse == b.phaseToUse);
    bRes = bRes && (a.setpoint == b.setpoint);
    bRes = bRes && (a.setpoint_L2 == b.setpoint_L2);
    bRes = bRes && (a.setpoint_L3 == b.setpoint_L3);
    bRes = bRes && (a.setpointReactive == b.setpointReactive);
    bRes = bRes && (a.setpointReactive_L2 == b.setpointReactive_L2);
    bRes = bRes && (a.setpointReactive_L3 == b.setpointReactive_L3);
    bRes = bRes && (a.dischargeLimit == b.dischargeLimit);
    bRes = bRes && (a.dischargeLimit_L2 == b.dischargeLimit_L2);
    bRes = bRes && (a.dischargeLimit_L3 == b.dischargeLimit_L3);
    bRes = bRes && (a.evseSleep == b.evseSleep);
    bRes = bRes && (a.operationMode == b.operationMode);
    bRes = bRes && (a.preconditioningRequest == b.preconditioningRequest);
    bRes = bRes && (a.v2xBaseline == b.v2xBaseline);
    bRes = bRes && (a.v2xFreqWattCurve == b.v2xFreqWattCurve);
    bRes = bRes && (a.v2xSignalWattCurve == b.v2xSignalWattCurve);

    return bRes;
}

bool operator!=(const ChargingSchedulePeriod& a, const ChargingSchedulePeriod& b) {
    return (!(a == b));
}

bool operator==(const CompositeSchedule& a, const CompositeSchedule& b) {
    bool bRes = true;

    if (a.chargingSchedulePeriod.size() != b.chargingSchedulePeriod.size()) {
        return false;
    }

    for (std::uint32_t i = 0; bRes && i < a.chargingSchedulePeriod.size(); i++) {
        bRes = a.chargingSchedulePeriod[i] == b.chargingSchedulePeriod[i];
    }

    bRes = bRes && (a.evseId == b.evseId);
    bRes = bRes && (a.duration == b.duration);
    bRes = bRes && (a.scheduleStart == b.scheduleStart);
    bRes = bRes && (a.chargingRateUnit == b.chargingRateUnit);

    return bRes;
}

bool operator!=(const CompositeSchedule& a, const CompositeSchedule& b) {
    return (!(a == b));
}

bool operator==(const LimitAtSoC& a, const LimitAtSoC& b) {
    return a.limit == b.limit and a.soc == b.soc;
}

bool operator==(const ChargingSchedule& a, const ChargingSchedule& b) {
    bool bRes = true;

    if (a.chargingSchedulePeriod.size() != b.chargingSchedulePeriod.size()) {
        return false;
    }

    for (std::uint32_t i = 0; bRes && i < a.chargingSchedulePeriod.size(); i++) {
        bRes = a.chargingSchedulePeriod[i] == b.chargingSchedulePeriod[i];
    }

    bRes = bRes && (a.chargingRateUnit == b.chargingRateUnit);
    bRes = bRes && (a.startSchedule == b.startSchedule);
    bRes = bRes && (a.duration == b.duration);
    bRes = bRes && (a.minChargingRate == b.minChargingRate);
    bRes = bRes && (a.powerTolerance == b.powerTolerance);
    bRes = bRes && (a.signatureId == b.signatureId);
    bRes = bRes && (a.limitAtSoC == b.limitAtSoC);
    bRes = bRes && (a.digestValue == b.digestValue);
    bRes = bRes && (a.powerTolerance == b.powerTolerance);
    bRes = bRes && (a.useLocalTime == b.useLocalTime);
    bRes = bRes && (a.randomizedDelay == b.randomizedDelay);
    // misses absolutePriceSchedule
    // misses priceLevelSchedule

    return bRes;
}

bool operator!=(const ChargingSchedule& a, const ChargingSchedule& b) {
    return !(a == b);
}

bool operator==(const period_entry_t& a, const period_entry_t& b) {
    bool bRes = (a.start == b.start) && (a.end == b.end) && (a.limit == b.limit) && (a.stack_level == b.stack_level) &&
                (a.charging_rate_unit == b.charging_rate_unit);
    if (a.number_phases && b.number_phases) {
        bRes = bRes && a.number_phases.value() == b.number_phases.value();
    }
    if (a.min_charging_rate && b.min_charging_rate) {
        bRes = bRes && a.min_charging_rate.value() == b.min_charging_rate.value();
    }
    return bRes;
}

bool operator!=(const period_entry_t& a, const period_entry_t& b) {
    return !(a == b);
}

bool operator==(const std::vector<period_entry_t>& a, const std::vector<period_entry_t>& b) {
    bool bRes = a.size() == b.size();
    if (bRes) {
        for (std::uint8_t i = 0; i < a.size(); i++) {
            bRes = a[i] == b[i];
            if (!bRes) {
                break;
            }
        }
    }
    return bRes;
}

std::string to_string(const period_entry_t& entry) {
    std::string result = "Period Entry: {";
    result += "Start: " + entry.start.to_rfc3339() + ", ";
    result += "End: " + entry.end.to_rfc3339() + ", ";
    result += "Limit: " + std::to_string(entry.limit) + ", ";
    if (entry.number_phases.has_value()) {
        result += "Number of Phases: " + std::to_string(entry.number_phases.value()) + ", ";
    }
    result += "Stack Level: " + std::to_string(entry.stack_level) + ", ";
    result += "ChargingRateUnit:" + conversions::charging_rate_unit_enum_to_string(entry.charging_rate_unit);

    if (entry.min_charging_rate.has_value()) {
        result += ", Min Charging Rate: " + std::to_string(entry.min_charging_rate.value());
    }

    result += "}";
    return result;
}

std::ostream& operator<<(std::ostream& os, const period_entry_t& entry) {
    os << to_string(entry);
    return os;
}

ocpp::DateTime dt(const std::string& dt_string) {
    ocpp::DateTime dt;

    if (dt_string.length() == 4) {
        dt = ocpp::DateTime("2024-01-01T0" + dt_string + ":00Z");
    } else if (dt_string.length() == 5) {
        dt = ocpp::DateTime("2024-01-01T" + dt_string + ":00Z");
    } else if (dt_string.length() == 7) {
        dt = ocpp::DateTime("2024-01-0" + dt_string + ":00Z");
    } else if (dt_string.length() == 8) {
        dt = ocpp::DateTime("2024-01-" + dt_string + ":00Z");
    } else if (dt_string.length() == 11) {
        dt = ocpp::DateTime("2024-" + dt_string + ":00Z");
    } else if (dt_string.length() == 16) {
        dt = ocpp::DateTime(dt_string + ":00Z");
    }

    return dt;
}

ChargingSchedule create_charge_schedule(ChargingRateUnitEnum charging_rate_unit) {
    ChargingSchedule charging_schedule;
    charging_schedule.chargingRateUnit = charging_rate_unit;
    return charging_schedule;
}

ChargingSchedule create_charge_schedule(ChargingRateUnitEnum charging_rate_unit,
                                        const std::vector<ChargingSchedulePeriod>& charging_schedule_period,
                                        std::optional<ocpp::DateTime> start_schedule) {
    ChargingSchedule charging_schedule;
    charging_schedule.chargingRateUnit = charging_rate_unit;
    charging_schedule.chargingSchedulePeriod = charging_schedule_period;
    charging_schedule.startSchedule = start_schedule;
    return charging_schedule;
}

std::vector<ChargingSchedulePeriod> create_charging_schedule_periods(int32_t start_period,
                                                                     std::optional<int32_t> number_phases,
                                                                     std::optional<int32_t> phase_to_use) {
    ChargingSchedulePeriod charging_schedule_period;
    charging_schedule_period.startPeriod = start_period;
    charging_schedule_period.numberPhases = number_phases;
    charging_schedule_period.phaseToUse = phase_to_use;

    return {charging_schedule_period};
}

std::vector<ChargingSchedulePeriod> create_charging_schedule_periods(const std::vector<int32_t>& start_periods) {
    auto charging_schedule_periods = std::vector<ChargingSchedulePeriod>();
    for (auto start_period : start_periods) {
        ChargingSchedulePeriod charging_schedule_period;
        charging_schedule_period.startPeriod = start_period;

        charging_schedule_periods.push_back(charging_schedule_period);
    }

    return charging_schedule_periods;
}

std::vector<ChargingSchedulePeriod>
create_charging_schedule_periods_with_phases(int32_t start_period, int32_t numberPhases, int32_t phaseToUse) {
    ChargingSchedulePeriod charging_schedule_period;
    charging_schedule_period.startPeriod = start_period;
    charging_schedule_period.numberPhases = numberPhases;
    charging_schedule_period.phaseToUse = phaseToUse;

    return {charging_schedule_period};
}

ChargingProfile create_charging_profile(int32_t charging_profile_id,
                                        ChargingProfilePurposeEnum charging_profile_purpose,
                                        const std::vector<ChargingSchedule>& charging_schedules,
                                        std::optional<std::string> transaction_id,
                                        ChargingProfileKindEnum charging_profile_kind, int stack_level,
                                        std::optional<ocpp::DateTime> validFrom,
                                        std::optional<ocpp::DateTime> validTo) {
    auto recurrency_kind = RecurrencyKindEnum::Daily;
    ChargingProfile charging_profile;
    charging_profile.id = charging_profile_id;
    charging_profile.stackLevel = stack_level;
    charging_profile.chargingProfilePurpose = charging_profile_purpose;
    charging_profile.chargingProfileKind = charging_profile_kind;
    charging_profile.chargingSchedule = charging_schedules;
    charging_profile.customData = {};
    charging_profile.recurrencyKind = recurrency_kind;
    charging_profile.validFrom = validFrom;
    charging_profile.validTo = validTo;
    charging_profile.transactionId = transaction_id;
    return charging_profile;
}

ChargingProfile create_charging_profile(int32_t charging_profile_id,
                                        ChargingProfilePurposeEnum charging_profile_purpose,
                                        ChargingSchedule charging_schedule, std::optional<std::string> transaction_id,
                                        ChargingProfileKindEnum charging_profile_kind, int stack_level,
                                        std::optional<ocpp::DateTime> validFrom,
                                        std::optional<ocpp::DateTime> validTo) {
    return create_charging_profile(charging_profile_id, charging_profile_purpose,
                                   std::vector<ChargingSchedule>{charging_schedule}, transaction_id,
                                   charging_profile_kind, stack_level, validFrom, validTo);
}

ChargingProfileCriterion create_charging_profile_criteria(std::optional<std::vector<ocpp::CiString<20>>> sources,
                                                          std::optional<std::vector<int32_t>> ids,
                                                          std::optional<ChargingProfilePurposeEnum> purpose,
                                                          std::optional<int32_t> stack_level) {
    ChargingProfileCriterion criteria;
    criteria.chargingLimitSource = sources;
    criteria.chargingProfileId = ids;
    criteria.chargingProfilePurpose = purpose;
    criteria.stackLevel = stack_level;
    return criteria;
}

GetChargingProfilesRequest create_get_charging_profile_request(int32_t request_id, ChargingProfileCriterion criteria,
                                                               std::optional<int32_t> evse_id) {
    GetChargingProfilesRequest req;
    req.requestId = request_id;
    req.chargingProfile = criteria;
    req.evseId = evse_id;
    return req;
}

ClearChargingProfileRequest create_clear_charging_profile_request(std::optional<int32_t> id,
                                                                  std::optional<ClearChargingProfile> criteria) {
    ClearChargingProfileRequest req;
    req.chargingProfileId = id;
    req.chargingProfileCriteria = criteria;
    return req;
}

ClearChargingProfile create_clear_charging_profile(std::optional<int32_t> evse_id,
                                                   std::optional<ChargingProfilePurposeEnum> purpose,
                                                   std::optional<int32_t> stack_level) {
    ClearChargingProfile clear_charging_profile;
    clear_charging_profile.customData = {};
    clear_charging_profile.evseId = evse_id;
    clear_charging_profile.chargingProfilePurpose = purpose;
    clear_charging_profile.stackLevel = stack_level;
    return clear_charging_profile;
}

namespace SmartChargingTestUtils {
std::vector<ChargingProfile> get_charging_profiles_from_directory(const std::string& path) {
    EVLOG_debug << "get_charging_profiles_from_directory: " << path;
    std::vector<ChargingProfile> profiles;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (!entry.is_directory()) {
            fs::path path = entry.path();
            if (path.extension() == ".json") {
                ChargingProfile profile = get_charging_profile_from_path(path);
                std::cout << path << std::endl;
                profiles.push_back(profile);
            }
        }
    }

    // Sort profiles by id in ascending order
    std::sort(profiles.begin(), profiles.end(),
              [](const ChargingProfile& a, const ChargingProfile& b) { return a.id < b.id; });

    EVLOG_debug << "get_charging_profiles_from_directory END";
    return profiles;
}

ChargingProfile get_charging_profile_from_path(const std::string& path) {
    EVLOG_debug << "get_charging_profile_from_path: " << path;
    std::ifstream f(path.c_str());
    json data = json::parse(f);

    ChargingProfile cp;
    from_json(data, cp);
    return cp;
}

ChargingProfile get_charging_profile_from_file(const std::string& filename) {
    const std::string full_path = BASE_JSON_PATH + "/" + filename;

    return get_charging_profile_from_path(full_path);
}

std::vector<ChargingProfile> get_charging_profiles_from_file(const std::string& filename) {
    std::vector<ChargingProfile> profiles;
    profiles.push_back(get_charging_profile_from_file(filename));
    return profiles;
}

/// \brief Returns a vector of ChargingProfiles to be used as a baseline for testing core functionality
/// of generating an EnhancedChargingSchedule.
std::vector<ChargingProfile> get_baseline_profile_vector() {
    return get_charging_profiles_from_directory(BASE_JSON_PATH + "/" + "baseline/");
}

std::string to_string(std::vector<ChargingProfile>& profiles) {
    std::string s;
    json cp_json;
    for (auto& profile : profiles) {
        if (!s.empty())
            s += ", ";
        to_json(cp_json, profile);
        s += cp_json.dump(4);
    }

    return "[" + s + "]";
}

/// \brief Validates that there is no overlap in the submitted period_entry_t collection
/// \param period_entry_t collection
/// \note If there are any overlapping period_entry_t entries the function returns false
bool validate_profile_result(const std::vector<period_entry_t>& result) {
    bool bRes{true};
    DateTime last{"1900-01-01T00:00:00Z"};
    for (const auto& i : result) {
        // ensure no overlaps
        bRes = i.start < i.end;
        bRes = bRes && i.start >= last;
        last = i.end;
        if (!bRes) {
            break;
        }
    }
    return bRes;
}
} // namespace SmartChargingTestUtils

void CompositeScheduleTestFixtureV2::SetUp() {
}

void CompositeScheduleTestFixtureV2::TearDown() {
}

void CompositeScheduleTestFixtureV2::load_charging_profiles_for_evse(const std::filesystem::path& path,
                                                                     int32_t evse_id) {
    std::vector<ChargingProfile> profiles = std::filesystem::is_directory(path)
                                                ? SmartChargingTestUtils::get_charging_profiles_from_directory(path)
                                                : SmartChargingTestUtils::get_charging_profiles_from_file(path);

    ON_CALL(*database_handler, get_charging_profiles_for_evse(evse_id)).WillByDefault(testing::Return(profiles));
}

CompositeScheduleTestFixtureV2::CompositeScheduleTestFixtureV2() :
    evse_manager(std::make_unique<EvseManagerFake>(NR_OF_EVSES)),
    device_model_test_helper(),
    mock_dispatcher(),
    device_model(device_model_test_helper.get_device_model()),
    connectivity_manager(),
    set_charging_profiles_callback_mock(),
    handler(create_smart_charging_handler()),
    uuid_generator(boost::uuids::random_generator()) {

    const auto& charging_rate_unit_cv = ControllerComponentVariables::ChargingScheduleChargingRateUnit;
    device_model->set_value(charging_rate_unit_cv.component, charging_rate_unit_cv.variable.value(),
                            AttributeEnum::Actual, "A,W", "test", true);

    const auto& ac_phase_switching_cv = ControllerComponentVariables::ACPhaseSwitchingSupported;
    device_model->set_value(ac_phase_switching_cv.component, ac_phase_switching_cv.variable.value(),
                            AttributeEnum::Actual, "true", "test", true);

    const auto& default_limit_amps_cv = ControllerComponentVariables::CompositeScheduleDefaultLimitAmps;
    device_model->set_value(default_limit_amps_cv.component, default_limit_amps_cv.variable.value(),
                            AttributeEnum::Actual, std::to_string(DEFAULT_LIMIT_AMPERE), "test", true);

    const auto& default_limit_watts_cv = ControllerComponentVariables::CompositeScheduleDefaultLimitWatts;
    device_model->set_value(default_limit_watts_cv.component, default_limit_watts_cv.variable.value(),
                            AttributeEnum::Actual, std::to_string(DEFAULT_LIMIT_WATT), "test", true);

    const auto& default_limit_phases_cv = ControllerComponentVariables::CompositeScheduleDefaultNumberPhases;
    device_model->set_value(default_limit_phases_cv.component, default_limit_phases_cv.variable.value(),
                            AttributeEnum::Actual, std::to_string(DEFAULT_NR_PHASES), "test", true);
}

std::unique_ptr<TestSmartCharging> CompositeScheduleTestFixtureV2::create_smart_charging_handler() {
    std::unique_ptr<everest::db::sqlite::Connection> database_connection =
        std::make_unique<everest::db::sqlite::Connection>(fs::path("/tmp/ocpp201") / "cp.db");
    this->database_handler =
        std::make_unique<DatabaseHandlerFake>(std::move(database_connection), MIGRATION_FILES_LOCATION_V2);
    database_handler->open_connection();
    this->functional_block_context = std::make_unique<FunctionalBlockContext>(
        this->mock_dispatcher, *this->device_model, this->connectivity_manager, *this->evse_manager,
        *this->database_handler, this->evse_security, this->component_state_manager);
    return std::make_unique<TestSmartCharging>(*functional_block_context,
                                               set_charging_profiles_callback_mock.AsStdFunction());
}

void CompositeScheduleTestFixtureV2::reconfigure_for_nr_of_evses(int32_t nr_of_evses) {
    this->evse_manager = std::make_unique<EvseManagerFake>(nr_of_evses);
    this->functional_block_context = std::make_unique<FunctionalBlockContext>(
        this->mock_dispatcher, *this->device_model, this->connectivity_manager, *this->evse_manager,
        *this->database_handler, this->evse_security, this->component_state_manager);
    this->handler = std::make_unique<TestSmartCharging>(*functional_block_context,
                                                        set_charging_profiles_callback_mock.AsStdFunction());
}

} // namespace ocpp::v2
