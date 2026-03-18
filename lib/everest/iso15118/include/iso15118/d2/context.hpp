// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#pragma once

#include <any>
#include <memory>
#include <optional>
#include <tuple>

// #include <iso15118/d20/timeout.hpp>
#include <iso15118/message/d2/variant.hpp>
#include <iso15118/session/feedback.hpp>
// #include <iso15118/session/logger.hpp>

#include "config.hpp"
// #include "control_event.hpp"
// #include "ev_information.hpp"
// #include "ev_session_info.hpp"
#include "session.hpp"

namespace iso15118::d2 {

// forward declare
class ControlEventQueue;

class MessageExchange {
public:
    MessageExchange(io::StreamOutputView);

    void set_request(std::unique_ptr<msg::Variant> new_request);
    std::unique_ptr<msg::Variant> pull_request();
    msg::Type peek_request_type() const;

    template <typename MessageType> void set_response(const MessageType& msg) {
        response_size = msg::serialize(msg, response);
        response_available = true;
        payload_type = io::v2gtp::PayloadType::SAP;
        response_type = msg::TypeTrait<MessageType>::type;
        response_message = msg;
    }

    template <typename Msg> std::optional<Msg> get_response() {
        static_assert(msg::TypeTrait<Msg>::type != msg::Type::None, "Unhandled type!");
        if (msg::TypeTrait<Msg>::type != response_type) {
            return std::nullopt;
        }
        try {
            return std::any_cast<Msg>(response_message);
        } catch (const std::bad_any_cast& ex) {
            return std::nullopt;
        }
    }

    std::tuple<bool, size_t, io::v2gtp::PayloadType, msg::Type> check_and_clear_response();

private:
    // input
    std::unique_ptr<msg::Variant> request{nullptr};

    // output
    const io::StreamOutputView response;
    size_t response_size{0};
    bool response_available{false};
    io::v2gtp::PayloadType payload_type;
    msg::Type response_type;
    std::any response_message;
};

std::unique_ptr<MessageExchange> create_message_exchange(uint8_t* buf, const size_t len);

struct StateBase;
using BasePointerType = std::unique_ptr<StateBase>;

class Context {
public:
    Context(session::feedback::Callbacks, SessionConfig, MessageExchange&);

    template <typename StateType, typename... Args> BasePointerType create_state(Args&&... args) {
        return std::make_unique<StateType>(*this, std::forward<Args>(args)...);
    }

    std::unique_ptr<msg::Variant> pull_request();
    msg::Type peek_request_type() const;

    template <typename MessageType> void respond(const MessageType& msg) {
        message_exchange.set_response(msg);
    }

    template <typename Msg> std::optional<Msg> get_response() {
        return message_exchange.get_response<Msg>();
    }
    bool session_stopped{false};
    bool session_paused{false};

    Session session;

    SessionConfig session_config;

    const session::Feedback feedback;

private:
    //     const auto& get_control_event() {
    //         return current_control_event;
    //     }

    //     template <typename T> T const* get_control_event() {
    //         if (not current_control_event.has_value()) {
    //             return nullptr;
    //         }

    //         if (not std::holds_alternative<T>(*current_control_event)) {
    //             return nullptr;
    //         }

    //         return &std::get<T>(*current_control_event);
    //     }

    //     session::SessionLogger& log;

    //     const std::optional<ControlEvent>& current_control_event;
    MessageExchange& message_exchange;

    //     Timeouts& timeouts;
    //     std::optional<TimeoutType> current_timeout{std::nullopt};
};

} // namespace iso15118::d2
