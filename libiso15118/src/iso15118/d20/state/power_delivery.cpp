// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/state/ac_charge_loop.hpp>
#include <iso15118/d20/state/dc_charge_loop.hpp>
#include <iso15118/d20/state/power_delivery.hpp>
#include <iso15118/d20/state/session_stop.hpp>
#include <iso15118/d20/timeout.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/dc_pre_charge.hpp>
#include <iso15118/detail/d20/state/power_delivery.hpp>
#include <iso15118/detail/helper.hpp>

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

message_20::PowerDeliveryResponse handle_request(const message_20::PowerDeliveryRequest& req,
                                                 const d20::Session& session, bool contactor_error) {

    message_20::PowerDeliveryResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_UnknownSession);
    }

    if (contactor_error) {
        return response_with_code(res, dt::ResponseCode::FAILED_ContactorError);
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

    const auto selected_energy_service = m_ctx.session.get_selected_services().selected_energy_service;
    if (ev == Event::CONTROL_MESSAGE) {

        if (const auto* control_data = m_ctx.get_control_event<PresentVoltageCurrent>()) {
            present_voltage = control_data->voltage;
        } else if (const auto* control_data = m_ctx.get_control_event<ClosedContactor>()) {
            ac_connector_closed = control_data;

            if (not ac_connector_closed) {
                logf_warning(
                    "Got ClosedContactor event, but contactor is not closed.  Waiting until the contactor is closed");
                return {};
            }

            m_ctx.stop_timeout(d20::TimeoutType::CONTACTOR);

            if (not previous_req.has_value()) {
                logf_warning("There is no power_delivery_req messages saved!");
                return {};
            }

            const auto& res = handle_request(previous_req.value(), m_ctx.session, false);
            m_ctx.respond(res);

            if (res.response_code >= dt::ResponseCode::FAILED) {
                m_ctx.session_stopped = true;
                return {};
            }

            return m_ctx.create_state<AC_ChargeLoop>();
        }

        return {};
    }

    if (ev == Event::TIMEOUT) {
        const auto timeout = m_ctx.get_active_timeout();
        if (timeout and *timeout == d20::TimeoutType::CONTACTOR) {
            // TODO(SL): Check if value_or is the correct way
            const auto& res =
                handle_request(previous_req.value_or(message_20::PowerDeliveryRequest{}), m_ctx.session, true);
            m_ctx.respond(res);
            m_ctx.session_stopped = true;
        }
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

        if (m_ctx.session.is_ac_charger() and ac_connector_closed == false and
            req->charge_progress == dt::Progress::Start) {
            // Save req
            previous_req = *req;
            // Close the AC contactor so that charging can start
            m_ctx.feedback.signal(session::feedback::Signal::AC_CLOSE_CONTACTOR);
            m_ctx.start_timeout(d20::TimeoutType::CONTACTOR, 3000);
            logf_info("Waiting for contactor is closed");
            return {};
        }

        const auto& res = handle_request(*req, m_ctx.session, false);

        m_ctx.respond(res);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        if (m_ctx.session.is_ac_charger()) {
            return m_ctx.create_state<AC_ChargeLoop>();
        }
        if (m_ctx.session.is_dc_charger()) {
            return m_ctx.create_state<DC_ChargeLoop>();
        }
        m_ctx.log("expected selected_energy_service AC, AC_BPT, DC, DC_BPT! But code type id: %d",
                  static_cast<int>(selected_energy_service));

        m_ctx.session_stopped = true;
        return {};

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
