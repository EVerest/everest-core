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
    ctx.log.enter_state("DC_PreCharge");
}

FsmSimpleState::HandleEventReturnType DC_PreCharge::handle_event(AllocatorType& sa, FsmEvent ev) {

    if (ev == FsmEvent::CONTROL_MESSAGE) {
        const auto control_data = ctx.get_control_event<PresentVoltageCurrent>();
        if (not control_data) {
            // Ignore control message
            return sa.HANDLED_INTERNALLY;
        }

        present_voltage = control_data->voltage;

        return sa.HANDLED_INTERNALLY;
    }

    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.pull_request();

    if (const auto req = variant->get_if<message_20::DC_PreChargeRequest>()) {
        const auto res = handle_request(*req, ctx.session, present_voltage);

        ctx.feedback.dc_pre_charge_target_voltage(message_20::datatypes::from_RationalNumber(req->target_voltage));

        ctx.respond(res);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        return sa.create_simple<PowerDelivery>(ctx);

    } else if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, ctx.session);

        ctx.respond(res);
        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected DC_PreChargeReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace iso15118::d20::state
