// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/message_queue.hpp>

#include <everest/logging.hpp>

namespace ocpp {

template <> ControlMessage<v16::MessageType>::ControlMessage(json message) {
    this->message = message.get<json::array_t>();
    this->messageType = v16::conversions::string_to_messagetype(message.at(CALL_ACTION));
    this->message_attempts = 0;
}

template <> ControlMessage<v201::MessageType>::ControlMessage(json message) {
    this->message = message.get<json::array_t>();
    this->messageType = v201::conversions::string_to_messagetype(message.at(CALL_ACTION));
    this->message_attempts = 0;
}

template <> bool MessageQueue<v16::MessageType>::isTransactionMessage(ControlMessage<v16::MessageType>* message) {
    if (message == nullptr) {
        return false;
    }
    if (message->messageType == v16::MessageType::StartTransaction ||
        message->messageType == v16::MessageType::StopTransaction ||
        message->messageType == v16::MessageType::MeterValues) {
        return true;
    }
    return false;
}

template <> bool MessageQueue<v201::MessageType>::isTransactionMessage(ControlMessage<v201::MessageType>* message) {
    // FIXME
    // if (message == nullptr) {
    //     return false;
    // }
    // if (message->messageType == v16::MessageType::StartTransaction ||
    //     message->messageType == v16::MessageType::StopTransaction ||
    //     message->messageType == v16::MessageType::MeterValues) {
    //     return true;
    // }
    return false;
}

template <> v16::MessageType MessageQueue<v16::MessageType>::string_to_messagetype(const std::string& s) {
    return v16::conversions::string_to_messagetype(s);
}

template <> v201::MessageType MessageQueue<v201::MessageType>::string_to_messagetype(const std::string& s) {
    return v201::conversions::string_to_messagetype(s);
}

} // namespace ocpp
