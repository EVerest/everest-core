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
const static std::string SCHEMAS_PATH = "./resources/example_config/v201/component_config";
const static std::string CONFIG_PATH = "./resources/example_config/v201/config.json";
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
        auto charging_schedule_period = ChargingSchedulePeriod{
            .startPeriod = start_period,
            .numberPhases = number_phases,
            .phaseToUse = phase_to_use,
        };

        return {charging_schedule_period};
    }

    std::vector<ChargingSchedulePeriod> create_charging_schedule_periods(std::vector<int32_t> start_periods) {
        auto charging_schedule_periods = std::vector<ChargingSchedulePeriod>();
        for (auto start_period : start_periods) {
            auto charging_schedule_period = ChargingSchedulePeriod{
                .startPeriod = start_period,
            };
            charging_schedule_periods.push_back(charging_schedule_period);
        }

        return charging_schedule_periods;
    }

    std::vector<ChargingSchedulePeriod>
    create_charging_schedule_periods_with_phases(int32_t start_period, int32_t numberPhases, int32_t phaseToUse) {
        auto charging_schedule_period =
            ChargingSchedulePeriod{.startPeriod = start_period, .numberPhases = numberPhases, .phaseToUse = phaseToUse};

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
        return ChargingProfile{.id = charging_profile_id,
                               .stackLevel = stack_level,
                               .chargingProfilePurpose = charging_profile_purpose,
                               .chargingProfileKind = charging_profile_kind,
                               .chargingSchedule = charging_schedules,
                               .customData = {},
                               .recurrencyKind = recurrency_kind,
                               .validFrom = validFrom,
                               .validTo = validTo,
                               .transactionId = transaction_id};
    }

    void create_device_model_db(const std::string& path) {
        InitDeviceModelDb db(path, MIGRATION_FILES_PATH);
        db.initialize_database(SCHEMAS_PATH, true);
        // db.initialize_database(CONFIG_PATH, false);
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
    CompositeSchedule expected = {
        .chargingSchedulePeriod = {{
                                       .startPeriod = 0,
                                       .limit = 1.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 3600,
                                       .limit = 2.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 7200,
                                       .limit = 3.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 10800,
                                       .limit = 4.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 14400,
                                       .limit = 5.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 18000,
                                       .limit = 6.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 21600,
                                       .limit = 7.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 25200,
                                       .limit = 8.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 28800,
                                       .limit = 9.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 32400,
                                       .limit = 10.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 36000,
                                       .limit = 11.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 39600,
                                       .limit = 12.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 43200,
                                       .limit = 13.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 46800,
                                       .limit = 14.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 50400,
                                       .limit = 15.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 54000,
                                       .limit = 16.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 57600,
                                       .limit = 17.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 61200,
                                       .limit = 18.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 64800,
                                       .limit = 19.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 68400,
                                       .limit = 20.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 72000,
                                       .limit = 21.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 75600,
                                       .limit = 22.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 79200,
                                       .limit = 23.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 82800,
                                       .limit = 24.0,
                                       .numberPhases = 1,
                                   }},
        .evseId = DEFAULT_EVSE_ID,
        .duration = 86400,
        .scheduleStart = start_time,
        .chargingRateUnit = ChargingRateUnitEnum::W,
    };

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

        CompositeSchedule expected = {
            .chargingSchedulePeriod = {{
                .startPeriod = 0,
                .limit = 19.0,
                .numberPhases = 1,
            }},
            .evseId = DEFAULT_EVSE_ID,
            .duration = 1080,
            .scheduleStart = start_time,
            .chargingRateUnit = ChargingRateUnitEnum::W,
        };

        CompositeSchedule actual = handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID,
                                                                        ChargingRateUnitEnum::W);

        ASSERT_EQ(actual, expected);
    }

    // Time Window: START = Stack #1 start time || END = After Stack #1 end time Before next Start #0 start time
    {
        const DateTime start_time = ocpp::DateTime("2024-01-17T18:04:00");
        const DateTime end_time = ocpp::DateTime("2024-01-17T18:33:00");

        CompositeSchedule expected = {
            .chargingSchedulePeriod = {{
                                           .startPeriod = 0,
                                           .limit = 2000.0,
                                           .numberPhases = 1,
                                       },
                                       {
                                           .startPeriod = 1080,
                                           .limit = 19.0,
                                           .numberPhases = 1,
                                       }},
            .evseId = DEFAULT_EVSE_ID,
            .duration = 1740,
            .scheduleStart = start_time,
            .chargingRateUnit = ChargingRateUnitEnum::W,
        };

        CompositeSchedule actual = handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID,
                                                                        ChargingRateUnitEnum::W);

        ASSERT_EQ(actual, expected);
    }

    // Time Window: START = Stack #1 start time || END = After next Start #0 start time
    {
        const DateTime start_time = ocpp::DateTime("2024-01-17T18:04:00");
        const DateTime end_time = ocpp::DateTime("2024-01-17T19:04:00");

        CompositeSchedule expected = {
            .chargingSchedulePeriod = {{
                                           .startPeriod = 0,
                                           .limit = 2000.0,
                                           .numberPhases = 1,
                                       },
                                       {
                                           .startPeriod = 1080,
                                           .limit = 19.0,
                                           .numberPhases = 1,
                                       },
                                       {
                                           .startPeriod = 3360,
                                           .limit = 20.0,
                                           .numberPhases = 1,
                                       }},
            .evseId = DEFAULT_EVSE_ID,
            .duration = 3600,
            .scheduleStart = start_time,
            .chargingRateUnit = ChargingRateUnitEnum::W,
        };

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

    CompositeSchedule expected = {
        .chargingSchedulePeriod = {{
            .startPeriod = 0,
            .limit = 2000.0,
            .numberPhases = 1,
        }},
        .evseId = DEFAULT_EVSE_ID,
        .duration = 60,
        .scheduleStart = start_time,
        .chargingRateUnit = ChargingRateUnitEnum::W,
    };

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_LayeredTest_PreviousStartTime) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_file("singles/TXProfile_Absolute_Start18-04.json");
    const DateTime start_time = ocpp::DateTime("2024-01-17T18:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-17T18:05:00");
    CompositeSchedule expected = {
        .chargingSchedulePeriod = {{
                                       .startPeriod = 0,
                                       .limit = DEFAULT_LIMIT_WATTS,
                                       .numberPhases = DEFAULT_AND_MAX_NUMBER_PHASES,
                                   },
                                   {
                                       .startPeriod = 240,
                                       .limit =
                                           profiles.at(0).chargingSchedule.front().chargingSchedulePeriod.front().limit,
                                       .numberPhases = 1,
                                   }},
        .evseId = DEFAULT_EVSE_ID,
        .duration = 300,
        .scheduleStart = start_time,
        .chargingRateUnit = ChargingRateUnitEnum::W,
    };

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);

    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_LayeredRecurringTest_PreviousStartTime) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/layered_recurring/");
    const DateTime start_time = ocpp::DateTime("2024-02-19T18:00:00");
    const DateTime end_time = ocpp::DateTime("2024-02-19T19:04:00");
    CompositeSchedule expected = {
        .chargingSchedulePeriod =
            {{
                 .startPeriod = 0,
                 .limit = 19.0,
                 .numberPhases = 1,
             },
             {
                 .startPeriod = 240,
                 .limit = profiles.back().chargingSchedule.front().chargingSchedulePeriod.front().limit,
                 .numberPhases = 1,
             },
             {
                 .startPeriod = 1320,
                 .limit = 19.0,
                 .numberPhases = 1,
             },
             {
                 .startPeriod = 3600,
                 .limit = 20.0,
                 .numberPhases = 1,
             }},
        .evseId = DEFAULT_EVSE_ID,
        .duration = 3840,
        .scheduleStart = start_time,
        .chargingRateUnit = ChargingRateUnitEnum::W,
    };

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
    CompositeSchedule expected = {
        .chargingSchedulePeriod = {{
                                       .startPeriod = 0,
                                       .limit = 2000.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 1020,
                                       .limit = 11000.0,
                                       .numberPhases = 3,
                                   },
                                   {
                                       .startPeriod = 25140,
                                       .limit = 6000.0,
                                       .numberPhases = 3,
                                   }},
        .evseId = DEFAULT_EVSE_ID,
        .duration = 43140,
        .scheduleStart = start_time,
        .chargingRateUnit = ChargingRateUnitEnum::W,
    };

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

    CompositeSchedule expected = {
        .chargingSchedulePeriod = {{
            .startPeriod = 0,
            .limit = 2000.0,
            .numberPhases = 1,
        }},
        .evseId = DEFAULT_EVSE_ID,
        .duration = 3600,
        .scheduleStart = start_time,
        .chargingRateUnit = ChargingRateUnitEnum::W,
    };

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

    CompositeSchedule expected = {
        .chargingSchedulePeriod = {{
                                       .startPeriod = 0,
                                       .limit = 2000.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 3601,
                                       .limit = 7.0,
                                       .numberPhases = 1,
                                   }},
        .evseId = DEFAULT_EVSE_ID,
        .duration = 3660,
        .scheduleStart = start_time,
        .chargingRateUnit = ChargingRateUnitEnum::W,
    };

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

    CompositeSchedule expected = {
        .chargingSchedulePeriod = {{
                                       .startPeriod = 0,
                                       .limit = 2000.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 1080,
                                       .limit = 11000.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 25200,
                                       .limit = 6000.0,
                                       .numberPhases = 1,
                                   }},
        .evseId = DEFAULT_EVSE_ID,
        .duration = 43200,
        .scheduleStart = start_time,
        .chargingRateUnit = ChargingRateUnitEnum::W,
    };

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

    CompositeSchedule expected = {
        .chargingSchedulePeriod = {{
                                       .startPeriod = 0,
                                       .limit = 11000.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 25200,
                                       .limit = 6000.0,
                                       .numberPhases = 1,
                                   }},
        .evseId = DEFAULT_EVSE_ID,
        .duration = 43200,
        .scheduleStart = start_time,
        .chargingRateUnit = ChargingRateUnitEnum::W,
    };

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);

    ASSERT_EQ(ProfileValidationResultEnum::Valid, handler.validate_profile(profiles.at(0), DEFAULT_EVSE_ID));
    ASSERT_EQ(ProfileValidationResultEnum::Valid, handler.validate_profile(profiles.at(1), DEFAULT_EVSE_ID));
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_MaxOverridesHigherLimits) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/max/");

    const DateTime start_time = ocpp::DateTime("2024-01-17T00:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-17T02:00:00");

    CompositeSchedule expected = {
        .chargingSchedulePeriod = {{
                                       .startPeriod = 0,
                                       .limit = 10.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 3600,
                                       .limit = 20.0,
                                       .numberPhases = 1,
                                   }},
        .evseId = DEFAULT_EVSE_ID,
        .duration = 7200,
        .scheduleStart = start_time,
        .chargingRateUnit = ChargingRateUnitEnum::W,
    };

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_MaxOverridenByLowerLimits) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/max/");

    const DateTime start_time = ocpp::DateTime("2024-01-17T22:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-18T00:00:00");

    CompositeSchedule expected = {
        .chargingSchedulePeriod = {{
                                       .startPeriod = 0,
                                       .limit = 230.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 3600,
                                       .limit = 10.0,
                                       .numberPhases = 1,
                                   }},
        .evseId = DEFAULT_EVSE_ID,
        .duration = 7200,
        .scheduleStart = start_time,
        .chargingRateUnit = ChargingRateUnitEnum::W,
    };

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_ExternalOverridesHigherLimits) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/external/");

    const DateTime start_time = ocpp::DateTime("2024-01-17T00:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-17T02:00:00");

    CompositeSchedule expected = {
        .chargingSchedulePeriod = {{
                                       .startPeriod = 0,
                                       .limit = 10.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 3600,
                                       .limit = 20.0,
                                       .numberPhases = 1,
                                   }},
        .evseId = DEFAULT_EVSE_ID,
        .duration = 7200,
        .scheduleStart = start_time,
        .chargingRateUnit = ChargingRateUnitEnum::W,
    };

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV201, K08_CalculateCompositeSchedule_ExternalOverridenByLowerLimits) {
    std::vector<ChargingProfile> profiles =
        SmartChargingTestUtils::get_charging_profiles_from_directory(BASE_JSON_PATH + "/external/");

    const DateTime start_time = ocpp::DateTime("2024-01-17T22:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-18T00:00:00");

    CompositeSchedule expected = {
        .chargingSchedulePeriod = {{
                                       .startPeriod = 0,
                                       .limit = 230.0,
                                       .numberPhases = 1,
                                   },
                                   {
                                       .startPeriod = 3600,
                                       .limit = 10.0,
                                       .numberPhases = 1,
                                   }},
        .evseId = DEFAULT_EVSE_ID,
        .duration = 7200,
        .scheduleStart = start_time,
        .chargingRateUnit = ChargingRateUnitEnum::W,
    };

    CompositeSchedule actual =
        handler.calculate_composite_schedule(profiles, start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W);
    ASSERT_EQ(actual, expected);
}

} // namespace ocpp::v201