// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include "date/tz.h"
#include "lib/ocpp/common/database_testing_utils.hpp"
#include "ocpp/common/types.hpp"
#include "ocpp/v201/ctrlr_component_variables.hpp"
#include "ocpp/v201/device_model.hpp"
#include "ocpp/v201/device_model_storage_sqlite.hpp"
#include "ocpp/v201/init_device_model_db.hpp"
#include "ocpp/v201/ocpp_types.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <sqlite3.h>

#include <component_state_manager_mock.hpp>
#include <device_model_storage_mock.hpp>
#include <evse_manager_fake.hpp>
#include <evse_mock.hpp>
#include <evse_security_mock.hpp>
#include <ocpp/common/call_types.hpp>
#include <ocpp/v201/evse.hpp>
#include <ocpp/v201/ocpp_enums.hpp>
#include <ocpp/v201/smart_charging.hpp>
#include <optional>

#include "comparators.hpp"
#include <sstream>
#include <vector>

namespace ocpp::v201 {

static const int NR_OF_EVSES = 2;
static const int STATION_WIDE_ID = 0;
static const int DEFAULT_EVSE_ID = 1;
static const int DEFAULT_PROFILE_ID = 1;
static const int DEFAULT_STACK_LEVEL = 1;
static const int DEFAULT_REQUEST_ID = 1;
static const std::string DEFAULT_TX_ID = "10c75ff7-74f5-44f5-9d01-f649f3ac7b78";
const static std::string MIGRATION_FILES_PATH = "./resources/v201/device_model_migration_files";
const static std::string SCHEMAS_PATH = "./resources/example_config/v201/component_config";
const static std::string DEVICE_MODEL_DB_IN_MEMORY_PATH = "file::memory:?cache=shared";

class TestSmartChargingHandler : public SmartChargingHandler {
public:
    using SmartChargingHandler::get_profiles_on_evse;
    using SmartChargingHandler::validate_charging_station_max_profile;
    using SmartChargingHandler::validate_evse_exists;
    using SmartChargingHandler::validate_profile_schedules;
    using SmartChargingHandler::validate_tx_default_profile;
    using SmartChargingHandler::validate_tx_profile;
    using SmartChargingHandler::verify_no_conflicting_external_constraints_id;

    using SmartChargingHandler::SmartChargingHandler;
};

class SmartChargingHandlerTestFixtureV201 : public DatabaseTestingUtils {
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

    ChargingProfileCriterion create_charging_profile_criteria(
        std::optional<std::vector<ocpp::v201::ChargingLimitSourceEnum>> sources = std::nullopt,
        std::optional<std::vector<int32_t>> ids = std::nullopt,
        std::optional<ChargingProfilePurposeEnum> purpose = std::nullopt,
        std::optional<int32_t> stack_level = std::nullopt) {
        ChargingProfileCriterion criteria;
        criteria.chargingLimitSource = sources;
        criteria.chargingProfileId = ids;
        criteria.chargingProfilePurpose = purpose;
        criteria.stackLevel = stack_level;
        return criteria;
    }

    GetChargingProfilesRequest create_get_charging_profile_request(int32_t request_id,
                                                                   ChargingProfileCriterion criteria,
                                                                   std::optional<int32_t> evse_id = std::nullopt) {
        GetChargingProfilesRequest req;
        req.requestId = request_id;
        req.chargingProfile = criteria;
        req.evseId = evse_id;
        return req;
    }

    ClearChargingProfileRequest
    create_clear_charging_profile_request(std::optional<int32_t> id = std::nullopt,
                                          std::optional<ClearChargingProfile> criteria = std::nullopt) {
        ClearChargingProfileRequest req;
        req.chargingProfileId = id;
        req.chargingProfileCriteria = criteria;
        return req;
    }

    ClearChargingProfile create_clear_charging_profile(std::optional<int32_t> evse_id = std::nullopt,
                                                       std::optional<ChargingProfilePurposeEnum> purpose = std::nullopt,
                                                       std::optional<int32_t> stack_level = std::nullopt) {
        return ClearChargingProfile{
            .customData = {}, .evseId = evse_id, .chargingProfilePurpose = purpose, .stackLevel = stack_level};
    }

    void create_device_model_db(const std::string& path) {
        InitDeviceModelDb db(path, MIGRATION_FILES_PATH);
        db.initialize_database(SCHEMAS_PATH, true);
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

    std::string uuid() {
        std::stringstream s;
        s << uuid_generator();
        return s.str();
    }

    void install_profile_on_evse(int evse_id, int profile_id,
                                 std::optional<ocpp::DateTime> validFrom = ocpp::DateTime("2024-01-01T17:00:00"),
                                 std::optional<ocpp::DateTime> validTo = ocpp::DateTime("2024-02-01T17:00:00")) {
        auto existing_profile = create_charging_profile(
            profile_id, ChargingProfilePurposeEnum::TxDefaultProfile, create_charge_schedule(ChargingRateUnitEnum::A),
            {}, ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, validFrom, validTo);
        handler.add_profile(existing_profile, evse_id);
    }

    std::optional<ChargingProfile> add_valid_profile_to(int evse_id, int profile_id) {
        auto periods = create_charging_schedule_periods({0, 1, 2});
        auto profile = create_charging_profile(
            profile_id, ChargingProfilePurposeEnum::TxDefaultProfile,
            create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
        auto response = handler.validate_and_add_profile(profile, evse_id);
        if (response.status == ChargingProfileStatusEnum::Accepted) {
            return profile;
        } else {
            return {};
        }
    }

    // Default values used within the tests
    std::unique_ptr<EvseManagerFake> evse_manager = std::make_unique<EvseManagerFake>(NR_OF_EVSES);

    sqlite3* db_handle;

    bool ignore_no_transaction = true;
    std::shared_ptr<DeviceModel> device_model = create_device_model();
    TestSmartChargingHandler handler = create_smart_charging_handler();
    boost::uuids::random_generator uuid_generator = boost::uuids::random_generator();
};

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR03_IfTxProfileIsMissingTransactionId_ThenProfileIsInvalid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {});
    auto sut = handler.validate_tx_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileMissingTransactionId));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR05_IfExistingChargingProfileWithSameIdIsChargingStationExternalConstraints_ThenProfileIsInvalid) {
    auto external_constraints =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationExternalConstraints,
                                create_charge_schedule(ChargingRateUnitEnum::A), {});
    handler.add_profile(external_constraints, STATION_WIDE_ID);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {});
    auto sut = handler.verify_no_conflicting_external_constraints_id(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ExistingChargingStationExternalConstraints));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR16_IfTxProfileHasEvseIdNotGreaterThanZero_ThenProfileIsInvalid) {
    auto wrong_evse_id = STATION_WIDE_ID;
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID);
    auto sut = handler.validate_tx_profile(profile, wrong_evse_id);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileEvseIdNotGreaterThanZero));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR33_IfTxProfileTransactionIsNotOnEvse_ThenProfileIsInvalid) {
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, "wrong transaction id");
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID);
    auto sut = handler.validate_tx_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileTransactionNotOnEvse));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR09_IfTxProfileEvseHasNoActiveTransaction_ThenProfileIsInvalid) {
    auto connector_id = 1;
    auto meter_start = MeterValue();
    auto id_token = IdToken();
    auto date_time = ocpp::DateTime("2024-01-17T17:00:00");
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID);
    auto sut = handler.validate_tx_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileEvseHasNoActiveTransaction));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR19AndFR48_NumberPhasesOtherThan1AndPhaseToUseSet_ThenProfileInvalid) {
    auto periods = create_charging_schedule_periods_with_phases(0, 0, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID,
        ChargingProfileKindEnum::Absolute, 1);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodInvalidPhaseToUse));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR20AndFR48_IfPhaseToUseSetAndACPhaseSwitchingSupportedUndefined_ThenProfileIsInvalid) {
    // As a device model with ac switching supported default set to 'true', we want to create a new database with the
    // ac switching support not set. But this is an in memory database, which is kept open until all handles to it are
    // closed. So we close all connections to the database.
    database->close_connection();
    device_model = nullptr;
    // Re open the connection, to an empty in memory database.
    database->open_connection();
    // And recreate the device model.
    auto device_model_without_ac_phase_switching = create_device_model({});
    device_model = std::move(device_model_without_ac_phase_switching);

    auto periods = create_charging_schedule_periods_with_phases(0, 1, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID,
        ChargingProfileKindEnum::Absolute, 1);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut,
                testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodPhaseToUseACPhaseSwitchingUnsupported));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR20AndFR48_IfPhaseToUseSetAndACPhaseSwitchingSupportedFalse_ThenProfileIsInvalid) {
    // As a device model with ac switching supported default set to 'true', we want to create a new database with the
    // ac switching support not set. But this is an in memory database, which is kept open until all handles to it are
    // closed. So we close all connections to the database.
    database->close_connection();
    device_model = nullptr;
    // Re open the connection, to an empty in memory database.
    database->open_connection();
    // And recreate the device model setting ac phase switching to 'false'.
    auto device_model_with_false_ac_phase_switching = create_device_model("false");
    device_model = std::move(device_model_with_false_ac_phase_switching);

    auto periods = create_charging_schedule_periods_with_phases(0, 1, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID,
        ChargingProfileKindEnum::Absolute, 1);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut,
                testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodPhaseToUseACPhaseSwitchingUnsupported));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR20AndFR48_IfPhaseToUseSetAndACPhaseSwitchingSupportedTrue_ThenProfileIsNotInvalid) {
    auto periods = create_charging_schedule_periods_with_phases(0, 1, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID,
        ChargingProfileKindEnum::Absolute, 1);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Not(ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR26_IfChargingRateUnitIsNotInChargingScheduleChargingRateUnits_ThenProfileIsInvalid) {
    const auto& charging_rate_unit_cv = ControllerComponentVariables::ChargingScheduleChargingRateUnit;
    device_model->set_value(charging_rate_unit_cv.component, charging_rate_unit_cv.variable.value(),
                            AttributeEnum::Actual, "W", "test", true);

    auto periods = create_charging_schedule_periods(0, 1, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID,
        ChargingProfileKindEnum::Absolute, 1);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingScheduleChargingRateUnitUnsupported));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01_IfChargingSchedulePeriodsAreMissing_ThenProfileIsInvalid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileNoChargingSchedulePeriods));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR31_IfStartPeriodOfFirstChargingSchedulePeriodIsNotZero_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(1);
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), DEFAULT_TX_ID);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileFirstStartScheduleIsNotZero));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR35_IfChargingSchedulePeriodsAreNotInChonologicalOrder_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods({0, 2, 1});
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), DEFAULT_TX_ID);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodsOutOfOrder));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01_ValidateChargingStationMaxProfile_NotChargingStationMaxProfile_Invalid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A));

    auto sut = handler.validate_charging_station_max_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::InvalidProfileType));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K04FR03_ValidateChargingStationMaxProfile_EvseIDgt0_Invalid) {
    const int EVSE_ID_1 = DEFAULT_EVSE_ID;
    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods));

    auto sut = handler.validate_charging_station_max_profile(profile, EVSE_ID_1);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingStationMaxProfileEvseIdGreaterThanZero));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR38_ChargingProfilePurposeIsChargingStationMaxProfile_KindIsAbsolute_Valid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A));

    auto sut = handler.validate_charging_station_max_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR38_ChargingProfilePurposeIsChargingStationMaxProfile_KindIsRecurring_Valid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {},
                                           ChargingProfileKindEnum::Recurring);

    auto sut = handler.validate_charging_station_max_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR38_ChargingProfilePurposeIsChargingStationMaxProfile_KindIsRelative_Invalid) {
    auto profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                create_charge_schedule(ChargingRateUnitEnum::A), {}, ChargingProfileKindEnum::Relative);

    auto sut = handler.validate_charging_station_max_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingStationMaxProfileCannotBeRelative));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR39_IfTxProfileHasSameTransactionAndStackLevelAsAnotherTxProfile_ThenProfileIsInvalid) {
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    auto same_stack_level = 42;
    auto profile_1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID,
                                             ChargingProfileKindEnum::Absolute, same_stack_level);
    auto profile_2 = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID,
                                             ChargingProfileKindEnum::Absolute, same_stack_level);
    handler.add_profile(profile_2, DEFAULT_EVSE_ID);
    auto sut = handler.validate_tx_profile(profile_1, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileConflictingStackLevel));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR39_IfTxProfileHasDifferentTransactionButSameStackLevelAsAnotherTxProfile_ThenProfileIsValid) {
    std::string different_transaction_id = uuid();
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    auto same_stack_level = 42;
    auto profile_1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID,
                                             ChargingProfileKindEnum::Absolute, same_stack_level);
    auto profile_2 = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), different_transaction_id,
                                             ChargingProfileKindEnum::Absolute, same_stack_level);
    handler.add_profile(profile_2, DEFAULT_EVSE_ID);
    auto sut = handler.validate_tx_profile(profile_1, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR39_IfTxProfileHasSameTransactionButDifferentStackLevelAsAnotherTxProfile_ThenProfileIsValid) {
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    auto stack_level_1 = 42;
    auto stack_level_2 = 43;

    auto profile_1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID,
                                             ChargingProfileKindEnum::Absolute, stack_level_1);
    auto profile_2 = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID,
                                             ChargingProfileKindEnum::Absolute, stack_level_2);

    handler.add_profile(profile_2, DEFAULT_EVSE_ID);
    auto sut = handler.validate_tx_profile(profile_1, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K05_IfTxProfileHasNoTransactionIdButAddChargingProfileSourceIsRequestRemoteStartTransaction_ThenProfileIsValid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {});
    auto sut =
        handler.validate_tx_profile(profile, DEFAULT_EVSE_ID, AddChargingProfileSource::RequestStartTransactionRequest);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR40_IfChargingProfileKindIsAbsoluteAndStartScheduleDoesNotExist_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), DEFAULT_TX_ID,
                                           ChargingProfileKindEnum::Absolute, 1);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileMissingRequiredStartSchedule));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR40_IfChargingProfileKindIsRecurringAndStartScheduleDoesNotExist_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), DEFAULT_TX_ID,
                                           ChargingProfileKindEnum::Recurring, 1);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileMissingRequiredStartSchedule));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR41_IfChargingProfileKindIsRelativeAndStartScheduleDoesExist_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID,
        ChargingProfileKindEnum::Relative, 1);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileExtraneousStartSchedule));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR28_WhenEvseDoesNotExistThenReject) {
    auto sut = handler.validate_evse_exists(NR_OF_EVSES + 1);
    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::EvseDoesNotExist));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR28_WhenEvseDoesExistThenAccept) {
    auto sut = handler.validate_evse_exists(DEFAULT_EVSE_ID);
    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

/*
 * 0 - For K01.FR.52, we return DuplicateTxDefaultProfileFound if an existing profile is on an EVSE,
 *  and a new profile with the same stack level and different profile ID will be associated with EVSE ID = 0.
 * 1 - For K01.FR.53, we return DuplicateTxDefaultProfileFound if an existing profile is on an EVSE ID = 0,
 *  and a new profile with the same stack level and different profile ID will be associated with an EVSE.
 * 2-7 - We return Valid for any other case, such as using the same EVSE or using the same profile ID.
 */
class SmartChargingHandlerTestFixtureV201_FR52
    : public SmartChargingHandlerTestFixtureV201,
      public ::testing::WithParamInterface<std::tuple<int, int, ProfileValidationResultEnum>> {};

INSTANTIATE_TEST_SUITE_P(TxDefaultProfileValidationV201_Param_Test_Instantiate_FR52,
                         SmartChargingHandlerTestFixtureV201_FR52,
                         testing::Values(std::make_tuple(DEFAULT_PROFILE_ID + 1, DEFAULT_STACK_LEVEL,
                                                         ProfileValidationResultEnum::DuplicateTxDefaultProfileFound),
                                         std::make_tuple(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL,
                                                         ProfileValidationResultEnum::Valid),
                                         std::make_tuple(DEFAULT_PROFILE_ID + 1, DEFAULT_STACK_LEVEL + 1,
                                                         ProfileValidationResultEnum::Valid)));

TEST_P(SmartChargingHandlerTestFixtureV201_FR52, K01FR52_TxDefaultProfileValidationV201Tests) {
    auto [added_profile_id, added_stack_level, expected] = GetParam();
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto profile = create_charging_profile(added_profile_id, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {},
                                           ChargingProfileKindEnum::Absolute, added_stack_level);
    auto sut = handler.validate_tx_default_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(expected));
}

class SmartChargingHandlerTestFixtureV201_FR53
    : public SmartChargingHandlerTestFixtureV201,
      public ::testing::WithParamInterface<std::tuple<int, int, ProfileValidationResultEnum>> {};

INSTANTIATE_TEST_SUITE_P(TxDefaultProfileValidationV201_Param_Test_Instantiate_FR53,
                         SmartChargingHandlerTestFixtureV201_FR53,
                         testing::Values(std::make_tuple(DEFAULT_PROFILE_ID + 1, DEFAULT_STACK_LEVEL,
                                                         ProfileValidationResultEnum::DuplicateTxDefaultProfileFound),
                                         std::make_tuple(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL,
                                                         ProfileValidationResultEnum::Valid),
                                         std::make_tuple(DEFAULT_PROFILE_ID + 1, DEFAULT_STACK_LEVEL + 1,
                                                         ProfileValidationResultEnum::Valid)));

TEST_P(SmartChargingHandlerTestFixtureV201_FR53, K01FR53_TxDefaultProfileValidationV201Tests) {
    auto [added_profile_id, added_stack_level, expected] = GetParam();
    install_profile_on_evse(STATION_WIDE_ID, DEFAULT_PROFILE_ID);

    auto profile = create_charging_profile(added_profile_id, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {},
                                           ChargingProfileKindEnum::Absolute, added_stack_level);
    auto sut = handler.validate_tx_default_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(expected));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR52_TxDefaultProfileValidIfAppliedToWholeSystemAgain) {
    install_profile_on_evse(STATION_WIDE_ID, DEFAULT_PROFILE_ID);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {},
                                           ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto sut = handler.validate_tx_default_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR53_TxDefaultProfileValidIfAppliedToExistingEvseAgain) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {},
                                           ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto sut = handler.validate_tx_default_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR53_TxDefaultProfileValidIfAppliedToDifferentEvse) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {},
                                           ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto sut = handler.validate_tx_default_profile(profile, DEFAULT_EVSE_ID + 1);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR44_IfNumberPhasesProvidedForDCEVSE_ThenProfileIsInvalid) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::DC));

    auto periods = create_charging_schedule_periods(0, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = handler.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR44_IfPhaseToUseProvidedForDCEVSE_ThenProfileIsInvalid) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::DC));

    auto periods = create_charging_schedule_periods(0, 1, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = handler.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR44_IfNumberPhasesProvidedForDCChargingStation_ThenProfileIsInvalid) {
    device_model->set_value(ControllerComponentVariables::ChargingStationSupplyPhases.component,
                            ControllerComponentVariables::ChargingStationSupplyPhases.variable.value(),
                            AttributeEnum::Actual, std::to_string(0), "test", true);

    auto periods = create_charging_schedule_periods(0, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = handler.validate_profile_schedules(profile, std::nullopt);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR44_IfPhaseToUseProvidedForDCChargingStation_ThenProfileIsInvalid) {
    device_model->set_value(ControllerComponentVariables::ChargingStationSupplyPhases.component,
                            ControllerComponentVariables::ChargingStationSupplyPhases.variable.value(),
                            AttributeEnum::Actual, std::to_string(0), "test", true);

    auto periods = create_charging_schedule_periods(0, 1, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = handler.validate_profile_schedules(profile, std::nullopt);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR45_IfNumberPhasesGreaterThanChargingStationSupplyPhasesForACEVSE_ThenProfileIsInvalid) {
    device_model->set_value(ControllerComponentVariables::ChargingStationSupplyPhases.component,
                            ControllerComponentVariables::ChargingStationSupplyPhases.variable.value(),
                            AttributeEnum::Actual, std::to_string(0), "test", true);
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::AC));

    auto periods = create_charging_schedule_periods(0, 4);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = handler.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedNumberPhases));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR45_IfNumberPhasesGreaterThanChargingStationSupplyPhasesForACChargingStation_ThenProfileIsInvalid) {
    device_model->set_value(ControllerComponentVariables::ChargingStationSupplyPhases.component,
                            ControllerComponentVariables::ChargingStationSupplyPhases.variable.value(),
                            AttributeEnum::Actual, std::to_string(1), "test", true);

    auto periods = create_charging_schedule_periods(0, 4);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = handler.validate_profile_schedules(profile, std::nullopt);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedNumberPhases));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR49_IfNumberPhasesMissingForACEVSE_ThenSetNumberPhasesToThree) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::AC));

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = handler.validate_profile_schedules(profile, &mock_evse);

    auto numberPhases = profile.chargingSchedule[0].chargingSchedulePeriod[0].numberPhases;

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
    EXPECT_THAT(numberPhases, testing::Eq(3));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR49_IfNumberPhasesMissingForACChargingStation_ThenSetNumberPhasesToThree) {
    device_model->set_value(ControllerComponentVariables::ChargingStationSupplyPhases.component,
                            ControllerComponentVariables::ChargingStationSupplyPhases.variable.value(),
                            AttributeEnum::Actual, std::to_string(3), "test", true);

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = handler.validate_profile_schedules(profile, std::nullopt);

    auto numberPhases = profile.chargingSchedule[0].chargingSchedulePeriod[0].numberPhases;

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
    EXPECT_THAT(numberPhases, testing::Eq(3));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR06_ExistingProfileLastsForever_RejectIncoming) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID, ocpp::DateTime(date::utc_clock::time_point::min()),
                            ocpp::DateTime(date::utc_clock::time_point::max()));

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-02T13:00:00"),
        ocpp::DateTime("2024-03-01T13:00:00"));

    auto sut = handler.validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateProfileValidityPeriod));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR06_ExisitingProfileHasValidFromIncomingValidToOverlaps_RejectIncoming) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID, ocpp::DateTime("2024-01-01T13:00:00"),
                            ocpp::DateTime(date::utc_clock::time_point::max()));

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, {}, ocpp::DateTime("2024-01-01T13:00:00"));

    auto sut = handler.validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateProfileValidityPeriod));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR06_ExisitingProfileHasValidToIncomingValidFromOverlaps_RejectIncoming) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID, ocpp::DateTime("2024-02-01T13:00:00"),
                            ocpp::DateTime(date::utc_clock::time_point::max()));

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-31T13:00:00"), {});

    auto sut = handler.validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateProfileValidityPeriod));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR06_ExisitingProfileHasValidPeriodIncomingIsNowToMax_RejectIncoming) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID,
                            ocpp::DateTime(date::utc_clock::now() - std::chrono::hours(5 * 24)),
                            ocpp::DateTime(date::utc_clock::now() + std::chrono::hours(5 * 24)));

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, {}, {});

    auto sut = handler.validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateProfileValidityPeriod));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01FR06_ExisitingProfileHasValidPeriodIncomingOverlaps_RejectIncoming) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID, ocpp::DateTime("2024-01-01T13:00:00"),
                            ocpp::DateTime("2024-02-01T13:00:00"));

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-15T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));

    auto sut = handler.validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateProfileValidityPeriod));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01_ValidateProfile_IfEvseDoesNotExist_ThenProfileIsInvalid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID);

    auto sut = handler.validate_profile(profile, NR_OF_EVSES + 1);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::EvseDoesNotExist));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01_ValidateProfile_IfScheduleIsInvalid_ThenProfileIsInvalid) {
    auto extraneous_start_schedule = ocpp::DateTime("2024-01-17T17:00:00");
    auto periods = create_charging_schedule_periods(0);
    auto profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                create_charge_schedule(ChargingRateUnitEnum::A, periods, extraneous_start_schedule),
                                DEFAULT_TX_ID, ChargingProfileKindEnum::Relative, 1);

    auto sut = handler.validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileExtraneousStartSchedule));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01_ValidateProfile_IfChargeStationMaxProfileIsInvalid_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut = handler.validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingStationMaxProfileEvseIdGreaterThanZero));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01_ValidateProfile_IfDuplicateTxDefaultProfileFoundOnEVSE_IsInvalid_ThenProfileIsInvalid) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto periods = create_charging_schedule_periods(0);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), {},
                                           ChargingProfileKindEnum::Relative, DEFAULT_STACK_LEVEL);

    auto sut = handler.validate_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateTxDefaultProfileFound));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01_ValidateProfile_IfDuplicateTxDefaultProfileFoundOnChargingStation_IsInvalid_ThenProfileIsInvalid) {
    install_profile_on_evse(STATION_WIDE_ID, DEFAULT_PROFILE_ID);

    auto periods = create_charging_schedule_periods(0);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), {},
                                           ChargingProfileKindEnum::Relative, DEFAULT_STACK_LEVEL);

    auto sut = handler.validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateTxDefaultProfileFound));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01_ValidateProfile_IfTxProfileIsInvalid_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto sut = handler.validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileMissingTransactionId));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01_ValidateProfile_IfTxProfileIsValid_ThenProfileIsValid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);
    auto sut = handler.validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01_ValidateProfile_IfTxDefaultProfileIsValid_ThenProfileIsValid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto sut = handler.validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01_ValidateProfile_IfChargeStationMaxProfileIsValid_ThenProfileIsValid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut = handler.validate_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(
    SmartChargingHandlerTestFixtureV201,
    K01_ValidateProfile_IfExistingChargingProfileWithSameIdIsChargingStationExternalConstraints_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto external_constraints =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationExternalConstraints,
                                create_charge_schedule(ChargingRateUnitEnum::A), {});
    handler.add_profile(external_constraints, STATION_WIDE_ID);

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto sut = handler.validate_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ExistingChargingStationExternalConstraints));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR14_IfTxDefaultProfileWithSameStackLevelDoesNotExist_ThenApplyStationWideTxDefaultProfileToAllEvses) {

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                           ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto sut = handler.add_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(handler.get_profiles(), testing::Contains(profile));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR15_IfTxDefaultProfileWithSameStackLevelDoesNotExist_ThenApplyTxDefaultProfileToEvse) {

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                           ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto sut = handler.add_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(handler.get_profiles(), testing::Contains(profile));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR05_IfProfileWithSameIdExistsOnEVSEAndIsNotChargingStationExternalContraints_ThenProfileIsReplaced) {

    auto profile1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto profile2 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto sut1 = handler.add_profile(profile1, DEFAULT_EVSE_ID);
    auto sut2 = handler.add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = handler.get_profiles();

    EXPECT_THAT(sut1.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut2.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(profiles, testing::Contains(profile2));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile1)));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR05_IfProfileWithSameIdExistsOnChargingStationAndNewProfileIsOnEVSE_ThenProfileIsReplaced) {
    auto profile1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto profile2 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto sut1 = handler.add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = handler.add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = handler.get_profiles();

    EXPECT_THAT(sut1.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut2.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(profiles, testing::Contains(profile2));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile1)));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR05_IfProfileWithSameIdExistsOnAnyEVSEAndNewProfileIsOnEVSE_ThenProfileIsReplaced) {
    auto profile1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto profile2 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto sut1 = handler.add_profile(profile1, DEFAULT_EVSE_ID);
    auto sut2 = handler.add_profile(profile2, DEFAULT_EVSE_ID + 1);

    auto profiles = handler.get_profiles();

    EXPECT_THAT(sut1.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut2.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(profiles, testing::Contains(profile2));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile1)));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR05_IfProfileWithSameIdExistsOnAnyEVSEAndNewProfileIsOnChargingStation_ThenProfileIsReplaced) {
    auto profile1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto profile2 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto sut1 = handler.add_profile(profile1, DEFAULT_EVSE_ID + 1);
    auto sut2 = handler.add_profile(profile2, STATION_WIDE_ID);

    auto profiles = handler.get_profiles();

    EXPECT_THAT(sut1.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut2.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(profiles, testing::Contains(profile2));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile1)));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR05_IfProfileWithSameIdExistsOnChargingStationAndNewProfileIsOnChargingStation_ThenProfileIsReplaced) {
    auto profile1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto profile2 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Relative, DEFAULT_STACK_LEVEL);

    auto sut1 = handler.add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = handler.add_profile(profile2, STATION_WIDE_ID);

    auto profiles = handler.get_profiles();

    EXPECT_THAT(sut1.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut2.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(profiles, testing::Contains(profile2));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile1)));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K01FR05_ChargingStationWithMultipleProfilesAddProfileWithExistingProfileId_ThenProfileIsReplaced) {
    auto profile1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto profile2 = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Relative, DEFAULT_STACK_LEVEL);
    auto profile3 = create_charging_profile(DEFAULT_PROFILE_ID + 2, ChargingProfilePurposeEnum::TxProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Recurring, DEFAULT_STACK_LEVEL);

    auto sut1 = handler.add_profile(profile1, DEFAULT_EVSE_ID);
    auto sut2 = handler.add_profile(profile2, DEFAULT_EVSE_ID);
    auto sut3 = handler.add_profile(profile3, STATION_WIDE_ID);

    auto profiles = handler.get_profiles();

    EXPECT_THAT(sut1.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut2.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut3.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    EXPECT_THAT(profiles, testing::Contains(profile1));
    EXPECT_THAT(profiles, testing::Contains(profile2));
    EXPECT_THAT(profiles, testing::Contains(profile3));

    auto profile4 = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto profile5 = create_charging_profile(DEFAULT_PROFILE_ID + 2, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Recurring, DEFAULT_STACK_LEVEL);

    auto sut4 = handler.add_profile(profile4, STATION_WIDE_ID);
    auto sut5 = handler.add_profile(profile5, DEFAULT_EVSE_ID);

    profiles = handler.get_profiles();

    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile2)));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile3)));

    EXPECT_THAT(profiles, testing::Contains(profile4));
    EXPECT_THAT(profiles, testing::Contains(profile5));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K04FR01_AddProfile_OnlyAddsToOneEVSE) {
    auto profile1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto profile2 = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto response = handler.add_profile(profile1, DEFAULT_EVSE_ID);
    auto response2 = handler.add_profile(profile2, DEFAULT_EVSE_ID + 1);

    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(response2.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    auto sut1 = handler.get_profiles_on_evse(DEFAULT_EVSE_ID);
    auto sut2 = handler.get_profiles_on_evse(DEFAULT_EVSE_ID + 1);

    EXPECT_THAT(sut1, testing::Contains(profile1));
    EXPECT_THAT(sut1, testing::Not(testing::Contains(profile2)));

    EXPECT_THAT(sut2, testing::Contains(profile2));
    EXPECT_THAT(sut2, testing::Not(testing::Contains(profile1)));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01_ValidateAndAdd_RejectsInvalidProfilesWithReasonCode) {
    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut = handler.validate_and_add_profile(profile, DEFAULT_EVSE_ID);
    auto status_info = sut.statusInfo;
    EXPECT_THAT(sut.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_THAT(status_info->reasonCode.get(), testing::Eq(conversions::profile_validation_result_to_reason_code(
                                                   ProfileValidationResultEnum::TxProfileMissingTransactionId)));

    EXPECT_THAT(status_info->additionalInfo.has_value(), testing::IsTrue());
    EXPECT_THAT(status_info->additionalInfo->get(), testing::Eq(conversions::profile_validation_result_to_string(
                                                        ProfileValidationResultEnum::TxProfileMissingTransactionId)));

    auto profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile)));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K01_ValidateAndAdd_AddsValidProfiles) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = handler.validate_and_add_profile(profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(sut.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut.statusInfo.has_value(), testing::IsFalse());

    auto profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::Contains(profile));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K09_GetChargingProfiles_EvseId) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile1 = create_charging_profile(
        1, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto profile2 = create_charging_profile(
        2, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut1 = handler.validate_and_add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = handler.validate_and_add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::SizeIs(2));

    auto reported_profiles = handler.get_reported_profiles(
        create_get_charging_profile_request(DEFAULT_REQUEST_ID, create_charging_profile_criteria(), STATION_WIDE_ID));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));

    auto reported_profile = reported_profiles.at(0);
    EXPECT_THAT(profile1, testing::Eq(reported_profile.profile));

    reported_profiles = handler.get_reported_profiles(
        create_get_charging_profile_request(DEFAULT_REQUEST_ID, create_charging_profile_criteria(), DEFAULT_EVSE_ID));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));

    reported_profile = reported_profiles.at(0);
    EXPECT_THAT(profile2, testing::Eq(reported_profile.profile));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K09_GetChargingProfiles_NoEvseId) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile1 = create_charging_profile(
        1, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto profile2 = create_charging_profile(
        2, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut1 = handler.validate_and_add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = handler.validate_and_add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::SizeIs(2));

    auto reported_profiles = handler.get_reported_profiles(
        create_get_charging_profile_request(DEFAULT_REQUEST_ID, create_charging_profile_criteria()));
    EXPECT_THAT(reported_profiles, testing::SizeIs(2));

    EXPECT_THAT(profiles, testing::Contains(reported_profiles.at(0).profile));
    EXPECT_THAT(profiles, testing::Contains(reported_profiles.at(1).profile));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K09_GetChargingProfiles_ProfileId) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile1 = create_charging_profile(
        1, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto profile2 = create_charging_profile(
        2, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut1 = handler.validate_and_add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = handler.validate_and_add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::SizeIs(2));

    std::vector<int32_t> requested_profile_ids{1};
    auto reported_profiles = handler.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID, create_charging_profile_criteria(std::nullopt, requested_profile_ids)));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));

    EXPECT_THAT(profile1, testing::Eq(reported_profiles.at(0).profile));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K09_GetChargingProfiles_EvseIdAndStackLevel) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile1 = create_charging_profile(
        1, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), std::nullopt,
        ChargingProfileKindEnum::Absolute, 2); // contains different stackLevel(2)
    auto profile2 = create_charging_profile(
        2, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods,
                               ocpp::DateTime("2024-01-17T17:00:00"))); // contains default stackLevel(1)

    auto sut1 = handler.validate_and_add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = handler.validate_and_add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::SizeIs(2));

    auto reported_profiles = handler.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID,
        create_charging_profile_criteria(std::nullopt, std::nullopt, std::nullopt, DEFAULT_STACK_LEVEL),
        DEFAULT_EVSE_ID));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));
    EXPECT_THAT(profile2, testing::Eq(reported_profiles.at(0).profile));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K09_GetChargingProfiles_EvseIdAndSource) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut1 = handler.validate_and_add_profile(profile, DEFAULT_EVSE_ID);

    auto profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::SizeIs(1));

    std::vector<ChargingLimitSourceEnum> requested_sources_cso{ChargingLimitSourceEnum::CSO};
    std::vector<ChargingLimitSourceEnum> requested_sources_ems{ChargingLimitSourceEnum::EMS};

    auto reported_profiles = handler.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID, create_charging_profile_criteria(requested_sources_cso), DEFAULT_EVSE_ID));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));
    EXPECT_THAT(profile, testing::Eq(reported_profiles.at(0).profile));

    reported_profiles = handler.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID, create_charging_profile_criteria(requested_sources_ems), DEFAULT_EVSE_ID));

    EXPECT_THAT(reported_profiles, testing::SizeIs(0));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K09_GetChargingProfiles_EvseIdAndPurposeAndStackLevel) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile1 = create_charging_profile(
        1, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto profile2 = create_charging_profile(
        2, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut1 = handler.validate_and_add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = handler.validate_and_add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::SizeIs(2));

    auto reported_profiles = handler.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID,
        create_charging_profile_criteria(std::nullopt, std::nullopt, ChargingProfilePurposeEnum::TxDefaultProfile),
        DEFAULT_EVSE_ID));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));
    EXPECT_THAT(profile2, testing::Eq(reported_profiles.at(0).profile));

    reported_profiles = handler.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID,
        create_charging_profile_criteria(std::nullopt, std::nullopt, ChargingProfilePurposeEnum::TxDefaultProfile,
                                         DEFAULT_STACK_LEVEL),
        DEFAULT_EVSE_ID));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));
    EXPECT_THAT(profile2, testing::Eq(reported_profiles.at(0).profile));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K10_ClearChargingProfile_ClearsId) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    handler.validate_and_add_profile(profile, STATION_WIDE_ID);

    auto profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::Contains(profile));

    auto sut = handler.clear_profiles(create_clear_charging_profile_request(DEFAULT_PROFILE_ID));
    EXPECT_THAT(sut.status, testing::Eq(ClearChargingProfileStatusEnum::Accepted));

    profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile)));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K10_ClearChargingProfile_ClearsStackLevelPurposeCombination) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::Not(testing::IsEmpty()));

    auto sut = handler.clear_profiles(create_clear_charging_profile_request(
        std::nullopt, create_clear_charging_profile(std::nullopt, ChargingProfilePurposeEnum::TxDefaultProfile,
                                                    DEFAULT_STACK_LEVEL)));
    EXPECT_THAT(sut.status, testing::Eq(ClearChargingProfileStatusEnum::Accepted));

    profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::IsEmpty());
}

TEST_F(SmartChargingHandlerTestFixtureV201, K10_ClearChargingProfile_UnknownStackLevelPurposeCombination) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::Not(testing::IsEmpty()));

    auto sut = handler.clear_profiles(create_clear_charging_profile_request(
        std::nullopt, create_clear_charging_profile(std::nullopt, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                                    STATION_WIDE_ID)));
    EXPECT_THAT(sut.status, testing::Eq(ClearChargingProfileStatusEnum::Unknown));

    profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::Not(testing::IsEmpty()));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K10_ClearChargingProfile_UnknownId) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    handler.validate_and_add_profile(profile, STATION_WIDE_ID);

    auto profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::Contains(profile));

    auto sut = handler.clear_profiles(create_clear_charging_profile_request(178));
    EXPECT_THAT(sut.status, testing::Eq(ClearChargingProfileStatusEnum::Unknown));

    profiles = handler.get_profiles();
    EXPECT_THAT(profiles, testing::Contains(profile));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K08_GetValidProfiles_IfNoProfiles_ThenNoValidProfilesReturned) {
    auto profiles = handler.get_valid_profiles(DEFAULT_EVSE_ID);
    EXPECT_THAT(profiles, testing::IsEmpty());
}

TEST_F(SmartChargingHandlerTestFixtureV201, K08_GetValidProfiles_IfEvseHasProfiles_ThenThoseProfilesReturned) {
    auto profile = add_valid_profile_to(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);
    ASSERT_TRUE(profile.has_value());

    auto profiles = handler.get_valid_profiles(DEFAULT_EVSE_ID);
    EXPECT_THAT(profiles, testing::Contains(profile));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K08_GetValidProfiles_IfOtherEvseHasProfiles_ThenThoseProfilesAreNotReturned) {
    auto profile1 = add_valid_profile_to(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);
    ASSERT_TRUE(profile1.has_value());
    auto profile2 = add_valid_profile_to(DEFAULT_EVSE_ID + 1, DEFAULT_PROFILE_ID + 1);
    ASSERT_TRUE(profile2.has_value());

    auto profiles = handler.get_valid_profiles(DEFAULT_EVSE_ID);
    EXPECT_THAT(profiles, testing::Contains(profile1));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile2)));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K08_GetValidProfiles_IfStationWideProfilesExist_ThenThoseProfilesAreReturned) {
    auto profile = add_valid_profile_to(STATION_WIDE_ID, DEFAULT_PROFILE_ID);
    ASSERT_TRUE(profile.has_value());

    auto profiles = handler.get_valid_profiles(DEFAULT_EVSE_ID);
    EXPECT_THAT(profiles, testing::Contains(profile));
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K08_GetValidProfiles_IfStationWideProfilesExist_ThenThoseProfilesAreReturnedOnce) {
    auto profile = add_valid_profile_to(STATION_WIDE_ID, DEFAULT_PROFILE_ID);
    ASSERT_TRUE(profile.has_value());

    auto profiles = handler.get_valid_profiles(STATION_WIDE_ID);
    EXPECT_THAT(profiles, testing::Contains(profile));
    EXPECT_THAT(profiles.size(), testing::Eq(1));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K08_GetValidProfiles_IfInvalidProfileExists_ThenThatProfileIsNotReturned) {
    auto extraneous_start_schedule = ocpp::DateTime("2024-01-17T17:00:00");
    auto periods = create_charging_schedule_periods(0);
    auto invalid_profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                create_charge_schedule(ChargingRateUnitEnum::A, periods, extraneous_start_schedule),
                                DEFAULT_TX_ID, ChargingProfileKindEnum::Relative, 1);
    handler.add_profile(invalid_profile, DEFAULT_EVSE_ID);

    auto invalid_station_wide_profile =
        create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxProfile,
                                create_charge_schedule(ChargingRateUnitEnum::A, periods, extraneous_start_schedule),
                                DEFAULT_TX_ID, ChargingProfileKindEnum::Relative, 1);
    handler.add_profile(invalid_station_wide_profile, STATION_WIDE_ID);

    auto profiles = handler.get_valid_profiles(DEFAULT_EVSE_ID);
    EXPECT_THAT(profiles, testing::Not(testing::Contains(invalid_profile)));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(invalid_station_wide_profile)));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K02FR05_SmartChargingTransactionEnds_DeletesTxProfilesByTransactionId) {
    auto transaction_id = uuid();
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, transaction_id);
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A,
                                                                  create_charging_schedule_periods({0, 1, 2}),
                                                                  ocpp::DateTime("2024-01-17T17:00:00")),
                                           transaction_id);

    auto response = handler.validate_and_add_profile(profile, DEFAULT_EVSE_ID);
    ASSERT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    handler.delete_transaction_tx_profiles(transaction_id);

    EXPECT_THAT(handler.get_profiles(), testing::IsEmpty());
}

TEST_F(SmartChargingHandlerTestFixtureV201,
       K02FR05_DeleteTransactionTxProfiles_DoesNotDeleteTxProfilesWithDifferentTransactionId) {
    auto transaction_id = uuid();
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, transaction_id);
    auto other_transaction_id = uuid();
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, other_transaction_id);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A,
                                                                  create_charging_schedule_periods({0, 1, 2}),
                                                                  ocpp::DateTime("2024-01-17T17:00:00")),
                                           other_transaction_id);

    auto response = handler.validate_and_add_profile(profile, DEFAULT_EVSE_ID);
    ASSERT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    handler.delete_transaction_tx_profiles(transaction_id);

    EXPECT_THAT(handler.get_profiles().size(), testing::Eq(1));
}

TEST_F(SmartChargingHandlerTestFixtureV201, K05FR02_RequestStartTransactionRequest_ChargingProfileMustBeTxProfile) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A,
                                                                  create_charging_schedule_periods({0, 1, 2}),
                                                                  ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut =
        handler.validate_profile(profile, DEFAULT_EVSE_ID, AddChargingProfileSource::RequestStartTransactionRequest);
    ASSERT_THAT(sut, testing::Eq(ProfileValidationResultEnum::RequestStartTransactionNonTxProfile));
}

} // namespace ocpp::v201
