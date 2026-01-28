// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/state/dc_welding_detection.hpp>
#include <iso15118/d20/state/session_stop.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/dc_welding_detection.hpp>
#include <iso15118/detail/d20/state/session_stop.hpp>
#include <iso15118/detail/helper.hpp>

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

message_20::DC_WeldingDetectionResponse handle_request(const message_20::DC_WeldingDetectionRequest& req,
                                                       const d20::Session& session, const float present_voltage) {
    message_20::DC_WeldingDetectionResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_UnknownSession);
    }

    res.present_voltage = dt::from_float(present_voltage);

    return response_with_code(res, dt::ResponseCode::OK);
}

void DC_WeldingDetection::enter() {
    m_ctx.log.enter_state("DC_WeldingDetection");
}

Result DC_WeldingDetection::feed(Event ev) {

    if (ev == Event::CONTROL_MESSAGE) {
        const auto control_data = m_ctx.get_control_event<PresentVoltageCurrent>();
        if (not control_data) {
            // Ignore control message
            return {};
        }

        present_voltage = control_data->voltage;

        return {};
    }

    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_request();

    if (const auto req = variant->get_if<message_20::DC_WeldingDetectionRequest>()) {
        const auto res = handle_request(*req, m_ctx.session, present_voltage);

        m_ctx.respond(res);
        m_ctx.feedback.response_code(res.response_code);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        if (req->processing == dt::Processing::Ongoing) {
            return {};
        }

        return m_ctx.create_state<SessionStop>();

    } else if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, m_ctx.session);

        if (req->ev_termination_code.has_value()) {
            logf_info("EV termination code: %s", req->ev_termination_code.value().c_str());
        }
        if (req->ev_termination_explanation.has_value()) {
            logf_info("EV Termination explanation: %s", req->ev_termination_explanation.value().c_str());
        }

        m_ctx.respond(res);
        m_ctx.feedback.response_code(res.response_code);

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
        m_ctx.log("expected DC_WeldingDetection! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        m_ctx.feedback.response_code(dt::ResponseCode::FAILED_SequenceError);
        m_ctx.session_stopped = true;
        return {};
    }
}

} // namespace iso15118::d20::state
