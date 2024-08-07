// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/state/dc_charge_loop.hpp>
#include <iso15118/d20/state/dc_welding_detection.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/dc_charge_loop.hpp>
#include <iso15118/detail/d20/state/power_delivery.hpp>
#include <iso15118/detail/helper.hpp>

namespace iso15118::d20::state {

using Scheduled_DC_Req = message_20::DC_ChargeLoopRequest::Scheduled_DC_CLReqControlMode;
using Scheduled_BPT_DC_Req = message_20::DC_ChargeLoopRequest::BPT_Scheduled_DC_CLReqControlMode;
using Dynamic_DC_Req = message_20::DC_ChargeLoopRequest::Dynamic_DC_CLReqControlMode;
using Dynamic_BPT_DC_Req = message_20::DC_ChargeLoopRequest::BPT_Dynamic_DC_CLReqControlMode;

using Scheduled_DC_Res = message_20::DC_ChargeLoopResponse::Scheduled_DC_CLResControlMode;
using Scheduled_BPT_DC_Res = message_20::DC_ChargeLoopResponse::BPT_Scheduled_DC_CLResControlMode;

std::tuple<message_20::DC_ChargeLoopResponse, std::optional<session::feedback::DcChargeTarget>>
handle_request(const message_20::DC_ChargeLoopRequest& req, const d20::Session& session, const float present_voltage,
               const float present_current, const bool stop) {

    message_20::DC_ChargeLoopResponse res;
    std::optional<session::feedback::DcChargeTarget> charge_target{std::nullopt};

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return {response_with_code(res, message_20::ResponseCode::FAILED_UnknownSession), charge_target};
    }

    if (std::holds_alternative<Scheduled_DC_Req>(req.control_mode)) {

        if (session.get_selected_energy_service() != message_20::ServiceCategory::DC) {
            return {response_with_code(res, message_20::ResponseCode::FAILED), charge_target};
        }

        const auto& req_mode = std::get<Scheduled_DC_Req>(req.control_mode);

        charge_target = {
            message_20::from_RationalNumber(req_mode.target_voltage),
            message_20::from_RationalNumber(req_mode.target_current),
        };

        auto& mode = res.control_mode.emplace<Scheduled_DC_Res>();

    } else if (std::holds_alternative<Scheduled_BPT_DC_Req>(req.control_mode)) {

        if (session.get_selected_energy_service() != message_20::ServiceCategory::DC_BPT) {
            return {response_with_code(res, message_20::ResponseCode::FAILED), charge_target};
        }

        const auto& req_mode = std::get<Scheduled_BPT_DC_Req>(req.control_mode);

        charge_target = {
            message_20::from_RationalNumber(req_mode.target_voltage),
            message_20::from_RationalNumber(req_mode.target_current),
        };

        auto& mode = res.control_mode.emplace<Scheduled_BPT_DC_Res>();
    }

    res.present_voltage = iso15118::message_20::from_float(present_voltage);
    res.present_current = iso15118::message_20::from_float(present_current);

    if (stop) {
        res.status = {0, iso15118::message_20::EvseNotification::Terminate};
    }

    return {response_with_code(res, message_20::ResponseCode::OK), charge_target};
}

void DC_ChargeLoop::enter() {
    ctx.log.enter_state("DC_ChargeLoop");
}

FsmSimpleState::HandleEventReturnType DC_ChargeLoop::handle_event(AllocatorType& sa, FsmEvent ev) {

    if (ev == FsmEvent::CONTROL_MESSAGE) {

        if (const auto control_data = ctx.get_control_event<PresentVoltageCurrent>()) {
            present_voltage = control_data->voltage;
            present_current = control_data->current;
        } else if (const auto control_data = ctx.get_control_event<StopCharging>()) {
            stop = *control_data;
        } else {
            // FIXME (aw): error handling
        }

        return sa.HANDLED_INTERNALLY;
    }

    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.pull_request();

    if (const auto req = variant->get_if<message_20::PowerDeliveryRequest>()) {
        const auto res = handle_request(*req, ctx.session);

        ctx.respond(res);

        if (res.response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        // Reset
        first_entry_in_charge_loop = true;

        // Todo(sl): React properly to Start, Stop, Standby and ScheduleRenegotiation
        if (req->charge_progress == message_20::PowerDeliveryRequest::Progress::Stop) {
            ctx.feedback.signal(session::feedback::Signal::CHARGE_LOOP_FINISHED);
            ctx.feedback.signal(session::feedback::Signal::DC_OPEN_CONTACTOR);
            return sa.create_simple<DC_WeldingDetection>(ctx);
        }

        return sa.HANDLED_INTERNALLY;
    } else if (const auto req = variant->get_if<message_20::DC_ChargeLoopRequest>()) {
        if (first_entry_in_charge_loop) {
            ctx.feedback.signal(session::feedback::Signal::CHARGE_LOOP_STARTED);
            first_entry_in_charge_loop = false;
        }

        const auto [res, charge_target] = handle_request(*req, ctx.session, present_voltage, present_current, stop);

        if (charge_target) {
            ctx.feedback.dc_charge_target(charge_target.value());
        }

        ctx.respond(res);

        if (res.response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        return sa.HANDLED_INTERNALLY;
    } else {
        ctx.log("Expected PowerDeliveryReq or DC_ChargeLoopReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace iso15118::d20::state
