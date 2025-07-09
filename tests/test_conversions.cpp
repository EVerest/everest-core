// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <catch2/catch_all.hpp>

#include <utils/conversions.hpp>
#include <utils/types.hpp>

SCENARIO("Check conversions", "[!throws]") {
    GIVEN("Valid CmdEvents") {
        THEN("It shouldn't throw") {
            CHECK(Everest::conversions::cmd_event_to_string(Everest::CmdEvent::MessageParsingFailed) ==
                  "MessageParsingFailed");

            CHECK(Everest::conversions::cmd_event_to_string(Everest::CmdEvent::SchemaValidationFailed) ==
                  "SchemaValidationFailed");
            CHECK(Everest::conversions::cmd_event_to_string(Everest::CmdEvent::HandlerException) == "HandlerException");
            CHECK(Everest::conversions::cmd_event_to_string(Everest::CmdEvent::Timeout) == "Timeout");
            CHECK(Everest::conversions::cmd_event_to_string(Everest::CmdEvent::Shutdown) == "Shutdown");
            CHECK(Everest::conversions::cmd_event_to_string(Everest::CmdEvent::NotReady) == "NotReady");
        }
    }

    GIVEN("Invalid CmdEvents") {
        THEN("It should throw") {
            CHECK_THROWS(Everest::conversions::cmd_event_to_string(static_cast<Everest::CmdEvent>(-1)));
        }
    }

    GIVEN("Valid CmdEvent strings") {
        THEN("It shouldn't throw") {
            CHECK(Everest::conversions::string_to_cmd_event("MessageParsingFailed") ==
                  Everest::CmdEvent::MessageParsingFailed);

            CHECK(Everest::conversions::string_to_cmd_event("SchemaValidationFailed") ==
                  Everest::CmdEvent::SchemaValidationFailed);
            CHECK(Everest::conversions::string_to_cmd_event("HandlerException") == Everest::CmdEvent::HandlerException);
            CHECK(Everest::conversions::string_to_cmd_event("Timeout") == Everest::CmdEvent::Timeout);
            CHECK(Everest::conversions::string_to_cmd_event("Shutdown") == Everest::CmdEvent::Shutdown);
            CHECK(Everest::conversions::string_to_cmd_event("NotReady") == Everest::CmdEvent::NotReady);
        }
    }

    GIVEN("Invalid CmdEvent strings") {
        THEN("It should throw") {
            CHECK_THROWS(Everest::conversions::string_to_cmd_event("ThisIsAnInvalidCmdEventString"));
        }
    }

    GIVEN("Valid CmdEventError") {
        THEN("It shouldn't throw") {
            Everest::CmdResultError cmd_result_error = {Everest::CmdEvent::Shutdown, "message", nullptr};
            json cmd_result_error_json = {{"event", "Shutdown"}, {"msg", "message"}};
            Everest::CmdResultError cmd_result_error_from_json = cmd_result_error_json;
            CHECK(json(cmd_result_error) == cmd_result_error_json);
            CHECK(json(cmd_result_error_from_json) == cmd_result_error_json);
        }
    }
}
