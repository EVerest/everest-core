// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/session/feedback.hpp>

using namespace iso15118::session;

struct FeedbackResults {
    feedback::Signal signal;
    feedback::DcChargeTarget dc_charge_target;
    feedback::DcMaximumLimits dc_max_limits;
    iso15118::message_20::Type v2g_message;
    std::string evcc_id;
    std::string selected_protocol;
    feedback::DisplayParameters display_parameters;
};

SCENARIO("Feedback Tests") {

    FeedbackResults feedback_results;
    feedback::Callbacks callbacks;

    callbacks.signal = [&feedback_results](feedback::Signal signal_) { feedback_results.signal = signal_; };
    callbacks.dc_charge_target = [&feedback_results](const feedback::DcChargeTarget& dc_charge_target_) {
        feedback_results.dc_charge_target = dc_charge_target_;
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
    callbacks.display_parameters = [&feedback_results](const feedback::DisplayParameters& parameters) {
        feedback_results.display_parameters = parameters;
    };

    const auto feedback = Feedback(callbacks);

    GIVEN("Test signal") {
        const feedback::Signal expected = feedback::Signal::REQUIRE_AUTH_EIM;
        feedback.signal(feedback::Signal::REQUIRE_AUTH_EIM);

        THEN("signal should be like expected") {
            REQUIRE(feedback_results.signal == expected);
        }
    }

    GIVEN("Test dc_charge_target") {
        const feedback::DcChargeTarget expected{421.4, 200.4};
        feedback.dc_charge_target({421.4, 200.4});

        THEN("dc_charge_target should be like expected") {
            REQUIRE(feedback_results.dc_charge_target.voltage == expected.voltage);
            REQUIRE(feedback_results.dc_charge_target.current == expected.current);
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
        const feedback::DisplayParameters expected{40, std::nullopt, 95};
        feedback.display_parameters({40, std::nullopt, 95});

        THEN("display_parameters should be like expected") {
            REQUIRE(feedback_results.display_parameters.present_soc.has_value() == true);
            REQUIRE(*feedback_results.display_parameters.present_soc == expected.present_soc.value_or(0));
            REQUIRE(feedback_results.display_parameters.minimum_soc.has_value() == false);
            REQUIRE(feedback_results.display_parameters.target_soc.has_value() == true);
            REQUIRE(*feedback_results.display_parameters.target_soc == expected.target_soc.value_or(0));
        }
    }
}
