// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <ocpp/v16/utils.hpp>

namespace ocpp {
namespace v16 {

class UtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(UtilsTest, test_drop_transaction_data) {
    auto call = ocpp::Call<StopTransactionRequest>();
    ASSERT_FALSE(call.msg.transactionData.has_value());

    std::vector<TransactionData> transaction_data = {
        {DateTime(), {{"1"}}},
        {DateTime(), {{"1"}, {"2"}}},
        {DateTime(), {{"1"}, {"2"}, {"3"}}},
        {DateTime(), {{"1"}, {"2"}, {"3"}, {"4"}}},
        {DateTime(), {{"1"}, {"2"}, {"3"}, {"4"}, {"5"}}},
    };

    ASSERT_EQ(transaction_data.size(), 5);
    call.msg.transactionData = transaction_data;
    ASSERT_TRUE(call.msg.transactionData.has_value());
    ASSERT_EQ(call.msg.transactionData.value().at(0).sampledValue.size(), 1);
    ASSERT_EQ(call.msg.transactionData.value().at(1).sampledValue.size(), 2);
    ASSERT_EQ(call.msg.transactionData.value().at(2).sampledValue.size(), 3);
    ASSERT_EQ(call.msg.transactionData.value().at(3).sampledValue.size(), 4);
    ASSERT_EQ(call.msg.transactionData.value().at(4).sampledValue.size(), 5);
    utils::drop_transaction_data(500, call);
    ASSERT_EQ(call.msg.transactionData.value().size(), 3);
    ASSERT_EQ(call.msg.transactionData.value().at(0).sampledValue.size(), 1);
    ASSERT_EQ(call.msg.transactionData.value().at(1).sampledValue.size(), 4);
    ASSERT_EQ(call.msg.transactionData.value().at(2).sampledValue.size(), 5);
}

} // namespace v16
} // namespace ocpp
