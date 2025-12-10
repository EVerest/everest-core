// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include "helper.hpp"

#include <iso15118/d20/state/authorization_setup.hpp>
#include <iso15118/d20/state/session_setup.hpp>

#include <iso15118/message/session_setup.hpp>

using namespace iso15118;

SCENARIO("ISO15118-20 session setup state transitions") {

    namespace dt = message_20::datatypes;

    // Move to helper function?
    const auto evse_id = std::string("everest se");
    const std::vector<dt::ServiceCategory> supported_energy_services = {dt::ServiceCategory::DC};
    const auto cert_install{false};
    const std::vector<uint16_t> vas_services{};
    const std::vector<dt::Authorization> auth_services = {dt::Authorization::EIM};
    const d20::DcTransferLimits dc_limits;
    const d20::AcTransferLimits ac_limits;
    const d20::DcTransferLimits powersupply_limits;
    const std::vector<d20::ControlMobilityNeedsModes> control_mobility_modes = {
        {dt::ControlMode::Scheduled, dt::MobilityNeedsMode::ProvidedByEvcc}};

    const d20::EvseSetupConfig evse_setup{
        evse_id,   supported_energy_services, auth_services, vas_services, cert_install, dc_limits,
        ac_limits, control_mobility_modes,    std::nullopt,  std::nullopt, std::nullopt, powersupply_limits};

    std::optional<d20::PauseContext> pause_ctx{std::nullopt};

    const session::feedback::Callbacks callbacks{};

    auto state_helper = FsmStateHelper(d20::SessionConfig(evse_setup), pause_ctx, callbacks);
    auto ctx = state_helper.get_context();

    const auto session_id = std::array<uint8_t, 8>{0x10, 0x34, 0xAB, 0x7A, 0x01, 0xF3, 0x95, 0x02};

    auto& pause = pause_ctx.emplace();
    pause.selected_service_parameters =
        d20::SelectedServiceParameters(dt::ServiceCategory::DC, dt::DcConnector::Extended, dt::ControlMode::Dynamic,
                                       dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing);
    pause.vehicle_cert_session_id_hash = {0x58, 0xD6, 0x9A, 0x86, 0xF5, 0xCF, 0x86, 0xC0, 0x2F, 0x06, 0x1A, 0xAC, 0x9B,
                                          0x26, 0x83, 0x0F, 0xB1, 0x66, 0xCC, 0x4E, 0xC8, 0x75, 0x68, 0xA2, 0xF2, 0x3D,
                                          0x11, 0xC7, 0x61, 0x64, 0x18, 0x34, 0x56, 0x7A, 0x34, 0x30, 0x0C, 0x7E, 0x9A,
                                          0xF4, 0x00, 0xF5, 0xBC, 0x61, 0x46, 0xC5, 0xA2, 0x74, 0x52, 0xFE, 0x15, 0x7D,
                                          0x28, 0x77, 0xC8, 0x52, 0x06, 0x59, 0x22, 0x45, 0x3A, 0x9F, 0xA6, 0x54};
    pause.old_session_id = session_id;

    GIVEN("Good case - New session") {

        fsm::v2::FSM<d20::StateBase> fsm{ctx.create_state<d20::state::SessionSetup>()};

        const auto header_req = message_20::Header{{0, 0, 0, 0, 0, 0, 0, 0}, 1691411798};
        const auto req = message_20::SessionSetupRequest{header_req, "WMIV1234567890ABCDEX"};

        state_helper.handle_request(req);
        const auto result = fsm.feed(d20::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == d20::StateID::AuthorizationSetup);
            REQUIRE(ctx.session.get_id() != std::array<uint8_t, 8>{0});

            const auto response_message = ctx.get_response<message_20::SessionSetupResponse>();
            REQUIRE(response_message.has_value());

            const auto& session_setup_res = response_message.value();
            REQUIRE(session_setup_res.response_code == dt::ResponseCode::OK_NewSessionEstablished);
            REQUIRE(session_setup_res.evseid == evse_id);
        }
    }

    GIVEN("Good case - resume old session") {
        fsm::v2::FSM<d20::StateBase> fsm{ctx.create_state<d20::state::SessionSetup>()};

        ctx.set_new_vehicle_cert_hash(io::sha512_hash_t{
            0x3F, 0x66, 0xE4, 0x5F, 0x3A, 0x30, 0x3B, 0x8F, 0x47, 0xCD, 0xD6, 0x86, 0xAD, 0x75, 0x13, 0x6F,
            0xCE, 0x44, 0xE6, 0xAD, 0xDC, 0x52, 0x8A, 0x6A, 0x3D, 0xAC, 0x5F, 0x8D, 0xCB, 0x5A, 0x67, 0xF3,
            0xE5, 0xA5, 0xF2, 0x56, 0x74, 0x5A, 0xFA, 0xF2, 0x28, 0x31, 0xCE, 0xAB, 0xE8, 0x3C, 0xD7, 0x3C,
            0xF2, 0x83, 0x81, 0xAA, 0x5D, 0x87, 0x13, 0xA5, 0x78, 0xA8, 0xB4, 0xAB, 0x0D, 0x62, 0x1F, 0x83});

        const auto header_req = message_20::Header{session_id, 1691411798};
        const auto req = message_20::SessionSetupRequest{header_req, "WMIV1234567890ABCDEX"};

        state_helper.handle_request(req);
        const auto result = fsm.feed(d20::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == d20::StateID::DC_ChargeParameterDiscovery);
            REQUIRE(ctx.session.get_id() == session_id);

            const auto response_message = ctx.get_response<message_20::SessionSetupResponse>();
            REQUIRE(response_message.has_value());

            const auto& session_setup_res = response_message.value();
            REQUIRE(session_setup_res.response_code == dt::ResponseCode::OK_OldSessionJoined);
            REQUIRE(session_setup_res.evseid == evse_id);
        }

        pause_ctx.reset();
    }

    GIVEN("Try to resume old session with another session id") {
        fsm::v2::FSM<d20::StateBase> fsm{ctx.create_state<d20::state::SessionSetup>()};

        ctx.set_new_vehicle_cert_hash(io::sha512_hash_t{
            0x3F, 0x66, 0xE4, 0x5F, 0x3A, 0x30, 0x3B, 0x8F, 0x47, 0xCD, 0xD6, 0x86, 0xAD, 0x75, 0x13, 0x6F,
            0xCE, 0x44, 0xE6, 0xAD, 0xDC, 0x52, 0x8A, 0x6A, 0x3D, 0xAC, 0x5F, 0x8D, 0xCB, 0x5A, 0x67, 0xF3,
            0xE5, 0xA5, 0xF2, 0x56, 0x74, 0x5A, 0xFA, 0xF2, 0x28, 0x31, 0xCE, 0xAB, 0xE8, 0x3C, 0xD7, 0x3C,
            0xF2, 0x83, 0x81, 0xAA, 0x5D, 0x87, 0x13, 0xA5, 0x78, 0xA8, 0xB4, 0xAB, 0x0D, 0x62, 0x1F, 0x83});

        const auto header_req = message_20::Header{{0x10, 0x34, 0xAB, 0x7B, 0x01, 0xF3, 0x95, 0x02}, 1691411798};
        const auto req = message_20::SessionSetupRequest{header_req, "WMIV1234567890ABCDEX"};

        state_helper.handle_request(req);
        const auto result = fsm.feed(d20::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == d20::StateID::AuthorizationSetup);
            REQUIRE(ctx.session.get_id() != session_id);
            REQUIRE(ctx.session.get_id() != std::array<uint8_t, 8>{0});

            const auto response_message = ctx.get_response<message_20::SessionSetupResponse>();
            REQUIRE(response_message.has_value());

            const auto& session_setup_res = response_message.value();
            REQUIRE(session_setup_res.response_code == dt::ResponseCode::OK_NewSessionEstablished);
            REQUIRE(session_setup_res.evseid == evse_id);
        }

        pause_ctx.reset();
    }

    GIVEN("Try to resume old session with no vehicle cert hash") {
        fsm::v2::FSM<d20::StateBase> fsm{ctx.create_state<d20::state::SessionSetup>()};

        const auto header_req = message_20::Header{session_id, 1691411798};
        const auto req = message_20::SessionSetupRequest{header_req, "WMIV1234567890ABCDEX"};

        state_helper.handle_request(req);
        const auto result = fsm.feed(d20::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == d20::StateID::AuthorizationSetup);
            REQUIRE(ctx.session.get_id() != session_id);
            REQUIRE(ctx.session.get_id() != std::array<uint8_t, 8>{0});

            const auto response_message = ctx.get_response<message_20::SessionSetupResponse>();
            REQUIRE(response_message.has_value());

            const auto& session_setup_res = response_message.value();
            REQUIRE(session_setup_res.response_code == dt::ResponseCode::OK_NewSessionEstablished);
            REQUIRE(session_setup_res.evseid == evse_id);
        }

        pause_ctx.reset();
    }

    GIVEN("Try to resume old session with different vehicle cert hash") {
        fsm::v2::FSM<d20::StateBase> fsm{ctx.create_state<d20::state::SessionSetup>()};

        ctx.set_new_vehicle_cert_hash(io::sha512_hash_t{
            0x3F, 0x66, 0xE4, 0x5F, 0x3A, 0x30, 0x3B, 0x8F, 0x47, 0xCD, 0xD6, 0x86, 0xAD, 0x75, 0x13, 0x6F,
            0xDF, 0x44, 0xE6, 0xAD, 0xDC, 0x52, 0x8A, 0x6A, 0x3D, 0xAC, 0x5F, 0x8D, 0xCB, 0x5A, 0x67, 0xF3,
            0xE5, 0xA5, 0xF2, 0x56, 0x74, 0x5A, 0xFA, 0xF2, 0x28, 0x31, 0xCE, 0xAB, 0xE8, 0x3C, 0xD7, 0x3C,
            0xF2, 0x83, 0x81, 0xAA, 0x5D, 0x87, 0x13, 0xA5, 0x78, 0xA8, 0xB4, 0xAB, 0x0D, 0x62, 0x1F, 0x83});

        const auto header_req = message_20::Header{session_id, 1691411798};
        const auto req = message_20::SessionSetupRequest{header_req, "WMIV1234567890ABCDEX"};

        state_helper.handle_request(req);
        const auto result = fsm.feed(d20::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == d20::StateID::AuthorizationSetup);
            REQUIRE(ctx.session.get_id() != session_id);
            REQUIRE(ctx.session.get_id() != std::array<uint8_t, 8>{0});

            const auto response_message = ctx.get_response<message_20::SessionSetupResponse>();
            REQUIRE(response_message.has_value());

            const auto& session_setup_res = response_message.value();
            REQUIRE(session_setup_res.response_code == dt::ResponseCode::OK_NewSessionEstablished);
            REQUIRE(session_setup_res.evseid == evse_id);
        }

        pause_ctx.reset();
    }
}
