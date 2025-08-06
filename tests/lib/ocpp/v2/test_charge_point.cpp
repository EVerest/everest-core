// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "comparators.hpp"
#include "device_model_test_helper.hpp"
#include "everest/logging.hpp"
#include "evse_security_mock.hpp"
#include "lib/ocpp/common/database_testing_utils.hpp"
#include "mocks/smart_charging_mock.hpp"
#include "ocpp/common/call_types.hpp"
#include "ocpp/common/message_queue.hpp"
#include "ocpp/v2/charge_point.hpp"
#include "ocpp/v2/ctrlr_component_variables.hpp"
#include "ocpp/v2/device_model_storage_sqlite.hpp"
#include "ocpp/v2/init_device_model_db.hpp"
#include "ocpp/v2/ocpp_enums.hpp"
#include "ocpp/v2/types.hpp"
#include "smart_charging_test_utils.hpp"

#include "gmock/gmock.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

static const ocpp::v2::AddChargingProfileSource DEFAULT_REQUEST_TO_ADD_PROFILE_SOURCE =
    ocpp::v2::AddChargingProfileSource::SetChargingProfile;
static const std::string TEMP_OUTPUT_PATH = "/tmp/ocpp201";
static const std::string DEFAULT_TX_ID = "10c75ff7-74f5-44f5-9d01-f649f3ac7b78";

namespace ocpp::v2 {

class ChargePointCommonTestFixtureV2 : public DatabaseTestingUtils {
public:
    ChargePointCommonTestFixtureV2() :
        device_model(create_device_model()), evse_connector_structure(create_evse_connector_structure()) {
    }
    ~ChargePointCommonTestFixtureV2() {
    }

    std::map<int32_t, int32_t> create_evse_connector_structure() {
        std::map<int32_t, int32_t> evse_connector_structure;
        evse_connector_structure.insert_or_assign(1, 1);
        evse_connector_structure.insert_or_assign(2, 1);
        return evse_connector_structure;
    }

    void create_device_model_db(const std::string& path) {
        InitDeviceModelDb db(path, MIGRATION_FILES_PATH);
        auto component_configs = get_all_component_configs(CONFIG_PATH);
        std::vector<DeviceModelVariable> variables;
        DeviceModelVariable v2x_enabled;
        v2x_enabled.name = "Enabled";
        VariableCharacteristics v2x_enabled_characteristics;
        v2x_enabled_characteristics.dataType = DataEnum::boolean;
        v2x_enabled_characteristics.supportsMonitoring = false;
        v2x_enabled.characteristics = v2x_enabled_characteristics;
        DbVariableAttribute v2x_enabled_attribute;
        v2x_enabled_attribute.variable_attribute.mutability = MutabilityEnum::ReadWrite;
        v2x_enabled_attribute.variable_attribute.value = "false";
        v2x_enabled_attribute.variable_attribute.type = AttributeEnum::Actual;
        v2x_enabled.attributes = std::vector<DbVariableAttribute>{v2x_enabled_attribute};
        variables.push_back(v2x_enabled);
        DeviceModelVariable v2x_available;
        v2x_available.name = "Available";
        VariableCharacteristics v2x_available_characteristics;
        v2x_available_characteristics.dataType = DataEnum::boolean;
        v2x_available_characteristics.supportsMonitoring = false;
        v2x_available.characteristics = v2x_available_characteristics;
        DbVariableAttribute v2x_available_attribute;
        v2x_available_attribute.variable_attribute.mutability =
            MutabilityEnum::ReadWrite; // Made readwrite to be able to modify during Tests
        v2x_available_attribute.variable_attribute.value = "false";
        v2x_available_attribute.variable_attribute.type = AttributeEnum::Actual;
        v2x_available.attributes = std::vector<DbVariableAttribute>{v2x_available_attribute};
        variables.push_back(v2x_available);
        ComponentKey v2x_ctrl;
        v2x_ctrl.name = "V2XChargingCtrlr";
        v2x_ctrl.evse_id = 1;
        component_configs.insert(std::make_pair(v2x_ctrl, variables));
        v2x_ctrl.evse_id = 2;
        component_configs.insert(std::make_pair(v2x_ctrl, variables));
        db.initialize_database(component_configs, true);
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

    std::vector<ChargingSchedulePeriod> create_charging_schedule_periods(const std::vector<int32_t>& start_periods) {
        auto charging_schedule_periods = std::vector<ChargingSchedulePeriod>();
        for (auto start_period : start_periods) {
            ChargingSchedulePeriod charging_schedule_period;
            charging_schedule_period.startPeriod = start_period;
            charging_schedule_periods.push_back(charging_schedule_period);
        }

        return charging_schedule_periods;
    }

    std::shared_ptr<DatabaseHandler> create_database_handler() {
        auto database_connection =
            std::make_unique<everest::db::sqlite::Connection>(fs::path("/tmp/ocpp201") / "cp.db");
        return std::make_shared<DatabaseHandler>(std::move(database_connection), MIGRATION_FILES_LOCATION_V2);
    }

    std::shared_ptr<MessageQueue<v2::MessageType>>
    create_message_queue(std::shared_ptr<DatabaseHandler>& database_handler) {
        const auto DEFAULT_MESSAGE_QUEUE_SIZE_THRESHOLD = 2E5;
        return std::make_shared<ocpp::MessageQueue<v2::MessageType>>(
            [this](json message) -> bool { return false; },
            MessageQueueConfig<v2::MessageType>{
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
        callbacks.reserve_now_callback = reserve_now_callback_mock.AsStdFunction();
        callbacks.cancel_reservation_callback = cancel_reservation_callback_mock.AsStdFunction();
        callbacks.update_allowed_energy_transfer_modes_callback =
            update_allowed_energy_transfer_modes_callback.AsStdFunction();
    }

    std::shared_ptr<DeviceModel> device_model;
    std::map<int32_t, int32_t> evse_connector_structure;

    testing::MockFunction<bool(const std::optional<const int32_t> evse_id, const ResetEnum& reset_type)>
        is_reset_allowed_callback_mock;
    testing::MockFunction<void(const std::optional<const int32_t> evse_id, const ResetEnum& reset_type)>
        reset_callback_mock;
    testing::MockFunction<RequestStartStopStatusEnum(const int32_t evse_id, const ReasonEnum& stop_reason)>
        stop_transaction_callback_mock;
    testing::MockFunction<void(const int32_t evse_id)> pause_charging_callback_mock;
    testing::MockFunction<void(const int32_t evse_id, const int32_t connector_id,
                               const OperationalStatusEnum new_status)>
        connector_effective_operative_status_changed_callback_mock;
    testing::MockFunction<GetLogResponse(const GetLogRequest& request)> get_log_request_callback_mock;
    testing::MockFunction<UnlockConnectorResponse(const int32_t evse_id, const int32_t connecor_id)>
        unlock_connector_callback_mock;
    testing::MockFunction<RequestStartStopStatusEnum(const RequestStartTransactionRequest& request,
                                                     const bool authorize_remote_start)>
        remote_start_transaction_callback_mock;
    testing::MockFunction<ocpp::ReservationCheckStatus(const int32_t evse_id, const CiString<255> idToken,
                                                       const std::optional<CiString<255>> groupIdToken)>
        is_reservation_for_token_callback_mock;
    testing::MockFunction<UpdateFirmwareResponse(const UpdateFirmwareRequest& request)>
        update_firmware_request_callback_mock;
    testing::MockFunction<void(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info)>
        security_event_callback_mock;
    testing::MockFunction<void()> set_charging_profiles_callback_mock;
    testing::MockFunction<ReserveNowStatusEnum(const ReserveNowRequest& request)> reserve_now_callback_mock;
    testing::MockFunction<bool(const int32_t reservationId)> cancel_reservation_callback_mock;
    testing::MockFunction<
        std::optional<bool(const std::vector<ocpp::v2::EnergyTransferModeEnum> allowed_energy_transfer_modes,
                           const CiString<36> transaction_id)>>
        update_allowed_energy_transfer_modes_callback;

    ocpp::v2::Callbacks callbacks;
};

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

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfSetChargingProfilesCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.set_charging_profiles_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

/*
 * For completeness, we also test that all other callbacks are checked by
 * all_callbacks_valid.
 */

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksAreInvalidWhenNotProvided) {
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksAreValidWhenAllRequiredCallbacksProvided) {
    configure_callbacks_with_mocks();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfResetIsAllowedCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.is_reset_allowed_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfResetCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.reset_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfStopTransactionCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.stop_transaction_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfPauseChargingCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.pause_charging_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2,
       K01FR02_CallbacksValidityChecksIfConnectorEffectiveOperativeStatusChangedCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.connector_effective_operative_status_changed_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfGetLogRequestCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.get_log_request_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfUnlockConnectorCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.unlock_connector_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfRemoteStartTransactionCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.remote_start_transaction_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfIsReservationForTokenCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.is_reservation_for_token_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfUpdateFirmwareRequestCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.update_firmware_request_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfSecurityEventCallbackExists) {
    configure_callbacks_with_mocks();
    callbacks.security_event_callback = nullptr;

    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2,
       K01FR02_CallbacksValidityChecksIfOptionalVariableChangedCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.variable_changed_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<void(const SetVariableData& set_variable_data)> variable_changed_callback_mock;
    callbacks.variable_changed_callback = variable_changed_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2,
       K01FR02_CallbacksValidityChecksIfOptionalVariableNetworkProfileCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.validate_network_profile_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<SetNetworkProfileStatusEnum(const int32_t configuration_slot,
                                                      const NetworkConnectionProfile& network_connection_profile)>
        validate_network_profile_callback_mock;
    callbacks.validate_network_profile_callback = validate_network_profile_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2,
       K01FR02_CallbacksValidityChecksIfOptionalConfigureNetworkConnectionProfileCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.configure_network_connection_profile_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<ConfigureNetworkConnectionProfileCallback> configure_network_connection_profile_callback_mock;
    callbacks.configure_network_connection_profile_callback =
        configure_network_connection_profile_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfOptionalTimeSyncCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.time_sync_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<void(const ocpp::DateTime& currentTime)> time_sync_callback_mock;
    callbacks.time_sync_callback = time_sync_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2,
       K01FR02_CallbacksValidityChecksIfOptionalBootNotificationCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.boot_notification_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<void(const ocpp::v2::BootNotificationResponse& response)> boot_notification_callback_mock;
    callbacks.boot_notification_callback = boot_notification_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfOptionalOCPPMessagesCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.ocpp_messages_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<void(const std::string& message, MessageDirection direction)> ocpp_messages_callback_mock;
    callbacks.ocpp_messages_callback = ocpp_messages_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2,
       K01FR02_CallbacksValidityChecksIfOptionalCSEffectiveOperativeStatusChangedCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.cs_effective_operative_status_changed_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<void(const OperationalStatusEnum new_status)>
        cs_effective_operative_status_changed_callback_mock;
    callbacks.cs_effective_operative_status_changed_callback =
        cs_effective_operative_status_changed_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2,
       K01FR02_CallbacksValidityChecksIfOptionalEvseEffectiveOperativeStatusChangedCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.evse_effective_operative_status_changed_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<void(const int32_t evse_id, const OperationalStatusEnum new_status)>
        evse_effective_operative_status_changed_callback_mock;
    callbacks.evse_effective_operative_status_changed_callback =
        evse_effective_operative_status_changed_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2,
       K01FR02_CallbacksValidityChecksIfOptionalGetCustomerInformationCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.get_customer_information_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<std::string(const std::optional<CertificateHashDataType> customer_certificate,
                                      const std::optional<IdToken> id_token,
                                      const std::optional<CiString<64>> customer_identifier)>
        get_customer_information_callback_mock;
    callbacks.get_customer_information_callback = get_customer_information_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2,
       K01FR02_CallbacksValidityChecksIfOptionalClearCustomerInformationCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.clear_customer_information_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<std::string(const std::optional<CertificateHashDataType> customer_certificate,
                                      const std::optional<IdToken> id_token,
                                      const std::optional<CiString<64>> customer_identifier)>
        clear_customer_information_callback_mock;
    callbacks.clear_customer_information_callback = clear_customer_information_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2,
       K01FR02_CallbacksValidityChecksIfOptionalAllConnectorsUnavailableCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.all_connectors_unavailable_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<void()> all_connectors_unavailable_callback_mock;
    callbacks.all_connectors_unavailable_callback = all_connectors_unavailable_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, K01FR02_CallbacksValidityChecksIfOptionalDataTransferCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.data_transfer_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<DataTransferResponse(const DataTransferRequest& request)> data_transfer_callback_mock;
    callbacks.data_transfer_callback = data_transfer_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2,
       K01FR02_CallbacksValidityChecksIfOptionalTransactionEventCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.transaction_event_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<void(const TransactionEventRequest& transaction_event)> transaction_event_callback_mock;
    callbacks.transaction_event_callback = transaction_event_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2,
       K01FR02_CallbacksValidityChecksIfOptionalTransactionEventResponseCallbackIsNotSetOrNotNull) {
    configure_callbacks_with_mocks();

    callbacks.transaction_event_response_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));

    testing::MockFunction<void(const TransactionEventRequest& transaction_event,
                               const TransactionEventResponse& transaction_event_response)>
        transaction_event_response_callback_mock;
    callbacks.transaction_event_response_callback = transaction_event_response_callback_mock.AsStdFunction();
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, ReservationAvailableReserveNowCallbackNotSet) {
    configure_callbacks_with_mocks();
    device_model->set_value(ControllerComponentVariables::ReservationCtrlrAvailable.component,
                            ControllerComponentVariables::ReservationCtrlrAvailable.variable.value(),
                            AttributeEnum::Actual, "true", "test", true);
    callbacks.reserve_now_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, ReservationAvailableCancelReservationCallbackNotSet) {
    configure_callbacks_with_mocks();
    device_model->set_value(ControllerComponentVariables::ReservationCtrlrAvailable.component,
                            ControllerComponentVariables::ReservationCtrlrAvailable.variable.value(),
                            AttributeEnum::Actual, "true", "test", true);
    callbacks.cancel_reservation_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, ReservationNotAvailableReserveNowCallbackNotSet) {
    configure_callbacks_with_mocks();
    device_model->set_value(ControllerComponentVariables::ReservationCtrlrAvailable.component,
                            ControllerComponentVariables::ReservationCtrlrAvailable.variable.value(),
                            AttributeEnum::Actual, "false", "test", true);
    callbacks.reserve_now_callback = nullptr;
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, ReservationNotAvailableCancelReservationCallbackNotSet) {
    configure_callbacks_with_mocks();
    device_model->set_value(ControllerComponentVariables::ReservationCtrlrAvailable.component,
                            ControllerComponentVariables::ReservationCtrlrAvailable.variable.value(),
                            AttributeEnum::Actual, "false", "test", true);
    callbacks.cancel_reservation_callback = nullptr;
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, NoV2XUpdateAllowedEnergyTransferModesCallbackNotSet) {
    configure_callbacks_with_mocks();
    callbacks.update_allowed_energy_transfer_modes_callback = nullptr;
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, WithV2XUpdateAllowedEnergyTransferModesCallbackNotSet) {
    configure_callbacks_with_mocks();
    device_model->set_value(
        V2xComponentVariables::get_component_variable(1, V2xComponentVariables::Available).component,
        V2xComponentVariables::get_component_variable(1, V2xComponentVariables::Available).variable.value(),
        AttributeEnum::Actual, "true", "");
    device_model->set_value(ControllerComponents::ISO15118Ctrlr,
                            ControllerComponentVariables::ISO15118CtrlrAvailable.variable.value(),
                            AttributeEnum::Actual, "true", "");
    callbacks.update_allowed_energy_transfer_modes_callback = nullptr;
    EXPECT_FALSE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

TEST_F(ChargePointCommonTestFixtureV2, WithV2XUpdateAllowedEnergyTransferModesCallbackSet) {
    configure_callbacks_with_mocks();
    device_model->set_value(
        V2xComponentVariables::get_component_variable(1, V2xComponentVariables::Available).component,
        V2xComponentVariables::get_component_variable(1, V2xComponentVariables::Available).variable.value(),
        AttributeEnum::Actual, "true", "");
    device_model->set_value(ControllerComponents::ISO15118Ctrlr,
                            ControllerComponentVariables::ISO15118CtrlrAvailable.variable.value(),
                            AttributeEnum::Actual, "true", "");
    EXPECT_TRUE(callbacks.all_callbacks_valid(device_model, evse_connector_structure));
}

class ChargePointConstructorTestFixtureV2 : public ChargePointCommonTestFixtureV2 {
public:
    ChargePointConstructorTestFixtureV2() :
        evse_connector_structure(create_evse_connector_structure()),
        database_handler(create_database_handler()),
        evse_security(std::make_shared<EvseSecurityMock>()) {
    }
    ~ChargePointConstructorTestFixtureV2() {
    }

    std::map<int32_t, int32_t> evse_connector_structure;
    std::shared_ptr<DatabaseHandler> database_handler;
    std::shared_ptr<EvseSecurityMock> evse_security;
};

TEST_F(ChargePointConstructorTestFixtureV2, CreateChargePoint) {
    configure_callbacks_with_mocks();

    EXPECT_NO_THROW(ocpp::v2::ChargePoint(evse_connector_structure, device_model, database_handler,
                                          create_message_queue(database_handler), "/tmp", evse_security, callbacks));
}

TEST_F(ChargePointConstructorTestFixtureV2, CreateChargePoint_InitializeInCorrectOrder) {
    database_handler->open_connection();
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

    ocpp::v2::ChargePoint charge_point(evse_connector_structure, device_model, database_handler, message_queue, "/tmp",
                                       evse_security, callbacks);

    EXPECT_NO_FATAL_FAILURE(charge_point.start(BootReasonEnum::PowerUp));

    charge_point.stop();
}

TEST_F(ChargePointConstructorTestFixtureV2,
       CreateChargePoint_EVSEConnectorStructureDefinedBadly_ThrowsDeviceModelError) {
    configure_callbacks_with_mocks();
    auto evse_connector_structure = std::map<int32_t, int32_t>();

    EXPECT_THROW(ocpp::v2::ChargePoint(evse_connector_structure, device_model, database_handler,
                                       create_message_queue(database_handler), "/tmp", evse_security, callbacks),
                 DeviceModelError);
}

TEST_F(ChargePointConstructorTestFixtureV2, CreateChargePoint_MissingDeviceModel_ThrowsInvalidArgument) {
    configure_callbacks_with_mocks();
    auto message_queue = std::make_shared<ocpp::MessageQueue<v2::MessageType>>(
        [this](json message) -> bool { return false; }, MessageQueueConfig<v2::MessageType>{}, database_handler);

    EXPECT_THROW(ocpp::v2::ChargePoint(evse_connector_structure, nullptr, database_handler, message_queue, "/tmp",
                                       evse_security, callbacks),
                 std::invalid_argument);
}

TEST_F(ChargePointConstructorTestFixtureV2, CreateChargePoint_MissingDatabaseHandler_ThrowsInvalidArgument) {
    configure_callbacks_with_mocks();
    auto message_queue = std::make_shared<ocpp::MessageQueue<v2::MessageType>>(
        [this](json message) -> bool { return false; }, MessageQueueConfig<v2::MessageType>{}, nullptr);

    auto database_handler = nullptr;

    EXPECT_THROW(ocpp::v2::ChargePoint(evse_connector_structure, device_model, database_handler, message_queue, "/tmp",
                                       evse_security, callbacks),
                 std::invalid_argument);
}

TEST_F(ChargePointConstructorTestFixtureV2, CreateChargePoint_CallbacksNotValid_ThrowsInvalidArgument) {
    EXPECT_THROW(ocpp::v2::ChargePoint(evse_connector_structure, device_model, database_handler,
                                       create_message_queue(database_handler), "/tmp", evse_security, callbacks),
                 std::invalid_argument);
}

class TestChargePoint : public ChargePoint {
public:
    using ChargePoint::handle_message;

    TestChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                    std::shared_ptr<DeviceModel> device_model, std::shared_ptr<DatabaseHandler> database_handler,
                    std::shared_ptr<MessageQueue<v2::MessageType>> message_queue, const std::string& message_log_path,
                    const std::shared_ptr<EvseSecurity> evse_security, const Callbacks& callbacks) :
        ChargePoint(evse_connector_structure, device_model, database_handler, message_queue, message_log_path,
                    evse_security, callbacks) {
    }
};

class ChargePointFunctionalityTestFixtureV2 : public ChargePointCommonTestFixtureV2 {
public:
    ChargePointFunctionalityTestFixtureV2() :
        uuid_generator(boost::uuids::random_generator()), charge_point(create_charge_point()) {
    }
    ~ChargePointFunctionalityTestFixtureV2() {
    }

    void SetUp() override {
        charge_point->start();
    }

    void TearDown() override {
        charge_point->stop();
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
        ocpp::Call<T> call(req);
        EnhancedMessage<MessageType> enhanced_message;
        enhanced_message.uniqueId = message_id;
        enhanced_message.messageType = M;
        enhanced_message.messageTypeId = MessageTypeId::CALL;

        call_to_json(enhanced_message.message, call);

        return enhanced_message;
    }

    std::unique_ptr<TestChargePoint> create_charge_point() {
        auto database_handler = create_database_handler();
        configure_callbacks_with_mocks();
        auto charge_point = std::make_unique<TestChargePoint>(
            create_evse_connector_structure(), device_model, database_handler, create_message_queue(database_handler),
            TEMP_OUTPUT_PATH, std::make_shared<EvseSecurityMock>(), callbacks);
        return charge_point;
    }

    boost::uuids::random_generator uuid_generator;
    std::unique_ptr<TestChargePoint> charge_point;
    std::unique_ptr<SmartChargingMock> smart_charging;
};

// Test currently disabled because this is not working now. Should be added to the transaction functional block.
TEST_F(ChargePointFunctionalityTestFixtureV2,
       K05FR05_RequestStartTransactionRequest_SmartChargingCtrlrEnabledTrue_ValidatesTxProfiles) {
    GTEST_SKIP_("Test currently disabled because this is not working now. Should be added to the transaction "
                "functional block.");
    const auto cv = ControllerComponentVariables::SmartChargingCtrlrEnabled;
    this->device_model->set_value(cv.component, cv.variable.value(), AttributeEnum::Actual, "true", "TEST", true);

    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    RequestStartTransactionRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.idToken.idToken = "Local";
    req.idToken.type = IdTokenEnumStringType::Local;
    req.chargingProfile = profile;

    auto start_transaction_req =
        request_to_enhanced_message<RequestStartTransactionRequest, MessageType::RequestStartTransaction>(req);

    EXPECT_CALL(*smart_charging, conform_validate_and_add_profile).Times(1);

    charge_point->handle_message(start_transaction_req);
}

// Test currently disabled because this is not working now. Should be added to the transaction functional block.
TEST_F(ChargePointFunctionalityTestFixtureV2,
       K05FR04_RequestStartTransactionRequest_SmartChargingCtrlrEnabledFalse_DoesNotValidateTxProfiles) {
    GTEST_SKIP_("Test currently disabled because this is not working now. Should be added to the transaction "
                "functional block.");
    const auto cv = ControllerComponentVariables::SmartChargingCtrlrEnabled;
    this->device_model->set_value(cv.component, cv.variable.value(), AttributeEnum::Actual, "false", "TEST", true);

    auto periods = create_charging_schedule_periods({0, 1, 2});

    auto profile = create_charging_profile(
        DEFAULT_PROFILE_ID, ChargingProfilePurposeEnum::TxProfile,
        create_charge_schedule(ChargingRateUnitEnum::A, periods, ocpp::DateTime("2024-01-17T17:00:00")), DEFAULT_TX_ID);

    RequestStartTransactionRequest req;
    req.evseId = DEFAULT_EVSE_ID;
    req.idToken.idToken = "Local";
    req.idToken.type = IdTokenEnumStringType::Local;
    req.chargingProfile = profile;

    auto start_transaction_req =
        request_to_enhanced_message<RequestStartTransactionRequest, MessageType::RequestStartTransaction>(req);

    EXPECT_CALL(*smart_charging, conform_validate_and_add_profile).Times(0);

    charge_point->handle_message(start_transaction_req);
}

// Test currently disabled because this is not working now. Should be added to the transaction functional block.
TEST_F(ChargePointFunctionalityTestFixtureV2, K02FR05_TransactionEnds_WillDeleteTxProfilesWithTransactionID) {
    GTEST_SKIP_("Test currently disabled because this is not working now. Should be added to the transaction "
                "functional block.");
    auto database_handler = create_database_handler();
    database_handler->open_connection();
    const auto cv = ControllerComponentVariables::ResumeTransactionsOnBoot;
    this->device_model->set_value(cv.component, cv.variable.value(), AttributeEnum::Actual, "true", "TEST", true);
    int32_t connector_id = 1;
    std::string session_id = "some-session-id";
    ocpp::DateTime timestamp("2024-01-17T17:00:00");

    charge_point->on_transaction_started(DEFAULT_EVSE_ID, connector_id, session_id, timestamp,
                                         ocpp::v2::TriggerReasonEnum::Authorized, MeterValue(), {}, {}, {}, {},
                                         ChargingStateEnum::EVConnected);
    auto transaction = database_handler->transaction_get(DEFAULT_EVSE_ID);
    ASSERT_THAT(transaction, testing::NotNull());

    EXPECT_CALL(*smart_charging, delete_transaction_tx_profiles(transaction->get_transaction().transactionId.get()));
    charge_point->on_transaction_finished(DEFAULT_EVSE_ID, timestamp, MeterValue(), ReasonEnum::StoppedByEV,
                                          TriggerReasonEnum::StopAuthorized, {}, {}, ChargingStateEnum::EVConnected);
}
} // namespace ocpp::v2
