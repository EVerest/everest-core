// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include "helper.hpp"

#include <iso15118/d20/state/service_detail.hpp>
#include <iso15118/d20/state/service_selection.hpp>

#include <iso15118/message/service_detail.hpp>

#include <iso15118/detail/helper.hpp>

using namespace iso15118;

SCENARIO("ISO15118-20 session setup state transitions") {

    namespace dt = message_20::datatypes;

    const auto evse_id = std::string("everest se");
    const std::vector<dt::ServiceCategory> supported_energy_services = {dt::ServiceCategory::DC};
    const auto cert_install{false};
    const std::vector<uint16_t> vas_services{}; // TODO(SL): Add Custom  service
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

    session::feedback::Callbacks callbacks{};

    callbacks.get_vas_parameters = [](uint16_t id) {
        auto service_parameter_list = dt::ServiceParameterList{};

        if (id == 4599) {
            auto& parameter_set = service_parameter_list.emplace_back();
            parameter_set.id = 0;
            parameter_set.parameter.push_back({"Service1", 40});
            parameter_set.parameter.push_back({"Service2", "house"});
        } else if (id == message_20::to_underlying_value(dt::ServiceCategory::ParkingStatus)) {
            auto& parameter_set = service_parameter_list.emplace_back();
            parameter_set.id = 0;
            parameter_set.parameter.push_back({"IntendedService", 1});
            parameter_set.parameter.push_back({"ParkingStatusType", 4});
        } else if (id == message_20::to_underlying_value(dt::ServiceCategory::Internet)) {
            auto& parameter_set = service_parameter_list.emplace_back();
            parameter_set.id = 3;
            parameter_set.parameter.push_back({"Protocol", "http"});
            parameter_set.parameter.push_back({"Port", 80});
        }

        return std::make_optional(service_parameter_list);
    };

    auto state_helper = FsmStateHelper(d20::SessionConfig(evse_setup), pause_ctx, callbacks);
    auto ctx = state_helper.get_context();
    ctx.session = d20::Session();

    GIVEN("Good case - Ev requests parameter from custom vas") {
        fsm::v2::FSM<d20::StateBase> fsm{ctx.create_state<d20::state::ServiceDetail>()};

        const auto header_req = message_20::Header{ctx.session.get_id(), 1691411798};
        const auto req = message_20::ServiceDetailRequest{header_req, 4599};

        state_helper.handle_request(req);

        ctx.session.offered_services.vas_services = {4599};

        const auto result = fsm.feed(d20::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == d20::StateID::ServiceSelection);

            const auto response_message = ctx.get_response<message_20::ServiceDetailResponse>();
            REQUIRE(response_message.has_value());

            const auto& service_detail_res = response_message.value();

            REQUIRE(service_detail_res.response_code == dt::ResponseCode::OK);
            REQUIRE(service_detail_res.service == 4599);
            REQUIRE(service_detail_res.service_parameter_list.size() == 1);
            auto& parameters = service_detail_res.service_parameter_list[0];
            REQUIRE(parameters.id == 0);
            REQUIRE(parameters.parameter.size() == 2);

            REQUIRE(parameters.parameter[0].name == "Service1");
            REQUIRE(std::holds_alternative<int32_t>(parameters.parameter[0].value));
            REQUIRE(std::get<int32_t>(parameters.parameter[0].value) == 40);

            REQUIRE(parameters.parameter[1].name == "Service2");
            REQUIRE(std::holds_alternative<std::string>(parameters.parameter[1].value));
            REQUIRE(std::get<std::string>(parameters.parameter[1].value) == "house");
        }
    }

    GIVEN("Good case - Ev requests parameter from parking vas") {
        fsm::v2::FSM<d20::StateBase> fsm{ctx.create_state<d20::state::ServiceDetail>()};

        const auto header_req = message_20::Header{ctx.session.get_id(), 1691411798};
        const auto req = message_20::ServiceDetailRequest{
            header_req, message_20::to_underlying_value(dt::ServiceCategory::ParkingStatus)};

        state_helper.handle_request(req);

        ctx.session.offered_services.vas_services = {
            message_20::to_underlying_value(dt::ServiceCategory::ParkingStatus)};

        const auto result = fsm.feed(d20::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == d20::StateID::ServiceSelection);

            const auto response_message = ctx.get_response<message_20::ServiceDetailResponse>();
            REQUIRE(response_message.has_value());

            const auto& service_detail_res = response_message.value();

            REQUIRE(service_detail_res.response_code == dt::ResponseCode::OK);
            REQUIRE(service_detail_res.service == message_20::to_underlying_value(dt::ServiceCategory::ParkingStatus));
            REQUIRE(service_detail_res.service_parameter_list.size() == 1);
            auto& parameters = service_detail_res.service_parameter_list[0];
            REQUIRE(parameters.id == 0);
            REQUIRE(parameters.parameter.size() == 2);

            REQUIRE(parameters.parameter[0].name == "IntendedService");
            REQUIRE(std::holds_alternative<int32_t>(parameters.parameter[0].value));
            REQUIRE(std::get<int32_t>(parameters.parameter[0].value) == 1);

            REQUIRE(parameters.parameter[1].name == "ParkingStatusType");
            REQUIRE(std::holds_alternative<int32_t>(parameters.parameter[1].value));
            REQUIRE(std::get<int32_t>(parameters.parameter[1].value) == 4);
        }
    }

    GIVEN("Good case - Ev requests parameter from internet vas") {
        fsm::v2::FSM<d20::StateBase> fsm{ctx.create_state<d20::state::ServiceDetail>()};

        const auto header_req = message_20::Header{ctx.session.get_id(), 1691411798};
        const auto req = message_20::ServiceDetailRequest{
            header_req, message_20::to_underlying_value(dt::ServiceCategory::Internet)};

        state_helper.handle_request(req);

        ctx.session.offered_services.vas_services = {message_20::to_underlying_value(dt::ServiceCategory::Internet)};

        const auto result = fsm.feed(d20::Event::V2GTP_MESSAGE);

        THEN("Check state transition") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == d20::StateID::ServiceSelection);

            const auto response_message = ctx.get_response<message_20::ServiceDetailResponse>();
            REQUIRE(response_message.has_value());

            const auto& service_detail_res = response_message.value();

            REQUIRE(service_detail_res.response_code == dt::ResponseCode::OK);
            REQUIRE(service_detail_res.service == message_20::to_underlying_value(dt::ServiceCategory::Internet));
            REQUIRE(service_detail_res.service_parameter_list.size() == 1);
            auto& parameters = service_detail_res.service_parameter_list[0];
            REQUIRE(parameters.id == 3);
            REQUIRE(parameters.parameter.size() == 2);

            REQUIRE(parameters.parameter[0].name == "Protocol");
            REQUIRE(std::holds_alternative<std::string>(parameters.parameter[0].value));
            REQUIRE(std::get<std::string>(parameters.parameter[0].value) == "http");

            REQUIRE(parameters.parameter[1].name == "Port");
            REQUIRE(std::holds_alternative<int32_t>(parameters.parameter[1].value));
            REQUIRE(std::get<int32_t>(parameters.parameter[1].value) == 80);
        }
    }
}
