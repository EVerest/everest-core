// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/database_handler.hpp>
#include <ocpp/v2/transaction.hpp>

using QueryExecutionException = ocpp::common::QueryExecutionException;

namespace ocpp {

namespace v2 {

Transaction EnhancedTransaction::get_transaction() {
    Transaction transaction = {this->transactionId,     std::nullopt,        this->chargingState,
                               this->timeSpentCharging, this->stoppedReason, this->remoteStartId};
    return transaction;
}

int32_t EnhancedTransaction::get_seq_no() {
    this->seq_no += 1;
    if (this->database_enabled) {
        try {
            this->database_handler.transaction_update_seq_no(this->transactionId, this->seq_no);
        } catch (const QueryExecutionException& e) {
            EVLOG_error << "Can't update transaction: " << e.what();
        }
    }
    return this->seq_no - 1;
}

void EnhancedTransaction::update_charging_state(const ChargingStateEnum charging_state) {
    this->chargingState = charging_state;
    if (this->database_enabled) {
        try {
            this->database_handler.transaction_update_charging_state(this->transactionId, charging_state);
        } catch (const QueryExecutionException& e) {
            EVLOG_error << "Can't update transaction: " << e.what();
        }
    }
}

void EnhancedTransaction::set_id_token_sent() {
    this->id_token_sent = true;
    if (this->database_enabled) {
        try {
            this->database_handler.transaction_update_id_token_sent(this->transactionId, this->id_token_sent);
        } catch (const QueryExecutionException& e) {
            EVLOG_error << "Can't update transaction: " << e.what();
        }
    }
}

} // namespace v2

} // namespace ocpp
