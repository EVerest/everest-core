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

message_20::SessionSetupResponse handle_request(const message_20::SessionSetupRequest& req, const d20::Session& session,
                                                const std::string& evse_id, bool new_session) {

    message_20::SessionSetupResponse res;
    setup_header(res.header, session);

    res.evseid = evse_id;

    if (new_session) {
        return response_with_code(res, message_20::ResponseCode::OK_NewSessionEstablished);
    } else {
        return response_with_code(res, message_20::ResponseCode::OK_OldSessionJoined);
    }
}

void SessionSetup::enter() {
    ctx.log.enter_state("SessionSetup");
}

FsmSimpleState::HandleEventReturnType SessionSetup::handle_event(AllocatorType& sa, FsmEvent ev) {

    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.pull_request();

    if (const auto req = variant->get_if<message_20::SessionSetupRequest>()) {

        logf_info("Received session setup with evccid: %s\n", req->evccid.c_str());
        ctx.feedback.evcc_id(req->evccid);

        bool new_session{true};

        if (session_is_zero(req->header)) {
            ctx.session = Session();
        } else if (req->header.session_id == ctx.session.get_id()) {
            new_session = false;
        } else {
            ctx.session = Session();
        }

        evse_id = ctx.session_config.evse_id;

        const auto res = handle_request(*req, ctx.session, evse_id, new_session);

        ctx.respond(res);

        return sa.create_simple<AuthorizationSetup>(ctx);

        // Todo(sl): Going straight to ChargeParameterDiscovery?

    } else {
        ctx.log("expected SessionSetupReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace iso15118::d20::state
