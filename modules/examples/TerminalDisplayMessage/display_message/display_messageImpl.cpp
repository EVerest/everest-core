// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "display_messageImpl.hpp"

namespace module {
namespace display_message {

void display_messageImpl::init() {
}

void display_messageImpl::ready() {
}

types::display_message::SetDisplayMessageResponse
display_messageImpl::handle_set_display_message(std::vector<types::display_message::DisplayMessage>& request) {
    types::display_message::SetDisplayMessageResponse response;
    if (request.empty()) {
        response.status = types::display_message::DisplayMessageStatusEnum::Rejected;
        response.status_info = "No request sent";
        return response;
    }

    for (const types::display_message::DisplayMessage& message : request) {
        EVLOG_info << "New display message"
                   << (message.identifier_id.has_value() ? " for identifier id " + message.identifier_id.value() : "")
                   << ": " << message.message.content;
    }

    response.status = types::display_message::DisplayMessageStatusEnum::Accepted;
    return response;
}

types::display_message::GetDisplayMessageResponse
display_messageImpl::handle_get_display_messages(types::display_message::GetDisplayMessageRequest& request) {
    EVLOG_info << "Get display messages request received"
               << (request.priority.has_value() ? " for display messages with priority " +
                                                      std::to_string(static_cast<int>(request.priority.value()))
                                                : "")
               << (request.state.has_value()
                       ? " for display messages with state " + std::to_string(static_cast<int>(request.state.value()))
                       : "");
    if (request.id.has_value()) {
        std::string ids;
        for (const int32_t& id : request.id.value()) {
            ids += std::to_string(id);
            ids += " ";
        }
        EVLOG_info << "Get display messages for specific id's: " << ids;
    }

    types::display_message::GetDisplayMessageResponse response;
    response.messages = std::vector<types::display_message::DisplayMessage>();
    types::display_message::DisplayMessage test_message;
    test_message.message.content = "This is a test message";
    test_message.message.format = types::text_message::MessageFormat::UTF8;
    test_message.message.language = "en";
    response.messages->push_back(test_message);
    types::display_message::DisplayMessage test_message_url;
    test_message_url.message.content = "https://pionix.de";
    test_message_url.message.format = types::text_message::MessageFormat::URI;
    response.messages->push_back(test_message_url);

    return response;
}

types::display_message::ClearDisplayMessageResponse
display_messageImpl::handle_clear_display_message(types::display_message::ClearDisplayMessageRequest& request) {
    EVLOG_info << "Clear display message request received for id: " << request.id;

    types::display_message::ClearDisplayMessageResponse response;
    response.status = types::display_message::ClearMessageResponseEnum::Accepted;
    response.status_info = "Yes it is done!";
    return response;
}

} // namespace display_message
} // namespace module
