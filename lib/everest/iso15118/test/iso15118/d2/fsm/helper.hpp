// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <array>

#include <iso15118/d2/config.hpp>
#include <iso15118/d2/context.hpp>
#include <iso15118/d2/states.hpp>
#include <iso15118/detail/cb_exi.hpp>
#include <iso15118/fsm/fsm.hpp>
#include <iso15118/io/logging.hpp>
#include <iso15118/io/sdp.hpp>
#include <iso15118/io/stream_view.hpp>
#include <iso15118/message/d2/variant.hpp>

using namespace iso15118;

class FsmStateHelper {
public:
    FsmStateHelper(const d2::SessionConfig& config, session::feedback::Callbacks& feedback_callbacks) :
        ctx(feedback_callbacks, config, msg_exch){

        };

    d2::Context& get_context();

    template <typename RequestType> void handle_request(const RequestType& request) {
        msg_exch.set_request(std::make_unique<d2::msg::Variant>(request));
    }

private:
    std::array<uint8_t, 1024> output_buffer{};
    io::StreamOutputView output_stream_view{output_buffer.data(), output_buffer.size()};

    d2::MessageExchange msg_exch{output_stream_view};

    d2::Context ctx;
};
