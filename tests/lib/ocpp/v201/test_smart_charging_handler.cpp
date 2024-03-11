#include "date/tz.h"
#include "ocpp/v201/ctrlr_component_variables.hpp"
#include "ocpp/v201/device_model_storage_sqlite.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include <component_state_manager_mock.hpp>
#include <device_model_storage_mock.hpp>
#include <evse_security_mock.hpp>
#include <ocpp/common/call_types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/evse.hpp>
#include <ocpp/v201/smart_charging.hpp>
#include <optional>

#include <sstream>

namespace ocpp::v201 {

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

    ChargingProfile create_tx_profile(ChargingSchedule charging_schedule, std::string transaction_id,
                                      int stack_level = 1) {
        auto charging_profile_id = 1;
        auto charging_profile_purpose = ChargingProfilePurposeEnum::TxProfile;
        auto charging_profile_kind = ChargingProfileKindEnum::Absolute;
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
        auto charging_profile_id = 1;
        auto stack_level = 1;
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
        return SmartChargingHandler();
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

    // Default values used within the tests
    std::map<int32_t, std::unique_ptr<Evse>> evses;
    std::shared_ptr<DatabaseHandler> database_handler;

    const int evse_id = 1;
    bool ignore_no_transaction = true;
    const int profile_max_stack_level = 1;
    const int max_charging_profiles_installed = 1;
    const int charging_schedule_max_periods = 1;
    DeviceModel device_model = create_device_model();
    SmartChargingHandler handler = create_smart_charging_handler();
    boost::uuids::random_generator uuid_generator = boost::uuids::random_generator();
};

TEST_F(ChargepointTestFixtureV201, K01FR03_IfTxProfileIsMissingTransactionId_ThenProfileIsInvalid) {
    create_evse_with_id(evse_id);
    auto profile = create_tx_profile_with_missing_transaction_id(create_charge_schedule(ChargingRateUnitEnum::A));
    auto sut = handler.validate_tx_profile(profile, *evses[evse_id]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileMissingTransactionId));
}

TEST_F(ChargepointTestFixtureV201, K01FR16_IfTxProfileHasEvseIdNotGreaterThanZero_ThenProfileIsInvalid) {
    auto wrong_evse_id = 0;
    create_evse_with_id(0);
    auto profile = create_tx_profile(create_charge_schedule(ChargingRateUnitEnum::A), uuid());
    auto sut = handler.validate_tx_profile(profile, *evses[wrong_evse_id]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileEvseIdNotGreaterThanZero));
}

TEST_F(ChargepointTestFixtureV201, K01FR33_IfTxProfileTransactionIsNotOnEvse_ThenProfileIsInvalid) {
    create_evse_with_id(evse_id);
    open_evse_transaction(evse_id, "wrong transaction id");
    auto profile = create_tx_profile(create_charge_schedule(ChargingRateUnitEnum::A), uuid());
    auto sut = handler.validate_tx_profile(profile, *evses[evse_id]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileTransactionNotOnEvse));
}

TEST_F(ChargepointTestFixtureV201, K01FR09_IfTxProfileEvseHasNoActiveTransaction_ThenProfileIsInvalid) {
    auto connector_id = 1;
    auto meter_start = MeterValue();
    auto id_token = IdToken();
    auto date_time = ocpp::DateTime("2024-01-17T17:00:00");
    create_evse_with_id(evse_id);
    auto profile = create_tx_profile(create_charge_schedule(ChargingRateUnitEnum::A), uuid());
    auto sut = handler.validate_tx_profile(profile, *evses[evse_id]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileEvseHasNoActiveTransaction));
}

TEST_F(ChargepointTestFixtureV201,
       K01FR06_IfTxProfileHasSameTransactionAndStackLevelAsAnotherTxProfile_ThenProfileIsInvalid) {
    create_evse_with_id(evse_id);
    std::string transaction_id = uuid();
    open_evse_transaction(evse_id, transaction_id);

    auto same_stack_level = 42;
    auto profile_1 =
        create_tx_profile(create_charge_schedule(ChargingRateUnitEnum::A), transaction_id, same_stack_level);
    auto profile_2 =
        create_tx_profile(create_charge_schedule(ChargingRateUnitEnum::A), transaction_id, same_stack_level);
    handler.add_profile(profile_2);
    auto sut = handler.validate_tx_profile(profile_1, *evses[evse_id]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::TxProfileConflictingStackLevel));
}

TEST_F(ChargepointTestFixtureV201,
       K01FR06_IfTxProfileHasDifferentTransactionButSameStackLevelAsAnotherTxProfile_ThenProfileIsValid) {
    create_evse_with_id(evse_id);
    std::string transaction_id = uuid();
    std::string different_transaction_id = uuid();
    open_evse_transaction(evse_id, transaction_id);

    auto same_stack_level = 42;
    auto profile_1 =
        create_tx_profile(create_charge_schedule(ChargingRateUnitEnum::A), transaction_id, same_stack_level);
    auto profile_2 =
        create_tx_profile(create_charge_schedule(ChargingRateUnitEnum::A), different_transaction_id, same_stack_level);
    handler.add_profile(profile_2);
    auto sut = handler.validate_tx_profile(profile_1, *evses[evse_id]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

TEST_F(ChargepointTestFixtureV201,
       K01FR06_IfTxProfileHasSameTransactionButDifferentStackLevelAsAnotherTxProfile_ThenProfileIsValid) {
    create_evse_with_id(evse_id);
    std::string same_transaction_id = uuid();
    open_evse_transaction(evse_id, same_transaction_id);

    auto stack_level_1 = 42;
    auto stack_level_2 = 43;
    auto profile_1 =
        create_tx_profile(create_charge_schedule(ChargingRateUnitEnum::A), same_transaction_id, stack_level_1);
    auto profile_2 =
        create_tx_profile(create_charge_schedule(ChargingRateUnitEnum::A), same_transaction_id, stack_level_2);
    handler.add_profile(profile_2);
    auto sut = handler.validate_tx_profile(profile_1, *evses[evse_id]);

    EXPECT_THAT(sut, testing::Eq(ProfileValidationResultEnum::Valid));
}

} // namespace ocpp::v201
