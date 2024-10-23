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
#include <ocpp/common/evse_security_impl.hpp>
// #include <ocpp/v16/charge_point_impl.hpp>
#include <ocpp/v16/charge_point_state_machine.hpp>
#include <ocpp/v16/smart_charging.hpp>
#include <optional>

namespace ocpp {
namespace v16 {

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

    void add_connector(int id) {
        auto connector = std::make_shared<Connector>(id);

        auto timer = std::unique_ptr<Everest::SteadyTimer>();

        connector->transaction =
            std::make_shared<Transaction>(-1, id, "test", "test", 1, std::nullopt, ocpp::DateTime(), std::move(timer));
        connectors[id] = connector;
    }

    SmartChargingHandler* create_smart_charging_handler(const int number_of_connectors) {
        for (int i = 0; i <= number_of_connectors; i++) {
            add_connector(i);
        }

        const std::string chargepoint_id = "1";
        const fs::path database_path = "na";
        const fs::path init_script_path = "na";

        auto database = std::make_unique<common::DatabaseConnection>(database_path / (chargepoint_id + ".db"));
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
    log_me(profiles);

    const DateTime my_date_start_range = ocpp::DateTime("2024-01-17T18:01:00");
    const DateTime my_date_end_range = ocpp::DateTime("2024-01-18T06:00:00");

    EVLOG_info << "    Start> " << my_date_start_range.to_rfc3339();
    EVLOG_info << "      End> " << my_date_end_range.to_rfc3339();

    auto composite_schedule = handler->calculate_enhanced_composite_schedule(
        profiles, my_date_start_range, my_date_end_range, 1, profiles.at(0).chargingSchedule.chargingRateUnit);

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
    auto& period_02 = composite_schedule.chargingSchedulePeriod.at(1);
    ASSERT_EQ(period_02.limit, 11000);
    ASSERT_EQ(period_02.numberPhases, 3);
    ASSERT_EQ(period_02.stackLevel, 0);
    ASSERT_EQ(period_02.startPeriod, 1020);
    auto& period_03 = composite_schedule.chargingSchedulePeriod.at(2);
    ASSERT_EQ(period_03.limit, 6000.0);
    ASSERT_EQ(period_03.numberPhases, 3);
    ASSERT_EQ(period_03.stackLevel, 0);
    ASSERT_EQ(period_03.startPeriod, 25140);
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

    const DateTime my_date_start_range = ocpp::DateTime("2024-01-18T18:04:00");
    const DateTime my_date_end_range = ocpp::DateTime("2024-01-18T18:22:00");

    EVLOG_info << "    Start> " << my_date_start_range.to_rfc3339();
    EVLOG_info << "      End> " << my_date_end_range.to_rfc3339();

    auto composite_schedule = handler->calculate_enhanced_composite_schedule(
        profiles, my_date_start_range, my_date_end_range, 1, profiles.at(0).chargingSchedule.chargingRateUnit);

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

    const DateTime my_date_start_range = ocpp::DateTime("2024-01-18T18:04:00");
    const DateTime my_date_end_range = ocpp::DateTime("2024-01-18T18:22:00");

    EVLOG_info << "    Start> " << my_date_start_range.to_rfc3339();
    EVLOG_info << "      End> " << my_date_end_range.to_rfc3339();

    auto composite_schedule = handler->calculate_enhanced_composite_schedule(
        profiles, my_date_start_range, my_date_end_range, 1, profiles.at(0).chargingSchedule.chargingRateUnit);

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

    const DateTime my_date_start_range = ocpp::DateTime("2024-01-18T18:02:00");
    const DateTime my_date_end_range = ocpp::DateTime("2024-01-18T18:24:00");

    EVLOG_info << "    Start> " << my_date_start_range.to_rfc3339();
    EVLOG_info << "      End> " << my_date_end_range.to_rfc3339();

    auto composite_schedule = handler->calculate_enhanced_composite_schedule(
        profiles, my_date_start_range, my_date_end_range, 1, profiles.at(0).chargingSchedule.chargingRateUnit);

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

    const DateTime my_date_start_range = ocpp::DateTime("2024-01-18T18:04:00");
    const DateTime my_date_end_range = ocpp::DateTime("2024-01-18T18:22:00");

    EVLOG_info << "    Start> " << my_date_start_range.to_rfc3339();
    EVLOG_info << "      End> " << my_date_end_range.to_rfc3339();

    auto composite_schedule = handler->calculate_enhanced_composite_schedule(
        profiles, my_date_start_range, my_date_end_range, 1, profiles.at(0).chargingSchedule.chargingRateUnit);

    log_me(composite_schedule, my_date_start_range);

    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.size(), 1);
    ASSERT_EQ(composite_schedule.duration, 1080);
    ASSERT_EQ(composite_schedule.chargingSchedulePeriod.at(0).limit, 19.0);
}

} // namespace v16
} // namespace ocpp
