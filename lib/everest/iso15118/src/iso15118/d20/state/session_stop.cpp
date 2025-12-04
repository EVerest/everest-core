// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/state/session_stop.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/session_stop.hpp>
#include <iso15118/detail/helper.hpp>

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

message_20::SessionStopResponse handle_request(const message_20::SessionStopRequest& req, const d20::Session& session) {

    message_20::SessionStopResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_UnknownSession);
    }

    if (req.charging_session == dt::ChargingSession::ServiceRenegotiation &&
        session.service_renegotiation_supported == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_NoServiceRenegotiationSupported);
    }

    // Todo(sl): Check req.charging_session

    return response_with_code(res, dt::ResponseCode::OK);
}

void SessionStop::enter() {
    m_ctx.log.enter_state("SessionStop");
}

Result SessionStop::feed(Event ev) {

    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_request();

    if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, m_ctx.session);

        std::string ev_termination_code;
        std::string ev_termination_explanation;

        if (req->ev_termination_code.has_value()) {
            logf_info("EV termination code: %s", req->ev_termination_code.value().c_str());
            ev_termination_code = req->ev_termination_code.value();
        }
        if (req->ev_termination_explanation.has_value()) {
            logf_info("EV Termination explanation: %s", req->ev_termination_explanation.value().c_str());
            ev_termination_explanation = req->ev_termination_explanation.value();
        }

        if (req->ev_termination_code.has_value() or req->ev_termination_explanation.has_value()) {
            m_ctx.feedback.ev_termination(ev_termination_code, ev_termination_explanation);
        }
        m_ctx.respond(res);

        // Todo(sl): Tell the reason why the charger is stopping. Shutdown, Error, etc.
        if (req->charging_session == message_20::datatypes::ChargingSession::Pause) {
            m_ctx.session_paused = true;
            if (not m_ctx.pause_ctx.has_value()) {
                logf_error("Pause the session but pause_ctx has no value");
                return {};
            }
            m_ctx.pause_ctx->selected_service_parameters = m_ctx.session.get_selected_services();
        } else if (req->charging_session == message_20::datatypes::ChargingSession::Terminate) {
            m_ctx.session_stopped = true;
            m_ctx.pause_ctx.reset();
        }

        return {};
    } else {
        m_ctx.log("expected SessionStop! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        m_ctx.session_stopped = true;
        return {};
    }
}

} // namespace iso15118::d20::state
