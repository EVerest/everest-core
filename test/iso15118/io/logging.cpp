// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/helper.hpp>
#include <iso15118/io/logging.hpp>

using namespace iso15118;

SCENARIO("Logging Tests") {

    LogLevel log_level;
    std::string log_msg;

    io::set_logging_callback([&log_level, &log_msg](const iso15118::LogLevel& level, const std::string& msg) {
        log_level = level;
        log_msg = msg;
    });

    GIVEN("Test logf without LogLevel") {

        const LogLevel expected_log_level{LogLevel::Info};
        const std::string expected_msg = "TEST!";

        logf("TEST!");

        THEN("log_level & log_msg should be like expected") {
            REQUIRE(log_level == expected_log_level);
            REQUIRE(log_msg == expected_msg);
        }
    }

    GIVEN("Test logf with LogLevel") {

        const LogLevel expected_log_level{LogLevel::Warning};
        const std::string expected_msg = "logf with warning";

        logf(LogLevel::Warning, "logf with warning");

        THEN("log_level & log_msg should be like expected") {
            REQUIRE(log_level == expected_log_level);
            REQUIRE(log_msg == expected_msg);
        }
    }

    GIVEN("Test logf with arguments") {

        const LogLevel expected_log_level{LogLevel::Info};
        const std::string expected_msg = "logf: 5";

        logf("logf: %d", 5);

        THEN("log_level & log_msg should be like expected") {
            REQUIRE(log_level == expected_log_level);
            REQUIRE(log_msg == expected_msg);
        }
    }

    GIVEN("Test logf_error") {

        const LogLevel expected_log_level{LogLevel::Error};
        const std::string expected_msg = "Test logf_error";

        logf_error("Test logf_error");

        THEN("log_level & log_msg should be like expected") {
            REQUIRE(log_level == expected_log_level);
            REQUIRE(log_msg == expected_msg);
        }
    }

    GIVEN("Test logf_error with arguments") {

        const LogLevel expected_log_level{LogLevel::Error};
        const std::string expected_msg = "Test logf_error: 8";

        logf_error("Test logf_error: %d", 8);

        THEN("log_level & log_msg should be like expected") {
            REQUIRE(log_level == expected_log_level);
            REQUIRE(log_msg == expected_msg);
        }
    }

    GIVEN("Test logf_warning") {

        const LogLevel expected_log_level{LogLevel::Warning};
        const std::string expected_msg = "Test logf_warning";

        logf_warning("Test logf_warning");

        THEN("log_level & log_msg should be like expected") {
            REQUIRE(log_level == expected_log_level);
            REQUIRE(log_msg == expected_msg);
        }
    }

    GIVEN("Test logf_warning with arguments") {

        const LogLevel expected_log_level{LogLevel::Warning};
        const std::string expected_msg = "Test logf_warning: 4";

        logf_warning("Test logf_warning: %d", 4);

        THEN("log_level & log_msg should be like expected") {
            REQUIRE(log_level == expected_log_level);
            REQUIRE(log_msg == expected_msg);
        }
    }

    GIVEN("Test logf_info") {

        const LogLevel expected_log_level{LogLevel::Info};
        const std::string expected_msg = "Test logf_info";

        logf_info("Test logf_info");

        THEN("log_level & log_msg should be like expected") {
            REQUIRE(log_level == expected_log_level);
            REQUIRE(log_msg == expected_msg);
        }
    }

    GIVEN("Test logf_info with arguments") {

        const LogLevel expected_log_level{LogLevel::Info};
        const std::string expected_msg = "Test logf_info: d";

        logf_info("Test logf_info: %c", 'd');

        THEN("log_level & log_msg should be like expected") {
            REQUIRE(log_level == expected_log_level);
            REQUIRE(log_msg == expected_msg);
        }
    }

    GIVEN("Test logf_debug") {

        const LogLevel expected_log_level{LogLevel::Debug};
        const std::string expected_msg = "Test logf_debug";

        logf_debug("Test logf_debug");

        THEN("log_level & log_msg should be like expected") {
            REQUIRE(log_level == expected_log_level);
            REQUIRE(log_msg == expected_msg);
        }
    }

    GIVEN("Test logf_debug with arguments") {

        const LogLevel expected_log_level{LogLevel::Debug};
        const std::string expected_msg = "Test logf_debug: 23";

        logf_debug("Test logf_debug: %d", 23);

        THEN("log_level & log_msg should be like expected") {
            REQUIRE(log_level == expected_log_level);
            REQUIRE(log_msg == expected_msg);
        }
    }

    GIVEN("Test logf_trace") {

        const LogLevel expected_log_level{LogLevel::Trace};
        const std::string expected_msg = "Test logf_trace";

        logf_trace("Test logf_trace");

        THEN("log_level & log_msg should be like expected") {
            REQUIRE(log_level == expected_log_level);
            REQUIRE(log_msg == expected_msg);
        }
    }

    GIVEN("Test logf_trace with arguments") {

        const LogLevel expected_log_level{LogLevel::Trace};
        const std::string expected_msg = "Test logf_trace: 20000";

        logf_trace("Test logf_trace: %d", 20000);

        THEN("log_level & log_msg should be like expected") {
            REQUIRE(log_level == expected_log_level);
            REQUIRE(log_msg == expected_msg);
        }
    }
}
