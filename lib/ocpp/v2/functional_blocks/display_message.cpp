// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/evse_manager.hpp>
#include <ocpp/v2/functional_blocks/display_message.hpp>

#include <ocpp/v2/messages/ClearDisplayMessage.hpp>
#include <ocpp/v2/messages/GetDisplayMessages.hpp>
#include <ocpp/v2/messages/NotifyDisplayMessages.hpp>

namespace ocpp::v2 {

DisplayMessageBlock::DisplayMessageBlock(MessageDispatcherInterface<MessageType>& message_dispatcher,
                                         DeviceModel& device_model, EvseManagerInterface& evse_manager,
                                         GetDisplayMessageCallback get_display_message_callback,
                                         SetDisplayMessageCallback set_display_message_callback,
                                         ClearDisplayMessageCallback clear_display_message_callback) :
    message_dispatcher(message_dispatcher),
    device_model(device_model),
    evse_manager(evse_manager),
    get_display_message_callback(get_display_message_callback),
    set_display_message_callback(set_display_message_callback),
    clear_display_message_callback(clear_display_message_callback) {
}

void DisplayMessageBlock::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    const auto& json_message = message.message;
    switch (message.messageType) {
    case MessageType::GetDisplayMessages:
        this->handle_get_display_message(json_message);
        break;
    case MessageType::SetDisplayMessage:
        this->handle_set_display_message(json_message);
        break;
    case MessageType::ClearDisplayMessage:
        this->handle_clear_display_message(json_message);
        break;
    default:
        throw MessageTypeNotImplementedException(message.messageType);
    }
}

void DisplayMessageBlock::handle_get_display_message(const Call<GetDisplayMessagesRequest> call) {
    // Call 'get display message callback' to get all display messages from the charging station.
    const std::vector<DisplayMessage> display_messages = this->get_display_message_callback(call.msg);

    NotifyDisplayMessagesRequest messages_request;
    messages_request.requestId = call.msg.requestId;
    messages_request.messageInfo = std::vector<MessageInfo>();
    // Convert all display messages from the charging station to the correct format. They will not be included if
    // they do not have the required values. That's why we wait with sending the response until we converted all
    // display messages, because we then know if there are any.
    for (const auto& display_message : display_messages) {
        const std::optional<MessageInfo> message_info = display_message_to_message_info_type(display_message);
        if (message_info.has_value()) {
            messages_request.messageInfo->push_back(message_info.value());
        }
    }

    // Send 'accepted' back to the CSMS if there is at least one message and send all the messages in another
    // request.
    GetDisplayMessagesResponse response;
    if (messages_request.messageInfo.value().empty()) {
        response.status = GetDisplayMessagesStatusEnum::Unknown;
        ocpp::CallResult<GetDisplayMessagesResponse> call_result(response, call.uniqueId);
        this->message_dispatcher.dispatch_call_result(call_result);
        return;
    } else {
        response.status = GetDisplayMessagesStatusEnum::Accepted;
        ocpp::CallResult<GetDisplayMessagesResponse> call_result(response, call.uniqueId);
        this->message_dispatcher.dispatch_call_result(call_result);
    }

    // Send display messages. The response is empty, so we don't have to get that back.
    // Sending multiple messages is not supported for now, because there is no need to split them up (yet).
    ocpp::Call<NotifyDisplayMessagesRequest> request(messages_request);
    this->message_dispatcher.dispatch_call(request);
}

void DisplayMessageBlock::handle_set_display_message(const Call<SetDisplayMessageRequest> call) {
    SetDisplayMessageResponse response;

    // Check if display messages are available, priority and message format are supported and if the given
    // transaction is running, if a transaction id was included in the message.
    bool error = false;
    const std::optional<bool> display_message_available =
        this->device_model.get_optional_value<bool>(ControllerComponentVariables::DisplayMessageCtrlrAvailable);
    const std::string supported_priorities =
        this->device_model.get_value<std::string>(ControllerComponentVariables::DisplayMessageSupportedPriorities);
    const std::string supported_message_formats =
        this->device_model.get_value<std::string>(ControllerComponentVariables::DisplayMessageSupportedFormats);

    const std::vector<std::string> priorities = split_string(supported_priorities, ',', true);
    const std::vector<std::string> formats = split_string(supported_message_formats, ',', true);
    const auto& supported_priority_it = std::find(
        priorities.begin(), priorities.end(), conversions::message_priority_enum_to_string(call.msg.message.priority));
    const auto& supported_format_it = std::find(
        formats.begin(), formats.end(), conversions::message_format_enum_to_string(call.msg.message.message.format));

    // Check if transaction is valid: this is the case if there is no transaction id, or if the transaction id
    // belongs to a running transaction.
    const bool transaction_valid =
        (!call.msg.message.transactionId.has_value() or
         this->evse_manager.get_transaction_evseid(call.msg.message.transactionId.value()) != std::nullopt);

    // Check if display messages are available.
    if (!display_message_available.has_value() or !display_message_available.value()) {
        error = true;
        response.status = DisplayMessageStatusEnum::Rejected;
    }
    // Check if the priority is supported.
    else if (supported_priority_it == priorities.end()) {
        error = true;
        response.status = DisplayMessageStatusEnum::NotSupportedPriority;
    }
    // Check if the message format is supported.
    else if (supported_format_it == formats.end()) {
        error = true;
        response.status = DisplayMessageStatusEnum::NotSupportedMessageFormat;
    }
    // Check if transaction is valid.
    else if (!transaction_valid) {
        error = true;
        response.status = DisplayMessageStatusEnum::UnknownTransaction;
    }
    // Check if message state is supported.
    else if (call.msg.message.state.has_value()) {
        const std::optional<std::string> supported_states = this->device_model.get_optional_value<std::string>(
            ControllerComponentVariables::DisplayMessageSupportedStates);
        if (supported_states.has_value()) {
            const std::vector<std::string> states = split_string(supported_states.value(), ',', true);
            const auto& supported_states_it =
                std::find(states.begin(), states.end(),
                          conversions::message_state_enum_to_string(call.msg.message.state.value()));
            if (supported_states_it == states.end()) {
                error = true;
                response.status = DisplayMessageStatusEnum::NotSupportedState;
            }
        }
    }

    if (error) {
        ocpp::CallResult<SetDisplayMessageResponse> call_result(response, call.uniqueId);
        this->message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    const DisplayMessage message = message_info_to_display_message(call.msg.message);
    response = this->set_display_message_callback({message});
    ocpp::CallResult<SetDisplayMessageResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);
}

void DisplayMessageBlock::handle_clear_display_message(const Call<ClearDisplayMessageRequest> call) {
    ClearDisplayMessageResponse response;
    response = this->clear_display_message_callback(call.msg);
    ocpp::CallResult<ClearDisplayMessageResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);
}

///
/// \brief Convert message content from OCPP spec to DisplayMessageContent.
/// \param message_content  The struct to convert.
/// \return The converted struct.
///
DisplayMessageContent message_content_to_display_message_content(const MessageContent& message_content) {
    DisplayMessageContent result;
    result.message = message_content.content;
    result.message_format = message_content.format;
    result.language = message_content.language;
    return result;
}

///
/// \brief Convert display message to MessageInfo from OCPP.
/// \param display_message  The struct to convert.
/// \return The converted struct.
///
std::optional<MessageInfo> display_message_to_message_info_type(const DisplayMessage& display_message) {
    // Each display message should have an id and p[riority, this is required for OCPP.
    if (!display_message.id.has_value()) {
        EVLOG_error << "Can not convert DisplayMessage to MessageInfo: No id is provided, which is required by OCPP.";
        return std::nullopt;
    }

    if (!display_message.priority.has_value()) {
        EVLOG_error
            << "Can not convert DisplayMessage to MessageInfo: No priority is provided, which is required by OCPP.";
        return std::nullopt;
    }

    MessageInfo info;
    info.message.content = display_message.message.message;
    info.message.format =
        (display_message.message.message_format.has_value() ? display_message.message.message_format.value()
                                                            : MessageFormatEnum::UTF8);
    info.message.language = display_message.message.language;
    info.endDateTime = display_message.timestamp_to;
    info.startDateTime = display_message.timestamp_from;
    info.id = display_message.id.value();
    info.priority = display_message.priority.value();
    info.state = display_message.state;
    info.transactionId = display_message.identifier_id;

    // Note: component is (not yet?) supported for display messages in libocpp.

    return info;
}

///
/// \brief Convert message info from OCPP to DisplayMessage.
/// \param message_info The struct to convert.
/// \return The converted struct.
///
DisplayMessage message_info_to_display_message(const MessageInfo& message_info) {
    DisplayMessage display_message;

    display_message.id = message_info.id;
    display_message.priority = message_info.priority;
    display_message.state = message_info.state;
    display_message.timestamp_from = message_info.startDateTime;
    display_message.timestamp_to = message_info.endDateTime;
    display_message.identifier_id = message_info.transactionId;
    display_message.identifier_type = IdentifierType::TransactionId;
    display_message.message = message_content_to_display_message_content(message_info.message);

    return display_message;
}

} // namespace ocpp::v2
