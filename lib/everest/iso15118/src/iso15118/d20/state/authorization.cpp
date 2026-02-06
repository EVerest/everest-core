// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <algorithm>

#include <iso15118/d20/state/authorization.hpp>
#include <iso15118/d20/state/service_discovery.hpp>
#include <iso15118/d20/timeout.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/authorization.hpp>
#include <iso15118/detail/d20/state/session_stop.hpp>

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

using AuthStatus = dt::AuthStatus;

static bool find_auth_service_in_offered_services(const dt::Authorization& req_selected_auth_service,
                                                  const d20::Session& session) {
    auto& offered_auth_services = session.offered_services.auth_services;
    return std::find(offered_auth_services.begin(), offered_auth_services.end(), req_selected_auth_service) !=
           offered_auth_services.end();
}

message_20::AuthorizationResponse handle_request(const message_20::AuthorizationRequest& req,
                                                 const d20::Session& session,
                                                 const dt::AuthStatus& authorization_status, bool timeout_reached) {

    message_20::AuthorizationResponse res = message_20::AuthorizationResponse();

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_UnknownSession);
    }

    if (timeout_reached) {
        return response_with_code(res, dt::ResponseCode::FAILED);
    }

    // [V2G20-2209] Check if authorization service was offered in authorization_setup res
    if (not find_auth_service_in_offered_services(req.selected_authorization_service, session)) {
        return response_with_code(
            res, dt::ResponseCode::WARNING_AuthorizationSelectionInvalid); // [V2G20-2226] Handling if warning
    }

    auto response_code = dt::ResponseCode::OK;

    switch (req.selected_authorization_service) {
    case dt::Authorization::EIM:
        switch (authorization_status) {
        case AuthStatus::Accepted:
            res.evse_processing = dt::Processing::Finished;
            response_code = dt::ResponseCode::OK;
            break;
        case AuthStatus::Rejected: // Failure [V2G20-2230]
            res.evse_processing = dt::Processing::Finished;
            response_code = dt::ResponseCode::WARNING_EIMAuthorizationFailure;
            break;
        case AuthStatus::Pending:
        default:
            res.evse_processing = dt::Processing::Ongoing;
            response_code = dt::ResponseCode::OK;
            break;
        }
        break;

    case dt::Authorization::PnC:
        // TODO(SL): Handle PnC
        break;

    default:
        // TODO(SL): Fill
        break;
    }

    return response_with_code(res, response_code);
}

void Authorization::enter() {
    m_ctx.log.enter_state("Authorization");
}

Result Authorization::feed(Event ev) {
    if (ev == Event::CONTROL_MESSAGE) {
        const auto control_data = m_ctx.get_control_event<AuthorizationResponse>();

        if (not control_data) {
            // Ignore control message
            return {};
        }

        if (*control_data) {
            authorization_status = AuthStatus::Accepted;
        } else {
            authorization_status = AuthStatus::Rejected;
        }

        return {};
    }

    if (ev == Event::TIMEOUT) {
        const auto timeout = m_ctx.get_active_timeout();
        if (timeout and *timeout == d20::TimeoutType::ONGOING) {
            timeout_ongoing_reached = true;
        }
        return {};
    }

    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_request();

    if (const auto req = variant->get_if<message_20::AuthorizationRequest>()) {

        if (first_req_msg) {
            // TODO(SL): Check if ExternalPayment or Contract is active
            m_ctx.start_timeout(d20::TimeoutType::ONGOING, TIMEOUT_EIM_ONGOING);
            first_req_msg = false;
        }

        const auto res = handle_request(*req, m_ctx.session, authorization_status, timeout_ongoing_reached);

        m_ctx.respond(res);
        m_ctx.feedback.response_code(res.response_code);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        if (authorization_status == AuthStatus::Accepted) {
            authorization_status = AuthStatus::Pending; // reset
            m_ctx.stop_timeout(d20::TimeoutType::ONGOING);
            return m_ctx.create_state<ServiceDiscovery>();
        } else {
            return {};
        }
    } else if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, m_ctx.session);
        m_ctx.respond(res);

        m_ctx.session_stopped = true;
        m_ctx.feedback.response_code(res.response_code);

        return {};
    } else {
        m_ctx.log("expected AuthorizationReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        m_ctx.feedback.response_code(dt::ResponseCode::FAILED_SequenceError);
        m_ctx.session_stopped = true;
        return {};
    }
}

} // namespace iso15118::d20::state
