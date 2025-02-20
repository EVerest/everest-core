// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include "connectivity_manager_mock.hpp"
#include "database_handler_mock.hpp"
#include "date/tz.h"
#include "device_model_test_helper.hpp"
#include "everest/logging.hpp"
#include "lib/ocpp/common/database_testing_utils.hpp"
#include "message_dispatcher_mock.hpp"
#include "ocpp/common/constants.hpp"
#include "ocpp/common/types.hpp"
#include "ocpp/v2/ctrlr_component_variables.hpp"
#include "ocpp/v2/device_model.hpp"
#include "ocpp/v2/device_model_storage_sqlite.hpp"
#include "ocpp/v2/functional_blocks/smart_charging.hpp"
#include "ocpp/v2/init_device_model_db.hpp"
#include "ocpp/v2/ocpp_types.hpp"
#include "ocpp/v2/utils.hpp"
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
#include <evse_manager_fake.hpp>
#include <evse_mock.hpp>
#include <evse_security_mock.hpp>
#include <ocpp/common/call_types.hpp>
#include <ocpp/v2/evse.hpp>

#include <optional>

#include "mocks/database_handler_fake.hpp"

#include "smart_charging_matchers.hpp"
#include "smart_charging_test_utils.hpp"

#include <sstream>
#include <vector>

namespace ocpp::v2 {
static const int NR_OF_EVSES = 1;
static const int STATION_WIDE_ID = 0;
static const int DEFAULT_EVSE_ID = 1;
static const int DEFAULT_PROFILE_ID = 1;
static const int DEFAULT_STACK_LEVEL = 1;
constexpr int32_t DEFAULT_LIMIT_AMPERE = 57;
constexpr int32_t DEFAULT_LIMIT_WATT = 55612;
constexpr int32_t DEFAULT_NR_PHASES = 3;
static const std::string DEFAULT_TX_ID = "f1522902-1170-416f-8e43-9e3bce28fde7";
static const std::string MIGRATION_FILES_PATH = "./resources/v2/device_model_migration_files";
static const std::string CONFIG_PATH = "./resources/example_config/v2/component_config";
static const std::string DEVICE_MODEL_DB_IN_MEMORY_PATH = "file::memory:?cache=shared";

using ::testing::MockFunction;

class TestSmartCharging : public SmartCharging {
public:
    using SmartCharging::calculate_composite_schedule;
    using SmartCharging::validate_charging_station_max_profile;
    using SmartCharging::validate_evse_exists;
    using SmartCharging::validate_profile_schedules;
    using SmartCharging::validate_tx_default_profile;
    using SmartCharging::validate_tx_profile;

    using SmartCharging::SmartCharging;
};

class CompositeScheduleTestFixtureV2 : public DatabaseTestingUtils {
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

    void load_charging_profiles_for_evse(const std::filesystem::path& path, int32_t evse_id) {
        std::vector<ChargingProfile> profiles = std::filesystem::is_directory(path)
                                                    ? SmartChargingTestUtils::get_charging_profiles_from_directory(path)
                                                    : SmartChargingTestUtils::get_charging_profiles_from_file(path);

        ON_CALL(*database_handler, get_charging_profiles_for_evse(evse_id)).WillByDefault(testing::Return(profiles));
    }

    CompositeScheduleTestFixtureV2() :
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

    std::unique_ptr<TestSmartCharging> create_smart_charging_handler() {
        std::unique_ptr<common::DatabaseConnection> database_connection =
            std::make_unique<common::DatabaseConnection>(fs::path("/tmp/ocpp201") / "cp.db");
        this->database_handler =
            std::make_unique<DatabaseHandlerFake>(std::move(database_connection), MIGRATION_FILES_LOCATION_V2);
        database_handler->open_connection();
        return std::make_unique<TestSmartCharging>(*device_model, *this->evse_manager, connectivity_manager,
                                                   mock_dispatcher, *database_handler,
                                                   set_charging_profiles_callback_mock.AsStdFunction());
    }

    void reconfigure_for_nr_of_evses(int32_t nr_of_evses) {
        this->evse_manager = std::make_unique<EvseManagerFake>(nr_of_evses);
        this->handler = std::make_unique<TestSmartCharging>(*device_model, *this->evse_manager, connectivity_manager,
                                                            mock_dispatcher, *database_handler,
                                                            set_charging_profiles_callback_mock.AsStdFunction());
    }

    // Default values used within the tests
    std::unique_ptr<EvseManagerFake> evse_manager;

    DeviceModelTestHelper device_model_test_helper;
    MockMessageDispatcher mock_dispatcher;
    DeviceModel* device_model;
    ::testing::NiceMock<ConnectivityManagerMock> connectivity_manager;
    std::unique_ptr<DatabaseHandlerFake> database_handler{};
    MockFunction<void()> set_charging_profiles_callback_mock;
    std::unique_ptr<TestSmartCharging> handler;
    boost::uuids::random_generator uuid_generator = boost::uuids::random_generator();
};

TEST_F(CompositeScheduleTestFixtureV2, NoSchedulesPresent) {
    const DateTime start_time = ocpp::DateTime("2024-01-02T00:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T01:00:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, STATION_WIDE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEquals(0, DEFAULT_LIMIT_AMPERE)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, ExtraSeconds) {
    this->load_charging_profiles_for_evse("singles/Absolute_301.json", STATION_WIDE_ID);

    const DateTime start_time = ocpp::DateTime("2024-01-01T12:01:59");
    const DateTime end_time = ocpp::DateTime("2024-01-01T13:02:01");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, STATION_WIDE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3602);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEquals(   0, DEFAULT_LIMIT_AMPERE),
                    PeriodEquals(   1, 32.0F),
                    PeriodEquals(1801, 31.0F),
                    PeriodEquals(2701, 30.0F),
                    PeriodEquals(3601, DEFAULT_LIMIT_AMPERE)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, FoundationTest_Grid) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/grid/", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

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

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV2, LayeredTest_SameStartTime) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/layered/", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

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

        CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                         ChargingRateUnitEnum::W, false, false);

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

        CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                         ChargingRateUnitEnum::W, false, false);

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

        CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                         ChargingRateUnitEnum::W, false, false);

        ASSERT_EQ(actual, expected);
    }
}

TEST_F(CompositeScheduleTestFixtureV2, LayeredRecurringTest_FutureStartTime) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/layered_recurring/", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

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

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV2, LayeredTest_PreviousStartTime) {
    this->load_charging_profiles_for_evse("singles/TXProfile_Absolute_Start18-04.json", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    const DateTime start_time = ocpp::DateTime("2024-01-17T18:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-17T18:05:00");
    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = DEFAULT_LIMIT_WATT;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 240;
    period2.limit = 2000;
    period2.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 300;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV2, LayeredRecurringTest_PreviousStartTime) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/layered_recurring/", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    const DateTime start_time = ocpp::DateTime("2024-02-19T18:00:00");
    const DateTime end_time = ocpp::DateTime("2024-02-19T19:04:00");
    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 19.0;
    period1.numberPhases = 1;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 240;
    period2.limit = 2000;
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
    expected.chargingSchedulePeriod = {period1, period2, period3, period4};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 3840;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);

    ASSERT_EQ(actual, expected);
}

/**
 * Calculate Composite Schedule
 */
TEST_F(CompositeScheduleTestFixtureV2, ValidateBaselineProfileVector) {
    const DateTime start_time = ocpp::DateTime("2024-01-17T18:01:00");
    const DateTime end_time = ocpp::DateTime("2024-01-18T06:00:00");
    std::vector<ChargingProfile> profiles = SmartChargingTestUtils::get_baseline_profile_vector();

    ON_CALL(*database_handler, get_charging_profiles_for_evse(DEFAULT_EVSE_ID))
        .WillByDefault(testing::Return(profiles));

    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

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

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV2, RelativeProfile_minutia) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/relative/", DEFAULT_EVSE_ID);

    const DateTime start_time = ocpp::DateTime("2024-05-17T05:00:00");
    const DateTime end_time = ocpp::DateTime("2024-05-17T06:00:00");

    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);
    this->evse_manager->get_evse(DEFAULT_EVSE_ID).get_transaction()->start_time = start_time;

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

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV2, RelativeProfile_e2e) {
    const DateTime start_time = ocpp::DateTime("2024-05-17T05:00:00");
    const DateTime end_time = ocpp::DateTime("2024-05-17T06:01:00");

    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/relative/", DEFAULT_EVSE_ID);

    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);
    this->evse_manager->get_evse(DEFAULT_EVSE_ID).get_transaction()->start_time = start_time;

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

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV2, DemoCaseOne_17th) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/case_one/", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

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

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV2, DemoCaseOne_19th) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/case_one/", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

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

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV2, MaxOverridesHigherLimits) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/max/0/", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/max/1/", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

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

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV2, MaxOverridenByLowerLimits) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/max/0/", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/max/1/", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

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

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV2, ExternalOverridesHigherLimits) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/external/0/", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/external/1/", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

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

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV2, ExternalOverridenByLowerLimits) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/external/0/", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/external/1/", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

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

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV2, OCTT_TC_K_41_CS) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/OCCT_TC_K_41_CS/0/", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/OCCT_TC_K_41_CS/1/", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

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

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::A, false, false);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV2, SingleStationMaxForEvse0) {
    this->load_charging_profiles_for_evse("singles/ChargingStationMaxProfile_401.json", STATION_WIDE_ID);

    const DateTime start_time = ocpp::DateTime("2024-08-21T08:00:00");
    const DateTime end_time = ocpp::DateTime("2024-08-21T09:00:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID,
                                                                     ChargingRateUnitEnum::A, false, false);

    EXPECT_EQ(result.evseId, STATION_WIDE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 24.0F, 1),
                    PeriodEqualsWithPhases(  900, 28.0F, 1),
                    PeriodEqualsWithPhases( 1800, 30.0F, 1),
                    PeriodEqualsWithPhases( 2700, 32.0F, 1)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, SingleTxDefaultProfileForEvse0WithSingleEvse) {
    this->load_charging_profiles_for_evse("singles/Relative_303.json", STATION_WIDE_ID);

    const DateTime start_time = ocpp::DateTime("2024-01-02T08:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:00:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, STATION_WIDE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 16.0F, 3),
                    PeriodEqualsWithPhases( 1800, 15.0F, 3),
                    PeriodEqualsWithPhases( 2700, 14.0F, 3)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, SingleTxDefaultProfileForEvse0WithMultipleEvses_Current) {
    this->load_charging_profiles_for_evse("singles/Relative_303.json", STATION_WIDE_ID);

    const DateTime start_time = ocpp::DateTime("2024-01-02T08:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:00:00");

    constexpr int32_t nr_of_evses = 2;

    this->reconfigure_for_nr_of_evses(nr_of_evses);

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, STATION_WIDE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, nr_of_evses * 16.0F, 3),
                    PeriodEqualsWithPhases( 1800, nr_of_evses * 15.0F, 3),
                    PeriodEqualsWithPhases( 2700, nr_of_evses * 14.0F, 3)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, SingleTxDefaultProfileForEvse0WithMultipleEvses_Power) {
    this->load_charging_profiles_for_evse("singles/TXDefaultProfile_25_Watt.json", STATION_WIDE_ID);

    constexpr int32_t nr_of_evses = 2;

    this->reconfigure_for_nr_of_evses(nr_of_evses);

    // Note the time is 1 minute off the whole hour
    const DateTime start_time = ocpp::DateTime("2024-01-02T08:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:00:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, STATION_WIDE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 36.0F, 3),
                    PeriodEqualsWithPhases(  300, 24.0F, 3),
                    PeriodEqualsWithPhases(  600, 18.0F, 3),
                    PeriodEqualsWithPhases(  900, 12.0F, 3),
                    PeriodEqualsWithPhases( 1200,  6.0F, 3)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, SingleTxDefaultProfileWithStationMaxForEvse0WithSingleEvse) {
    this->load_charging_profiles_for_evse("singles/ChargingStationMaxProfile_401.json", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse("singles/Relative_303.json", DEFAULT_EVSE_ID);

    // Note the time is 1 minute off the whole hour
    const DateTime start_time = ocpp::DateTime("2024-01-02T08:01:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:01:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, STATION_WIDE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 16.0F, 1),
                    PeriodEqualsWithPhases( 1800, 15.0F, 1),
                    PeriodEqualsWithPhases( 2700, 14.0F, 1)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, SingleTxDefaultProfileWithStationMaxForEvse0WithMultipleEvses) {
    this->load_charging_profiles_for_evse("singles/ChargingStationMaxProfile_401.json", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse("singles/Relative_303.json", DEFAULT_EVSE_ID);
    this->load_charging_profiles_for_evse("singles/Relative_303.json", 2);

    constexpr int32_t nr_of_evses = 2;

    this->reconfigure_for_nr_of_evses(nr_of_evses);

    // Note the time is 1 minute off the whole hour
    const DateTime start_time = ocpp::DateTime("2024-01-02T08:01:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:01:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, STATION_WIDE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 24.0F, 1),
                    PeriodEqualsWithPhases(  840, 28.0F, 1),
                    PeriodEqualsWithPhases( 1740, 30.0F, 1),
                    PeriodEqualsWithPhases( 2700, 28.0F, 1)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, TxProfilePerEvseWithMultipleEvses) {
    this->load_charging_profiles_for_evse("singles/Recurring_Daily_301.json", DEFAULT_EVSE_ID);
    this->load_charging_profiles_for_evse("singles/Relative_303.json", 2);

    constexpr int32_t nr_of_evses = 2;

    this->reconfigure_for_nr_of_evses(nr_of_evses);

    // Note the time is 1 minute off the whole hour
    const DateTime start_time = ocpp::DateTime("2024-01-02T08:01:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:01:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, STATION_WIDE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 32.0F, 3),
                    PeriodEqualsWithPhases( 1740, 31.0F, 3),
                    PeriodEqualsWithPhases( 1800, 30.0F, 3),
                    PeriodEqualsWithPhases( 2640, 29.0F, 3),
                    PeriodEqualsWithPhases( 2700, 28.0F, 3),
                    PeriodEqualsWithPhases( 3540, 14.0F + DEFAULT_LIMIT_AMPERE, 3)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, MixingCurrentAndPower_16A_1P) {
    this->load_charging_profiles_for_evse("singles/ChargingStationMaxProfile_24_Ampere.json", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse("singles/TXDefaultProfile_25_Watt.json", DEFAULT_EVSE_ID);

    const DateTime start_time = ocpp::DateTime("2024-01-02T00:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T01:00:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, DEFAULT_EVSE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 16.0F, 1),
                    PeriodEqualsWithPhases( 1200,  9.0F, 1)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, MixingCurrentAndPower_16A_3P) {
    this->load_charging_profiles_for_evse("singles/ChargingStationMaxProfile_24_Ampere.json", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse("singles/TXDefaultProfile_25_Watt.json", DEFAULT_EVSE_ID);

    const DateTime start_time = ocpp::DateTime("2024-01-02T01:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T02:00:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, DEFAULT_EVSE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 16.0F, 3),
                    PeriodEqualsWithPhases(  300, 12.0F, 3),
                    PeriodEqualsWithPhases(  600,  9.0F, 3),
                    PeriodEqualsWithPhases(  900,  6.0F, 3),
                    PeriodEqualsWithPhases( 1200,  3.0F, 3)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, MixingCurrentAndPower_10A_1P) {
    this->load_charging_profiles_for_evse("singles/ChargingStationMaxProfile_24_Ampere.json", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse("singles/TXDefaultProfile_25_Watt.json", DEFAULT_EVSE_ID);

    const DateTime start_time = ocpp::DateTime("2024-01-02T02:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T03:00:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, DEFAULT_EVSE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 10.0F, 1),
                    PeriodEqualsWithPhases( 1200,  9.0F, 1)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, MixingCurrentAndPower_10A_3P) {
    this->load_charging_profiles_for_evse("singles/ChargingStationMaxProfile_24_Ampere.json", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse("singles/TXDefaultProfile_25_Watt.json", DEFAULT_EVSE_ID);

    const DateTime start_time = ocpp::DateTime("2024-01-02T03:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T04:00:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, DEFAULT_EVSE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 10.0F, 3),
                    PeriodEqualsWithPhases(  600,  9.0F, 3),
                    PeriodEqualsWithPhases(  900,  6.0F, 3),
                    PeriodEqualsWithPhases( 1200,  3.0F, 3)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, MixingCurrentAndPower_10A_1P_RequestPower) {
    this->load_charging_profiles_for_evse("singles/ChargingStationMaxProfile_24_Ampere.json", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse("singles/TXDefaultProfile_25_Watt.json", DEFAULT_EVSE_ID);

    const DateTime start_time = ocpp::DateTime("2024-01-02T02:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T03:00:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, true);

    EXPECT_EQ(result.evseId, DEFAULT_EVSE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::W);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 2300.0F, 1),
                    PeriodEqualsWithPhases( 1200, 2070.0F, 1)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, MixingCurrentAndPower_10A_3P_RequestPower) {
    this->load_charging_profiles_for_evse("singles/ChargingStationMaxProfile_24_Ampere.json", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse("singles/TXDefaultProfile_25_Watt.json", DEFAULT_EVSE_ID);

    const DateTime start_time = ocpp::DateTime("2024-01-02T03:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T04:00:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, true);

    EXPECT_EQ(result.evseId, DEFAULT_EVSE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::W);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 6900.0F, 3),
                    PeriodEqualsWithPhases(  600, 6210.0F, 3),
                    PeriodEqualsWithPhases(  900, 4140.0F, 3),
                    PeriodEqualsWithPhases( 1200, 2070.0F, 3)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, TxProfilePerEvseWithMultipleEvses_DifferentNrOfPhases) {
    this->load_charging_profiles_for_evse("singles/Recurring_Daily_302_phase_limit.json", DEFAULT_EVSE_ID);
    this->load_charging_profiles_for_evse("singles/Relative_302_phase_limit.json", 2);

    constexpr int32_t nr_of_evses = 2;

    this->reconfigure_for_nr_of_evses(nr_of_evses);

    // Note the time is 1 minute off the whole hour
    const DateTime start_time = ocpp::DateTime("2024-01-02T08:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:00:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, STATION_WIDE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 32.0F, 3),
                    PeriodEqualsWithPhases( 1800, 30.0F, 1),
                    PeriodEqualsWithPhases( 2700, 28.0F, 3)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, MixingNumberOfPhasesOnSingleEvse) {
    this->load_charging_profiles_for_evse("singles/ChargingStationMaxProfile_24_Ampere.json", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse("singles/Relative_302_phase_limit.json", DEFAULT_EVSE_ID);

    // Note the time is 40 minute off the whole hour
    const DateTime start_time = ocpp::DateTime("2024-01-02T00:40:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T01:40:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, DEFAULT_EVSE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 16.0F, 1),
                    PeriodEqualsWithPhases( 1200, 16.0F, 3),
                    PeriodEqualsWithPhases( 1800, 15.0F, 1),
                    PeriodEqualsWithPhases( 2700, 14.0F, 3)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixtureV2, NoGapsWithSequentialProfiles) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH + "/no_gap/", STATION_WIDE_ID);

    const DateTime start_time = ocpp::DateTime("2024-01-02T08:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T08:20:00");

    CompositeSchedule result = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::A, false, true);

    EXPECT_EQ(result.evseId, DEFAULT_EVSE_ID);
    EXPECT_EQ(result.scheduleStart, start_time);
    EXPECT_EQ(result.duration, 1200);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnitEnum::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEquals(   0, 16.0F),
                    PeriodEquals( 300, 12.0F),
                    PeriodEquals( 600, DEFAULT_LIMIT_AMPERE)
                ));
    // clang-format on
}
} // namespace ocpp::v2
