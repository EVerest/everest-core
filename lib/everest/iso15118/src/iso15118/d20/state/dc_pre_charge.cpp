// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/state/dc_pre_charge.hpp>
#include <iso15118/d20/state/power_delivery.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/dc_pre_charge.hpp>
#include <iso15118/detail/d20/state/session_stop.hpp>
#include <iso15118/detail/helper.hpp>

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

message_20::DC_PreChargeResponse handle_request(const message_20::DC_PreChargeRequest& req, const d20::Session& session,
                                                const float present_voltage) {

    message_20::DC_PreChargeResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_UnknownSession);
    }

    res.present_voltage = dt::from_float(present_voltage);

    return response_with_code(res, dt::ResponseCode::OK);
}

void DC_PreCharge::enter() {
    m_ctx.log.enter_state("DC_PreCharge");
}

Result DC_PreCharge::feed(Event ev) {

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

    if (const auto req = variant->get_if<message_20::DC_PreChargeRequest>()) {
        if (not pre_charge_initiated) {
            m_ctx.feedback.signal(session::feedback::Signal::PRE_CHARGE_STARTED);
            pre_charge_initiated = true;
        }
        const auto res = handle_request(*req, m_ctx.session, present_voltage);

        m_ctx.feedback.dc_pre_charge_target_voltage(message_20::datatypes::from_RationalNumber(req->target_voltage));

        m_ctx.respond(res);
        m_ctx.feedback.response_code(res.response_code);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        return m_ctx.create_state<PowerDelivery>();

    } else if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, m_ctx.session);

        m_ctx.respond(res);
        m_ctx.session_stopped = true;
        m_ctx.feedback.response_code(res.response_code);

        return {};
    } else {
        m_ctx.log("expected DC_PreChargeReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        m_ctx.feedback.response_code(dt::ResponseCode::FAILED_SequenceError);
        m_ctx.session_stopped = true;
        return {};
    }
}

} // namespace iso15118::d20::state
