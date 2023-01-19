// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/transaction.hpp>

namespace ocpp {

namespace v201 {

Transaction EnhancedTransaction::get_transaction() {
    Transaction transaction = {this->transactionId,     boost::none,         this->chargingState,
                               this->timeSpentCharging, this->stoppedReason, this->remoteStartId};
    return transaction;
}

int32_t EnhancedTransaction::get_seq_no() {
    this->seq_no += 1;
    return this->seq_no - 1;
}

} // namespace v201

} // namespace ocpp