#include "date/tz.h"
#include "ocpp/v201/ctrlr_component_variables.hpp"
#include "ocpp/v201/device_model_storage_sqlite.hpp"
#include "ocpp/v201/ocpp_types.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include <component_state_manager_mock.hpp>
#include <device_model_storage_mock.hpp>
#include <evse_mock.hpp>
#include <evse_security_mock.hpp>
#include <ocpp/common/call_types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/evse.hpp>
#include <ocpp/v201/smart_charging.hpp>
#include <optional>

#include <sstream>
#include <vector>

namespace ocpp::v201 {

static const int STATION_WIDE_ID = 0;
static const int DEFAULT_EVSE_ID = 1;
static const int DEFAULT_PROFILE_ID = 1;
static const int DEFAULT_STACK_LEVEL = 1;

class ChargepointTestFixtureV201 : public testing::Test {
protected:
    void SetUp() override {
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
                            ChargingSchedule charging_schedule, std::string transaction_id,
                            ChargingProfileKindEnum charging_profile_kind = ChargingProfileKindEnum::Absolute,
                            int stack_level = DEFAULT_STACK_LEVEL) {
        auto recurrency_kind = RecurrencyKindEnum::Daily;
        std::vector<ChargingSchedule> charging_schedules = {charging_schedule};
        return ChargingProfile{.id = charging_profile_id,
                               .stackLevel = stack_level,
                               .chargingProfilePurpose = charging_profile_purpose,
                               .chargingProfileKind = charging_profile_kind,
                               .chargingSchedule = charging_schedules,
                               .customData = {},
                               .recurrencyKind = recurrency_kind,
                               .validFrom = {},
                               .validTo = {},
                               .transactionId = transaction_id};
    }

    ChargingProfile create_tx_profile_with_missing_transaction_id(ChargingSchedule charging_schedule) {
        auto charging_profile_id = DEFAULT_PROFILE_ID;
        auto stack_level = DEFAULT_STACK_LEVEL;
        auto charging_profile_purpose = ChargingProfilePurposeEnum::TxProfile;
        auto charging_profile_kind = ChargingProfileKindEnum::Absolute;
        auto recurrency_kind = RecurrencyKindEnum::Daily;
        std::vector<ChargingSchedule> charging_schedules = {charging_schedule};
        return ChargingProfile{
            charging_profile_id,
            stack_level,
            charging_profile_purpose,
            charging_profile_kind,
            charging_schedules,
            {}, // transactionId
            recurrency_kind,
            {}, // validFrom
            {}  // validTo
        };
    }

    DeviceModel create_device_model() {
        std::unique_ptr<DeviceModelStorageMock> storage_mock =
            std::make_unique<testing::NiceMock<DeviceModelStorageMock>>();
        ON_CALL(*storage_mock, get_device_model).WillByDefault(testing::Return(DeviceModelMap()));
        return DeviceModel(std::move(storage_mock));
    }

    void create_evse_with_id(int id) {
        testing::MockFunction<void(const MeterValue& meter_value, const Transaction& transaction, const int32_t seq_no,
                                   const std::optional<int32_t> reservation_id)>
            transaction_meter_value_req_mock;
        testing::MockFunction<void()> pause_charging_callback_mock;
        auto e1 = std::make_unique<Evse>(
            id, 1, device_model, database_handler, std::make_shared<ComponentStateManagerMock>(),
            transaction_meter_value_req_mock.AsStdFunction(), pause_charging_callback_mock.AsStdFunction());
        evses[id] = std::move(e1);
    }

    SmartChargingHandler create_smart_charging_handler() {
        return SmartChargingHandler(evses);
    }

    std::string uuid() {
        std::stringstream s;
        s << uuid_generator();
        return s.str();
    }

    void open_evse_transaction(int evse_id, std::string transaction_id) {
        auto connector_id = 1;
        auto meter_start = MeterValue();
        auto id_token = IdToken();
        auto date_time = ocpp::DateTime("2024-01-17T17:00:00");
        evses[evse_id]->open_transaction(
            transaction_id, connector_id, date_time, meter_start, id_token, {}, {},
            std::chrono::seconds(static_cast<int64_t>(1)), std::chrono::seconds(static_cast<int64_t>(1)),
            std::chrono::seconds(static_cast<int64_t>(1)), std::chrono::seconds(static_cast<int64_t>(1)));
    }

    void install_profile_on_evse(int evse_id, int profile_id) {
        if (evse_id != STATION_WIDE_ID) {
            create_evse_with_id(evse_id);
        }
        auto existing_profile = create_charging_profile(profile_id, ChargingProfilePurposeEnum::TxDefaultProfile,
                                                        create_charge_schedule(ChargingRateUnitEnum::A), uuid());
        handler.add_profile(evse_id, existing_profile);
    }

    // Default values used within the tests
    std::map<int32_t, std::unique_ptr<EvseInterface>> evses;
    std::shared_ptr<DatabaseHandler> database_handler;

    bool ignore_no_transaction = true;
    DeviceModel device_model = create_device_model();
    SmartChargingHandler handler = create_smart_charging_handler();
    boost::uuids::random_generator uuid_generator = boost::uuids::random_generator();
};

TEST_F(ChargepointTestFixtureV201, K01FR03_IfTxProfileIsMissingTransactionId_ThenProfileIsInvalid) {
    create_evse_with_id(DEFAULT_EVSE_ID);
    auto profile = create_tx_profile_with_missing_transaction_id(create_charge_schedule(ChargingRateUnitEnum::A));
    auto sut = handler.validate_tx_profile(profile, *evses[DEFAULT_EVSE_ID]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileMissingTransactionId));
}

TEST_F(ChargepointTestFixtureV201, K01FR16_IfTxProfileHasEvseIdNotGreaterThanZero_ThenProfileIsInvalid) {
    auto wrong_evse_id = STATION_WIDE_ID;
    create_evse_with_id(wrong_evse_id);
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), uuid());
    auto sut = handler.validate_tx_profile(profile, *evses[wrong_evse_id]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileEvseIdNotGreaterThanZero));
}

TEST_F(ChargepointTestFixtureV201, K01FR33_IfTxProfileTransactionIsNotOnEvse_ThenProfileIsInvalid) {
    create_evse_with_id(DEFAULT_EVSE_ID);
    open_evse_transaction(DEFAULT_EVSE_ID, "wrong transaction id");
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), uuid());
    auto sut = handler.validate_tx_profile(profile, *evses[DEFAULT_EVSE_ID]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileTransactionNotOnEvse));
}

TEST_F(ChargepointTestFixtureV201, K01FR09_IfTxProfileEvseHasNoActiveTransaction_ThenProfileIsInvalid) {
    auto connector_id = 1;
    auto meter_start = MeterValue();
    auto id_token = IdToken();
    auto date_time = ocpp::DateTime("2024-01-17T17:00:00");
    create_evse_with_id(DEFAULT_EVSE_ID);
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), uuid());
    auto sut = handler.validate_tx_profile(profile, *evses[DEFAULT_EVSE_ID]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileEvseHasNoActiveTransaction));
}

TEST_F(ChargepointTestFixtureV201,
       K01FR06_IfTxProfileHasSameTransactionAndStackLevelAsAnotherTxProfile_ThenProfileIsInvalid) {
    create_evse_with_id(DEFAULT_EVSE_ID);
    std::string transaction_id = uuid();
    open_evse_transaction(DEFAULT_EVSE_ID, transaction_id);

    auto same_stack_level = 42;
    auto profile_1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), transaction_id,
                                             ChargingProfileKindEnum::Absolute, same_stack_level);
    auto profile_2 = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), transaction_id,
                                             ChargingProfileKindEnum::Absolute, same_stack_level);
    handler.add_profile(DEFAULT_EVSE_ID, profile_2);
    auto sut = handler.validate_tx_profile(profile_1, *evses[DEFAULT_EVSE_ID]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileConflictingStackLevel));
}

TEST_F(ChargepointTestFixtureV201,
       K01FR06_IfTxProfileHasDifferentTransactionButSameStackLevelAsAnotherTxProfile_ThenProfileIsValid) {
    create_evse_with_id(DEFAULT_EVSE_ID);
    std::string transaction_id = uuid();
    std::string different_transaction_id = uuid();
    open_evse_transaction(DEFAULT_EVSE_ID, transaction_id);

    auto same_stack_level = 42;
    auto profile_1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), transaction_id,
                                             ChargingProfileKindEnum::Absolute, same_stack_level);
    auto profile_2 = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), different_transaction_id,
                                             ChargingProfileKindEnum::Absolute, same_stack_level);
    handler.add_profile(DEFAULT_EVSE_ID, profile_2);
    auto sut = handler.validate_tx_profile(profile_1, *evses[DEFAULT_EVSE_ID]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(ChargepointTestFixtureV201,
       K01FR06_IfTxProfileHasSameTransactionButDifferentStackLevelAsAnotherTxProfile_ThenProfileIsValid) {
    create_evse_with_id(DEFAULT_EVSE_ID);
    std::string same_transaction_id = uuid();
    open_evse_transaction(DEFAULT_EVSE_ID, same_transaction_id);

    auto stack_level_1 = 42;
    auto stack_level_2 = 43;

    auto profile_1 = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), same_transaction_id,
                                             ChargingProfileKindEnum::Absolute, stack_level_1);
    auto profile_2 = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxProfile,
                                             create_charge_schedule(ChargingRateUnitEnum::A), same_transaction_id,
                                             ChargingProfileKindEnum::Absolute, stack_level_2);

    handler.add_profile(DEFAULT_EVSE_ID, profile_2);
    auto sut = handler.validate_tx_profile(profile_1, *evses[DEFAULT_EVSE_ID]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(ChargepointTestFixtureV201, K01FR19_NumberPhasesOtherThan1AndPhaseToUseSet_ThenProfileInvalid) {
    auto periods = create_charging_schedule_periods_with_phases(0, 0, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), uuid(),
        ChargingProfileKindEnum::Absolute, 1);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodInvalidPhaseToUse));
}

TEST_F(ChargepointTestFixtureV201, K01_IfChargingSchedulePeriodsAreMissing_ThenProfileIsInvalid) {
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), uuid());

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileNoChargingSchedulePeriods));
}

TEST_F(ChargepointTestFixtureV201, K01FR31_IfStartPeriodOfFirstChargingSchedulePeriodIsNotZero_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(1);
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), uuid());

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileFirstStartScheduleIsNotZero));
}

TEST_F(ChargepointTestFixtureV201, K01FR35_IfChargingSchedulePeriodsAreNotInChonologicalOrder_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods({0, 2, 1});
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), uuid());

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodsOutOfOrder));
}

TEST_F(ChargepointTestFixtureV201,
       K01FR40_IfChargingProfileKindIsAbsoluteAndStartScheduleDoesNotExist_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), uuid(),
                                           ChargingProfileKindEnum::Absolute, 1);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileMissingRequiredStartSchedule));
}

TEST_F(ChargepointTestFixtureV201,
       K01FR40_IfChargingProfileKindIsRecurringAndStartScheduleDoesNotExist_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A, periods), uuid(),
                                           ChargingProfileKindEnum::Recurring, 1);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileMissingRequiredStartSchedule));
}

TEST_F(ChargepointTestFixtureV201,
       K01FR41_IfChargingProfileKindIsRelativeAndStartScheduleDoesExist_ThenProfileIsInvalid) {
    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), uuid(),
        ChargingProfileKindEnum::Relative, 1);

    auto sut = handler.validate_profile_schedules(profile);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingProfileExtraneousStartSchedule));
}

TEST_F(ChargepointTestFixtureV201, K01FR28_WhenEvseDoesNotExistThenReject) {
    auto sut = handler.validate_evse_exists(DEFAULT_EVSE_ID);
    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::EvseDoesNotExist));
}

TEST_F(ChargepointTestFixtureV201, K01FR28_WhenEvseDoesExistThenAccept) {
    create_evse_with_id(DEFAULT_EVSE_ID);
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
class ChargepointTestFixtureV201_FR52
    : public ChargepointTestFixtureV201,
      public ::testing::WithParamInterface<std::tuple<int, int, ProfileValidationResultEnum>> {};

INSTANTIATE_TEST_SUITE_P(TxDefaultProfileValidationV201_Param_Test_Instantiate_FR52, ChargepointTestFixtureV201_FR52,
                         testing::Values(std::make_tuple(DEFAULT_PROFILE_ID + 1, DEFAULT_STACK_LEVEL,
                                                         ProfileValidationResultEnum::DuplicateTxDefaultProfileFound),
                                         std::make_tuple(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL,
                                                         ProfileValidationResultEnum::Valid),
                                         std::make_tuple(DEFAULT_PROFILE_ID + 1, DEFAULT_STACK_LEVEL + 1,
                                                         ProfileValidationResultEnum::Valid)));

TEST_P(ChargepointTestFixtureV201_FR52, K01FR52_TxDefaultProfileValidationV201Tests) {
    auto [added_profile_id, added_stack_level, expected] = GetParam();
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto profile = create_charging_profile(added_profile_id, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                           ChargingProfileKindEnum::Absolute, added_stack_level);
    auto sut = handler.validate_tx_default_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(expected));
}

class ChargepointTestFixtureV201_FR53
    : public ChargepointTestFixtureV201,
      public ::testing::WithParamInterface<std::tuple<int, int, ProfileValidationResultEnum>> {};

INSTANTIATE_TEST_SUITE_P(TxDefaultProfileValidationV201_Param_Test_Instantiate_FR53, ChargepointTestFixtureV201_FR53,
                         testing::Values(std::make_tuple(DEFAULT_PROFILE_ID + 1, DEFAULT_STACK_LEVEL,
                                                         ProfileValidationResultEnum::DuplicateTxDefaultProfileFound),
                                         std::make_tuple(DEFAULT_PROFILE_ID, DEFAULT_STACK_LEVEL,
                                                         ProfileValidationResultEnum::Valid),
                                         std::make_tuple(DEFAULT_PROFILE_ID + 1, DEFAULT_STACK_LEVEL + 1,
                                                         ProfileValidationResultEnum::Valid)));

TEST_P(ChargepointTestFixtureV201_FR53, K01FR53_TxDefaultProfileValidationV201Tests) {
    auto [added_profile_id, added_stack_level, expected] = GetParam();
    install_profile_on_evse(STATION_WIDE_ID, DEFAULT_PROFILE_ID);
    create_evse_with_id(DEFAULT_EVSE_ID);

    auto profile = create_charging_profile(added_profile_id, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                           ChargingProfileKindEnum::Absolute, added_stack_level);
    auto sut = handler.validate_tx_default_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(expected));
}

TEST_F(ChargepointTestFixtureV201, K01FR52_TxDefaultProfileValidIfAppliedToWholeSystemAgain) {
    install_profile_on_evse(STATION_WIDE_ID, DEFAULT_PROFILE_ID);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                           ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto sut = handler.validate_tx_default_profile(profile, STATION_WIDE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(ChargepointTestFixtureV201, K01FR53_TxDefaultProfileValidIfAppliedToExistingEvseAgain) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                           ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto sut = handler.validate_tx_default_profile(profile, DEFAULT_EVSE_ID);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(ChargepointTestFixtureV201, K01FR53_TxDefaultProfileValidIfAppliedToDifferentEvse) {
    install_profile_on_evse(DEFAULT_EVSE_ID, DEFAULT_PROFILE_ID);
    create_evse_with_id(DEFAULT_EVSE_ID + 1);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID + 1, ChargingProfilePurposeEnum::TxDefaultProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A), uuid(),
                                           ChargingProfileKindEnum::Absolute, DEFAULT_STACK_LEVEL);
    auto sut = handler.validate_tx_default_profile(profile, DEFAULT_EVSE_ID + 1);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(ChargepointTestFixtureV201, K01FR44_IfNumberPhasesProvidedForDCEVSE_ThenProfileIsInvalid) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::DC));

    auto periods = create_charging_schedule_periods(0, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), uuid());

    auto sut = handler.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues));
}

TEST_F(ChargepointTestFixtureV201, K01FR44_IfPhaseToUseProvidedForDCEVSE_ThenProfileIsInvalid) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::DC));

    auto periods = create_charging_schedule_periods(0, 1, 1);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), uuid());

    auto sut = handler.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodExtraneousPhaseValues));
}

TEST_F(ChargepointTestFixtureV201, K01FR45_IfNumberPhasesGreaterThanMaxNumberPhasesForACEVSE_ThenProfileIsInvalid) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::AC));

    auto periods = create_charging_schedule_periods(0, 4);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), uuid());

    auto sut = handler.validate_profile_schedules(profile, &mock_evse);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::ChargingSchedulePeriodUnsupportedNumberPhases));
}

TEST_F(ChargepointTestFixtureV201, K01FR49_IfNumberPhasesMissingForACEVSE_ThenSetNumberPhasesToThree) {
    auto mock_evse = testing::NiceMock<EvseMock>();
    ON_CALL(mock_evse, get_current_phase_type).WillByDefault(testing::Return(CurrentPhaseType::AC));

    auto periods = create_charging_schedule_periods(0);
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), uuid());

    auto sut = handler.validate_profile_schedules(profile, &mock_evse);

    auto numberPhases = profile.chargingSchedule[0].chargingSchedulePeriod[0].numberPhases;

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
    EXPECT_THAT(numberPhases, testing::Eq(3));
}

} // namespace ocpp::v201
