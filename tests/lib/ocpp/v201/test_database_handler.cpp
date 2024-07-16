// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include "database_testing_utils.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ocpp/v201/database_handler.hpp>

using namespace ocpp;
using namespace ocpp::v201;

class DatabaseHandlerTest : public DatabaseTestingUtils {
public:
    DatabaseHandler database_handler{std::make_unique<DatabaseConnection>("file::memory:?cache=shared"),
                                     std::filesystem::path(MIGRATION_FILES_LOCATION_V201)};

    DatabaseHandlerTest() {
        this->database_handler.open_connection();
    }

    std::unique_ptr<EnhancedTransaction> default_transaction() {
        auto transaction = std::make_unique<EnhancedTransaction>(this->database_handler, true);
        transaction->transactionId = "txId";
        transaction->connector_id = 1;
        transaction->start_time = DateTime{"2024-07-15T08:01:02Z"};
        transaction->seq_no = 10;
        transaction->chargingState = ChargingStateEnum::SuspendedEV;
        transaction->id_token_sent = true;

        return transaction;
    }
};

TEST_F(DatabaseHandlerTest, TransactionInsertAndGet) {
    constexpr int32_t evse_id = 1;

    auto transaction = default_transaction();

    this->database_handler.transaction_insert(*transaction, evse_id);

    auto transaction_get = this->database_handler.transaction_get(evse_id);

    EXPECT_NE(transaction_get, nullptr);
    EXPECT_EQ(transaction->transactionId, transaction_get->transactionId);
    EXPECT_EQ(transaction->connector_id, transaction_get->connector_id);
    EXPECT_EQ(transaction->start_time, transaction_get->start_time);
    EXPECT_EQ(transaction->seq_no, transaction_get->seq_no);
    EXPECT_EQ(transaction->chargingState, transaction_get->chargingState);
    EXPECT_EQ(transaction->id_token_sent, transaction_get->id_token_sent);
}

TEST_F(DatabaseHandlerTest, TransactionGetNotFound) {
    constexpr int32_t evse_id = 1;
    auto transaction_get = this->database_handler.transaction_get(evse_id);
    EXPECT_EQ(transaction_get, nullptr);
}

TEST_F(DatabaseHandlerTest, TransactionInsertDuplicateTransactionId) {
    constexpr int32_t evse_id = 1;

    auto transaction = default_transaction();

    this->database_handler.transaction_insert(*transaction, evse_id);

    EXPECT_THROW(this->database_handler.transaction_insert(*transaction, evse_id + 1), DatabaseException);
}

TEST_F(DatabaseHandlerTest, TransactionInsertDuplicateEvseId) {
    constexpr int32_t evse_id = 1;

    auto transaction = default_transaction();

    this->database_handler.transaction_insert(*transaction, evse_id);

    transaction->transactionId = "txId2";

    EXPECT_THROW(this->database_handler.transaction_insert(*transaction, evse_id), DatabaseException);
}

TEST_F(DatabaseHandlerTest, TransactionUpdateSeqNo) {
    constexpr int32_t evse_id = 1;
    constexpr int32_t new_seq_no = 20;

    auto transaction = default_transaction();

    transaction->seq_no = 10;

    this->database_handler.transaction_insert(*transaction, evse_id);
    this->database_handler.transaction_update_seq_no(transaction->transactionId, new_seq_no);

    auto transaction_get = this->database_handler.transaction_get(evse_id);

    EXPECT_NE(transaction_get, nullptr);
    EXPECT_EQ(transaction_get->seq_no, new_seq_no);
}

TEST_F(DatabaseHandlerTest, TransactionUpdateChargingState) {
    constexpr int32_t evse_id = 1;
    constexpr auto new_state = ChargingStateEnum::Charging;

    auto transaction = default_transaction();

    transaction->chargingState = ChargingStateEnum::SuspendedEV;

    this->database_handler.transaction_insert(*transaction, evse_id);
    this->database_handler.transaction_update_charging_state(transaction->transactionId, new_state);

    auto transaction_get = this->database_handler.transaction_get(evse_id);

    EXPECT_NE(transaction_get, nullptr);
    EXPECT_EQ(transaction_get->chargingState, new_state);
}

TEST_F(DatabaseHandlerTest, TransactionUpdateIdTokenSent) {
    constexpr int32_t evse_id = 1;
    constexpr bool new_state = true;

    auto transaction = default_transaction();

    transaction->id_token_sent = false;

    this->database_handler.transaction_insert(*transaction, evse_id);
    this->database_handler.transaction_update_id_token_sent(transaction->transactionId, new_state);

    auto transaction_get = this->database_handler.transaction_get(evse_id);

    EXPECT_NE(transaction_get, nullptr);
    EXPECT_EQ(transaction_get->id_token_sent, new_state);
}

TEST_F(DatabaseHandlerTest, TransactionDelete) {
    constexpr int32_t evse_id = 1;

    auto transaction = default_transaction();

    this->database_handler.transaction_insert(*transaction, evse_id);

    EXPECT_NE(this->database_handler.transaction_get(evse_id), nullptr);

    this->database_handler.transaction_delete(transaction->transactionId);

    EXPECT_EQ(this->database_handler.transaction_get(evse_id), nullptr);
}

TEST_F(DatabaseHandlerTest, TransactionDeleteNotFound) {
    EXPECT_NO_THROW(this->database_handler.transaction_delete("txIdNotFound"));
}