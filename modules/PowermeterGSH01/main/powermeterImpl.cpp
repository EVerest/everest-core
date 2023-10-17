// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"

namespace module {
namespace main {

void powermeterImpl::init() {
}

void powermeterImpl::ready() {
}

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& value) {
    // your code for cmd start_transaction goes here
    return {};
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    // your code for cmd stop_transaction goes here
    return {};
}

} // namespace main
} // namespace module
