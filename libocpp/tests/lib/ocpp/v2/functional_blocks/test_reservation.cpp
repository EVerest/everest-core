// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "component_state_manager_mock.hpp"
#include "connectivity_manager_mock.hpp"
#include "evse_security_mock.hpp"
#include "mocks/database_handler_mock.hpp"
#include <evse_manager_fake.hpp>
#include <message_dispatcher_mock.hpp>

#include <device_model_test_helper.hpp>
#include <ocpp/v2/functional_blocks/reservation.hpp>

#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model_storage_sqlite.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/init_device_model_db.hpp>

#include <ocpp/v2/messages/CancelReservation.hpp>
#include <ocpp/v2/messages/ReservationStatusUpdate.hpp>
#include <ocpp/v2/messages/ReserveNow.hpp>
#include <ocpp/v2/messages/Reset.hpp>
const static uint32_t NR_OF_EVSES = 2;

using namespace ocpp::v2;
using ::testing::_;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::Return;

class ReservationTest : public ::testing::Test {
public:
protected: // Functions
    ReservationTest() :
        database_connection(std::make_unique<everest::db::sqlite::Connection>(DEVICE_MODEL_DB_IN_MEMORY_PATH)),
        ocpp_version(ocpp::OcppProtocolVersion::v201) {
        database_connection->open_connection();
        this->device_model = create_device_model();
        this->functional_block_context = std::make_unique<FunctionalBlockContext>(
            this->mock_dispatcher, *this->device_model, this->connectivity_manager, this->evse_manager,
            this->database_handler, this->evse_security, this->component_state_manager, this->ocpp_version);
        this->reservation = std::make_unique<Reservation>(
            *functional_block_context, reserve_now_callback_mock.AsStdFunction(),
            cancel_reservation_callback_mock.AsStdFunction(), is_reservation_for_token_callback_mock.AsStdFunction());
        default_test_token.idToken = "SOME_TOKEN";
        default_test_token.type = IdTokenEnumStringType::ISO14443;
    }

    ///
    /// \brief Create the database for the device model and apply migrations.
    /// \param path Database path.
    ///
    void create_device_model_db(const std::string& path) {
        InitDeviceModelDb db(path, MIGRATION_FILES_PATH);
        const auto component_configs = get_all_component_configs(CONFIG_PATH);
        db.initialize_database(component_configs, true);
    }

    ///
    /// \brief Create device model.
    /// \param is_reservation_available     Value of ReservationCtrlr variable 'Available' in the device model.
    /// \param is_reservation_enabled       Value of ReservationCtrlr variable 'enabled' in the device model.
    /// \param non_evse_specific_enabled    Enable/disable non evse specific reservations in the device model.
    /// \return The created device model.
    ///
    std::unique_ptr<DeviceModel> create_device_model(const bool is_reservation_available = true,
                                                     const bool is_reservation_enabled = true,
                                                     const bool non_evse_specific_enabled = true) {
        create_device_model_db(DEVICE_MODEL_DB_IN_MEMORY_PATH);
        auto device_model_storage = std::make_unique<DeviceModelStorageSqlite>(DEVICE_MODEL_DB_IN_MEMORY_PATH);
        auto dm = std::make_unique<DeviceModel>(std::move(device_model_storage));
        // Defaults
        set_reservation_available(dm.get(), is_reservation_available);
        set_reservation_enabled(dm.get(), is_reservation_enabled);
        set_non_evse_specific(dm.get(), non_evse_specific_enabled);

        // Check values
        const bool reservation_available_in_device_model =
            dm->get_optional_value<bool>(ControllerComponentVariables::ReservationCtrlrAvailable).value_or(false);
        EXPECT_EQ(reservation_available_in_device_model, is_reservation_available);

        const bool reservation_enabled_in_device_model =
            dm->get_optional_value<bool>(ControllerComponentVariables::ReservationCtrlrEnabled).value_or(false);
        EXPECT_EQ(reservation_enabled_in_device_model, is_reservation_enabled);

        const bool non_evse_specific_enabled_device_model =
            dm->get_optional_value<bool>(ControllerComponentVariables::ReservationCtrlrNonEvseSpecific).value_or(false);
        EXPECT_EQ(non_evse_specific_enabled_device_model, non_evse_specific_enabled);

        return dm;
    }

    ///
    /// \brief Set value of ReservationCtrlr variable 'Enabled' in the device model.
    /// \param device_model The device model to set the value in.
    /// \param enabled      True to set to enabled.
    ///
    void set_reservation_enabled(DeviceModel* device_model, const bool enabled) {
        const auto& reservation_enabled = ControllerComponentVariables::ReservationCtrlrEnabled;
        EXPECT_EQ(device_model->set_value(reservation_enabled.component, reservation_enabled.variable.value(),
                                          AttributeEnum::Actual, enabled ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    ///
    /// \brief Set value of ReservationCtrlr variable 'Available' in the device model.
    /// \param device_model The device model to set the value in.
    /// \param available    True to set to available.
    ///
    void set_reservation_available(DeviceModel* device_model, const bool available) {
        const auto& reservation_available = ControllerComponentVariables::ReservationCtrlrAvailable;
        EXPECT_EQ(device_model->set_value(reservation_available.component, reservation_available.variable.value(),
                                          AttributeEnum::Actual, (available ? "true" : "false"), "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    ///
    /// \brief Enable or disable non evse specific reservations in the device model.
    /// \param device_model                 The device model to set the value in.
    /// \param non_evse_specific_enabled    True to enable non evse specific reservations.
    ///
    void set_non_evse_specific(DeviceModel* device_model, const bool non_evse_specific_enabled) {
        const auto& non_evse_specific = ControllerComponentVariables::ReservationCtrlrNonEvseSpecific;
        EXPECT_EQ(device_model->set_value(non_evse_specific.component, non_evse_specific.variable.value(),
                                          AttributeEnum::Actual, (non_evse_specific_enabled ? "true" : "false"),
                                          "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    ///
    /// \brief Create example ReserveNow request to use in tests.
    /// \param evse_id          Optional evse id.
    /// \param connector_type   Optional connector type.
    /// \return The request message.
    ///
    ocpp::EnhancedMessage<MessageType>
    create_example_reserve_now_request(const std::optional<int32_t> evse_id = std::nullopt,
                                       const std::optional<ocpp::CiString<20>> connector_type = std::nullopt) {
        ReserveNowRequest request;
        request.connectorType = connector_type;
        request.evseId = evse_id;
        request.id = 1;
        request.idToken = default_test_token;
        ocpp::Call<ReserveNowRequest> call(request);
        ocpp::EnhancedMessage<MessageType> enhanced_message;
        enhanced_message.messageType = MessageType::ReserveNow;
        enhanced_message.message = call;
        return enhanced_message;
    }

    ///
    /// \brief Create example CancelReservation request to use in tests.
    /// \param reservation_id   The reservation id.
    /// \return The request message.
    ///
    ocpp::EnhancedMessage<MessageType> create_example_cancel_reservation_request(const int32_t reservation_id) {
        CancelReservationRequest request;
        request.reservationId = reservation_id;
        ocpp::Call<CancelReservationRequest> call(request);
        ocpp::EnhancedMessage<MessageType> enhanced_message;
        enhanced_message.messageType = MessageType::CancelReservation;
        enhanced_message.message = call;
        return enhanced_message;
    }

protected: // Members
    ConnectivityManagerMock connectivity_manager;
    ocpp::v2::DatabaseHandlerMock database_handler;
    ocpp::EvseSecurityMock evse_security;
    ComponentStateManagerMock component_state_manager;
    // Connection as member so the database keeps open and is not destroyed (because this is an in memory
    // database).
    std::unique_ptr<everest::db::sqlite::Connection> database_connection;
    MockMessageDispatcher mock_dispatcher;
    EvseManagerFake evse_manager{NR_OF_EVSES};
    // Device model is a unique ptr here because of the database: it is stored in memory so as soon as the handle to
    // the database closes, the database is removed. So the handle should be opened before creating the devide model.
    // So the device model is initialized on nullptr, then the handle is opened, the devide model is created and the
    // handle stays open until the whole test is destructed.
    std::unique_ptr<DeviceModel> device_model;
    MockFunction<ReserveNowStatusEnum(const ReserveNowRequest& request)> reserve_now_callback_mock;
    MockFunction<bool(const int32_t reservationId)> cancel_reservation_callback_mock;
    MockFunction<ocpp::ReservationCheckStatus(const int32_t evse_id, const ocpp::CiString<255> idToken,
                                              const std::optional<ocpp::CiString<255>> groupIdToken)>
        is_reservation_for_token_callback_mock;
    std::atomic<ocpp::OcppProtocolVersion> ocpp_version;
    std::unique_ptr<FunctionalBlockContext> functional_block_context;
    // Make reservation a unique ptr so we can create it after creating the device model.
    std::unique_ptr<Reservation> reservation;

    IdToken default_test_token;
};

TEST_F(ReservationTest, handle_reserve_now_reservation_not_available) {
    // In the device model, reservation is set to not available. This should reject the request.
    set_reservation_available(this->device_model.get(), false);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ReserveNowResponse>();
        EXPECT_EQ(response.status, ReserveNowStatusEnum::Rejected);
        ASSERT_TRUE(response.statusInfo.has_value());
        ASSERT_TRUE(response.statusInfo.value().additionalInfo.has_value());
        EXPECT_EQ(response.statusInfo.value().additionalInfo.value(), "Reservation is not available");
    }));

    EvseMock& m1 = evse_manager.get_mock(1);
    EvseMock& m2 = evse_manager.get_mock(2);

    EXPECT_CALL(m1, get_connector_status(_)).Times(0);
    EXPECT_CALL(m2, get_connector_status(_)).Times(0);

    const ocpp::EnhancedMessage<MessageType> request = create_example_reserve_now_request();
    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_reserve_now_callback_nullptr) {
    // The callback to make the reservation is a nullptr. This should reject the request.
    Reservation r{*this->functional_block_context, nullptr, cancel_reservation_callback_mock.AsStdFunction(),
                  is_reservation_for_token_callback_mock.AsStdFunction()};

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ReserveNowResponse>();
        EXPECT_EQ(response.status, ReserveNowStatusEnum::Rejected);
        ASSERT_TRUE(response.statusInfo.has_value());
        ASSERT_TRUE(response.statusInfo.value().additionalInfo.has_value());
        EXPECT_EQ(response.statusInfo.value().additionalInfo.value(), "Reservation is not implemented");
    }));

    EvseMock& m1 = evse_manager.get_mock(1);
    EvseMock& m2 = evse_manager.get_mock(2);

    EXPECT_CALL(m1, get_connector_status(_)).Times(0);
    EXPECT_CALL(m2, get_connector_status(_)).Times(0);

    const ocpp::EnhancedMessage<MessageType> request = create_example_reserve_now_request();
    r.handle_message(request);
}

TEST_F(ReservationTest, handle_reserve_now_reservation_disabled) {
    // In the device model, reservation is set to not enabled. This should reject the request.
    set_reservation_enabled(this->device_model.get(), false);

    const bool reservation_enabled_in_device_model =
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::ReservationCtrlrEnabled)
            .value_or(false);
    EXPECT_EQ(reservation_enabled_in_device_model, false);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ReserveNowResponse>();
        EXPECT_EQ(response.status, ReserveNowStatusEnum::Rejected);
        ASSERT_TRUE(response.statusInfo.has_value());
        ASSERT_TRUE(response.statusInfo.value().additionalInfo.has_value());
        EXPECT_EQ(response.statusInfo.value().additionalInfo.value(), "Reservation is not enabled");
    }));

    EvseMock& m1 = evse_manager.get_mock(1);
    EvseMock& m2 = evse_manager.get_mock(2);

    EXPECT_CALL(m1, get_connector_status(_)).Times(0);
    EXPECT_CALL(m2, get_connector_status(_)).Times(0);

    const ocpp::EnhancedMessage<MessageType> request = create_example_reserve_now_request();
    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_reserve_now_non_evse_specific_disabled) {
    // In the device model, non evse specific reservations are disabled. So when we try to make a reservation for
    // a non specific evse (no evse id given), the request should be rejected.
    set_non_evse_specific(this->device_model.get(), false);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ReserveNowResponse>();
        EXPECT_EQ(response.status, ReserveNowStatusEnum::Rejected);
        ASSERT_TRUE(response.statusInfo.has_value());
        ASSERT_TRUE(response.statusInfo.value().additionalInfo.has_value());
        EXPECT_EQ(response.statusInfo.value().additionalInfo.value(),
                  "No evse id was given while it should be sent in the request when NonEvseSpecific is disabled");
    }));

    EvseMock& m1 = evse_manager.get_mock(1);
    EvseMock& m2 = evse_manager.get_mock(2);

    EXPECT_CALL(m1, get_connector_status(_)).Times(0);
    EXPECT_CALL(m2, get_connector_status(_)).Times(0);

    const ocpp::EnhancedMessage<MessageType> request = create_example_reserve_now_request();
    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_reserve_now_evse_not_existing) {
    // Try to make a reservation with a not existing evse id. This should reject the request.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ReserveNowResponse>();
        EXPECT_EQ(response.status, ReserveNowStatusEnum::Rejected);
        ASSERT_TRUE(response.statusInfo.has_value());
        ASSERT_TRUE(response.statusInfo.value().additionalInfo.has_value());
        EXPECT_EQ(response.statusInfo.value().additionalInfo.value(), "Evse id does not exist");
    }));

    const ocpp::EnhancedMessage<MessageType> request = create_example_reserve_now_request(5);
    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_reserve_now_connector_not_existing) {
    // Try to make a reservation for a connector type that does not exist. This should reject the request.
    EvseMock& m1 = evse_manager.get_mock(1);
    EXPECT_CALL(m1, does_connector_exist(ConnectorEnumStringType::Pan)).WillOnce(Return(false));

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ReserveNowResponse>();
        EXPECT_EQ(response.status, ReserveNowStatusEnum::Rejected);
        ASSERT_TRUE(response.statusInfo.has_value());
        ASSERT_TRUE(response.statusInfo.value().additionalInfo.has_value());
        EXPECT_EQ(response.statusInfo.value().additionalInfo.value(), "Connector type does not exist");
    }));

    const ocpp::EnhancedMessage<MessageType> request =
        create_example_reserve_now_request(1, ConnectorEnumStringType::Pan);
    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_reserve_now_connectors_not_existing) {
    // Try to make a  non evse specific reservation for a connector type that does not exist. This should reject the
    // request.
    EvseMock& m1 = evse_manager.get_mock(1);
    ON_CALL(m1, does_connector_exist(ConnectorEnumStringType::cG105)).WillByDefault(Return(false));

    EvseMock& m2 = evse_manager.get_mock(2);
    ON_CALL(m2, does_connector_exist(ConnectorEnumStringType::cG105)).WillByDefault(Return(false));

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ReserveNowResponse>();
        EXPECT_EQ(response.status, ReserveNowStatusEnum::Rejected);
        ASSERT_TRUE(response.statusInfo.has_value());
        ASSERT_TRUE(response.statusInfo.value().additionalInfo.has_value());
        EXPECT_EQ(response.statusInfo.value().additionalInfo.value(), "Could not get status info from connector");
    }));

    const ocpp::EnhancedMessage<MessageType> request =
        create_example_reserve_now_request(std::nullopt, ConnectorEnumStringType::cG105);
    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_reserve_now_one_connector_not_existing) {
    // Try to make a non evse specific reservation. One connector does not have the given connector, but the other does,
    // so the reservation request should be accepted.
    const ocpp::EnhancedMessage<MessageType> request =
        create_example_reserve_now_request(std::nullopt, ConnectorEnumStringType::cTesla);

    EvseMock& m1 = evse_manager.get_mock(1);
    EXPECT_CALL(m1, does_connector_exist(ConnectorEnumStringType::cTesla)).WillOnce(Return(false));

    EvseMock& m2 = evse_manager.get_mock(2);
    EXPECT_CALL(m2, does_connector_exist(ConnectorEnumStringType::cTesla)).WillOnce(Return(true));

    EXPECT_CALL(reserve_now_callback_mock, Call(_)).WillOnce(Return(ReserveNowStatusEnum::Accepted));

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ReserveNowResponse>();
        EXPECT_EQ(response.status, ReserveNowStatusEnum::Accepted);
    }));

    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_reserve_now_all_connectors_not_available) {
    // Try to make a reservation with all connectors unavailable. Since the evse manager has the last word in this,
    // we try to do the request and it can be accepted anyway (or at least the correct reason for not accepting the
    // reservation can be returned, if this is a real scenario).
    const ocpp::EnhancedMessage<MessageType> request =
        create_example_reserve_now_request(std::nullopt, ConnectorEnumStringType::cTesla);

    EvseMock& m1 = evse_manager.get_mock(1);
    EXPECT_CALL(m1, does_connector_exist(ConnectorEnumStringType::cTesla)).WillOnce(Return(true));

    EvseMock& m2 = evse_manager.get_mock(2);
    EXPECT_CALL(m2, does_connector_exist(ConnectorEnumStringType::cTesla)).Times(0);

    EXPECT_CALL(reserve_now_callback_mock, Call(_)).WillOnce(Return(ReserveNowStatusEnum::Accepted));

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ReserveNowResponse>();
        EXPECT_EQ(response.status, ReserveNowStatusEnum::Accepted);
    }));

    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_reserve_now_non_specific_evse_successful) {
    // Try to make a non evse specific reservation which is accepted.
    const ocpp::EnhancedMessage<MessageType> request =
        create_example_reserve_now_request(std::nullopt, ConnectorEnumStringType::cTesla);

    EvseMock& m1 = evse_manager.get_mock(1);
    EXPECT_CALL(m1, does_connector_exist(ConnectorEnumStringType::cTesla)).WillOnce(Return(true));

    ON_CALL(reserve_now_callback_mock, Call(_)).WillByDefault(Invoke([](const ReserveNowRequest reserve_now_request) {
        EXPECT_FALSE(reserve_now_request.evseId.has_value());
        return ReserveNowStatusEnum::Accepted;
    }));

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ReserveNowResponse>();
        EXPECT_EQ(response.status, ReserveNowStatusEnum::Accepted);
    }));

    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_reserve_now_specific_evse_successful) {
    // Try to make a reservation for an existing evse, which is accepted.
    std::optional<ocpp::CiString<20>> tesla_connector_type = ConnectorEnumStringType::cTesla;

    const ocpp::EnhancedMessage<MessageType> request = create_example_reserve_now_request(2, tesla_connector_type);

    EvseMock& m2 = evse_manager.get_mock(2);
    ON_CALL(m2, does_connector_exist(ConnectorEnumStringType::cTesla)).WillByDefault(Return(true));

    ON_CALL(reserve_now_callback_mock, Call(_)).WillByDefault(Invoke([](const ReserveNowRequest reserve_now_request) {
        EXPECT_TRUE(reserve_now_request.evseId.has_value());
        EXPECT_EQ(reserve_now_request.evseId.value(), 2);
        return ReserveNowStatusEnum::Accepted;
    }));

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ReserveNowResponse>();
        EXPECT_EQ(response.status, ReserveNowStatusEnum::Accepted);
    }));

    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_reserve_now_specific_evse_occupied) {
    // Try to make a reservation for a non specific evse, but all evse's are occupied.
    std::optional<ocpp::CiString<20>> tesla_connector_type = ConnectorEnumStringType::cTesla;

    const ocpp::EnhancedMessage<MessageType> request = create_example_reserve_now_request(2, tesla_connector_type);

    EvseMock& m2 = evse_manager.get_mock(2);
    ON_CALL(m2, does_connector_exist(ConnectorEnumStringType::cTesla)).WillByDefault(Return(true));

    EXPECT_CALL(reserve_now_callback_mock, Call(_)).WillOnce(Return(ReserveNowStatusEnum::Occupied));

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ReserveNowResponse>();
        EXPECT_EQ(response.status, ReserveNowStatusEnum::Occupied);
    }));

    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_cancel_reservation_reservation_not_available) {
    // Try to cancel a reservation, while Reservations is not available in the device model. This will reject the
    // request.
    set_reservation_available(this->device_model.get(), false);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<CancelReservationResponse>();
        EXPECT_EQ(response.status, CancelReservationStatusEnum::Rejected);
    }));

    const ocpp::EnhancedMessage<MessageType> request = create_example_cancel_reservation_request(2);

    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_cancel_reservation_reservation_not_enabled) {
    // Try to cancel a reservation, while Reservations is not enabled in the device model. This will reject the request.
    set_reservation_enabled(this->device_model.get(), false);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<CancelReservationResponse>();
        EXPECT_EQ(response.status, CancelReservationStatusEnum::Rejected);
    }));

    const ocpp::EnhancedMessage<MessageType> request = create_example_cancel_reservation_request(2);

    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_cancel_reservation_callback_nullptr) {
    // Try to cancel a reservation, while the cancel reservation callback is a nullptr. This will reject the request.
    Reservation r{*this->functional_block_context, reserve_now_callback_mock.AsStdFunction(), nullptr,
                  is_reservation_for_token_callback_mock.AsStdFunction()};

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<CancelReservationResponse>();
        EXPECT_EQ(response.status, CancelReservationStatusEnum::Rejected);
    }));

    const ocpp::EnhancedMessage<MessageType> request = create_example_cancel_reservation_request(2);

    r.handle_message(request);
}

TEST_F(ReservationTest, handle_cancel_reservation_accepted) {
    // Try to cancel a reservation, which is accepted.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<CancelReservationResponse>();
        EXPECT_EQ(response.status, CancelReservationStatusEnum::Accepted);
    }));

    const ocpp::EnhancedMessage<MessageType> request = create_example_cancel_reservation_request(2);

    EXPECT_CALL(cancel_reservation_callback_mock, Call(_)).WillOnce(Return(true));

    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_cancel_reservation_rejected) {
    // Try to cancel a reservation, which is rejected.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<CancelReservationResponse>();
        EXPECT_EQ(response.status, CancelReservationStatusEnum::Rejected);
    }));

    const ocpp::EnhancedMessage<MessageType> request = create_example_cancel_reservation_request(2);

    EXPECT_CALL(cancel_reservation_callback_mock, Call(_)).WillOnce(Return(false));

    this->reservation->handle_message(request);
}

TEST_F(ReservationTest, handle_message_wrong_type) {
    // Try to handle a message with the wrong type, should throw an exception.
    ResetRequest request;
    request.type = ResetEnum::Immediate;
    ocpp::Call<ResetRequest> call(request);
    ocpp::EnhancedMessage<MessageType> enhanced_message;
    enhanced_message.messageType = MessageType::Reset;
    enhanced_message.message = call;

    EXPECT_THROW(reservation->handle_message(enhanced_message), MessageTypeNotImplementedException);
}

TEST_F(ReservationTest, handle_reserve_now_no_evses) {
    // Try to make a 'global' reservation, but there are no evse's in the evse manager.
    EvseManagerFake evse_manager_no_evses{0};

    const FunctionalBlockContext b{this->mock_dispatcher,         *this->device_model,    this->connectivity_manager,
                                   evse_manager_no_evses,         this->database_handler, this->evse_security,
                                   this->component_state_manager, this->ocpp_version};
    this->functional_block_context = std::make_unique<FunctionalBlockContext>(b);

    Reservation r{*this->functional_block_context, reserve_now_callback_mock.AsStdFunction(),
                  cancel_reservation_callback_mock.AsStdFunction(),
                  is_reservation_for_token_callback_mock.AsStdFunction()};

    const ocpp::EnhancedMessage<MessageType> request =
        create_example_reserve_now_request(std::nullopt, ConnectorEnumStringType::cTesla);

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ReserveNowResponse>();
        EXPECT_EQ(response.status, ReserveNowStatusEnum::Rejected);
        ASSERT_TRUE(response.statusInfo.has_value());
        ASSERT_TRUE(response.statusInfo.value().additionalInfo.has_value());
        EXPECT_EQ(response.statusInfo.value().additionalInfo.value(), "No evse's found in charging station");
    }));

    r.handle_message(request);
}

TEST_F(ReservationTest, on_reservation_status) {
    // Call 'on_reservation_status' and check if the request is sent (to the dispatcher)
    ReservationStatusUpdateRequest request;
    request.reservationId = 3;
    request.reservationUpdateStatus = ReservationUpdateStatusEnum::Removed;
    ocpp::Call<ReservationStatusUpdateRequest> call(request);
    ocpp::EnhancedMessage<MessageType> enhanced_message;
    enhanced_message.messageType = MessageType::CancelReservation;
    enhanced_message.message = call;

    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _)).WillOnce(Invoke([](const json& call, bool triggered) {
        auto response = call[ocpp::CALL_PAYLOAD].get<ReservationStatusUpdateRequest>();
        EXPECT_EQ(response.reservationUpdateStatus, ReservationUpdateStatusEnum::Removed);
        EXPECT_EQ(response.reservationId, 3);
        EXPECT_FALSE(triggered);
    }));

    reservation->on_reservation_status(3, ReservationUpdateStatusEnum::Removed);
}

TEST_F(ReservationTest, is_evse_reserved_for_other) {
    // Call 'is_evse_reserved_for_other' and check if callback is called and the correct value is returned. In this
    // case: NotReserved.
    EXPECT_CALL(is_reservation_for_token_callback_mock, Call(42, _, _))
        .WillOnce(Return(ocpp::ReservationCheckStatus::NotReserved));

    EvseMock& m1 = evse_manager.get_mock(1);
    IdToken id_token;
    id_token.idToken = "ID_TOKEN_THINGIE";

    EXPECT_CALL(m1, get_id).WillOnce(Return(42));

    EXPECT_EQ(reservation->is_evse_reserved_for_other(m1, id_token, std::nullopt),
              ocpp::ReservationCheckStatus::NotReserved);
}

TEST_F(ReservationTest, on_reserved) {
    // Call 'on_reserved' and check if the event is submitted to the evse.
    EvseMock& m1 = evse_manager.get_mock(2);
    EXPECT_CALL(m1, submit_event(1, ConnectorEvent::Reserve)).Times(1);

    reservation->on_reserved(2, 1);
}

TEST_F(ReservationTest, on_reservation_cleared) {
    // Cann 'on_reservation_cleared' and check if the event is submitted to the evse.
    EvseMock& m1 = evse_manager.get_mock(1);
    EXPECT_CALL(m1, submit_event(1, ConnectorEvent::ReservationCleared)).Times(1);

    reservation->on_reservation_cleared(1, 1);
}
