// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include "helper.hpp"

#include <iso15118/d2/state/service_discovery.hpp>
#include <iso15118/d2/state/session_setup.hpp>

#include <iso15118/message/d2/session_setup.hpp>

using namespace iso15118;

SCENARIO("ISO15118-2 session setup state transitions") {

    namespace dt = d2::msg::data_types;

    // Move to helper function?
    const auto evse_id = std::string("everest se");
    d2::EvseSetupConfig evse_setup{};
    evse_setup.evse_id = evse_id;

    session::feedback::Callbacks feedback_callbacks;

    auto state_helper = FsmStateHelper(d2::SessionConfig(evse_setup), feedback_callbacks);
    auto ctx = state_helper.get_context();

    GIVEN("Good case - New session") {

        fsm::v2::FSM<d2::StateBase> fsm{ctx.create_state<d2::state::SessionSetup>()};

        const auto header_req = d2::msg::Header{{0, 0, 0, 0, 0, 0, 0, 0}, std::nullopt};
        const auto req = d2::msg::SessionSetupRequest{header_req, {0x12, 0xE2, 0x3E, 0x10, 0xD3, 0x48}};

        state_helper.handle_request(req);
        const auto result = fsm.feed(d2::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == d2::StateID::ServiceDiscovery);
            REQUIRE(ctx.session.get_id() != std::array<uint8_t, 8>{0});

            const auto response_message = ctx.get_response<d2::msg::SessionSetupResponse>();
            REQUIRE(response_message.has_value());

            const auto& session_setup_res = response_message.value();
            REQUIRE(session_setup_res.response_code == dt::ResponseCode::OK_NewSessionEstablished);
            REQUIRE(session_setup_res.evse_id == evse_id);
        }
    }
}
