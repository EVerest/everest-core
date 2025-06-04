// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/session/feedback.hpp>

using namespace iso15118::session;

namespace dt = iso15118::message_20::datatypes;

struct FeedbackResults {
    feedback::Signal signal;
    float target_voltage;
    feedback::DcChargeLoopReq dc_charge_loop_req;
    feedback::DcMaximumLimits dc_max_limits;
    iso15118::message_20::Type v2g_message;
    std::string evcc_id;
    std::string selected_protocol;
};

SCENARIO("Feedback Tests") {

    FeedbackResults feedback_results;
    feedback::Callbacks callbacks;

    callbacks.signal = [&feedback_results](feedback::Signal signal_) { feedback_results.signal = signal_; };
    callbacks.dc_pre_charge_target_voltage = [&feedback_results](float target_voltage_) {
        feedback_results.target_voltage = target_voltage_;
    };
    callbacks.dc_charge_loop_req = [&feedback_results](const feedback::DcChargeLoopReq& dc_charge_loop_req) {
        feedback_results.dc_charge_loop_req = dc_charge_loop_req;
    };
    callbacks.dc_max_limits = [&feedback_results](const feedback::DcMaximumLimits& dc_max_limits_) {
        feedback_results.dc_max_limits = dc_max_limits_;
    };
    callbacks.v2g_message = [&feedback_results](const iso15118::message_20::Type& type) {
        feedback_results.v2g_message = type;
    };
    callbacks.evccid = [&feedback_results](const std::string& evcc_id_) { feedback_results.evcc_id = evcc_id_; };
    callbacks.selected_protocol = [&feedback_results](const std::string& protocol) {
        feedback_results.selected_protocol = protocol;
    };

    const auto feedback = Feedback(callbacks);

    GIVEN("Test signal") {
        const feedback::Signal expected = feedback::Signal::REQUIRE_AUTH_EIM;
        feedback.signal(feedback::Signal::REQUIRE_AUTH_EIM);

        THEN("signal should be like expected") {
            REQUIRE(feedback_results.signal == expected);
        }
    }

    GIVEN("Test dc_pre_charge_target_voltage") {
        float expected{421.4};
        feedback.dc_pre_charge_target_voltage(421.4);

        THEN("dc_pre_charge_target_voltage should be like expected") {
            REQUIRE(feedback_results.target_voltage == expected);
        }
    }

    GIVEN("Test dc_charge_loop_req - bpt scheduled") {

        using BPT_ScheduleReqControlMode = dt::BPT_Scheduled_DC_CLReqControlMode;

        const BPT_ScheduleReqControlMode expected = {{
                                                         {std::nullopt, std::nullopt, std::nullopt},
                                                         {4402, -1},
                                                         {30, 0},
                                                         std::nullopt,
                                                         dt::RationalNumber{34, 0},
                                                         std::nullopt,
                                                         std::nullopt,
                                                         std::nullopt,
                                                     },
                                                     dt::RationalNumber{11, 3},
                                                     dt::RationalNumber{32, 1},
                                                     std::nullopt};

        feedback.dc_charge_loop_req(expected);

        THEN("dc_charge_loop_req should be like expected") {

            const auto* dc_control_mode = std::get_if<feedback::DcReqControlMode>(&feedback_results.dc_charge_loop_req);

            const auto* bpt_scheduled_control_mode = std::get_if<BPT_ScheduleReqControlMode>(dc_control_mode);

            REQUIRE(bpt_scheduled_control_mode->target_energy_request.has_value() == false);
            REQUIRE(bpt_scheduled_control_mode->max_energy_request.has_value() == false);
            REQUIRE(bpt_scheduled_control_mode->min_energy_request.has_value() == false);

            REQUIRE(dt::from_RationalNumber(bpt_scheduled_control_mode->target_current) ==
                    dt::from_RationalNumber(expected.target_current));
            REQUIRE(dt::from_RationalNumber(bpt_scheduled_control_mode->target_voltage) ==
                    dt::from_RationalNumber(expected.target_voltage));
            REQUIRE(bpt_scheduled_control_mode->max_charge_power.has_value() == false);
            REQUIRE(bpt_scheduled_control_mode->min_charge_power.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*bpt_scheduled_control_mode->min_charge_power) ==
                    dt::from_RationalNumber(expected.min_charge_power.value_or(dt::RationalNumber{})));
            REQUIRE(bpt_scheduled_control_mode->max_charge_current.has_value() == false);
            REQUIRE(bpt_scheduled_control_mode->max_voltage.has_value() == false);
            REQUIRE(bpt_scheduled_control_mode->min_voltage.has_value() == false);

            REQUIRE(bpt_scheduled_control_mode->max_discharge_power.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*bpt_scheduled_control_mode->max_discharge_power) ==
                    dt::from_RationalNumber(expected.max_discharge_power.value_or(dt::RationalNumber{})));
            REQUIRE(bpt_scheduled_control_mode->min_discharge_power.has_value() == true);
            REQUIRE(dt::from_RationalNumber(*bpt_scheduled_control_mode->min_discharge_power) ==
                    dt::from_RationalNumber(expected.min_discharge_power.value_or(dt::RationalNumber{})));
            REQUIRE(bpt_scheduled_control_mode->max_discharge_current.has_value() == false);
        }
    }

    GIVEN("Test dc_charge_loop_req - dynamic") {

        using DynamicReqControlMode = dt::Dynamic_DC_CLReqControlMode;

        const DynamicReqControlMode expected = {
            {std::nullopt, {2344, 1}, {30, 3}, {10, 3}}, {22, 3}, {0, 0}, {5, 2}, {9, 2}, {25, 1}};

        feedback.dc_charge_loop_req(expected);

        THEN("dc_charge_loop_req should be like expected") {

            const auto* dc_control_mode = std::get_if<feedback::DcReqControlMode>(&feedback_results.dc_charge_loop_req);

            const auto* dynamic_control_mode = std::get_if<DynamicReqControlMode>(dc_control_mode);

            REQUIRE(dynamic_control_mode->departure_time.has_value() == false);

            REQUIRE(dt::from_RationalNumber(dynamic_control_mode->target_energy_request) ==
                    dt::from_RationalNumber(expected.target_energy_request));
            REQUIRE(dt::from_RationalNumber(dynamic_control_mode->max_energy_request) ==
                    dt::from_RationalNumber(expected.max_energy_request));
            REQUIRE(dt::from_RationalNumber(dynamic_control_mode->min_energy_request) ==
                    dt::from_RationalNumber(expected.min_energy_request));

            REQUIRE(dt::from_RationalNumber(dynamic_control_mode->max_charge_power) ==
                    dt::from_RationalNumber(expected.max_charge_power));
            REQUIRE(dt::from_RationalNumber(dynamic_control_mode->min_charge_power) ==
                    dt::from_RationalNumber(expected.min_charge_power));
            REQUIRE(dt::from_RationalNumber(dynamic_control_mode->max_charge_current) ==
                    dt::from_RationalNumber(expected.max_charge_current));
            REQUIRE(dt::from_RationalNumber(dynamic_control_mode->max_voltage) ==
                    dt::from_RationalNumber(expected.max_voltage));
            REQUIRE(dt::from_RationalNumber(dynamic_control_mode->min_voltage) ==
                    dt::from_RationalNumber(expected.min_voltage));
        }
    }

    GIVEN("Test dc_max_limits") {
        const feedback::DcMaximumLimits expected{803.1, 10.0, 8031};
        feedback.dc_max_limits({803.1, 10.0, 8031});

        THEN("dc_max_limits should be like expected") {
            REQUIRE(feedback_results.dc_max_limits.voltage == expected.voltage);
            REQUIRE(feedback_results.dc_max_limits.current == expected.current);
            REQUIRE(feedback_results.dc_max_limits.power == expected.power);
        }
    }

    GIVEN("Test v2g_message") {
        using Type = iso15118::message_20::Type;
        const Type expected = Type::DC_CableCheckReq;
        feedback.v2g_message(Type::DC_CableCheckReq);

        THEN("v2g_message should be like expected") {
            REQUIRE(feedback_results.v2g_message == expected);
        }
    }

    GIVEN("Test evcc_id") {
        const std::string expected = "54EA7E40B356";
        feedback.evcc_id("54EA7E40B356");

        THEN("evcc_id should be like expected") {
            REQUIRE(feedback_results.evcc_id == expected);
        }
    }

    GIVEN("Test selected_protocol") {
        const std::string expected = "ISO15118-20:DC";
        feedback.selected_protocol("ISO15118-20:DC");

        THEN("selected_protocol should be like expected") {
            REQUIRE(feedback_results.selected_protocol == expected);
        }
    }

    GIVEN("Test display_parameters") {
        const dt::DisplayParameters expected{40,           std::nullopt, 95,           std::nullopt, std::nullopt,
                                             std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt};

        feedback.dc_charge_loop_req(expected);

        THEN("display_parameters should be like expected") {
            const auto* display_parameters = std::get_if<dt::DisplayParameters>(&feedback_results.dc_charge_loop_req);
            REQUIRE(display_parameters->present_soc.has_value() == true);
            REQUIRE(*display_parameters->present_soc == expected.present_soc.value_or(0));
            REQUIRE(display_parameters->min_soc.has_value() == false);
            REQUIRE(display_parameters->target_soc.has_value() == true);
            REQUIRE(*display_parameters->target_soc == expected.target_soc.value_or(0));
        }
    }

    GIVEN("Test dc_present_voltage") {
        feedback::PresentVoltage expected{7044, -1};
        feedback.dc_charge_loop_req(expected);

        THEN("dc_present_voltage should be like expected") {
            const auto* present_voltage = std::get_if<feedback::PresentVoltage>(&feedback_results.dc_charge_loop_req);
            REQUIRE(dt::from_RationalNumber(*present_voltage) == dt::from_RationalNumber(expected));
        }
    }

    GIVEN("Test meter_info_requested") {
        feedback::MeterInfoRequested expected{true};
        feedback.dc_charge_loop_req(true);

        THEN("meter_info_requested should be like expected") {
            REQUIRE(*std::get_if<feedback::MeterInfoRequested>(&feedback_results.dc_charge_loop_req) == expected);
        }
    }

    // TODO(SL): Missing tests for notify_ev_charging_needs, selected_service_parameters
}
