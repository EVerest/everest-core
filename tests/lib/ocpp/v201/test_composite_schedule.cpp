// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include "date/tz.h"
#include "everest/logging.hpp"
#include "lib/ocpp/common/database_testing_utils.hpp"
#include "ocpp/common/constants.hpp"
#include "ocpp/common/types.hpp"
#include "ocpp/v201/ctrlr_component_variables.hpp"
#include "ocpp/v201/device_model.hpp"
#include "ocpp/v201/device_model_storage_sqlite.hpp"
#include "ocpp/v201/init_device_model_db.hpp"
#include "ocpp/v201/ocpp_types.hpp"
#include "ocpp/v201/utils.hpp"
#include <algorithm>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cstdint>
#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <sqlite3.h>
#include <string>

#include <component_state_manager_mock.hpp>
#include <device_model_storage_mock.hpp>
#include <evse_manager_fake.hpp>
#include <evse_mock.hpp>
#include <evse_security_mock.hpp>
#include <ocpp/common/call_types.hpp>
#include <ocpp/v201/evse.hpp>
#include <ocpp/v201/smart_charging.hpp>
#include <optional>

#include "smart_charging_test_utils.hpp"

#include <sstream>
#include <vector>

namespace ocpp::v201 {
static const int NR_OF_EVSES = 1;
static const int STATION_WIDE_ID = 0;
static const int DEFAULT_EVSE_ID = 1;
static const int DEFAULT_PROFILE_ID = 1;
static const int DEFAULT_STACK_LEVEL = 1;
static const std::string DEFAULT_TX_ID = "10c75ff7-74f5-44f5-9d01-f649f3ac7b78";
const static std::string MIGRATION_FILES_PATH = "./resources/v201/device_model_migration_files";
const static std::string CONFIG_PATH = "./resources/example_config/v201/component_config";
const static std::string DEVICE_MODEL_DB_IN_MEMORY_PATH = "file::memory:?cache=shared";

class TestSmartChargingHandler : public SmartChargingHandler {
public:
    using SmartChargingHandler::validate_charging_station_max_profile;
    using SmartChargingHandler::validate_evse_exists;
    using SmartChargingHandler::validate_profile_schedules;
    using SmartChargingHandler::validate_tx_default_profile;
    using SmartChargingHandler::validate_tx_profile;

    using SmartChargingHandler::SmartChargingHandler;
};

class CompositeScheduleTestFixtureV201 : public DatabaseTestingUtils {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }

    ChargingSchedule create_charge_schedule(ChargingRateUnitEnum charging_rate_unit) {
        int32_t id;
        std::vector<ChargingSchedulePeriod> charging_schedule_period;
        std::optional<CustomData> custom_data;
        std::optional<ocpp::DateTime> start_schedule;
        std::optional<int32_t> duration;
        std::optional<float> min_charging_rate;
        std::optional<SalesTariff> sales_tariff;

        return ChargingSchedule{
            id,
            charging_rate_unit,
            charging_schedule_period,
            custom_data,
            start_schedule,
            duration,
            min_charging_rate,
            sales_tariff,
        };
    }

    ChargingSchedule create_charge_schedule(ChargingRateUnitEnum charging_rate_unit,
                                            std::vector<ChargingSchedulePeriod> charging_schedule_period,
                                            std::optional<ocpp::DateTime> start_schedule = std::nullopt) {
        int32_t id;
        std::optional<CustomData> custom_data;
        std::optional<int32_t> duration;
        std::optional<float> min_charging_rate;
        std::optional<SalesTariff> sales_tariff;

        return ChargingSchedule{
            id,
            charging_rate_unit,
            charging_schedule_period,
            custom_data,
            start_schedule,
            duration,
            min_charging_rate,
            sales_tariff,
        };
    }

    std::vector<ChargingSchedulePeriod>
    create_charging_schedule_periods(int32_t start_period, std::optional<int32_t> number_phases = std::nullopt,
                                     std::optional<int32_t> phase_to_use = std::nullopt) {
        ChargingSchedulePeriod charging_schedule_period;
        charging_schedule_period.startPeriod = start_period;
        charging_schedule_period.numberPhases = number_phases;
        charging_schedule_period.phaseToUse = phase_to_use;

        return {charging_schedule_period};
    }

    std::vector<ChargingSchedulePeriod> create_charging_schedule_periods(std::vector<int32_t> start_periods) {
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

    ChargingProfile
    create_charging_profile(int32_t charging_profile_id, ChargingProfilePurposeEnum charging_profile_purpose,
                            ChargingSchedule charging_schedule, std::optional<std::string> transaction_id = {},
                            ChargingProfileKindEnum charging_profile_kind = ChargingProfileKindEnum::Absolute,
                            int stack_level = DEFAULT_STACK_LEVEL, std::optional<ocpp::DateTime> validFrom = {},
                            std::optional<ocpp::DateTime> validTo = {}) {
        auto recurrency_kind = RecurrencyKindEnum::Daily;
        std::vector<ChargingSchedule> charging_schedules = {charging_schedule};
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

    void create_device_model_db(const std::string& path) {
        InitDeviceModelDb db(path, MIGRATION_FILES_PATH);
        db.initialize_database(CONFIG_PATH, true);
    }

    std::shared_ptr<DeviceModel>
    create_device_model(const std::optional<std::string> ac_phase_switching_supported = "true") {
        create_device_model_db(DEVICE_MODEL_DB_IN_MEMORY_PATH);
        auto device_model_storage = std::make_unique<DeviceModelStorageSqlite>(DEVICE_MODEL_DB_IN_MEMORY_PATH);
        auto device_model = std::make_shared<DeviceModel>(std::move(device_model_storage));

        // Defaults
        const auto& charging_rate_unit_cv = ControllerComponentVariables::ChargingScheduleChargingRateUnit;
        device_model->set_value(charging_rate_unit_cv.component, charging_rate_unit_cv.variable.value(),
                                AttributeEnum::Actual, "A,W", "test", true);

        const auto& ac_phase_switching_cv = ControllerComponentVariables::ACPhaseSwitchingSupported;
        device_model->set_value(ac_phase_switching_cv.component, ac_phase_switching_cv.variable.value(),
                                AttributeEnum::Actual, ac_phase_switching_supported.value_or(""), "test", true);

        return device_model;
    }

    TestSmartChargingHandler create_smart_charging_handler() {
        std::unique_ptr<common::DatabaseConnection> database_connection =
            std::make_unique<common::DatabaseConnection>(fs::path("/tmp/ocpp201") / "cp.db");
        std::shared_ptr<DatabaseHandler> database_handler =
            std::make_shared<DatabaseHandler>(std::move(database_connection), MIGRATION_FILES_LOCATION_V201);
        database_handler->open_connection();
        return TestSmartChargingHandler(*this->evse_manager, device_model, database_handler);
    }

    // Default values used within the tests
    std::unique_ptr<EvseManagerFake> evse_manager = std::make_unique<EvseManagerFake>(NR_OF_EVSES);

    sqlite3* db_handle;

    bool ignore_no_transaction = true;
    std::shared_ptr<DeviceModel> device_model = create_device_model();
    TestSmartChargingHandler handler = create_smart_charging_handler();
    boost::uuids::random_generator uuid_generator = boost::uuids::random_generator();
};

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_FoundationTest_Grid) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/grid/");
    const DateTime start_time = ocpp::DateTime("2024-01-17T00:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-18T00:00:00");
    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 1.0;
    period1.numberPhases = 1;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 3600;
    period2.limit = 2.0;
    period2.numberPhases = 1;
    ChargingSchedulePeriod period3;
    period3.startPeriod = 7200;
    period3.limit = 3.0;
    period3.numberPhases = 1;
    ChargingSchedulePeriod period4;
    period4.startPeriod = 10800;
    period4.limit = 4.0;
    period4.numberPhases = 1;
    ChargingSchedulePeriod period5;
    period5.startPeriod = 14400;
    period5.limit = 5.0;
    period5.numberPhases = 1;
    ChargingSchedulePeriod period6;
    period6.startPeriod = 18000;
    period6.limit = 6.0;
    period6.numberPhases = 1;
    ChargingSchedulePeriod period7;
    period7.startPeriod = 21600;
    period7.limit = 7.0;
    period7.numberPhases = 1;
    ChargingSchedulePeriod period8;
    period8.startPeriod = 25200;
    period8.limit = 8.0;
    period8.numberPhases = 1;
    ChargingSchedulePeriod period9;
    period9.startPeriod = 28800;
    period9.limit = 9.0;
    period9.numberPhases = 1;
    ChargingSchedulePeriod period10;
    period10.startPeriod = 32400;
    period10.limit = 10.0;
    period10.numberPhases = 1;
    ChargingSchedulePeriod period11;
    period11.startPeriod = 36000;
    period11.limit = 11.0;
    period11.numberPhases = 1;
    ChargingSchedulePeriod period12;
    period12.startPeriod = 39600;
    period12.limit = 12.0;
    period12.numberPhases = 1;
    ChargingSchedulePeriod period13;
    period13.startPeriod = 43200;
    period13.limit = 13.0;
    period13.numberPhases = 1;
    ChargingSchedulePeriod period14;
    period14.startPeriod = 46800;
    period14.limit = 14.0;
    period14.numberPhases = 1;
    ChargingSchedulePeriod period15;
    period15.startPeriod = 50400;
    period15.limit = 15.0;
    period15.numberPhases = 1;
    ChargingSchedulePeriod period16;
    period16.startPeriod = 54000;
    period16.limit = 16.0;
    period16.numberPhases = 1;
    ChargingSchedulePeriod period17;
    period17.startPeriod = 57600;
    period17.limit = 17.0;
    period17.numberPhases = 1;
    ChargingSchedulePeriod period18;
    period18.startPeriod = 61200;
    period18.limit = 18.0;
    period18.numberPhases = 1;
    ChargingSchedulePeriod period19;
    period19.startPeriod = 64800;
    period19.limit = 19.0;
    period19.numberPhases = 1;
    ChargingSchedulePeriod period20;
    period20.startPeriod = 68400;
    period20.limit = 20.0;
    period20.numberPhases = 1;
    ChargingSchedulePeriod period21;
    period21.startPeriod = 72000;
    period21.limit = 21.0;
    period21.numberPhases = 1;
    ChargingSchedulePeriod period22;
    period22.startPeriod = 75600;
    period22.limit = 22.0;
    period22.numberPhases = 1;
    ChargingSchedulePeriod period23;
    period23.startPeriod = 79200;
    period23.limit = 23.0;
    period23.numberPhases = 1;
    ChargingSchedulePeriod period24;
    period24.startPeriod = 82800;
    period24.limit = 24.0;
    period24.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1,  period2,  period3,  period4,  period5,  period6,  period7,  period8,
                                       period9,  period10, period11, period12, period13, period14, period15, period16,
                                       period17, period18, period19, period20, period21, period22, period23, period24};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 86400;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_LayeredTest_SameStartTime) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/layered/");

    // Time Window: START = Stack #1 start time || END = Stack #1 end time
    {
        const DateTime start_time = ocpp::DateTime("2024-01-18T18:04:00");
        const DateTime end_time = ocpp::DateTime("2024-01-18T18:22:00");

        ChargingSchedulePeriod period;
        period.startPeriod = 0;
        period.limit = 19.0;
        period.numberPhases = 1;
        CompositeSchedule expected;
        expected.chargingSchedulePeriod = {period};
        expected.evseId = DEFAULT_EVSE_ID;
        expected.duration = 1080;
        expected.scheduleStart = start_time;
        expected.chargingRateUnit = ChargingRateUnitEnum::W;

        CompositeSchedule actual = handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID,
                                                                        ChargingRateUnitEnum::W);

        ASSERT_EQ(actual, expected);
    }

    // Time Window: START = Stack #1 start time || END = After Stack #1 end time Before next Start #0 start time
    {
        const DateTime start_time = ocpp::DateTime("2024-01-17T18:04:00");
        const DateTime end_time = ocpp::DateTime("2024-01-17T18:33:00");

        ChargingSchedulePeriod period1;
        period1.startPeriod = 0;
        period1.limit = 2000.0;
        period1.numberPhases = 1;
        ChargingSchedulePeriod period2;
        period2.startPeriod = 1080;
        period2.limit = 19.0;
        period2.numberPhases = 1;
        CompositeSchedule expected;
        expected.chargingSchedulePeriod = {period1, period2};
        expected.evseId = DEFAULT_EVSE_ID;
        expected.duration = 1740;
        expected.scheduleStart = start_time;
        expected.chargingRateUnit = ChargingRateUnitEnum::W;

        CompositeSchedule actual = handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID,
                                                                        ChargingRateUnitEnum::W);

        ASSERT_EQ(actual, expected);
    }

    // Time Window: START = Stack #1 start time || END = After next Start #0 start time
    {
        const DateTime start_time = ocpp::DateTime("2024-01-17T18:04:00");
        const DateTime end_time = ocpp::DateTime("2024-01-17T19:04:00");

        ChargingSchedulePeriod period1;
        period1.startPeriod = 0;
        period1.limit = 2000.0;
        period1.numberPhases = 1;
        ChargingSchedulePeriod period2;
        period2.startPeriod = 1080;
        period2.limit = 19.0;
        period2.numberPhases = 1;
        ChargingSchedulePeriod period3;
        period3.startPeriod = 3360;
        period3.limit = 20.0;
        period3.numberPhases = 1;

        CompositeSchedule expected;
        expected.chargingSchedulePeriod = {period1, period2, period3};
        expected.evseId = DEFAULT_EVSE_ID;
        expected.duration = 3600;
        expected.scheduleStart = start_time;
        expected.chargingRateUnit = ChargingRateUnitEnum::W;

        CompositeSchedule actual = handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID,
                                                                        ChargingRateUnitEnum::W);

        ASSERT_EQ(actual, expected);
    }
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_LayeredRecurringTest_FutureStartTime) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/layered_recurring/");

    const DateTime start_time = ocpp::DateTime("2024-02-17T18:04:00");
    const DateTime end_time = ocpp::DateTime("2024-02-17T18:05:00");

    ChargingSchedulePeriod period;
    period.startPeriod = 0;
    period.limit = 2000.0;
    period.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 60;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_LayeredTest_PreviousStartTime) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_file("singles/TXProfile_Absolute_Start18-04.json");
    const DateTime start_time = ocpp::DateTime("2024-01-17T18:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-17T18:05:00");
    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = DEFAULT_LIMIT_WATTS;
    period1.numberPhases = DEFAULT_AND_MAX_NUMBER_PHASES;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 240;
    period2.limit = profiles.at(0).chargingSchedule.front().chargingSchedulePeriod.front().limit;
    period2.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 300;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_LayeredRecurringTest_PreviousStartTime) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/layered_recurring/");
    const DateTime start_time = ocpp::DateTime("2024-02-19T18:00:00");
    const DateTime end_time = ocpp::DateTime("2024-02-19T19:04:00");
    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 19.0;
    period1.numberPhases = 1;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 240;
    period2.limit = profiles.back().chargingSchedule.front().chargingSchedulePeriod.front().limit;
    period2.numberPhases = 1;
    ChargingSchedulePeriod period3;
    period3.startPeriod = 1320;
    period3.limit = 19.0;
    period3.numberPhases = 1;
    ChargingSchedulePeriod period4;
    period4.startPeriod = 3600;
    period4.limit = 20.0;
    period4.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2, period3, period4}, expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 3840;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);

    ASSERT_EQ(actual, expected);
}

/**
 * Calculate Composite Schedule
 */
TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_ValidateBaselineProfileVector) {
    const DateTime start_time = ocpp::DateTime("2024-01-17T18:01:00");
    const DateTime end_time = ocpp::DateTime("2024-01-18T06:00:00");
    std::vector<ChargingProfile> profiles = SmartChargingTestUtils::get_baseline_profile_vector();
    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 2000.0;
    period1.numberPhases = 1;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 1020;
    period2.limit = 11000.0;
    period2.numberPhases = 3;
    ChargingSchedulePeriod period3;
    period3.startPeriod = 25140;
    period3.limit = 6000.0;
    period3.numberPhases = 3;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2, period3};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 43140;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_RelativeProfile_minutia) {
    const DateTime start_time = ocpp::DateTime("2024-05-17T05:00:00");
    const DateTime end_time = ocpp::DateTime("2024-05-17T06:00:00");

    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/relative/");
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, profiles.at(0).transactionId.value());

    // Doing this in order to avoid mocking system_clock::now()
    auto transaction = std::move(this->evse_manager->get_evse(DEFAULT_EVSE_ID).get_transaction());

    ChargingSchedulePeriod period;
    period.startPeriod = 0;
    period.limit = 2000.0;
    period.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 3600;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_RelativeProfile_e2e) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/relative/");
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, profiles.at(0).transactionId.value());

    // Doing this in order to avoid mocking system_clock::now()
    auto transaction = std::move(this->evse_manager->get_evse(DEFAULT_EVSE_ID).get_transaction());

    const DateTime start_time = ocpp::DateTime("2024-05-17T05:00:00");
    const DateTime end_time = ocpp::DateTime("2024-05-17T06:01:00");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 2000.0;
    period1.numberPhases = 1;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 3601;
    period2.limit = 7.0;
    period2.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 3660;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_DemoCaseOne_17th) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/case_one/");
    ChargingProfile relative_profile = profiles.front();
    auto transaction_id = relative_profile.transactionId.value();
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, transaction_id);
    const DateTime start_time = ocpp::DateTime("2024-01-17T18:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-18T06:00:00");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 2000.0;
    period1.numberPhases = 1;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 1080;
    period2.limit = 11000.0;
    period2.numberPhases = 1;
    ChargingSchedulePeriod period3;
    period3.startPeriod = 25200;
    period3.limit = 6000.0;
    period3.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2, period3};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 43200;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_DemoCaseOne_19th) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/case_one/");
    ChargingProfile first_profile = profiles.front();
    auto transaction_id = first_profile.transactionId.value();
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, transaction_id);
    const DateTime start_time = ocpp::DateTime("2024-01-19T18:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-20T06:00:00");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 11000.0;
    period1.numberPhases = 1;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 25200;
    period2.limit = 6000.0;
    period2.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 43200;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);

    ASSERT_EQ(ProfileValidationResultEnum::Valid,
              handler.conform_and_validate_profile(profiles.at(0), DEFAULT_EVSE_ID));
    ASSERT_EQ(ProfileValidationResultEnum::Valid,
              handler.conform_and_validate_profile(profiles.at(1), DEFAULT_EVSE_ID));
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_MaxOverridesHigherLimits) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/max/");

    const DateTime start_time = ocpp::DateTime("2024-01-17T00:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-17T02:00:00");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 10.0;
    period1.numberPhases = 1;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 3600;
    period2.limit = 20.0;
    period2.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 7200;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_MaxOverridenByLowerLimits) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/max/");

    const DateTime start_time = ocpp::DateTime("2024-01-17T22:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-18T00:00:00");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 230.0;
    period1.numberPhases = 1;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 3600;
    period2.limit = 10.0;
    period2.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 7200;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_ExternalOverridesHigherLimits) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/external/");

    const DateTime start_time = ocpp::DateTime("2024-01-17T00:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-17T02:00:00");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 10.0;
    period1.numberPhases = 1;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 3600;
    period2.limit = 20.0;
    period2.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 7200;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_ExternalOverridenByLowerLimits) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/external/");

    const DateTime start_time = ocpp::DateTime("2024-01-17T22:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-18T00:00:00");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 230.0;
    period1.numberPhases = 1;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 3600;
    period2.limit = 10.0;
    period2.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 7200;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, OCTT_TC_K_41_CS) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/OCCT_TC_K_41_CS/");

    const DateTime start_time = ocpp::DateTime("2024-08-21T12:24:40");
    const DateTime end_time = ocpp::DateTime("2024-08-21T12:31:20");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 8.0;
    period1.numberPhases = 3;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 46;
    period2.limit = 10.0;
    period2.numberPhases = 3;
    ChargingSchedulePeriod period3;
    period3.startPeriod = 196;
    period3.limit = 6.0;
    period3.numberPhases = 3;
    ChargingSchedulePeriod period4;
    period4.startPeriod = 236;
    period4.limit = 10.0;
    period4.numberPhases = 3;
    ChargingSchedulePeriod period5;
    period5.startPeriod = 260;
    period5.limit = 8.0;
    period5.numberPhases = 3;
    ChargingSchedulePeriod period6;
    period6.startPeriod = 300;
    period6.limit = 10.0;
    period6.numberPhases = 3;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2, period3, period4, period5, period6};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 400;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::A;

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::A);

    ASSERT_EQ(actual, expected);
}

} // namespace ocpp::v201
