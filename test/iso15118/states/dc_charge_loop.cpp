// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/d20/state/dc_charge_loop.hpp>

#include <iso15118/d20/config.hpp>

#include <iostream>

using namespace iso15118;

namespace dt = message_20::datatypes;

using Scheduled_DC_Req = message_20::datatypes::Scheduled_DC_CLReqControlMode;
using Scheduled_BPT_DC_Req = message_20::datatypes::BPT_Scheduled_DC_CLReqControlMode;
using Dynamic_DC_Req = message_20::datatypes::Dynamic_DC_CLReqControlMode;
using Dynamic_BPT_DC_Req = message_20::datatypes::BPT_Dynamic_DC_CLReqControlMode;

using Scheduled_DC_Res = message_20::datatypes::Scheduled_DC_CLResControlMode;
using Scheduled_BPT_DC_Res = message_20::datatypes::BPT_Scheduled_DC_CLResControlMode;
using Dynamic_DC_Res = message_20::datatypes::Dynamic_DC_CLResControlMode;
using Dynamic_BPT_DC_Res = message_20::datatypes::BPT_Dynamic_DC_CLResControlMode;

SCENARIO("DC charge loop state handling") {

    const auto evse_id = std::string("everest se");
    const std::vector<dt::ServiceCategory> supported_energy_services = {dt::ServiceCategory::DC,
                                                                        dt::ServiceCategory::DC_BPT};
    const auto cert_install{false};
    const std::vector<dt::Authorization> auth_services = {dt::Authorization::EIM};

    d20::DcTransferLimits dc_limits;
    dc_limits.charge_limits.power.max = {22, 3};
    dc_limits.charge_limits.power.min = {10, 0};
    dc_limits.charge_limits.current.max = {250, 0};
    dc_limits.voltage.max = {900, 0};
    auto& discharge_limits = dc_limits.discharge_limits.emplace<>();
    discharge_limits.power.max = {11, 3};
    discharge_limits.power.min = {10, 0};
    discharge_limits.current.max = {30, 0};

    const std::vector<d20::ControlMobilityNeedsModes> control_mobility_modes = {
        {dt::ControlMode::Scheduled, dt::MobilityNeedsMode::ProvidedByEvcc},
        {dt::ControlMode::Dynamic, dt::MobilityNeedsMode::ProvidedByEvcc},
        {dt::ControlMode::Dynamic, dt::MobilityNeedsMode::ProvidedBySecc}};

    const d20::EvseSetupConfig evse_setup{evse_id,   supported_energy_services, auth_services, cert_install,
                                          dc_limits, control_mobility_modes};

    GIVEN("Bad case - Unknown session") {

        d20::Session session = d20::Session();

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Scheduled_DC_Req>();
        req_control_mode.target_current = {40, 0};
        req_control_mode.target_voltage = {400, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const auto res = d20::state::handle_request(req, d20::Session(), 330, 30, false, false, evse_setup.dc_limits,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_UnknownSession);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 0.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 0.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);
            REQUIRE(std::holds_alternative<Scheduled_DC_Res>(res.control_mode));
        }
    }

    GIVEN("Bad case - false energy mode") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::DC_BPT, dt::DcConnector::Extended, dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing, dt::BptChannel::Unified,
            dt::GeneratorMode::GridFollowing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Scheduled_DC_Req>();
        req_control_mode.target_current = {40, 0};
        req_control_mode.target_voltage = {400, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const auto res = d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: FAILED, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 0.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 0.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);
            REQUIRE(std::holds_alternative<Scheduled_DC_Res>(res.control_mode));
        }
    }

    GIVEN("Bad case - false control mode") {

        d20::SelectedServiceParameters service_parameters =
            d20::SelectedServiceParameters(dt::ServiceCategory::DC, dt::DcConnector::Extended, dt::ControlMode::Dynamic,
                                           dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Scheduled_DC_Req>();
        req_control_mode.target_current = {40, 0};
        req_control_mode.target_voltage = {400, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const auto res = d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: FAILED, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 0.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 0.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);
            REQUIRE(std::holds_alternative<Scheduled_DC_Res>(res.control_mode));
        }
    }

    GIVEN("Good case - DC scheduled mode") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::DC, dt::DcConnector::Extended, dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Scheduled_DC_Req>();
        req_control_mode.target_current = {40, 0};
        req_control_mode.target_voltage = {400, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const auto res = d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 30.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 330.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);

            REQUIRE(std::holds_alternative<Scheduled_DC_Res>(res.control_mode));
            const auto& res_control_mode = std::get<Scheduled_DC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_power.value_or(dt::RationalNumber{0, 0})) ==
                    22000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_charge_power.value_or(dt::RationalNumber{0, 0})) ==
                    10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_current.value_or(dt::RationalNumber{0, 0})) ==
                    250.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_voltage.value_or(dt::RationalNumber{0, 0})) == 900.0f);
        }
    }

    GIVEN("Good case - DC_BPT scheduled mode") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::DC_BPT, dt::DcConnector::Extended, dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Scheduled_BPT_DC_Req>();
        req_control_mode.target_current = {40, 0};
        req_control_mode.target_voltage = {400, 0};
        req_control_mode.max_discharge_power.emplace<dt::RationalNumber>({11, 3});

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const auto res = d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 30.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 330.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);

            REQUIRE(std::holds_alternative<Scheduled_BPT_DC_Res>(res.control_mode));
            const auto& res_control_mode = std::get<Scheduled_BPT_DC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_power.value_or(dt::RationalNumber{0, 0})) ==
                    22000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_charge_power.value_or(dt::RationalNumber{0, 0})) ==
                    10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_current.value_or(dt::RationalNumber{0, 0})) ==
                    250.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_voltage.value_or(dt::RationalNumber{0, 0})) == 900.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_discharge_power.value_or(dt::RationalNumber{0, 0})) ==
                    11000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_discharge_power.value_or(dt::RationalNumber{0, 0})) ==
                    10.0f);
            REQUIRE(dt::from_RationalNumber(
                        res_control_mode.max_discharge_current.value_or(dt::RationalNumber{0, 0})) == 30.0f);
        }
    }

    GIVEN("Good case - DC dynamic mode") {
        d20::SelectedServiceParameters service_parameters =
            d20::SelectedServiceParameters(dt::ServiceCategory::DC, dt::DcConnector::Extended, dt::ControlMode::Dynamic,
                                           dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_DC_Req>();
        req_control_mode.target_energy_request = {68, 3};
        req_control_mode.max_energy_request = {70, 3};
        req_control_mode.min_energy_request = {40, 3};
        req_control_mode.max_charge_power = {30, 3};
        req_control_mode.min_charge_power = {27, 2};
        req_control_mode.max_charge_current = {400, 0};
        req_control_mode.max_voltage = {950, 0};
        req_control_mode.min_voltage = {150, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const auto res = d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 30.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 330.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);

            REQUIRE(std::holds_alternative<Dynamic_DC_Res>(res.control_mode));
            const auto& res_control_mode = std::get<Dynamic_DC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_power) == 22000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_charge_power) == 10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_current) == 250.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_voltage) == 900.0f);
        }
    }

    GIVEN("Good case - DC_BPT dynamic mode") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::DC_BPT, dt::DcConnector::Extended, dt::ControlMode::Dynamic,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_BPT_DC_Req>();
        req_control_mode.target_energy_request = {68, 3};
        req_control_mode.max_energy_request = {70, 3};
        req_control_mode.min_energy_request = {40, 3};
        req_control_mode.max_charge_power = {30, 3};
        req_control_mode.min_charge_power = {27, 2};
        req_control_mode.max_charge_current = {400, 0};
        req_control_mode.max_voltage = {950, 0};
        req_control_mode.min_voltage = {150, 0};
        req_control_mode.max_discharge_power = {11, 3};
        req_control_mode.min_discharge_power = {10, 0};
        req_control_mode.max_discharge_current = {30, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const auto res = d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 30.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 330.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);

            REQUIRE(std::holds_alternative<Dynamic_BPT_DC_Res>(res.control_mode));
            const auto& res_control_mode = std::get<Dynamic_BPT_DC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_power) == 22000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_charge_power) == 10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_current) == 250.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_voltage) == 900.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_discharge_power) == 11000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_discharge_power) == 10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_discharge_current) == 30.0f);
        }
    }

    GIVEN("Good case - DC dynamic mode, mobility_needs_mode = 2") {
        d20::SelectedServiceParameters service_parameters =
            d20::SelectedServiceParameters(dt::ServiceCategory::DC, dt::DcConnector::Extended, dt::ControlMode::Dynamic,
                                           dt::MobilityNeedsMode::ProvidedBySecc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_DC_Req>();
        req_control_mode.target_energy_request = {68, 3};
        req_control_mode.max_energy_request = {70, 3};
        req_control_mode.min_energy_request = {40, 3};
        req_control_mode.max_charge_power = {30, 3};
        req_control_mode.min_charge_power = {27, 2};
        req_control_mode.max_charge_current = {400, 0};
        req_control_mode.max_voltage = {950, 0};
        req_control_mode.min_voltage = {150, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const d20::UpdateDynamicModeParameters dynamic_parameters = {std::time(nullptr) + 60, 95, std::nullopt};

        const auto res =
            d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits, dynamic_parameters);

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 30.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 330.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);

            REQUIRE(std::holds_alternative<Dynamic_DC_Res>(res.control_mode));
            const auto& res_control_mode = std::get<Dynamic_DC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_power) == 22000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_charge_power) == 10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_current) == 250.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_voltage) == 900.0f);

            REQUIRE(res_control_mode.departure_time.value_or(0) >= 59);
            REQUIRE(res_control_mode.target_soc.value_or(0) == 95);
            REQUIRE(res_control_mode.ack_max_delay.value_or(0) == 30);
        }
    }

    GIVEN("Good case - DC_BPT dynamic mode, mobility_needs_mode = 2") {
        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::DC_BPT, dt::DcConnector::Extended, dt::ControlMode::Dynamic,
            dt::MobilityNeedsMode::ProvidedBySecc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_BPT_DC_Req>();
        req_control_mode.target_energy_request = {68, 3};
        req_control_mode.max_energy_request = {70, 3};
        req_control_mode.min_energy_request = {40, 3};
        req_control_mode.max_charge_power = {30, 3};
        req_control_mode.min_charge_power = {27, 2};
        req_control_mode.max_charge_current = {400, 0};
        req_control_mode.max_voltage = {950, 0};
        req_control_mode.min_voltage = {150, 0};
        req_control_mode.max_discharge_power = {11, 3};
        req_control_mode.min_discharge_power = {10, 0};
        req_control_mode.max_discharge_current = {30, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const d20::UpdateDynamicModeParameters dynamic_parameters = {std::time(nullptr) + 40, std::nullopt, 95};

        const auto res =
            d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits, dynamic_parameters);

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 30.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 330.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);

            REQUIRE(std::holds_alternative<Dynamic_BPT_DC_Res>(res.control_mode));
            const auto& res_control_mode = std::get<Dynamic_BPT_DC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_power) == 22000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_charge_power) == 10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_current) == 250.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_voltage) == 900.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_discharge_power) == 11000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_discharge_power) == 10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_discharge_current) == 30.0f);

            REQUIRE(res_control_mode.departure_time.value_or(0) >= 39);
            REQUIRE(res_control_mode.minimum_soc.value_or(0) == 95);
            REQUIRE(res_control_mode.ack_max_delay.value_or(0) == 30);
        }
    }

    GIVEN("Good case - DC dynamic mode & pause from charger") {
        d20::SelectedServiceParameters service_parameters =
            d20::SelectedServiceParameters(dt::ServiceCategory::DC, dt::DcConnector::Extended, dt::ControlMode::Dynamic,
                                           dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_DC_Req>();
        req_control_mode.target_energy_request = {68, 3};
        req_control_mode.max_energy_request = {70, 3};
        req_control_mode.min_energy_request = {40, 3};
        req_control_mode.max_charge_power = {30, 3};
        req_control_mode.min_charge_power = {27, 2};
        req_control_mode.max_charge_current = {400, 0};
        req_control_mode.max_voltage = {950, 0};
        req_control_mode.min_voltage = {150, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const auto res = d20::state::handle_request(req, session, 330, 30, false, true, evse_setup.dc_limits,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 30.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 330.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);

            REQUIRE(std::holds_alternative<Dynamic_DC_Res>(res.control_mode));
            const auto& res_control_mode = std::get<Dynamic_DC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_power) == 22000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_charge_power) == 10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_current) == 250.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_voltage) == 900.0f);

            REQUIRE(res.status.has_value() == true);
            REQUIRE(res.status.value().notification == dt::EvseNotification::Pause);
            REQUIRE(res.status.value().notification_max_delay == 60);
        }
    }

    GIVEN("Good case - MCS scheduled mode") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::MCS, dt::DcConnector::Extended, dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Scheduled_DC_Req>();
        req_control_mode.target_current = {40, 0};
        req_control_mode.target_voltage = {400, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const auto res = d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 30.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 330.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);

            REQUIRE(std::holds_alternative<Scheduled_DC_Res>(res.control_mode));
            const auto& res_control_mode = std::get<Scheduled_DC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_power.value_or(dt::RationalNumber{0, 0})) ==
                    22000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_charge_power.value_or(dt::RationalNumber{0, 0})) ==
                    10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_current.value_or(dt::RationalNumber{0, 0})) ==
                    250.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_voltage.value_or(dt::RationalNumber{0, 0})) == 900.0f);
        }
    }

    GIVEN("Good case - MCS_BPT scheduled mode") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::MCS_BPT, dt::DcConnector::Extended, dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Scheduled_BPT_DC_Req>();
        req_control_mode.target_current = {40, 0};
        req_control_mode.target_voltage = {400, 0};
        req_control_mode.max_discharge_power.emplace<dt::RationalNumber>({11, 3});

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const auto res = d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 30.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 330.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);

            REQUIRE(std::holds_alternative<Scheduled_BPT_DC_Res>(res.control_mode));
            const auto& res_control_mode = std::get<Scheduled_BPT_DC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_power.value_or(dt::RationalNumber{0, 0})) ==
                    22000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_charge_power.value_or(dt::RationalNumber{0, 0})) ==
                    10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_current.value_or(dt::RationalNumber{0, 0})) ==
                    250.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_voltage.value_or(dt::RationalNumber{0, 0})) == 900.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_discharge_power.value_or(dt::RationalNumber{0, 0})) ==
                    11000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_discharge_power.value_or(dt::RationalNumber{0, 0})) ==
                    10.0f);
            REQUIRE(dt::from_RationalNumber(
                        res_control_mode.max_discharge_current.value_or(dt::RationalNumber{0, 0})) == 30.0f);
        }
    }

    GIVEN("Good case - MCS dynamic mode") {
        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::MCS, dt::DcConnector::Extended, dt::ControlMode::Dynamic,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_DC_Req>();
        req_control_mode.target_energy_request = {68, 3};
        req_control_mode.max_energy_request = {70, 3};
        req_control_mode.min_energy_request = {40, 3};
        req_control_mode.max_charge_power = {30, 3};
        req_control_mode.min_charge_power = {27, 2};
        req_control_mode.max_charge_current = {400, 0};
        req_control_mode.max_voltage = {950, 0};
        req_control_mode.min_voltage = {150, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const auto res = d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 30.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 330.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);

            REQUIRE(std::holds_alternative<Dynamic_DC_Res>(res.control_mode));
            const auto& res_control_mode = std::get<Dynamic_DC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_power) == 22000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_charge_power) == 10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_current) == 250.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_voltage) == 900.0f);
        }
    }

    GIVEN("Good case - MCS_BPT dynamic mode") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::MCS_BPT, dt::DcConnector::Extended, dt::ControlMode::Dynamic,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_BPT_DC_Req>();
        req_control_mode.target_energy_request = {68, 3};
        req_control_mode.max_energy_request = {70, 3};
        req_control_mode.min_energy_request = {40, 3};
        req_control_mode.max_charge_power = {30, 3};
        req_control_mode.min_charge_power = {27, 2};
        req_control_mode.max_charge_current = {400, 0};
        req_control_mode.max_voltage = {950, 0};
        req_control_mode.min_voltage = {150, 0};
        req_control_mode.max_discharge_power = {11, 3};
        req_control_mode.min_discharge_power = {10, 0};
        req_control_mode.max_discharge_current = {30, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const auto res = d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 30.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 330.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);

            REQUIRE(std::holds_alternative<Dynamic_BPT_DC_Res>(res.control_mode));
            const auto& res_control_mode = std::get<Dynamic_BPT_DC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_power) == 22000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_charge_power) == 10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_current) == 250.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_voltage) == 900.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_discharge_power) == 11000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_discharge_power) == 10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_discharge_current) == 30.0f);
        }
    }

    GIVEN("Good case - MCS dynamic mode, mobility_needs_mode = 2") {
        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::MCS, dt::DcConnector::Extended, dt::ControlMode::Dynamic,
            dt::MobilityNeedsMode::ProvidedBySecc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_DC_Req>();
        req_control_mode.target_energy_request = {68, 3};
        req_control_mode.max_energy_request = {70, 3};
        req_control_mode.min_energy_request = {40, 3};
        req_control_mode.max_charge_power = {30, 3};
        req_control_mode.min_charge_power = {27, 2};
        req_control_mode.max_charge_current = {400, 0};
        req_control_mode.max_voltage = {950, 0};
        req_control_mode.min_voltage = {150, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const d20::UpdateDynamicModeParameters dynamic_parameters = {std::time(nullptr) + 60, 95, std::nullopt};

        const auto res =
            d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits, dynamic_parameters);

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 30.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 330.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);

            REQUIRE(std::holds_alternative<Dynamic_DC_Res>(res.control_mode));
            const auto& res_control_mode = std::get<Dynamic_DC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_power) == 22000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_charge_power) == 10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_current) == 250.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_voltage) == 900.0f);

            REQUIRE(res_control_mode.departure_time.value_or(0) >= 59);
            REQUIRE(res_control_mode.target_soc.value_or(0) == 95);
            REQUIRE(res_control_mode.ack_max_delay.value_or(0) == 30);
        }
    }

    GIVEN("Good case - MCS_BPT dynamic mode, mobility_needs_mode = 2") {
        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::MCS_BPT, dt::DcConnector::Extended, dt::ControlMode::Dynamic,
            dt::MobilityNeedsMode::ProvidedBySecc, dt::Pricing::NoPricing);

        d20::Session session = d20::Session(service_parameters);

        message_20::DC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_BPT_DC_Req>();
        req_control_mode.target_energy_request = {68, 3};
        req_control_mode.max_energy_request = {70, 3};
        req_control_mode.min_energy_request = {40, 3};
        req_control_mode.max_charge_power = {30, 3};
        req_control_mode.min_charge_power = {27, 2};
        req_control_mode.max_charge_current = {400, 0};
        req_control_mode.max_voltage = {950, 0};
        req_control_mode.min_voltage = {150, 0};
        req_control_mode.max_discharge_power = {11, 3};
        req_control_mode.min_discharge_power = {10, 0};
        req_control_mode.max_discharge_current = {30, 0};

        req.meter_info_requested = false;
        req.present_voltage = {330, 0};

        const d20::UpdateDynamicModeParameters dynamic_parameters = {std::time(nullptr) + 40, std::nullopt, 95};

        const auto res =
            d20::state::handle_request(req, session, 330, 30, false, false, evse_setup.dc_limits, dynamic_parameters);

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(dt::from_RationalNumber(res.present_current) == 30.0f);
            REQUIRE(dt::from_RationalNumber(res.present_voltage) == 330.0f);
            REQUIRE(res.current_limit_achieved == false);
            REQUIRE(res.power_limit_achieved == false);
            REQUIRE(res.voltage_limit_achieved == false);

            REQUIRE(std::holds_alternative<Dynamic_BPT_DC_Res>(res.control_mode));
            const auto& res_control_mode = std::get<Dynamic_BPT_DC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_power) == 22000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_charge_power) == 10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_charge_current) == 250.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_voltage) == 900.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_discharge_power) == 11000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.min_discharge_power) == 10.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.max_discharge_current) == 30.0f);

            REQUIRE(res_control_mode.departure_time.value_or(0) >= 39);
            REQUIRE(res_control_mode.minimum_soc.value_or(0) == 95);
            REQUIRE(res_control_mode.ack_max_delay.value_or(0) == 30);
        }
    }

    // Note(sl): Only in scheduled mode and if a powertolerance was sent from the secc
    // TODO(sl): Adding test
    // GIVEN("Warning case - Warning_EVPowerProfileViolation [V2G20-1864]") {}
    // GIVEN("Bad case - Failed_EVPowerProfileViolation [V2G20-1864]") {}

    // TODO(sl): Adding test if ScheduleRenegotion is supported
    // GIVEN("Warning case - Warning_ScheduleRenegotiationFailed") {}
    // GIVEN("Bad case - Failed_ScheduleRenegotiationFailed") {}

    // Note(sl): Not yet
    // GIVEN("Bad case - Failed_AssociationError (ACDP only)") {}

    // GIVEN("Bad Case - sequence error") {} // TODO(sl): not here

    // GIVEN("Bad Case - Performance Timeout") {} // TODO(sl): not here

    // GIVEN("Bad Case - Sequence Timeout") {} // TODO(sl): not here
}