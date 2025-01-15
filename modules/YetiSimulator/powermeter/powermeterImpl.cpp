// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"
#include <string>

namespace module::powermeter {

void powermeterImpl::init() {
}

void powermeterImpl::ready() {
}

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& _) {
    return {.status = types::powermeter::TransactionRequestStatus::OK};
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    return {.status = types::powermeter::TransactionRequestStatus::NOT_SUPPORTED,
            .error = "YetiDriver does not support stop transaction request."};
}

} // namespace module::powermeter