// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH, Roger Bedell, and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include "helper.hpp"

#include <iso15118/ev/d20/state/authorization.hpp>
#include <iso15118/ev/d20/state/authorization_setup.hpp>
#include <iso15118/ev/d20/state/service_discovery.hpp>
#include <iso15118/message/authorization.hpp>
#include <iso15118/message/authorization_setup.hpp>
#include <iso15118/message/service_discovery.hpp>
#include <iso15118/message/type.hpp>

using namespace iso15118;

SCENARIO("ISO15118-20 EV authorization state transitions") {

    const ev::d20::session::feedback::Callbacks callbacks{};

    auto state_helper = FsmStateHelper(callbacks);

    auto ctx = state_helper.get_context();

    GIVEN("Good case - authorization response with OK and EVSEProcessing Finished") {

        // setup the state and context to something reasonable
        const auto header = message_20::Header{
            .session_id = std::array<uint8_t, 8>{0x10, 0x34, 0xAB, 0x7A, 0x01, 0xF3, 0x95, 0x02},
            .timestamp = 1691411798,
        };

        ctx.get_session().set_id(header.session_id);

        fsm::v2::FSM<ev::d20::StateBase> fsm{ctx.create_state<ev::d20::state::Authorization>()};

        const auto res = message_20::AuthorizationResponse{header, message_20::datatypes::ResponseCode::OK,
                                                           message_20::datatypes::Processing::Finished};

        state_helper.handle_response(res);

        const auto result = fsm.feed(ev::d20::Event::V2GTP_MESSAGE);

        THEN("Check if passes to Service Discovery state and sends ServiceDiscoveryRequest") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == ev::d20::StateID::ServiceDiscovery);

            const auto request_message = ctx.get_request<message_20::ServiceDiscoveryRequest>();
            REQUIRE(request_message.has_value());

            const auto& request = request_message.value();
            REQUIRE(request.header.session_id == header.session_id);
        }
    }

    // This is a little different, because we need to resend the AuthorizationRequest which needs to have been saved
    // before (see AuthorizationSetup state)
    GIVEN("Good case - authorization response with OK and EVSEProcessing Ongoing") {

        // setup the state and context to something reasonable
        const auto header = message_20::Header{
            .session_id = std::array<uint8_t, 8>{0x10, 0x34, 0xAB, 0x7A, 0x01, 0xF3, 0x95, 0x02},
            .timestamp = 1691411798,
        };

        ctx.get_session().set_id(header.session_id);

        fsm::v2::FSM<ev::d20::StateBase> fsm{ctx.create_state<ev::d20::state::AuthorizationSetup>()};

        const auto res = message_20::AuthorizationSetupResponse{
            header, message_20::datatypes::ResponseCode::OK, {message_20::datatypes::Authorization::EIM}, false};

        state_helper.handle_response(res);

        const auto result = fsm.feed(ev::d20::Event::V2GTP_MESSAGE);
        REQUIRE(result.transitioned() == true);
        REQUIRE(fsm.get_current_state_id() == ev::d20::StateID::Authorization);

        const auto res2 = message_20::AuthorizationResponse{header, message_20::datatypes::ResponseCode::OK,
                                                            message_20::datatypes::Processing::Ongoing};

        state_helper.handle_response(res2);

        const auto result2 = fsm.feed(ev::d20::Event::V2GTP_MESSAGE);

        THEN("Check if stays in Authorization state and sends AuthorizationRequest again") {
            REQUIRE(result2.transitioned() == false);
            REQUIRE(fsm.get_current_state_id() == ev::d20::StateID::Authorization);

            const auto request_message = ctx.get_request<message_20::AuthorizationRequest>();
            REQUIRE(request_message.has_value());

            const auto& request = request_message.value();
            REQUIRE(request.header.session_id == header.session_id);
            REQUIRE(request.selected_authorization_service == message_20::datatypes::Authorization::EIM);
            REQUIRE(
                std::holds_alternative<message_20::datatypes::EIM_ASReqAuthorizationMode>(request.authorization_mode));
        }
    }
}
