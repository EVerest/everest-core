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
    return {types::powermeter::TransactionRequestStatus::NOT_SUPPORTED,
            "DcSupplySimulator does not support start transaction request."};
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    return {types::powermeter::TransactionRequestStatus::NOT_SUPPORTED,
            {},
            {},
            "DcSupplySimulator does not support stop transaction request."};
}

} // namespace powermeter
} // namespace module
