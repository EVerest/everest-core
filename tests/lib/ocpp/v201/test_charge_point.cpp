// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include "comparators.hpp"
#include "everest/logging.hpp"
#include "evse_security_mock.hpp"
#include "lib/ocpp/common/database_testing_utils.hpp"
#include "ocpp/common/call_types.hpp"
#include "ocpp/common/message_queue.hpp"
#include "ocpp/v201/charge_point.hpp"
#include "ocpp/v201/ctrlr_component_variables.hpp"
#include "ocpp/v201/device_model_storage_sqlite.hpp"
#include "ocpp/v201/init_device_model_db.hpp"
#include "ocpp/v201/messages/GetCompositeSchedule.hpp"
#include "ocpp/v201/messages/SetChargingProfile.hpp"
#include "ocpp/v201/ocpp_enums.hpp"
#include "ocpp/v201/smart_charging.hpp"
#include "ocpp/v201/types.hpp"
#include "smart_charging_handler_mock.hpp"
#include "gmock/gmock.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

static const int DEFAULT_EVSE_ID = 1;
static const int DEFAULT_PROFILE_ID = 1;
static const int DEFAULT_STACK_LEVEL = 1;
static const ocpp::v201::AddChargingProfileSource DEFAULT_REQUEST_TO_ADD_PROFILE_SOURCE =
    ocpp::v201::AddChargingProfileSource::SetChargingProfile;
static const std::string TEMP_OUTPUT_PATH = "/tmp/ocpp201";
const static std::string MIGRATION_FILES_PATH = "./resources/v201/device_model_migration_files";
const static std::string SCHEMAS_PATH = "./resources/example_config/v201/component_config";
const static std::string DEVICE_MODEL_DB_IN_MEMORY_PATH = "file::memory:?cache=shared";
static const std::string DEFAULT_TX_ID = "10c75ff7-74f5-44f5-9d01-f649f3ac7b78";

namespace ocpp::v201 {

class TestChargePoint : public ChargePoint {
public:
    using ChargePoint::handle_message;
    using ChargePoint::smart_charging_handler;

    TestChargePoint(std::map<int32_t, int32_t>& evse_connector_structure,
                    std::unique_ptr<DeviceModelStorage> device_model_storage, const std::string& ocpp_main_path,
                    const std::string& core_database_path, const std::string& sql_init_path,
                    const std::string& message_log_path, const std::shared_ptr<EvseSecurity> evse_security,
                    const Callbacks& callbacks, std::shared_ptr<SmartChargingHandlerInterface> smart_charging_handler) :
        ChargePoint(evse_connector_structure, std::move(device_model_storage), ocpp_main_path, core_database_path,
                    sql_init_path, message_log_path, evse_security, callbacks) {
        this->smart_charging_handler = smart_charging_handler;
    }
};

class ChargepointTestFixtureV201 : public DatabaseTestingUtils {
public:
    ChargepointTestFixtureV201() {
    }
    ~ChargepointTestFixtureV201() {
    }

    void SetUp() override {
        charge_point->start();
    }

    void TearDown() override {
        charge_point->stop();
    }

    std::map<int32_t, int32_t> create_evse_connector_structure() {
        std::map<int32_t, int32_t> evse_connector_structure;
        evse_connector_structure.insert_or_assign(1, 1);
        evse_connector_structure.insert_or_assign(2, 1);
        return evse_connector_structure;
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

    std::shared_ptr<DatabaseHandler> create_database_handler() {
        auto database_connection = std::make_unique<common::DatabaseConnection>(fs::path("/tmp/ocpp201") / "cp.db");
        return std::make_shared<DatabaseHandler>(std::move(database_connection), MIGRATION_FILES_LOCATION_V201);
    }

    std::unique_ptr<TestChargePoint> create_charge_point() {
        std::map<int32_t, int32_t> evse_connector_structure = {{1, 1}, {2, 1}};
        std::unique_ptr<DeviceModelStorage> device_model_storage =
            std::make_unique<DeviceModelStorageSqlite>(DEVICE_MODEL_DB_IN_MEMORY_PATH);
        auto charge_point = std::make_unique<TestChargePoint>(evse_connector_structure, std::move(device_model_storage),
                                                              "", TEMP_OUTPUT_PATH, MIGRATION_FILES_LOCATION_V201,
                                                              TEMP_OUTPUT_PATH, std::make_shared<EvseSecurityMock>(),
                                                              create_callbacks_with_mocks(), smart_charging_handler);
        return charge_point;
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

    ocpp::v201::Callbacks create_callbacks_with_mocks() {
        ocpp::v201::Callbacks callbacks;

        callbacks.is_reset_allowed_callback = is_reset_allowed_callback_mock.AsStdFunction();
        callbacks.reset_callback = reset_callback_mock.AsStdFunction();
        callbacks.stop_transaction_callback = stop_transaction_callback_mock.AsStdFunction();
        callbacks.pause_charging_callback = pause_charging_callback_mock.AsStdFunction();
        callbacks.connector_effective_operative_status_changed_callback =
            connector_effective_operative_status_changed_callback_mock.AsStdFunction();
        callbacks.get_log_request_callback = get_log_request_callback_mock.AsStdFunction();
        callbacks.unlock_connector_callback = unlock_connector_callback_mock.AsStdFunction();
        callbacks.remote_start_transaction_callback = remote_start_transaction_callback_mock.AsStdFunction();
        callbacks.is_reservation_for_token_callback = is_reservation_for_token_callback_mock.AsStdFunction();
        callbacks.update_firmware_request_callback = update_firmware_request_callback_mock.AsStdFunction();
        callbacks.security_event_callback = security_event_callback_mock.AsStdFunction();
        callbacks.set_charging_profiles_callback = set_charging_profiles_callback_mock.AsStdFunction();

        return callbacks;
    }

    sqlite3* db_handle;
    std::shared_ptr<DeviceModel> device_model = create_device_model();
    std::shared_ptr<SmartChargingHandlerMock> smart_charging_handler = std::make_shared<SmartChargingHandlerMock>();
    std::unique_ptr<TestChargePoint> charge_point = create_charge_point();
    boost::uuids::random_generator uuid_generator = boost::uuids::random_generator();

    std::shared_ptr<MessageQueue<v201::MessageType>>
    create_message_queue(std::shared_ptr<DatabaseHandler>& database_handler) {
        const auto DEFAULT_MESSAGE_QUEUE_SIZE_THRESHOLD = 2E5;
        return std::make_shared<ocpp::MessageQueue<v201::MessageType>>(
            [this](json message) -> bool { return false; },
            MessageQueueConfig<v201::MessageType>{
                this->device_model->get_value<int>(ControllerComponentVariables::MessageAttempts),
                this->device_model->get_value<int>(ControllerComponentVariables::MessageAttemptInterval),
                this->device_model->get_optional_value<int>(ControllerComponentVariables::MessageQueueSizeThreshold)
                    .value_or(DEFAULT_MESSAGE_QUEUE_SIZE_THRESHOLD),
                this->device_model->get_optional_value<bool>(ControllerComponentVariables::QueueAllMessages)
                    .value_or(false),
                {},
                this->device_model->get_value<int>(ControllerComponentVariables::MessageTimeout)},
            database_handler);
    }

    void configure_callbacks_with_mocks() {
        callbacks.is_reset_allowed_callback = is_reset_allowed_callback_mock.AsStdFunction();
        callbacks.reset_callback = reset_callback_mock.AsStdFunction();
        callbacks.stop_transaction_callback = stop_transaction_callback_mock.AsStdFunction();
        callbacks.pause_charging_callback = pause_charging_callback_mock.AsStdFunction();
        callbacks.connector_effective_operative_status_changed_callback =
            connector_effective_operative_status_changed_callback_mock.AsStdFunction();
        callbacks.get_log_request_callback = get_log_request_callback_mock.AsStdFunction();
        callbacks.unlock_connector_callback = unlock_connector_callback_mock.AsStdFunction();
        callbacks.remote_start_transaction_callback = remote_start_transaction_callback_mock.AsStdFunction();
        callbacks.is_reservation_for_token_callback = is_reservation_for_token_callback_mock.AsStdFunction();
        callbacks.update_firmware_request_callback = update_firmware_request_callback_mock.AsStdFunction();
        callbacks.security_event_callback = security_event_callback_mock.AsStdFunction();
        callbacks.set_charging_profiles_callback = set_charging_profiles_callback_mock.AsStdFunction();
    }

    std::string uuid() {
        std::stringstream s;
        s << uuid_generator();
        return s.str();
    }

    template <class T> void call_to_json(json& j, const ocpp::Call<T>& call) {
        j = json::array();
        j.push_back(MessageTypeId::CALL);
        j.push_back(call.uniqueId.get());
        j.push_back(call.msg.get_type());
        j.push_back(json(call.msg));
    }

    template <class T, MessageType M> EnhancedMessage<MessageType> request_to_enhanced_message(const T& req) {
        auto message_id = uuid();
        ocpp::Call<T> call(req, message_id);
        EnhancedMessage<MessageType> enhanced_message{
            .uniqueId = message_id,
            .messageType = M,
            .messageTypeId = MessageTypeId::CALL,
        };

        call_to_json(enhanced_message.message, call);

        return enhanced_message;
    }

    testing::MockFunction<bool(const std::optional<const int32_t> evse_id, const ResetEnum& reset_type)>
        is_reset_allowed_callback_mock;
    testing::MockFunction<void(const std::optional<const int32_t> evse_id, const ResetEnum& reset_type)>
        reset_callback_mock;
    testing::MockFunction<void(const int32_t evse_id, const ReasonEnum& stop_reason)> stop_transaction_callback_mock;
    testing::MockFunction<void(const int32_t evse_id)> pause_charging_callback_mock;
    testing::MockFunction<void(const int32_t evse_id, const int32_t connector_id,
                               const OperationalStatusEnum new_status)>
        connector_effective_operative_status_changed_callback_mock;
    testing::MockFunction<GetLogResponse(const GetLogRequest& request)> get_log_request_callback_mock;
    testing::MockFunction<UnlockConnectorResponse(const int32_t evse_id, const int32_t connecor_id)>
        unlock_connector_callback_mock;
    testing::MockFunction<void(const RequestStartTransactionRequest& request, const bool authorize_remote_start)>
        remote_start_transaction_callback_mock;
    testing::MockFunction<bool(const int32_t evse_id, const CiString<36> idToken,
                               const std::optional<CiString<36>> groupIdToken)>
        is_reservation_for_token_callback_mock;
    testing::MockFunction<UpdateFirmwareResponse(const UpdateFirmwareRequest& request)>
        update_firmware_request_callback_mock;
    testing::MockFunction<void(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info)>
        security_event_callback_mock;
    testing::MockFunction<void()> set_charging_profiles_callback_mock;
    ocpp::v201::Callbacks callbacks;
};

TEST_F(ChargepointTestFixtureV201, CreateChargePoint) {
    auto evse_connector_structure = create_evse_connector_structure();
    auto database_handler = create_database_handler();
    auto evse_security = std::make_shared<EvseSecurityMock>();
    configure_callbacks_with_mocks();
    auto message_queue = create_message_queue(database_handler);

    EXPECT_NO_THROW(ocpp::v201::ChargePoint(evse_connector_structure, device_model, database_handler, message_queue,
                                            "/tmp", evse_security, callbacks));
}

TEST_F(ChargepointTestFixtureV201, CreateChargePoint_InitializeInCorrectOrder) {
    auto evse_connector_structure = create_evse_connector_structure();
    auto database_handler = create_database_handler();
    database_handler->open_connection();
    auto evse_security = std::make_shared<EvseSecurityMock>();
    configure_callbacks_with_mocks();
    auto message_queue = create_message_queue(database_handler);

    const auto cv = ControllerComponentVariables::ResumeTransactionsOnBoot;
    this->device_model->set_value(cv.component, cv.variable.value(), AttributeEnum::Actual, "false", "TEST", true);

    auto profile = create_charging_profile(DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
                                           create_charge_schedule(ChargingRateUnitEnum::A,
                                                                  create_charging_schedule_periods({0, 1, 2}),
                                                                  ocpp::DateTime("2024-01-17T17:00:00")),
                                           DEFAULT_TX_ID);
    database_handler->insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile);

    ocpp::v201::ChargePoint charge_point(evse_connector_structure, device_model, database_handler, message_queue,
                                         "/tmp", evse_security, callbacks);

    EXPECT_NO_FATAL_FAILURE(charge_point.start(BootReasonEnum::PowerUp));

    charge_point.stop();
}

TEST_F(ChargepointTestFixtureV201, CreateChargePoint_EVSEConnectorStructureDefinedBadly_ThrowsDeviceModelStorageError) {
    auto database_handler = create_database_handler();
    auto evse_security = std::make_shared<EvseSecurityMock>();
    configure_callbacks_with_mocks();
    auto message_queue = create_message_queue(database_handler);

    auto evse_connector_structure = std::map<int32_t, int32_t>();

    EXPECT_THROW(ocpp::v201::ChargePoint(evse_connector_structure, device_model, database_handler, message_queue,
                                         "/tmp", evse_security, callbacks),
                 DeviceModelStorageError);
}

TEST_F(ChargepointTestFixtureV201, CreateChargePoint_MissingDeviceModel_ThrowsInvalidArgument) {
    auto evse_connector_structure = create_evse_connector_structure();
    auto database_handler = create_database_handler();
    auto evse_security = std::make_shared<EvseSecurityMock>();
    configure_callbacks_with_mocks();
    auto message_queue = std::make_shared<ocpp::MessageQueue<v201::MessageType>>(
        [this](json message) -> bool { return false; }, MessageQueueConfig<v201::MessageType>{}, database_handler);

    EXPECT_THROW(ocpp::v201::ChargePoint(evse_connector_structure, nullptr, database_handler, message_queue, "/tmp",
                                         evse_security, callbacks),
                 std::invalid_argument);
}

TEST_F(ChargepointTestFixtureV201, CreateChargePoint_MissingDatabaseHandler_ThrowsInvalidArgument) {
    auto evse_connector_structure = create_evse_connector_structure();
    auto evse_security = std::make_shared<EvseSecurityMock>();
    configure_callbacks_with_mocks();
    auto message_queue = std::make_shared<ocpp::MessageQueue<v201::MessageType>>(
        [this](json message) -> bool { return false; }, MessageQueueConfig<v201::MessageType>{}, nullptr);

    auto database_handler = nullptr;

    EXPECT_THROW(ocpp::v201::ChargePoint(evse_connector_structure, device_model, database_handler, message_queue,
                                         "/tmp", evse_security, callbacks),
                 std::invalid_argument);
}

TEST_F(ChargepointTestFixtureV201, CreateChargePoint_CallbacksNotValid_ThrowsInvalidArgument) {
    auto evse_connector_structure = create_evse_connector_structure();
    auto database_handler = create_database_handler();
    auto evse_security = std::make_shared<EvseSecurityMock>();
    auto message_queue = create_message_queue(database_handler);

    EXPECT_THROW(ocpp::v201::ChargePoint(evse_connector_structure, device_model, database_handler, message_queue,
                                         "/tmp", evse_security, callbacks),
                 std::invalid_argument);
}

/*
 * K01.FR.02 states
 *
 *     "The CSMS MAY send a new charging profile for the EVSE that SHALL be used
 *      as a limit schedule for the EV."
 *
 * When using libocpp, a charging station is notified of a new charging profile
 * by means of the set_charging_profiles_callback. In order to ensure that a new
 * profile can be immediately "used as a limit schedule for the EV", a
 * valid set_charging_profiles_callback must be provided.
 *
 * As part of testing that K01.FR.02 is met, we provide the following tests that
 * confirm an OCPP 2.0.1 ChargePoint with smart charging enabled will only
 * consider its collection of callbacks valid if set_charging_profiles_callback
 * is provided.
 */

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfSetChargingProfilesCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.set_charging_profiles_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid());
}

/*
 * For completeness, we also test that all other callbacks are checked by
 * all_callbacks_valid.
 */

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksAreInvalidWhenNotProvided) {
    EXPECT_FALSE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksAreValidWhenAllRequiredCallbacksProvided) {
    configure_callbacks_with_mocks();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfResetIsAllowedCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.is_reset_allowed_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfResetCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.reset_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfStopTransactionCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.stop_transaction_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfPauseChargingCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.pause_charging_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201,
       K01FR02_CallbacksValidityChecksIfConnectorEffectiveOperativeStatusChangedCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.connector_effective_operative_status_changed_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfGetLogRequestCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.get_log_request_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfUnlockConnectorCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.unlock_connector_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfRemoteStartTransactionCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.remote_start_transaction_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfIsReservationForTokenCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.is_reservation_for_token_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfUpdateFirmwareRequestCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.update_firmware_request_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfSecurityEventCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.security_event_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfOptionalVariableChangedCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.variable_changed_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<void(const SetVariableData& set_variable_data)> variable_changed_callback_mock;
    callbacks.variable_changed_callback = variable_changed_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201,
       K01FR02_CallbacksValidityChecksIfOptionalVariableNetworkProfileCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.validate_network_profile_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<SetNetworkProfileStatusEnum(const int32_t configuration_slot,
                                                      const NetworkConnectionProfile& network_connection_profile)>
        validate_network_profile_callback_mock;
    callbacks.validate_network_profile_callback = validate_network_profile_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201,
       K01FR02_CallbacksValidityChecksIfOptionalConfigureNetworkConnectionProfileCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.configure_network_connection_profile_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<bool(const NetworkConnectionProfile& network_connection_profile)>
        configure_network_connection_profile_callback_mock;
    callbacks.configure_network_connection_profile_callback =
        configure_network_connection_profile_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfOptionalTimeSyncCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.time_sync_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<void(const ocpp::DateTime& currentTime)> time_sync_callback_mock;
    callbacks.time_sync_callback = time_sync_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfOptionalBootNotificationCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.boot_notification_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<void(const ocpp::v201::BootNotificationResponse& response)> boot_notification_callback_mock;
    callbacks.boot_notification_callback = boot_notification_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfOptionalOCPPMessagesCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.ocpp_messages_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<void(const std::string& message, MessageDirection direction)> ocpp_messages_callback_mock;
    callbacks.ocpp_messages_callback = ocpp_messages_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201,
       K01FR02_CallbacksValidityChecksIfOptionalCSEffectiveOperativeStatusChangedCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.cs_effective_operative_status_changed_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<void(const OperationalStatusEnum new_status)>
        cs_effective_operative_status_changed_callback_mock;
    callbacks.cs_effective_operative_status_changed_callback =
        cs_effective_operative_status_changed_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201,
       K01FR02_CallbacksValidityChecksIfOptionalEvseEffectiveOperativeStatusChangedCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.evse_effective_operative_status_changed_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<void(const int32_t evse_id, const OperationalStatusEnum new_status)>
        evse_effective_operative_status_changed_callback_mock;
    callbacks.evse_effective_operative_status_changed_callback =
        evse_effective_operative_status_changed_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201,
       K01FR02_CallbacksValidityChecksIfOptionalGetCustomerInformationCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.get_customer_information_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<std::string(const std::optional<CertificateHashDataType> customer_certificate,
                                      const std::optional<IdToken> id_token,
                                      const std::optional<CiString<64>> customer_identifier)>
        get_customer_information_callback_mock;
    callbacks.get_customer_information_callback = get_customer_information_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201,
       K01FR02_CallbacksValidityChecksIfOptionalClearCustomerInformationCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.clear_customer_information_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<std::string(const std::optional<CertificateHashDataType> customer_certificate,
                                      const std::optional<IdToken> id_token,
                                      const std::optional<CiString<64>> customer_identifier)>
        clear_customer_information_callback_mock;
    callbacks.clear_customer_information_callback = clear_customer_information_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201,
       K01FR02_CallbacksValidityChecksIfOptionalAllConnectorsUnavailableCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.all_connectors_unavailable_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<void()> all_connectors_unavailable_callback_mock;
    callbacks.all_connectors_unavailable_callback = all_connectors_unavailable_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfOptionalDataTransferCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.data_transfer_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<DataTransferResponse(const DataTransferRequest& request)> data_transfer_callback_mock;
    callbacks.data_transfer_callback = data_transfer_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01FR02_CallbacksValidityChecksIfOptionalTransactionEventCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.transaction_event_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<void(const TransactionEventRequest& transaction_event)> transaction_event_callback_mock;
    callbacks.transaction_event_callback = transaction_event_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201,
       K01FR02_CallbacksValidityChecksIfOptionalTransactionEventResponseCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.transaction_event_response_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid());

    testing::MockFunction<void(const TransactionEventRequest& transaction_event,
                               const TransactionEventResponse& transaction_event_response)>
        transaction_event_response_callback_mock;
    callbacks.transaction_event_response_callback = transaction_event_response_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid());
}

TEST_F(ChargepointTestFixtureV201, K01_SetChargingProfileRequest_ValidatesAndAddsProfile) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    SetChargingProfileRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.chargingProfile = profile;

    auto set_charging_profile_req =
        request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(req);

    EXPECT_CALL(*smart_charging_handler,
                validate_and_add_profile(profile, DEFAULT_EVSE_ID, DEFAULT_REQUEST_TO_ADD_PROFILE_SOURCE));

    charge_point->handle_message(set_charging_profile_req);
}

TEST_F(ChargepointTestFixtureV201, K01FR07_SetChargingProfileRequest_TriggersCallbackWhenValid) {
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

    ON_CALL(*smart_charging_handler, validate_and_add_profile).WillByDefault(testing::Return(accept_response));
    EXPECT_CALL(set_charging_profiles_callback_mock, Call);

    charge_point->handle_message(set_charging_profile_req);
}

TEST_F(ChargepointTestFixtureV201, K01FR07_SetChargingProfileRequest_DoesNotTriggerCallbackWhenInvalid) {
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

    ON_CALL(*smart_charging_handler, validate_and_add_profile).WillByDefault(testing::Return(reject_response));
    EXPECT_CALL(set_charging_profiles_callback_mock, Call).Times(0);

    charge_point->handle_message(set_charging_profile_req);
}

TEST_F(ChargepointTestFixtureV201, K01FR22_SetChargingProfileRequest_RejectsChargingStationExternalConstraints) {
    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::ChargingStationExternalConstraints,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    SetChargingProfileRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.chargingProfile = profile;

    auto set_charging_profile_req =
        request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(req);

    EXPECT_CALL(*smart_charging_handler, validate_and_add_profile).Times(0);
    EXPECT_CALL(set_charging_profiles_callback_mock, Call).Times(0);

    charge_point->handle_message(set_charging_profile_req);
}

TEST_F(ChargepointTestFixtureV201, K01FR29_SmartChargingCtrlrAvailableIsFalse_RespondsCallError) {
    auto evse_connector_structure = create_evse_connector_structure();
    auto database_handler = create_database_handler();
    auto evse_security = std::make_shared<EvseSecurityMock>();
    configure_callbacks_with_mocks();

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

    EXPECT_CALL(*smart_charging_handler, validate_and_add_profile).Times(0);

    charge_point->handle_message(set_charging_profile_req);
}

TEST_F(ChargepointTestFixtureV201,
       K05FR05_RequestStartTransactionRequest_SmartChargingCtrlrEnabledTrue_ValidatesTxProfiles) {
    const auto cv = ControllerComponentVariables::SmartChargingCtrlrEnabled;
    this->device_model->set_value(cv.component, cv.variable.value(), AttributeEnum::Actual, "true", "TEST", true);

    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    RequestStartTransactionRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.idToken = IdToken{.idToken = "Local", .type = IdTokenEnum::Local};
    req.chargingProfile = profile;

    auto start_transaction_req =
        request_to_enhanced_message<RequestStartTransactionRequest, MessageType::RequestStartTransaction>(req);

    EXPECT_CALL(*smart_charging_handler, validate_and_add_profile).Times(1);

    charge_point->handle_message(start_transaction_req);
}

TEST_F(ChargepointTestFixtureV201, K01FR29_SmartChargingCtrlrAvailableIsTrue_CallsValidateAndAddProfile) {
    auto evse_connector_structure = create_evse_connector_structure();
    auto database_handler = create_database_handler();
    auto evse_security = std::make_shared<EvseSecurityMock>();
    configure_callbacks_with_mocks();

    const auto cv = ControllerComponentVariables::SmartChargingCtrlrAvailable;
    this->device_model->set_value(cv.component, cv.variable.value(), AttributeEnum::Actual, "true", "TEST", true);

    auto periods = create_charging_schedule_periods({0, 1, 2});
    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    SetChargingProfileRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.chargingProfile = profile;

    auto set_charging_profile_req =
        request_to_enhanced_message<SetChargingProfileRequest, MessageType::SetChargingProfile>(req);

    EXPECT_CALL(*smart_charging_handler, validate_and_add_profile).Times(1);

    charge_point->handle_message(set_charging_profile_req);
}

TEST_F(ChargepointTestFixtureV201, K08_GetCompositeSchedule_CallsCalculateGetCompositeSchedule) {
    GetCompositeScheduleRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.chargingRateUnit = ChargingRateUnitEnum::W;

    auto get_composite_schedule_req =
        request_to_enhanced_message<GetCompositeScheduleRequest, MessageType::GetCompositeSchedule>(req);

    EXPECT_CALL(*smart_charging_handler, calculate_composite_schedule(testing::_, testing::_, testing::_,
                                                                      DEFAULT_EVSE_ID, req.chargingRateUnit));

    charge_point->handle_message(get_composite_schedule_req);
}

TEST_F(ChargepointTestFixtureV201, K08_GetCompositeSchedule_CallsCalculateGetCompositeScheduleWithValidProfiles) {
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

    ON_CALL(*smart_charging_handler, get_valid_profiles(DEFAULT_EVSE_ID)).WillByDefault(testing::Return(profiles));
    EXPECT_CALL(*smart_charging_handler,
                calculate_composite_schedule(profiles, testing::_, testing::_, DEFAULT_EVSE_ID, req.chargingRateUnit));

    charge_point->handle_message(get_composite_schedule_req);
}

TEST_F(ChargepointTestFixtureV201, K08FR05_GetCompositeSchedule_DoesNotCalculateCompositeScheduleForNonexistentEVSE) {
    GetCompositeScheduleRequest req;
    req.evseId = DEFAULT_EVSE_ID + 3;
    req.chargingRateUnit = ChargingRateUnitEnum::W;

    auto get_composite_schedule_req =
        request_to_enhanced_message<GetCompositeScheduleRequest, MessageType::GetCompositeSchedule>(req);

    EXPECT_CALL(*smart_charging_handler, get_valid_profiles(testing::_)).Times(0);
    EXPECT_CALL(*smart_charging_handler,
                calculate_composite_schedule(testing::_, testing::_, testing::_, testing::_, testing::_))
        .Times(0);

    charge_point->handle_message(get_composite_schedule_req);
}

TEST_F(ChargepointTestFixtureV201,
       K08FR07_GetCompositeSchedule_DoesNotCalculateCompositeScheduleForIncorrectChargingRateUnit) {
    GetCompositeScheduleRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.chargingRateUnit = ChargingRateUnitEnum::W;

    auto get_composite_schedule_req =
        request_to_enhanced_message<GetCompositeScheduleRequest, MessageType::GetCompositeSchedule>(req);

    const auto& charging_rate_unit_cv = ControllerComponentVariables::ChargingScheduleChargingRateUnit;
    device_model->set_value(charging_rate_unit_cv.component, charging_rate_unit_cv.variable.value(),
                            AttributeEnum::Actual, "A", "test", true);

    EXPECT_CALL(*smart_charging_handler, get_valid_profiles(testing::_)).Times(0);
    EXPECT_CALL(*smart_charging_handler,
                calculate_composite_schedule(testing::_, testing::_, testing::_, testing::_, testing::_))
        .Times(0);

    charge_point->handle_message(get_composite_schedule_req);
}

TEST_F(ChargepointTestFixtureV201,
       K05FR04_RequestStartTransactionRequest_SmartChargingCtrlrEnabledFalse_DoesNotValidateTxProfiles) {
    const auto cv = ControllerComponentVariables::SmartChargingCtrlrEnabled;
    this->device_model->set_value(cv.component, cv.variable.value(), AttributeEnum::Actual, "false", "TEST", true);

    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    RequestStartTransactionRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.idToken = IdToken{.idToken = "Local", .type = IdTokenEnum::Local};
    req.chargingProfile = profile;

    auto start_transaction_req =
        request_to_enhanced_message<RequestStartTransactionRequest, MessageType::RequestStartTransaction>(req);

    EXPECT_CALL(*smart_charging_handler, validate_and_add_profile).Times(0);

    charge_point->handle_message(start_transaction_req);
}

TEST_F(ChargepointTestFixtureV201, K02FR05_TransactionEnds_WillDeleteTxProfilesWithTransactionID) {
    auto database_handler = create_database_handler();
    database_handler->open_connection();
    const auto cv = ControllerComponentVariables::ResumeTransactionsOnBoot;
    this->device_model->set_value(cv.component, cv.variable.value(), AttributeEnum::Actual, "true", "TEST", true);
    int32_t connector_id = 1;
    std::string session_id = "some-session-id";
    ocpp::DateTime timestamp("2024-01-17T17:00:00");

    charge_point->on_transaction_started(DEFAULT_EVSE_ID, connector_id, session_id, timestamp,
                                         ocpp::v201::TriggerReasonEnum::Authorized, MeterValue(), {}, {}, {}, {},
                                         ChargingStateEnum::EVConnected);
    auto transaction = database_handler->transaction_get(DEFAULT_EVSE_ID);
    ASSERT_THAT(transaction, testing::NotNull());

    EXPECT_CALL(*smart_charging_handler,
                delete_transaction_tx_profiles(transaction->get_transaction().transactionId.get()));
    charge_point->on_transaction_finished(DEFAULT_EVSE_ID, timestamp, MeterValue(), ReasonEnum::StoppedByEV,
                                          TriggerReasonEnum::StopAuthorized, {}, {}, ChargingStateEnum::EVConnected);
}

} // namespace ocpp::v201
