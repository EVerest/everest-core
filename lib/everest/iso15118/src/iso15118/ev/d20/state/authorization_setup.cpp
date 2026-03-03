// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH, Roger Bedell and Contributors to EVerest
#include <iso15118/detail/helper.hpp>
#include <iso15118/ev/d20/context.hpp>
#include <iso15118/ev/d20/state/authorization.hpp>
#include <iso15118/ev/d20/state/authorization_setup.hpp>
#include <iso15118/ev/detail/d20/context_helper.hpp>
#include <iso15118/ev/session/feedback.hpp>
#include <iso15118/message/authorization.hpp>
#include <iso15118/message/authorization_setup.hpp>

namespace iso15118::ev::d20::state {

namespace {
using ResponseCode = message_20::datatypes::ResponseCode;
bool check_response_code(ResponseCode response_code) {
    switch (response_code) {
    case ResponseCode::OK:
        return true;
    default:
        logf_warning("Unexpected response code received: %d", static_cast<int>(response_code));
        return iso15118::ev::d20::check_response_code(response_code);
    }
}
} // namespace

void AuthorizationSetup::enter() {
    // TODO(SL): Adding logging
}

Result AuthorizationSetup::feed([[maybe_unused]] Event ev) {
    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_response();

    if (const auto res = variant->get_if<message_20::AuthorizationSetupResponse>()) {

        if (not check_response_code(res->response_code)) {
            m_ctx.stop_session(true); // Tell stack to close the tcp/tls connection
            return {};
        }

        if (res->certificate_installation_service) {
            // TODO(RB): Implement certificate installation service
            logf_warning("EVSE supports certificate installation service, but this is not implemented in the EV yet.");
            // Remember it in the context for later use
            m_ctx.evse_session_info.certificate_installation_service = true;
        }

        // Save the offered authorization services in the session context so we can use it later and also report it to
        // the EV.
        m_ctx.evse_session_info.auth_services = res->authorization_services;

        if (std::holds_alternative<message_20::datatypes::PnC_ASResAuthorizationMode>(res->authorization_mode)) {
            const auto& pnc_auth_mode =
                std::get<message_20::datatypes::PnC_ASResAuthorizationMode>(res->authorization_mode);
            m_ctx.evse_session_info.supported_providers = pnc_auth_mode.supported_providers;
            m_ctx.evse_session_info.gen_challenge = pnc_auth_mode.gen_challenge;
        } else {
            // EIM selected, nothing to do here for now since authorization_mode is empty for EIM
        }
        // Inform the ev about the evse session information
        m_ctx.feedback.evse_session_info(m_ctx.evse_session_info);

        // Send request and transition to next state
        message_20::AuthorizationRequest req;
        setup_header(req.header, m_ctx.get_session());
        // TODO(RB): Choose the authorization service based on user preference and what the evse supports
        // For now, we just pick the first one offered by the evse
        if (m_ctx.evse_session_info.auth_services.empty()) {
            logf_error("No authorization services offered by the EVSE. Abort the session.");
            m_ctx.stop_session(true); // Tell stack to close the tcp/tls connection
            return {};
        }
        req.selected_authorization_service = m_ctx.evse_session_info.auth_services.front();
        if (req.selected_authorization_service == message_20::datatypes::Authorization::EIM) {
            req.authorization_mode = message_20::datatypes::EIM_ASReqAuthorizationMode{};
        } else if (req.selected_authorization_service == message_20::datatypes::Authorization::PnC) {
            // TODO(RB): Fill in the PnC authorization mode data
            req.authorization_mode = message_20::datatypes::PnC_ASReqAuthorizationMode{};
        } else {
            logf_error("Unknown authorization service selected. Abort the session.");
            m_ctx.stop_session(true); // Tell stack to close the tcp/tls connection
            return {};
        }

        // Remember the request in the context in case we need to resend it
        m_ctx.save_request(req);
        m_ctx.respond(req);
        return m_ctx.create_state<Authorization>();
    } else {
        logf_error("expected AuthorizationSetupResponse! But code type id: %d", variant->get_type());
        m_ctx.stop_session(true); // Tell stack to close the tcp/tls connection
        return {};
    }
}
} // namespace iso15118::ev::d20::state
