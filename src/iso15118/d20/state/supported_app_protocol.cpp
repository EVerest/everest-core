// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/state/supported_app_protocol.hpp>

#include <optional>
#include <tuple>

#include <iso15118/d20/state/session_setup.hpp>

#include <iso15118/message/supported_app_protocol.hpp>

#include <iso15118/detail/d20/context_helper.hpp>

namespace iso15118::d20::state {

static std::tuple<message_20::SupportedAppProtocolResponse, std::optional<std::string>>
handle_request(const message_20::SupportedAppProtocolRequest& req) {
    message_20::SupportedAppProtocolResponse res;
    std::optional<std::string> selected_protocol{std::nullopt};

    for (const auto& protocol : req.app_protocol) {
        if (protocol.protocol_namespace.compare("urn:iso:std:iso:15118:-20:DC") == 0) {
            res.schema_id = protocol.schema_id;
            selected_protocol = "ISO15118-20:DC";
            return {response_with_code(
                        res, message_20::SupportedAppProtocolResponse::ResponseCode::OK_SuccessfulNegotiation),
                    selected_protocol};
        }
    }

    return {response_with_code(res, message_20::SupportedAppProtocolResponse::ResponseCode::Failed_NoNegotiation),
            selected_protocol};
}

void SupportedAppProtocol::enter() {
    ctx.log.enter_state("SupportedAppProtocol");
}

FsmSimpleState::HandleEventReturnType SupportedAppProtocol::handle_event(AllocatorType& sa, FsmEvent ev) {
    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    auto variant = ctx.pull_request();

    if (const auto req = variant->get_if<message_20::SupportedAppProtocolRequest>()) {

        const auto [res, selected_protocol] = handle_request(*req);

        if (selected_protocol.has_value()) {
            ctx.feedback.selected_protocol(*selected_protocol);
        }

        ctx.respond(res);

        return sa.create_simple<SessionSetup>(ctx);

    } else {
        ctx.log("expected SupportedAppProtocolReq! But code type id: %d", variant->get_type());

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace iso15118::d20::state
