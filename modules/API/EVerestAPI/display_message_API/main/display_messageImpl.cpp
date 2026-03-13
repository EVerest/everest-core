// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "display_messageImpl.hpp"

#include <everest_api_types/display_message/API.hpp>
#include <everest_api_types/display_message/codec.hpp>
#include <everest_api_types/display_message/json_codec.hpp>
#include <everest_api_types/display_message/wrapper.hpp>
#include <everest_api_types/system/wrapper.hpp>
#include <everest_api_types/utilities/AsyncApiRequestReply.hpp>

#include <generated/types/display_message.hpp>

namespace API_types_ext = ev_API::V1_0::types::display_message;

namespace module {
namespace main {

void display_messageImpl::init() {
    timeout_s = mod->config.cfg_request_reply_to_s;
}

void display_messageImpl::ready() {
}

template <class T, class ReqT>
auto display_messageImpl::generic_request_reply(T const& default_value, ReqT const& request, std::string const& topic) {
    using namespace API_types_ext;
    using ExtT = decltype(to_external_api(std::declval<T>()));
    auto result = ev_API::request_reply_handler<ExtT>(mod->mqtt, mod->get_topics(), request, topic, timeout_s);
    if (!result) {
        return default_value;
    }
    return result.value();
}

types::display_message::SetDisplayMessageResponse
display_messageImpl::handle_set_display_message(std::vector<types::display_message::DisplayMessage>& request) {
    static const types::display_message::SetDisplayMessageResponse default_response{
        types::display_message::DisplayMessageStatusEnum::UnknownTransaction, {}};
    std::vector<API_types_ext::DisplayMessage> messages;
    for (const auto& message : request) {
        messages.push_back(API_types_ext::to_external_api(message));
    }
    return generic_request_reply(default_response, messages, "set_display_message");
}

types::display_message::GetDisplayMessageResponse
display_messageImpl::handle_get_display_messages(types::display_message::GetDisplayMessageRequest& request) {
    types::display_message::GetDisplayMessageResponse default_response;
    return generic_request_reply(default_response, API_types_ext::to_external_api(request), "get_display_message");
}

types::display_message::ClearDisplayMessageResponse
display_messageImpl::handle_clear_display_message(types::display_message::ClearDisplayMessageRequest& request) {
    types::display_message::ClearDisplayMessageResponse default_response{
        types::display_message::ClearMessageResponseEnum::Unknown, {}};
    return generic_request_reply(default_response, API_types_ext::to_external_api(request), "clear_display_message");
}

} // namespace main
} // namespace module
