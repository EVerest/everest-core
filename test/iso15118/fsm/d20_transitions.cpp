// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include "helper.hpp"

#include <iso15118/d20/state/session_setup.hpp>
#include <iso15118/d20/state/supported_app_protocol.hpp>

#include <iso15118/message/supported_app_protocol.hpp>

using namespace iso15118;

SCENARIO("ISO15118-20 state transitions") {

    auto state_helper = FsmStateHelper(d20::SessionConfig());

    d20::state::SupportedAppProtocol state(state_helper.get_context());

    // state.enter(); // ctx.log.enter_state() breaks something

    message_20::SupportedAppProtocolRequest req;
    auto& ap = req.app_protocol.emplace_back();
    ap.priority = 0;
    ap.protocol_namespace = "Foobar";
    ap.schema_id = 12;
    ap.version_number_major = 2;
    ap.version_number_minor = 11;

    auto res = state_helper.handle_request(state, io::v2gtp::PayloadType::SAP, req);

    REQUIRE(res.is_new_state());

    REQUIRE(state_helper.next_simple_state_is<d20::state::SessionSetup>());
}
