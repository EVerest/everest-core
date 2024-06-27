// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"

namespace module {
namespace powermeter {

void powermeterImpl::init() {
}

void powermeterImpl::ready() {
}

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& value) {
    // your code for cmd start_transaction goes here
    return {types::powermeter::TransactionRequestStatus::NOT_SUPPORTED,
            "MicroMegaWattBSP powermeter does not support the start_transaction command"};
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    return {types::powermeter::TransactionRequestStatus::NOT_SUPPORTED,
            {},
            {},
            "MicroMegaWattBSP powermeter does not support the stop_transaction command"};
}

} // namespace powermeter
} // namespace module
