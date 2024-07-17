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
        EVLOG_error << "New display message: " << message.message.content;
    }

    response.status = types::display_message::DisplayMessageStatusEnum::Accepted;
    // TODO
    return response;
}

types::display_message::GetDisplayMessageResponse
display_messageImpl::handle_get_display_messages(types::display_message::GetDisplayMessageRequest& request) {
    // your code for cmd get_display_messages goes here
    return {};
}

types::display_message::ClearDisplayMessageResponse
display_messageImpl::handle_clear_display_message(types::display_message::ClearDisplayMessageRequest& request) {
    // your code for cmd clear_display_message goes here
    return {};
}

} // namespace display_message
} // namespace module
