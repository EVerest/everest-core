// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/d20/state/dc_charge_parameter_discovery.hpp>

using namespace iso15118;

using DC_ModeReq = message_20::DC_ChargeParameterDiscoveryRequest::DC_CPDReqEnergyTransferMode;
using BPT_DC_ModeReq = message_20::DC_ChargeParameterDiscoveryRequest::BPT_DC_CPDReqEnergyTransferMode;

using DC_ModeRes = message_20::DC_ChargeParameterDiscoveryResponse::DC_CPDResEnergyTransferMode;
using BPT_DC_ModeRes = message_20::DC_ChargeParameterDiscoveryResponse::BPT_DC_CPDResEnergyTransferMode;

SCENARIO("DC charge parameter discovery state handling") {
    GIVEN("Bad Case - Unknown session") {

        d20::Session session = d20::Session();

        message_20::DC_ChargeParameterDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_out = req.transfer_mode.emplace<DC_ModeReq>();
        req_out.max_charge_power = {50, 3};
        req_out.min_charge_power = {0, 0};
        req_out.max_charge_current = {125, 0};
        req_out.min_charge_current = {0, 0};
        req_out.max_voltage = {400, 0};
        req_out.min_voltage = {0, 0};

        const auto res = d20::state::handle_request(req, d20::Session(), d20::SessionConfig());

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == message_20::ResponseCode::FAILED_UnknownSession);

            REQUIRE(std::holds_alternative<DC_ModeRes>(res.transfer_mode));
            const auto& transfer_mode = std::get<DC_ModeRes>(res.transfer_mode);
            REQUIRE(transfer_mode.max_charge_power.value == 0);
            REQUIRE(transfer_mode.max_charge_power.exponent == 0);
            REQUIRE(transfer_mode.min_charge_power.value == 0);
            REQUIRE(transfer_mode.min_charge_power.exponent == 0);
            REQUIRE(transfer_mode.max_charge_current.value == 0);
            REQUIRE(transfer_mode.max_charge_current.exponent == 0);
            REQUIRE(transfer_mode.min_charge_current.value == 0);
            REQUIRE(transfer_mode.min_charge_current.exponent == 0);
            REQUIRE(transfer_mode.max_voltage.value == 0);
            REQUIRE(transfer_mode.max_voltage.exponent == 0);
            REQUIRE(transfer_mode.min_voltage.value == 0);
            REQUIRE(transfer_mode.min_voltage.exponent == 0);
            REQUIRE(transfer_mode.power_ramp_limit.has_value() == false);
        }
    }

    GIVEN("Bad Case: e.g. dc transfer mod instead of dc_bpt transfer mod - FAILED_WrongChargeParameter") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            message_20::ServiceCategory::DC_BPT, message_20::DcConnector::Extended, message_20::ControlMode::Scheduled,
            message_20::MobilityNeedsMode::ProvidedByEvcc, message_20::Pricing::NoPricing,
            message_20::BptChannel::Unified, message_20::GeneratorMode::GridFollowing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeParameterDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_out = req.transfer_mode.emplace<DC_ModeReq>();
        req_out.max_charge_power = {50, 3};
        req_out.min_charge_power = {0, 0};
        req_out.max_charge_current = {125, 0};
        req_out.min_charge_current = {0, 0};
        req_out.max_voltage = {400, 0};
        req_out.min_voltage = {0, 0};

        const auto res = d20::state::handle_request(req, session, d20::SessionConfig());

        THEN("ResponseCode: FAILED_WrongChargeParameter, mandatory fields should be set") {
            REQUIRE(res.response_code == message_20::ResponseCode::FAILED_WrongChargeParameter);

            REQUIRE(std::holds_alternative<DC_ModeRes>(res.transfer_mode));
            const auto& transfer_mode = std::get<DC_ModeRes>(res.transfer_mode);
            REQUIRE(transfer_mode.max_charge_power.value == 0);
            REQUIRE(transfer_mode.max_charge_power.exponent == 0);
            REQUIRE(transfer_mode.min_charge_power.value == 0);
            REQUIRE(transfer_mode.min_charge_power.exponent == 0);
            REQUIRE(transfer_mode.max_charge_current.value == 0);
            REQUIRE(transfer_mode.max_charge_current.exponent == 0);
            REQUIRE(transfer_mode.min_charge_current.value == 0);
            REQUIRE(transfer_mode.min_charge_current.exponent == 0);
            REQUIRE(transfer_mode.max_voltage.value == 0);
            REQUIRE(transfer_mode.max_voltage.exponent == 0);
            REQUIRE(transfer_mode.min_voltage.value == 0);
            REQUIRE(transfer_mode.min_voltage.exponent == 0);
            REQUIRE(transfer_mode.power_ramp_limit.has_value() == false);
        }
    }

    GIVEN("Bad Case: e.g. DC_BPT transfer mod instead of dc transfer mod - FAILED_WrongChargeParameter") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            message_20::ServiceCategory::DC, message_20::DcConnector::Extended, message_20::ControlMode::Scheduled,
            message_20::MobilityNeedsMode::ProvidedByEvcc, message_20::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeParameterDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_out = req.transfer_mode.emplace<BPT_DC_ModeReq>();
        req_out.max_charge_power = {50, 3};
        req_out.min_charge_power = {0, 0};
        req_out.max_charge_current = {125, 0};
        req_out.min_charge_current = {0, 0};
        req_out.max_voltage = {400, 0};
        req_out.min_voltage = {0, 0};
        req_out.max_discharge_power = {11, 3};
        req_out.min_discharge_power = {0, 0};
        req_out.max_discharge_current = {25, 0};
        req_out.min_discharge_current = {0, 0};

        const auto res = d20::state::handle_request(req, session, d20::SessionConfig());

        THEN("ResponseCode: FAILED_WrongChargeParameter, mandatory fields should be set") {
            REQUIRE(res.response_code == message_20::ResponseCode::FAILED_WrongChargeParameter);

            REQUIRE(std::holds_alternative<DC_ModeRes>(res.transfer_mode));
            const auto& transfer_mode = std::get<DC_ModeRes>(res.transfer_mode);
            REQUIRE(transfer_mode.max_charge_power.value == 0);
            REQUIRE(transfer_mode.max_charge_power.exponent == 0);
            REQUIRE(transfer_mode.min_charge_power.value == 0);
            REQUIRE(transfer_mode.min_charge_power.exponent == 0);
            REQUIRE(transfer_mode.max_charge_current.value == 0);
            REQUIRE(transfer_mode.max_charge_current.exponent == 0);
            REQUIRE(transfer_mode.min_charge_current.value == 0);
            REQUIRE(transfer_mode.min_charge_current.exponent == 0);
            REQUIRE(transfer_mode.max_voltage.value == 0);
            REQUIRE(transfer_mode.max_voltage.exponent == 0);
            REQUIRE(transfer_mode.min_voltage.value == 0);
            REQUIRE(transfer_mode.min_voltage.exponent == 0);
            REQUIRE(transfer_mode.power_ramp_limit.has_value() == false);
        }
    }

    GIVEN("Good Case: DC") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            message_20::ServiceCategory::DC, message_20::DcConnector::Extended, message_20::ControlMode::Scheduled,
            message_20::MobilityNeedsMode::ProvidedByEvcc, message_20::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);
        d20::SessionConfig config = d20::SessionConfig();
        DC_ModeRes evse_dc_parameter = {
            {22, 3},  // max_charge_power
            {0, 0},   // min_charge_power
            {25, 0},  // max_charge_current
            {0, 0},   // min_charge_current
            {900, 0}, // max_voltage
            {0, 0},   // min_voltage
        };
        message_20::RationalNumber power_ramp_limit = {20, 0};
        evse_dc_parameter.power_ramp_limit.emplace<>(power_ramp_limit);
        config.evse_dc_parameter = evse_dc_parameter;

        message_20::DC_ChargeParameterDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_out = req.transfer_mode.emplace<DC_ModeReq>();
        req_out.max_charge_power = {50, 3};
        req_out.min_charge_power = {0, 0};
        req_out.max_charge_current = {125, 0};
        req_out.min_charge_current = {0, 0};
        req_out.max_voltage = {400, 0};
        req_out.min_voltage = {0, 0};

        const auto res = d20::state::handle_request(req, session, config);

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == message_20::ResponseCode::OK);

            REQUIRE(std::holds_alternative<DC_ModeRes>(res.transfer_mode));
            const auto& transfer_mode = std::get<DC_ModeRes>(res.transfer_mode);
            REQUIRE(transfer_mode.max_charge_power.value == 22);
            REQUIRE(transfer_mode.max_charge_power.exponent == 3);
            REQUIRE(transfer_mode.min_charge_power.value == 0);
            REQUIRE(transfer_mode.min_charge_power.exponent == 0);
            REQUIRE(transfer_mode.max_charge_current.value == 25);
            REQUIRE(transfer_mode.max_charge_current.exponent == 0);
            REQUIRE(transfer_mode.min_charge_current.value == 0);
            REQUIRE(transfer_mode.min_charge_current.exponent == 0);
            REQUIRE(transfer_mode.max_voltage.value == 900);
            REQUIRE(transfer_mode.max_voltage.exponent == 0);
            REQUIRE(transfer_mode.min_voltage.value == 0);
            REQUIRE(transfer_mode.min_voltage.exponent == 0);
            REQUIRE(transfer_mode.power_ramp_limit.has_value() == true);
            REQUIRE(transfer_mode.power_ramp_limit.value().value == 20);
            REQUIRE(transfer_mode.power_ramp_limit.value().exponent == 0);
        }
    }

    GIVEN("Good Case: DC_BPT") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            message_20::ServiceCategory::DC_BPT, message_20::DcConnector::Extended, message_20::ControlMode::Scheduled,
            message_20::MobilityNeedsMode::ProvidedByEvcc, message_20::Pricing::NoPricing,
            message_20::BptChannel::Unified, message_20::GeneratorMode::GridFollowing);

        d20::Session session = d20::Session(service_parameters);
        d20::SessionConfig config = d20::SessionConfig();

        BPT_DC_ModeRes evse_dc_bpt_parameter = {
            {
                {22, 3},  // max_charge_power
                {0, 0},   // min_charge_power
                {25, 0},  // max_charge_current
                {0, 0},   // min_charge_current
                {900, 0}, // max_voltage
                {0, 0},   // min_voltage
            },
            {11, 3},      // max_discharge_power
            {0, 0},       // min_discharge_power
            {25, 0},      // max_discharge_current
            {0, 0},       // min_discharge_current
        };
        config.evse_dc_bpt_parameter = evse_dc_bpt_parameter;

        message_20::DC_ChargeParameterDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_out = req.transfer_mode.emplace<BPT_DC_ModeReq>();
        req_out.max_charge_power = {50, 3};
        req_out.min_charge_power = {0, 0};
        req_out.max_charge_current = {125, 0};
        req_out.min_charge_current = {0, 0};
        req_out.max_voltage = {400, 0};
        req_out.min_voltage = {0, 0};
        req_out.max_discharge_power = {11, 3};
        req_out.min_discharge_power = {0, 0};
        req_out.max_discharge_current = {25, 0};
        req_out.min_discharge_current = {0, 0};

        const auto res = d20::state::handle_request(req, session, config);

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == message_20::ResponseCode::OK);

            REQUIRE(std::holds_alternative<BPT_DC_ModeRes>(res.transfer_mode));
            const auto& transfer_mode = std::get<BPT_DC_ModeRes>(res.transfer_mode);
            REQUIRE(transfer_mode.max_charge_power.value == 22);
            REQUIRE(transfer_mode.max_charge_power.exponent == 3);
            REQUIRE(transfer_mode.min_charge_power.value == 0);
            REQUIRE(transfer_mode.min_charge_power.exponent == 0);
            REQUIRE(transfer_mode.max_charge_current.value == 25);
            REQUIRE(transfer_mode.max_charge_current.exponent == 0);
            REQUIRE(transfer_mode.min_charge_current.value == 0);
            REQUIRE(transfer_mode.min_charge_current.exponent == 0);
            REQUIRE(transfer_mode.max_voltage.value == 900);
            REQUIRE(transfer_mode.max_voltage.exponent == 0);
            REQUIRE(transfer_mode.min_voltage.value == 0);
            REQUIRE(transfer_mode.min_voltage.exponent == 0);
            REQUIRE(transfer_mode.power_ramp_limit.has_value() == false);
            REQUIRE(transfer_mode.max_discharge_power.value == 11);
            REQUIRE(transfer_mode.max_discharge_power.exponent == 3);
            REQUIRE(transfer_mode.min_discharge_power.value == 0);
            REQUIRE(transfer_mode.min_discharge_power.exponent == 0);
            REQUIRE(transfer_mode.max_discharge_current.value == 25);
            REQUIRE(transfer_mode.max_discharge_current.exponent == 0);
            REQUIRE(transfer_mode.min_discharge_current.value == 0);
            REQUIRE(transfer_mode.min_discharge_current.exponent == 0);
        }
    }

    GIVEN("Bad Case: EV Parameter does not fit in evse parameters - FAILED_WrongChargeParameter") {
    }

    // GIVEN("Bad Case - sequence error") {} // todo(sl): not here

    // GIVEN("Bad Case - Performance Timeout") {} // todo(sl): not here

    // GIVEN("Bad Case - Sequence Timeout") {} // todo(sl): not here
}
