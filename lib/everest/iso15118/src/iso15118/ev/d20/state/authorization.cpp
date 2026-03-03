// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH, Roger Bedell, and Contributors to EVerest
#include <iso15118/detail/helper.hpp>
#include <iso15118/ev/d20/state/authorization.hpp>
#include <iso15118/ev/d20/state/service_discovery.hpp>
#include <iso15118/ev/detail/d20/context_helper.hpp>
#include <iso15118/message/authorization.hpp>
#include <iso15118/message/service_discovery.hpp>

namespace iso15118::ev::d20::state {

namespace {
using ResponseCode = message_20::datatypes::ResponseCode;
bool check_response_code(ResponseCode response_code) {
    switch (response_code) {
    case ResponseCode::OK:
    case ResponseCode::OK_CertificateExpiresSoon:
        return true;
    case ResponseCode::WARNING_AuthorizationSelectionInvalid:
    case ResponseCode::WARNING_CertificateExpired:
    case ResponseCode::WARNING_CertificateNotYetValid:
    case ResponseCode::WARNING_CertificateRevoked:
    case ResponseCode::WARNING_CertificateValidationError:
    case ResponseCode::WARNING_ChallengeInvalid:
    case ResponseCode::WARNING_EIMAuthorizationFailure:
    case ResponseCode::WARNING_eMSPUnknown:
    case ResponseCode::WARNING_GeneralPnCAuthorizationError:
    case ResponseCode::WARNING_NoCertificateAvailable:
    case ResponseCode::WARNING_NoContractMatchingPCIDFound:
    case ResponseCode::FAILED:
    case ResponseCode::FAILED_SequenceError:
    case ResponseCode::FAILED_UnknownSession:
        return false;
    default:
        logf_warning("Unexpected response code received: %d", static_cast<int>(response_code));
        return iso15118::ev::d20::check_response_code(response_code);
    }
}
} // namespace

void Authorization::enter() {
    // TODO(SL): Adding logging
}

Result Authorization::feed(Event ev) {
    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }
    const auto variant = m_ctx.pull_response();
    if (const auto res = variant->get_if<message_20::AuthorizationResponse>()) {

        if (not check_response_code(res->response_code)) {
            m_ctx.stop_session(true); // Tell stack to close the tcp/tls connection
            return {};
        }

        // If EVSEProcessing, we wait for it to be finished
        if (res->evse_processing != message_20::datatypes::Processing::Finished) {
            // resend the request, it needs to have been saved before.
            const auto request_message = m_ctx.get_saved_request<message_20::AuthorizationRequest>();
            if (request_message.has_value()) {
                m_ctx.respond(request_message.value());
                return {};
            } else {
                logf_error("No AuthorizationRequest found in context to resend.");
                m_ctx.stop_session(true); // Tell stack to close the tcp/tls connection
                return {};
            }
        } else {
            // Send request and transition to next state
            message_20::ServiceDiscoveryRequest req;
            setup_header(req.header, m_ctx.get_session());
            // TODO(RB): Choose the service ids based on user preference and what the evse supports
            // We might want to feedback the supported services to the user via the feedback interface, and
            // then listen for a user selection?
            // For now, we just leave it empty to indicate that we want to hear about all services
            req.supported_service_ids = std::nullopt;

            m_ctx.respond(req);
            return m_ctx.create_state<ServiceDiscovery>();
        }
    } else {
        logf_error("expected AuthorizationResponse! But code type id: %d", variant->get_type());
        m_ctx.stop_session(true); // Tell stack to close the tcp/tls connection
        return {};
    }
}

} // namespace iso15118::ev::d20::state
