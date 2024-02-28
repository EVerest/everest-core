// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "transactionData.hpp"
#include "everest/logging.hpp"
#include <mutex>

// publishes to everest/ocpp/ocpp_transaction/var

namespace module::ocpp_transaction {

void TransactionData::transaction_start(const impl_p& impl, int32_t connector, const std::string& session_id) {
    const std::lock_guard<std::mutex> lock(mux);
    if (auto it = transaction_map.find(connector); it != transaction_map.end()) {
        EVLOG_error << "New Transaction with existing, connector: " << connector
                    << " session: " << it->second.session_id
                    << " transaction: " << it->second.transaction_id.value_or(-1);
    }
    types::ocpp::TransactionEvent tevent = {
        types::ocpp::Event::Start,
        connector,
        session_id,
        std::nullopt,
    };
    transaction_map[connector] = {session_id, std::nullopt};
    impl->publish_transaction_event(tevent);
}

void TransactionData::transaction_update(const impl_p& impl, int32_t connector, int32_t transaction_id) {
    const std::lock_guard<std::mutex> lock(mux);
    if (auto it = transaction_map.find(connector); it != transaction_map.end()) {
        it->second.transaction_id = transaction_id;
        types::ocpp::TransactionEvent tevent = {
            types::ocpp::Event::Update,
            connector,
            it->second.session_id,
            transaction_id,
        };
        impl->publish_transaction_event(tevent);
    } else {
        EVLOG_error << "Transaction ID with no start, connector: " << connector << " transaction: " << transaction_id;
    }
}

void TransactionData::transaction_end(const impl_p& impl, int32_t connector, const std::string& session_id) {
    const std::lock_guard<std::mutex> lock(mux);
    if (auto it = transaction_map.find(connector); it != transaction_map.end()) {
        if (it->second.session_id != session_id) {
            EVLOG_error << "Transaction end with wrong session, connector: " << connector
                        << " expected: " << it->second.session_id << " actual: " << session_id;
        }
        types::ocpp::TransactionEvent tevent = {
            types::ocpp::Event::End,
            connector,
            it->second.session_id,
            it->second.transaction_id,
        };
        transaction_map.erase(it);
        impl->publish_transaction_event(tevent);
    } else {
        EVLOG_error << "Transaction end with no start, connector: " << connector << " session: " << session_id;
    }
}

} // namespace module::ocpp_transaction
