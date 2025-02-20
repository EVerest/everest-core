// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <message_dispatcher_mock.hpp>
#include <ocpp/common/constants.hpp>
#include <ocpp/v2/functional_blocks/data_transfer.hpp>
#include <ocpp/v2/messages/DataTransfer.hpp>

using namespace ocpp::v2;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

DataTransferRequest create_example_request() {
    DataTransferRequest request;
    request.vendorId = "TestVendor";
    request.messageId = "TestMessage";
    request.data = json{{"key", "value"}};
    return request;
}

TEST(DataTransferTest, HandleDataTransferReq_NotImplemented) {
    MockMessageDispatcher mock_dispatcher;
    DataTransfer data_transfer(mock_dispatcher, std::nullopt, ocpp::DEFAULT_WAIT_FOR_FUTURE_TIMEOUT);

    DataTransferRequest request = create_example_request();
    ocpp::Call<DataTransferRequest> call(request);
    ocpp::EnhancedMessage<MessageType> enhanced_message;
    enhanced_message.messageType = MessageType::Authorize; // this cant be handled by DataTransfer functional block
    enhanced_message.message = call;

    EXPECT_THROW(data_transfer.handle_message(enhanced_message), MessageTypeNotImplementedException);
}

TEST(DataTransferTest, HandleDataTransferReq_NoCallback) {
    MockMessageDispatcher mock_dispatcher;
    DataTransfer data_transfer(mock_dispatcher, std::nullopt, ocpp::DEFAULT_WAIT_FOR_FUTURE_TIMEOUT);

    DataTransferRequest request = create_example_request();
    ocpp::Call<DataTransferRequest> call(request);
    ocpp::EnhancedMessage<MessageType> enhanced_message;
    enhanced_message.messageType = MessageType::DataTransfer;
    enhanced_message.message = call;

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<DataTransferResponse>();
        EXPECT_EQ(response.status, DataTransferStatusEnum::UnknownVendorId);
    }));

    data_transfer.handle_message(enhanced_message);
}

TEST(DataTransferTest, HandleDataTransferReq_WithCallback) {
    MockMessageDispatcher mock_dispatcher;

    auto callback = [](const DataTransferRequest&) {
        DataTransferResponse response;
        response.status = DataTransferStatusEnum::Accepted;
        return response;
    };

    DataTransfer data_transfer(mock_dispatcher, callback, ocpp::DEFAULT_WAIT_FOR_FUTURE_TIMEOUT);

    DataTransferRequest request = create_example_request();
    ocpp::Call<DataTransferRequest> call(request);
    ocpp::EnhancedMessage<MessageType> enhanced_message;
    enhanced_message.messageType = MessageType::DataTransfer;
    enhanced_message.message = call;

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<DataTransferResponse>();
        EXPECT_EQ(response.status, DataTransferStatusEnum::Accepted);
    }));

    data_transfer.handle_message(enhanced_message);
}

TEST(DataTransferTest, DataTransferReq_Offline) {
    MockMessageDispatcher mock_dispatcher;
    DataTransfer data_transfer(mock_dispatcher, std::nullopt, ocpp::DEFAULT_WAIT_FOR_FUTURE_TIMEOUT);

    DataTransferRequest request = create_example_request();

    ocpp::EnhancedMessage<MessageType> offline_message;
    offline_message.offline = true;

    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _))
        .WillOnce(Return(std::async(std::launch::deferred, [offline_message]() { return offline_message; })));

    auto response = data_transfer.data_transfer_req(request);

    EXPECT_FALSE(response.has_value());
}

TEST(DataTransferTest, DataTransferReq_Timeout) {
    MockMessageDispatcher mock_dispatcher;
    DataTransfer data_transfer(mock_dispatcher, std::nullopt, std::chrono::seconds(1));

    DataTransferRequest request = create_example_request();

    auto timeout_future = std::async(std::launch::async, []() -> ocpp::EnhancedMessage<MessageType> {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return {};
    });

    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::move(timeout_future)));

    auto response = data_transfer.data_transfer_req(request);

    EXPECT_FALSE(response.has_value());
}

TEST(DataTransferTest, DataTransferReq_Accepted) {
    MockMessageDispatcher mock_dispatcher;
    DataTransfer data_transfer(mock_dispatcher, std::nullopt, ocpp::DEFAULT_WAIT_FOR_FUTURE_TIMEOUT);

    DataTransferRequest request = create_example_request();

    DataTransferResponse expected_response;
    expected_response.status = DataTransferStatusEnum::Accepted;

    ocpp::CallResult<DataTransferResponse> call_result(expected_response, "uniqueId");

    ocpp::EnhancedMessage<MessageType> enhanced_message;
    enhanced_message.messageType = MessageType::DataTransferResponse;
    enhanced_message.message = call_result;

    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _))
        .WillOnce(Return(std::async(std::launch::deferred, [enhanced_message]() { return enhanced_message; })));

    auto response = data_transfer.data_transfer_req(request.vendorId, request.messageId, request.data);

    ASSERT_TRUE(response.has_value());
    EXPECT_EQ(response->status, DataTransferStatusEnum::Accepted);
}

TEST(DataTransferTest, DataTransferReq_EnumConversionException) {
    MockMessageDispatcher mock_dispatcher;
    DataTransfer data_transfer(mock_dispatcher, std::nullopt, ocpp::DEFAULT_WAIT_FOR_FUTURE_TIMEOUT);

    DataTransferRequest request = create_example_request();

    ocpp::EnhancedMessage<MessageType> enhanced_message;
    enhanced_message.offline = false;
    enhanced_message.messageType = MessageType::DataTransferResponse;
    enhanced_message.uniqueId = "unique-id-123";
    enhanced_message.message =
        json::parse("[3, \"unique-id-123\", {\"status\": \"Wrong\"}]"); // will cause a throw of EnumConversionException

    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _))
        .WillOnce(Return(std::async(std::launch::deferred, [enhanced_message]() -> ocpp::EnhancedMessage<MessageType> {
            return enhanced_message;
        })));

    EXPECT_CALL(mock_dispatcher, dispatch_call_error(_)).WillOnce([](const ocpp::CallError& call_error) {
        EXPECT_EQ(call_error.errorCode, "FormationViolation");
    });

    auto result = data_transfer.data_transfer_req(request);

    EXPECT_FALSE(result.has_value());
}

TEST(DataTransferTest, DataTransferReq_JsonException) {
    MockMessageDispatcher mock_dispatcher;
    DataTransfer data_transfer(mock_dispatcher, std::nullopt, ocpp::DEFAULT_WAIT_FOR_FUTURE_TIMEOUT);

    DataTransferRequest request = create_example_request();

    ocpp::EnhancedMessage<MessageType> enhanced_message;
    enhanced_message.offline = false;
    enhanced_message.messageType = MessageType::DataTransferResponse;
    enhanced_message.uniqueId = "unique-id-123";
    enhanced_message.message = "{NoValidJson"; // will cause a throw of json exception

    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _))
        .WillOnce(Return(std::async(std::launch::deferred, [enhanced_message]() -> ocpp::EnhancedMessage<MessageType> {
            return enhanced_message;
        })));

    EXPECT_CALL(mock_dispatcher, dispatch_call_error(_)).WillOnce([](const ocpp::CallError& call_error) {
        EXPECT_EQ(call_error.errorCode, "FormationViolation");
    });

    auto result = data_transfer.data_transfer_req(request);

    EXPECT_FALSE(result.has_value());
}
