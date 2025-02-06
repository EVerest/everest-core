// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/state/dc_cable_check.hpp>
#include <iso15118/d20/state/dc_pre_charge.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/dc_cable_check.hpp>
#include <iso15118/detail/d20/state/session_stop.hpp>
#include <iso15118/detail/helper.hpp>

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

message_20::DC_CableCheckResponse handle_request(const message_20::DC_CableCheckRequest& req,
                                                 const d20::Session& session, bool cable_check_done) {

    message_20::DC_CableCheckResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_UnknownSession);
    }

    if (not cable_check_done) {
        res.processing = dt::Processing::Ongoing;
    } else {
        res.processing = dt::Processing::Finished;
    }

    return response_with_code(res, dt::ResponseCode::OK);
}

void DC_CableCheck::enter() {
    m_ctx.log.enter_state("DC_CableCheck");
}

Result DC_CableCheck::feed(Event ev) {

    if (ev == Event::CONTROL_MESSAGE) {
        const auto control_data = m_ctx.get_control_event<CableCheckFinished>();
        if (not control_data) {
            // Ignore control message
            return {};
        }

        cable_check_done = *control_data;

        return {};
    }

    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_request();

    if (const auto req = variant->get_if<message_20::DC_CableCheckRequest>()) {
        if (not cable_check_initiated) {
            m_ctx.feedback.signal(session::feedback::Signal::START_CABLE_CHECK);
            cable_check_initiated = true;
        }

        const auto res = handle_request(*req, m_ctx.session, cable_check_done);

        m_ctx.respond(res);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        if (cable_check_done) {
            return m_ctx.create_state<DC_PreCharge>();
        } else {
            return {};
        }
    } else if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, m_ctx.session);

        m_ctx.respond(res);
        m_ctx.session_stopped = true;

        return {};
    } else {
        m_ctx.log("expected DC_CableCheckReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        m_ctx.session_stopped = true;
        return {};
    }
}

} // namespace iso15118::d20::state
