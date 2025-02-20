// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include <ocpp/common/message_queue.hpp>
#include <ocpp/v2/messages/Authorize.hpp>

namespace ocpp {

namespace v2 {

class ControlMessageV2Test : public ::testing::Test {

protected:
};

TEST_F(ControlMessageV2Test, test_is_transactional) {

    EXPECT_TRUE(is_transaction_message((ControlMessage<v2::MessageType>{
        Call<v2::TransactionEventRequest>{
            v2::TransactionEventRequest{}}}.messageType)));

    EXPECT_TRUE(!is_transaction_message(
        ControlMessage<v2::MessageType>{Call<v2::AuthorizeRequest>{v2::AuthorizeRequest{}}}.messageType));
}

TEST_F(ControlMessageV2Test, test_is_transactional_update) {

    v2::TransactionEventRequest transaction_event_request{};
    transaction_event_request.eventType = v2::TransactionEventEnum::Updated;

    EXPECT_TRUE((ControlMessage<v2::MessageType>{Call<v2::TransactionEventRequest>{transaction_event_request}}
                     .is_transaction_update_message()));

    transaction_event_request.eventType = v2::TransactionEventEnum::Started;
    EXPECT_TRUE(!(ControlMessage<v2::MessageType>{Call<v2::TransactionEventRequest>{transaction_event_request}}
                      .is_transaction_update_message()));

    transaction_event_request.eventType = v2::TransactionEventEnum::Ended;
    EXPECT_TRUE(!(ControlMessage<v2::MessageType>{Call<v2::TransactionEventRequest>{transaction_event_request}}
                      .is_transaction_update_message()));

    EXPECT_TRUE(!(ControlMessage<v2::MessageType>{Call<v2::AuthorizeRequest>{v2::AuthorizeRequest{}}}
                      .is_transaction_update_message()));
}

} // namespace v2
} // namespace ocpp
