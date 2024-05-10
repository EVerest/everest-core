// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ocpp/v16/types.hpp>

namespace ocpp {
namespace v16 {
namespace utils {
bool is_transaction_message_type(const MessageType& message_type) {
    return message_type == MessageType::StartTransaction or message_type == MessageType::StopTransaction or
           message_type == MessageType::MeterValues or message_type == MessageType::SecurityEventNotification;
}
} // namespace utils
} // namespace v16
} // namespace ocpp
