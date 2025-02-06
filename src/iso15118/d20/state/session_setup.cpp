// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <algorithm>

#include <iso15118/d20/state/authorization_setup.hpp>
#include <iso15118/d20/state/session_setup.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/session_setup.hpp>
#include <iso15118/detail/helper.hpp>

static bool session_is_zero(const iso15118::message_20::Header& header) {
    return std::all_of(header.session_id.begin(), header.session_id.end(), [](int i) { return i == 0; });
}

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

message_20::SessionSetupResponse handle_request([[maybe_unused]] const message_20::SessionSetupRequest& req,
                                                const d20::Session& session, const std::string& evse_id,
                                                bool new_session) {

    message_20::SessionSetupResponse res;
    // FIXME(sl): Check req
    setup_header(res.header, session);

    res.evseid = evse_id;

    if (new_session) {
        return response_with_code(res, dt::ResponseCode::OK_NewSessionEstablished);
    } else {
        return response_with_code(res, dt::ResponseCode::OK_OldSessionJoined);
    }
}

void SessionSetup::enter() {
    m_ctx.log.enter_state("SessionSetup");
}

Result SessionSetup::feed(Event ev) {

    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_request();

    if (const auto req = variant->get_if<message_20::SessionSetupRequest>()) {

        logf_info("Received session setup with evccid: %s", req->evccid.c_str());
        m_ctx.feedback.evcc_id(req->evccid);

        bool new_session{true};

        if (session_is_zero(req->header)) {
            m_ctx.session = Session();
        } else if (req->header.session_id == m_ctx.session.get_id()) {
            new_session = false;
        } else {
            m_ctx.session = Session();
        }

        evse_id = m_ctx.session_config.evse_id;

        const auto res = handle_request(*req, m_ctx.session, evse_id, new_session);

        m_ctx.respond(res);

        return m_ctx.create_state<AuthorizationSetup>();

        // Todo(sl): Going straight to ChargeParameterDiscovery?

    } else {
        m_ctx.log("expected SessionSetupReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        m_ctx.session_stopped = true;
        return {};
    }
}

} // namespace iso15118::d20::state
