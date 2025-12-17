// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/message_queue.hpp>

#include <everest/logging.hpp>

namespace ocpp {

template <>
ControlMessage<v16::MessageType>::ControlMessage(const json& message, const bool stall_until_accepted) :
    message(message.get<json::array_t>()),
    messageType(v16::conversions::string_to_messagetype(message.at(CALL_ACTION))),
    message_attempts(0),
    initial_unique_id(message[MESSAGE_ID]),
    stall_until_accepted(stall_until_accepted) {
}

bool is_transaction_message(const ocpp::v16::MessageType message_type) {
    return (message_type == v16::MessageType::StartTransaction) ||
           (message_type == v16::MessageType::StopTransaction) || (message_type == v16::MessageType::MeterValues) ||
           (message_type == v16::MessageType::SecurityEventNotification);
}

bool is_start_transaction_message(const ocpp::v16::MessageType message_type) {
    return message_type == v16::MessageType::StartTransaction;
}

bool is_boot_notification_message(const ocpp::v16::MessageType message_type) {
    return message_type == ocpp::v16::MessageType::BootNotification;
}

template <> bool ControlMessage<v16::MessageType>::is_transaction_update_message() const {
    return (this->messageType == v16::MessageType::MeterValues);
}

template <> v16::MessageType MessageQueue<v16::MessageType>::string_to_messagetype(const std::string& s) {
    return v16::conversions::string_to_messagetype(s);
}

template <> std::string MessageQueue<v16::MessageType>::messagetype_to_string(v16::MessageType m) {
    return v16::conversions::messagetype_to_string(m);
}

} // namespace ocpp
