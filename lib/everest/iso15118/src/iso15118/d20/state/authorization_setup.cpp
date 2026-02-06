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

namespace dt = message_20::datatypes;

message_20::AuthorizationSetupResponse handle_request(const message_20::AuthorizationSetupRequest& req,
                                                      d20::Session& session, bool cert_install_service,
                                                      const std::vector<dt::Authorization>& authorization_services) {

    auto res = message_20::AuthorizationSetupResponse(); // default mandatory values [V2G20-736]

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_UnknownSession);
    }

    res.certificate_installation_service = cert_install_service;

    if (authorization_services.empty()) {
        logf_warning("authorization_services was not set. Setting EIM as auth_mode");
        res.authorization_services = {dt::Authorization::EIM};
    } else {
        res.authorization_services = authorization_services;
    }

    session.offered_services.auth_services = res.authorization_services;

    if (res.authorization_services.size() == 1 && res.authorization_services[0] == dt::Authorization::EIM) {
        res.authorization_mode.emplace<dt::EIM_ASResAuthorizationMode>();
    } else {
        auto& pnc_auth_mode = res.authorization_mode.emplace<dt::PnC_ASResAuthorizationMode>();

        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<uint8_t> distribution(0x00, 0xff);

        for (auto& item : pnc_auth_mode.gen_challenge) {
            item = distribution(generator);
        }
    }

    return response_with_code(res, dt::ResponseCode::OK);
}

void AuthorizationSetup::enter() {
    m_ctx.log.enter_state("AuthorizationSetup");
}

Result AuthorizationSetup::feed(Event ev) {

    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_request();

    if (const auto req = variant->get_if<message_20::AuthorizationSetupRequest>()) {
        const auto res = handle_request(*req, m_ctx.session, m_ctx.session_config.cert_install_service,
                                        m_ctx.session_config.authorization_services);

        logf_info("Timestamp: %d", req->header.timestamp);

        m_ctx.respond(res);
        m_ctx.feedback.response_code(res.response_code);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        // Todo(sl): PnC is currently not supported
        m_ctx.feedback.signal(session::feedback::Signal::REQUIRE_AUTH_EIM);

        return m_ctx.create_state<Authorization>();
    } else if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, m_ctx.session);

        m_ctx.respond(res);
        m_ctx.session_stopped = true;
        m_ctx.feedback.response_code(res.response_code);

        return {};
    } else {
        m_ctx.log("expected AuthorizationSetupReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        m_ctx.feedback.response_code(dt::ResponseCode::FAILED_SequenceError);
        m_ctx.session_stopped = true;
        return {};
    }
}

} // namespace iso15118::d20::state
