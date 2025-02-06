// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/state/dc_charge_loop.hpp>
#include <iso15118/d20/state/power_delivery.hpp>
#include <iso15118/d20/state/session_stop.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/dc_pre_charge.hpp>
#include <iso15118/detail/d20/state/power_delivery.hpp>
#include <iso15118/detail/helper.hpp>

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

message_20::PowerDeliveryResponse handle_request(const message_20::PowerDeliveryRequest& req,
                                                 const d20::Session& session) {

    message_20::PowerDeliveryResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_UnknownSession);
    }

    // TODO(sl): Check Req PowerProfile & ChannelSelection

    // Todo(sl): Add standby feature and define as everest module config
    if (req.charge_progress == dt::Progress::Standby) {
        return response_with_code(res, dt::ResponseCode::WARNING_StandbyNotAllowed);
    }

    return response_with_code(res, dt::ResponseCode::OK);
}

void PowerDelivery::enter() {
    m_ctx.log.enter_state("PowerDelivery");
}

Result PowerDelivery::feed(Event ev) {

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
        const auto res = handle_request(*req, m_ctx.session, present_voltage);

        m_ctx.feedback.dc_pre_charge_target_voltage(dt::from_RationalNumber(req->target_voltage));

        m_ctx.respond(res);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        return {};
    } else if (const auto req = variant->get_if<message_20::PowerDeliveryRequest>()) {
        if (req->charge_progress == dt::Progress::Start) {
            m_ctx.feedback.signal(session::feedback::Signal::SETUP_FINISHED);
        }

        const auto& res = handle_request(*req, m_ctx.session);

        m_ctx.respond(res);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        return m_ctx.create_state<DC_ChargeLoop>();
    } else {
        m_ctx.log("Expected DC_PreChargeReq or PowerDeliveryReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        m_ctx.session_stopped = true;
        return {};
    }
}

} // namespace iso15118::d20::state
