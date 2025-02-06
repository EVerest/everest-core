// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <memory>
#include <optional>

#include <iso15118/config.hpp>

#include <iso15118/d20/config.hpp>
#include <iso15118/d20/context.hpp>
#include <iso15118/d20/control_event_queue.hpp>
#include <iso15118/d20/states.hpp>
#include <iso15118/detail/fsm/fsm.hpp>

#include <iso15118/io/connection_abstract.hpp>
#include <iso15118/io/poll_manager.hpp>
#include <iso15118/io/sdp_packet.hpp>
#include <iso15118/io/time.hpp>

#include <iso15118/session/feedback.hpp>
#include <iso15118/session/logger.hpp>

namespace iso15118 {

struct SessionState {
    bool connected{false};
    bool new_data{false};
    bool fsm_needs_call{false};
};

class Session {
public:
    Session(std::unique_ptr<io::IConnection>, d20::SessionConfig, const session::feedback::Callbacks&);
    ~Session();

    TimePoint const& poll();
    void push_control_event(const d20::ControlEvent&);

    bool is_finished() const {
        return ctx.session_stopped;
    }

private:
    std::unique_ptr<io::IConnection> connection;
    session::SessionLogger log;

    SessionState state;
    // input buffer
    io::SdpPacket packet;

    // output buffer
    uint8_t response_buffer[1028];

    d20::MessageExchange message_exchange{{response_buffer + io::SdpPacket::V2GTP_HEADER_SIZE,
                                           sizeof(response_buffer) - io::SdpPacket::V2GTP_HEADER_SIZE}};

    // control event buffer
    d20::ControlEventQueue control_event_queue;
    std::optional<d20::ControlEvent> active_control_event{std::nullopt};

    d20::Context ctx;

    fsm::v2::FSM<d20::StateBase> fsm;

    TimePoint next_session_event;

    void handle_connection_event(io::ConnectionEvent event);
};

} // namespace iso15118
