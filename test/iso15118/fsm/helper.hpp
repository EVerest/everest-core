// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>

#include <iso15118/d20/config.hpp>
#include <iso15118/d20/control_event.hpp>
#include <iso15118/d20/fsm.hpp>
#include <iso15118/detail/cb_exi.hpp>
#include <iso15118/io/sdp.hpp>
#include <iso15118/io/stream_view.hpp>
#include <iso15118/message/variant.hpp>
#include <iso15118/session/feedback.hpp>

using namespace iso15118;

class FsmStateHelper {
public:
    FsmStateHelper(const d20::SessionConfig& config) :
        log(this), ctx(msg_exch, active_control_event, callbacks, session_stopped, log, config){};

    d20::Context& get_context();

    template <typename RequestType, typename StateType>
    fsm::states::HandleEventResult handle_request(StateType& state, io::v2gtp::PayloadType payload_type,
                                                  const RequestType& request) {
        const auto encoded_size = message_20::serialize_helper(request, output_stream_view);

        msg_exch.set_request(std::make_unique<message_20::Variant>(payload_type, output_stream_view));

        return state.handle_event(state_allocator, d20::FsmEvent::V2GTP_MESSAGE);
    }

    template <typename StateType> bool next_simple_state_is() {
        auto new_state_ptr = state_allocator_impl.pull_simple_state<d20::FsmSimpleState>();

        return (nullptr != dynamic_cast<StateType*>(new_state_ptr));
    }

    template <typename StateType>
    fsm::states::HandleEventResult handle_control_event(StateType& state, const d20::ControlEvent& event) {
        active_control_event = event;

        return state.handle_event(state_allocator, d20::FsmEvent::CONTROL_MESSAGE);
    }

private:
    uint8_t output_buffer[1024];
    io::StreamOutputView output_stream_view{output_buffer, sizeof(output_buffer)};

    d20::MessageExchange msg_exch{output_stream_view};
    std::optional<d20::ControlEvent> active_control_event;

    session::feedback::Callbacks callbacks;
    session::SessionLogger log;
    bool session_stopped{false};

    d20::Context ctx;

    fsm::_impl::DynamicStateAllocator state_allocator_impl;
    fsm::states::StateAllocator<void> state_allocator{state_allocator_impl};
};
