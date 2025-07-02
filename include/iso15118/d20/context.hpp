// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <any>
#include <memory>
#include <optional>
#include <string>
#include <tuple>

#include <iso15118/d20/timeout.hpp>
#include <iso15118/message/payload_type.hpp>
#include <iso15118/message/variant.hpp>
#include <iso15118/session/feedback.hpp>
#include <iso15118/session/logger.hpp>

#include "config.hpp"
#include "control_event.hpp"
#include "ev_information.hpp"
#include "ev_session_info.hpp"
#include "session.hpp"

namespace iso15118::d20 {

// forward declare
class ControlEventQueue;

class MessageExchange {
public:
    MessageExchange(io::StreamOutputView);

    void set_request(std::unique_ptr<message_20::Variant> new_request);
    std::unique_ptr<message_20::Variant> pull_request();
    message_20::Type peek_request_type() const;

    template <typename MessageType> void set_response(const MessageType& msg) {
        response_size = message_20::serialize(msg, response);
        response_available = true;
        payload_type = message_20::PayloadTypeTrait<MessageType>::type;
        response_type = message_20::TypeTrait<MessageType>::type;
        response_message = msg;
    }

    template <typename Msg> std::optional<Msg> get_response() {
        static_assert(message_20::TypeTrait<Msg>::type != message_20::Type::None, "Unhandled type!");
        if (message_20::TypeTrait<Msg>::type != response_type) {
            return std::nullopt;
        }
        try {
            return std::any_cast<Msg>(response_message);
        } catch (const std::bad_any_cast& ex) {
            return std::nullopt;
        }
    }

    std::tuple<bool, size_t, io::v2gtp::PayloadType, message_20::Type> check_and_clear_response();

private:
    // input
    std::unique_ptr<message_20::Variant> request{nullptr};

    // output
    const io::StreamOutputView response;
    size_t response_size{0};
    bool response_available{false};
    io::v2gtp::PayloadType payload_type;
    message_20::Type response_type;
    std::any response_message;
};

std::unique_ptr<MessageExchange> create_message_exchange(uint8_t* buf, const size_t len);

struct StateBase;
using BasePointerType = std::unique_ptr<StateBase>;

class Context {
public:
    // FIXME (aw): bundle arguments
    Context(session::feedback::Callbacks, session::SessionLogger&, d20::SessionConfig, std::optional<PauseContext>&,
            const std::optional<ControlEvent>&, MessageExchange&, Timeouts&);

    template <typename StateType, typename... Args> BasePointerType create_state(Args&&... args) {
        return std::make_unique<StateType>(*this, std::forward<Args>(args)...);
    }

    std::unique_ptr<message_20::Variant> pull_request();
    message_20::Type peek_request_type() const;

    template <typename MessageType> void respond(const MessageType& msg) {
        message_exchange.set_response(msg);
    }

    template <typename Msg> std::optional<Msg> get_response() {
        return message_exchange.get_response<Msg>();
    }

    const auto& get_control_event() {
        return current_control_event;
    }

    template <typename T> T const* get_control_event() {
        if (not current_control_event.has_value()) {
            return nullptr;
        }

        if (not std::holds_alternative<T>(*current_control_event)) {
            return nullptr;
        }

        return &std::get<T>(*current_control_event);
    }

    void set_new_vehicle_cert_hash(std::optional<io::sha512_hash_t> hash) {
        vehicle_cert_hash = hash;
    }

    auto get_new_vehicle_cert_hash() const {
        return vehicle_cert_hash;
    }

    void start_timeout(d20::TimeoutType type, uint32_t time_ms) {
        timeouts.start_timeout(type, time_ms);
    }

    void stop_timeout(d20::TimeoutType type) {
        timeouts.stop_timeout(type);
    }

    d20::TimeoutType const* get_active_timeout() {
        if (not current_timeout.has_value()) {
            return nullptr;
        }
        return &current_timeout.value();
    }

    void set_active_timeout(TimeoutType timeout) {
        current_timeout = timeout;
    }

    const session::Feedback feedback;

    session::SessionLogger& log;

    Session session;

    SessionConfig session_config;

    // Contains the EV received data
    EVSessionInfo session_ev_info;
    EVInformation ev_info;

    std::optional<d20::PauseContext>& pause_ctx;

    bool session_stopped{false};
    bool session_paused{false};

private:
    const std::optional<ControlEvent>& current_control_event;
    MessageExchange& message_exchange;

    std::optional<io::sha512_hash_t> vehicle_cert_hash{std::nullopt};

    Timeouts& timeouts;

    std::optional<TimeoutType> current_timeout{std::nullopt};
};

} // namespace iso15118::d20
