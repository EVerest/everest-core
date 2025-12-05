// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

// execute: ./libocpp_unit_tests --gtest_filter=ProfileTests.*

#include "database_stub.hpp"
#include "ocpp/v16/ocpp_types.hpp"
#include "ocpp/v16/types.hpp"
#include "profile_tests_common.hpp"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>
#include <ocpp/v16/connector.hpp>
#include <ocpp/v16/database_handler.hpp>
#include <ocpp/v16/smart_charging.hpp>
#include <optional>
#include <ostream>
#include <string>

using namespace ocpp::v16;
using namespace ocpp;
namespace fs = std::filesystem;
using json = nlohmann::json;

// ----------------------------------------------------------------------------
// Test anonymous namespace
namespace {
using namespace std::chrono;

// ----------------------------------------------------------------------------
// Test charging profiles

const auto now = date::utc_clock::now();
const ocpp::DateTime profileA_start_time(now - seconds(600));
const ocpp::DateTime profileA_end_time(now + hours(2));
const ChargingProfile profileA{
    301,                                          // chargingProfileId
    5,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Absolute,            // chargingProfileKind
    {
        // ChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // ChargingSchedulePeriod
                0,            // startPeriod
                32.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // ChargingSchedulePeriod
                6000,         // startPeriod
                31.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // ChargingSchedulePeriod
                12000,        // startPeriod
                30.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        std::nullopt,        // optional - int32_t duration
        profileA_start_time, // optional - ocpp::DateTime - startSchedule
        std::nullopt,        // optional - float - minChargingRate
    },                       // chargingSchedule
    std::nullopt,            // transactionId
    std::nullopt,            // recurrencyKind
    profileA_start_time,     // validFrom
    profileA_end_time,       // validTo
};

ocpp::DateTime profileB_start_time(now);
ocpp::DateTime profileB_end_time(now + hours(4));
ChargingProfile profileB{
    302,                                          // chargingProfileId
    5,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Absolute,            // chargingProfileKind
    {
        // ChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // ChargingSchedulePeriod
                0,            // startPeriod
                10.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
            {
                // ChargingSchedulePeriod
                7000,         // startPeriod
                11.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        std::nullopt,        // optional - int32_t duration
        profileB_start_time, // optional - ocpp::DateTime - startSchedule
        std::nullopt,        // optional - float - minChargingRate
    },                       // chargingSchedule
    std::nullopt,            // transactionId
    std::nullopt,            // recurrencyKind
    profileB_start_time,     // validFrom
    profileB_end_time,       // validTo
};

ocpp::DateTime profileNoCharge_start_time(now - seconds(300));
ocpp::DateTime profileNoCharge_end_time(now + hours(300));
ChargingProfile profileNoCharge{
    302,                                          // chargingProfileId
    5,                                            // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Relative,            // chargingProfileKind
    {
        // ChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // ChargingSchedulePeriod
                0,            // startPeriod
                0.0,          // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        std::nullopt,           // optional - int32_t duration
        std::nullopt,           // optional - ocpp::DateTime - startSchedule
        std::nullopt,           // optional - float - minChargingRate
    },                          // chargingSchedule
    std::nullopt,               // transactionId
    std::nullopt,               // recurrencyKind
    profileNoCharge_start_time, // validFrom
    profileNoCharge_end_time,   // validTo
};

ocpp::DateTime profileStack_start_time(now - minutes(5));
ocpp::DateTime profileStack_end_time(now + hours(9));
ChargingProfile profileStackA{
    303,                                          // chargingProfileId
    10,                                           // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Absolute,            // chargingProfileKind
    {
        // ChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // ChargingSchedulePeriod
                0,            // startPeriod
                24.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        std::nullopt,            // optional - int32_t duration
        profileStack_start_time, // optional - ocpp::DateTime - startSchedule
        std::nullopt,            // optional - float - minChargingRate
    },                           // chargingSchedule
    std::nullopt,                // transactionId
    std::nullopt,                // recurrencyKind
    profileStack_start_time,     // validFrom
    profileStack_end_time,       // validTo
};

ChargingProfile profileStackB{
    304,                                          // chargingProfileId
    20,                                           // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Absolute,            // chargingProfileKind
    {
        // ChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // ChargingSchedulePeriod
                0,            // startPeriod
                26.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        std::nullopt,            // optional - int32_t duration
        profileStack_start_time, // optional - ocpp::DateTime - startSchedule
        std::nullopt,            // optional - float - minChargingRate
    },                           // chargingSchedule
    std::nullopt,                // transactionId
    std::nullopt,                // recurrencyKind
    profileStack_start_time,     // validFrom
    profileStack_end_time,       // validTo
};

ocpp::DateTime profileStackC_start_time(now + minutes(30));
ocpp::DateTime profileStackC_end_time(now + hours(9));
ChargingProfile profileStackC{
    305,                                          // chargingProfileId
    50,                                           // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Absolute,            // chargingProfileKind
    {
        // ChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // ChargingSchedulePeriod
                0,            // startPeriod
                28.0,         // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        std::nullopt,             // optional - int32_t duration
        profileStackC_start_time, // optional - ocpp::DateTime - startSchedule
        std::nullopt,             // optional - float - minChargingRate
    },                            // chargingSchedule
    std::nullopt,                 // transactionId
    std::nullopt,                 // recurrencyKind
    profileStackC_start_time,     // validFrom
    profileStackC_end_time,       // validTo
};

ocpp::DateTime profileTime_start_time(now + minutes(5));
ocpp::DateTime profileTime_end_time(now + hours(9));
ChargingProfile profileTime{
    401,                                          // chargingProfileId
    90,                                           // stackLevel
    ChargingProfilePurposeType::TxDefaultProfile, // chargingProfilePurpose
    ChargingProfileKindType::Absolute,            // chargingProfileKind
    {
        // ChargingSchedule
        ChargingRateUnit::A, // chargingRateUnit
        {
            {
                // ChargingSchedulePeriod
                0,            // startPeriod
                8.0,          // limit
                std::nullopt, // optional - int32_t - numberPhases
            },
        },
        std::nullopt,           // optional - int32_t duration
        profileTime_start_time, // optional - ocpp::DateTime - startSchedule
        std::nullopt,           // optional - float - minChargingRate
    },                          // chargingSchedule
    std::nullopt,               // transactionId
    std::nullopt,               // recurrencyKind
    profileTime_start_time,     // validFrom
    profileTime_end_time,       // validTo
};

// ----------------------------------------------------------------------------
// Test class
class ProfileTestsB : public stubs::DbTestBase {};

// ----------------------------------------------------------------------------
// Test cases

TEST(DateTime, init) {
    const ocpp::DateTime base(now);
    const ocpp::DateTime construct(base);
    const ocpp::DateTime construct_time(base.to_time_point());

    EXPECT_EQ(base, construct);
    EXPECT_EQ(base, construct_time);
    EXPECT_TRUE(nearly_equal(base, construct));
    EXPECT_TRUE(nearly_equal(base, construct_time));

    const ocpp::DateTime floor_construct_time(floor<seconds>(base.to_time_point()));
    EXPECT_TRUE(nearly_equal(base, floor_construct_time));

    std::optional<ocpp::DateTime> opt_dt;
    opt_dt.emplace(ocpp::DateTime(floor<seconds>(base.to_time_point())));
    EXPECT_EQ(floor_construct_time, opt_dt.value());
    EXPECT_TRUE(nearly_equal(base, opt_dt.value()));
}

TEST_F(ProfileTestsB, init) {
    add_connectors(2);
    // map doesn't include connector 0, database does
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    ChargingProfile profile{
        101,                                                                 // chargingProfileId
        20,                                                                  // stackLevel
        ChargingProfilePurposeType::TxDefaultProfile,                        // chargingProfilePurpose
        ChargingProfileKindType::Relative,                                   // chargingProfileKind
        {ChargingRateUnit::A, {}, std::nullopt, std::nullopt, std::nullopt}, // chargingSchedule
        std::nullopt,                                                        // transactionId
        std::nullopt,                                                        // recurrencyKind
        std::nullopt,                                                        // validFrom
        std::nullopt,                                                        // validTo
    };
    handler.add_tx_default_profile(profile, 1);
    // the following has a valgrind/memcheck reported leak via EVLOG_info and into the
    // boost libraries
    handler.clear_all_profiles();
}

TEST_F(ProfileTestsB, validate_profileA) {
    // need to have a transaction for calculate_composite_schedule()
    // and calculate_enhanced_composite_schedule()
    int connector_id = 1;
    std::int32_t meter_start = 0;
    ocpp::DateTime timestamp(now);
    add_connectors(5);
    connectors[connector_id]->transaction =
        std::make_shared<Transaction>(-1, connector_id, "1234", "4567", meter_start, std::nullopt, timestamp, nullptr);
    // map doesn't include connector 0, database does
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    auto tmp_profile = profileA;
    EXPECT_TRUE(
        handler.validate_profile(tmp_profile, 0, true, 100, 10, 10, {ChargingRateUnit::A, ChargingRateUnit::W}));
    // check profile not updated
    EXPECT_EQ(tmp_profile, profileA);
    handler.add_tx_default_profile(tmp_profile, connector_id);
    auto valid_profiles = handler.get_valid_profiles(profileA_start_time, profileA_end_time, connector_id);
    auto schedule = handler.calculate_composite_schedule(profileA_start_time, profileA_end_time, connector_id,
                                                         ChargingRateUnit::A, false, true);
    // std::cout << "chargingSchedule:\n" << profileA << std::endl;
    // std::cout << "schedule:\n" << schedule << std::endl;
    EXPECT_EQ(profileA.chargingSchedule, schedule);
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(
        profileA_start_time, profileA_end_time, connector_id, ChargingRateUnit::A, false, true);
    // std::cout << "enhanced schedule:\n" << enhanced_schedule << std::endl;
    EXPECT_EQ(profileA.chargingSchedule, enhanced_schedule);
}

TEST_F(ProfileTestsB, validate_profileB) {
    // need to have a transaction for calculate_composite_schedule()
    // and calculate_enhanced_composite_schedule()
    int connector_id = 1;
    std::int32_t meter_start = 0;
    ocpp::DateTime timestamp(now + seconds(5));
    add_connectors(5);
    connectors[connector_id]->transaction =
        std::make_shared<Transaction>(-1, connector_id, "1234", "4567", meter_start, std::nullopt, timestamp, nullptr);
    // map doesn't include connector 0, database does
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    auto tmp_profile = profileB;
    EXPECT_TRUE(
        handler.validate_profile(tmp_profile, 0, true, 100, 10, 10, {ChargingRateUnit::A, ChargingRateUnit::W}));
    // check profile not updated
    EXPECT_EQ(tmp_profile, profileB);
    handler.add_tx_default_profile(tmp_profile, connector_id);
    auto valid_profiles = handler.get_valid_profiles(profileB_start_time, profileB_end_time, connector_id);
    auto schedule = handler.calculate_composite_schedule(profileB_start_time, profileB_end_time, connector_id,
                                                         ChargingRateUnit::A, false, true);
    EXPECT_EQ(profileB.chargingSchedule, schedule);
    auto enhanced_schedule = handler.calculate_enhanced_composite_schedule(
        profileB_start_time, profileB_end_time, connector_id, ChargingRateUnit::A, false, true);
    EXPECT_EQ(profileB.chargingSchedule, enhanced_schedule);
}

TEST_F(ProfileTestsB, tx_default_0) {
    add_connectors(5);
    // map doesn't include connector 0, database does
    SmartChargingHandler handler(connectors, database_handler, *configuration);
    ChargingProfile profile{
        201,                                                                 // chargingProfileId
        22,                                                                  // stackLevel
        ChargingProfilePurposeType::TxDefaultProfile,                        // chargingProfilePurpose
        ChargingProfileKindType::Relative,                                   // chargingProfileKind
        {ChargingRateUnit::A, {}, std::nullopt, std::nullopt, std::nullopt}, // chargingSchedule
        std::nullopt,                                                        // transactionId
        std::nullopt,                                                        // recurrencyKind
        std::nullopt,                                                        // validFrom
        std::nullopt,                                                        // validTo
    };
    handler.add_tx_default_profile(profile, 0);
    handler.clear_all_profiles();
}

TEST_F(ProfileTestsB, single_profile) {
    std::int32_t connector = 1;
    std::int32_t meter_start = 0;
    ocpp::DateTime timestamp(now + seconds(5));
    add_connectors(1);

    connectors[1]->transaction =
        std::make_shared<Transaction>(-1, connector, "1234", "4567", meter_start, std::nullopt, timestamp, nullptr);
    // map doesn't include connector 0, database does
    SmartChargingHandler handler(connectors, database_handler, *configuration);

    handler.add_tx_default_profile(profileA, 1);

    auto valid_profiles = handler.get_valid_profiles(profileA_start_time, profileA_end_time, 1);
    // std::cout << valid_profiles << std::endl;
    ASSERT_EQ(valid_profiles.size(), 1);
    EXPECT_EQ(profileA.chargingSchedule, valid_profiles[0].chargingSchedule);

    auto schedule = handler.calculate_composite_schedule(profileA_start_time, profileA_end_time, 1, ChargingRateUnit::A,
                                                         false, true);
    // std::cout << schedule << std::endl;
    EXPECT_EQ(profileA.chargingSchedule, schedule);
}

TEST_F(ProfileTestsB, startup_no_charge) {
    std::int32_t connector = 1;
    std::int32_t meter_start = 0;
    ocpp::DateTime start_time(now);
    ocpp::DateTime end_time(now + hours(1));
    ocpp::DateTime timestamp(now);
    add_connectors(1);

    // no active transaction
    // map doesn't include connector 0, database does
    SmartChargingHandler handler(connectors, database_handler, *configuration);

    handler.add_tx_default_profile(profileNoCharge, 1);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, 1);
    ASSERT_EQ(valid_profiles.size(), 1);
    EXPECT_EQ(profileNoCharge.chargingSchedule, valid_profiles[0].chargingSchedule);

    // std::cout << "profileNoCharge: no transaction" << std::endl;
    auto schedule = handler.calculate_enhanced_composite_schedule(start_time, profileNoCharge_end_time, 1,
                                                                  ChargingRateUnit::A, false, true);
    // std::cout << "chargingSchedule:" << profileNoCharge.chargingSchedule << std::endl;
    // std::cout << "schedule:" << schedule.chargingSchedulePeriod << std::endl;
    EXPECT_EQ(profileNoCharge.chargingSchedule, schedule);

    // now with a transaction
    connectors[1]->transaction =
        std::make_shared<Transaction>(-1, connector, "1234", "4567", meter_start, std::nullopt, timestamp, nullptr);
    valid_profiles = handler.get_valid_profiles(start_time, profileNoCharge_end_time, 1);
    ASSERT_EQ(valid_profiles.size(), 1);
    EXPECT_EQ(profileNoCharge.chargingSchedule, valid_profiles[0].chargingSchedule);

    // std::cout << "profileNoCharge: with transaction" << std::endl;
    schedule = handler.calculate_enhanced_composite_schedule(start_time, profileNoCharge_end_time, 1,
                                                             ChargingRateUnit::A, false, true);
    // std::cout << "chargingSchedule:" << profileNoCharge.chargingSchedule << std::endl;
    // std::cout << "schedule:" << schedule.chargingSchedulePeriod << std::endl;
    EXPECT_EQ(profileNoCharge.chargingSchedule, schedule);

    // transaction ended
    start_time = ocpp::DateTime(now + minutes(60));
    connectors[1]->transaction = nullptr;
    // std::cout << "profileNoCharge: with transaction finished" << std::endl;
    valid_profiles = handler.get_valid_profiles(start_time, profileNoCharge_end_time, 1);
    ASSERT_EQ(valid_profiles.size(), 1);
    EXPECT_EQ(profileNoCharge.chargingSchedule, valid_profiles[0].chargingSchedule);

    schedule = handler.calculate_enhanced_composite_schedule(start_time, profileNoCharge_end_time, 1,
                                                             ChargingRateUnit::A, false, true);
    // std::cout << "chargingSchedule:" << profileNoCharge.chargingSchedule << std::endl;
    // std::cout << "schedule:" << schedule.chargingSchedulePeriod << std::endl;
    EXPECT_EQ(profileNoCharge.chargingSchedule, schedule);
}

// ----------------------------------------------------------------------------
// get_valid_profiles tests

TEST_F(ProfileTestsB, get_valid_profiles_absolute) {
    std::int32_t connector = 1;
    add_connectors(1);
    ocpp::DateTime start_time(now);
    ocpp::DateTime end_time(now + minutes(10));
    connectors[1]->transaction =
        std::make_shared<Transaction>(-1, connector, "1234", "4567", 100, std::nullopt, start_time, nullptr);

    SmartChargingHandler handler(connectors, database_handler, *configuration);

    handler.add_tx_default_profile(profileStackA, 1);
    handler.add_tx_default_profile(profileStackB, 1);
    handler.add_tx_default_profile(profileStackC, 1);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, 1);
    auto nProfiles = valid_profiles.size();

    // std::cout << "profiles:" << valid_profiles << std::endl;
    ASSERT_EQ(nProfiles, 3);
    EXPECT_EQ(profileStackA, valid_profiles[0]);
    EXPECT_EQ(profileStackB, valid_profiles[1]);
    EXPECT_EQ(profileStackC, valid_profiles[2]);
}

TEST_F(ProfileTestsB, get_valid_profiles_absolute_delay) {
    std::int32_t connector = 1;
    add_connectors(1);
    ocpp::DateTime start_time(now);
    ocpp::DateTime end_time(now + minutes(10));
    connectors[1]->transaction =
        std::make_shared<Transaction>(-1, connector, "1234", "4567", 100, std::nullopt, start_time, nullptr);

    auto absoluteB = profileStackB;
    absoluteB.validFrom = ocpp::DateTime(now + minutes(5));
    absoluteB.chargingSchedule.startSchedule = ocpp::DateTime(now + minutes(5));

    SmartChargingHandler handler(connectors, database_handler, *configuration);

    handler.add_tx_default_profile(profileStackA, 1);
    handler.add_tx_default_profile(absoluteB, 1);
    handler.add_tx_default_profile(profileStackC, 1);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, 1);
    auto nProfiles = valid_profiles.size();

    // std::cout << "profiles:" << valid_profiles << std::endl;
    ASSERT_EQ(nProfiles, 3);
    EXPECT_EQ(profileStackA, valid_profiles[0]);
    EXPECT_EQ(absoluteB, valid_profiles[1]);
    EXPECT_EQ(profileStackC, valid_profiles[2]);
}

TEST_F(ProfileTestsB, get_valid_profiles_relative) {
    std::int32_t connector = 1;
    add_connectors(1);
    ocpp::DateTime start_time(now);
    ocpp::DateTime end_time(now + minutes(10));
    connectors[1]->transaction =
        std::make_shared<Transaction>(-1, connector, "1234", "4567", 100, std::nullopt, start_time, nullptr);

    auto relativeA = profileStackA;
    auto relativeB = profileStackB;
    relativeA.chargingProfileKind = ChargingProfileKindType::Relative;
    relativeA.chargingSchedule.startSchedule = std::nullopt;
    relativeB.chargingProfileKind = ChargingProfileKindType::Relative;
    relativeB.chargingSchedule.startSchedule = std::nullopt;

    SmartChargingHandler handler(connectors, database_handler, *configuration);

    handler.add_tx_default_profile(relativeA, 1);
    handler.add_tx_default_profile(relativeB, 1);
    handler.add_tx_default_profile(profileStackC, 1);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, 1);
    auto nProfiles = valid_profiles.size();

    // std::cout << "profiles:" << valid_profiles << std::endl;
    ASSERT_EQ(nProfiles, 3);
    EXPECT_EQ(relativeA, valid_profiles[0]);
    EXPECT_EQ(relativeB, valid_profiles[1]);
    EXPECT_EQ(profileStackC, valid_profiles[2]);
}

TEST_F(ProfileTestsB, get_valid_profiles_relative_delay) {
    std::int32_t connector = 1;
    add_connectors(1);
    ocpp::DateTime start_time(now);
    ocpp::DateTime end_time(now + minutes(10));
    connectors[1]->transaction =
        std::make_shared<Transaction>(-1, connector, "1234", "4567", 100, std::nullopt, start_time, nullptr);

    auto relativeA = profileStackA;
    auto relativeB = profileStackB;
    relativeA.chargingProfileKind = ChargingProfileKindType::Relative;
    relativeA.chargingSchedule.startSchedule = std::nullopt;
    relativeB.chargingProfileKind = ChargingProfileKindType::Relative;
    relativeB.validFrom = ocpp::DateTime(now + minutes(5));
    relativeB.chargingSchedule.startSchedule = ocpp::DateTime(now + minutes(5));
    // relativeB.chargingSchedule.startSchedule = std::nullopt;

    SmartChargingHandler handler(connectors, database_handler, *configuration);

    handler.add_tx_default_profile(relativeA, 1);
    handler.add_tx_default_profile(relativeB, 1);
    handler.add_tx_default_profile(profileStackC, 1);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, 1);
    auto nProfiles = valid_profiles.size();

    // std::cout << "profiles:" << valid_profiles << std::endl;
    ASSERT_EQ(nProfiles, 3);
    EXPECT_EQ(relativeA, valid_profiles[0]);
    EXPECT_EQ(relativeB, valid_profiles[1]);
    EXPECT_EQ(profileStackC, valid_profiles[2]);
}

// ----------------------------------------------------------------------------
// calculate_enhanced_composite_schedule tests

TEST_F(ProfileTestsB, single_absolute) {
    std::int32_t connector = 1;
    add_connectors(1);
    ocpp::DateTime start_time(now);
    ocpp::DateTime end_time(now + minutes(10));
    connectors[1]->transaction =
        std::make_shared<Transaction>(-1, connector, "1234", "4567", 100, std::nullopt, start_time, nullptr);

    auto absoluteA = profileStackA;

    SmartChargingHandler handler(connectors, database_handler, *configuration);

    handler.add_tx_default_profile(absoluteA, 1);
    handler.add_tx_default_profile(profileStackC, 1);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, 1);
    auto nProfiles = valid_profiles.size();

    // std::cout << "profiles:" << valid_profiles << std::endl;

    auto enhanced_schedule =
        handler.calculate_enhanced_composite_schedule(start_time, end_time, 1, ChargingRateUnit::A, false, true);
    // std::cout << "schedule:                  " << enhanced_schedule << std::endl;
    // std::cout << "absoluteA.chargingSchedule:" << absoluteA.chargingSchedule << std::endl;
    EXPECT_EQ(enhanced_schedule.duration.value_or(-1), 600);
    EXPECT_EQ(enhanced_schedule.startSchedule, floor_seconds(start_time));
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(absoluteA.chargingSchedule.chargingSchedulePeriod[0].limit,
              enhanced_schedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(absoluteA.chargingSchedule.chargingSchedulePeriod[0].startPeriod,
              enhanced_schedule.chargingSchedulePeriod[0].startPeriod);
}

TEST_F(ProfileTestsB, stack_absolute) {
    // GTEST_SKIP() << "ignore for now";
    std::int32_t connector = 1;
    add_connectors(1);
    ocpp::DateTime start_time(now);
    ocpp::DateTime end_time(now + minutes(10));
    connectors[1]->transaction =
        std::make_shared<Transaction>(-1, connector, "1234", "4567", 100, std::nullopt, start_time, nullptr);

    SmartChargingHandler handler(connectors, database_handler, *configuration);

    handler.add_tx_default_profile(profileStackA, 1);
    handler.add_tx_default_profile(profileStackB, 1);
    handler.add_tx_default_profile(profileStackC, 1);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, 1);
    auto nProfiles = valid_profiles.size();

    // std::cout << "profiles:" << valid_profiles << std::endl;
    ASSERT_EQ(nProfiles, 3);
    EXPECT_EQ(profileStackA, valid_profiles[0]);
    EXPECT_EQ(profileStackB, valid_profiles[1]);
    EXPECT_EQ(profileStackC, valid_profiles[2]);

    auto schedule = handler.calculate_composite_schedule(start_time, end_time, 1, ChargingRateUnit::A, false, true);
    ASSERT_EQ(schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(schedule.chargingSchedulePeriod[0].limit, profileStackB.chargingSchedule.chargingSchedulePeriod[0].limit);

    auto enhanced_schedule =
        handler.calculate_enhanced_composite_schedule(start_time, end_time, 1, ChargingRateUnit::A, false, true);
    // std::cout << "chargingSchedule:" << profileNoCharge.chargingSchedule << std::endl;
    // std::cout << "schedule:" << schedule.chargingSchedulePeriod << std::endl;
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 1);
    EXPECT_EQ(enhanced_schedule.startSchedule, floor_seconds(start_time));
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileStackB.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsB, stack_absolute_delay) {
    // GTEST_SKIP() << "ignore for now";
    std::int32_t connector = 1;
    add_connectors(1);
    ocpp::DateTime start_time(now);
    ocpp::DateTime end_time(now + minutes(10));
    connectors[1]->transaction =
        std::make_shared<Transaction>(-1, connector, "1234", "4567", 100, std::nullopt, start_time, nullptr);

    auto absoluteA = profileStackA;
    absoluteA.validTo = ocpp::DateTime(now + minutes(5));

    auto absoluteB = profileStackB;
    absoluteB.validFrom = absoluteA.validTo;
    absoluteB.chargingSchedule.startSchedule = absoluteA.validTo;

    SmartChargingHandler handler(connectors, database_handler, *configuration);

    handler.add_tx_default_profile(absoluteA, 1);
    handler.add_tx_default_profile(absoluteB, 1);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, 1);
    auto nProfiles = valid_profiles.size();

    // std::cout << "Time now: " << start_time << std::endl;
    // std::cout << "profiles:" << valid_profiles << std::endl;
    ASSERT_EQ(nProfiles, 2);
    EXPECT_EQ(absoluteA, valid_profiles[0]);
    EXPECT_EQ(absoluteB, valid_profiles[1]);

    auto schedule = handler.calculate_composite_schedule(start_time, end_time, 1, ChargingRateUnit::A, false, true);
    // std::cout << "schedule:" << schedule << std::endl;
    // expecting two periods
    ASSERT_GE(schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(schedule.startSchedule, floor_seconds(start_time));
    EXPECT_EQ(schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(schedule.chargingSchedulePeriod[0].limit, profileStackA.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(schedule.chargingSchedulePeriod[1].limit, absoluteB.chargingSchedule.chargingSchedulePeriod[0].limit);

    auto enhanced_schedule =
        handler.calculate_enhanced_composite_schedule(start_time, end_time, 1, ChargingRateUnit::A, false, true);
    // std::cout << "schedule:" << enhanced_schedule << std::endl;
    // expecting two periods
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.startSchedule, floor_seconds(start_time));
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileStackA.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              absoluteB.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsB, stack_absolute_delay_overlap) {
    // GTEST_SKIP() << "ignore for now";
    std::int32_t connector = 1;
    add_connectors(1);
    ocpp::DateTime start_time(now);
    ocpp::DateTime end_time(now + minutes(10));
    connectors[1]->transaction =
        std::make_shared<Transaction>(-1, connector, "1234", "4567", 100, std::nullopt, start_time, nullptr);

    auto absoluteB = profileStackB;
    absoluteB.validFrom = ocpp::DateTime(now + minutes(5));
    absoluteB.chargingSchedule.startSchedule = ocpp::DateTime(now + minutes(5));

    SmartChargingHandler handler(connectors, database_handler, *configuration);

    handler.add_tx_default_profile(profileStackA, 1);
    handler.add_tx_default_profile(absoluteB, 1);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, 1);
    auto nProfiles = valid_profiles.size();

    // std::cout << "Time now: " << start_time << std::endl;
    // std::cout << "profiles:" << valid_profiles << std::endl;
    ASSERT_EQ(nProfiles, 2);
    EXPECT_EQ(profileStackA, valid_profiles[0]);
    EXPECT_EQ(absoluteB, valid_profiles[1]);

    auto schedule = handler.calculate_composite_schedule(start_time, end_time, 1, ChargingRateUnit::A, false, true);
    // std::cout << "schedule:" << schedule << std::endl;
    // expecting two periods
    ASSERT_EQ(schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(schedule.startSchedule, floor_seconds(start_time));
    EXPECT_EQ(schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(schedule.chargingSchedulePeriod[0].limit, profileStackA.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(schedule.chargingSchedulePeriod[1].limit, absoluteB.chargingSchedule.chargingSchedulePeriod[0].limit);

    auto enhanced_schedule =
        handler.calculate_enhanced_composite_schedule(start_time, end_time, 1, ChargingRateUnit::A, false, true);
    // std::cout << "schedule:" << enhanced_schedule << std::endl;
    // expecting two periods
    ASSERT_EQ(enhanced_schedule.chargingSchedulePeriod.size(), 2);
    EXPECT_EQ(enhanced_schedule.startSchedule, floor_seconds(start_time));
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].startPeriod, 0);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[0].limit,
              profileStackA.chargingSchedule.chargingSchedulePeriod[0].limit);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].startPeriod, 300);
    EXPECT_EQ(enhanced_schedule.chargingSchedulePeriod[1].limit,
              absoluteB.chargingSchedule.chargingSchedulePeriod[0].limit);
}

TEST_F(ProfileTestsB, stack_relative) {
    std::int32_t connector = 1;
    add_connectors(1);
    ocpp::DateTime start_time(now);
    ocpp::DateTime end_time(now + minutes(10));
    connectors[1]->transaction =
        std::make_shared<Transaction>(-1, connector, "1234", "4567", 100, std::nullopt, start_time, nullptr);

    auto relativeA = profileStackA;
    auto relativeB = profileStackB;
    relativeA.chargingProfileKind = ChargingProfileKindType::Relative;
    relativeA.chargingSchedule.startSchedule = std::nullopt;
    relativeB.chargingProfileKind = ChargingProfileKindType::Relative;
    relativeB.chargingSchedule.startSchedule = std::nullopt;

    SmartChargingHandler handler(connectors, database_handler, *configuration);

    handler.add_tx_default_profile(relativeA, 1);
    handler.add_tx_default_profile(relativeB, 1);

    auto valid_profiles = handler.get_valid_profiles(start_time, end_time, 1);
    auto nShedules = valid_profiles.size();
    EXPECT_EQ(nShedules, 2);
    if (nShedules > 0) {
        EXPECT_EQ(relativeA.chargingSchedule, valid_profiles[0].chargingSchedule);
    }
    if (nShedules > 1) {
        EXPECT_EQ(relativeB.chargingSchedule, valid_profiles[1].chargingSchedule);
    }

    auto schedule =
        handler.calculate_enhanced_composite_schedule(start_time, end_time, 1, ChargingRateUnit::A, false, true);
    // std::cout << "chargingSchedule:" << profileNoCharge.chargingSchedule << std::endl;
    // std::cout << "schedule:" << schedule.chargingSchedulePeriod << std::endl;
    EXPECT_EQ(relativeB.chargingSchedule, schedule);
}

} // namespace
