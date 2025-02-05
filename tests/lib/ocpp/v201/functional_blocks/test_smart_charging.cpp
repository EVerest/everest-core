// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include "connectivity_manager_mock.hpp"
#include "date/tz.h"
#include "everest/logging.hpp"
#include "lib/ocpp/common/database_testing_utils.hpp"
#include "message_dispatcher_mock.hpp"
#include "ocpp/common/types.hpp"
#include "ocpp/v201/ctrlr_component_variables.hpp"
#include "ocpp/v201/device_model.hpp"
#include "ocpp/v201/functional_blocks/smart_charging.hpp"
#include "ocpp/v201/init_device_model_db.hpp"
#include "ocpp/v201/ocpp_types.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <sqlite3.h>

#include <device_model_test_helper.hpp>

#include <component_state_manager_mock.hpp>
#include <device_model_storage_interface_mock.hpp>
#include <evse_manager_fake.hpp>
#include <evse_mock.hpp>
#include <evse_security_mock.hpp>
#include <ocpp/common/call_types.hpp>
#include <ocpp/v201/evse.hpp>
#include <ocpp/v201/ocpp_enums.hpp>
#include <optional>

#include "comparators.hpp"
#include "smart_charging_test_utils.hpp"
#include <sstream>
#include <vector>

using ::testing::_;
using ::testing::ByMove;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::Return;
using ::testing::ReturnRef;

namespace ocpp::v201 {

static const int NR_OF_EVSES = 2;
static const int STATION_WIDE_ID = 0;
static const int DEFAULT_EVSE_ID = 1;
static const int DEFAULT_PROFILE_ID = 1;
static const int DEFAULT_STACK_LEVEL = 1;
static const int DEFAULT_REQUEST_ID = 1;
static const std::string DEFAULT_TX_ID = "10c75ff7-74f5-44f5-9d01-f649f3ac7b78";
const static std::string MIGRATION_FILES_PATH = "./resources/v201/device_model_migration_files";
const static std::string CONFIG_PATH = "./resources/example_config/v201/component_config";
const static std::string DEVICE_MODEL_DB_IN_MEMORY_PATH = "file::memory:?cache=shared";

class TestSmartCharging : public SmartCharging {
public:
    using SmartCharging::add_profile;
    using SmartCharging::clear_profiles;
    using SmartCharging::get_reported_profiles;
    using SmartCharging::get_valid_profiles;
    using SmartCharging::validate_charging_station_max_profile;
    using SmartCharging::validate_evse_exists;
    using SmartCharging::validate_profile_schedules;
    using SmartCharging::validate_tx_default_profile;
    using SmartCharging::validate_tx_profile;
    using SmartCharging::verify_no_conflicting_external_constraints_id;

    using SmartCharging::SmartCharging;
};

class SmartChargingTest : public DatabaseTestingUtils {
protected:
    void SetUp() override {
    }

    void TearDown() override {
        // TODO: use in-memory db so we don't need to reset the db between tests
        this->database_handler->clear_charging_profiles();
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
        ClearChargingProfile clear_charging_profile;
        clear_charging_profile.customData = {};
        clear_charging_profile.evseId = evse_id;
        clear_charging_profile.chargingProfilePurpose = purpose;
        clear_charging_profile.stackLevel = stack_level;
        return clear_charging_profile;
    }

    template <class T> void call_to_json(json& j, const ocpp::Call<T>& call) {
        j = json::array();
        j.push_back(ocpp::MessageTypeId::CALL);
        j.push_back(call.uniqueId.get());
        j.push_back(call.msg.get_type());
        j.push_back(json(call.msg));
    }

    template <class T, MessageType M> ocpp::EnhancedMessage<MessageType> request_to_enhanced_message(const T& req) {
        auto message_id = uuid();
        ocpp::Call<T> call(req);
        ocpp::EnhancedMessage<MessageType> enhanced_message;
        enhanced_message.uniqueId = message_id;
        enhanced_message.messageType = M;
        enhanced_message.messageTypeId = ocpp::MessageTypeId::CALL;

        call_to_json(enhanced_message.message, call);

        return enhanced_message;
    }

    TestSmartCharging create_smart_charging(const std::optional<std::string> ac_phase_switching_supported = "true") {
        std::unique_ptr<common::DatabaseConnection> database_connection =
            std::make_unique<common::DatabaseConnection>(fs::path("/tmp/ocpp201") / "cp.db");
        database_handler =
            std::make_shared<DatabaseHandler>(std::move(database_connection), MIGRATION_FILES_LOCATION_V201);
        database_handler->open_connection();
        device_model = device_model_test_helper.get_device_model();
        // Defaults
        const auto& charging_rate_unit_cv = ControllerComponentVariables::ChargingScheduleChargingRateUnit;
        device_model->set_value(charging_rate_unit_cv.component, charging_rate_unit_cv.variable.value(),
                                AttributeEnum::Actual, "A,W", "test", true);

        const auto& ac_phase_switching_cv = ControllerComponentVariables::ACPhaseSwitchingSupported;
        device_model->set_value(ac_phase_switching_cv.component, ac_phase_switching_cv.variable.value(),
                                AttributeEnum::Actual, ac_phase_switching_supported.value_or(""), "test", true);
        return TestSmartCharging(*device_model, *this->evse_manager, connectivity_manager, mock_dispatcher,
                                 *database_handler, set_charging_profiles_callback_mock.AsStdFunction());
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
        smart_charging.add_profile(existing_profile, evse_id);
    }

    std::optional<ChargingProfile> add_valid_profile_to(int evse_id, int profile_id) {
        auto periods = create_charging_schedule_periods({0, 1, 2});
        auto profile = create_charging_profile(
            profile_id, ChargingProfilePurposeEnum::TxDefaultProfile,
            create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
        auto response = smart_charging.conform_validate_and_add_profile(profile, evse_id);
        if (response.status == ChargingProfileStatusEnum::Accepted) {
            return profile;
        } else {
            return {};
        }
    }

    // Default values used within the tests
    DeviceModelTestHelper device_model_test_helper;
    MockMessageDispatcher mock_dispatcher;
    std::unique_ptr<EvseManagerFake> evse_manager = std::make_unique<EvseManagerFake>(NR_OF_EVSES);

    sqlite3* db_handle;
    std::shared_ptr<DatabaseHandler> database_handler;

    bool ignore_no_transaction = true;
    DeviceModel* device_model;
    ::testing::NiceMock<ConnectivityManagerMock> connectivity_manager;
    MockFunction<void()> set_charging_profiles_callback_mock;
    TestSmartCharging smart_charging = create_smart_charging();
    boost::uuids::random_generator uuid_generator = boost::uuids::random_generator();
};

TEST_F(SmartChargingTest, K01FR03_IfTxProfileIsMissingTransactionId_ThenProfileIsInvalid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {});
    auto sut = smart_charging.validate_tx_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileMissingTransactionId));
}

TEST_F(SmartChargingTest,
       K01FR05_IfExistingChargingProfileWithSameIdIsChargingStationExternalConstraints_ThenProfileIsInvalid) {
    auto external_constraints =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationExternalConstraints,
                                create_charge_schedule(ChargingRateUnitEnum::A), {});
    smart_charging.add_profile(external_constraints, STATION_WIDE_ID);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {});
    auto sut = smart_charging.verify_no_conflicting_external_constraints_id(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ExistingChargingStationExternalConstraints));
}

TEST_F(SmartChargingTest, K01FR16_IfTxProfileHasEvseIdNotGreaterThanZero_ThenProfileIsInvalid) {
    auto wrong_evse_id = STATION_WIDE_ID;
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID);
    auto sut = smart_charging.validate_tx_profile(profile, wrong_evse_id);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileEvseIdNotGreaterThanZero));
}

TEST_F(SmartChargingTest, K01FR33_IfTxProfileTransactionIsNotOnEvse_ThenProfileIsInvalid) {
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, "wrong transaction id");
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID);
    auto sut = smart_charging.validate_tx_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileTransactionNotOnEvse));
}

TEST_F(SmartChargingTest, K01FR09_IfTxProfileEvseHasNoActiveTransaction_ThenProfileIsInvalid) {
    auto connector_id = 1;
    auto meter_start = MeterValue();
    auto id_token = IdToken();
    auto date_time = ocpp::DateTime("2024-01-17T17:00:00");
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID);
    auto sut = smart_charging.validate_tx_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileEvseHasNoActiveTransaction));
}

TEST_F(SmartChargingTest, K01FR19AndFR48_NumberPhasesOtherThan1AndPhaseToUseSet_ThenProfileInvalid) {
    auto periods = create_charging_schedule_periods_with_phases(0, 0, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID,
        ChargingProfileKindEnum::Absolute, 1);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodInvalidPhaseToUse));
}

TEST_F(SmartChargingTest, K01FR20AndFR48_IfPhaseToUseSetAndACPhaseSwitchingSupportedUndefined_ThenProfileIsInvalid) {
    // As a device model with ac switching supported default set to 'true', we want to have the
    // ac switching support not set.
    const auto& ac_phase_switching_cv = ControllerComponentVariables::ACPhaseSwitchingSupported;
    device_model_test_helper.set_variable_attribute_value_null(
        ac_phase_switching_cv.component.name, std::nullopt, std::nullopt, std::nullopt,
        ac_phase_switching_cv.variable->name, std::nullopt, AttributeEnum::Actual);

    auto periods = create_charging_schedule_periods_with_phases(0, 1, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID,
        ChargingProfileKindEnum::Absolute, 1);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut,
                testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodPhaseToUseACPhaseSwitchingUnsupported));
}

TEST_F(SmartChargingTest, K01FR20AndFR48_IfPhaseToUseSetAndACPhaseSwitchingSupportedFalse_ThenProfileIsInvalid) {
    // As a device model with ac switching supported default set to 'true', we want to have the ac switching support not
    // set.
    const auto& ac_phase_switching_cv = ControllerComponentVariables::ACPhaseSwitchingSupported;
    device_model_test_helper.set_variable_attribute_value_null(
        ac_phase_switching_cv.component.name, std::nullopt, std::nullopt, std::nullopt,
        ac_phase_switching_cv.variable->name, std::nullopt, AttributeEnum::Actual);

    auto periods = create_charging_schedule_periods_with_phases(0, 1, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID,
        ChargingProfileKindEnum::Absolute, 1);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut,
                testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodPhaseToUseACPhaseSwitchingUnsupported));
}

TEST_F(SmartChargingTest, K01FR20AndFR48_IfPhaseToUseSetAndACPhaseSwitchingSupportedTrue_ThenProfileIsNotInvalid) {
    auto periods = create_charging_schedule_periods_with_phases(0, 1, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID,
        ChargingProfileKindEnum::Absolute, 1);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Not(ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues));
}

TEST_F(SmartChargingTest, K01FR26_IfChargingRateUnitIsNotInChargingScheduleChargingRateUnits_ThenProfileIsInvalid) {
    const auto& charging_rate_unit_cv = ControllerComponentVariables::ChargingScheduleChargingRateUnit;
    device_model->set_value(charging_rate_unit_cv.component, charging_rate_unit_cv.variable.value(),
                            AttributeEnum::Actual, "W", "test", true);

    auto periods = create_charging_schedule_periods(0, 1, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID,
        ChargingProfileKindEnum::Absolute, 1);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingScheduleChargingRateUnitUnsupported));
}

TEST_F(SmartChargingTest, K01_IfChargingSchedulePeriodsAreMissing_ThenProfileIsInvalid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileNoChargingSchedulePeriods));
}

TEST_F(SmartChargingTest, K01FR31_IfStartPeriodOfFirstChargingSchedulePeriodIsNotZero_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(1);
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileFirstStartScheduleIsNotZero));
}

TEST_F(SmartChargingTest, K01FR35_IfChargingSchedulePeriodsAreNotInChonologicalOrder_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods({0, 2, 1});
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodsOutOfOrder));
}

TEST_F(SmartChargingTest, K01_ValidateChargingStationMaxProfile_NotChargingStationMaxProfile_Invalid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A));

    auto sut = smart_charging.validate_charging_station_max_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::InvalidProfileType));
}

TEST_F(SmartChargingTest, K04FR03_ValidateChargingStationMaxProfile_EvseIDgt0_Invalid) {
    const int EVSE_ID_1 = DEFAULT_EVSE_ID;
    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods));

    auto sut = smart_charging.validate_charging_station_max_profile(profile, EVSE_ID_1);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingStationMaxProfileEvseIdGreaterThanZero));
}

TEST_F(SmartChargingTest, K01FR38_ChargingProfilePurposeIsChargingStationMaxProfile_KindIsAbsolute_Valid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A));

    auto sut = smart_charging.validate_charging_station_max_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01FR38_ChargingProfilePurposeIsChargingStationMaxProfile_KindIsRecurring_Valid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {},
                                           ChargingProfileKindEnum::Recurring);

    auto sut = smart_charging.validate_charging_station_max_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01FR38_ChargingProfilePurposeIsChargingStationMaxProfile_KindIsRelative_Invalid) {
    auto profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                create_charge_schedule(ChargingRateUnitEnum::A), {}, ChargingProfileKindEnum::Relative);

    auto sut = smart_charging.validate_charging_station_max_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingStationMaxProfileCannotBeRelative));
}

TEST_F(SmartChargingTest, K01FR39_IfTxProfileHasSameTransactionAndStackLevelAsAnotherTxProfile_ThenProfileIsInvalid) {
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    auto same_stack_level = 42;
    auto profile_1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID,
                                             ChargingProfileKindEnum::Absolute, same_stack_level);
    auto profile_2 = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID,
                                             ChargingProfileKindEnum::Absolute, same_stack_level);
    smart_charging.add_profile(profile_2, DEFAULT_EVSE_ID);
    auto sut = smart_charging.validate_tx_profile(profile_1, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileConflictingStackLevel));
}

TEST_F(SmartChargingTest,
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
    smart_charging.add_profile(profile_2, DEFAULT_EVSE_ID);
    auto sut = smart_charging.validate_tx_profile(profile_1, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest,
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

    smart_charging.add_profile(profile_2, DEFAULT_EVSE_ID);
    auto sut = smart_charging.validate_tx_profile(profile_1, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest,
       K05_IfTxProfileHasNoTransactionIdButAddChargingProfileSourceIsRequestRemoteStartTransaction_ThenProfileIsValid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {});
    auto sut = smart_charging.validate_tx_profile(profile, DEFAULT_EVSE_ID,
                                                  AddChargingProfileSource::RequestStartTransactionRequest);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01FR40_IfChargingProfileKindIsAbsoluteAndStartScheduleDoesNotExist_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), DEFAULT_TX_ID,
                                           ChargingProfileKindEnum::Absolute, 1);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileMissingRequiredStartSchedule));
}

TEST_F(SmartChargingTest, K01FR40_IfChargingProfileKindIsRecurringAndStartScheduleDoesNotExist_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), DEFAULT_TX_ID,
                                           ChargingProfileKindEnum::Recurring, 1);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileMissingRequiredStartSchedule));
}

TEST_F(SmartChargingTest, K01FR41_IfChargingProfileKindIsRelativeAndStartScheduleDoesExist_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID,
        ChargingProfileKindEnum::Relative, 1);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileExtraneousStartSchedule));
}

TEST_F(SmartChargingTest, K01FR28_WhenEvseDoesNotExistThenReject) {
    auto sut = smart_charging.validate_evse_exists(NR_OF_EVSES + 1);
    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::EvseDoesNotExist));
}

TEST_F(SmartChargingTest, K01FR28_WhenEvseDoesExistThenAccept) {
    auto sut = smart_charging.validate_evse_exists(DEFAULT_EVSE_ID);
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
    : public SmartChargingTest,
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
    auto sut = smart_charging.validate_tx_default_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(expected));
}

class SmartChargingHandlerTestFixtureV201_FR53
    : public SmartChargingTest,
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
    auto sut = smart_charging.validate_tx_default_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(expected));
}

TEST_F(SmartChargingTest, K01FR52_TxDefaultProfileValidIfAppliedToWholeSystemAgain) {
    install_profile_on_evse(STATION_WIDE_ID, DEFAULT_PROFILE_ID);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {},
                                           ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto sut = smart_charging.validate_tx_default_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01FR53_TxDefaultProfileValidIfAppliedToExistingEvseAgain) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {},
                                           ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto sut = smart_charging.validate_tx_default_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01FR53_TxDefaultProfileValidIfAppliedToDifferentEvse) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {},
                                           ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto sut = smart_charging.validate_tx_default_profile(profile, DEFAULT_EVSE_ID + 1);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01FR44_IfNumberPhasesProvidedForDCEVSE_ThenProfileIsInvalid) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::DC));

    auto periods = create_charging_schedule_periods(0, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues));
}

TEST_F(SmartChargingTest, K01FR44_IfPhaseToUseProvidedForDCEVSE_ThenProfileIsInvalid) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::DC));

    auto periods = create_charging_schedule_periods(0, 1, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues));
}

TEST_F(SmartChargingTest, K01FR44_IfNumberPhasesProvidedForDCChargingStation_ThenProfileIsInvalid) {
    device_model->set_value(ControllerComponentVariables::ChargingStationSupplyPhases.component,
                            ControllerComponentVariables::ChargingStationSupplyPhases.variable.value(),
                            AttributeEnum::Actual, std::to_string(0), "test", true);

    auto periods = create_charging_schedule_periods(0, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, std::nullopt);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues));
}

TEST_F(SmartChargingTest, K01FR44_IfPhaseToUseProvidedForDCChargingStation_ThenProfileIsInvalid) {
    device_model->set_value(ControllerComponentVariables::ChargingStationSupplyPhases.component,
                            ControllerComponentVariables::ChargingStationSupplyPhases.variable.value(),
                            AttributeEnum::Actual, std::to_string(0), "test", true);

    auto periods = create_charging_schedule_periods(0, 1, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, std::nullopt);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues));
}

TEST_F(SmartChargingTest, K01FR45_IfNumberPhasesGreaterThanChargingStationSupplyPhasesForACEVSE_ThenProfileIsInvalid) {
    device_model->set_value(ControllerComponentVariables::ChargingStationSupplyPhases.component,
                            ControllerComponentVariables::ChargingStationSupplyPhases.variable.value(),
                            AttributeEnum::Actual, std::to_string(0), "test", true);
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::AC));

    auto periods = create_charging_schedule_periods(0, 4);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedNumberPhases));
}

TEST_F(SmartChargingTest,
       K01FR45_IfNumberPhasesGreaterThanChargingStationSupplyPhasesForACChargingStation_ThenProfileIsInvalid) {
    device_model->set_value(ControllerComponentVariables::ChargingStationSupplyPhases.component,
                            ControllerComponentVariables::ChargingStationSupplyPhases.variable.value(),
                            AttributeEnum::Actual, std::to_string(1), "test", true);

    auto periods = create_charging_schedule_periods(0, 4);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, std::nullopt);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedNumberPhases));
}

TEST_F(SmartChargingTest, K01FR49_IfNumberPhasesMissingForACEVSE_ThenSetNumberPhasesToThree) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::AC));

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, &mock_evse);

    auto numberPhases = profile.chargingSchedule[0].chargingSchedulePeriod[0].numberPhases;

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
    EXPECT_THAT(numberPhases, testing::Eq(3));
}

TEST_F(SmartChargingTest, K01FR49_IfNumberPhasesMissingForACChargingStation_ThenSetNumberPhasesToThree) {
    device_model->set_value(ControllerComponentVariables::ChargingStationSupplyPhases.component,
                            ControllerComponentVariables::ChargingStationSupplyPhases.variable.value(),
                            AttributeEnum::Actual, std::to_string(3), "test", true);

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile, std::nullopt);

    auto numberPhases = profile.chargingSchedule[0].chargingSchedulePeriod[0].numberPhases;

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
    EXPECT_THAT(numberPhases, testing::Eq(3));
}

TEST_F(SmartChargingTest, K01FR06_ExistingProfileLastsForever_RejectIncoming) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID, ocpp::DateTime(date::utc_clock::time_point::min()),
                            ocpp::DateTime(date::utc_clock::time_point::max()));

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-02T13:00:00"),
        ocpp::DateTime("2024-03-01T13:00:00"));

    auto sut = smart_charging.conform_and_validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateProfileValidityPeriod));
}

TEST_F(SmartChargingTest, K01FR06_ExisitingProfileHasValidFromIncomingValidToOverlaps_RejectIncoming) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID, ocpp::DateTime("2024-01-01T13:00:00"),
                            ocpp::DateTime(date::utc_clock::time_point::max()));

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, {}, ocpp::DateTime("2024-01-01T13:00:00"));

    auto sut = smart_charging.conform_and_validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateProfileValidityPeriod));
}

TEST_F(SmartChargingTest, K01FR06_ExisitingProfileHasValidToIncomingValidFromOverlaps_RejectIncoming) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID, ocpp::DateTime("2024-02-01T13:00:00"),
                            ocpp::DateTime(date::utc_clock::time_point::max()));

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-31T13:00:00"), {});

    auto sut = smart_charging.conform_and_validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateProfileValidityPeriod));
}

TEST_F(SmartChargingTest, K01FR06_ExisitingProfileHasValidPeriodIncomingIsNowToMax_RejectIncoming) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID,
                            ocpp::DateTime(date::utc_clock::now() - std::chrono::hours(5 * 24)),
                            ocpp::DateTime(date::utc_clock::now() + std::chrono::hours(5 * 24)));

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, {}, {});

    auto sut = smart_charging.conform_and_validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateProfileValidityPeriod));
}

TEST_F(SmartChargingTest, K01FR06_ExisitingProfileHasValidPeriodIncomingOverlaps_RejectIncoming) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID, ocpp::DateTime("2024-01-01T13:00:00"),
                            ocpp::DateTime("2024-02-01T13:00:00"));

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-15T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));

    auto sut = smart_charging.conform_and_validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateProfileValidityPeriod));
}

TEST_F(SmartChargingTest,
       K01FR06_ExisitingProfileHasValidPeriodIncomingOverlaps_IfProfileHasDifferentPurpose_ThenProfileIsValid) {
    auto periods = create_charging_schedule_periods(0);
    auto existing_profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), {},
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-01T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));
    auto response = smart_charging.conform_validate_and_add_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID,
        ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL, ocpp::DateTime("2024-01-15T13:00:00"),
        ocpp::DateTime("2024-02-01T13:00:00"));

    response = smart_charging.conform_validate_and_add_profile(profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    // verify TxDefaultProfile can still be validated when TxProfile was added
    auto sut = smart_charging.conform_and_validate_profile(existing_profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01_ValidateProfile_IfEvseDoesNotExist_ThenProfileIsInvalid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), DEFAULT_TX_ID);

    auto sut = smart_charging.conform_and_validate_profile(profile, NR_OF_EVSES + 1);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::EvseDoesNotExist));
}

TEST_F(SmartChargingTest, K01_ValidateProfile_IfScheduleIsInvalid_ThenProfileIsInvalid) {
    auto extraneous_start_schedule = ocpp::DateTime("2024-01-17T17:00:00");
    auto periods = create_charging_schedule_periods(0);
    auto profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                create_charge_schedule(ChargingRateUnitEnum::A, periods, extraneous_start_schedule),
                                DEFAULT_TX_ID, ChargingProfileKindEnum::Relative, 1);

    auto sut = smart_charging.conform_and_validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileExtraneousStartSchedule));
}

TEST_F(SmartChargingTest, K01_ValidateProfile_IfChargeStationMaxProfileIsInvalid_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut = smart_charging.conform_and_validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingStationMaxProfileEvseIdGreaterThanZero));
}

TEST_F(SmartChargingTest, K01_ValidateProfile_IfDuplicateTxDefaultProfileFoundOnEVSE_IsInvalid_ThenProfileIsInvalid) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto periods = create_charging_schedule_periods(0);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), {},
                                           ChargingProfileKindEnum::Relative, DEFAULT_STACK_LEVEL);

    auto sut = smart_charging.conform_and_validate_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateTxDefaultProfileFound));
}

TEST_F(SmartChargingTest,
       K01_ValidateProfile_IfDuplicateTxDefaultProfileFoundOnChargingStation_IsInvalid_ThenProfileIsInvalid) {
    install_profile_on_evse(STATION_WIDE_ID, DEFAULT_PROFILE_ID);

    auto periods = create_charging_schedule_periods(0);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), {},
                                           ChargingProfileKindEnum::Relative, DEFAULT_STACK_LEVEL);

    auto sut = smart_charging.conform_and_validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::DuplicateTxDefaultProfileFound));
}

TEST_F(SmartChargingTest, K01_ValidateProfile_IfTxProfileIsInvalid_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto sut = smart_charging.conform_and_validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileMissingTransactionId));
}

TEST_F(SmartChargingTest, K01_ValidateProfile_IfTxProfileIsValid_ThenProfileIsValid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);
    auto sut = smart_charging.conform_and_validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01_ValidateProfile_IfTxDefaultProfileIsValid_ThenProfileIsValid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto sut = smart_charging.conform_and_validate_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01_ValidateProfile_IfChargeStationMaxProfileIsValid_ThenProfileIsValid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut = smart_charging.conform_and_validate_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(
    SmartChargingTest,
    K01_ValidateProfile_IfExistingChargingProfileWithSameIdIsChargingStationExternalConstraints_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto external_constraints =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationExternalConstraints,
                                create_charge_schedule(ChargingRateUnitEnum::A), {});
    smart_charging.add_profile(external_constraints, STATION_WIDE_ID);

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto sut = smart_charging.conform_and_validate_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ExistingChargingStationExternalConstraints));
}

TEST_F(SmartChargingTest,
       K01FR14_IfTxDefaultProfileWithSameStackLevelDoesNotExist_ThenApplyStationWideTxDefaultProfileToAllEvses) {

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                           ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto sut = smart_charging.add_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(database_handler->get_all_charging_profiles(), testing::Contains(profile));
}

TEST_F(SmartChargingTest, K01FR15_IfTxDefaultProfileWithSameStackLevelDoesNotExist_ThenApplyTxDefaultProfileToEvse) {

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                           ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto sut = smart_charging.add_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(database_handler->get_all_charging_profiles(), testing::Contains(profile));
}

TEST_F(SmartChargingTest,
       K01FR05_IfProfileWithSameIdExistsOnEVSEAndIsNotChargingStationExternalContraints_ThenProfileIsReplaced) {

    auto profile1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto profile2 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto sut1 = smart_charging.add_profile(profile1, DEFAULT_EVSE_ID);
    auto sut2 = smart_charging.add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = database_handler->get_all_charging_profiles();

    EXPECT_THAT(sut1.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut2.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(profiles, testing::Contains(profile2));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile1)));
}

TEST_F(SmartChargingTest,
       K01FR05_IfProfileWithSameIdExistsOnChargingStationAndNewProfileIsOnEVSE_ThenProfileIsReplaced) {
    auto profile1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto profile2 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto sut1 = smart_charging.add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = smart_charging.add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = database_handler->get_all_charging_profiles();

    EXPECT_THAT(sut1.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut2.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(profiles, testing::Contains(profile2));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile1)));
}

TEST_F(SmartChargingTest, K01FR05_IfProfileWithSameIdExistsOnAnyEVSEAndNewProfileIsOnEVSE_ThenProfileIsReplaced) {
    auto profile1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto profile2 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto sut1 = smart_charging.add_profile(profile1, DEFAULT_EVSE_ID);
    auto sut2 = smart_charging.add_profile(profile2, DEFAULT_EVSE_ID + 1);

    auto profiles = database_handler->get_all_charging_profiles();

    EXPECT_THAT(sut1.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut2.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(profiles, testing::Contains(profile2));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile1)));
}

TEST_F(SmartChargingTest,
       K01FR05_IfProfileWithSameIdExistsOnAnyEVSEAndNewProfileIsOnChargingStation_ThenProfileIsReplaced) {
    auto profile1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto profile2 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto sut1 = smart_charging.add_profile(profile1, DEFAULT_EVSE_ID + 1);
    auto sut2 = smart_charging.add_profile(profile2, STATION_WIDE_ID);

    auto profiles = database_handler->get_all_charging_profiles();

    EXPECT_THAT(sut1.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut2.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(profiles, testing::Contains(profile2));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile1)));
}

TEST_F(SmartChargingTest,
       K01FR05_IfProfileWithSameIdExistsOnChargingStationAndNewProfileIsOnChargingStation_ThenProfileIsReplaced) {
    auto profile1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto profile2 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Relative, DEFAULT_STACK_LEVEL);

    auto sut1 = smart_charging.add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = smart_charging.add_profile(profile2, STATION_WIDE_ID);

    auto profiles = database_handler->get_all_charging_profiles();

    EXPECT_THAT(sut1.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut2.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(profiles, testing::Contains(profile2));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile1)));
}

TEST_F(SmartChargingTest,
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

    auto sut1 = smart_charging.add_profile(profile1, DEFAULT_EVSE_ID);
    auto sut2 = smart_charging.add_profile(profile2, DEFAULT_EVSE_ID);
    auto sut3 = smart_charging.add_profile(profile3, STATION_WIDE_ID);

    auto profiles = database_handler->get_all_charging_profiles();

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

    auto sut4 = smart_charging.add_profile(profile4, STATION_WIDE_ID);
    auto sut5 = smart_charging.add_profile(profile5, DEFAULT_EVSE_ID);

    profiles = database_handler->get_all_charging_profiles();

    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile2)));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile3)));

    EXPECT_THAT(profiles, testing::Contains(profile4));
    EXPECT_THAT(profiles, testing::Contains(profile5));
}

TEST_F(SmartChargingTest, K04FR01_AddProfile_OnlyAddsToOneEVSE) {
    auto profile1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto profile2 = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                            ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    auto response = smart_charging.add_profile(profile1, DEFAULT_EVSE_ID);
    auto response2 = smart_charging.add_profile(profile2, DEFAULT_EVSE_ID + 1);

    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(response2.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    auto sut1 = this->database_handler->get_charging_profiles_for_evse(DEFAULT_EVSE_ID);
    auto sut2 = this->database_handler->get_charging_profiles_for_evse(DEFAULT_EVSE_ID + 1);

    EXPECT_THAT(sut1, testing::Contains(profile1));
    EXPECT_THAT(sut1, testing::Not(testing::Contains(profile2)));

    EXPECT_THAT(sut2, testing::Contains(profile2));
    EXPECT_THAT(sut2, testing::Not(testing::Contains(profile1)));
}

TEST_F(SmartChargingTest, AddProfile_StoresChargingLimitSource) {
    auto charging_limit_source = ChargingLimitSourceEnum::SO;

    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto response = smart_charging.add_profile(profile, DEFAULT_EVSE_ID, charging_limit_source);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    ChargingProfileCriterion criteria;
    criteria.chargingProfileId = {{profile.id}};

    auto profiles = this->database_handler->get_charging_profiles_matching_criteria(DEFAULT_EVSE_ID, criteria);
    const auto [e, p, sut] = profiles[0];
    EXPECT_THAT(sut, ChargingLimitSourceEnum::SO);
}

TEST_F(SmartChargingTest, ValidateAndAddProfile_StoresChargingLimitSource) {
    auto charging_limit_source = ChargingLimitSourceEnum::SO;

    auto periods = create_charging_schedule_periods({0, 1, 2});

    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto response = smart_charging.conform_validate_and_add_profile(profile, DEFAULT_EVSE_ID, charging_limit_source);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    ChargingProfileCriterion criteria;
    criteria.chargingProfileId = {{profile.id}};

    auto profiles = this->database_handler->get_charging_profiles_matching_criteria(DEFAULT_EVSE_ID, criteria);
    ASSERT_THAT(profiles.size(), testing::Ge(1));
    const auto [e, p, sut] = profiles[0];
    EXPECT_THAT(sut, ChargingLimitSourceEnum::SO);
}

TEST_F(SmartChargingTest, K01_ValidateAndAdd_RejectsInvalidProfilesWithReasonCode) {
    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut = smart_charging.conform_validate_and_add_profile(profile, DEFAULT_EVSE_ID);
    auto status_info = sut.statusInfo;
    EXPECT_THAT(sut.status, testing::Eq(ChargingProfileStatusEnum::Rejected));
    EXPECT_THAT(status_info->reasonCode.get(), testing::Eq(conversions::profile_validation_result_to_reason_code(
                                                   ProfileValidationResultEnum::TxProfileMissingTransactionId)));

    EXPECT_THAT(status_info->additionalInfo.has_value(), testing::IsTrue());
    EXPECT_THAT(status_info->additionalInfo->get(), testing::Eq(conversions::profile_validation_result_to_string(
                                                        ProfileValidationResultEnum::TxProfileMissingTransactionId)));

    auto profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile)));
}

TEST_F(SmartChargingTest, K01_ValidateAndAdd_AddsValidProfiles) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    auto sut = smart_charging.conform_validate_and_add_profile(profile, DEFAULT_EVSE_ID);
    EXPECT_THAT(sut.status, testing::Eq(ChargingProfileStatusEnum::Accepted));
    EXPECT_THAT(sut.statusInfo.has_value(), testing::IsFalse());

    auto profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::Contains(profile));
}

TEST_F(SmartChargingTest, K09_GetChargingProfiles_EvseId) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile1 = create_charging_profile(
        1, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto profile2 = create_charging_profile(
        2, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut1 = smart_charging.conform_validate_and_add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = smart_charging.conform_validate_and_add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::SizeIs(2));

    auto reported_profiles = smart_charging.get_reported_profiles(
        create_get_charging_profile_request(DEFAULT_REQUEST_ID, create_charging_profile_criteria(), STATION_WIDE_ID));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));

    auto reported_profile = reported_profiles.at(0);
    EXPECT_THAT(profile1, testing::Eq(reported_profile.profile));

    reported_profiles = smart_charging.get_reported_profiles(
        create_get_charging_profile_request(DEFAULT_REQUEST_ID, create_charging_profile_criteria(), DEFAULT_EVSE_ID));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));

    reported_profile = reported_profiles.at(0);
    EXPECT_THAT(profile2, testing::Eq(reported_profile.profile));
}

TEST_F(SmartChargingTest, K09_GetChargingProfiles_NoEvseId) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile1 = create_charging_profile(
        1, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto profile2 = create_charging_profile(
        2, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut1 = smart_charging.conform_validate_and_add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = smart_charging.conform_validate_and_add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::SizeIs(2));

    auto reported_profiles = smart_charging.get_reported_profiles(
        create_get_charging_profile_request(DEFAULT_REQUEST_ID, create_charging_profile_criteria()));
    EXPECT_THAT(reported_profiles, testing::SizeIs(2));

    EXPECT_THAT(profiles, testing::Contains(reported_profiles.at(0).profile));
    EXPECT_THAT(profiles, testing::Contains(reported_profiles.at(1).profile));
}

TEST_F(SmartChargingTest, K09_GetChargingProfiles_ProfileId) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile1 = create_charging_profile(
        1, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto profile2 = create_charging_profile(
        2, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut1 = smart_charging.conform_validate_and_add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = smart_charging.conform_validate_and_add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::SizeIs(2));

    std::vector<int32_t> requested_profile_ids{1};
    auto reported_profiles = smart_charging.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID, create_charging_profile_criteria(std::nullopt, requested_profile_ids)));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));

    EXPECT_THAT(profile1, testing::Eq(reported_profiles.at(0).profile));
}

TEST_F(SmartChargingTest, K09_GetChargingProfiles_EvseIdAndStackLevel) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile1 = create_charging_profile(
        1, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), std::nullopt,
        ChargingProfileKindEnum::Absolute, 2); // contains different stackLevel(2)
    auto profile2 = create_charging_profile(
        2, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods,
                               ocpp::DateTime("2024-01-17T17:00:00"))); // contains default stackLevel(1)

    auto sut1 = smart_charging.conform_validate_and_add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = smart_charging.conform_validate_and_add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::SizeIs(2));

    auto reported_profiles = smart_charging.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID,
        create_charging_profile_criteria(std::nullopt, std::nullopt, std::nullopt, DEFAULT_STACK_LEVEL),
        DEFAULT_EVSE_ID));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));
    EXPECT_THAT(profile2, testing::Eq(reported_profiles.at(0).profile));
}

TEST_F(SmartChargingTest, K09_GetChargingProfiles_EvseIdAndSource) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut1 = smart_charging.conform_validate_and_add_profile(profile, DEFAULT_EVSE_ID);

    auto profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::SizeIs(1));

    std::vector<ChargingLimitSourceEnum> requested_sources_cso{ChargingLimitSourceEnum::CSO};
    std::vector<ChargingLimitSourceEnum> requested_sources_ems{ChargingLimitSourceEnum::EMS};

    auto reported_profiles = smart_charging.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID, create_charging_profile_criteria(requested_sources_cso), DEFAULT_EVSE_ID));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));
    EXPECT_THAT(profile, testing::Eq(reported_profiles.at(0).profile));

    reported_profiles = smart_charging.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID, create_charging_profile_criteria(requested_sources_ems), DEFAULT_EVSE_ID));

    EXPECT_THAT(reported_profiles, testing::SizeIs(0));
}

TEST_F(SmartChargingTest, K09_GetChargingProfiles_EvseIdAndPurposeAndStackLevel) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile1 = create_charging_profile(
        1, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    auto profile2 = create_charging_profile(
        2, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut1 = smart_charging.conform_validate_and_add_profile(profile1, STATION_WIDE_ID);
    auto sut2 = smart_charging.conform_validate_and_add_profile(profile2, DEFAULT_EVSE_ID);

    auto profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::SizeIs(2));

    auto reported_profiles = smart_charging.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID,
        create_charging_profile_criteria(std::nullopt, std::nullopt, ChargingProfilePurposeEnum::TxDefaultProfile),
        DEFAULT_EVSE_ID));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));
    EXPECT_THAT(profile2, testing::Eq(reported_profiles.at(0).profile));

    reported_profiles = smart_charging.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID,
        create_charging_profile_criteria(std::nullopt, std::nullopt, ChargingProfilePurposeEnum::TxDefaultProfile,
                                         DEFAULT_STACK_LEVEL),
        DEFAULT_EVSE_ID));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));
    EXPECT_THAT(profile2, testing::Eq(reported_profiles.at(0).profile));
}

TEST_F(SmartChargingTest, K09_GetChargingProfiles_ReportsProfileWithSource) {
    auto charging_limit_source = ChargingLimitSourceEnum::SO;

    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto response = smart_charging.conform_validate_and_add_profile(profile, DEFAULT_EVSE_ID, charging_limit_source);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    std::vector<int32_t> requested_profile_ids{1};
    auto reported_profiles = smart_charging.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID, create_charging_profile_criteria(std::nullopt, requested_profile_ids)));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));

    auto reported_profile = reported_profiles.at(0);
    EXPECT_THAT(profile, testing::Eq(reported_profile.profile));
    EXPECT_THAT(reported_profile.source, ChargingLimitSourceEnum::SO);
}

TEST_F(SmartChargingTest, K10_ClearChargingProfile_ClearsId) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    smart_charging.conform_validate_and_add_profile(profile, STATION_WIDE_ID);

    auto profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::Contains(profile));

    auto sut = smart_charging.clear_profiles(create_clear_charging_profile_request(DEFAULT_PROFILE_ID));
    EXPECT_THAT(sut.status, testing::Eq(ClearChargingProfileStatusEnum::Accepted));

    profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile)));
}

TEST_F(SmartChargingTest, K10_ClearChargingProfile_ClearsStackLevelPurposeCombination) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::Not(testing::IsEmpty()));

    auto sut = smart_charging.clear_profiles(create_clear_charging_profile_request(
        std::nullopt, create_clear_charging_profile(std::nullopt, ChargingProfilePurposeEnum::TxDefaultProfile,
                                                    DEFAULT_STACK_LEVEL)));
    EXPECT_THAT(sut.status, testing::Eq(ClearChargingProfileStatusEnum::Accepted));

    profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::IsEmpty());
}

TEST_F(SmartChargingTest, K10_ClearChargingProfile_UnknownStackLevelPurposeCombination) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::Not(testing::IsEmpty()));

    auto sut = smart_charging.clear_profiles(create_clear_charging_profile_request(
        std::nullopt, create_clear_charging_profile(std::nullopt, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                                    STATION_WIDE_ID)));
    EXPECT_THAT(sut.status, testing::Eq(ClearChargingProfileStatusEnum::Unknown));

    profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::Not(testing::IsEmpty()));
}

TEST_F(SmartChargingTest, K10_ClearChargingProfile_UnknownId) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));
    smart_charging.conform_validate_and_add_profile(profile, STATION_WIDE_ID);

    auto profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::Contains(profile));

    auto sut = smart_charging.clear_profiles(create_clear_charging_profile_request(178));
    EXPECT_THAT(sut.status, testing::Eq(ClearChargingProfileStatusEnum::Unknown));

    profiles = database_handler->get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::Contains(profile));
}

TEST_F(SmartChargingTest, K08_GetValidProfiles_IfNoProfiles_ThenNoValidProfilesReturned) {
    auto profiles = smart_charging.get_valid_profiles(DEFAULT_EVSE_ID);
    EXPECT_THAT(profiles, testing::IsEmpty());
}

TEST_F(SmartChargingTest, K08_GetValidProfiles_IfEvseHasProfiles_ThenThoseProfilesReturned) {
    auto profile = add_valid_profile_to(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);
    ASSERT_TRUE(profile.has_value());

    auto profiles = smart_charging.get_valid_profiles(DEFAULT_EVSE_ID);
    EXPECT_THAT(profiles, testing::Contains(profile));
}

TEST_F(SmartChargingTest, K08_GetValidProfiles_IfOtherEvseHasProfiles_ThenThoseProfilesAreNotReturned) {
    auto profile1 = add_valid_profile_to(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);
    ASSERT_TRUE(profile1.has_value());
    auto profile2 = add_valid_profile_to(DEFAULT_EVSE_ID + 1, DEFAULT_PROFILE_ID + 1);
    ASSERT_TRUE(profile2.has_value());

    auto profiles = smart_charging.get_valid_profiles(DEFAULT_EVSE_ID);
    EXPECT_THAT(profiles, testing::Contains(profile1));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile2)));
}

TEST_F(SmartChargingTest, K08_GetValidProfiles_IfStationWideProfilesExist_ThenThoseProfilesAreReturned) {
    auto profile = add_valid_profile_to(STATION_WIDE_ID, DEFAULT_PROFILE_ID);
    ASSERT_TRUE(profile.has_value());

    auto profiles = smart_charging.get_valid_profiles(DEFAULT_EVSE_ID);
    EXPECT_THAT(profiles, testing::Contains(profile));
}

TEST_F(SmartChargingTest, K08_GetValidProfiles_IfStationWideProfilesExist_ThenThoseProfilesAreReturnedOnce) {
    auto profile = add_valid_profile_to(STATION_WIDE_ID, DEFAULT_PROFILE_ID);
    ASSERT_TRUE(profile.has_value());

    auto profiles = smart_charging.get_valid_profiles(STATION_WIDE_ID);
    EXPECT_THAT(profiles, testing::Contains(profile));
    EXPECT_THAT(profiles.size(), testing::Eq(1));
}

TEST_F(SmartChargingTest, K08_GetValidProfiles_IfInvalidProfileExists_ThenThatProfileIsNotReturned) {
    auto extraneous_start_schedule = ocpp::DateTime("2024-01-17T17:00:00");
    auto periods = create_charging_schedule_periods(0);
    auto invalid_profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                create_charge_schedule(ChargingRateUnitEnum::A, periods, extraneous_start_schedule),
                                DEFAULT_TX_ID, ChargingProfileKindEnum::Relative, 1);
    smart_charging.add_profile(invalid_profile, DEFAULT_EVSE_ID);

    auto invalid_station_wide_profile =
        create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxProfile,
                                create_charge_schedule(ChargingRateUnitEnum::A, periods, extraneous_start_schedule),
                                DEFAULT_TX_ID, ChargingProfileKindEnum::Relative, 1);
    smart_charging.add_profile(invalid_station_wide_profile, STATION_WIDE_ID);

    auto profiles = smart_charging.get_valid_profiles(DEFAULT_EVSE_ID);
    EXPECT_THAT(profiles, testing::Not(testing::Contains(invalid_profile)));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(invalid_station_wide_profile)));
}

TEST_F(SmartChargingTest, K02FR05_SmartChargingTransactionEnds_DeletesTxProfilesByTransactionId) {
    auto transaction_id = uuid();
    EVLOG_debug << "TRANSACTION ID: " << transaction_id;
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, transaction_id);
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A,
                                                                  create_charging_schedule_periods({0, 1, 2}),
                                                                  ocpp::DateTime("2024-01-17T17:00:00")),
                                           transaction_id);

    auto response = smart_charging.conform_validate_and_add_profile(profile, DEFAULT_EVSE_ID);
    ASSERT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    smart_charging.delete_transaction_tx_profiles(transaction_id);

    EXPECT_THAT(this->database_handler->get_all_charging_profiles(), testing::IsEmpty());
}

TEST_F(SmartChargingTest, K02FR05_DeleteTransactionTxProfiles_DoesNotDeleteTxProfilesWithDifferentTransactionId) {
    auto transaction_id = uuid();
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, transaction_id);
    auto other_transaction_id = uuid();
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, other_transaction_id);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A,
                                                                  create_charging_schedule_periods({0, 1, 2}),
                                                                  ocpp::DateTime("2024-01-17T17:00:00")),
                                           other_transaction_id);

    auto response = smart_charging.conform_validate_and_add_profile(profile, DEFAULT_EVSE_ID);
    ASSERT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    smart_charging.delete_transaction_tx_profiles(transaction_id);

    EXPECT_THAT(this->database_handler->get_all_charging_profiles().size(), testing::Eq(1));
}

TEST_F(SmartChargingTest, K05FR02_RequestStartTransactionRequest_ChargingProfileMustBeTxProfile) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A,
                                                                  create_charging_schedule_periods({0, 1, 2}),
                                                                  ocpp::DateTime("2024-01-17T17:00:00")));

    auto sut = smart_charging.conform_and_validate_profile(profile, DEFAULT_EVSE_ID,
                                                           AddChargingProfileSource::RequestStartTransactionRequest);
    ASSERT_THAT(sut, testing::Eq(ProfileValidationResultEnum::RequestStartTransactionNonTxProfile));
}

TEST_F(SmartChargingTest, K01_ValidateChargingStationMaxProfile_AllowsExistingMatchingProfile) {
    auto profile =
        SmartChargingTestUtils::get_charging_profile_from_file("max/0/ChargingStationMaxProfile_grid_hourly.json");
    auto res = smart_charging.conform_validate_and_add_profile(profile, STATION_WIDE_ID);
    ASSERT_THAT(res.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    auto sut = smart_charging.validate_charging_station_max_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01_ValidateTxDefaultProfile_AllowsExistingMatchingProfile) {
    auto profile = SmartChargingTestUtils::get_charging_profile_from_file("singles/Absolute_301.json");
    auto res = smart_charging.conform_validate_and_add_profile(profile, DEFAULT_EVSE_ID);
    ASSERT_THAT(res.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    auto sut = smart_charging.validate_tx_default_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01_ValidateTxProfile_AllowsExistingMatchingProfile) {
    auto profile = SmartChargingTestUtils::get_charging_profile_from_file("baseline/TxProfile_1.json");
    this->evse_manager->open_transaction(DEFAULT_EVSE_ID, profile.transactionId.value());

    auto res = smart_charging.conform_validate_and_add_profile(profile, DEFAULT_EVSE_ID);
    ASSERT_THAT(res.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    auto sut = smart_charging.validate_tx_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01_SetChargingProfileRequest_ValidatesAndAddsProfile) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    SetChargingProfileRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.chargingProfile = profile;

    auto set_charging_profile_req =
        request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(req);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SetChargingProfileResponse>();
        EXPECT_EQ(response.status, ChargingProfileStatusEnum::Rejected);
        ASSERT_TRUE(response.statusInfo.has_value());
        EXPECT_EQ(response.statusInfo.value().reasonCode, "TxNotFound");
        ASSERT_TRUE(response.statusInfo->additionalInfo.has_value());
        EXPECT_EQ(response.statusInfo->additionalInfo.value(), "TxProfileEvseHasNoActiveTransaction");
    }));

    smart_charging.handle_message(set_charging_profile_req);
}

TEST_F(SmartChargingTest, K01FR07_SetChargingProfileRequest_TriggersCallbackWhenValid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    SetChargingProfileRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.chargingProfile = profile;

    auto set_charging_profile_req =
        request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(req);

    SetChargingProfileResponse accept_response;
    accept_response.status = ChargingProfileStatusEnum::Accepted;

    evse_manager->open_transaction(1, profile.transactionId.value());

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SetChargingProfileResponse>();
        EXPECT_EQ(response.status, ChargingProfileStatusEnum::Accepted);
    }));

    EXPECT_CALL(set_charging_profiles_callback_mock, Call);

    smart_charging.handle_message(set_charging_profile_req);
}

TEST_F(SmartChargingTest, K01FR07_SetChargingProfileRequest_DoesNotTriggerCallbackWhenInvalid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    SetChargingProfileRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.chargingProfile = profile;

    auto set_charging_profile_req =
        request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(req);

    SetChargingProfileResponse reject_response;
    reject_response.status = ChargingProfileStatusEnum::Rejected;
    reject_response.statusInfo = StatusInfo();
    reject_response.statusInfo->reasonCode = conversions::profile_validation_result_to_reason_code(
        ProfileValidationResultEnum::TxProfileEvseHasNoActiveTransaction);
    reject_response.statusInfo->additionalInfo = conversions::profile_validation_result_to_string(
        ProfileValidationResultEnum::TxProfileEvseHasNoActiveTransaction);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([reject_response](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SetChargingProfileResponse>();
        EXPECT_EQ(response.status, reject_response.status);
        ASSERT_TRUE(response.statusInfo.has_value());
        EXPECT_EQ(response.statusInfo.value().reasonCode.get(), reject_response.statusInfo.value().reasonCode.get());
        EXPECT_EQ(response.statusInfo.value().additionalInfo->get(),
                  reject_response.statusInfo.value().additionalInfo->get());
    }));

    EXPECT_CALL(set_charging_profiles_callback_mock, Call).Times(0);

    smart_charging.handle_message(set_charging_profile_req);
}

TEST_F(SmartChargingTest, K01FR22_SetChargingProfileRequest_RejectsChargingStationExternalConstraints) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationExternalConstraints,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    SetChargingProfileRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.chargingProfile = profile;

    auto set_charging_profile_req =
        request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(req);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SetChargingProfileResponse>();
        EXPECT_EQ(response.status, ChargingProfileStatusEnum::Rejected);
        ASSERT_TRUE(response.statusInfo.has_value());
        EXPECT_EQ(response.statusInfo.value().additionalInfo->get(),
                  "ChargingStationExternalConstraintsInSetChargingProfileRequest");
    }));
    EXPECT_CALL(set_charging_profiles_callback_mock, Call).Times(0);

    smart_charging.handle_message(set_charging_profile_req);
}

TEST_F(SmartChargingTest, K01FR29_SmartChargingCtrlrAvailableIsFalse_RespondsCallError) {
    const auto cv = ControllerComponentVariables::SmartChargingCtrlrAvailable;
    this->device_model->set_value(cv.component, cv.variable.value(), AttributeEnum::Actual, "false", "TEST", true);

    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    SetChargingProfileRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.chargingProfile = profile;

    auto set_charging_profile_req =
        request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(req);

    EXPECT_CALL(mock_dispatcher, dispatch_call_error(_)).WillOnce([](const ocpp::CallError& call_error) {
        EXPECT_EQ(call_error.errorCode, "NotSupported");
    });

    smart_charging.handle_message(set_charging_profile_req);
}

TEST_F(SmartChargingTest, K08_GetCompositeSchedule_CallsCalculateGetCompositeSchedule) {
    GetCompositeScheduleRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.chargingRateUnit = ChargingRateUnitEnum::W;

    auto get_composite_schedule_req =
        request_to_enhanced_message<GetCompositeScheduleRequest, MessageType::GetCompositeSchedule>(req);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<GetCompositeScheduleResponse>();
        EXPECT_EQ(response.status, GenericStatusEnum::Accepted);
    }));

    smart_charging.handle_message(get_composite_schedule_req);
}

TEST_F(SmartChargingTest, K08_GetCompositeSchedule_CallsCalculateGetCompositeScheduleWithValidProfiles) {
    GetCompositeScheduleRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.chargingRateUnit = ChargingRateUnitEnum::W;

    auto get_composite_schedule_req =
        request_to_enhanced_message<GetCompositeScheduleRequest, MessageType::GetCompositeSchedule>(req);

    std::vector<ChargingProfile> profiles = {
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                create_charge_schedule(ChargingRateUnitEnum::A,
                                                       create_charging_schedule_periods({0, 1, 2}),
                                                       ocpp::DateTime("2024-01-17T17:00:00")),
                                DEFAULT_TX_ID),
    };

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<GetCompositeScheduleResponse>();
        EXPECT_EQ(response.status, GenericStatusEnum::Accepted);
    }));

    smart_charging.handle_message(get_composite_schedule_req);
}

TEST_F(SmartChargingTest, K08FR05_GetCompositeSchedule_DoesNotCalculateCompositeScheduleForNonexistentEVSE) {
    GetCompositeScheduleRequest req;
    req.evseId = DEFAULT_EVSE_ID + 3;
    req.chargingRateUnit = ChargingRateUnitEnum::W;

    auto get_composite_schedule_req =
        request_to_enhanced_message<GetCompositeScheduleRequest, MessageType::GetCompositeSchedule>(req);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<GetCompositeScheduleResponse>();
        EXPECT_EQ(response.status, GenericStatusEnum::Rejected);
        ASSERT_TRUE(response.statusInfo.has_value());
        ASSERT_TRUE(response.statusInfo.value().additionalInfo.has_value());
        EXPECT_EQ(response.statusInfo.value().additionalInfo.value(), "EvseDoesNotExist");
    }));

    smart_charging.handle_message(get_composite_schedule_req);
}

TEST_F(SmartChargingTest, K08FR07_GetCompositeSchedule_DoesNotCalculateCompositeScheduleForIncorrectChargingRateUnit) {
    GetCompositeScheduleRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.chargingRateUnit = ChargingRateUnitEnum::W;

    auto get_composite_schedule_req =
        request_to_enhanced_message<GetCompositeScheduleRequest, MessageType::GetCompositeSchedule>(req);

    const auto& charging_rate_unit_cv = ControllerComponentVariables::ChargingScheduleChargingRateUnit;
    device_model->set_value(charging_rate_unit_cv.component, charging_rate_unit_cv.variable.value(),
                            AttributeEnum::Actual, "A", "test", true);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<GetCompositeScheduleResponse>();
        EXPECT_EQ(response.status, GenericStatusEnum::Rejected);
        ASSERT_TRUE(response.statusInfo.has_value());
        ASSERT_TRUE(response.statusInfo.value().additionalInfo.has_value());
        EXPECT_EQ(response.statusInfo.value().additionalInfo.value(), "ChargingScheduleChargingRateUnitUnsupported");
        EXPECT_EQ(response.statusInfo.value().reasonCode, "UnsupportedRateUnit");
    }));

    smart_charging.handle_message(get_composite_schedule_req);
}

} // namespace ocpp::v201
