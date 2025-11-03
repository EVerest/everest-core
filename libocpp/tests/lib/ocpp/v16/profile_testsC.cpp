// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

// execute: ./libocpp_unit_tests --gtest_filter=ProfileTestsC.*

#include <chrono>
#include <cstdint>
#include <optional>

#include <gtest/gtest.h>

#include "database_stub.hpp"
#include "ocpp/common/types.hpp"
#include "ocpp/v16/ocpp_enums.hpp"
#include "ocpp/v16/smart_charging.hpp"
#include "profile_tests_common.hpp"

// #define DEBUG_PROFILE
#ifdef DEBUG_PROFILE
#include "ocpp/v16/profile.hpp"
#endif

// ----------------------------------------------------------------------------
// Test anonymous namespace
namespace {
using namespace ocpp;
using namespace ocpp::v16;
using namespace std::chrono;

constexpr std::int32_t default_stack_level{0};
constexpr std::int32_t default_numberPhases{3};
constexpr float default_limit_A{48.0};
constexpr float default_limit_W{33120.0};

float to_watts(const ChargingSchedulePeriod& period) {
    const int nPhases = period.numberPhases.value_or(ocpp::v16::DEFAULT_AND_MAX_NUMBER_PHASES);
    return period.limit * nPhases * 230;
}

float to_amps(const ChargingSchedulePeriod& period) {
    const int nPhases = period.numberPhases.value_or(ocpp::v16::DEFAULT_AND_MAX_NUMBER_PHASES);
    return period.limit / (nPhases * 230);
}

// ----------------------------------------------------------------------------
// Test class

class ProfileTestsC : public stubs::DbTestBase {
protected:
    void configure_transaction(const char* transaction_start_time) {
        ocpp::DateTime transaction_start(transaction_start_time);
        configure_transaction(transaction_start);
    }

    void configure_transaction(const DateTime& transaction_start) {
        add_connectors(1);
        std::int32_t meter_start = 0;
        std::int32_t connector_id = 1;
        connectors[connector_id]->transaction = std::make_shared<Transaction>(
            -1, connector_id, "1234", "4567", meter_start, std::nullopt, transaction_start, nullptr);
    }

    void configure() {
        add_connectors(1);
    }
};

// 2024-01-01 is a Monday, daily starting at 08:00 for 10 hours
const DateTime profileRecurring_validFrom("2024-01-01T12:00:00Z");
const DateTime profileRecurring_startSchedule("2024-01-01T08:00:00Z");
const DateTime profileRecurring_validTo("2024-01-05T12:00:00Z");
const ChargingProfile profileRecurring{
    301,                                          // chargingProfileId
    5,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Recurring,           // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,            // startPeriod
                32.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                36000,        // startPeriod
                7.0,          // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        std::nullopt,                   // optional - int32_t duration
        profileRecurring_startSchedule, // optional - ocpp::DateTime - startSchedule
        std::nullopt,                   // optional - float - minChargingRate
    },                                  // chargingSchedule
    std::nullopt,                       // transactionId
    RecurrencyKindType::Daily,          // recurrencyKind
    profileRecurring_validFrom,         // validFrom
    profileRecurring_validTo,           // validTo
};

const ChargingProfile profileRecurringAlt{
    301,                                          // chargingProfileId
    5,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Recurring,           // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,            // startPeriod
                32.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        36000,                          // optional - int32_t duration
        profileRecurring_startSchedule, // optional - ocpp::DateTime - startSchedule
        std::nullopt,                   // optional - float - minChargingRate
    },                                  // chargingSchedule
    std::nullopt,                       // transactionId
    RecurrencyKindType::Daily,          // recurrencyKind
    profileRecurring_validFrom,         // validFrom
    profileRecurring_validTo,           // validTo
};

const ChargingProfile profileMinimum{
    200,                                          // chargingProfileId
    1,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Relative,            // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,            // startPeriod
                6.0,          // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        std::nullopt, // optional - int32_t duration
        std::nullopt, // optional - ocpp::DateTime - startSchedule
        std::nullopt, // optional - float - minChargingRate
    },                // chargingSchedule
    std::nullopt,     // transactionId
    std::nullopt,     // recurrencyKind
    std::nullopt,     // validFrom
    std::nullopt,     // validTo
};

// 2024-01-01 is a Monday, daily starting at 08:00 for 10 hours
const DateTime profileRelative_validFrom("2024-01-01T12:00:00Z");
const DateTime profileRelative_startSchedule("2024-01-01T08:00:00Z");
const DateTime profileRelative_validTo("2024-01-05T12:00:00Z");
const ChargingProfile profileRelative{
    301,                                          // chargingProfileId
    5,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Relative,            // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,            // startPeriod
                32.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                36000,        // startPeriod
                7.0,          // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        std::nullopt,           // optional - int32_t duration
        std::nullopt,           // optional - ocpp::DateTime - startSchedule
        std::nullopt,           // optional - float - minChargingRate
    },                          // chargingSchedule
    std::nullopt,               // transactionId
    std::nullopt,               // recurrencyKind
    profileRecurring_validFrom, // validFrom
    profileRecurring_validTo,   // validTo
};

// ----------------------------------------------------------------------------
// Test cases - check reconfiguring works

TEST_F(ProfileTestsC, DTFloor) {
    DateTime dt("2024-01-01T07:00:00.987Z");
    DateTime dt_expected("2024-01-01T07:01:02Z");
    DateTime updated(std::chrono::floor<seconds>(dt.to_time_point() + seconds(62)));

    EXPECT_EQ(updated, dt_expected);
}

// ----------------------------------------------------------------------------
// Test cases - check reconfiguring works

TEST_F(ProfileTestsC, Setup) {
    // check that re-configuring connectors works
    // note profiles are cleared as well (not in test)

    DateTime session_start("2024-01-01T07:00:00Z");

    configure();
    ASSERT_EQ(connectors.size(), 2);
    ASSERT_NE(connectors[1], nullptr);
    ASSERT_EQ(connectors[1]->transaction, nullptr);

    configure_transaction(session_start);
    ASSERT_EQ(connectors.size(), 2);
    ASSERT_NE(connectors[1], nullptr);
    ASSERT_NE(connectors[1]->transaction, nullptr);

    EXPECT_EQ(connectors[1]->transaction->get_start_energy_wh()->timestamp, session_start);

    configure();
    ASSERT_EQ(connectors.size(), 2);
    ASSERT_NE(connectors[1], nullptr);
    ASSERT_EQ(connectors[1]->transaction, nullptr);
}

// ----------------------------------------------------------------------------
// Test cases - Daily Recurring single profile

TEST_F(ProfileTestsC, DailyRecurringNotValidYet) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-01T07:00:00Z");
    DateTime start_time("2024-01-01T07:55:00Z");
    DateTime end_time("2024-01-01T08:05:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurring, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, default_stack_level);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit, default_limit_A);
}

TEST_F(ProfileTestsC, DailyRecurringAlmostValid) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-01T07:00:00Z");
    DateTime start_time("2024-01-01T11:55:00Z");
    DateTime end_time("2024-01-01T12:05:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurring, connector_id);

#ifdef DEBUG_PROFILE
    auto periods = ocpp::v16::calculate_profile(start_time, session_start, profileRecurring, end_time);
    std::cout << "periods: " << periods << std::endl;
    auto composite = ocpp::v16::calculate_composite_schedule(periods, start_time, end_time);
    std::cout << "composite: " << composite << std::endl;
    std::vector<period_entry_t> none;
    auto empty = ocpp::v16::calculate_composite_schedule(none, start_time, end_time);
    auto combined = ocpp::v16::calculate_composite_schedule(empty, composite, empty);
    std::cout << "combined: " << combined << std::endl;
#endif

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, default_stack_level);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit, default_limit_A);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileRecurring.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profileRecurring.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsC, DailyRecurringDuring) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-01T07:00:00Z");
    DateTime start_time("2024-01-01T15:00:00Z");
    DateTime end_time("2024-01-01T15:10:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurring, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileRecurring.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileRecurring.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsC, DailyRecurringApproachingEnd) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-01T07:00:00Z");
    DateTime start_time("2024-01-01T17:55:00Z");
    DateTime end_time("2024-01-01T18:05:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurring, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileRecurring.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileRecurring.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileRecurring.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profileRecurring.chargingSchedule.chargingSchedulePeriod[1].limit);
}

TEST_F(ProfileTestsC, DailyRecurringNextDay) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-01T07:00:00Z");
    DateTime start_time("2024-01-01T07:30:00Z");
    DateTime end_time("2024-01-01T07:40:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurring, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, default_stack_level);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit, default_limit_A);
}

TEST_F(ProfileTestsC, DailyRecurringNextDayStart) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-02T07:00:00Z");
    DateTime start_time("2024-01-02T07:55:00Z");
    DateTime end_time("2024-01-02T08:05:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurring, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileRecurring.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileRecurring.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileRecurring.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profileRecurring.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsC, DailyRecurringApproachingInvalid) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-05T07:00:00Z");
    DateTime start_time("2024-01-05T11:55:00Z");
    DateTime end_time("2024-01-05T12:05:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurring, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileRecurring.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileRecurring.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, default_stack_level);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit, default_limit_A);
}

// ----------------------------------------------------------------------------
// Test cases - profileMinimum

TEST_F(ProfileTestsC, MinimumSession) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-05T07:00:00Z");
    DateTime start_time("2024-01-05T11:55:00Z");
    DateTime end_time("2024-01-05T12:05:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsC, MinimumNoSession) {
    const std::int32_t connector_id{1};
    DateTime start_time("2024-01-05T11:55:00Z");
    DateTime end_time("2024-01-05T12:05:00Z");
    configure();
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);
}

// ----------------------------------------------------------------------------
// Test cases - Daily Recurring Alternate profile

TEST_F(ProfileTestsC, DailyRecurringAltNotValidYet) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-01T07:00:00Z");
    DateTime start_time("2024-01-01T07:55:00Z");
    DateTime end_time("2024-01-01T08:05:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);

    // now without a session
    configure();
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                      ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsC, DailyRecurringAltAlmostValid) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-01T07:00:00Z");
    DateTime start_time("2024-01-01T11:55:00Z");
    DateTime end_time("2024-01-01T12:05:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileRecurringAlt.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profileRecurringAlt.chargingSchedule.chargingSchedulePeriod[0].limit);

    // now without a session
    configure();
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                      ChargingRateUnit::A, false, true);
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileRecurringAlt.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profileRecurringAlt.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsC, DailyRecurringAltDuring) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-01T07:00:00Z");
    DateTime start_time("2024-01-01T15:00:00Z");
    DateTime end_time("2024-01-01T15:10:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileRecurringAlt.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileRecurringAlt.chargingSchedule.chargingSchedulePeriod[0].limit);

    // now without a session
    configure();
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                      ChargingRateUnit::A, false, true);
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileRecurringAlt.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileRecurringAlt.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsC, DailyRecurringAltApproachingEnd) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-01T07:00:00Z");
    DateTime start_time("2024-01-01T17:55:00Z");
    DateTime end_time("2024-01-01T18:05:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileRecurringAlt.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileRecurringAlt.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);

    // now without a session
    configure();
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                      ChargingRateUnit::A, false, true);
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileRecurringAlt.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileRecurringAlt.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsC, DailyRecurringAltNextDay) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-01T07:00:00Z");
    DateTime start_time("2024-01-01T07:30:00Z");
    DateTime end_time("2024-01-01T07:40:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);

    // now without a session
    configure();
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                      ChargingRateUnit::A, false, true);
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsC, DailyRecurringAltNextDayStart) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-02T07:00:00Z");
    DateTime start_time("2024-01-02T07:55:00Z");
    DateTime end_time("2024-01-02T08:05:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileRecurringAlt.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profileRecurringAlt.chargingSchedule.chargingSchedulePeriod[0].limit);

    // now without a session
    configure();
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                      ChargingRateUnit::A, false, true);
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileRecurringAlt.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profileRecurringAlt.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsC, DailyRecurringAltApproachingInvalid) {
    const std::int32_t connector_id{1};
    DateTime session_start("2024-01-05T07:00:00Z");
    DateTime start_time("2024-01-05T11:55:00Z");
    DateTime end_time("2024-01-05T12:05:00Z");
    configure_transaction(session_start);
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileRecurringAlt.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileRecurringAlt.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);

    // now without a session
    configure();
    handler.add_tx_default_profile(profileRecurringAlt, connector_id);
    handler.add_tx_default_profile(profileMinimum, connector_id);

    valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                      ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileRecurringAlt.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileRecurringAlt.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileMinimum.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profileMinimum.chargingSchedule.chargingSchedulePeriod[0].limit);
}

// ----------------------------------------------------------------------------
// Test cases - issue 609 https://github.com/EVerest/libocpp/issues/609

const DateTime profile609A_startSchedule("2024-01-17T18:00:00.000Z");
const ChargingProfile profile609A{
    1,                                            // chargingProfileId
    1,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Absolute,            // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::W, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,      // startPeriod
                2000.0, // limit
                1,      // optional - int32_t - numberPhases
            },
        },
        1080,                      // optional - int32_t duration
        profile609A_startSchedule, // optional - ocpp::DateTime - startSchedule
        0.0,                       // optional - float - minChargingRate
    },                             // chargingSchedule
    std::nullopt,                  // transactionId
    RecurrencyKindType::Daily,     // recurrencyKind
    std::nullopt,                  // validFrom
    std::nullopt,                  // validTo
};

const DateTime profile609B_startSchedule("2023-01-17T17:00:00.000Z");
const ChargingProfile profile609B{
    100,                                          // chargingProfileId
    0,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Recurring,           // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::W, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,       // startPeriod
                11000.0, // limit
                3,       // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                28800,  // startPeriod
                6000.0, // limit
                3,      // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                72000,   // startPeriod
                12000.0, // limit
                3,       // optional - int32_t - numberPhases
            },
        },
        86400,                     // optional - int32_t duration
        profile609B_startSchedule, // optional - ocpp::DateTime - startSchedule
        0.0,                       // optional - float - minChargingRate
    },                             // chargingSchedule
    std::nullopt,                  // transactionId
    RecurrencyKindType::Daily,     // recurrencyKind
    std::nullopt,                  // validFrom
    std::nullopt,                  // validTo
};

TEST_F(ProfileTestsC, Issue609During) {
    const std::int32_t connector_id{1};
    const std::int32_t duration = 21540;
    DateTime start_time(profile609A_startSchedule.to_time_point() + minutes(1));
    DateTime end_time(start_time.to_time_point() + seconds(duration));
    configure();
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profile609A, 0);
    handler.add_tx_default_profile(profile609B, 0);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::W, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::W);
    EXPECT_EQ(enhanced_schedule.duration, duration);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profile609A.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profile609A.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 1020);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profile609B.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profile609B.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsC, Issue609Before) {
    const std::int32_t connector_id{1};
    const std::int32_t duration = 21601;
    DateTime start_time(profile609A_startSchedule.to_time_point() - seconds(1));
    DateTime end_time(start_time.to_time_point() + seconds(duration));
    configure();
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profile609A, 0);
    handler.add_tx_default_profile(profile609B, 0);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::W, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::W);
    EXPECT_EQ(enhanced_schedule.duration, duration);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 3);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profile609B.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profile609B.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profile609A.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profile609A.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].startPeriod, 1081);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].stackLevel, profile609B.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].limit,
              profile609B.chargingSchedule.chargingSchedulePeriod[0].limit);
}

// ----------------------------------------------------------------------------
// Test cases - charging units

const DateTime profileWatts_startSchedule("2024-01-01T12:00:00.000Z");
const ChargingProfile profileWatts{
    1,                                            // chargingProfileId
    1,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Absolute,            // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::W, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,      // startPeriod
                2000.0, // limit
                1,      // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                300,    // startPeriod
                1800.0, // limit
                2,      // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                600,    // startPeriod
                1600.0, // limit
                3,      // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                900,          // startPeriod
                1200.0,       // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        3500,                       // optional - int32_t duration
        profileWatts_startSchedule, // optional - ocpp::DateTime - startSchedule
        0.0,                        // optional - float - minChargingRate
    },                              // chargingSchedule
    std::nullopt,                   // transactionId
    std::nullopt,                   // recurrencyKind
    std::nullopt,                   // validFrom
    std::nullopt,                   // validTo
};

const DateTime profileAmps_startSchedule("2024-01-01T13:00:00.000Z");
const ChargingProfile profileAmps{
    1,                                            // chargingProfileId
    2,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Absolute,            // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,    // startPeriod
                32.0, // limit
                3,    // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                300,  // startPeriod
                24.0, // limit
                2,    // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                600,  // startPeriod
                12.0, // limit
                1,    // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                900,          // startPeriod
                10.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        3600,                      // optional - int32_t duration
        profileAmps_startSchedule, // optional - ocpp::DateTime - startSchedule
        0.0,                       // optional - float - minChargingRate
    },                             // chargingSchedule
    std::nullopt,                  // transactionId
    std::nullopt,                  // recurrencyKind
    std::nullopt,                  // validFrom
    std::nullopt,                  // validTo
};

TEST_F(ProfileTestsC, ChargeRateW) {
    const std::int32_t connector_id{1};
    const std::int32_t duration = 3600 * 2;
    DateTime start_time = profileWatts_startSchedule;
    DateTime end_time(start_time.to_time_point() + seconds(duration));
    configure();
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileWatts, 0);
    handler.add_tx_default_profile(profileAmps, 0);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::W, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::W);
    EXPECT_EQ(enhanced_schedule.duration, duration);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 9);
    // profileWatts
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileWatts.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileWatts.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileWatts.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              profileWatts.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].startPeriod, 600);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].stackLevel, profileWatts.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].limit,
              profileWatts.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].startPeriod, 900);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].stackLevel, profileWatts.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].limit,
              profileWatts.chargingSchedule.chargingSchedulePeriod[3].limit);
    // undefined period (defaults)
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].startPeriod, 3500);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].stackLevel, default_stack_level);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].limit, default_limit_W);
    // profileAmps - converted to Watts
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].startPeriod, 3600);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].stackLevel, profileAmps.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].limit,
              to_watts(profileAmps.chargingSchedule.chargingSchedulePeriod[0]));
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].startPeriod, 3900);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].stackLevel, profileAmps.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].limit,
              to_watts(profileAmps.chargingSchedule.chargingSchedulePeriod[1]));
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].startPeriod, 4200);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].stackLevel, profileAmps.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].limit,
              to_watts(profileAmps.chargingSchedule.chargingSchedulePeriod[2]));
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[8].startPeriod, 4500);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[8].stackLevel, profileAmps.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[8].limit,
              to_watts(profileAmps.chargingSchedule.chargingSchedulePeriod[3]));
}

TEST_F(ProfileTestsC, ChargeRateA) {
    const std::int32_t connector_id{1};
    const std::int32_t duration = 3600 * 2;
    DateTime start_time = profileWatts_startSchedule;
    DateTime end_time(start_time.to_time_point() + seconds(duration));
    configure();
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileWatts, 0);
    handler.add_tx_default_profile(profileAmps, 0);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, duration);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 9);
    // profileWatts
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileWatts.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              to_amps(profileWatts.chargingSchedule.chargingSchedulePeriod[0]));
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileWatts.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              to_amps(profileWatts.chargingSchedule.chargingSchedulePeriod[1]));
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].startPeriod, 600);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].stackLevel, profileWatts.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].limit,
              to_amps(profileWatts.chargingSchedule.chargingSchedulePeriod[2]));
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].startPeriod, 900);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].stackLevel, profileWatts.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].limit,
              to_amps(profileWatts.chargingSchedule.chargingSchedulePeriod[3]));
    // undefined period (defaults)
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].startPeriod, 3500);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].stackLevel, default_stack_level);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].limit, default_limit_A);
    // profileAmps - converted to Watts
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].startPeriod, 3600);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].stackLevel, profileAmps.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].limit,
              profileAmps.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].startPeriod, 3900);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].stackLevel, profileAmps.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].limit,
              profileAmps.chargingSchedule.chargingSchedulePeriod[1].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].startPeriod, 4200);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].stackLevel, profileAmps.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].limit,
              profileAmps.chargingSchedule.chargingSchedulePeriod[2].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[8].startPeriod, 4500);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[8].stackLevel, profileAmps.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[8].limit,
              profileAmps.chargingSchedule.chargingSchedulePeriod[3].limit);

    // test default (Amps)
    auto enhanced_schedule_default = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                                   ChargingRateUnit::A, false, true);
    EXPECT_EQ(enhanced_schedule, enhanced_schedule_default);
}

const DateTime profileCombined_startSchedule("2024-01-01T13:00:00.000Z");
const ChargingProfile profileCombinedCPM{
    1,                                                 // chargingProfileId
    2,                                                 // stackLevel
    ChargingProfilePurposeType::ChargePointMaxProfile, // chargingProfilePurpose
    ChargingProfileKindType::Absolute,                 // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,            // startPeriod
                10.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                80,   // startPeriod
                20.0, // limit
                1,    // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                100,  // startPeriod
                20.0, // limit
                3,    // optional - int32_t - numberPhases
            },
        },
        200,                           // optional - int32_t duration
        profileCombined_startSchedule, // optional - ocpp::DateTime - startSchedule
        std::nullopt,                  // optional - float - minChargingRate
    },                                 // chargingSchedule
    std::nullopt,                      // transactionId
    std::nullopt,                      // recurrencyKind
    std::nullopt,                      // validFrom
    std::nullopt,                      // validTo
};

const ChargingProfile profileCombinedTXD{
    11,                                           // chargingProfileId
    12,                                           // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Absolute,            // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::W, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,       // startPeriod
                11000.0, // limit
                3,       // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                60,     // startPeriod
                6900.0, // limit
                1,      // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                120,          // startPeriod
                5520.0,       // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                180,          // startPeriod
                17250.0,      // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                260,          // startPeriod
                5520.0,       // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        300,                           // optional - int32_t duration
        profileCombined_startSchedule, // optional - ocpp::DateTime - startSchedule
        std::nullopt,                  // optional - float - minChargingRate
    },                                 // chargingSchedule
    std::nullopt,                      // transactionId
    std::nullopt,                      // recurrencyKind
    std::nullopt,                      // validFrom
    std::nullopt,                      // validTo
};

TEST_F(ProfileTestsC, Combined1) {
    const std::int32_t connector_id{1};
    const std::int32_t duration = 400;
    DateTime start_time = profileCombined_startSchedule;
    DateTime end_time(start_time.to_time_point() + seconds(duration));
    configure();
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_charge_point_max_profile(profileCombinedCPM);
    handler.add_tx_default_profile(profileCombinedTXD, 0);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::W, false, true);
    std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::W);
    EXPECT_EQ(enhanced_schedule.duration, duration);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 8);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileCombinedCPM.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit, 6900);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].periodTransformed, true);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].numberPhases.value_or(-1), 3);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 60);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileCombinedCPM.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit, 2300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].periodTransformed, true);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].numberPhases.value_or(-1), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].startPeriod, 80);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].stackLevel, profileCombinedCPM.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].limit, 4600);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].periodTransformed, true);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].numberPhases.value_or(-1), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].startPeriod, 120);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].stackLevel, profileCombinedTXD.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].limit, 5520);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].periodTransformed, false);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].numberPhases.value_or(-1), 3);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].startPeriod, 180);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].stackLevel, profileCombinedCPM.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].limit, 13800);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].periodTransformed, true);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].numberPhases.value_or(-1), 3);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].startPeriod, 200);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].stackLevel, profileCombinedTXD.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].limit, 17250);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].periodTransformed, false);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].startPeriod, 260);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].stackLevel, profileCombinedTXD.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].limit, 5520);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].periodTransformed, false);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].stackLevel, default_stack_level);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].limit, default_limit_W);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].periodTransformed, false);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].numberPhases.value_or(-1), -1);

    // test default (Amps)
    enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                      ChargingRateUnit::A, false, true);
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, duration);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 8);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileCombinedCPM.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit, 10);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].numberPhases.value_or(-1), 3);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 60);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileCombinedCPM.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit, 10);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].numberPhases.value_or(-1), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].startPeriod, 80);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].stackLevel, profileCombinedCPM.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].limit, 20);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].numberPhases.value_or(-1), 1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].startPeriod, 120);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].stackLevel, profileCombinedTXD.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].limit, 8);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].numberPhases.value_or(-1), 3);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].startPeriod, 180);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].stackLevel, profileCombinedCPM.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].limit, 20);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].numberPhases.value_or(-1), 3);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].startPeriod, 200);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].stackLevel, profileCombinedTXD.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].limit, 25);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].startPeriod, 260);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].stackLevel, profileCombinedTXD.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].limit, 8);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].stackLevel, default_stack_level);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].limit, default_limit_A);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].numberPhases.value_or(-1), -1);
}

const DateTime profileRecurringA_startSchedule("2024-06-03T12:31:53Z");
const ChargingProfile profileRecurringA{
    1,                                            // chargingProfileId
    0,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Recurring,           // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,            // startPeriod
                14.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                5000,         // startPeriod
                16.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                15000,        // startPeriod
                20.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        std::nullopt,                    // optional - int32_t duration
        profileRecurringA_startSchedule, // optional - ocpp::DateTime - startSchedule
        std::nullopt,                    // optional - float - minChargingRate
    },                                   // chargingSchedule
    std::nullopt,                        // transactionId
    RecurrencyKindType::Daily,           // recurrencyKind
    std::nullopt,                        // validFrom
    std::nullopt,                        // validTo
};

const DateTime profileRecurringB_startSchedule("2024-06-03T12:31:53Z");
const ChargingProfile profileRecurringB{
    2,                                            // chargingProfileId
    1,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Recurring,           // chargingProfileKind
    {
        // EnhancedChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // EnhancedChargingSchedulePeriod
                0,            // startPeriod
                10.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                10000,        // startPeriod
                22.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // EnhancedChargingSchedulePeriod
                20000,        // startPeriod
                6.0,          // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        86400,                           // optional - int32_t duration
        profileRecurringB_startSchedule, // optional - ocpp::DateTime - startSchedule
        std::nullopt,                    // optional - float - minChargingRate
    },                                   // chargingSchedule
    std::nullopt,                        // transactionId
    RecurrencyKindType::Daily,           // recurrencyKind
    std::nullopt,                        // validFrom
    std::nullopt,                        // validTo
};

TEST_F(ProfileTestsC, Recurring2a) {
    // test recurring over 2 days
    const std::int32_t connector_id{1};
    const std::int32_t duration = 172800;
    DateTime start_time = profileRecurringA_startSchedule;
    DateTime end_time(start_time.to_time_point() + seconds(duration));
    configure();
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurringA, 0);
    handler.add_tx_default_profile(profileRecurringB, 0);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, duration);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 6);

    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit, 10);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 10000);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit, 22);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].startPeriod, 20000);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].limit, 6);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].startPeriod, 86400);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].limit, 10);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].startPeriod, 96400);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].limit, 22);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].startPeriod, 106400);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].limit, 6);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].numberPhases.value_or(-1), -1);
}

TEST_F(ProfileTestsC, Recurring2b) {
    // test recurring over 4 days
    const std::int32_t connector_id{1};
    const std::int32_t duration = 345600;
    DateTime start_time = profileRecurringA_startSchedule;
    DateTime end_time(start_time.to_time_point() + seconds(duration));
    configure();
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    handler.add_tx_default_profile(profileRecurringA, 0);
    handler.add_tx_default_profile(profileRecurringB, 0);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, connector_id);
    // std::cout << "valid_profiles: " << valid_profiles << std::endl;
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(start_time, end_time, connector_id,
                                                                           ChargingRateUnit::A, false, true);
    // std::cout << "enhanced_schedule: " << enhanced_schedule << std::endl;
    EXPECT_EQ(enhanced_schedule.chargingRateUnit, ChargingRateUnit::A);
    EXPECT_EQ(enhanced_schedule.duration, duration);
    EXPECT_EQ(enhanced_schedule.startSchedule, start_time);
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 12);

    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit, 10);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 10000);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit, 22);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].startPeriod, 20000);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].limit, 6);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[2].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].startPeriod, 86400);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].limit, 10);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[3].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].startPeriod, 96400);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].limit, 22);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[4].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].startPeriod, 106400);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].limit, 6);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[5].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].startPeriod, 172800);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].limit, 10);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[6].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].startPeriod, 182800);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].limit, 22);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[7].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[8].startPeriod, 192800);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[8].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[8].limit, 6);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[8].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[9].startPeriod, 259200);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[9].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[9].limit, 10);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[9].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[10].startPeriod, 269200);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[10].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[10].limit, 22);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[10].numberPhases.value_or(-1), -1);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[11].startPeriod, 279200);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[11].stackLevel, profileRecurringB.stackLevel);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[11].limit, 6);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[11].numberPhases.value_or(-1), -1);
}

} // namespace
