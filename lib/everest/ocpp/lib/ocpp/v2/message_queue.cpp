// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/message_queue.hpp>

#include <everest/logging.hpp>

namespace ocpp {

bool is_transaction_message(const ocpp::v2::MessageType message_type) {
    return (message_type == v2::MessageType::TransactionEvent) ||
           (message_type == v2::MessageType::SecurityEventNotification);
}

bool is_start_transaction_message(const ocpp::v2::MessageType /*message_type*/) {
    return false;
}

bool is_boot_notification_message(const ocpp::v2::MessageType message_type) {
    return message_type == ocpp::v2::MessageType::BootNotification;
}

template <> bool ControlMessage<v2::MessageType>::is_transaction_update_message() const {
    if (this->messageType == v2::MessageType::TransactionEvent) {
        return v2::TransactionEventRequest{this->message.at(CALL_PAYLOAD)}.eventType ==
               v2::TransactionEventEnum::Updated;
    }
    return false;
}

template <>
ControlMessage<v2::MessageType>::ControlMessage(const json& message, const bool stall_until_accepted) :
    message(message.get<json::array_t>()),
    messageType(v2::conversions::string_to_messagetype(message.at(CALL_ACTION))),
    message_attempts(0),
    initial_unique_id(message[MESSAGE_ID]),
    stall_until_accepted(stall_until_accepted) {
}

template <> v2::MessageType MessageQueue<v2::MessageType>::string_to_messagetype(const std::string& s) {
    return v2::conversions::string_to_messagetype(s);
}

template <> std::string MessageQueue<v2::MessageType>::messagetype_to_string(const v2::MessageType m) {
    return v2::conversions::messagetype_to_string(m);
}

} // namespace ocpp
