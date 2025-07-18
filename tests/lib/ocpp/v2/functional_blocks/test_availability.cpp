// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ocpp/v2/functional_blocks/availability.hpp>

#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>

#include <ocpp/v2/messages/Heartbeat.hpp>
#include <ocpp/v2/messages/StatusNotification.hpp>

#include "component_state_manager_mock.hpp"
#include "connectivity_manager_mock.hpp"
#include "device_model_test_helper.hpp"
#include "evse_manager_fake.hpp"
// #include "evse_manager_mock.hpp"
#include "evse_mock.hpp"
#include "evse_security_mock.hpp"
#include "message_dispatcher_mock.hpp"
#include "mocks/database_handler_mock.hpp"

using namespace ocpp::v2;
using testing::_;
using testing::Invoke;
using testing::MockFunction;
using testing::Return;
using testing::ReturnRef;

using EvseIteratorImpl = ocpp::VectorOfUniquePtrIterator<EvseInterface>;

class AvailabilityTest : public ::testing::Test {
protected: // Members
    DeviceModelTestHelper device_model_test_helper;
    MockMessageDispatcher mock_dispatcher;
    DeviceModel* device_model;
    ::testing::NiceMock<ConnectivityManagerMock> connectivity_manager;
    ::testing::NiceMock<ocpp::v2::DatabaseHandlerMock> database_handler_mock;
    ocpp::EvseSecurityMock evse_security;
    EvseManagerFake evse_manager;
    ComponentStateManagerMock component_state_manager;
    std::atomic<ocpp::OcppProtocolVersion> ocpp_version;
    FunctionalBlockContext functional_block_context;
    MockFunction<void(const ocpp::DateTime& currentTime)> time_sync_callback;
    MockFunction<void()> all_connectors_unavailable_callback;
    EvseMock& evse_1;
    EvseMock& evse_2;
    std::unique_ptr<Availability> availability;

protected: // Functions
    AvailabilityTest() :
        device_model_test_helper(),
        mock_dispatcher(),
        device_model(device_model_test_helper.get_device_model()),
        connectivity_manager(),
        database_handler_mock(),
        evse_security(),
        evse_manager(2),
        component_state_manager(),
        ocpp_version(ocpp::OcppProtocolVersion::v201),
        functional_block_context{
            this->mock_dispatcher,       *this->device_model, this->connectivity_manager,    this->evse_manager,
            this->database_handler_mock, this->evse_security, this->component_state_manager, this->ocpp_version},
        evse_1(evse_manager.get_mock(1)),
        evse_2(evse_manager.get_mock(2)),
        availability(std::make_unique<Availability>(functional_block_context, time_sync_callback.AsStdFunction(),
                                                    all_connectors_unavailable_callback.AsStdFunction())) {
    }

    ocpp::EnhancedMessage<MessageType>
    create_example_change_availability_request(const OperationalStatusEnum operational_status,
                                               const std::optional<int32_t> evse_id,
                                               const std::optional<int32_t> connector_id) {
        ChangeAvailabilityRequest request;
        request.operationalStatus = operational_status;
        if (evse_id.has_value()) {
            EVSE evse;
            evse.id = evse_id.value();
            if (connector_id.has_value()) {
                evse.connectorId = connector_id.value();
            }
            request.evse = evse;
        }
        ocpp::Call<ChangeAvailabilityRequest> call(request);
        ocpp::EnhancedMessage<MessageType> enhanced_message;
        enhanced_message.messageType = MessageType::ChangeAvailability;
        enhanced_message.message = call;
        return enhanced_message;
    }

    ocpp::EnhancedMessage<MessageType> create_example_heartbeat_response(const ocpp::DateTime& current_time) {
        HeartbeatResponse response;
        response.currentTime = current_time;
        ocpp::CallResult<HeartbeatResponse> call_result(response, "uniqueId");
        ocpp::EnhancedMessage<MessageType> enhanced_message;
        enhanced_message.messageType = MessageType::HeartbeatResponse;
        enhanced_message.message = call_result;
        return enhanced_message;
    }
};

TEST_F(AvailabilityTest, heartbeat_req) {
    // When heartbeat request is called, a HeartBeatRequest should be sent to the message dispatcher.
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _)).WillOnce(Invoke([](const json& call, bool /*triggered*/) {
        const auto message = call[ocpp::CALL_PAYLOAD].get<HeartbeatRequest>();
        EXPECT_EQ(message.get_type(), "Heartbeat");
    }));
    availability->heartbeat_req(false);
}

TEST_F(AvailabilityTest, status_notification_req) {
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _)).WillOnce(Invoke([](const json& call, bool triggered) {
        const auto message = call[ocpp::CALL_PAYLOAD].get<StatusNotificationRequest>();
        EXPECT_EQ(message.connectorStatus, ConnectorStatusEnum::Unavailable);
        EXPECT_EQ(message.connectorId, 2);
        EXPECT_EQ(message.evseId, 1);
        EXPECT_LE(message.timestamp, ocpp::DateTime());
        EXPECT_FALSE(triggered);
    }));

    availability->status_notification_req(1, 2, ConnectorStatusEnum::Unavailable, false);
}

TEST_F(AvailabilityTest, handle_message_not_implemented) {
    // Only 'ChangeAvailability' and 'HeartbeatResponse' are implemented.
    ocpp::EnhancedMessage<MessageType> enhanced_message;
    enhanced_message.messageType = MessageType::ClearDisplayMessage;
    EXPECT_THROW(availability->handle_message(enhanced_message), MessageTypeNotImplementedException);
}

TEST_F(AvailabilityTest, handle_message_heartbeat_response_timesource_not_heartbeat) {
    // When a heartbeat response is received and the time source is not 'Heartbeat', the time sync callback is not
    // called.
    const auto heartbeat_response = create_example_heartbeat_response(ocpp::DateTime());
    auto time_source_variable = ControllerComponentVariables::TimeSource;
    device_model->set_value(time_source_variable.component, time_source_variable.variable.value(),
                            AttributeEnum::Actual, "NTP", "test", true);
    EXPECT_CALL(time_sync_callback, Call(_)).Times(0);
    availability->handle_message(heartbeat_response);
}

TEST_F(AvailabilityTest, handle_message_heartbeat_response_timesource_heartbeat) {
    // When a heartbeat response is received and the time source is 'Heartbeat', the time sync callback should be
    // called.
    const auto heartbeat_response = create_example_heartbeat_response(ocpp::DateTime());
    auto time_source_variable = ControllerComponentVariables::TimeSource;
    device_model->set_value(time_source_variable.component, time_source_variable.variable.value(),
                            AttributeEnum::Actual, "Heartbeat", "test", true);
    EXPECT_CALL(time_sync_callback, Call(_)).Times(1);
    availability->handle_message(heartbeat_response);
}

TEST_F(AvailabilityTest, handle_scheduled_changed_availability_requests_nothing_scheduled) {
    // Call handle_scheduled_change_availability_requests without having any change availability requests scheduled.
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(false));
    ON_CALL(evse_1, get_connector_effective_operational_status(_))
        .WillByDefault(Return(OperationalStatusEnum::Inoperative));
    ON_CALL(evse_2, get_connector_effective_operational_status(_))
        .WillByDefault(Return(OperationalStatusEnum::Inoperative));
    EXPECT_CALL(all_connectors_unavailable_callback, Call()).Times(0);
    this->availability->handle_scheduled_change_availability_requests(1);
}

TEST_F(AvailabilityTest, handle_scheduled_changed_availability_requests_transaction_active) {
    // Call handle_scheduled_change_availability_requests with a current active transaction. This will not call the
    // all_connectors_unavailable callback.
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(true));
    ON_CALL(evse_1, get_connector_effective_operational_status(_))
        .WillByDefault(Return(OperationalStatusEnum::Inoperative));
    ON_CALL(evse_2, get_connector_effective_operational_status(_))
        .WillByDefault(Return(OperationalStatusEnum::Inoperative));
    EVSE evse;
    evse.id = 1;
    AvailabilityChange change;
    change.persist = false;
    change.request.evse = evse;
    change.request.operationalStatus = OperationalStatusEnum::Inoperative;
    this->availability->set_scheduled_change_availability_requests(1, change);
    EXPECT_CALL(all_connectors_unavailable_callback, Call()).Times(0);
    this->availability->handle_scheduled_change_availability_requests(1);
}

TEST_F(AvailabilityTest, handle_scheduled_changed_availability_requests_no_transaction_active_not_all_inoperative) {
    // Call handle_scheduled_change_availability_requests with no active transaction, but not all connectors are
    // inoperative. This will not call the all_connectors_unavailable callback.
    // Only an evse will be set to inoperative.
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(false));
    ON_CALL(evse_1, get_connector_effective_operational_status(_))
        .WillByDefault(Return(OperationalStatusEnum::Operative));
    ON_CALL(evse_2, get_connector_effective_operational_status(_))
        .WillByDefault(Return(OperationalStatusEnum::Operative));
    ON_CALL(evse_1, get_number_of_connectors()).WillByDefault(Return(1));
    ON_CALL(evse_2, get_number_of_connectors()).WillByDefault(Return(1));
    ON_CALL(evse_1, has_active_transaction(_)).WillByDefault(Return(false));
    ON_CALL(evse_2, has_active_transaction(_)).WillByDefault(Return(false));
    EXPECT_CALL(evse_1, set_evse_operative_status(OperationalStatusEnum::Inoperative, false));
    EVSE evse;
    evse.id = 1;
    AvailabilityChange change;
    change.persist = false;
    change.request.evse = evse;
    change.request.operationalStatus = OperationalStatusEnum::Inoperative;
    this->availability->set_scheduled_change_availability_requests(1, change);
    EXPECT_CALL(all_connectors_unavailable_callback, Call()).Times(0);
    this->availability->handle_scheduled_change_availability_requests(1);
}

TEST_F(AvailabilityTest, handle_scheduled_changed_availability_requests_no_transaction_active_all_inoperative) {
    // Call handle_scheduled_change_availability_requests with no active transaction, and all connectors are
    // inoperative. This will call the all_connectors_unavailable callback.
    // This time a connector is set to inoperative and persist is true.
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(false));
    ON_CALL(evse_1, get_connector_effective_operational_status(_))
        .WillByDefault(Return(OperationalStatusEnum::Inoperative));
    ON_CALL(evse_2, get_connector_effective_operational_status(_))
        .WillByDefault(Return(OperationalStatusEnum::Inoperative));
    ON_CALL(evse_1, get_number_of_connectors()).WillByDefault(Return(1));
    ON_CALL(evse_2, get_number_of_connectors()).WillByDefault(Return(1));
    ON_CALL(evse_1, has_active_transaction(_)).WillByDefault(Return(false));
    ON_CALL(evse_2, has_active_transaction(_)).WillByDefault(Return(false));
    EXPECT_CALL(evse_1, set_connector_operative_status(2, OperationalStatusEnum::Inoperative, true));
    EVSE evse;
    evse.id = 1;
    evse.connectorId = 2;
    AvailabilityChange change;
    change.persist = true;
    change.request.evse = evse;
    change.request.operationalStatus = OperationalStatusEnum::Inoperative;
    this->availability->set_scheduled_change_availability_requests(1, change);
    EXPECT_CALL(all_connectors_unavailable_callback, Call()).Times(1);
    this->availability->handle_scheduled_change_availability_requests(1);
}

TEST_F(AvailabilityTest, handle_scheduled_change_availability_requests_negative_evseid) {
    // Call handle_scheduled_change_availability_requests with a non existing (negative) evse id. This will not call any
    // callback.
    EXPECT_CALL(evse_manager, any_transaction_active(_)).Times(0);
    EVSE evse;
    evse.id = 1;
    evse.connectorId = 2;
    AvailabilityChange change;
    change.persist = true;
    change.request.evse = evse;
    change.request.operationalStatus = OperationalStatusEnum::Inoperative;
    this->availability->set_scheduled_change_availability_requests(1, change);
    EXPECT_CALL(all_connectors_unavailable_callback, Call()).Times(0);
    this->availability->handle_scheduled_change_availability_requests(-9999);
}

TEST_F(AvailabilityTest, handle_message_change_availability_cs_operative) {
    ocpp::EnhancedMessage<MessageType> request =
        create_example_change_availability_request(OperationalStatusEnum::Operative, std::nullopt, std::nullopt);

    // No EVSE, but if it requests if it's valid, return true. No transaction active. Operational status is Operative.
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(false));
    ON_CALL(component_state_manager, get_cs_individual_operational_status())
        .WillByDefault(Return(OperationalStatusEnum::Operative));
    EXPECT_CALL(component_state_manager, set_cs_individual_operational_status(OperationalStatusEnum::Operative, true));

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        const auto message = call_result[ocpp::CALLRESULT_PAYLOAD].get<ChangeAvailabilityResponse>();
        EXPECT_EQ(message.status, ChangeAvailabilityStatusEnum::Accepted);
    }));

    this->availability->handle_message(request);
}

TEST_F(AvailabilityTest, handle_message_change_availability_cs_inoperative_transaction_active) {
    ocpp::EnhancedMessage<MessageType> request =
        create_example_change_availability_request(OperationalStatusEnum::Inoperative, std::nullopt, std::nullopt);

    // No EVSE, but if it requests if it's valid, return true. There is an active transaction. Operational status is
    // currently Operative.
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(true));
    ON_CALL(component_state_manager, get_cs_individual_operational_status())
        .WillByDefault(Return(OperationalStatusEnum::Operative));
    EXPECT_CALL(component_state_manager, set_cs_individual_operational_status(OperationalStatusEnum::Operative, true))
        .Times(0);

    // The CS will be scheduled to set to inoperative, but all evse's that do not have an active transaction will
    // already be set to inoperative.
    ON_CALL(evse_1, has_active_transaction()).WillByDefault(Return(false));
    ON_CALL(evse_2, has_active_transaction()).WillByDefault(Return(true));

    // EVSE 1 has no active transaction, EVSE 2 has, so EVSE 1 will be set to inoperative.
    EXPECT_CALL(evse_1, set_evse_operative_status(OperationalStatusEnum::Inoperative, false));
    EXPECT_CALL(evse_2, set_evse_operative_status(OperationalStatusEnum::Inoperative, false)).Times(0);

    // And since there is an active transaction, changing of the availability will be scheduled.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        const auto message = call_result[ocpp::CALLRESULT_PAYLOAD].get<ChangeAvailabilityResponse>();
        EXPECT_EQ(message.status, ChangeAvailabilityStatusEnum::Scheduled);
    }));

    this->availability->handle_message(request);
}

TEST_F(AvailabilityTest, handle_message_change_availability_cs_inoperative_transaction_not_active) {
    ocpp::EnhancedMessage<MessageType> request =
        create_example_change_availability_request(OperationalStatusEnum::Inoperative, std::nullopt, std::nullopt);

    // No EVSE, but if it requests if it's valid, return true. There is no active transaction. Operational status is
    // currently Operative.
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(false));
    ON_CALL(component_state_manager, get_cs_individual_operational_status())
        .WillByDefault(Return(OperationalStatusEnum::Operative));

    // The CS will be scheduled to set to inoperative, but all evse's that do not have an active transaction will
    // already be set to inoperative.
    ON_CALL(evse_1, has_active_transaction()).WillByDefault(Return(false));
    ON_CALL(evse_2, has_active_transaction()).WillByDefault(Return(false));

    // CS has no active transaction, so it will be set to inoperative.
    EXPECT_CALL(component_state_manager,
                set_cs_individual_operational_status(OperationalStatusEnum::Inoperative, true));

    // And since there is an active transaction, changing of the availability will be scheduled.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        const auto message = call_result[ocpp::CALLRESULT_PAYLOAD].get<ChangeAvailabilityResponse>();
        EXPECT_EQ(message.status, ChangeAvailabilityStatusEnum::Accepted);
    }));

    this->availability->handle_message(request);
}

TEST_F(AvailabilityTest, handle_message_change_availability_cs_operative_transaction_not_active) {
    ocpp::EnhancedMessage<MessageType> request =
        create_example_change_availability_request(OperationalStatusEnum::Operative, std::nullopt, std::nullopt);

    // No EVSE, but if it requests if it's valid, return true. There is no active transaction. Operational status is
    // currently Operative.
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(false));
    ON_CALL(component_state_manager, get_cs_individual_operational_status())
        .WillByDefault(Return(OperationalStatusEnum::Operative));

    // The CS will be scheduled to set to inoperative, but all evse's that do not have an active transaction will
    // already be set to inoperative.
    ON_CALL(evse_1, has_active_transaction()).WillByDefault(Return(false));
    ON_CALL(evse_2, has_active_transaction()).WillByDefault(Return(false));

    // EVSE's have no active transaction, so they will be set to inoperative.
    EXPECT_CALL(component_state_manager, set_cs_individual_operational_status(OperationalStatusEnum::Operative, true));

    // And since the CS is already in the Operative state, the status is 'accepted'.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        const auto message = call_result[ocpp::CALLRESULT_PAYLOAD].get<ChangeAvailabilityResponse>();
        EXPECT_EQ(message.status, ChangeAvailabilityStatusEnum::Accepted);
    }));

    this->availability->handle_message(request);
}

TEST_F(AvailabilityTest, handle_message_change_availability_wrong_evse) {
    ocpp::EnhancedMessage<MessageType> request =
        create_example_change_availability_request(OperationalStatusEnum::Operative, 42, std::nullopt);

    // Because the evse id does not exist, 'Rejected' is returned.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        const auto message = call_result[ocpp::CALLRESULT_PAYLOAD].get<ChangeAvailabilityResponse>();
        EXPECT_EQ(message.status, ChangeAvailabilityStatusEnum::Rejected);
    }));

    this->availability->handle_message(request);
}

TEST_F(AvailabilityTest, handle_message_change_availability_evse_operative_transaction_active) {
    ocpp::EnhancedMessage<MessageType> request =
        create_example_change_availability_request(OperationalStatusEnum::Operative, 2, std::nullopt);

    // Change availability for an evse when there is an active transaction
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(true));
    ON_CALL(evse_1, has_active_transaction()).WillByDefault(Return(true));
    ON_CALL(evse_2, has_active_transaction()).WillByDefault(Return(true));
    ON_CALL(component_state_manager, get_evse_individual_operational_status(_))
        .WillByDefault(Return(OperationalStatusEnum::Operative));

    // And since the EVSE is already in the Operative state, 'Accepted' is returned.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        const auto message = call_result[ocpp::CALLRESULT_PAYLOAD].get<ChangeAvailabilityResponse>();
        EXPECT_EQ(message.status, ChangeAvailabilityStatusEnum::Accepted);
    }));

    EXPECT_CALL(component_state_manager, set_cs_individual_operational_status(OperationalStatusEnum::Operative, _))
        .Times(0);
    EXPECT_CALL(evse_1, set_evse_operative_status(_, _)).Times(0);
    EXPECT_CALL(evse_2, set_evse_operative_status(_, _)).Times(0);

    this->availability->handle_message(request);
}

TEST_F(AvailabilityTest, handle_message_change_availability_connector_operative_transaction_inactive) {
    ocpp::EnhancedMessage<MessageType> request =
        create_example_change_availability_request(OperationalStatusEnum::Operative, 2, 1);

    // Change availability for a connector when there is no active transaction and the connector is currently
    // inoperative
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(false));
    ON_CALL(evse_1, has_active_transaction()).WillByDefault(Return(false));
    ON_CALL(evse_2, has_active_transaction()).WillByDefault(Return(false));
    ON_CALL(component_state_manager, get_evse_individual_operational_status(_))
        .WillByDefault(Return(OperationalStatusEnum::Inoperative));
    ON_CALL(component_state_manager, get_connector_individual_operational_status(_, _))
        .WillByDefault(Return(OperationalStatusEnum::Inoperative));

    // And since the EVSE is already in the Operative state, 'Accepted' is returned.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        const auto message = call_result[ocpp::CALLRESULT_PAYLOAD].get<ChangeAvailabilityResponse>();
        EXPECT_EQ(message.status, ChangeAvailabilityStatusEnum::Accepted);
    }));

    EXPECT_CALL(component_state_manager, set_cs_individual_operational_status(OperationalStatusEnum::Operative, _))
        .Times(0);
    EXPECT_CALL(evse_1, set_connector_operative_status(1, OperationalStatusEnum::Operative, _)).Times(0);
    EXPECT_CALL(evse_2, set_connector_operative_status(1, OperationalStatusEnum::Operative, _));

    this->availability->handle_message(request);
}

TEST_F(AvailabilityTest, handle_message_change_availability_evse_inoperative_transaction_active) {
    ocpp::EnhancedMessage<MessageType> request =
        create_example_change_availability_request(OperationalStatusEnum::Inoperative, 1, std::nullopt);

    // Change availability to inoperative for an evse when there is an active transaction and the connector is currently
    // operative
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(true));
    ON_CALL(evse_1, has_active_transaction()).WillByDefault(Return(true));
    ON_CALL(evse_2, has_active_transaction()).WillByDefault(Return(true));
    ON_CALL(component_state_manager, get_evse_individual_operational_status(_))
        .WillByDefault(Return(OperationalStatusEnum::Operative));
    ON_CALL(component_state_manager, get_connector_individual_operational_status(_, _))
        .WillByDefault(Return(OperationalStatusEnum::Operative));

    // And since the EVSE is already in the Operative state, 'Accepted' is returned.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        const auto message = call_result[ocpp::CALLRESULT_PAYLOAD].get<ChangeAvailabilityResponse>();
        EXPECT_EQ(message.status, ChangeAvailabilityStatusEnum::Scheduled);
    }));

    EXPECT_CALL(component_state_manager, set_cs_individual_operational_status(OperationalStatusEnum::Inoperative, _))
        .Times(0);
    EXPECT_CALL(evse_1, set_evse_operative_status(OperationalStatusEnum::Inoperative, _)).Times(0);
    EXPECT_CALL(evse_2, set_evse_operative_status(OperationalStatusEnum::Inoperative, _)).Times(0);

    this->availability->handle_message(request);
}

TEST_F(AvailabilityTest, handle_message_change_availability_evse_inoperative_transaction_active_2) {
    ocpp::EnhancedMessage<MessageType> request =
        create_example_change_availability_request(OperationalStatusEnum::Inoperative, 1, std::nullopt);

    // Change availability to inoperative for an evse when there is an active transaction and the connector is currently
    // operative
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(true));
    ON_CALL(evse_1, has_active_transaction()).WillByDefault(Return(false));
    ON_CALL(evse_2, has_active_transaction()).WillByDefault(Return(true));
    ON_CALL(evse_1, get_number_of_connectors()).WillByDefault(Return(2));
    ON_CALL(evse_2, get_number_of_connectors()).WillByDefault(Return(2));
    ON_CALL(component_state_manager, get_evse_individual_operational_status(_))
        .WillByDefault(Return(OperationalStatusEnum::Operative));
    ON_CALL(component_state_manager, get_connector_individual_operational_status(_, _))
        .WillByDefault(Return(OperationalStatusEnum::Operative));

    // And since the EVSE is already in the Operative state, 'Accepted' is returned.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        const auto message = call_result[ocpp::CALLRESULT_PAYLOAD].get<ChangeAvailabilityResponse>();
        EXPECT_EQ(message.status, ChangeAvailabilityStatusEnum::Scheduled);
    }));

    // All connectors of the evse are set to inoperative.
    EXPECT_CALL(component_state_manager, set_cs_individual_operational_status(OperationalStatusEnum::Inoperative, _))
        .Times(0);
    EXPECT_CALL(evse_1, set_connector_operative_status(1, OperationalStatusEnum::Inoperative, _));
    EXPECT_CALL(evse_1, set_connector_operative_status(2, OperationalStatusEnum::Inoperative, _));
    EXPECT_CALL(evse_2, set_connector_operative_status(1, OperationalStatusEnum::Inoperative, _)).Times(0);

    this->availability->handle_message(request);
}

TEST_F(AvailabilityTest,
       handle_message_change_availability_evse_inoperative_transaction_active_connector_has_active_transaction) {
    ocpp::EnhancedMessage<MessageType> request =
        create_example_change_availability_request(OperationalStatusEnum::Inoperative, 2, std::nullopt);

    // Change availability to inoperative for an evse when there is an active transaction and the connector is currently
    // operative
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(true));
    ON_CALL(evse_1, has_active_transaction()).WillByDefault(Return(false));
    ON_CALL(evse_2, has_active_transaction()).WillByDefault(Return(true));
    ON_CALL(evse_2, has_active_transaction(1)).WillByDefault(Return(false));
    // Active transaction on connector 2, which will set connector 1 to inoperative and not touch connector 2.
    ON_CALL(evse_2, has_active_transaction(2)).WillByDefault(Return(true));
    ON_CALL(evse_1, get_number_of_connectors()).WillByDefault(Return(2));
    ON_CALL(evse_2, get_number_of_connectors()).WillByDefault(Return(2));
    ON_CALL(component_state_manager, get_evse_individual_operational_status(_))
        .WillByDefault(Return(OperationalStatusEnum::Operative));
    ON_CALL(component_state_manager, get_connector_individual_operational_status(_, _))
        .WillByDefault(Return(OperationalStatusEnum::Operative));

    // And since the EVSE is already in the Operative state, 'Accepted' is returned.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        const auto message = call_result[ocpp::CALLRESULT_PAYLOAD].get<ChangeAvailabilityResponse>();
        EXPECT_EQ(message.status, ChangeAvailabilityStatusEnum::Scheduled);
    }));

    // All connectors of the evse are set to inoperative.
    EXPECT_CALL(component_state_manager, set_cs_individual_operational_status(OperationalStatusEnum::Inoperative, _))
        .Times(0);
    EXPECT_CALL(evse_1, set_connector_operative_status(1, OperationalStatusEnum::Inoperative, _)).Times(0);
    EXPECT_CALL(evse_1, set_connector_operative_status(2, OperationalStatusEnum::Inoperative, _)).Times(0);
    // Connector 1 of evse 2 is set to inoperative because it has no active transaction, connector 2 has an active
    // transaction so the operational status is not changed.
    EXPECT_CALL(evse_2, set_connector_operative_status(1, OperationalStatusEnum::Inoperative, _));
    EXPECT_CALL(evse_2, set_connector_operative_status(2, OperationalStatusEnum::Inoperative, _)).Times(0);

    this->availability->handle_message(request);

    // There is now a scheduled change availability request. Let's stop the transaction and check if the evse will
    // be set to Inoperative now.
    ON_CALL(evse_manager, any_transaction_active(_)).WillByDefault(Return(false));
    EXPECT_CALL(evse_2, set_evse_operative_status(OperationalStatusEnum::Inoperative, _));
    this->availability->handle_scheduled_change_availability_requests(2);
}

TEST_F(AvailabilityTest, set_heartbeat_timer_interval) {
    // When setting the heartbeat timer interval, a heartbeat request should be sent after the interval has ended.
    EXPECT_CALL(mock_dispatcher, dispatch_call(_, _)).WillOnce(Invoke([](const json& call, bool /*triggered*/) {
        const auto message = call[ocpp::CALL_PAYLOAD].get<HeartbeatRequest>();
        EXPECT_EQ(message.get_type(), "Heartbeat");
    }));

    // std::chrono::milliseconds ms(100);
    this->availability->set_heartbeat_timer_interval(std::chrono::seconds(1));
    usleep(1200000);
}
