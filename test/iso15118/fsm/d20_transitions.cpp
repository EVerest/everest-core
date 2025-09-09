// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include "helper.hpp"

#include <iso15118/d20/state/session_setup.hpp>
#include <iso15118/d20/state/supported_app_protocol.hpp>

#include <iso15118/message/supported_app_protocol.hpp>

using namespace iso15118;

SCENARIO("ISO15118-20 supported app protocol state transitions") {

    // Move to helper function?
    const auto evse_id = std::string("everest se");
    const std::vector<message_20::datatypes::ServiceCategory> supported_energy_services = {
        message_20::datatypes::ServiceCategory::DC};
    const std::vector<uint16_t> vas_services{};
    const auto cert_install{false};
    const std::vector<message_20::datatypes::Authorization> auth_services = {message_20::datatypes::Authorization::EIM};
    const d20::DcTransferLimits dc_limits;
    const d20::AcTransferLimits ac_limits;
    const std::vector<d20::ControlMobilityNeedsModes> control_mobility_modes = {
        {message_20::datatypes::ControlMode::Scheduled, message_20::datatypes::MobilityNeedsMode::ProvidedByEvcc}};
    const std::string custom_namespace = "urn:iso:std:iso:15118:-20:AABB";

    const d20::EvseSetupConfig evse_setup{
        evse_id,   supported_energy_services, auth_services,   vas_services, cert_install, dc_limits,
        ac_limits, control_mobility_modes,    custom_namespace};

    std::optional<d20::PauseContext> pause_ctx{std::nullopt};

    const session::feedback::Callbacks callbacks{};

    auto state_helper = FsmStateHelper(d20::SessionConfig(evse_setup), pause_ctx, callbacks);
    auto ctx = state_helper.get_context();

    GIVEN("Good case - DC") {
        fsm::v2::FSM<d20::StateBase> fsm{ctx.create_state<d20::state::SupportedAppProtocol>()};

        message_20::SupportedAppProtocolRequest req;
        auto& ap = req.app_protocol.emplace_back();
        ap.priority = 1;
        ap.protocol_namespace = "urn:iso:std:iso:15118:-20:DC";
        ap.schema_id = 1;
        ap.version_number_major = 1;
        ap.version_number_minor = 0;

        state_helper.handle_request(req);
        const auto result = fsm.feed(d20::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == d20::StateID::SessionSetup);

            const auto response_message = ctx.get_response<message_20::SupportedAppProtocolResponse>();
            REQUIRE(response_message.has_value());

            const auto& supported_app_res = response_message.value();

            REQUIRE(supported_app_res.response_code ==
                    message_20::SupportedAppProtocolResponse::ResponseCode::OK_SuccessfulNegotiation);
            REQUIRE(supported_app_res.schema_id.value_or(0) == 1);
        }
    }

    GIVEN("Good case - Custom") {
        fsm::v2::FSM<d20::StateBase> fsm{ctx.create_state<d20::state::SupportedAppProtocol>()};

        message_20::SupportedAppProtocolRequest req;

        auto& ap = req.app_protocol.emplace_back();
        ap.priority = 1;
        ap.protocol_namespace = "urn:iso:std:iso:15118:-20:AABB";
        ap.schema_id = 1;
        ap.version_number_major = 1;
        ap.version_number_minor = 0;

        state_helper.handle_request(req);
        const auto result = fsm.feed(d20::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == d20::StateID::SessionSetup);

            const auto response_message = ctx.get_response<message_20::SupportedAppProtocolResponse>();
            REQUIRE(response_message.has_value());

            const auto& supported_app_res = response_message.value();

            REQUIRE(supported_app_res.response_code ==
                    message_20::SupportedAppProtocolResponse::ResponseCode::OK_SuccessfulNegotiation);
            REQUIRE(supported_app_res.schema_id.value_or(0) == 1);
        }
    }

    GIVEN("Good case - Priority") {
        fsm::v2::FSM<d20::StateBase> fsm{ctx.create_state<d20::state::SupportedAppProtocol>()};

        message_20::SupportedAppProtocolRequest req;
        auto& ap_dc = req.app_protocol.emplace_back();
        ap_dc.priority = 2;
        ap_dc.protocol_namespace = "urn:iso:std:iso:15118:-20:DC";
        ap_dc.schema_id = 1;
        ap_dc.version_number_major = 1;
        ap_dc.version_number_minor = 0;

        auto& ap_custom = req.app_protocol.emplace_back();
        ap_custom.priority = 1;
        ap_custom.protocol_namespace = "urn:iso:std:iso:15118:-20:AC";
        ap_custom.schema_id = 3;
        ap_custom.version_number_major = 1;
        ap_custom.version_number_minor = 0;

        state_helper.handle_request(req);
        const auto result = fsm.feed(d20::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == d20::StateID::SessionSetup);

            const auto response_message = ctx.get_response<message_20::SupportedAppProtocolResponse>();
            REQUIRE(response_message.has_value());

            const auto& supported_app_res = response_message.value();

            REQUIRE(supported_app_res.response_code ==
                    message_20::SupportedAppProtocolResponse::ResponseCode::OK_SuccessfulNegotiation);
            REQUIRE(supported_app_res.schema_id.value_or(0) == 3);
        }
    }

    GIVEN("Bad case - Unknown protocol namespace") {
        fsm::v2::FSM<d20::StateBase> fsm{ctx.create_state<d20::state::SupportedAppProtocol>()};

        message_20::SupportedAppProtocolRequest req;
        auto& ap = req.app_protocol.emplace_back();
        ap.priority = 1;
        ap.protocol_namespace = "Foobar";
        ap.schema_id = 12;
        ap.version_number_major = 2;
        ap.version_number_minor = 11;

        state_helper.handle_request(req);
        const auto result = fsm.feed(d20::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == false);
            REQUIRE(fsm.get_current_state_id() == d20::StateID::SupportedAppProtocol);

            const auto response_message = ctx.get_response<message_20::SupportedAppProtocolResponse>();
            REQUIRE(response_message.has_value());

            const auto& supported_app_res = response_message.value();

            REQUIRE(supported_app_res.response_code ==
                    message_20::SupportedAppProtocolResponse::ResponseCode::Failed_NoNegotiation);
        }
    }

    GIVEN("Bad case - empty app protocol") {
        fsm::v2::FSM<d20::StateBase> fsm{ctx.create_state<d20::state::SupportedAppProtocol>()};

        message_20::SupportedAppProtocolRequest req;
        req.app_protocol.emplace_back();

        state_helper.handle_request(req);
        const auto result = fsm.feed(d20::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == false);
            REQUIRE(fsm.get_current_state_id() == d20::StateID::SupportedAppProtocol);

            const auto response_message = ctx.get_response<message_20::SupportedAppProtocolResponse>();
            REQUIRE(response_message.has_value());

            const auto& supported_app_res = response_message.value();

            REQUIRE(supported_app_res.response_code ==
                    message_20::SupportedAppProtocolResponse::ResponseCode::Failed_NoNegotiation);
        }
    }
}
