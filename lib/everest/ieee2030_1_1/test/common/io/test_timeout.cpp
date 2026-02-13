// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>
#include <thread>

#include <ieee2030/common/io/time.hpp>

using namespace ieee2030;

SCENARIO("Testing timeout") {
    GIVEN("Timeout not started") {
        io::Timeout timeout;

        WHEN("Check if timeout has not started") {
            REQUIRE(timeout.timeout_reached() == std::nullopt);
        }
    }

    GIVEN("Timeout 4 second started") {
        io::Timeout timeout;

        timeout.start(4);

        WHEN("Check if timeout has started, but not reached") {
            REQUIRE(timeout.timeout_reached() == false);
        }
    }

    GIVEN("Timeout 1 second started") {
        io::Timeout timeout;

        timeout.start(1);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        WHEN("Check if timeout has started and reached") {
            REQUIRE(timeout.timeout_reached() == true);
        }
    }

    GIVEN("Timeout 1 second started") {
        io::Timeout timeout;

        timeout.start(1);

        WHEN("Check if reset is working") {
            REQUIRE(timeout.timeout_reached() == false);

            timeout.reset();

            REQUIRE(timeout.timeout_reached() == std::nullopt);
        }
    }
}