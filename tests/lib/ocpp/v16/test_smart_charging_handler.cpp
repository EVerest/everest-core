#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
namespace fs = std::filesystem;

#include <database_handler_mock.hpp>
#include <evse_security_mock.hpp>
#include <ocpp/common/call_types.hpp>
#include <ocpp/v16/smart_charging.hpp>
#include <optional>

namespace ocpp {
namespace v16 {

/**
 * Chargepoint Test Fixture
 *
 * Validate Profile Test Matrix:
 *
 * Positive Boundary Conditions:
 * - PB01 Valid Profile
 * - PB02 Valid Profile No startSchedule & handler allows no startSchedule & profile.chargingProfileKind == Absolute
 * - PB03 Valid Profile No startSchedule & handler allows no startSchedule & profile.chargingProfileKind == Relative
 * - PB04 Absolute ChargePointMaxProfile Profile with connector id 0
 * - PB05 Absolute TxDefaultProfile
 * - PB06 Absolute TxProfile ignore_no_transaction == true
 * - PB07 Absolute TxProfile && connector transaction != nullptr && transaction_id matches
 *
 * Negative Boundary Conditions:
 * - NB01 Valid Profile, ConnectorID gt this->connectors.size()
 * - NB02 Valid Profile, ConnectorID lt 0
 * - NB03 profile.stackLevel lt 0
 * - NB04 profile.stackLevel gt profile_max_stack_level
 * - NB05 profile.chargingProfileKind == Absolute && !profile.chargingSchedule.startSchedule
 * - NB06 Number of installed Profiles is > max_charging_profiles_installed
 * - NB07 Invalid ChargingSchedule
 * - NB08 profile.chargingProfileKind == Recurring && !profile.recurrencyKind
 * - NB09 profile.chargingProfileKind == Recurring && !startSchedule
 * - NB10 profile.chargingProfileKind == Recurring && !startSchedule && !allow_charging_profile_without_start_schedule
 * - NB11 Absolute ChargePointMaxProfile Profile with connector id not 0
 * - NB12 Absolute TxProfile connector_id == 0
 * - NB13 Absolute TxProfile && connector transaction == nullptr && ignore_no_transaction == true
 *   NB14 Absolute TxProfile && connector transactionId doesn't match && ignore_no_transaction == true
 *   NB15 Absolute TxProfile && connector transaction == nullptr && ignore_no_transaction == false
 *   NB16 Absolute TxProfile && connector transactionId doesn't match && ignore_no_transaction == false
 */
class ChargepointTestFixture : public testing::Test {
protected:
    void SetUp() override {
    }

    void addConnector(int id) {
        auto connector = Connector{id};

        auto timer = std::unique_ptr<Everest::SteadyTimer>();

        connector.transaction =
            std::make_shared<Transaction>(-1, id, "test", "test", 1, std::nullopt, ocpp::DateTime(), std::move(timer));
        connectors[id] = std::make_shared<Connector>(connector);
    }

    ChargingSchedule createChargeSchedule() {
        return ChargingSchedule{{}};
    }

    ChargingSchedule createChargeSchedule(ChargingRateUnit chargingRateUnit) {
        std::vector<ChargingSchedulePeriod> chargingSchedulePeriod;
        std::optional<int32_t> duration;
        std::optional<ocpp::DateTime> startSchedule;
        std::optional<float> minChargingRate;

        return ChargingSchedule{chargingRateUnit, chargingSchedulePeriod, duration, startSchedule, minChargingRate};
    }

    ChargingProfile createMaxChargingProfile(ChargingSchedule chargingSchedule) {
        auto chargingProfileId = 1;
        auto stackLevel = 1;
        auto chargingProfilePurpose = ChargingProfilePurposeType::ChargePointMaxProfile;
        auto chargingProfileKind = ChargingProfileKindType::Absolute;
        auto recurrencyKind = RecurrencyKindType::Daily;
        return ChargingProfile{chargingProfileId,
                               stackLevel,
                               chargingProfilePurpose,
                               chargingProfileKind,
                               chargingSchedule,
                               {}, // transactionId
                               recurrencyKind,
                               ocpp::DateTime("2024-01-01T00:00:00"),
                               ocpp::DateTime("2024-03-19T00:00:00")};
    }

    ChargingProfile createChargingProfile(ChargingSchedule chargingSchedule) {
        auto chargingProfileId = 1;
        auto stackLevel = 1;
        auto chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        auto chargingProfileKind = ChargingProfileKindType::Absolute;
        auto recurrencyKind = RecurrencyKindType::Daily;
        return ChargingProfile{chargingProfileId,
                               stackLevel,
                               chargingProfilePurpose,
                               chargingProfileKind,
                               chargingSchedule,
                               {}, // transactionId
                               recurrencyKind,
                               ocpp::DateTime("2024-01-01T00:00:00"),
                               ocpp::DateTime("2024-03-19T00:00:00")};
    }

    ChargingProfile createTxChargingProfile(ChargingSchedule chargingSchedule) {
        auto chargingProfileId = 1;
        auto stackLevel = 1;
        auto chargingProfilePurpose = ChargingProfilePurposeType::TxProfile;
        auto chargingProfileKind = ChargingProfileKindType::Absolute;
        auto recurrencyKind = RecurrencyKindType::Daily;
        return ChargingProfile{chargingProfileId,
                               stackLevel,
                               chargingProfilePurpose,
                               chargingProfileKind,
                               chargingSchedule,
                               {}, // transactionId
                               recurrencyKind,
                               ocpp::DateTime("2024-01-01T00:00:00"),
                               ocpp::DateTime("2024-03-19T00:00:00")};
    }

    ChargingProfile createChargingProfile(int id, int stackLevel, ChargingProfilePurposeType chargingProfilePurpose,
                                          ChargingProfileKindType chargingProfileKind,
                                          RecurrencyKindType recurrencyKind, ChargingSchedule chargingSchedule) {
        return ChargingProfile{id,
                               stackLevel,
                               chargingProfilePurpose,
                               chargingProfileKind,
                               chargingSchedule,
                               {}, // transactionId
                               recurrencyKind,
                               ocpp::DateTime("2024-01-01T00:00:00"),
                               ocpp::DateTime("2024-03-19T00:00:00")};
    }

    /**
     * TxDefaultProfile, stack #1: time-of-day limitation to 2 kW, recurring every day from 17:00h to 20:00h.
     *
     * This profile is Example #1 taken from the OCPP 2.0.1 Spec Part 2, page 241.
     */
    ChargingProfile createChargingProfile_Example1() {
        auto chargingRateUnit = ChargingRateUnit::W;
        auto chargingSchedulePeriod = std::vector<ChargingSchedulePeriod>{ChargingSchedulePeriod{0, 2000, 1}};
        auto duration = 1080;
        auto startSchedule = ocpp::DateTime("2024-01-17T17:00:00");
        float minChargingRate = 0;
        auto chargingSchedule =
            ChargingSchedule{chargingRateUnit, chargingSchedulePeriod, duration, startSchedule, minChargingRate};

        auto chargingProfileId = 1;
        auto stackLevel = 1;
        auto chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        auto chargingProfileKind = ChargingProfileKindType::Absolute;
        auto recurrencyKind = RecurrencyKindType::Daily;
        return ChargingProfile{chargingProfileId,
                               stackLevel,
                               chargingProfilePurpose,
                               chargingProfileKind,
                               chargingSchedule,
                               {}, // transactionId
                               recurrencyKind,
                               ocpp::DateTime("2024-01-01T00:00:00"),
                               ocpp::DateTime("2024-03-19T00:00:00")};
    }

    /**
     * TxDefaultProfile, stack #2: overruling Sundays to no limit, recurring every week starting 2020-01-05.
     *
     * This profile is Example #2 taken from the OCPP 2.0.1 Spec Part 2, page 241.
     */
    ChargingProfile createChargingProfile_Example2() {
        auto chargingRateUnit = ChargingRateUnit::W;
        auto chargingSchedulePeriod = std::vector<ChargingSchedulePeriod>{ChargingSchedulePeriod{0, 999999, 1}};
        auto duration = 0;
        auto startSchedule = ocpp::DateTime("2020-01-19T00:00:00");
        float minChargingRate = 0;
        auto chargingSchedule =
            ChargingSchedule{chargingRateUnit, chargingSchedulePeriod, duration, startSchedule, minChargingRate};

        auto chargingProfileId = 11;
        auto stackLevel = 2;
        auto chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        auto chargingProfileKind = ChargingProfileKindType::Recurring;
        auto recurrencyKind = RecurrencyKindType::Weekly;
        return ChargingProfile{chargingProfileId,
                               stackLevel,
                               chargingProfilePurpose,
                               chargingProfileKind,
                               chargingSchedule,
                               {}, // transactionId
                               recurrencyKind,
                               ocpp::DateTime("2024-01-01T00:00:00"),
                               ocpp::DateTime("2024-03-19T00:00:00")};
    }

    SmartChargingHandler* createSmartChargingHandler() {
        const std::string chargepoint_id = "1";
        const fs::path database_path = "na";
        const fs::path init_script_path = "na";
        auto database = std::make_unique<common::DatabaseConnection>(database_path / (chargepoint_id + ".db"));
        std::shared_ptr<DatabaseHandlerMock> database_handler =
            std::make_shared<DatabaseHandlerMock>(std::move(database), init_script_path);
        auto handler = new SmartChargingHandler(connectors, database_handler, true);
        return handler;
    }

    SmartChargingHandler* createSmartChargingHandler(const int number_of_connectors) {
        for (int i = 0; i <= number_of_connectors; i++) {
            addConnector(i);
        }

        const std::string chargepoint_id = "1";
        const fs::path database_path = "na";
        const fs::path init_script_path = "na";

        auto database = std::make_unique<common::DatabaseConnection>(database_path / (chargepoint_id + ".db"));
        std::shared_ptr<DatabaseHandlerMock> database_handler =
            std::make_shared<DatabaseHandlerMock>(std::move(database), init_script_path);

        auto handler = new SmartChargingHandler(connectors, database_handler, true);

        return handler;
    }

    SmartChargingHandler* createSmartChargingHandlerWithChargePointMaxProfile() {
        auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
        const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
        auto handler = createSmartChargingHandler(10);

        profile.chargingProfilePurpose = ChargingProfilePurposeType::ChargePointMaxProfile;
        profile.chargingProfileKind = ChargingProfileKindType::Absolute;
        handler->add_charge_point_max_profile(profile);

        return handler;
    }

    // Default values used within the tests
    std::map<int32_t, std::shared_ptr<Connector>> connectors;
    std::shared_ptr<DatabaseHandler> database_handler;

    const int connector_id = 1;
    bool ignore_no_transaction = true;
    const int profile_max_stack_level = 1;
    const int max_charging_profiles_installed = 1;
    const int charging_schedule_max_periods = 1;
    const DateTime date_start_range = ocpp::DateTime("2023-01-01T00:00:00");
    const DateTime date_end_range = ocpp::DateTime("2024-03-19T00:00:00");
};

/**
 * PB01 Valid Profile
 *
 * Happy path simple test
 */
TEST_F(ChargepointTestFixture, ValidateProfile) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};

    addConnector(1);
    auto handler = createSmartChargingHandler();

    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_TRUE(sut);
}

/**
 * PB01 Valid Profile: Example 1
 *
 * This example is taken from the OCPP 2.0.1 Spec page. 241
 */
TEST_F(ChargepointTestFixture, ValidateProfile__example1) {
    auto profile = createChargingProfile_Example1();
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::W};

    addConnector(1);
    auto handler = createSmartChargingHandler();

    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_TRUE(sut);
}

/**
 * NB01 Valid Profile, ConnectorID gt this->connectors.size()
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ConnectorIdGreaterThanConnectorsSize__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    const int connector_id = INT_MAX;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB02 Valid Profile, ConnectorID lt 0
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfile_NegativeConnectorIdTest__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    const int connector_id = -1;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB03 profile.stackLevel lt 0
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfile_ConnectorIdZero__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    profile.stackLevel = -1;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB04 profile.stackLevel gt this->profile_max_stack_level
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfile_StackLevelGreaterThanMaxStackLevel__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    profile.stackLevel = profile_max_stack_level + 1;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB05 profile.chargingProfileKind == Absolute && !profile.chargingSchedule.startSchedule
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfile_ChargingProfileKindAbsoluteNoStartSchedule__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    // Create a SmartChargingHandler where allow_charging_profile_without_start_schedule is set to false
    addConnector(1);
    auto allow_charging_profile_without_start_schedule = false;
    auto handler =
        new SmartChargingHandler(connectors, database_handler, allow_charging_profile_without_start_schedule);

    profile.chargingProfileKind = ChargingProfileKindType::Absolute;
    profile.chargingSchedule.startSchedule = std::nullopt;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * PB02 Valid Profile No startSchedule & handler allows no startSchedule
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfile_AllowsNoStartSchedule__ReturnsTrue) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    // Create a SmartChargingHandler where allow_charging_profile_without_start_schedule is set to false
    addConnector(1);
    auto handler = createSmartChargingHandler();

    // Configure to have no start schedule
    profile.chargingProfileKind = ChargingProfileKindType::Absolute;
    profile.chargingSchedule.startSchedule = std::nullopt;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_TRUE(sut);
}

/**
 * NB06 Number of installed Profiles is > max_charging_profiles_installed
 */
TEST_F(ChargepointTestFixture,
       ValidateProfile__ValidProfile_InstalledProfilesGreaterThanMaxInstalledProfiles__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    const int max_charging_profiles_installed = 0;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB07 Invalid ChargingSchedule
 *
 * Creating a ChargingProfile with a different ChargingRateUnit
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfile_InvalidChargingSchedule__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    profile.chargingSchedule.chargingSchedulePeriod = std::vector<ChargingSchedulePeriod>{};
    auto handler = createSmartChargingHandler();

    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::W};
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 *  NB08 profile.chargingProfileKind == Recurring && !profile.recurrencyKind
 */
TEST_F(ChargepointTestFixture,
       ValidateProfile__ValidProfile_ChargingProfileKindRecurringNoRecurrencyKind__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    profile.chargingProfileKind = ChargingProfileKindType::Recurring;
    profile.recurrencyKind = std::nullopt;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB09 profile.chargingProfileKind == Recurring && !profile.chargingSchedule.startSchedule
 */
TEST_F(ChargepointTestFixture,
       ValidateProfile__ValidProfile_ChargingProfileKindRecurringNoStartSchedule__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    // Create a SmartChargingHandler where allow_charging_profile_without_start_schedule is set to false
    addConnector(1);
    auto allow_charging_profile_without_start_schedule = false;
    auto handler =
        new SmartChargingHandler(connectors, database_handler, allow_charging_profile_without_start_schedule);

    profile.chargingProfileKind = ChargingProfileKindType::Recurring;
    profile.chargingSchedule.startSchedule = std::nullopt;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * PB03 Valid Profile No startSchedule & handler allows no startSchedule & profile.chargingProfileKind == Relative
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfile_NoStartScheduleAllowedRelative__ReturnsTrue) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};

    addConnector(1);
    auto handler = createSmartChargingHandler();

    profile.chargingProfileKind = ChargingProfileKindType::Recurring;
    profile.chargingSchedule.startSchedule = std::nullopt;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_TRUE(sut);
}

/**
 * NB10 profile.chargingProfileKind == Recurring && !startSchedule && !allow_charging_profile_without_start_schedule
 */
TEST_F(ChargepointTestFixture, ValidateProfile__RecurringNoStartScheduleNotAllowed__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};

    addConnector(1);
    auto allow_charging_profile_without_start_schedule = false;
    auto handler =
        new SmartChargingHandler(connectors, database_handler, allow_charging_profile_without_start_schedule);

    profile.chargingProfileKind = ChargingProfileKindType::Recurring;
    profile.chargingSchedule.startSchedule = std::nullopt;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * PB04 Absolute ChargePointMaxProfile Profile with connector id 0
 *
 * Absolute ChargePointMaxProfile Profile need a connector id of 0
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfile_NotRecurrencyKindConnectorId0__ReturnsTrue) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    profile.chargingProfilePurpose = ChargingProfilePurposeType::ChargePointMaxProfile;
    profile.chargingProfileKind = ChargingProfileKindType::Absolute;
    const int connector_id = 0;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_TRUE(sut);
}

/**
 * NB11 Absolute ChargePointMaxProfile Profile with connector id not 0
 *
 * ChargePointMaxProfile Profiles where chargingProfileKind == Absolute need a connector id of 0 and not had a
 * ChargingProfileKindType of Relative
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfile_NotRecurrencyKindConnectorIdNot0__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    profile.chargingProfilePurpose = ChargingProfilePurposeType::ChargePointMaxProfile;
    profile.chargingProfileKind = ChargingProfileKindType::Absolute;
    const int connector_id = 1;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * PB05 Absolute TxDefaultProfile
 */
TEST_F(ChargepointTestFixture, ValidateProfile__ValidProfileTxDefaultProfile__ReturnsTrue) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};

    addConnector(1);
    auto handler = createSmartChargingHandler();

    profile.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
    profile.chargingProfileKind = ChargingProfileKindType::Absolute;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_TRUE(sut);
}

/**
 * PB06 Absolute TxProfile ignore_no_transaction == true
 */
TEST_F(ChargepointTestFixture, ValidateProfile__AbsoluteTxProfileIgnoreNoTransaction__ReturnsTrue) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};

    addConnector(1);
    auto handler = createSmartChargingHandler();

    profile.chargingProfilePurpose = ChargingProfilePurposeType::TxProfile;
    profile.chargingProfileKind = ChargingProfileKindType::Absolute;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_TRUE(sut);
}

/**
 * PB07 Absolute TxProfile && connector transaction != nullptr && transaction_id matches
 */
TEST_F(ChargepointTestFixture, ValidateProfile_AbsoluteTxProfileTransactionIdMatches__ReturnsTrue) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};

    addConnector(1);
    auto handler = createSmartChargingHandler();

    profile.chargingProfilePurpose = ChargingProfilePurposeType::TxProfile;
    profile.chargingProfileKind = ChargingProfileKindType::Absolute;
    profile.transactionId = 1;
    connectors.at(1)->transaction->set_transaction_id(1);
    bool sut = handler->validate_profile(profile, connector_id, false, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_TRUE(sut);
}

/**
 * NB12 Absolute TxProfile connector_id == 0
 */
TEST_F(ChargepointTestFixture, ValidateProfile__AbsoluteTxProfileConnectorId0__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    auto handler = createSmartChargingHandler();

    profile.chargingProfileKind = ChargingProfileKindType::Absolute;
    profile.chargingProfilePurpose = ChargingProfilePurposeType::TxProfile;
    const int connector_id = 0;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB13 Absolute TxProfile && connector transaction == nullptr && ignore_no_transaction == true
 */

TEST_F(ChargepointTestFixture, ValidateProfile_AbsoluteTxProfileNoActiveTransactionIgnoreNoTransaction__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};

    addConnector(1);
    auto handler = createSmartChargingHandler();

    profile.chargingProfilePurpose = ChargingProfilePurposeType::TxProfile;
    profile.chargingProfileKind = ChargingProfileKindType::Absolute;
    profile.transactionId = 1;
    connectors.at(1)->transaction = nullptr;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB14 Absolute TxProfile && connector transactionId doesn't match && ignore_no_transaction == true
 */

TEST_F(ChargepointTestFixture, ValidateProfile_AbsoluteTxProfileTransactionIdNoMatchIgnoreNoTransaction__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};

    addConnector(1);
    auto handler = createSmartChargingHandler();

    profile.chargingProfilePurpose = ChargingProfilePurposeType::TxProfile;
    profile.chargingProfileKind = ChargingProfileKindType::Absolute;
    profile.transactionId = 1;
    bool sut = handler->validate_profile(profile, connector_id, ignore_no_transaction, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB15 Absolute TxProfile && connector transaction == nullptr && ignore_no_transaction == false
 */

TEST_F(ChargepointTestFixture, ValidateProfile_AbsoluteTxProfileNoActiveTransaction__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};

    addConnector(1);
    auto handler = createSmartChargingHandler();

    profile.chargingProfilePurpose = ChargingProfilePurposeType::TxProfile;
    profile.chargingProfileKind = ChargingProfileKindType::Absolute;
    profile.transactionId = 1;
    connectors.at(1)->transaction = nullptr;
    bool sut = handler->validate_profile(profile, connector_id, false, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 * NB16 Absolute TxProfile && connector transactionId doesn't match && ignore_no_transaction == false
 */

TEST_F(ChargepointTestFixture, ValidateProfile_AbsoluteTxProfileTransactionIdNoMatch__ReturnsFalse) {
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};

    addConnector(1);
    auto handler = createSmartChargingHandler();

    profile.chargingProfilePurpose = ChargingProfilePurposeType::TxProfile;
    profile.chargingProfileKind = ChargingProfileKindType::Absolute;
    profile.transactionId = 1;
    bool sut = handler->validate_profile(profile, connector_id, false, profile_max_stack_level,
                                         max_charging_profiles_installed, charging_schedule_max_periods,
                                         charging_schedule_allowed_charging_rate_units);

    ASSERT_FALSE(sut);
}

/**
 *
 * 2. Testing the branches within ClearAllProfilesWithFilter
 *
 */

// FIXME: Should this be ignored. There are no filter parameters.
TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__AllOptionalsEmpty__ReturnsFalse) {
    GTEST_SKIP() << "No parameter filter is clearing all profiles";

    const int connector_id_1 = 1;
    addConnector(connector_id_1);

    const int connector_id_2 = 2;
    addConnector(connector_id_2);

    auto handler = createSmartChargingHandler();

    auto profile_1 =
        createChargingProfile(1, 1, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_2 =
        createChargingProfile(2, 2, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    handler->add_tx_profile(profile_1, connector_id_1);
    handler->add_tx_profile(profile_2, connector_id_2);

    auto profiles_1 = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_1);
    auto profiles_2 = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_2);
    ASSERT_EQ(1, profiles_1.size());
    ASSERT_EQ(1, profiles_2.size());

    // All empty tokens
    bool sut = handler->clear_all_profiles_with_filter(std::nullopt, std::nullopt, std::nullopt, std::nullopt, false);

    profiles_1 = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_1);
    profiles_2 = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_2);
    ASSERT_EQ(0, profiles_1.size());
    ASSERT_EQ(0, profiles_2.size());

    ASSERT_TRUE(sut);
}

TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__AllOptionalsEmpty_CheckIdOnly__ReturnsFalse) {
    const int connector_id = 1;
    addConnector(connector_id);

    auto handler = createSmartChargingHandler();

    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));

    handler->add_tx_profile(profile, connector_id);

    // All empty tokens
    bool sut = handler->clear_all_profiles_with_filter(std::nullopt, std::nullopt, std::nullopt, std::nullopt, true);

    ASSERT_FALSE(sut);
}

TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__NoProfiles_ProfileId_CheckIdOnly__ReturnsFalse) {
    auto handler = createSmartChargingHandler();

    bool sut = handler->clear_all_profiles_with_filter(1, std::nullopt, std::nullopt, std::nullopt, true);

    ASSERT_FALSE(sut);
}

/**
 * There is an issue open https://github.com/EVerest/libocpp/issues/432 for the below clear_all_profiles_with_filter
 * The current method call will allow for all parameters to be passed in at a single time and then act on all of them.
 * The issue is that a call should either have a profile id and delete that specific profile or any combination of the
 * other three to delete n number of profiles (to a single one if given all three).
 * The logic is exclusionary but the method does not guard against it which can put the system in an odd state.
 */

// 0, 1 and many connectors

// max, default, tx profiles

// profile id only
//  or
//  connector id only

// stacklevel only

// charging profile purpose only

// a mix of them all

// Max

TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__Max_OnlyOneMatchingProfileId_CheckIdOnly__ReturnsTrue) {
    const int connector_id = 0;

    auto handler = createSmartChargingHandler();

    auto profile = createMaxChargingProfile(createChargeSchedule(ChargingRateUnit::A));

    handler->add_charge_point_max_profile(profile);

    auto profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(1, profiles.size());

    bool sut = handler->clear_all_profiles_with_filter(1, std::nullopt, std::nullopt, std::nullopt, true);
    profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);
    ASSERT_EQ(0, profiles.size());

    ASSERT_TRUE(sut);
}

TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__Max_MatchingProfileId_CheckIdOnly__ReturnsTrue) {
    const int connector_id = 0;

    auto handler = createSmartChargingHandler();

    auto profile_1 = createChargingProfile(1, 1, ChargingProfilePurposeType::ChargePointMaxProfile,
                                           ChargingProfileKindType::Absolute, RecurrencyKindType::Daily,
                                           createChargeSchedule(ChargingRateUnit::A));

    auto profile_2 = createChargingProfile(2, 2, ChargingProfilePurposeType::ChargePointMaxProfile,
                                           ChargingProfileKindType::Absolute, RecurrencyKindType::Daily,
                                           createChargeSchedule(ChargingRateUnit::A));

    handler->add_charge_point_max_profile(profile_1);
    handler->add_charge_point_max_profile(profile_2);

    auto profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(2, profiles.size());

    bool sut = handler->clear_all_profiles_with_filter(1, std::nullopt, std::nullopt, std::nullopt, true);
    profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(1, profiles.size());
    ASSERT_TRUE(sut);
}

TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__Max_MultipleNoMatchingProfileId_CheckIdOnly__ReturnsFalse) {
    const int connector_id = 0;

    auto handler = createSmartChargingHandler();

    auto profile_1 = createChargingProfile(1, 1, ChargingProfilePurposeType::ChargePointMaxProfile,
                                           ChargingProfileKindType::Absolute, RecurrencyKindType::Daily,
                                           createChargeSchedule(ChargingRateUnit::A));

    auto profile_2 = createChargingProfile(2, 2, ChargingProfilePurposeType::ChargePointMaxProfile,
                                           ChargingProfileKindType::Absolute, RecurrencyKindType::Daily,
                                           createChargeSchedule(ChargingRateUnit::A));

    handler->add_charge_point_max_profile(profile_1);
    handler->add_charge_point_max_profile(profile_2);

    auto profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(2, profiles.size());

    bool sut = handler->clear_all_profiles_with_filter(3, std::nullopt, std::nullopt, std::nullopt, true);
    profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(2, profiles.size());
    ASSERT_FALSE(sut);
}

// Default

TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__Default_OnlyOneMatchingProfileId_CheckIdOnly__ReturnsTrue) {
    const int connector_id = 1;
    addConnector(connector_id);

    auto handler = createSmartChargingHandler();

    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));

    handler->add_tx_default_profile(profile, connector_id);

    auto profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(1, profiles.size());

    bool sut = handler->clear_all_profiles_with_filter(1, std::nullopt, std::nullopt, std::nullopt, true);
    profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);
    ASSERT_EQ(0, profiles.size());

    ASSERT_TRUE(sut);
}

TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__Default_MatchingProfileId_CheckIdOnly__ReturnsTrue) {
    const int connector_id = 1;
    addConnector(connector_id);

    auto handler = createSmartChargingHandler();

    auto profile_1 =
        createChargingProfile(1, 1, ChargingProfilePurposeType::TxDefaultProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_2 =
        createChargingProfile(2, 2, ChargingProfilePurposeType::TxDefaultProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    handler->add_tx_default_profile(profile_1, connector_id);
    handler->add_tx_default_profile(profile_2, connector_id);

    auto profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(2, profiles.size());

    bool sut = handler->clear_all_profiles_with_filter(1, std::nullopt, std::nullopt, std::nullopt, true);
    profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(1, profiles.size());
    ASSERT_TRUE(sut);
}

TEST_F(ChargepointTestFixture,
       ClearAllProfilesWithFilter__Default_MultipleNoMatchingProfileId_CheckIdOnly__ReturnsFalse) {
    const int connector_id = 1;
    addConnector(connector_id);

    auto handler = createSmartChargingHandler();

    auto profile_1 =
        createChargingProfile(1, 1, ChargingProfilePurposeType::TxDefaultProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_2 =
        createChargingProfile(2, 2, ChargingProfilePurposeType::TxDefaultProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    handler->add_tx_default_profile(profile_1, connector_id);
    handler->add_tx_default_profile(profile_2, connector_id);

    auto profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(2, profiles.size());

    bool sut = handler->clear_all_profiles_with_filter(3, std::nullopt, std::nullopt, std::nullopt, true);
    profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(2, profiles.size());
    ASSERT_FALSE(sut);
}

// TX

TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__Tx_OnlyOneMatchingProfileId_CheckIdOnly__ReturnsTrue) {
    const int connector_id = 1;
    addConnector(connector_id);

    auto handler = createSmartChargingHandler();

    auto profile = createTxChargingProfile(createChargeSchedule(ChargingRateUnit::A));

    handler->add_tx_profile(profile, connector_id);

    auto profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(1, profiles.size());

    bool sut = handler->clear_all_profiles_with_filter(1, std::nullopt, std::nullopt, std::nullopt, true);
    profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);
    ASSERT_EQ(0, profiles.size());

    ASSERT_TRUE(sut);
}

TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__Tx_MatchingProfileId_CheckIdOnly__ReturnsTrue) {
    const int connector_id = 1;
    addConnector(connector_id);

    auto handler = createSmartChargingHandler();

    auto profile_1 =
        createChargingProfile(1, 1, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_2 =
        createChargingProfile(2, 2, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    handler->add_tx_profile(profile_1, connector_id);
    handler->add_tx_profile(profile_2, connector_id);

    auto profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(2, profiles.size());

    bool sut = handler->clear_all_profiles_with_filter(1, std::nullopt, std::nullopt, std::nullopt, true);
    profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(1, profiles.size());
    ASSERT_TRUE(sut);
}

TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__Tx_MultipleNoMatchingProfileId_CheckIdOnly__ReturnsFalse) {
    const int connector_id = 1;
    addConnector(connector_id);

    auto handler = createSmartChargingHandler();

    auto profile_1 =
        createChargingProfile(1, 1, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_2 =
        createChargingProfile(2, 2, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    handler->add_tx_profile(profile_1, connector_id);
    handler->add_tx_profile(profile_2, connector_id);

    auto profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(2, profiles.size());

    bool sut = handler->clear_all_profiles_with_filter(3, std::nullopt, std::nullopt, std::nullopt, true);
    profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(2, profiles.size());
    ASSERT_FALSE(sut);
}

TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__ConnectorId__ReturnsTrue) {
    const int connector_id_1 = 100;
    addConnector(connector_id_1);

    const int connector_id_2 = 200;
    addConnector(connector_id_2);

    auto handler = createSmartChargingHandler();

    auto profile_c1_1 =
        createChargingProfile(1, 1, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_c1_2 =
        createChargingProfile(2, 2, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_c2_3 =
        createChargingProfile(3, 1, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_c2_4 =
        createChargingProfile(4, 2, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    handler->add_tx_profile(profile_c1_1, connector_id_1);
    handler->add_tx_profile(profile_c1_2, connector_id_1);

    handler->add_tx_profile(profile_c2_3, connector_id_2);
    handler->add_tx_profile(profile_c2_4, connector_id_2);

    auto connector_id_1_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_1);
    auto connector_id_2_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_2);

    ASSERT_EQ(2, connector_id_1_profiles.size());
    ASSERT_EQ(2, connector_id_2_profiles.size());

    auto check_id_only = false;

    bool sut = handler->clear_all_profiles_with_filter(std::nullopt, connector_id_1, std::nullopt, std::nullopt,
                                                       check_id_only);

    connector_id_1_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_1);
    connector_id_2_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_2);

    ASSERT_EQ(0, connector_id_1_profiles.size());
    ASSERT_EQ(2, connector_id_2_profiles.size());
    ASSERT_TRUE(sut);
}

// TODO: Needs negative

TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__StackLevel__ReturnsTrue) {
    const int connector_id_1 = 100;
    addConnector(connector_id_1);

    const int connector_id_2 = 200;
    addConnector(connector_id_2);

    auto handler = createSmartChargingHandler();

    auto profile_c1_1 =
        createChargingProfile(1, 1, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_c1_2 =
        createChargingProfile(2, 2, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_c2_3 =
        createChargingProfile(3, 1, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_c2_4 =
        createChargingProfile(4, 2, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    handler->add_tx_profile(profile_c1_1, connector_id_1);
    handler->add_tx_profile(profile_c1_2, connector_id_1);

    handler->add_tx_profile(profile_c2_3, connector_id_2);
    handler->add_tx_profile(profile_c2_4, connector_id_2);

    auto connector_id_1_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_1);
    auto connector_id_2_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_2);

    ASSERT_EQ(2, connector_id_1_profiles.size());
    ASSERT_EQ(2, connector_id_2_profiles.size());

    auto check_id_only = false;

    bool sut = handler->clear_all_profiles_with_filter(std::nullopt, std::nullopt, 1, std::nullopt, check_id_only);

    connector_id_1_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_1);
    connector_id_2_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_2);

    ASSERT_EQ(1, connector_id_1_profiles.size());
    ASSERT_EQ(1, connector_id_2_profiles.size());
    ASSERT_TRUE(sut);
}

// TODO: Needs negative

TEST_F(ChargepointTestFixture, ClearAllProfilesWithFilter__ChargingProfilePurposeType__ReturnsTrue) {
    const int connector_id_1 = 100;
    addConnector(connector_id_1);

    const int connector_id_2 = 200;
    addConnector(connector_id_2);

    auto handler = createSmartChargingHandler();

    auto profile_c1_1 =
        createChargingProfile(1, 1, ChargingProfilePurposeType::TxDefaultProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_c1_2 =
        createChargingProfile(2, 2, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_c2_3 =
        createChargingProfile(3, 1, ChargingProfilePurposeType::TxDefaultProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_c2_4 =
        createChargingProfile(4, 2, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    handler->add_tx_default_profile(profile_c1_1, connector_id_1);
    handler->add_tx_profile(profile_c1_2, connector_id_1);

    handler->add_tx_default_profile(profile_c2_3, connector_id_2);
    handler->add_tx_profile(profile_c2_4, connector_id_2);

    auto connector_id_1_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_1);
    auto connector_id_2_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_2);

    ASSERT_EQ(2, connector_id_1_profiles.size());
    ASSERT_EQ(2, connector_id_2_profiles.size());

    auto check_id_only = false;

    bool sut = handler->clear_all_profiles_with_filter(std::nullopt, std::nullopt, std::nullopt,
                                                       ChargingProfilePurposeType::TxDefaultProfile, check_id_only);

    connector_id_1_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_1);
    connector_id_2_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_2);

    ASSERT_EQ(1, connector_id_1_profiles.size());
    ASSERT_EQ(1, connector_id_2_profiles.size());
    ASSERT_TRUE(sut);
}

// TODO: Needs negative

TEST_F(ChargepointTestFixture,
       ClearAllProfilesWithFilter__ConnectorIdStackLevelChargingProfilePurposeType__ReturnsTrue) {
    const int connector_id_1 = 100;
    addConnector(connector_id_1);

    const int connector_id_2 = 200;
    addConnector(connector_id_2);

    auto handler = createSmartChargingHandler();

    auto profile_c1_1 =
        createChargingProfile(1, 1, ChargingProfilePurposeType::TxDefaultProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_c1_2 =
        createChargingProfile(2, 2, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_c2_3 =
        createChargingProfile(3, 1, ChargingProfilePurposeType::TxDefaultProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    auto profile_c2_4 =
        createChargingProfile(4, 2, ChargingProfilePurposeType::TxProfile, ChargingProfileKindType::Absolute,
                              RecurrencyKindType::Daily, createChargeSchedule(ChargingRateUnit::A));

    handler->add_tx_default_profile(profile_c1_1, connector_id_1);
    handler->add_tx_profile(profile_c1_2, connector_id_1);

    handler->add_tx_default_profile(profile_c2_3, connector_id_2);
    handler->add_tx_profile(profile_c2_4, connector_id_2);

    auto connector_id_1_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_1);
    auto connector_id_2_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_2);

    ASSERT_EQ(2, connector_id_1_profiles.size());
    ASSERT_EQ(2, connector_id_2_profiles.size());

    auto check_id_only = false;

    bool sut = handler->clear_all_profiles_with_filter(std::nullopt, 100, 1,
                                                       ChargingProfilePurposeType::TxDefaultProfile, check_id_only);

    connector_id_1_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_1);
    connector_id_2_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id_2);

    ASSERT_EQ(1, connector_id_1_profiles.size());
    ASSERT_EQ(2, connector_id_2_profiles.size());
    ASSERT_TRUE(sut);
}

/**
 * SmartChargingHandler::add_charge_point_max_profile tests
 */
TEST_F(ChargepointTestFixture, AddChargePointMaxProfile) {
    auto handler = createSmartChargingHandlerWithChargePointMaxProfile();

    auto now = ocpp::DateTime();
    auto valid_profiles = handler->get_valid_profiles(date_start_range, date_end_range, 0);
    ASSERT_EQ(1, valid_profiles.size());
    auto retrieved = valid_profiles[0];

    ASSERT_EQ(ChargingProfilePurposeType::ChargePointMaxProfile, retrieved.chargingProfilePurpose);
    ASSERT_EQ(ChargingProfileKindType::Absolute, retrieved.chargingProfileKind);
}

TEST_F(ChargepointTestFixture, AddTxDefaultProfile_ConnectorId_eq_0) {
    auto handler = createSmartChargingHandler(1);
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    bool is_profile_valid = handler->validate_profile(
        profile, connector_id, ignore_no_transaction, profile_max_stack_level, max_charging_profiles_installed,
        charging_schedule_max_periods, charging_schedule_allowed_charging_rate_units);
    ASSERT_TRUE(is_profile_valid);

    const int connector_id = 0;
    handler->add_tx_default_profile(profile, connector_id);
    // While the connector id is 0 when it is added, it is retrieved with a connector id of 1
    // See AddTxDefaultProfile_ConnectorId_eq_0_Retrieved_at_0__NoProfilesReturned for a demonstration of this behavior
    const int retrieved_connector_id = 1;
    auto valid_profiles = handler->get_valid_profiles(date_start_range, date_end_range, retrieved_connector_id);
    auto retrieved = valid_profiles[0];

    ASSERT_EQ(1, valid_profiles.size());
    ASSERT_EQ(ChargingProfileKindType::Absolute, retrieved.chargingProfileKind);
    ASSERT_EQ(ChargingProfilePurposeType::TxDefaultProfile, retrieved.chargingProfilePurpose);
}

TEST_F(ChargepointTestFixture, AddTxDefaultProfile_ConnectorId_eq_0_Retrieved_at_0__NoProfilesReturned) {
    auto handler = createSmartChargingHandler(1);
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};
    bool is_profile_valid = handler->validate_profile(
        profile, connector_id, ignore_no_transaction, profile_max_stack_level, max_charging_profiles_installed,
        charging_schedule_max_periods, charging_schedule_allowed_charging_rate_units);
    ASSERT_TRUE(is_profile_valid);

    const int connector_id = 0;
    handler->add_tx_default_profile(profile, connector_id);
    // When profiles are retrieved with the same connector id of 0, nothing is returned
    // See AddTxDefaultProfile_ConnectorId_eq_0 for a demonstration of how to retrieve the profile
    auto valid_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);

    ASSERT_EQ(0, valid_profiles.size());
}

/**
 * SmartChargingHandler::add_tx_default_profile test
 */
TEST_F(ChargepointTestFixture, AddTxDefaultProfile__ConnectorId_gt_0) {
    auto handler = createSmartChargingHandlerWithChargePointMaxProfile();
    auto valid_profiles = handler->get_valid_profiles(date_start_range, date_end_range, 0);
    ASSERT_EQ(1, valid_profiles.size());
    auto profile = createChargingProfile(createChargeSchedule(ChargingRateUnit::A));
    const std::vector<ChargingRateUnit>& charging_schedule_allowed_charging_rate_units{ChargingRateUnit::A};

    const int connector_id = 2;
    handler->add_tx_default_profile(profile, connector_id);

    valid_profiles = handler->get_valid_profiles(date_start_range, date_end_range, connector_id);
    ASSERT_EQ(2, valid_profiles.size());
    auto chargepoint_max_profile = valid_profiles[0];
    ASSERT_EQ(ChargingProfilePurposeType::ChargePointMaxProfile, chargepoint_max_profile.chargingProfilePurpose);
    ASSERT_EQ(ChargingProfileKindType::Absolute, chargepoint_max_profile.chargingProfileKind);
    auto tx_default_profile = valid_profiles[1];
    ASSERT_EQ(ChargingProfilePurposeType::TxDefaultProfile, tx_default_profile.chargingProfilePurpose);
    ASSERT_EQ(ChargingProfileKindType::Absolute, tx_default_profile.chargingProfileKind);
}

} // namespace v16
} // namespace ocpp
