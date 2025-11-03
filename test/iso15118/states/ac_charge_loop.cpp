// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/d20/state/ac_charge_loop.hpp>

#include <iso15118/d20/config.hpp>

using namespace iso15118;

namespace dt = message_20::datatypes;

using Scheduled_AC_Req = dt::Scheduled_AC_CLReqControlMode;
using Scheduled_BPT_AC_Req = dt::BPT_Scheduled_AC_CLReqControlMode;
using Dynamic_AC_Req = dt::Dynamic_AC_CLReqControlMode;
using Dynamic_BPT_AC_Req = dt::BPT_Dynamic_AC_CLReqControlMode;

using Scheduled_AC_Res = dt::Scheduled_AC_CLResControlMode;
using Scheduled_BPT_AC_Res = dt::BPT_Scheduled_AC_CLResControlMode;
using Dynamic_AC_Res = dt::Dynamic_AC_CLResControlMode;
using Dynamic_BPT_AC_Res = dt::BPT_Dynamic_AC_CLResControlMode;

SCENARIO("AC charge loop state handling") {

    const auto evse_id = std::string("everest se");
    const std::vector<dt::ServiceCategory> supported_energy_services = {dt::ServiceCategory::AC,
                                                                        dt::ServiceCategory::AC_BPT};
    const auto cert_install{false};
    const std::vector<dt::Authorization> auth_services = {dt::Authorization::EIM};
    const std::vector<uint16_t> vas_services{};

    d20::DcTransferLimits dc_limits;
    d20::AcTransferLimits ac_limits;
    ac_limits.charge_power = {{22, 3}, {10, 0}};
    ac_limits.nominal_frequency = {50, 0};

    auto& discharge_limits = ac_limits.discharge_power.emplace();
    discharge_limits.max = {11, 3};
    discharge_limits.min = {10, 0};

    const std::vector<d20::ControlMobilityNeedsModes> control_mobility_modes = {
        {dt::ControlMode::Scheduled, dt::MobilityNeedsMode::ProvidedByEvcc},
        {dt::ControlMode::Dynamic, dt::MobilityNeedsMode::ProvidedByEvcc},
        {dt::ControlMode::Dynamic, dt::MobilityNeedsMode::ProvidedBySecc}};

    const d20::EvseSetupConfig evse_setup{
        evse_id,   supported_energy_services, auth_services, vas_services, cert_install, dc_limits,
        ac_limits, control_mobility_modes};

    GIVEN("Bad case - Unknown session") {
        d20::Session session = d20::Session();
        message_20::AC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Scheduled_AC_Req>();
        req_control_mode.present_active_power = {0, 0};

        req.meter_info_requested = false;

        const auto res = d20::state::handle_request(req, d20::Session(), false, false, 50, d20::AcTargetPower(),
                                                    d20::AcPresentPower(), d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_UnknownSession);
            REQUIRE(res.status.has_value() == false);
            REQUIRE(res.meter_info.has_value() == false);
            REQUIRE(res.receipt.has_value() == false);
            REQUIRE(res.target_frequency.has_value() == false);
            REQUIRE(std::holds_alternative<Scheduled_AC_Res>(res.control_mode));
        }
    }

    GIVEN("Bad case - false energy mode") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::AC_BPT, dt::AcConnector::ThreePhase, dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing, dt::BptChannel::Unified,
            dt::GeneratorMode::GridFollowing, 230, dt::GridCodeIslandingDetectionMethod::Passive);

        d20::Session session = d20::Session(service_parameters);
        message_20::AC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Scheduled_AC_Req>();
        req_control_mode.present_active_power = {0, 0};

        req.meter_info_requested = false;

        const auto res = d20::state::handle_request(req, d20::Session(), false, false, 50, d20::AcTargetPower(),
                                                    d20::AcPresentPower(), d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_UnknownSession);
            REQUIRE(res.status.has_value() == false);
            REQUIRE(res.meter_info.has_value() == false);
            REQUIRE(res.receipt.has_value() == false);
            REQUIRE(res.target_frequency.has_value() == false);
            REQUIRE(std::holds_alternative<Scheduled_AC_Res>(res.control_mode));
        }
    }

    GIVEN("Bad case - false control mode") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::AC, dt::AcConnector::ThreePhase, dt::ControlMode::Dynamic,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing, 230);

        d20::Session session = d20::Session(service_parameters);
        message_20::AC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Scheduled_AC_Req>();
        req_control_mode.present_active_power = {0, 0};

        req.meter_info_requested = false;

        const auto res = d20::state::handle_request(req, d20::Session(), false, false, 50, d20::AcTargetPower(),
                                                    d20::AcPresentPower(), d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_UnknownSession);
            REQUIRE(res.status.has_value() == false);
            REQUIRE(res.meter_info.has_value() == false);
            REQUIRE(res.receipt.has_value() == false);
            REQUIRE(res.target_frequency.has_value() == false);
            REQUIRE(std::holds_alternative<Scheduled_AC_Res>(res.control_mode));
        }
    }

    GIVEN("Good case - AC scheduled mode") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::AC, dt::AcConnector::ThreePhase, dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing, 230);

        d20::Session session = d20::Session(service_parameters);
        message_20::AC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Scheduled_AC_Req>();
        req_control_mode.present_active_power = {11, 3};

        req.meter_info_requested = false;

        const auto ac_target_power = d20::AcTargetPower{};
        auto ac_present_power = d20::AcPresentPower{};
        ac_present_power.present_active_power = {11, 3};

        const auto res = d20::state::handle_request(req, session, false, false, 50, ac_target_power, ac_present_power,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.status.has_value() == false);
            REQUIRE(res.meter_info.has_value() == false);
            REQUIRE(res.receipt.has_value() == false);
            REQUIRE(dt::from_RationalNumber(res.target_frequency.value_or(dt::RationalNumber{0, 0})) == 50.0f);
            REQUIRE(std::holds_alternative<Scheduled_AC_Res>(res.control_mode));

            const auto& res_control_mode = std::get<Scheduled_AC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.present_active_power.value_or(dt::RationalNumber{0, 0})) ==
                    11000.0f);
        }
    }

    GIVEN("Good case - AC_BPT scheduled mode") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::AC_BPT, dt::AcConnector::ThreePhase, dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing, dt::BptChannel::Unified,
            dt::GeneratorMode::GridFollowing, 230, dt::GridCodeIslandingDetectionMethod::Passive);

        d20::Session session = d20::Session(service_parameters);
        message_20::AC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Scheduled_BPT_AC_Req>();
        req_control_mode.present_active_power = {0, 0};

        req.meter_info_requested = false;

        const auto ac_target_power = d20::AcTargetPower{};
        auto ac_present_power = d20::AcPresentPower{};
        ac_present_power.present_active_power = {11, 3};

        const auto res = d20::state::handle_request(req, session, false, false, 50, ac_target_power, ac_present_power,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.status.has_value() == false);
            REQUIRE(res.meter_info.has_value() == false);
            REQUIRE(res.receipt.has_value() == false);
            REQUIRE(dt::from_RationalNumber(res.target_frequency.value_or(dt::RationalNumber{0, 0})) == 50.0f);
            REQUIRE(std::holds_alternative<Scheduled_BPT_AC_Res>(res.control_mode));

            const auto& res_control_mode = std::get<Scheduled_BPT_AC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.present_active_power.value_or(dt::RationalNumber{0, 0})) ==
                    11000.0f);
        }
    }

    GIVEN("Good case - AC dynamic mode") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::AC, dt::AcConnector::ThreePhase, dt::ControlMode::Dynamic,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing, 230);

        d20::Session session = d20::Session(service_parameters);
        message_20::AC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_AC_Req>();
        req_control_mode.present_active_power = {11, 3};
        req_control_mode.max_charge_power = {11, 3};
        req_control_mode.min_charge_power = {4, 0};
        req_control_mode.present_reactive_power = {10, 0};

        req.meter_info_requested = false;

        auto ac_target_power = d20::AcTargetPower{};
        ac_target_power.target_active_power = {11, 3};
        auto ac_present_power = d20::AcPresentPower{};
        ac_present_power.present_active_power = {11, 3};

        const auto res = d20::state::handle_request(req, session, false, false, 50, ac_target_power, ac_present_power,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.status.has_value() == false);
            REQUIRE(res.meter_info.has_value() == false);
            REQUIRE(res.receipt.has_value() == false);
            REQUIRE(dt::from_RationalNumber(res.target_frequency.value_or(dt::RationalNumber{0, 0})) == 50.0f);
            REQUIRE(std::holds_alternative<Dynamic_AC_Res>(res.control_mode));

            const auto& res_control_mode = std::get<Dynamic_AC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.present_active_power.value_or(dt::RationalNumber{0, 0})) ==
                    11000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.target_active_power) == 11000.0f);
        }
    }

    GIVEN("Good case - AC_BPT dynamic mode") {
        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::AC_BPT, dt::AcConnector::ThreePhase, dt::ControlMode::Dynamic,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing, dt::BptChannel::Unified,
            dt::GeneratorMode::GridFollowing, 230, dt::GridCodeIslandingDetectionMethod::Passive);

        d20::Session session = d20::Session(service_parameters);
        message_20::AC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_BPT_AC_Req>();
        req_control_mode.present_active_power = {11, 3};
        req_control_mode.max_charge_power = {11, 3};
        req_control_mode.min_charge_power = {4, 0};
        req_control_mode.present_reactive_power = {10, 0};

        req.meter_info_requested = false;

        auto ac_target_power = d20::AcTargetPower{};
        ac_target_power.target_active_power = {11, 3};
        auto ac_present_power = d20::AcPresentPower{};
        ac_present_power.present_active_power = {11, 3};

        const auto res = d20::state::handle_request(req, session, false, false, 50, ac_target_power, ac_present_power,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.status.has_value() == false);
            REQUIRE(res.meter_info.has_value() == false);
            REQUIRE(res.receipt.has_value() == false);
            REQUIRE(dt::from_RationalNumber(res.target_frequency.value_or(dt::RationalNumber{0, 0})) == 50.0f);
            REQUIRE(std::holds_alternative<Dynamic_BPT_AC_Res>(res.control_mode));

            const auto& res_control_mode = std::get<Dynamic_BPT_AC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.present_active_power.value_or(dt::RationalNumber{0, 0})) ==
                    11000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.target_active_power) == 11000.0f);
        }
    }

    GIVEN("Good case - AC dynamic mode, mobility_needs_mode = 2") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::AC, dt::AcConnector::ThreePhase, dt::ControlMode::Dynamic,
            dt::MobilityNeedsMode::ProvidedBySecc, dt::Pricing::NoPricing, 230);

        d20::Session session = d20::Session(service_parameters);
        message_20::AC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_AC_Req>();
        req_control_mode.present_active_power = {11, 3};
        req_control_mode.max_charge_power = {11, 3};
        req_control_mode.min_charge_power = {4, 0};
        req_control_mode.present_reactive_power = {10, 0};

        req.meter_info_requested = false;

        auto ac_target_power = d20::AcTargetPower{};
        ac_target_power.target_active_power = {11, 3};
        auto ac_present_power = d20::AcPresentPower{};
        ac_present_power.present_active_power = {11, 3};

        const d20::UpdateDynamicModeParameters dynamic_parameters = {std::time(nullptr) + 40, std::nullopt, 95};

        const auto res = d20::state::handle_request(req, session, false, false, 50, ac_target_power, ac_present_power,
                                                    dynamic_parameters);

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.status.has_value() == false);
            REQUIRE(res.meter_info.has_value() == false);
            REQUIRE(res.receipt.has_value() == false);
            REQUIRE(dt::from_RationalNumber(res.target_frequency.value_or(dt::RationalNumber{0, 0})) == 50.0f);
            REQUIRE(std::holds_alternative<Dynamic_AC_Res>(res.control_mode));

            const auto& res_control_mode = std::get<Dynamic_AC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.present_active_power.value_or(dt::RationalNumber{0, 0})) ==
                    11000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.target_active_power) == 11000.0f);

            REQUIRE(res_control_mode.departure_time.value_or(0) >= 39);
            REQUIRE(res_control_mode.minimum_soc.value_or(0) == 95);
            REQUIRE(res_control_mode.ack_max_delay.value_or(0) == 30);
        }
    }

    GIVEN("Good case - AC_BPT dynamic mode, mobility_needs_mode = 2") {
        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::AC_BPT, dt::AcConnector::ThreePhase, dt::ControlMode::Dynamic,
            dt::MobilityNeedsMode::ProvidedBySecc, dt::Pricing::NoPricing, dt::BptChannel::Unified,
            dt::GeneratorMode::GridFollowing, 230, dt::GridCodeIslandingDetectionMethod::Passive);

        d20::Session session = d20::Session(service_parameters);
        message_20::AC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_BPT_AC_Req>();
        req_control_mode.present_active_power = {11, 3};
        req_control_mode.max_charge_power = {11, 3};
        req_control_mode.min_charge_power = {4, 0};
        req_control_mode.present_reactive_power = {10, 0};

        req.meter_info_requested = false;

        auto ac_target_power = d20::AcTargetPower{};
        ac_target_power.target_active_power = {11, 3};
        auto ac_present_power = d20::AcPresentPower{};
        ac_present_power.present_active_power = {11, 3};

        const d20::UpdateDynamicModeParameters dynamic_parameters = {std::time(nullptr) + 40, std::nullopt, 95};

        const auto res = d20::state::handle_request(req, session, false, false, 50, ac_target_power, ac_present_power,
                                                    dynamic_parameters);

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.status.has_value() == false);
            REQUIRE(res.meter_info.has_value() == false);
            REQUIRE(res.receipt.has_value() == false);
            REQUIRE(dt::from_RationalNumber(res.target_frequency.value_or(dt::RationalNumber{0, 0})) == 50.0f);
            REQUIRE(std::holds_alternative<Dynamic_BPT_AC_Res>(res.control_mode));

            const auto& res_control_mode = std::get<Dynamic_BPT_AC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.present_active_power.value_or(dt::RationalNumber{0, 0})) ==
                    11000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.target_active_power) == 11000.0f);

            REQUIRE(res_control_mode.departure_time.value_or(0) >= 39);
            REQUIRE(res_control_mode.minimum_soc.value_or(0) == 95);
            REQUIRE(res_control_mode.ack_max_delay.value_or(0) == 30);
        }
    }

    GIVEN("Good case - AC dynamic mode & pause from charger") {

        d20::SelectedServiceParameters service_parameters = d20::SelectedServiceParameters(
            dt::ServiceCategory::AC, dt::AcConnector::ThreePhase, dt::ControlMode::Dynamic,
            dt::MobilityNeedsMode::ProvidedByEvcc, dt::Pricing::NoPricing, 230);

        d20::Session session = d20::Session(service_parameters);
        message_20::AC_ChargeLoopRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& req_control_mode = req.control_mode.emplace<Dynamic_AC_Req>();
        req_control_mode.present_active_power = {11, 3};
        req_control_mode.max_charge_power = {11, 3};
        req_control_mode.min_charge_power = {4, 0};
        req_control_mode.present_reactive_power = {10, 0};

        req.meter_info_requested = false;

        auto ac_target_power = d20::AcTargetPower{};
        ac_target_power.target_active_power = {11, 3};
        auto ac_present_power = d20::AcPresentPower{};
        ac_present_power.present_active_power = {11, 3};

        const auto res = d20::state::handle_request(req, session, false, true, 50, ac_target_power, ac_present_power,
                                                    d20::UpdateDynamicModeParameters());

        THEN("ResponseCode: OK, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.meter_info.has_value() == false);
            REQUIRE(res.receipt.has_value() == false);
            REQUIRE(dt::from_RationalNumber(res.target_frequency.value_or(dt::RationalNumber{0, 0})) == 50.0f);
            REQUIRE(std::holds_alternative<Dynamic_AC_Res>(res.control_mode));

            const auto& res_control_mode = std::get<Dynamic_AC_Res>(res.control_mode);
            REQUIRE(dt::from_RationalNumber(res_control_mode.present_active_power.value_or(dt::RationalNumber{0, 0})) ==
                    11000.0f);
            REQUIRE(dt::from_RationalNumber(res_control_mode.target_active_power) == 11000.0f);

            REQUIRE(res.status.has_value() == true);
            REQUIRE(res.status.value().notification == dt::EvseNotification::Pause);
            REQUIRE(res.status.value().notification_max_delay == 60);
        }
    }
}
