// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/state/supported_app_protocol.hpp>

#include <iso15118/d20/state/session_setup.hpp>

#include <iso15118/message/supported_app_protocol.hpp>

#include <iso15118/detail/d20/context_helper.hpp>

namespace iso15118::d20::state {

static auto handle_request(const message_20::SupportedAppProtocolRequest& req) {
    message_20::SupportedAppProtocolResponse res;

    for (const auto& protocol : req.app_protocol) {
        if (protocol.protocol_namespace.compare("urn:iso:std:iso:15118:-20:DC") == 0) {
            res.schema_id = protocol.schema_id;
            return response_with_code(res,
                                      message_20::SupportedAppProtocolResponse::ResponseCode::OK_SuccessfulNegotiation);
        }
    }

    return response_with_code(res, message_20::SupportedAppProtocolResponse::ResponseCode::Failed_NoNegotiation);
}

void SupportedAppProtocol::enter() {
    ctx.log.enter_state("SupportedAppProtocol");
}

FsmSimpleState::HandleEventReturnType SupportedAppProtocol::handle_event(AllocatorType& sa, FsmEvent ev) {
    if (ev == FsmEvent::V2GTP_MESSAGE) {
        auto variant = ctx.pull_request();
        if (variant->get_type() != message_20::Type::SupportedAppProtocolReq) {
            ctx.log("expected SupportedAppProtocolReq!");
            return sa.PASS_ON;
        }

        const auto res = handle_request(variant->get<message_20::SupportedAppProtocolRequest>());

        ctx.respond(res);
        return sa.create_simple<SessionSetup>(ctx);
    }

    return sa.PASS_ON;
}

} // namespace iso15118::d20::state
