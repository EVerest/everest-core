// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include <ocpp/common/message_queue.hpp>
#include <ocpp/v201/messages/Authorize.hpp>

namespace ocpp {

namespace v201 {

class ControlMessageV201Test : public ::testing::Test {

protected:
};

TEST_F(ControlMessageV201Test, test_is_transactional) {

    EXPECT_TRUE(is_transaction_message(
        (ControlMessage<v201::MessageType>{Call<v201::TransactionEventRequest>{v201::TransactionEventRequest{}}}
             .messageType)));

    EXPECT_TRUE(!is_transaction_message(
        ControlMessage<v201::MessageType>{Call<v201::AuthorizeRequest>{v201::AuthorizeRequest{}}}.messageType));
}

TEST_F(ControlMessageV201Test, test_is_transactional_update) {

    v201::TransactionEventRequest transaction_event_request{};
    transaction_event_request.eventType = v201::TransactionEventEnum::Updated;

    EXPECT_TRUE((ControlMessage<v201::MessageType>{Call<v201::TransactionEventRequest>{transaction_event_request}}
                     .is_transaction_update_message()));

    transaction_event_request.eventType = v201::TransactionEventEnum::Started;
    EXPECT_TRUE(!(ControlMessage<v201::MessageType>{Call<v201::TransactionEventRequest>{transaction_event_request}}
                      .is_transaction_update_message()));

    transaction_event_request.eventType = v201::TransactionEventEnum::Ended;
    EXPECT_TRUE(!(ControlMessage<v201::MessageType>{Call<v201::TransactionEventRequest>{transaction_event_request}}
                      .is_transaction_update_message()));

    EXPECT_TRUE(!(ControlMessage<v201::MessageType>{Call<v201::AuthorizeRequest>{v201::AuthorizeRequest{}}}
                      .is_transaction_update_message()));
}

} // namespace v201
} // namespace ocpp
