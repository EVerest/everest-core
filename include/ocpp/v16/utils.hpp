// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/call_types.hpp>
#include <ocpp/v16/ocpp_types.hpp>
#include <ocpp/v16/types.hpp>

namespace ocpp {
namespace v16 {
namespace utils {
bool is_transaction_message_type(const MessageType& message_type) {
    return message_type == MessageType::StartTransaction or message_type == MessageType::StopTransaction or
           message_type == MessageType::MeterValues or message_type == MessageType::SecurityEventNotification;
}

size_t get_message_size(const ocpp::Call<StopTransactionRequest>& call) {
    return json(call).at(CALL_PAYLOAD).dump().length();
}

/// \brief Drops every second entry from transactionData as long as the message size of the \p call is greater than the
/// \p max_message_size
void drop_transaction_data(size_t max_message_size, ocpp::Call<StopTransactionRequest>& call) {
    auto& transaction_data = call.msg.transactionData.value();
    while (get_message_size(call) > max_message_size && transaction_data.size() > 2) {
        for (size_t i = 1; i < transaction_data.size() - 1; i = i + 2) {
            transaction_data.erase(transaction_data.begin() + i);
        }
    }
}
} // namespace utils
} // namespace v16
} // namespace ocpp
