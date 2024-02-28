// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef TRANSACTION_DATA_HPP
#define TRANSACTION_DATA_HPP

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include <generated/interfaces/transaction/Implementation.hpp>

namespace module::ocpp_transaction {

class TransactionData {
private:
    struct TransactionInfo {
        std::string session_id;
        std::optional<int32_t> transaction_id;
    };
    std::map<int32_t, TransactionInfo> transaction_map;
    std::mutex mux;

public:
    TransactionData() = default;

    using impl_p = std::unique_ptr<transactionImplBase>;

    void transaction_start(const impl_p& impl, std::int32_t connector, const std::string& session_id);
    void transaction_update(const impl_p& impl, std::int32_t connector, std::int32_t transaction_id);
    void transaction_end(const impl_p& impl, std::int32_t connector, const std::string& session_id);
};

} // namespace module::ocpp_transaction

#endif // TRANSACTION_DATA_HPP
