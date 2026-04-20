// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include "connectivity_manager_mock.hpp"
#include "date/tz.h"
#include "everest/logging.hpp"
#include "lib/ocpp/common/database_testing_utils.hpp"
#include "message_dispatcher_mock.hpp"
#include "ocpp/common/types.hpp"
#include "ocpp/v2/ctrlr_component_variables.hpp"
#include "ocpp/v2/device_model.hpp"
#include "ocpp/v2/functional_blocks/functional_block_context.hpp"
#include "ocpp/v2/functional_blocks/smart_charging.hpp"
#include "ocpp/v2/ocpp_types.hpp"
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
#include <limits>
#include <ocpp/common/call_types.hpp>
#include <ocpp/v2/evse.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <optional>

#include "comparators.hpp"
#include "smart_charging_test_utils.hpp"
#include <sstream>
#include <vector>

#include <ocpp/v2/messages/ClearChargingProfile.hpp>
#include <ocpp/v2/messages/GetChargingProfiles.hpp>
#include <ocpp/v2/messages/GetCompositeSchedule.hpp>
#include <ocpp/v2/messages/NotifyEVChargingNeeds.hpp>
#include <ocpp/v2/messages/NotifyEVChargingSchedule.hpp>
#include <ocpp/v2/messages/SetChargingProfile.hpp>

using ::testing::_;
using ::testing::ByMove;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::Return;
using ::testing::ReturnRef;

namespace ocpp::v2 {
class SmartChargingTest : public DatabaseTestingUtils {
protected:
    void SetUp() override {
    }

    void TearDown() override {
        // TODO: use in-memory db so we don't need to reset the db between tests
        this->database_handler->clear_charging_profiles();
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
        std::unique_ptr<everest::db::sqlite::Connection> database_connection =
            std::make_unique<everest::db::sqlite::Connection>(fs::path("/tmp/ocpp201") / "cp.db");
        database_handler =
            std::make_shared<DatabaseHandler>(std::move(database_connection), MIGRATION_FILES_LOCATION_V2);
        database_handler->open_connection();
        device_model = device_model_test_helper.get_device_model();
        this->functional_block_context = std::make_unique<FunctionalBlockContext>(
            this->mock_dispatcher, *this->device_model, this->connectivity_manager, *this->evse_manager,
            *this->database_handler, this->evse_security, this->component_state_manager, this->ocpp_version);
        // Defaults
        const auto& charging_rate_unit_cv = ControllerComponentVariables::ChargingScheduleChargingRateUnit;
        device_model->set_value(charging_rate_unit_cv.component, charging_rate_unit_cv.variable.value(),
                                AttributeEnum::Actual, "A,W", "test", true);

        const auto& ac_phase_switching_cv = ControllerComponentVariables::ACPhaseSwitchingSupported;
        device_model->set_value(ac_phase_switching_cv.component, ac_phase_switching_cv.variable.value(),
                                AttributeEnum::Actual, ac_phase_switching_supported.value_or(""), "test", true);
        return TestSmartCharging(*functional_block_context, set_charging_profiles_callback_mock.AsStdFunction(),
                                 stop_transaction_callback_mock.AsStdFunction());
    }

    /// \brief Builds a TestSmartCharging with the HLC handoff + K16 renegotiation callbacks
    /// wired. Existing K01FR07 tests assume these callbacks are NOT wired (they implicitly
    /// disable K15.FR.17's InvalidMessageSeq path), so only tests that exercise HLC flows use
    /// this helper — via the `smart_charging_hlc` unique_ptr.
    std::unique_ptr<TestSmartCharging> create_smart_charging_with_hlc() {
        return std::make_unique<TestSmartCharging>(*functional_block_context,
                                                   set_charging_profiles_callback_mock.AsStdFunction(),
                                                   stop_transaction_callback_mock.AsStdFunction(),
                                                   notify_ev_charging_needs_response_callback_mock.AsStdFunction(),
                                                   transfer_ev_charging_schedules_callback_mock.AsStdFunction(),
                                                   trigger_schedule_renegotiation_callback_mock.AsStdFunction());
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
    std::unique_ptr<EvseManagerFake> evse_manager = std::make_unique<EvseManagerFake>(NR_OF_TWO_EVSES);

    sqlite3* db_handle;
    std::shared_ptr<DatabaseHandler> database_handler;

    bool ignore_no_transaction = true;
    DeviceModel* device_model;
    ::testing::NiceMock<ConnectivityManagerMock> connectivity_manager;
    ocpp::EvseSecurityMock evse_security;
    ComponentStateManagerMock component_state_manager;
    MockFunction<void()> set_charging_profiles_callback_mock;
    MockFunction<RequestStartStopStatusEnum(const std::int32_t evse_id, const ReasonEnum& stop_reason)>
        stop_transaction_callback_mock;
    MockFunction<void(std::int32_t evse_id, NotifyEVChargingNeedsStatusEnum status)>
        notify_ev_charging_needs_response_callback_mock;
    MockFunction<void(std::int32_t evse_id, const std::string& transaction_id,
                      const std::vector<ChargingSchedule>& schedules,
                      const std::vector<std::optional<SalesTariff>>& tariffs,
                      const std::vector<std::optional<std::string>>& signature_value_b64,
                      const std::optional<std::int32_t>& selected_charging_schedule_id)>
        transfer_ev_charging_schedules_callback_mock;
    MockFunction<void(std::int32_t evse_id)> trigger_schedule_renegotiation_callback_mock;
    std::unique_ptr<FunctionalBlockContext> functional_block_context;
    TestSmartCharging smart_charging = create_smart_charging();
    boost::uuids::random_generator uuid_generator = boost::uuids::random_generator();
    std::atomic<OcppProtocolVersion> ocpp_version = OcppProtocolVersion::v201;
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

TEST_F(SmartChargingTest, IfChargingSchedulePeriodLimitIsInfinity_ThenProfileIsInvalid) {
    ChargingSchedulePeriod period;
    period.startPeriod = 0;
    period.limit = std::numeric_limits<float>::infinity();
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, {period}), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodNonFiniteValue));
}

TEST_F(SmartChargingTest, IfChargingSchedulePeriodLimitIsNaN_ThenProfileIsInvalid) {
    ChargingSchedulePeriod period;
    period.startPeriod = 0;
    period.limit = std::numeric_limits<float>::quiet_NaN();
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, {period}), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodNonFiniteValue));
}

TEST_F(SmartChargingTest, IfChargingScheduleMinChargingRateIsInfinity_ThenProfileIsInvalid) {
    auto schedule = create_charge_schedule(ChargingRateUnitEnum::A, create_charging_schedule_periods(0));
    schedule.minChargingRate = std::numeric_limits<float>::infinity();
    auto profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile, schedule, DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingScheduleNonFiniteValue));
}

TEST_F(SmartChargingTest, IfChargingSchedulePowerToleranceIsNaN_ThenProfileIsInvalid) {
    auto schedule = create_charge_schedule(ChargingRateUnitEnum::A, create_charging_schedule_periods(0));
    schedule.powerTolerance = std::numeric_limits<float>::quiet_NaN();
    auto profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile, schedule, DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingScheduleNonFiniteValue));
}

TEST_F(SmartChargingTest, IfChargingScheduleLimitAtSoCIsInfinity_ThenProfileIsInvalid) {
    auto schedule = create_charge_schedule(ChargingRateUnitEnum::A, create_charging_schedule_periods(0));
    LimitAtSoC limit_at_soc;
    limit_at_soc.soc = 80;
    limit_at_soc.limit = std::numeric_limits<float>::infinity();
    schedule.limitAtSoC = limit_at_soc;
    auto profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile, schedule, DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingScheduleNonFiniteValue));
}

TEST_F(SmartChargingTest, IfV2XFreqWattCurveFrequencyIsInfinity_ThenProfileIsInvalid) {
    ChargingSchedulePeriod period;
    period.startPeriod = 0;
    V2XFreqWattPoint point;
    point.frequency = std::numeric_limits<float>::infinity();
    point.power = 1000.0f;
    period.v2xFreqWattCurve = {point};
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, {period}), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodNonFiniteValue));
}

TEST_F(SmartChargingTest, IfV2XSignalWattCurvePowerIsNaN_ThenProfileIsInvalid) {
    ChargingSchedulePeriod period;
    period.startPeriod = 0;
    V2XSignalWattPoint point;
    point.signal = 0;
    point.power = std::numeric_limits<float>::quiet_NaN();
    period.v2xSignalWattCurve = {point};
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, {period}), DEFAULT_TX_ID);

    auto sut = smart_charging.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodNonFiniteValue));
}

TEST_F(SmartChargingTest, K01_ValidateChargingStationMaxProfile_NotChargingStationMaxProfile_Invalid) {
    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A));

    auto sut = validate_charging_station_max_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::InvalidProfileType));
}

TEST_F(SmartChargingTest, K04FR03_ValidateChargingStationMaxProfile_EvseIDgt0_Invalid) {
    const int EVSE_ID_1 = DEFAULT_EVSE_ID;
    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods));

    auto sut = validate_charging_station_max_profile(profile, EVSE_ID_1);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingStationMaxProfileEvseIdGreaterThanZero));
}

TEST_F(SmartChargingTest, K01FR38_ChargingProfilePurposeIsChargingStationMaxProfile_KindIsAbsolute_Valid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A));

    auto sut = validate_charging_station_max_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01FR38_ChargingProfilePurposeIsChargingStationMaxProfile_KindIsRecurring_Valid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {},
                                           ChargingProfileKindEnum::Recurring);

    auto sut = validate_charging_station_max_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(SmartChargingTest, K01FR38_ChargingProfilePurposeIsChargingStationMaxProfile_KindIsRelative_Invalid) {
    auto profile =
        create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                create_charge_schedule(ChargingRateUnitEnum::A), {}, ChargingProfileKindEnum::Relative);

    auto sut = validate_charging_station_max_profile(profile, STATION_WIDE_ID);

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
    auto sut = smart_charging.validate_evse_exists(NR_OF_TWO_EVSES + 1);
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
class SmartChargingHandlerTestFixtureV2_FR52
    : public SmartChargingTest,
      public ::testing::WithParamInterface<std::tuple<int, int, ProfileValidationResultEnum>> {};

INSTANTIATE_TEST_SUITE_P(TxDefaultProfileValidationV2_Param_Test_Instantiate_FR52,
                         SmartChargingHandlerTestFixtureV2_FR52,
                         testing::Values(std::make_tuple(DEFAULT_PROFILE_ID + 1, DEFAULT_STACK_LEVEL,
                                                         ProfileValidationResultEnum::DuplicateTxDefaultProfileFound),
                                         std::make_tuple(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL,
                                                         ProfileValidationResultEnum::Valid),
                                         std::make_tuple(DEFAULT_PROFILE_ID + 1, DEFAULT_STACK_LEVEL + 1,
                                                         ProfileValidationResultEnum::Valid)));

TEST_P(SmartChargingHandlerTestFixtureV2_FR52, K01FR52_TxDefaultProfileValidationV2Tests) {
    auto [added_profile_id, added_stack_level, expected] = GetParam();
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto profile = create_charging_profile(added_profile_id, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), {},
                                           ChargingProfileKindEnum::Absolute, added_stack_level);
    auto sut = smart_charging.validate_tx_default_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(expected));
}

class SmartChargingHandlerTestFixtureV2_FR53
    : public SmartChargingTest,
      public ::testing::WithParamInterface<std::tuple<int, int, ProfileValidationResultEnum>> {};

INSTANTIATE_TEST_SUITE_P(TxDefaultProfileValidationV2_Param_Test_Instantiate_FR53,
                         SmartChargingHandlerTestFixtureV2_FR53,
                         testing::Values(std::make_tuple(DEFAULT_PROFILE_ID + 1, DEFAULT_STACK_LEVEL,
                                                         ProfileValidationResultEnum::DuplicateTxDefaultProfileFound),
                                         std::make_tuple(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL,
                                                         ProfileValidationResultEnum::Valid),
                                         std::make_tuple(DEFAULT_PROFILE_ID + 1, DEFAULT_STACK_LEVEL + 1,
                                                         ProfileValidationResultEnum::Valid)));

TEST_P(SmartChargingHandlerTestFixtureV2_FR53, K01FR53_TxDefaultProfileValidationV2Tests) {
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

    auto sut = smart_charging.conform_and_validate_profile(profile, NR_OF_TWO_EVSES + 1);

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
    auto charging_limit_source = ChargingLimitSourceEnumStringType::SO;

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
    EXPECT_THAT(sut, ChargingLimitSourceEnumStringType::SO);
}

TEST_F(SmartChargingTest, ValidateAndAddProfile_StoresChargingLimitSource) {
    auto charging_limit_source = ChargingLimitSourceEnumStringType::SO;

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
    EXPECT_THAT(sut, ChargingLimitSourceEnumStringType::SO);
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

    std::vector<std::int32_t> requested_profile_ids{1};
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

    std::vector<CiString<20>> requested_sources_cso{ChargingLimitSourceEnumStringType::CSO};
    std::vector<CiString<20>> requested_sources_ems{ChargingLimitSourceEnumStringType::EMS};

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
    auto charging_limit_source = ChargingLimitSourceEnumStringType::SO;

    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")));

    auto response = smart_charging.conform_validate_and_add_profile(profile, DEFAULT_EVSE_ID, charging_limit_source);
    EXPECT_THAT(response.status, testing::Eq(ChargingProfileStatusEnum::Accepted));

    std::vector<std::int32_t> requested_profile_ids{1};
    auto reported_profiles = smart_charging.get_reported_profiles(create_get_charging_profile_request(
        DEFAULT_REQUEST_ID, create_charging_profile_criteria(std::nullopt, requested_profile_ids)));
    EXPECT_THAT(reported_profiles, testing::SizeIs(1));

    auto reported_profile = reported_profiles.at(0);
    EXPECT_THAT(profile, testing::Eq(reported_profile.profile));
    EXPECT_THAT(reported_profile.source, ChargingLimitSourceEnumStringType::SO);
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

    auto sut = validate_charging_station_max_profile(profile, STATION_WIDE_ID);

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

TEST_F(SmartChargingTest, K01_ValidateTxProfile_EmptyChargingSchedule) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationMaxProfile,
                                           std::vector<ChargingSchedule>{}, ocpp::DateTime("2024-01-17T17:00:00"));

    auto sut = smart_charging.conform_and_validate_profile(profile, DEFAULT_EVSE_ID);
    ASSERT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileEmptyChargingSchedules));
}

// --- EVerest#1199: OCPP ↔ ISO 15118 schedule handoff ---

TEST_F(SmartChargingTest, K15FR21_NotifyEvChargingSchedule_SelectedIdOmittedOnV201) {
    // OCPP 2.0.1 must not populate selectedChargingScheduleId (K15.FR.21 is 2.1 only).
    ocpp_version.store(OcppProtocolVersion::v201);

    NotifyEVChargingScheduleRequest captured;
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _)).WillOnce(Invoke([&captured](const json& call, bool) {
        captured = call.at(ocpp::CALL_PAYLOAD).get<NotifyEVChargingScheduleRequest>();
    }));

    ChargingSchedule schedule;
    schedule.id = 42;
    schedule.chargingRateUnit = ChargingRateUnitEnum::A;
    smart_charging.notify_ev_charging_schedule_req(DEFAULT_EVSE_ID, 42, 7, schedule);

    EXPECT_EQ(captured.evseId, DEFAULT_EVSE_ID);
    EXPECT_EQ(captured.chargingSchedule.id, 42);
    EXPECT_FALSE(captured.selectedChargingScheduleId.has_value());
}

TEST_F(SmartChargingTest, K15FR21_NotifyEvChargingSchedule_SelectedIdPopulatedOnV21) {
    // OCPP 2.1 SHOULD set selectedChargingScheduleId to the id of the schedule the EV selected.
    ocpp_version.store(OcppProtocolVersion::v21);

    NotifyEVChargingScheduleRequest captured;
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _)).WillOnce(Invoke([&captured](const json& call, bool) {
        captured = call.at(ocpp::CALL_PAYLOAD).get<NotifyEVChargingScheduleRequest>();
    }));

    ChargingSchedule schedule;
    schedule.id = 42;
    schedule.chargingRateUnit = ChargingRateUnitEnum::A;
    smart_charging.notify_ev_charging_schedule_req(DEFAULT_EVSE_ID, 42, 7, schedule);

    ASSERT_TRUE(captured.selectedChargingScheduleId.has_value());
    EXPECT_EQ(captured.selectedChargingScheduleId.value(), 7);
    EXPECT_EQ(captured.chargingSchedule.id, 42);
}

// K15.FR.22: compose up to three SA schedules by combining stack levels slot-wise.
TEST(SaScheduleCompositionTest, K15FR22_EmptyWhenNoTxProfileForTransaction) {
    EXPECT_TRUE(compute_sa_schedule_list({}, "tx-1").empty());

    auto other_tx = create_charging_profile(1, ChargingProfilePurposeEnum::TxProfile,
                                            create_charge_schedule(ChargingRateUnitEnum::A), std::string{"tx-other"});
    EXPECT_TRUE(compute_sa_schedule_list({other_tx}, "tx-1").empty());
}

TEST(SaScheduleCompositionTest, K15FR22_SingleTxProfileEmitsItsSchedulesInOrder) {
    auto periods = create_charging_schedule_periods({0, 1800, 3600});
    auto s1 = create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00"));
    s1.id = 10;
    auto s2 = create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00"));
    s2.id = 20;

    auto profile = create_charging_profile(1, ChargingProfilePurposeEnum::TxProfile,
                                           std::vector<ChargingSchedule>{s1, s2}, std::string{"tx-1"});

    const auto slots = compute_sa_schedule_list({profile}, "tx-1");
    ASSERT_EQ(slots.size(), 2);
    EXPECT_EQ(slots[0].schedule.id, 10);
    EXPECT_EQ(slots[1].schedule.id, 20);
}

TEST(SaScheduleCompositionTest, K15FR22_StackLevelPrecedencePicksHighest) {
    auto periods = create_charging_schedule_periods({0});
    auto low = create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00"));
    low.id = 1;
    auto high = create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00"));
    high.id = 99;

    auto low_profile =
        create_charging_profile(1, ChargingProfilePurposeEnum::TxProfile, std::vector<ChargingSchedule>{low},
                                std::string{"tx-1"}, ChargingProfileKindEnum::Absolute, /*stack_level=*/0);
    auto high_profile =
        create_charging_profile(2, ChargingProfilePurposeEnum::TxProfile, std::vector<ChargingSchedule>{high},
                                std::string{"tx-1"}, ChargingProfileKindEnum::Absolute, /*stack_level=*/5);

    const auto slots = compute_sa_schedule_list({low_profile, high_profile}, "tx-1");
    ASSERT_EQ(slots.size(), 1);
    EXPECT_EQ(slots[0].schedule.id, 99);
}

TEST(SaScheduleCompositionTest, K15FR22_CapsAtThreeSlotsAcrossStackLevels) {
    auto periods = create_charging_schedule_periods({0});
    auto mk_schedule = [&](int id) {
        auto s = create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00"));
        s.id = id;
        return s;
    };

    auto a = create_charging_profile(
        1, ChargingProfilePurposeEnum::TxProfile,
        std::vector<ChargingSchedule>{mk_schedule(10), mk_schedule(11), mk_schedule(12), mk_schedule(13)},
        std::string{"tx-1"}, ChargingProfileKindEnum::Absolute, 1);

    const auto slots = compute_sa_schedule_list({a}, "tx-1");
    ASSERT_EQ(slots.size(), 3);
    EXPECT_EQ(slots[0].schedule.id, 10);
    EXPECT_EQ(slots[1].schedule.id, 11);
    EXPECT_EQ(slots[2].schedule.id, 12);
}

TEST_F(SmartChargingTest, K15FR21_NotifyEvChargingSchedule_NoEvScheduleUsesFallback) {
    // When the EV did not return a profile we still emit a well-formed message; the id of the
    // fallback schedule matches the SAScheduleTupleId the EV picked (K15.FR.19). The fallback
    // chargingSchedule must carry at least one chargingSchedulePeriod — the OCPP JSON schema
    // marks chargingSchedulePeriod as a required array with minItems=1, so an empty array would
    // be CSMS-rejected.
    ocpp_version.store(OcppProtocolVersion::v201);

    NotifyEVChargingScheduleRequest captured;
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _)).WillOnce(Invoke([&captured](const json& call, bool) {
        captured = call.at(ocpp::CALL_PAYLOAD).get<NotifyEVChargingScheduleRequest>();
    }));

    smart_charging.notify_ev_charging_schedule_req(DEFAULT_EVSE_ID, 7, std::nullopt, std::nullopt);

    EXPECT_EQ(captured.chargingSchedule.id, 7);
    ASSERT_FALSE(captured.chargingSchedule.chargingSchedulePeriod.empty())
        << "K15.FR.19 fallback must include at least one chargingSchedulePeriod";
    EXPECT_EQ(captured.chargingSchedule.chargingSchedulePeriod.front().startPeriod, 0)
        << "First period must start at offset 0 per K01.FR.31";
}

/// \brief K16 renegotiation tests — Set/ClearChargingProfile during an active HLC session.
///
/// Shared setup sequence for the K16 tests:
///  1. Open a transaction on an EVSE.
///  2. Dispatch `notify_ev_charging_needs_req` to populate the per-tx HLC coordination state.
///  3. Send an initial SetChargingProfile (TxProfile) for that tx — the existing K15.FR.07 path
///     fires `transfer_ev_charging_schedules_callback` and caches the handed-off schedule.
///  4. Drive the renegotiation scenario (second SetChargingProfile with same or different limit,
///     or a ClearChargingProfile).
class K16RenegotiationFixture : public SmartChargingTest {
protected:
    std::unique_ptr<TestSmartCharging> smart_charging_hlc;

    void SetUp() override {
        SmartChargingTest::SetUp();
        smart_charging_hlc = create_smart_charging_with_hlc();
    }

    // Build a TxProfile with a single-period schedule at the given limit, so two profiles
    // with different limits produce materially different composite schedules.
    ChargingProfile make_tx_profile(std::int32_t profile_id, std::int32_t stack_level, float limit) {
        auto period = create_charging_schedule_periods(0, DEFAULT_NR_PHASES, std::nullopt, limit);
        auto schedule = create_charge_schedule(ChargingRateUnitEnum::A, period, ocpp::DateTime("2024-01-17T17:00:00"));
        return create_charging_profile(profile_id, ChargingProfilePurposeEnum::TxProfile, schedule, DEFAULT_TX_ID,
                                       ChargingProfileKindEnum::Absolute, stack_level);
    }

    // Drive the initial HLC handoff for DEFAULT_TX_ID on DEFAULT_EVSE_ID. After this call the
    // `last_handed_off_schedules` cache for DEFAULT_EVSE_ID is populated with the schedule of
    // the given profile, and the per-tx `ev_schedule_state` entry has been erased.
    void drive_initial_handoff(const ChargingProfile& initial_profile) {
        evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

        NotifyEVChargingNeedsRequest needs_req;
        needs_req.evseId = DEFAULT_EVSE_ID;
        needs_req.chargingNeeds.requestedEnergyTransfer = EnergyTransferModeEnum::AC_three_phase;
        EXPECT_CALL(mock_dispatcher, dispatch_call(_, _));
        smart_charging_hlc->notify_ev_charging_needs_req(needs_req);

        SetChargingProfileRequest set_req;
        set_req.evseId = DEFAULT_EVSE_ID;
        set_req.chargingProfile = initial_profile;
        auto initial_set =
            request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(set_req);

        EXPECT_CALL(mock_dispatcher, dispatch_call_result(_));
        EXPECT_CALL(set_charging_profiles_callback_mock, Call);
        EXPECT_CALL(transfer_ev_charging_schedules_callback_mock, Call(DEFAULT_EVSE_ID, DEFAULT_TX_ID, _, _, _, _));
        smart_charging_hlc->handle_message(initial_set);
        ::testing::Mock::VerifyAndClearExpectations(&mock_dispatcher);
        ::testing::Mock::VerifyAndClearExpectations(&set_charging_profiles_callback_mock);
        ::testing::Mock::VerifyAndClearExpectations(&transfer_ev_charging_schedules_callback_mock);
    }
};

// Second SetChargingProfile with a materially different schedule fires renegotiation,
// and also re-delivers the recomputed composite schedule so the ISO stack serves the
// new bundle when the EV re-enters CPD (K16.FR.03). Without the re-delivery, the
// ReNegotiation latch fires but evse_sa_schedule_list is stale.
TEST_F(K16RenegotiationFixture, K16FR02_DifferentScheduleTriggersRenegotiation) {
    auto initial = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/16.0f);
    drive_initial_handoff(initial);

    // Same profile id + same stack_level → replaces the existing profile (K01FR05).
    auto updated = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/32.0f);
    SetChargingProfileRequest set_req;
    set_req.evseId = DEFAULT_EVSE_ID;
    set_req.chargingProfile = updated;
    auto second_set = request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(set_req);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_));
    EXPECT_CALL(set_charging_profiles_callback_mock, Call);
    // K16.FR.03: the recomputed composite schedule must be delivered to the ISO stack
    // alongside the renegotiation trigger, otherwise the EV sees the old bundle on re-CPD.
    EXPECT_CALL(transfer_ev_charging_schedules_callback_mock, Call(DEFAULT_EVSE_ID, DEFAULT_TX_ID, _, _, _, _));
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call(DEFAULT_EVSE_ID));

    smart_charging_hlc->handle_message(second_set);
}

// A no-op re-send (same schedule content) must NOT fire renegotiation.
TEST_F(K16RenegotiationFixture, K16FR02_IdenticalScheduleDoesNotTriggerRenegotiation) {
    auto initial = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/16.0f);
    drive_initial_handoff(initial);

    // Re-send with the SAME profile id + SAME stack_level + SAME schedule: replaces the
    // existing profile (K01FR05) but leaves composite_schedule materially unchanged.
    auto resend = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/16.0f);
    SetChargingProfileRequest set_req;
    set_req.evseId = DEFAULT_EVSE_ID;
    set_req.chargingProfile = resend;
    auto second_set = request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(set_req);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_));
    EXPECT_CALL(set_charging_profiles_callback_mock, Call);
    EXPECT_CALL(transfer_ev_charging_schedules_callback_mock, Call).Times(0);
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call).Times(0);

    smart_charging_hlc->handle_message(second_set);
}

// Without a prior HLC handoff for this EVSE (no cache entry), a SetChargingProfile that slips
// past K15.FR.17 (e.g. a station-wide TxDefaultProfile or a TxProfile on an EVSE the CS never
// called NotifyEVChargingNeeds for) must NOT fire the renegotiation callback. We exercise a
// TxDefaultProfile because K15.FR.17 and the K15.FR.07 transfer path both ignore non-TxProfiles,
// so only the K16 check could plausibly fire — and it must not, since no handoff was cached.
TEST_F(K16RenegotiationFixture, K16FR02_NoPriorHandoffDoesNotTriggerRenegotiation) {
    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    auto periods = create_charging_schedule_periods(0, DEFAULT_NR_PHASES, std::nullopt, 16.0f);
    auto schedule = create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00"));
    auto default_profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxDefaultProfile, schedule,
        /*transaction_id=*/std::nullopt, ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    SetChargingProfileRequest set_req;
    set_req.evseId = DEFAULT_EVSE_ID;
    set_req.chargingProfile = default_profile;
    auto msg = request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(set_req);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_));
    EXPECT_CALL(set_charging_profiles_callback_mock, Call);
    EXPECT_CALL(transfer_ev_charging_schedules_callback_mock, Call).Times(0);
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call).Times(0);

    smart_charging_hlc->handle_message(msg);
}

// After an accepted ClearChargingProfile the cache entry is invalidated; a subsequent
// SetChargingProfile must be treated as a fresh start — no renegotiation callback.
TEST_F(K16RenegotiationFixture, K16FR02_ClearChargingProfileInvalidatesCache) {
    auto initial = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/16.0f);
    drive_initial_handoff(initial);

    // ClearChargingProfile Accepted → invalidate cached handoff for this EVSE.
    ClearChargingProfileRequest clear_req;
    clear_req.chargingProfileId = DEFAULT_PROFILE_ID;
    auto clear_msg =
        request_to_enhanced_message<ClearChargingProfileRequest, MessageType::ClearChargingProfile>(clear_req);
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_));
    EXPECT_CALL(set_charging_profiles_callback_mock, Call);
    smart_charging_hlc->handle_message(clear_msg);
    ::testing::Mock::VerifyAndClearExpectations(&mock_dispatcher);
    ::testing::Mock::VerifyAndClearExpectations(&set_charging_profiles_callback_mock);

    // A fresh SetChargingProfile on the same tx must NOT trigger renegotiation because the
    // cache was cleared (no active HLC handoff record). After ClearChargingProfile wiped the
    // previously-accepted TxProfile and the NotifyEVChargingNeeds coordination entry is already
    // gone, K15.FR.17 rejects this profile as InvalidMessageSeq — that's correct, the CSMS
    // must re-send NotifyEVChargingNeeds. Either way the renegotiation callback must not fire.
    auto next = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/32.0f);
    SetChargingProfileRequest set_req;
    set_req.evseId = DEFAULT_EVSE_ID;
    set_req.chargingProfile = next;
    auto msg = request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(set_req);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_));
    EXPECT_CALL(set_charging_profiles_callback_mock, Call).Times(::testing::AnyNumber());
    EXPECT_CALL(transfer_ev_charging_schedules_callback_mock, Call).Times(0);
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call).Times(0);

    smart_charging_hlc->handle_message(msg);
}

// delete_transaction_tx_profiles (called at transaction end) invalidates the cache so that a
// new HLC session on the same EVSE is treated as a fresh handoff, not a K16 renegotiation.
TEST_F(K16RenegotiationFixture, K16FR02_DeleteTransactionInvalidatesCache) {
    auto initial = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/16.0f);
    drive_initial_handoff(initial);

    smart_charging_hlc->delete_transaction_tx_profiles(DEFAULT_TX_ID);

    // A new tx on the same EVSE, fresh HLC handoff. There is no K16 renegotiation callback
    // because the cache for this evse was cleared; the initial handoff path fires as normal.
    const std::string new_tx_id = "20c75ff7-74f5-44f5-9d01-f649f3ac7b78";
    evse_manager->open_transaction(DEFAULT_EVSE_ID, new_tx_id);

    NotifyEVChargingNeedsRequest needs_req;
    needs_req.evseId = DEFAULT_EVSE_ID;
    needs_req.chargingNeeds.requestedEnergyTransfer = EnergyTransferModeEnum::AC_three_phase;
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _));
    smart_charging_hlc->notify_ev_charging_needs_req(needs_req);

    auto period = create_charging_schedule_periods(0, DEFAULT_NR_PHASES, std::nullopt, 32.0f);
    auto schedule = create_charge_schedule(ChargingRateUnitEnum::A, period, ocpp::DateTime("2024-01-17T18:00:00"));
    auto new_profile = create_charging_profile(DEFAULT_PROFILE_ID + 10, ChargingProfilePurposeEnum::TxProfile, schedule,
                                               new_tx_id, ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);

    SetChargingProfileRequest set_req;
    set_req.evseId = DEFAULT_EVSE_ID;
    set_req.chargingProfile = new_profile;
    auto msg = request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(set_req);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_));
    EXPECT_CALL(set_charging_profiles_callback_mock, Call);
    EXPECT_CALL(transfer_ev_charging_schedules_callback_mock, Call(DEFAULT_EVSE_ID, new_tx_id, _, _, _, _));
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call).Times(0);

    smart_charging_hlc->handle_message(msg);
}

// Regression: if a transaction ends before the CSMS responds to NotifyEVChargingNeeds, the
// per-tx ev_schedule_state entry (and its armed 60 s timer) must be cleaned up. Otherwise
// the dangling timer can fire into a subsequent session on the same EVSE, canceling that
// session's legitimate HLC wait.
TEST_F(K16RenegotiationFixture, DeleteTransactionClearsEvScheduleState) {
    evse_manager->open_transaction(DEFAULT_EVSE_ID, DEFAULT_TX_ID);

    NotifyEVChargingNeedsRequest needs_req;
    needs_req.evseId = DEFAULT_EVSE_ID;
    needs_req.chargingNeeds.requestedEnergyTransfer = EnergyTransferModeEnum::AC_three_phase;
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _));
    smart_charging_hlc->notify_ev_charging_needs_req(needs_req);

    {
        auto handle = smart_charging_hlc->ev_schedule_state.handle();
        ASSERT_EQ(handle->count(DEFAULT_TX_ID), 1u);
    }

    smart_charging_hlc->delete_transaction_tx_profiles(DEFAULT_TX_ID);

    {
        auto handle = smart_charging_hlc->ev_schedule_state.handle();
        EXPECT_EQ(handle->count(DEFAULT_TX_ID), 0u);
    }
}

// The initial HLC handoff populates the cache; a second SetChargingProfile with the same
// content is the positive control for the "identical schedule" path. Kept separate to pin
// down the caching write-site behavior distinct from the change-detection fire-site.
TEST_F(K16RenegotiationFixture, K16FR02_InitialHandoffCachesScheduleForSecondCheck) {
    auto initial = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/16.0f);
    drive_initial_handoff(initial);

    // With the cache populated, a SECOND profile with the SAME content is treated as a no-op
    // renegotiation (silence). If the cache were not written on initial handoff, the second
    // SetChargingProfile would have no prior entry and NoPriorHandoff would apply — either way
    // the callback does not fire, so we cross-check by changing limit AFTER: callback fires.
    auto resend = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/16.0f);
    SetChargingProfileRequest set_req1;
    set_req1.evseId = DEFAULT_EVSE_ID;
    set_req1.chargingProfile = resend;
    auto second_set = request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(set_req1);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_));
    EXPECT_CALL(set_charging_profiles_callback_mock, Call);
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call).Times(0);
    smart_charging_hlc->handle_message(second_set);
    ::testing::Mock::VerifyAndClearExpectations(&mock_dispatcher);
    ::testing::Mock::VerifyAndClearExpectations(&set_charging_profiles_callback_mock);
    ::testing::Mock::VerifyAndClearExpectations(&trigger_schedule_renegotiation_callback_mock);

    // Now change the limit and confirm the cache is still in place (i.e. the initial handoff
    // *did* write it, and the identical-resend did not invalidate).
    auto changed = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/32.0f);
    SetChargingProfileRequest set_req2;
    set_req2.evseId = DEFAULT_EVSE_ID;
    set_req2.chargingProfile = changed;
    auto third_set = request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(set_req2);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_));
    EXPECT_CALL(set_charging_profiles_callback_mock, Call);
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call(DEFAULT_EVSE_ID));
    smart_charging_hlc->handle_message(third_set);
}

/// \brief K15.FR.09 / K16.FR.04 / K18.FR.09 — boundary check of the EV-returned ChargingSchedule
/// against the CSMS-sent composite schedule cached at handoff time.
class K15BoundaryCheckFixture : public K16RenegotiationFixture {
protected:
    // Build a single-period ChargingSchedule. Kept minimal so tests only exercise the
    // boundary-check axes: chargingRateUnit, limit, numberPhases, duration/window.
    ChargingSchedule make_schedule(ChargingRateUnitEnum unit, float limit,
                                   std::optional<std::int32_t> number_phases = DEFAULT_NR_PHASES,
                                   std::optional<std::int32_t> duration = std::nullopt) {
        auto periods = create_charging_schedule_periods(0, number_phases, std::nullopt, limit);
        return create_charge_schedule(unit, periods, ocpp::DateTime("2024-01-17T17:00:00"), duration);
    }
};

// In-bounds EV schedule does not trigger renegotiation.
TEST_F(K15BoundaryCheckFixture, K15FR09_InBoundsProfileDoesNotFireRenegotiation) {
    auto initial = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/32.0f);
    drive_initial_handoff(initial);

    // EV returns a schedule whose limit is strictly below the CSMS envelope.
    auto ev_schedule = make_schedule(ChargingRateUnitEnum::A, /*limit=*/16.0f);
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _));
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call).Times(0);

    smart_charging_hlc->notify_ev_charging_schedule_req(DEFAULT_EVSE_ID, /*sa_schedule_tuple_id=*/1,
                                                        /*selected_charging_schedule_id=*/std::nullopt, ev_schedule);
}

// EV schedule asks for a higher limit than the CSMS bound → renegotiation fires, but the
// NotifyEVChargingSchedule message is still dispatched to the CSMS.
TEST_F(K15BoundaryCheckFixture, K15FR09_LimitExceedsCsmsFiresRenegotiation) {
    auto initial = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/16.0f);
    drive_initial_handoff(initial);

    auto ev_schedule = make_schedule(ChargingRateUnitEnum::A, /*limit=*/32.0f);
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _));
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call(DEFAULT_EVSE_ID));

    smart_charging_hlc->notify_ev_charging_schedule_req(DEFAULT_EVSE_ID, /*sa_schedule_tuple_id=*/1,
                                                        /*selected_charging_schedule_id=*/std::nullopt, ev_schedule);
}

// chargingRateUnit mismatch (CSMS sent A, EV responds with W) → renegotiation fires.
TEST_F(K15BoundaryCheckFixture, K15FR09_UnitMismatchFiresRenegotiation) {
    auto initial = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/32.0f);
    drive_initial_handoff(initial);

    // CSMS schedule is in Amps (make_tx_profile uses A); EV returns Watts.
    auto ev_schedule = make_schedule(ChargingRateUnitEnum::W, /*limit=*/1000.0f);
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _));
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call(DEFAULT_EVSE_ID));

    smart_charging_hlc->notify_ev_charging_schedule_req(DEFAULT_EVSE_ID, /*sa_schedule_tuple_id=*/1,
                                                        /*selected_charging_schedule_id=*/std::nullopt, ev_schedule);
}

// EV schedule's duration extends past the CSMS schedule's duration → renegotiation fires.
TEST_F(K15BoundaryCheckFixture, K15FR09_OutOfWindowFiresRenegotiation) {
    // Construct an initial profile whose composite has an explicit duration. We bypass
    // make_tx_profile so we can set duration on the schedule.
    auto period = create_charging_schedule_periods(0, DEFAULT_NR_PHASES, std::nullopt, 32.0f);
    auto schedule = create_charge_schedule(ChargingRateUnitEnum::A, period, ocpp::DateTime("2024-01-17T17:00:00"),
                                           /*duration=*/3600);
    auto initial = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile, schedule,
                                           DEFAULT_TX_ID, ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    drive_initial_handoff(initial);

    // EV schedule extends to 7200s — past the CSMS 3600s window.
    auto ev_schedule = make_schedule(ChargingRateUnitEnum::A, /*limit=*/16.0f, DEFAULT_NR_PHASES, /*duration=*/7200);
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _));
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call(DEFAULT_EVSE_ID));

    smart_charging_hlc->notify_ev_charging_schedule_req(DEFAULT_EVSE_ID, /*sa_schedule_tuple_id=*/1,
                                                        /*selected_charging_schedule_id=*/std::nullopt, ev_schedule);
}

// EV flips sign on the limit (CSMS was positive / unidirectional) → renegotiation fires.
TEST_F(K15BoundaryCheckFixture, K15FR09_NegativeLimitFiresRenegotiation) {
    auto initial = make_tx_profile(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL, /*limit=*/32.0f);
    drive_initial_handoff(initial);

    auto ev_schedule = make_schedule(ChargingRateUnitEnum::A, /*limit=*/-10.0f);
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _));
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call(DEFAULT_EVSE_ID));

    smart_charging_hlc->notify_ev_charging_schedule_req(DEFAULT_EVSE_ID, /*sa_schedule_tuple_id=*/1,
                                                        /*selected_charging_schedule_id=*/std::nullopt, ev_schedule);
}

// Empty cache (no prior HLC handoff for this EVSE) → permissive fallback, no renegotiation fire.
TEST_F(K15BoundaryCheckFixture, K15FR09_EmptyCacheSkipsBoundaryCheck) {
    // No drive_initial_handoff here — last_handed_off_schedules is empty for DEFAULT_EVSE_ID.
    auto ev_schedule = make_schedule(ChargingRateUnitEnum::A, /*limit=*/9999.0f);
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _));
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call).Times(0);

    smart_charging_hlc->notify_ev_charging_schedule_req(DEFAULT_EVSE_ID, /*sa_schedule_tuple_id=*/1,
                                                        /*selected_charging_schedule_id=*/std::nullopt, ev_schedule);
}

// EvseV2G maps OCPP schedule ids into SAScheduleTupleID's 1..255 range via
// ((id - 1) % 255) + 1. For schedule ids > 255, the biased id differs from the original, so a
// raw id-compare in pick_csms_reference silently falls back to front() and bounds-checks the EV
// against the WRONG tuple. Cache two schedules with large distinct ids; expect the match to pick
// the tuple whose biased id equals the EV's reported id.
TEST_F(K15BoundaryCheckFixture, K15FR09_MultiTupleCacheMatchesBiasedId) {
    // Seed the cache directly with two schedules whose ids require biasing on the wire.
    // biased(300) = 299 % 255 + 1 = 45
    // biased(500) = 499 % 255 + 1 = 245
    ChargingSchedule cached_first = make_schedule(ChargingRateUnitEnum::A, /*limit=*/16.0f);
    cached_first.id = 300;
    ChargingSchedule cached_second = make_schedule(ChargingRateUnitEnum::A, /*limit=*/32.0f);
    cached_second.id = 500;
    {
        auto handle = smart_charging_hlc->last_handed_off_schedules.handle();
        (*handle)[DEFAULT_EVSE_ID] = {cached_first, cached_second};
    }

    // EV picks the second tuple (id=500) — reports it via the biased SAScheduleTupleID (245).
    // Its limit matches cached_second's 32 A envelope, so no renegotiation should fire.
    auto ev_schedule = make_schedule(ChargingRateUnitEnum::A, /*limit=*/32.0f);
    ev_schedule.id = 245;
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _));
    EXPECT_CALL(trigger_schedule_renegotiation_callback_mock, Call).Times(0);

    smart_charging_hlc->notify_ev_charging_schedule_req(DEFAULT_EVSE_ID, /*sa_schedule_tuple_id=*/245,
                                                        /*selected_charging_schedule_id=*/std::nullopt, ev_schedule);
}

} // namespace ocpp::v2
