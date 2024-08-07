// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <random>

#include <iso15118/d20/state/authorization.hpp>
#include <iso15118/d20/state/authorization_setup.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/helper.hpp>

#include <iso15118/detail/d20/state/authorization_setup.hpp>
#include <iso15118/detail/d20/state/session_stop.hpp>

namespace iso15118::d20::state {

message_20::AuthorizationSetupResponse
handle_request(const message_20::AuthorizationSetupRequest& req, d20::Session& session, bool cert_install_service,
               const std::vector<message_20::Authorization>& authorization_services) {

    auto res = message_20::AuthorizationSetupResponse(); // default mandatory values [V2G20-736]

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, message_20::ResponseCode::FAILED_UnknownSession);
    }

    res.certificate_installation_service = cert_install_service;

    if (authorization_services.empty()) {
        logf("Warning: authorization_services was not set. Setting EIM as auth_mode\n");
        res.authorization_services = {message_20::Authorization::EIM};
    } else {
        res.authorization_services = authorization_services;
    }

    session.offered_services.auth_services = res.authorization_services;

    if (res.authorization_services.size() == 1 && res.authorization_services[0] == message_20::Authorization::EIM) {
        res.authorization_mode.emplace<message_20::AuthorizationSetupResponse::EIM_ASResAuthorizationMode>();
    } else {
        auto& pnc_auth_mode =
            res.authorization_mode.emplace<message_20::AuthorizationSetupResponse::PnC_ASResAuthorizationMode>();

        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<uint8_t> distribution(0x00, 0xff);

        for (auto& item : pnc_auth_mode.gen_challenge) {
            item = distribution(generator);
        }
    }

    return response_with_code(res, message_20::ResponseCode::OK);
}

void AuthorizationSetup::enter() {
    ctx.log.enter_state("AuthorizationSetup");
}

FsmSimpleState::HandleEventReturnType AuthorizationSetup::handle_event(AllocatorType& sa, FsmEvent ev) {

    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.pull_request();

    if (const auto req = variant->get_if<message_20::AuthorizationSetupRequest>()) {
        const auto res =
            handle_request(*req, ctx.session, ctx.config.cert_install_service, ctx.config.authorization_services);

        logf("Timestamp: %d\n", req->header.timestamp);

        ctx.respond(res);

        if (res.response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        // Todo(sl): PnC is currently not supported
        ctx.feedback.signal(session::feedback::Signal::REQUIRE_AUTH_EIM);

        return sa.create_simple<Authorization>(ctx);
    } else if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, ctx.session);

        ctx.respond(res);
        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected AuthorizationSetupReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace iso15118::d20::state
