// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <filesystem>
#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <vector>
namespace fs = std::filesystem;

#include "everest/logging.hpp"
#include <database_handler_mock.hpp>
#include <evse_security_mock.hpp>
#include <ocpp/common/call_types.hpp>
#include <ocpp/common/constants.hpp>
#include <ocpp/common/evse_security_impl.hpp>
#include <ocpp/v16/charge_point_state_machine.hpp>
#include <ocpp/v16/smart_charging.hpp>
#include <optional>
#include <smart_charging_matchers.hpp>

namespace ocpp {
namespace v16 {

constexpr int STATION_WIDE_ID = 0;
constexpr int DEFAULT_EVSE_ID = 1;

/**
 * CompositeSchedule Test Fixture
 */
class CompositeScheduleTestFixture : public testing::Test {
protected:
    void SetUp() override {
        this->evse_security = std::make_shared<EvseSecurityMock>();
        std::ifstream ifs(CONFIG_FILE_LOCATION_V16);
        const std::string config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        this->configuration =
            std::make_unique<ChargePointConfiguration>(config_file, CONFIG_DIR_V16, USER_CONFIG_FILE_LOCATION_V16);
    }

    void add_connector(int id, bool start_transaction) {
        auto connector = std::make_shared<Connector>(id);

        auto timer = std::unique_ptr<Everest::SteadyTimer>();

        if (start_transaction) {
            connector->transaction = std::make_shared<Transaction>(-1, id, "test", "test", 1, std::nullopt,
                                                                   ocpp::DateTime(), std::move(timer));
        }
        connectors[id] = connector;
    }

    SmartChargingHandler* create_smart_charging_handler(const int number_of_connectors, bool start_transaction = true) {
        for (int i = 0; i <= number_of_connectors; i++) {
            add_connector(i, start_transaction);
        }

        const std::string chargepoint_id = "1";
        const fs::path database_path = "na";
        const fs::path init_script_path = "na";

        auto database = std::make_unique<everest::db::sqlite::Connection>(database_path / (chargepoint_id + ".db"));
        std::shared_ptr<testing::NiceMock<DatabaseHandlerMock>> database_handler =
            std::make_shared<testing::NiceMock<DatabaseHandlerMock>>(std::move(database), init_script_path);

        auto handler = new SmartChargingHandler(connectors, database_handler, *configuration);

        return handler;
    }

    ChargingProfile get_charging_profile_from_file(const std::string& filename) {
        const std::string base_path = std::string(TEST_PROFILES_LOCATION_V16) + "/json/";
        const std::string full_path = base_path + filename;

        std::ifstream f(full_path.c_str());
        json data = json::parse(f);

        ChargingProfile cp;
        from_json(data, cp);
        return cp;
    }

    std::string get_log_duration_string(int32_t duration) {
        if (duration < 1) {
            return "0 Seconds ";
        }

        int32_t remaining = duration;

        std::string log_str = "";

        if (remaining >= 86400) {
            int32_t days = remaining / 86400;
            remaining = remaining % 86400;
            if (days > 1) {
                log_str += std::to_string(days) + " Days ";
            } else {
                log_str += std::to_string(days) + " Day ";
            }
        }
        if (remaining >= 3600) {
            int32_t hours = remaining / 3600;
            remaining = remaining % 3600;
            log_str += std::to_string(hours) + " Hours ";
        }
        if (remaining >= 60) {
            int32_t minutes = remaining / 60;
            remaining = remaining % 60;
            log_str += std::to_string(minutes) + " Minutes ";
        }
        if (remaining > 0) {
            log_str += std::to_string(remaining) + " Seconds ";
        }
        return log_str;
    }

    void log_duration(int32_t duration) {
        EVLOG_info << get_log_duration_string(duration);
    }

    void log_me(ChargingProfile& cp) {
        json cp_json;
        to_json(cp_json, cp);

        EVLOG_info << "  ChargingProfile> " << cp_json.dump(4);
        log_duration(cp.chargingSchedule.duration.value_or(0));
    }

    void log_me(std::vector<ChargingProfile> profiles) {
        EVLOG_info << "[";
        for (auto& profile : profiles) {
            log_me(profile);
        }
        EVLOG_info << "]";
    }

    void log_me(EnhancedChargingSchedule& ecs) {
        json ecs_json;
        to_json(ecs_json, ecs);

        EVLOG_info << "EnhancedChargingSchedule> " << ecs_json.dump(4);
    }

    void log_me(EnhancedChargingSchedule& ecs, const DateTime start_time) {
        log_me(ecs);
        EVLOG_info << "Start Time> " << start_time.to_rfc3339();

        int32_t i = 0;
        for (auto& period : ecs.chargingSchedulePeriod) {
            i++;
            int32_t numberPhases = 0;
            if (period.numberPhases.has_value()) {
                numberPhases = period.numberPhases.value();
            }
            EVLOG_info << "   period #" << i << " {limit: " << period.limit << " numberPhases:" << numberPhases
                       << " stackLevel:" << period.stackLevel << "} starts "
                       << get_log_duration_string(period.startPeriod) << "in";
        }
        if (ecs.duration.has_value()) {
            EVLOG_info << "   period #" << i << " ends after " << get_log_duration_string(ecs.duration.value());
        } else {
            EVLOG_info << "   period #" << i << " ends in 0 Seconds";
        }
    }

    /// \brief Returns a vector of ChargingProfiles to be used as a baseline for testing core functionality
    /// of generating an EnhancedChargingSchedule.
    std::vector<ChargingProfile> get_baseline_profile_vector() {
        auto profile_01 = get_charging_profile_from_file("TxDefaultProfile_01.json");
        // auto profile_02 = getChargingProfileFromFile("TxDefaultProfile_02.json");
        auto profile_100 = get_charging_profile_from_file("TxDefaultProfile_100.json");
        // return {profile_01, profile_02, profile_100};
        return {profile_01, profile_100};
    }

    // Default values used within the tests
    std::map<int32_t, std::shared_ptr<Connector>> connectors;
    std::shared_ptr<DatabaseHandler> database_handler;
    std::shared_ptr<EvseSecurityMock> evse_security;
    std::unique_ptr<ChargePointConfiguration> configuration;
};

TEST_F(CompositeScheduleTestFixture, CalculateEnhancedCompositeSchedule_ValidatedBaseline) {
    // GTEST_SKIP();
    auto handler = create_smart_charging_handler(1);
    std::vector<ChargingProfile> profiles = get_baseline_profile_vector();
    handler->add_tx_default_profile(profiles.at(0), 1);
    handler->add_tx_default_profile(profiles.at(1), 1);
    log_me(profiles);

    const DateTime my_date_start_range = ocpp::DateTime("2024-01-17T18:01:00");
    const DateTime my_date_end_range = ocpp::DateTime("2024-01-18T06:00:00");

    EVLOG_info << "    Start> " << my_date_start_range.to_rfc3339();
    EVLOG_info << "      End> " << my_date_end_range.to_rfc3339();

    auto composite_schedule = handler->calculate_enhanced_composite_schedule(
        my_date_start_range, my_date_end_range, 1, profiles.at(0).chargingSchedule.chargingRateUnit, false, true);

    log_me(composite_schedule, my_date_start_range);

    ASSERT_EQ(composite_schedule.chargingRateUnit, ChargingRateUnit::W);
    ASSERT_EQ(composite_schedule.duration, 43140);
    ASSERT_EQ(profiles.size(), 2);
    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.size(), 3);
    auto& period_01 = composite_schedule.chargingSchedulePeriod.at(0);
    ASSERT_EQ(period_01.limit, 2000);
    ASSERT_EQ(period_01.numberPhases, 1);
    ASSERT_EQ(period_01.stackLevel, 1);
    ASSERT_EQ(period_01.startPeriod, 0);
    ASSERT_EQ(period_01.periodTransformed, false);
    auto& period_02 = composite_schedule.chargingSchedulePeriod.at(1);
    ASSERT_EQ(period_02.limit, 11000);
    ASSERT_EQ(period_02.numberPhases, 3);
    ASSERT_EQ(period_02.stackLevel, 0);
    ASSERT_EQ(period_02.startPeriod, 1020);
    ASSERT_EQ(period_02.periodTransformed, false);
    auto& period_03 = composite_schedule.chargingSchedulePeriod.at(2);
    ASSERT_EQ(period_03.limit, 6000.0);
    ASSERT_EQ(period_03.numberPhases, 3);
    ASSERT_EQ(period_03.stackLevel, 0);
    ASSERT_EQ(period_03.startPeriod, 25140);
    ASSERT_EQ(period_03.periodTransformed, false);
}

///
/// This was a defect in the earier 1.6 code that has now been fixed. Basically a tight test with a higher
/// stack Profile that is Absolute but marked with a recurrencyKind of Daily.
///
TEST_F(CompositeScheduleTestFixture, CalculateEnhancedCompositeSchedule_TightLayeredTestWithAbsoluteProfile) {
    auto handler = create_smart_charging_handler(1);

    ChargingProfile profile_grid = get_charging_profile_from_file("TxProfile_grid.json");
    ChargingProfile txprofile_03 = get_charging_profile_from_file("TxProfile_03_Absolute.json");
    std::vector<ChargingProfile> profiles = {profile_grid, txprofile_03};

    handler->add_tx_profile(txprofile_03, 1);
    handler->add_tx_default_profile(profile_grid, 1);

    const DateTime my_date_start_range = ocpp::DateTime("2024-01-18T18:04:00");
    const DateTime my_date_end_range = ocpp::DateTime("2024-01-18T18:22:00");

    EVLOG_info << "    Start> " << my_date_start_range.to_rfc3339();
    EVLOG_info << "      End> " << my_date_end_range.to_rfc3339();

    auto composite_schedule = handler->calculate_enhanced_composite_schedule(
        my_date_start_range, my_date_end_range, 1, profiles.at(0).chargingSchedule.chargingRateUnit, false, true);

    log_me(composite_schedule, my_date_start_range);

    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.size(), 1);
    ASSERT_EQ(composite_schedule.duration, 1080);
    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.at(0).limit, 19.0);
}

///
/// A tight CompositeSchedude test is one where the start and end times exactly match the time window of the
/// highest stack level.
///
TEST_F(CompositeScheduleTestFixture, CalculateEnhancedCompositeSchedule_TightLayeredTest) {
    auto handler = create_smart_charging_handler(1);

    ChargingProfile profile_grid = get_charging_profile_from_file("TxProfile_grid.json");
    ChargingProfile txprofile_02 = get_charging_profile_from_file("TxProfile_02.json");
    std::vector<ChargingProfile> profiles = {profile_grid, txprofile_02};

    handler->add_tx_profile(txprofile_02, 1);
    handler->add_tx_default_profile(profile_grid, 1);

    const DateTime my_date_start_range = ocpp::DateTime("2024-01-18T18:04:00");
    const DateTime my_date_end_range = ocpp::DateTime("2024-01-18T18:22:00");

    EVLOG_info << "    Start> " << my_date_start_range.to_rfc3339();
    EVLOG_info << "      End> " << my_date_end_range.to_rfc3339();

    auto composite_schedule = handler->calculate_enhanced_composite_schedule(
        my_date_start_range, my_date_end_range, 1, profiles.at(0).chargingSchedule.chargingRateUnit, false, true);

    log_me(composite_schedule, my_date_start_range);

    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.size(), 1);
    ASSERT_EQ(composite_schedule.duration, 1080);
    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.at(0).limit, 2000.0);
}

///
/// A fat CompositeSchedude test is one where the start time begins before the time window of the highest stack level
/// profile, and end time is afterwards.
///
TEST_F(CompositeScheduleTestFixture, CalculateEnhancedCompositeSchedule_FatLayeredTest) {
    auto handler = create_smart_charging_handler(1);

    ChargingProfile profile_grid = get_charging_profile_from_file("TxProfile_grid.json");
    ChargingProfile txprofile_02 = get_charging_profile_from_file("TxProfile_02.json");
    std::vector<ChargingProfile> profiles = {profile_grid, txprofile_02};

    handler->add_tx_profile(profile_grid, 1);
    handler->add_tx_profile(txprofile_02, 1);

    const DateTime my_date_start_range = ocpp::DateTime("2024-01-18T18:02:00");
    const DateTime my_date_end_range = ocpp::DateTime("2024-01-18T18:24:00");

    EVLOG_info << "    Start> " << my_date_start_range.to_rfc3339();
    EVLOG_info << "      End> " << my_date_end_range.to_rfc3339();

    auto composite_schedule = handler->calculate_enhanced_composite_schedule(
        my_date_start_range, my_date_end_range, 1, profiles.at(0).chargingSchedule.chargingRateUnit, false, true);

    log_me(composite_schedule, my_date_start_range);

    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.size(), 3);
    ASSERT_EQ(composite_schedule.duration, 1320);
    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.at(0).limit, 19.0);
    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.at(1).limit, 2000.0);
    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.at(1).startPeriod, 120.0);
    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.at(2).limit, 19.0);
    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.at(2).startPeriod, 1200.0);
}

///
/// A simple test to verify that the generated CompositeSchedule where the start and end times
/// are thin, aka they fall inside a specific ChargingSchedulePeriod's time window
///
TEST_F(CompositeScheduleTestFixture, CalculateEnhancedCompositeSchedule_ThinTest) {
    auto handler = create_smart_charging_handler(1);

    ChargingProfile profile_grid = get_charging_profile_from_file("TxProfile_grid.json");
    std::vector<ChargingProfile> profiles = {profile_grid};

    handler->add_tx_profile(profile_grid, 1);

    const DateTime my_date_start_range = ocpp::DateTime("2024-01-18T18:04:00");
    const DateTime my_date_end_range = ocpp::DateTime("2024-01-18T18:22:00");

    EVLOG_info << "    Start> " << my_date_start_range.to_rfc3339();
    EVLOG_info << "      End> " << my_date_end_range.to_rfc3339();

    auto composite_schedule = handler->calculate_enhanced_composite_schedule(
        my_date_start_range, my_date_end_range, 1, profiles.at(0).chargingSchedule.chargingRateUnit, false, true);

    log_me(composite_schedule, my_date_start_range);

    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.size(), 1);
    ASSERT_EQ(composite_schedule.duration, 1080);
    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.at(0).limit, 19.0);
}

TEST_F(CompositeScheduleTestFixture, NoSchedulesPresent) {
    auto handler = create_smart_charging_handler(1, false);
    const DateTime start_time = ocpp::DateTime("2024-01-02T00:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T01:00:00");

    auto result =
        handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEquals(0, configuration->getCompositeScheduleDefaultLimitAmps().value_or(DEFAULT_LIMIT_AMPS))
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixture, ExtraSeconds) {
    auto handler = create_smart_charging_handler(1, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("Absolute_301.json");
    handler->add_tx_default_profile(profile_grid, 0);

    const DateTime start_time = ocpp::DateTime("2024-01-01T12:01:59");
    const DateTime end_time = ocpp::DateTime("2024-01-01T13:02:01");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3602);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEquals(   0, configuration->getCompositeScheduleDefaultLimitAmps().value_or(DEFAULT_LIMIT_AMPS)),
                    PeriodEquals(   1, 32.0F),
                    PeriodEquals(1801, 31.0F),
                    PeriodEquals(2701, 30.0F),
                    PeriodEquals(3601, configuration->getCompositeScheduleDefaultLimitAmps().value_or(DEFAULT_LIMIT_AMPS))
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixture, SingleStationMaxForEvse0) {
    auto handler = create_smart_charging_handler(1, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("ChargingStationMaxProfile_401.json");
    handler->add_charge_point_max_profile(profile_grid);

    const DateTime start_time = ocpp::DateTime("2024-08-21T08:00:00");
    const DateTime end_time = ocpp::DateTime("2024-08-21T09:00:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID, ChargingRateUnit::A, false, false);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

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

TEST_F(CompositeScheduleTestFixture, SingleTxDefaultProfileForEvse0WithSingleEvse) {
    auto handler = create_smart_charging_handler(1, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("Relative_303.json");
    handler->add_tx_default_profile(profile_grid, 0);

    const DateTime start_time = ocpp::DateTime("2024-01-02T08:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:00:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEquals(    0, 16.0F),
                    PeriodEquals( 1800, 15.0F),
                    PeriodEquals( 2700, 14.0F)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixture, SingleTxDefaultProfileForEvse0WithMultipleEvses_Current) {
    auto handler = create_smart_charging_handler(2, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("Relative_303.json");
    handler->add_tx_default_profile(profile_grid, 0);

    const DateTime start_time = ocpp::DateTime("2024-01-02T08:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:00:00");

    constexpr int32_t nr_of_evses = 2;

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEquals(    0, nr_of_evses * 16.0F ),
                    PeriodEquals( 1800, nr_of_evses * 15.0F ),
                    PeriodEquals( 2700, nr_of_evses * 14.0F )
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixture, SingleTxDefaultProfileForEvse0WithMultipleEvses_Power) {
    auto handler = create_smart_charging_handler(2, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("TXDefaultProfile_25_Watt.json");
    handler->add_tx_default_profile(profile_grid, 0);

    // Note the time is 1 minute off the whole hour
    const DateTime start_time = ocpp::DateTime("2024-01-02T08:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:00:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEquals(    0, 36.0F   ),
                    PeriodEquals(  300, 24.0F   ),
                    PeriodEquals(  600, 18.0F   ),
                    PeriodEquals(  900, 12.0F   ),
                    PeriodEquals( 1200,  6.0F   )
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixture, SingleTxDefaultProfileWithStationMaxForEvse0WithSingleEvse) {
    auto handler = create_smart_charging_handler(1, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("ChargingStationMaxProfile_401.json");
    ChargingProfile profile_tx_default = get_charging_profile_from_file("Relative_303.json");

    handler->add_charge_point_max_profile(profile_grid);
    handler->add_tx_default_profile(profile_tx_default, 1);

    // Note the time is 1 minute off the whole hour
    const DateTime start_time = ocpp::DateTime("2024-01-02T08:01:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:01:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 16.0F, 1),
                    PeriodEqualsWithPhases( 1800, 15.0F, 1),
                    PeriodEqualsWithPhases( 2700, 14.0F, 1)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixture, SingleTxDefaultProfileWithStationMaxForEvse0WithMultipleEvses) {
    auto handler = create_smart_charging_handler(2, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("ChargingStationMaxProfile_401.json");
    ChargingProfile profile_tx_default_1 = get_charging_profile_from_file("Relative_303.json");
    ChargingProfile profile_tx_default_2 = get_charging_profile_from_file("Relative_303.json");

    handler->add_charge_point_max_profile(profile_grid);
    handler->add_tx_default_profile(profile_tx_default_1, 1);
    handler->add_tx_default_profile(profile_tx_default_2, 2);

    // Note the time is 1 minute off the whole hour
    const DateTime start_time = ocpp::DateTime("2024-01-02T08:01:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:01:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

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

TEST_F(CompositeScheduleTestFixture, TxProfilePerEvseWithMultipleEvses) {
    auto handler = create_smart_charging_handler(2, false);
    ChargingProfile profile_tx_default_1 = get_charging_profile_from_file("Recurring_Daily_301.json");
    ChargingProfile profile_tx_default_2 = get_charging_profile_from_file("Relative_303.json");

    handler->add_tx_default_profile(profile_tx_default_1, 1);
    handler->add_tx_default_profile(profile_tx_default_2, 2);

    // Note the time is 1 minute off the whole hour
    const DateTime start_time = ocpp::DateTime("2024-01-02T08:01:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:01:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEquals(    0, 32.0F ),
                    PeriodEquals( 1740, 31.0F ),
                    PeriodEquals( 1800, 30.0F ),
                    PeriodEquals( 2640, 29.0F ),
                    PeriodEquals( 2700, 28.0F ),
                    PeriodEquals( 3540, 14.0F + DEFAULT_LIMIT_AMPS )
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixture, MixingCurrentAndPower_16A_1P) {
    auto handler = create_smart_charging_handler(1, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("ChargingStationMaxProfile_24_Ampere.json");
    ChargingProfile profile_tx_default = get_charging_profile_from_file("TXDefaultProfile_25_Watt.json");

    handler->add_charge_point_max_profile(profile_grid);
    handler->add_tx_default_profile(profile_tx_default, 1);

    const DateTime start_time = ocpp::DateTime("2024-01-02T00:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T01:00:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 16.0F, 1),
                    PeriodEqualsWithPhases( 1200,  9.0F, 1)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixture, MixingCurrentAndPower_16A_3P) {
    auto handler = create_smart_charging_handler(1, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("ChargingStationMaxProfile_24_Ampere.json");
    ChargingProfile profile_tx_default = get_charging_profile_from_file("TXDefaultProfile_25_Watt.json");

    handler->add_charge_point_max_profile(profile_grid);
    handler->add_tx_default_profile(profile_tx_default, 1);

    const DateTime start_time = ocpp::DateTime("2024-01-02T01:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T02:00:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

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

TEST_F(CompositeScheduleTestFixture, MixingCurrentAndPower_10A_1P) {
    auto handler = create_smart_charging_handler(1, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("ChargingStationMaxProfile_24_Ampere.json");
    ChargingProfile profile_tx_default = get_charging_profile_from_file("TXDefaultProfile_25_Watt.json");

    handler->add_charge_point_max_profile(profile_grid);
    handler->add_tx_default_profile(profile_tx_default, 1);

    const DateTime start_time = ocpp::DateTime("2024-01-02T02:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T03:00:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
        testing::ElementsAre(
            PeriodEqualsWithPhases(    0, 10.0F, 1),
            PeriodEqualsWithPhases( 1200,  9.0F, 1)
        ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixture, MixingCurrentAndPower_10A_3P) {
    auto handler = create_smart_charging_handler(1, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("ChargingStationMaxProfile_24_Ampere.json");
    ChargingProfile profile_tx_default = get_charging_profile_from_file("TXDefaultProfile_25_Watt.json");

    handler->add_charge_point_max_profile(profile_grid);
    handler->add_tx_default_profile(profile_tx_default, 1);

    const DateTime start_time = ocpp::DateTime("2024-01-02T03:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T04:00:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

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

TEST_F(CompositeScheduleTestFixture, MixingCurrentAndPower_10A_1P_RequestPower) {
    auto handler = create_smart_charging_handler(1, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("ChargingStationMaxProfile_24_Ampere.json");
    ChargingProfile profile_tx_default = get_charging_profile_from_file("TXDefaultProfile_25_Watt.json");

    handler->add_charge_point_max_profile(profile_grid);
    handler->add_tx_default_profile(profile_tx_default, 1);

    const DateTime start_time = ocpp::DateTime("2024-01-02T02:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T03:00:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnit::W, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::W);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
        testing::ElementsAre(
            PeriodEqualsWithPhases(    0, 2300.0F, 1),
            PeriodEqualsWithPhases( 1200, 2070.0F, 1)
        ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixture, MixingCurrentAndPower_10A_3P_RequestPower) {
    auto handler = create_smart_charging_handler(1, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("ChargingStationMaxProfile_24_Ampere.json");
    ChargingProfile profile_tx_default = get_charging_profile_from_file("TXDefaultProfile_25_Watt.json");

    handler->add_charge_point_max_profile(profile_grid);
    handler->add_tx_default_profile(profile_tx_default, 1);

    const DateTime start_time = ocpp::DateTime("2024-01-02T03:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T04:00:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnit::W, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::W);

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

TEST_F(CompositeScheduleTestFixture, TxProfilePerEvseWithMultipleEvses_DifferentNrOfPhases) {
    auto handler = create_smart_charging_handler(2, false);
    ChargingProfile profile_tx_default_1 = get_charging_profile_from_file("Recurring_Daily_302_phase_limit.json");
    ChargingProfile profile_tx_default_2 = get_charging_profile_from_file("Relative_302_phase_limit.json");

    handler->add_tx_default_profile(profile_tx_default_1, 1);
    handler->add_tx_default_profile(profile_tx_default_2, 2);

    // Note the time is 1 minute off the whole hour
    const DateTime start_time = ocpp::DateTime("2024-01-02T08:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T09:00:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEqualsWithPhases(    0, 32.0F, 3),
                    PeriodEqualsWithPhases( 1800, 30.0F, 1),
                    PeriodEqualsWithPhases( 2700, 28.0F, 3)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixture, MixingNumberOfPhasesOnSingleEvse) {
    auto handler = create_smart_charging_handler(1, false);
    ChargingProfile profile_grid = get_charging_profile_from_file("ChargingStationMaxProfile_24_Ampere.json");
    ChargingProfile profile_tx_default = get_charging_profile_from_file("Relative_302_phase_limit.json");

    handler->add_charge_point_max_profile(profile_grid);
    handler->add_tx_default_profile(profile_tx_default, 1);

    // Note the time is 1 minute off the whole hour
    const DateTime start_time = ocpp::DateTime("2024-01-02T00:40:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T01:40:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 3600);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

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

TEST_F(CompositeScheduleTestFixture, NoGapsWithSequentialProfiles) {
    GTEST_SKIP()
        << "profile_tx_default_2 overrides profile_tx_default_1 because it has the same purposes and stack level";
    auto handler = create_smart_charging_handler(1, false);
    ChargingProfile profile_tx_default_1 = get_charging_profile_from_file("TxDefaultProfile_401.json");
    ChargingProfile profile_tx_default_2 = get_charging_profile_from_file("TxDefaultProfile_402.json");

    handler->add_tx_default_profile(profile_tx_default_1, 0);
    handler->add_tx_default_profile(profile_tx_default_2, 0);

    const DateTime start_time = ocpp::DateTime("2024-01-02T08:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T08:20:00");

    const auto result =
        handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID, ChargingRateUnit::A, false, true);

    EXPECT_EQ(result.startSchedule, start_time);
    EXPECT_EQ(result.duration, 1200);
    EXPECT_EQ(result.chargingRateUnit, ChargingRateUnit::A);

    // clang-format off
    EXPECT_THAT(result.chargingSchedulePeriod,
                testing::ElementsAre(
                    PeriodEquals(   0, 16.0F),
                    PeriodEquals( 300, 12.0F),
                    PeriodEquals( 600, DEFAULT_LIMIT_AMPS)
                ));
    // clang-format on
}

TEST_F(CompositeScheduleTestFixture, TxDefaultConnector0) {

    auto handler = create_smart_charging_handler(2, false);
    ChargingProfile profile_tx_default_1 = get_charging_profile_from_file("TxDefault_relative.json");

    handler->add_tx_default_profile(profile_tx_default_1, 0);

    const DateTime start_time = ocpp::DateTime("2024-01-02T08:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-02T08:20:00");

    auto result_con0 =
        handler->calculate_composite_schedule(start_time, end_time, STATION_WIDE_ID, ChargingRateUnit::W, false, true);

    EXPECT_EQ(result_con0.startSchedule, start_time);
    EXPECT_EQ(result_con0.duration, 1200);
    EXPECT_EQ(result_con0.chargingRateUnit, ChargingRateUnit ::W);

    EXPECT_THAT(result_con0.chargingSchedulePeriod,
                testing::ElementsAre(PeriodEquals(0, 2000.0F), PeriodEquals(70, 14000.0F), PeriodEquals(140, 0.0F),
                                     PeriodEquals(210, 12000.0F), PeriodEquals(280, 3000.0F),
                                     PeriodEquals(350, 10000.0F)));

    const auto result_con1 =
        handler->calculate_composite_schedule(start_time, end_time, 1, ChargingRateUnit::W, false, true);

    EXPECT_EQ(result_con1.startSchedule, start_time);
    EXPECT_EQ(result_con1.duration, 1200);
    EXPECT_EQ(result_con1.chargingRateUnit, ChargingRateUnit::W);

    EXPECT_THAT(result_con1.chargingSchedulePeriod,
                testing::ElementsAre(PeriodEquals(0, 1000.0F), PeriodEquals(70, 7000.0F), PeriodEquals(140, 0.0F),
                                     PeriodEquals(210, 6000.0F), PeriodEquals(280, 1500.0F),
                                     PeriodEquals(350, 5000.0F)));

    const auto result_con2 =
        handler->calculate_composite_schedule(start_time, end_time, 2, ChargingRateUnit::W, false, true);

    EXPECT_EQ(result_con2.startSchedule, start_time);
    EXPECT_EQ(result_con2.duration, 1200);
    EXPECT_EQ(result_con2.chargingRateUnit, ChargingRateUnit::W);

    EXPECT_THAT(result_con2.chargingSchedulePeriod,
                testing::ElementsAre(PeriodEquals(0, 1000.0F), PeriodEquals(70, 7000.0F), PeriodEquals(140, 0.0F),
                                     PeriodEquals(210, 6000.0F), PeriodEquals(280, 1500.0F),
                                     PeriodEquals(350, 5000.0F)));

    const DateTime tx_start_time = ocpp::DateTime("2024-01-02T08:00:00");
    const DateTime request_time = ocpp::DateTime("2024-01-02T08:03:00");
    auto timer = std::unique_ptr<Everest::SteadyTimer>();
    connectors.at(DEFAULT_EVSE_ID)->transaction = std::make_shared<Transaction>(
        -1, DEFAULT_EVSE_ID, "test", "test", 1, std::nullopt, tx_start_time, std::move(timer));

    const auto result_con1_tx_active =
        handler->calculate_composite_schedule(request_time, end_time, 1, ChargingRateUnit::W, false, true);

    EXPECT_EQ(result_con1_tx_active.startSchedule, request_time);
    EXPECT_EQ(result_con1_tx_active.duration, 1020);
    EXPECT_EQ(result_con1_tx_active.chargingRateUnit, ChargingRateUnit::W);

    EXPECT_THAT(result_con1_tx_active.chargingSchedulePeriod,
                testing::ElementsAre(PeriodEquals(0, 0.0F), PeriodEquals(30, 6000.0F), PeriodEquals(100, 1500.0F),
                                     PeriodEquals(170, 5000.0F)));

    result_con0 = handler->calculate_composite_schedule(request_time, end_time, STATION_WIDE_ID, ChargingRateUnit::W,
                                                        false, true);

    EXPECT_EQ(result_con0.startSchedule, request_time);
    EXPECT_EQ(result_con0.duration, 1020);
    EXPECT_EQ(result_con0.chargingRateUnit, ChargingRateUnit::W);

    EXPECT_THAT(result_con0.chargingSchedulePeriod,
                testing::ElementsAre(PeriodEquals(0, 1000.0F), PeriodEquals(30, 7000.0F), PeriodEquals(70, 13000.0F),
                                     PeriodEquals(100, 8500.0F), PeriodEquals(140, 1500.0F), PeriodEquals(170, 5000.0F),
                                     PeriodEquals(210, 11000.0F), PeriodEquals(280, 6500.0F),
                                     PeriodEquals(350, 10000.0F)));
}

} // namespace v16
} // namespace ocpp
