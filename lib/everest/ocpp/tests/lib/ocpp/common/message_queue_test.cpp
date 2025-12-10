// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include "ocpp/v16/types.hpp"
#include <gtest/gtest.h>

#include <memory>
#include <ocpp/common/message_queue.hpp>

namespace {
using namespace ocpp;

#if 0
    MessageQueue(
        const std::function<bool(json message)>& send_callback, const MessageQueueConfig<M>& config,
        const std::vector<M>& external_notify, std::shared_ptr<common::DatabaseHandlerCommon> database_handler,
        const std::function<void(const std::string& new_message_id, const std::string& old_message_id)>
            start_transaction_message_retry_callback =
                [](const std::string& new_message_id, const std::string& old_message_id) {})
#endif

bool send_callback(json message) {
    return true;
}

void start_transaction_message_retry_callback(const std::string& new_message_id, const std::string& old_message_id) {
}

struct DatabaseHandlerCommonTest : public common::DatabaseHandlerCommon {
    DatabaseHandlerCommonTest() :
        common::DatabaseHandlerCommon(std::unique_ptr<common::DatabaseConnectionInterface>{}, "", 1) {
    }
    void init_sql() override {
    }
};

TEST(MessageQueue, init) {
    MessageQueueConfig<v16::MessageType> config;
    std::vector<v16::MessageType> external_notify;
    std::shared_ptr<common::DatabaseHandlerCommon> database_handler = std::make_shared<DatabaseHandlerCommonTest>();

    MessageQueue<v16::MessageType> queue(&send_callback, config, external_notify, database_handler,
                                         &start_transaction_message_retry_callback);

    queue.start();
    queue.stop();
}

} // namespace