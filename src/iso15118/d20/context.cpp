// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/context.hpp>

#include <stdexcept>

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

std::unique_ptr<message_20::Variant> MessageExchange::get_request() {
    if (not request) {
        throw std::runtime_error("Tried to access V2G message, but there is none");
    }

    return std::move(request);
}

std::tuple<bool, size_t, io::v2gtp::PayloadType> MessageExchange::check_and_clear_response() {
    auto retval = std::make_tuple(response_available, response_size, payload_type);

    response_available = false;
    response_size = 0;

    return retval;
}

Context::Context(MessageExchange& message_exchange_, const std::optional<ControlEvent>& current_control_event_,
                 session::feedback::Callbacks feedback_callbacks, bool& stopping_, session::SessionLogger& logger,
                 const d20::SessionConfig& config_) :
    current_control_event{current_control_event_},
    feedback(std::move(feedback_callbacks)),
    log(logger),
    message_exchange(message_exchange_),
    session_stopped(stopping_),
    config(config_) {
}

std::unique_ptr<message_20::Variant> Context::get_request() {
    return message_exchange.get_request();
}

} // namespace iso15118::d20
