// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/functional_blocks/smart_charging.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <optional>

#include <date/tz.h>
#include <sqlite3.h>

#include <comparators.hpp>
#include <ocpp/common/call_types.hpp>
#include <ocpp/common/types.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/database_handler.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/evse.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

#include "component_state_manager_mock.hpp"
#include "connectivity_manager_mock.hpp"
#include "device_model_test_helper.hpp"
#include "evse_manager_fake.hpp"
#include "evse_mock.hpp"
#include "evse_security_mock.hpp"
#include "lib/ocpp/common/database_testing_utils.hpp"
#include "message_dispatcher_mock.hpp"
#include "smart_charging_test_utils.hpp"

using ::testing::_;
using ::testing::ByMove;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::Return;
using ::testing::ReturnRef;

namespace ocpp::v2 {
class SmartChargingTestV21 : public DatabaseTestingUtils {
protected:
    void SetUp() override {
        const auto& charging_rate_unit_cv = ControllerComponentVariables::ChargingScheduleChargingRateUnit;
        device_model->set_value(charging_rate_unit_cv.component, charging_rate_unit_cv.variable.value(),
                                AttributeEnum::Actual, "A,W", "test", true);

        const auto& ac_phase_switching_cv = ControllerComponentVariables::ACPhaseSwitchingSupported;
        device_model->set_value(ac_phase_switching_cv.component, ac_phase_switching_cv.variable.value(),
                                AttributeEnum::Actual, "true", "test", true);
    }

    void TearDown() override {
        // TODO: use in-memory db so we don't need to reset the db between tests
        this->database_handler->clear_charging_profiles();
    }

    TestSmartCharging create_smart_charging() {
        std::unique_ptr<everest::db::sqlite::Connection> database_connection =
            std::make_unique<everest::db::sqlite::Connection>(fs::path("/tmp/ocpp201") / "cp.db");
        database_handler =
            std::make_shared<DatabaseHandler>(std::move(database_connection), MIGRATION_FILES_LOCATION_V2);
        database_handler->open_connection();
        device_model = device_model_test_helper.get_device_model();
        this->functional_block_context = std::make_unique<FunctionalBlockContext>(
            this->mock_dispatcher, *this->device_model, this->connectivity_manager, *this->evse_manager,
            *this->database_handler, this->evse_security, this->component_state_manager, this->ocpp_version);
        return TestSmartCharging(*functional_block_context, set_charging_profiles_callback_mock.AsStdFunction(),
                                 stop_transaction_callback_mock.AsStdFunction());
    }

    // Default values used within the tests
    DeviceModelTestHelper device_model_test_helper;
    MockMessageDispatcher mock_dispatcher;
    std::unique_ptr<EvseManagerFake> evse_manager = std::make_unique<EvseManagerFake>(NR_OF_TWO_EVSES);
    std::shared_ptr<DatabaseHandler> database_handler;
    DeviceModel* device_model;
    ::testing::NiceMock<ConnectivityManagerMock> connectivity_manager;
    ocpp::EvseSecurityMock evse_security;
    ComponentStateManagerMock component_state_manager;
    MockFunction<void()> set_charging_profiles_callback_mock;
    MockFunction<RequestStartStopStatusEnum(const int32_t evse_id, const ReasonEnum& stop_reason)>
        stop_transaction_callback_mock;
    std::unique_ptr<FunctionalBlockContext> functional_block_context;
    TestSmartCharging smart_charging = create_smart_charging();
    std::atomic<OcppProtocolVersion> ocpp_version = OcppProtocolVersion::v21;
};

TEST_F(SmartChargingTestV21,
       K01FR44_IfPhaseToUseProvidedForDCChargingStationAndDCInputPhaseControlFalse_ThenProfileIsInvalid) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::DC));
    ON_CALL(mock_evse, get_id).WillByDefault(testing::Return(1));
    ComponentVariable c =
        EvseComponentVariables::get_component_variable(1, EvseComponentVariables::DCInputPhaseControl);
    device_model->set_value(c.component, c.variable.value(), AttributeEnum::Actual, "false", "test", true);

    auto periods = create_charging_schedule_periods(0, 1, 1, 5.0f);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodNoPhaseForDC));
}

TEST_F(SmartChargingTestV21,
       K01FR44_IfPhaseToUseProvidedForDCChargingStationAndDCInputPhaseControlTrue_ThenProfileIsValid) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::DC));
    ON_CALL(mock_evse, get_id).WillByDefault(testing::Return(1));
    ComponentVariable c =
        EvseComponentVariables::get_component_variable(1, EvseComponentVariables::DCInputPhaseControl);
    device_model->set_value(c.component, c.variable.value(), AttributeEnum::Actual, "true", "test", true);

    auto periods = create_charging_schedule_periods(0, 1, 1, 5.0f);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTestV21, K01FR55_AllChargingProfilesAreStored) {
    // In the default device model, all charging profiles must be set to persistent true, because we don't have a local
    // storage or in-memory storage for the charging profiles (everything is currently stored in the database).
    EXPECT_TRUE(device_model
                    ->get_optional_value<bool>(
                        ControllerComponentVariables::ChargingProfilePersistenceChargingStationExternalConstraints)
                    .value_or(false));
    EXPECT_TRUE(
        device_model->get_optional_value<bool>(ControllerComponentVariables::ChargingProfilePersistenceLocalGeneration)
            .value_or(false));
    EXPECT_TRUE(
        device_model->get_optional_value<bool>(ControllerComponentVariables::ChargingProfilePersistenceTxProfile)
            .value_or(false));
}

TEST_F(SmartChargingTestV21, K01FR56_NewChargingProfileStoredTooQuicklyAfterThePrevious) {
    const ComponentVariable update_rate_limit = ControllerComponentVariables::ChargingProfileUpdateRateLimit;
    device_model->set_value(update_rate_limit.component, update_rate_limit.variable.value(), AttributeEnum::Actual,
                            "5000", "test", true);

    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    auto existing_profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-01T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_EQ(response.statusInfo.value().additionalInfo, "ChargingProfileRateLimitExceeded");

    device_model->set_value(update_rate_limit.component, update_rate_limit.variable.value(), AttributeEnum::Actual, "0",
                            "test", true);

    response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
}

TEST_F(SmartChargingTestV21, K01FR70_PriorityChargingCanNotHaveValueForDuration) {
    const ComponentVariable supported_additional_purpose = ControllerComponentVariables::SupportedAdditionalPurposes;
    device_model->set_value(supported_additional_purpose.component, supported_additional_purpose.variable.value(),
                            AttributeEnum::Actual, "PriorityCharging", "test", true);
    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    auto existing_profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::PriorityCharging,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00"), 5000), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-01T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_EQ(response.statusInfo.value().reasonCode, "InvalidSchedule");
    EXPECT_EQ(response.statusInfo.value().additionalInfo, "ChargingSchedulePriorityExtranousDuration");
}

TEST_F(SmartChargingTestV21, K01FR71_PriorityChargingMustHaveOperationModeChargingOnly_Rejected) {
    const ComponentVariable supported_additional_purpose = ControllerComponentVariables::SupportedAdditionalPurposes;
    device_model->set_value(supported_additional_purpose.component, supported_additional_purpose.variable.value(),
                            AttributeEnum::Actual, "PriorityCharging", "test", true);
    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    ASSERT_GE(periods.size(), 1);
    periods.at(0).operationMode = OperationModeEnum::ExternalLimits;
    auto existing_profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::PriorityCharging,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-01T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_EQ(response.statusInfo.value().reasonCode, "InvalidSchedule");
    EXPECT_EQ(response.statusInfo.value().additionalInfo, "ChargingSchedulePeriodPriorityChargingNotChargingOnly");
}

TEST_F(SmartChargingTestV21, K01FR71_PriorityChargingMustHaveOperationModeChargingOnly_Accepted) {
    const ComponentVariable supported_additional_purpose = ControllerComponentVariables::SupportedAdditionalPurposes;
    device_model->set_value(supported_additional_purpose.component, supported_additional_purpose.variable.value(),
                            AttributeEnum::Actual, "PriorityCharging", "test", true);
    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    ASSERT_GE(periods.size(), 1);
    periods.at(0).operationMode = OperationModeEnum::ChargingOnly;
    auto existing_profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::PriorityCharging,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-01T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
}

TEST_F(SmartChargingTestV21, K01FR81_ChargingProfileIdIsBiggerThanMaxExternalConstraintsId_Rejected) {
    const ComponentVariable max_external_constraints_id = ControllerComponentVariables::MaxExternalConstraintsId;
    device_model->set_value(max_external_constraints_id.component, max_external_constraints_id.variable.value(),
                            AttributeEnum::Actual, "25", "test", true);
    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    auto existing_profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-01T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_EQ(response.statusInfo.value().reasonCode, "InvalidProfileId");
    EXPECT_EQ(response.statusInfo.value().additionalInfo, "ChargingProfileIdSmallerThanMaxExternalConstraintsId");
}

TEST_F(SmartChargingTestV21, K01FR81_ChargingProfileIdIsBiggerThanMaxExternalConstraintsId_Accepted) {
    const ComponentVariable max_external_constraints_id = ControllerComponentVariables::MaxExternalConstraintsId;
    device_model->set_value(max_external_constraints_id.component, max_external_constraints_id.variable.value(),
                            AttributeEnum::Actual, "25", "test", true);
    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    auto existing_profile = create_charging_profile(
        26, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-01T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
}

TEST_F(SmartChargingTestV21, K01FR120_PriorityChargingNotSupported) {
    const ComponentVariable supported_additional_purpose = ControllerComponentVariables::SupportedAdditionalPurposes;
    device_model->set_value(supported_additional_purpose.component, supported_additional_purpose.variable.value(),
                            AttributeEnum::Actual, "", "test", true);
    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    ASSERT_GE(periods.size(), 1);
    periods.at(0).operationMode = OperationModeEnum::ChargingOnly;
    auto existing_profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::PriorityCharging,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-01T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_EQ(response.statusInfo.value().reasonCode, "UnsupportedPurpose");
    EXPECT_EQ(response.statusInfo.value().additionalInfo, "ChargingProfileUnsupportedPurpose");
}

TEST_F(SmartChargingTestV21, K01FR120_LocalGenerationNotSupported) {
    const ComponentVariable supported_additional_purpose = ControllerComponentVariables::SupportedAdditionalPurposes;
    device_model->set_value(supported_additional_purpose.component, supported_additional_purpose.variable.value(),
                            AttributeEnum::Actual, "", "test", true);
    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    ASSERT_GE(periods.size(), 1);
    auto existing_profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::LocalGeneration,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-01T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_EQ(response.statusInfo.value().reasonCode, "UnsupportedPurpose");
    EXPECT_EQ(response.statusInfo.value().additionalInfo, "ChargingProfileUnsupportedPurpose");
}

TEST_F(SmartChargingTestV21, K01FR121_DynamicProfilesNotSupported) {
    const ComponentVariable supports_dynamic_profiles = ControllerComponentVariables::SupportsDynamicProfiles;
    device_model->set_value(supports_dynamic_profiles.component, supports_dynamic_profiles.variable.value(),
                            AttributeEnum::Actual, "false", "test", true);
    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    ASSERT_GE(periods.size(), 1);
    auto existing_profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Dynamic, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-01T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_EQ(response.statusInfo.value().reasonCode, "UnsupportedKind");
    EXPECT_EQ(response.statusInfo.value().additionalInfo, "ChargingProfileUnsupportedKind");
}

TEST_F(SmartChargingTestV21, K01FR122_DynUpdateIntervalSetWhileProfileIsNotDynamic) {
    const ComponentVariable supports_dynamic_profiles = ControllerComponentVariables::SupportsDynamicProfiles;
    device_model->set_value(supports_dynamic_profiles.component, supports_dynamic_profiles.variable.value(),
                            AttributeEnum::Actual, "true", "test", true);
    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    ASSERT_GE(periods.size(), 1);
    auto existing_profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-01T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));
    existing_profile.dynUpdateInterval = 20;

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_EQ(response.statusInfo.value().reasonCode, "InvalidProfile");
    EXPECT_EQ(response.statusInfo.value().additionalInfo, "ChargingProfileNotDynamic");
}

TEST_F(SmartChargingTestV21, K01FR123_LocalTimeNotSupported) {
    // Local time is default not supported, so we don't set the value in the device model here.
    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    ASSERT_GE(periods.size(), 1);
    auto charging_schedule =
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00"));
    charging_schedule.useLocalTime = true;
    auto existing_profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile, charging_schedule, {},
                                ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL,
                                ocpp::DateTime("2024-01-01T13:00:00"), ocpp::DateTime("2024-02-01T13:00:00"));

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_EQ(response.statusInfo.value().reasonCode, "InvalidSchedule");
    EXPECT_EQ(response.statusInfo.value().additionalInfo, "ChargingScheduleUnsupportedLocalTime");
}

TEST_F(SmartChargingTestV21, K01FR124_RandomizedDelayNotSupported) {
    // Randomized Delay is default not supported, so we don't set the value in the device model here.
    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    ASSERT_GE(periods.size(), 1);
    auto charging_schedule =
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00"));
    charging_schedule.randomizedDelay = 10;
    auto existing_profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile, charging_schedule, {},
                                ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL,
                                ocpp::DateTime("2024-01-01T13:00:00"), ocpp::DateTime("2024-02-01T13:00:00"));

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_EQ(response.statusInfo.value().reasonCode, "InvalidSchedule");
    EXPECT_EQ(response.statusInfo.value().additionalInfo, "ChargingScheduleUnsupportedRandomizedDelay");
}

TEST_F(SmartChargingTestV21, K01FR125_LimitAtSoCNotSupported) {
    // LimitAtSoc is default not supported, so we don't set the value in the device model here.
    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    ASSERT_GE(periods.size(), 1);
    auto charging_schedule =
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00"));
    charging_schedule.limitAtSoC = {1, 22.0f, std::nullopt};
    auto existing_profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile, charging_schedule, {},
                                ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL,
                                ocpp::DateTime("2024-01-01T13:00:00"), ocpp::DateTime("2024-02-01T13:00:00"));

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_EQ(response.statusInfo.value().reasonCode, "InvalidSchedule");
    EXPECT_EQ(response.statusInfo.value().additionalInfo, "ChargingScheduleUnsupportedLimitAtSoC");
}

TEST_F(SmartChargingTestV21, K01FR126_EvseSleepNotSupported) {
    // EvseSleep is default not supported, so we don't set the value in the device model here.
    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    ASSERT_GE(periods.size(), 1);
    periods.at(0).evseSleep = true;
    auto charging_schedule =
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00"));
    auto existing_profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile, charging_schedule, {},
                                ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL,
                                ocpp::DateTime("2024-01-01T13:00:00"), ocpp::DateTime("2024-02-01T13:00:00"));

    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_EQ(response.statusInfo.value().reasonCode, "InvalidSchedule");
    EXPECT_EQ(response.statusInfo.value().additionalInfo, "ChargingScheduleUnsupportedEvseSleep");
}

// Test for Table 95. operationMode for various ChargingProfilePurposes
class OperationModesForChargingProfileV21_Param_Test
    : public ::testing::WithParamInterface<std::tuple<ChargingProfilePurposeEnum, OperationModeEnum, bool>>,
      public SmartChargingTestV21 {};

INSTANTIATE_TEST_SUITE_P(
    OperationModesForChargingProfile_Instantiate, OperationModesForChargingProfileV21_Param_Test,
    testing::Values(
        std::make_tuple(ChargingProfilePurposeEnum::TxProfile, OperationModeEnum::ChargingOnly, true),
        std::make_tuple(ChargingProfilePurposeEnum::TxProfile, OperationModeEnum::ExternalSetpoint, true),
        std::make_tuple(ChargingProfilePurposeEnum::TxProfile, OperationModeEnum::ExternalLimits, true),
        std::make_tuple(ChargingProfilePurposeEnum::TxProfile, OperationModeEnum::CentralFrequency, true),
        std::make_tuple(ChargingProfilePurposeEnum::TxProfile, OperationModeEnum::LocalFrequency, true),
        std::make_tuple(ChargingProfilePurposeEnum::TxProfile, OperationModeEnum::LocalLoadBalancing, true),
        std::make_tuple(ChargingProfilePurposeEnum::TxProfile, OperationModeEnum::Idle, true),
        std::make_tuple(ChargingProfilePurposeEnum::TxDefaultProfile, OperationModeEnum::ChargingOnly, true),
        std::make_tuple(ChargingProfilePurposeEnum::TxDefaultProfile, OperationModeEnum::ExternalSetpoint, true),
        std::make_tuple(ChargingProfilePurposeEnum::TxDefaultProfile, OperationModeEnum::ExternalLimits, true),
        std::make_tuple(ChargingProfilePurposeEnum::TxDefaultProfile, OperationModeEnum::CentralFrequency, true),
        std::make_tuple(ChargingProfilePurposeEnum::TxDefaultProfile, OperationModeEnum::LocalFrequency, true),
        std::make_tuple(ChargingProfilePurposeEnum::TxDefaultProfile, OperationModeEnum::LocalLoadBalancing, true),
        std::make_tuple(ChargingProfilePurposeEnum::TxDefaultProfile, OperationModeEnum::Idle, true),
        std::make_tuple(ChargingProfilePurposeEnum::PriorityCharging, OperationModeEnum::ChargingOnly, true),
        std::make_tuple(ChargingProfilePurposeEnum::PriorityCharging, OperationModeEnum::ExternalSetpoint, false),
        std::make_tuple(ChargingProfilePurposeEnum::PriorityCharging, OperationModeEnum::ExternalLimits, false),
        std::make_tuple(ChargingProfilePurposeEnum::PriorityCharging, OperationModeEnum::CentralFrequency, false),
        std::make_tuple(ChargingProfilePurposeEnum::PriorityCharging, OperationModeEnum::LocalFrequency, false),
        std::make_tuple(ChargingProfilePurposeEnum::PriorityCharging, OperationModeEnum::LocalLoadBalancing, false),
        std::make_tuple(ChargingProfilePurposeEnum::PriorityCharging, OperationModeEnum::Idle, false),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationMaxProfile, OperationModeEnum::ChargingOnly, true),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationMaxProfile, OperationModeEnum::ExternalSetpoint,
                        false),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationMaxProfile, OperationModeEnum::ExternalLimits,
                        false),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationMaxProfile, OperationModeEnum::CentralFrequency,
                        false),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationMaxProfile, OperationModeEnum::LocalFrequency,
                        false),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationMaxProfile, OperationModeEnum::LocalLoadBalancing,
                        false),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationMaxProfile, OperationModeEnum::Idle, false),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationExternalConstraints, OperationModeEnum::ChargingOnly,
                        true),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationExternalConstraints,
                        OperationModeEnum::ExternalSetpoint, true),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationExternalConstraints,
                        OperationModeEnum::ExternalLimits, true),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationExternalConstraints,
                        OperationModeEnum::CentralFrequency, false),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationExternalConstraints,
                        OperationModeEnum::LocalFrequency, false),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationExternalConstraints,
                        OperationModeEnum::LocalLoadBalancing, false),
        std::make_tuple(ChargingProfilePurposeEnum::ChargingStationExternalConstraints, OperationModeEnum::Idle, false),
        std::make_tuple(ChargingProfilePurposeEnum::LocalGeneration, OperationModeEnum::ChargingOnly, true),
        std::make_tuple(ChargingProfilePurposeEnum::LocalGeneration, OperationModeEnum::ExternalSetpoint, false),
        std::make_tuple(ChargingProfilePurposeEnum::LocalGeneration, OperationModeEnum::ExternalLimits, true),
        std::make_tuple(ChargingProfilePurposeEnum::LocalGeneration, OperationModeEnum::CentralFrequency, false),
        std::make_tuple(ChargingProfilePurposeEnum::LocalGeneration, OperationModeEnum::LocalFrequency, false),
        std::make_tuple(ChargingProfilePurposeEnum::LocalGeneration, OperationModeEnum::LocalLoadBalancing, false),
        std::make_tuple(ChargingProfilePurposeEnum::LocalGeneration, OperationModeEnum::Idle, false)));

TEST_P(OperationModesForChargingProfileV21_Param_Test, SupportedOperationModesForChargingProfile) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    const ComponentVariable supported_additional_purpose = ControllerComponentVariables::SupportedAdditionalPurposes;
    device_model->set_value(supported_additional_purpose.component, supported_additional_purpose.variable.value(),
                            AttributeEnum::Actual, "PriorityCharging,LocalGeneration", "test", true);
    ON_CALL(mock_evse, get_id).WillByDefault(testing::Return(1));

    auto periods = create_charging_schedule_periods(0, 1, 1, 0.5f);
    ASSERT_GE(periods.size(), 1);
    periods.at(0).operationMode = std::get<1>(GetParam());
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, std::get<0>(GetParam()),
        create_charge_schedule(ChargingRateUnitEnum::W, periods, ocpp::DateTime("2024-01-17T17:00:00")),
        (std::get<0>(GetParam()) == ChargingProfilePurposeEnum::TxProfile ? std::optional<std::string>{DEFAULT_TX_ID}
                                                                          : std::nullopt));

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    if (std::get<2>(GetParam())) {
        // We can not make a valid profile for each of the purposes and operation modes, so we only check if the
        // validation result is something else than an unsupported operation mode.
        EXPECT_NE(sut, ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedOperationMode);
    } else {
        if (std::get<0>(GetParam()) == ChargingProfilePurposeEnum::PriorityCharging) {
            EXPECT_EQ(sut, ProfileValidationResultEnum::ChargingSchedulePeriodPriorityChargingNotChargingOnly);
        } else {
            EXPECT_EQ(sut, ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedOperationMode);
        }
    }
}

struct LimitAndSetpointsForOperatonModeV21 {
    OperationModeEnum operation_mode;
    bool set_limit;
    bool set_discharge_limit;
    bool set_setpoint;
    bool set_setpoint_reactive;
    bool valid;
};

// Test for Table 95. operationMode for various ChargingProfilePurposes
class LimitsAndSetpointsForOperationModeV21_Param_Test
    : public ::testing::WithParamInterface<std::tuple<OperationModeEnum, bool, bool, bool, bool, bool>>,
      public SmartChargingTestV21 {};

INSTANTIATE_TEST_SUITE_P(
    LimitsAndSetpointsForOperationModeV21_Instantiate, LimitsAndSetpointsForOperationModeV21_Param_Test,
    testing::Values(std::make_tuple(OperationModeEnum::ChargingOnly, true, true, true, true, false),
                    std::make_tuple(OperationModeEnum::ChargingOnly, true, false, false, false, true),
                    std::make_tuple(OperationModeEnum::ChargingOnly, true, false, true, false, false),
                    std::make_tuple(OperationModeEnum::ChargingOnly, false, false, false, false, false),
                    std::make_tuple(OperationModeEnum::CentralSetpoint, true, true, true, true, true),
                    std::make_tuple(OperationModeEnum::CentralSetpoint, true, false, false, false, false),
                    std::make_tuple(OperationModeEnum::CentralSetpoint, true, false, true, false, true),
                    std::make_tuple(OperationModeEnum::CentralSetpoint, false, false, false, false, false),
                    std::make_tuple(OperationModeEnum::CentralSetpoint, false, false, true, false, true),
                    std::make_tuple(OperationModeEnum::CentralFrequency, true, true, true, true, false),
                    std::make_tuple(OperationModeEnum::CentralFrequency, true, false, false, false, false),
                    std::make_tuple(OperationModeEnum::CentralFrequency, true, false, true, false, true),
                    std::make_tuple(OperationModeEnum::CentralFrequency, false, false, false, false, false),
                    std::make_tuple(OperationModeEnum::CentralFrequency, false, false, true, false, true),
                    std::make_tuple(OperationModeEnum::LocalFrequency, true, true, true, true, false),
                    std::make_tuple(OperationModeEnum::LocalFrequency, true, false, false, false, false),
                    std::make_tuple(OperationModeEnum::LocalFrequency, true, false, true, false, false),
                    std::make_tuple(OperationModeEnum::LocalFrequency, false, false, false, false, true),
                    std::make_tuple(OperationModeEnum::LocalFrequency, false, false, true, false, false),
                    std::make_tuple(OperationModeEnum::ExternalSetpoint, true, true, true, true, false),
                    std::make_tuple(OperationModeEnum::ExternalSetpoint, true, false, false, false, true),
                    std::make_tuple(OperationModeEnum::ExternalSetpoint, true, false, true, false, false),
                    std::make_tuple(OperationModeEnum::ExternalSetpoint, false, false, false, false, true),
                    std::make_tuple(OperationModeEnum::ExternalSetpoint, false, false, true, false, false),
                    std::make_tuple(OperationModeEnum::ExternalLimits, true, true, true, true, false),
                    std::make_tuple(OperationModeEnum::ExternalLimits, true, false, false, false, false),
                    std::make_tuple(OperationModeEnum::ExternalLimits, true, false, true, false, false),
                    std::make_tuple(OperationModeEnum::ExternalLimits, false, false, false, false, true),
                    std::make_tuple(OperationModeEnum::ExternalLimits, false, false, true, false, false),
                    std::make_tuple(OperationModeEnum::LocalLoadBalancing, true, true, true, true, false),
                    std::make_tuple(OperationModeEnum::LocalLoadBalancing, true, false, false, false, false),
                    std::make_tuple(OperationModeEnum::LocalLoadBalancing, true, false, true, false, false),
                    std::make_tuple(OperationModeEnum::LocalLoadBalancing, false, false, false, false, true),
                    std::make_tuple(OperationModeEnum::LocalLoadBalancing, false, false, true, false, false)));

TEST_P(LimitsAndSetpointsForOperationModeV21_Param_Test, Q08FR04_LimitsAndSetpoints) {
    LimitAndSetpointsForOperatonModeV21 test_data = {std::get<0>(GetParam()), std::get<1>(GetParam()),
                                                     std::get<2>(GetParam()), std::get<3>(GetParam()),
                                                     std::get<4>(GetParam()), std::get<5>(GetParam())};
    const ComponentVariable supported_additional_purpose = ControllerComponentVariables::SupportedAdditionalPurposes;
    device_model->set_value(supported_additional_purpose.component, supported_additional_purpose.variable.value(),
                            AttributeEnum::Actual, "PriorityCharging,LocalGeneration", "test", true);
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_id).WillByDefault(testing::Return(1));
    auto periods = create_charging_schedule_periods(0, 1, 1);
    ASSERT_GE(periods.size(), 1);
    auto& period = periods.at(0);
    period.operationMode = test_data.operation_mode;

    if (test_data.set_limit) {
        period.limit = 0.6f;
    }

    if (test_data.set_discharge_limit) {
        period.dischargeLimit = -3.3f;
    }

    if (test_data.set_setpoint) {
        period.setpoint = 4.2f;
    }

    if (test_data.set_setpoint_reactive) {
        period.setpointReactive = 2.2f;
    }

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::W, periods, ocpp::DateTime("2024-01-17T17:00:00")), {});

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    if (test_data.valid) {
        EXPECT_NE(sut, ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedLimitSetpoint);
    } else {
        EXPECT_EQ(sut, ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedLimitSetpoint);
    }
}

TEST_F(SmartChargingTestV21, Q08FR02_LocalFrequency_ChargingRateUnitW_NoFreqWattCurve) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_id).WillByDefault(testing::Return(1));

    auto periods = create_charging_schedule_periods(0, 1, 1);
    ASSERT_GE(periods.size(), 1);
    periods.at(0).operationMode = OperationModeEnum::LocalFrequency;
    periods.at(0).v2xBaseline = 6.6f;
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::W, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodNoFreqWattCurve));
}

TEST_F(SmartChargingTestV21, Q08FR02_LocalFrequency_ChargingRateUnitW_TooSmallFreqWattCurve) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_id).WillByDefault(testing::Return(1));

    auto periods = create_charging_schedule_periods(0, 1, 1);
    ASSERT_GE(periods.size(), 1);
    periods.at(0).operationMode = OperationModeEnum::LocalFrequency;
    periods.at(0).v2xFreqWattCurve = {{0.2f, 1.0f, std::nullopt}};
    periods.at(0).v2xBaseline = 6.6f;
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::W, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodNoFreqWattCurve));
}

TEST_F(SmartChargingTestV21, Q08FR02_LocalFrequency_ChargingRateUnitW_NoV2xBaseLine) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_id).WillByDefault(testing::Return(1));

    auto periods = create_charging_schedule_periods(0, 1, 1);
    ASSERT_GE(periods.size(), 1);
    periods.at(0).operationMode = OperationModeEnum::LocalFrequency;
    periods.at(0).v2xFreqWattCurve = {{0.2f, 1.0f, std::nullopt}, {3.4f, 4.2f, std::nullopt}};
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::W, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodNoFreqWattCurve));
}

TEST_F(SmartChargingTestV21, Q08FR05_LocalFrequency_ChargingRateUnitA_Invalid) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_id).WillByDefault(testing::Return(1));

    auto periods = create_charging_schedule_periods(0, 1, 1, 5.0f);
    ASSERT_GE(periods.size(), 1);
    periods.at(0).operationMode = OperationModeEnum::LocalFrequency;
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingScheduleChargingRateUnitUnsupported));
}

TEST_F(SmartChargingTestV21, Q08FR05_LocalFrequency_ChargingRateUnitW_Valid) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_id).WillByDefault(testing::Return(1));

    auto periods = create_charging_schedule_periods(0, 1, 1);
    ASSERT_GE(periods.size(), 1);
    periods.at(0).operationMode = OperationModeEnum::LocalFrequency;
    periods.at(0).v2xFreqWattCurve = {{0.2f, 1.0f, std::nullopt}, {3.4f, 4.2f, std::nullopt}};
    periods.at(0).v2xBaseline = 6.6f;
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::W, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTestV21, AllSetpointsSameSign) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_id).WillByDefault(testing::Return(1));

    // First test with different combinations of charging schedule periods positive and negative.
    auto periods = create_charging_schedule_periods(0, 1, 1, 42.0f);
    periods.at(0).operationMode = OperationModeEnum::CentralSetpoint;
    periods.at(0).setpoint = 5.5f;
    periods.at(0).setpoint_L2 = -5.5f;

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::W, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_EQ(sut, ProfileValidationResultEnum::ChargingSchedulePeriodSignDifference);

    periods.at(0).setpoint = 5.5f;
    periods.at(0).setpoint_L2 = 5.5f;
    periods.at(0).setpoint_L3 = -1.0f;

    profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::W, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_EQ(sut, ProfileValidationResultEnum::ChargingSchedulePeriodSignDifference);

    periods.at(0).setpoint = 5.5f;
    periods.at(0).setpoint_L2 = -5.5f;
    periods.at(0).setpoint_L3 = -1.0f;

    profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::W, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_EQ(sut, ProfileValidationResultEnum::ChargingSchedulePeriodSignDifference);

    periods.at(0).setpoint = -5.5f;
    periods.at(0).setpoint_L2 = 5.5f;
    periods.at(0).setpoint_L3 = -1.0f;

    profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::W, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_EQ(sut, ProfileValidationResultEnum::ChargingSchedulePeriodSignDifference);

    periods.at(0).setpoint = -5.5f;
    periods.at(0).setpoint_L3 = 5.5f;

    profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::W, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_EQ(sut, ProfileValidationResultEnum::ChargingSchedulePeriodSignDifference);

    // Then test with all setpoints having the same sign.
    periods.at(0).setpoint = -5.5f;
    periods.at(0).setpoint_L2 = -5.5f;
    periods.at(0).setpoint_L3 = -1.0f;

    profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::W, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_EQ(sut, ProfileValidationResultEnum::Valid);

    periods.at(0).setpoint = 5.5f;
    periods.at(0).setpoint_L2 = 5.5f;
    periods.at(0).setpoint_L3 = 1.0f;

    profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::W, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_EQ(sut, ProfileValidationResultEnum::Valid);
}
} // namespace ocpp::v2
