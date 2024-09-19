// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/d20/state/schedule_exchange.hpp>

using namespace iso15118;

SCENARIO("Schedule Exchange state handling") {

    using Scheduled_ModeReq = message_20::ScheduleExchangeRequest::Scheduled_SEReqControlMode;
    using Scheduled_ModeRes = message_20::ScheduleExchangeResponse::Scheduled_SEResControlMode;
    using Dynamic_ModeReq = message_20::ScheduleExchangeRequest::Dynamic_SEReqControlMode;
    using Dynamic_ModeRes = message_20::ScheduleExchangeResponse::Dynamic_SEResControlMode;

    GIVEN("Bad case - Unknown session") {

        d20::Session session = d20::Session();

        message_20::ScheduleExchangeRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.control_mode.emplace<Scheduled_ModeReq>();

        message_20::RationalNumber max_power = {0, 0};

        const auto res = d20::state::handle_request(req, d20::Session(), max_power);

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == message_20::ResponseCode::FAILED_UnknownSession);

            REQUIRE(res.processing == message_20::Processing::Finished);

            REQUIRE(std::holds_alternative<Dynamic_ModeRes>(res.control_mode) == true);
            const auto& res_control_mode = std::get<Dynamic_ModeRes>(res.control_mode);
            REQUIRE(std::holds_alternative<std::monostate>(res_control_mode.price_schedule) == true);
        }
    }

    GIVEN("Bad case - False control mode") {
        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            message_20::ServiceCategory::DC, message_20::DcConnector::Extended, message_20::ControlMode::Scheduled,
            message_20::MobilityNeedsMode::ProvidedByEvcc, message_20::Pricing::NoPricing);

        auto session = d20::Session(service_parameters);

        message_20::ScheduleExchangeRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.control_mode.emplace<Dynamic_ModeReq>();

        message_20::RationalNumber max_power = {0, 0};

        const auto res = d20::state::handle_request(req, session, max_power);

        THEN("ResponseCode: FAILED, mandatory fields should be set") {
            REQUIRE(res.response_code == message_20::ResponseCode::FAILED);

            REQUIRE(res.processing == message_20::Processing::Finished);

            REQUIRE(std::holds_alternative<Dynamic_ModeRes>(res.control_mode) == true);
            const auto& res_control_mode = std::get<Dynamic_ModeRes>(res.control_mode);
            REQUIRE(std::holds_alternative<std::monostate>(res_control_mode.price_schedule) == true);
        }
    }

    GIVEN("Good case - Scheduled Mode") {
        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            message_20::ServiceCategory::DC, message_20::DcConnector::Extended, message_20::ControlMode::Scheduled,
            message_20::MobilityNeedsMode::ProvidedByEvcc, message_20::Pricing::NoPricing);

        auto session = d20::Session(service_parameters);

        message_20::ScheduleExchangeRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.control_mode.emplace<Scheduled_ModeReq>();

        message_20::RationalNumber max_power = {22, 3};

        const auto res = d20::state::handle_request(req, session, max_power);

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == message_20::ResponseCode::OK);

            REQUIRE(res.processing == message_20::Processing::Finished);

            REQUIRE(std::holds_alternative<Scheduled_ModeRes>(res.control_mode) == true);
            const auto& res_control_mode = std::get<Scheduled_ModeRes>(res.control_mode);

            REQUIRE(res_control_mode.schedule_tuple.size() == 1);
            const auto& schedule_tuple = res_control_mode.schedule_tuple[0];

            REQUIRE(schedule_tuple.charging_schedule.power_schedule.entries[0].power.value == 22);
            REQUIRE(schedule_tuple.charging_schedule.power_schedule.entries[0].power.exponent == 3);
        }
    }

    // GIVEN("Good case - Dynamic Mode") {} // todo(sl): dynamic mode is missing

    // GIVEN("Bad Case - sequence error") {} // todo(sl): not here

    // GIVEN("Performance Timeout") {} // todo(sl): not here

    // GIVEN("Sequence Timeout") {} // todo(sl): not here
}
