// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/message_queue.hpp>

#include <everest/logging.hpp>

namespace ocpp {

template <> ControlMessage<v16::MessageType>::ControlMessage(const json& message) {
    this->message = message.get<json::array_t>();
    this->messageType = v16::conversions::string_to_messagetype(message.at(CALL_ACTION));
    this->message_attempts = 0;
    this->initial_unique_id = this->message[MESSAGE_ID];
}

template <> bool ControlMessage<v16::MessageType>::isTransactionMessage() const {
    if (this->messageType == v16::MessageType::StartTransaction ||
        this->messageType == v16::MessageType::StopTransaction || this->messageType == v16::MessageType::MeterValues ||
        this->messageType == v16::MessageType::SecurityEventNotification) {
        return true;
    }
    return false;
}

template <> bool ControlMessage<v16::MessageType>::isTransactionUpdateMessage() const {
    return (this->messageType == v16::MessageType::MeterValues);
}

template <> bool ControlMessage<v16::MessageType>::isBootNotificationMessage() const {
    return this->messageType == v16::MessageType::BootNotification;
}

template <> v16::MessageType MessageQueue<v16::MessageType>::string_to_messagetype(const std::string& s) {
    return v16::conversions::string_to_messagetype(s);
}

template <> std::string MessageQueue<v16::MessageType>::messagetype_to_string(v16::MessageType m) {
    return v16::conversions::messagetype_to_string(m);
}

} // namespace ocpp
