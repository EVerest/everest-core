// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/context.hpp>

#include <stdexcept>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d20 {

std::unique_ptr<MessageExchange> create_message_exchange(uint8_t* buf, const size_t len) {
    io::StreamOutputView view = {buf, len};
    return std::make_unique<MessageExchange>(std::move(view));
}

MessageExchange::MessageExchange(io::StreamOutputView output_) : response(std::move(output_)) {
}

void MessageExchange::set_request(std::unique_ptr<message_20::Variant> new_request) {
    if (request) {
        // FIXME (aw): we might want to have a stack here?
        throw std::runtime_error("Previous V2G message has not been handled yet");
    }

    request = std::move(new_request);
}

std::unique_ptr<message_20::Variant> MessageExchange::pull_request() {
    if (not request) {
        throw std::runtime_error("Tried to access V2G message, but there is none");
    }

    return std::move(request);
}

std::tuple<bool, size_t, io::v2gtp::PayloadType, message_20::Type> MessageExchange::check_and_clear_response() {
    auto retval = std::make_tuple(response_available, response_size, payload_type, response_type);

    response_available = false;
    response_size = 0;
    response_type = message_20::Type::None;

    return retval;
}

message_20::Type MessageExchange::peek_request_type() const {
    if (not request) {
        logf_warning("Tried to access V2G message, but there is none");
        return message_20::Type::None;
    }
    return request->get_type();
}

Context::Context(session::feedback::Callbacks feedback_callbacks, session::SessionLogger& logger,
                 SessionConfig session_config_, std::optional<PauseContext>& pause_ctx_,
                 const std::optional<ControlEvent>& current_control_event_, MessageExchange& message_exchange_) :
    feedback(std::move(feedback_callbacks)),
    log(logger),
    session_config(std::move(session_config_)),
    pause_ctx(pause_ctx_),
    current_control_event{current_control_event_},
    message_exchange(message_exchange_) {
}

std::unique_ptr<message_20::Variant> Context::pull_request() {
    return message_exchange.pull_request();
}

message_20::Type Context::peek_request_type() const {
    return message_exchange.peek_request_type();
}

} // namespace iso15118::d20
