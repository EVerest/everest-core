// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/message_dispatcher.hpp>

namespace ocpp {
namespace v2 {

void MessageDispatcher::dispatch_call(const json& call, bool triggered) {
    const auto message_type = conversions::string_to_messagetype(call.at(CALL_ACTION));
    const auto message_transmission_priority = get_message_transmission_priority(
        is_boot_notification_message(message_type), triggered,
        (this->registration_status == RegistrationStatusEnum::Accepted), is_transaction_message(message_type),
        this->device_model.get_optional_value<bool>(ControllerComponentVariables::QueueAllMessages).value_or(false));
    switch (message_transmission_priority) {
    case MessageTransmissionPriority::SendImmediately:
        this->message_queue.push_call(call);
        return;
    case MessageTransmissionPriority::SendAfterRegistrationStatusAccepted:
        this->message_queue.push_call(call, true);
        return;
    case MessageTransmissionPriority::Discard:
        return;
    }
    throw std::runtime_error("Missing handling for MessageTransmissionPriority");
}

std::future<ocpp::EnhancedMessage<MessageType>> MessageDispatcher::dispatch_call_async(const json& call,
                                                                                       bool /*triggered*/) {
    const auto message_type = conversions::string_to_messagetype(call.at(CALL_ACTION));
    const auto message_transmission_priority = get_message_transmission_priority(
        is_boot_notification_message(message_type), false,
        (this->registration_status == RegistrationStatusEnum::Accepted), is_transaction_message(message_type),
        this->device_model.get_optional_value<bool>(ControllerComponentVariables::QueueAllMessages).value_or(false));
    switch (message_transmission_priority) {
    case MessageTransmissionPriority::SendImmediately:
        return this->message_queue.push_call_async(call);
    case MessageTransmissionPriority::SendAfterRegistrationStatusAccepted:
    case MessageTransmissionPriority::Discard:
        auto promise = std::promise<EnhancedMessage<MessageType>>();
        auto enhanced_message = EnhancedMessage<MessageType>();
        enhanced_message.offline = true;
        promise.set_value(enhanced_message);
        return promise.get_future();
    }
    throw std::runtime_error("Missing handling for MessageTransmissionPriority");
}

void MessageDispatcher::dispatch_call_result(const json& call_result) {
    this->message_queue.push_call_result(call_result);
}

void MessageDispatcher::dispatch_call_error(const json& call_error) {
    this->message_queue.push_call_error(call_error);
}

} // namespace v2
} // namespace ocpp
