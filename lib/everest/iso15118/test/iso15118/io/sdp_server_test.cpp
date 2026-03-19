// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <optional>
#include <thread>

#include <iso15118/io/time.hpp>

SCENARIO("V2G Communication Setup Timeout Pattern") {

    GIVEN("A timeout starts when dlink becomes ready") {
        std::optional<iso15118::Timeout> timeout;
        timeout.emplace(100);

        REQUIRE(timeout.has_value());
        REQUIRE(timeout->is_reached() == false);
    }

    GIVEN("A timeout fires after duration expires") {
        std::optional<iso15118::Timeout> timeout;
        timeout.emplace(15);

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        REQUIRE(timeout->is_reached() == true);
    }

    GIVEN("A timeout is cancelled when session is established") {
        std::optional<iso15118::Timeout> timeout;
        timeout.emplace(100);

        REQUIRE(timeout.has_value());
        // Session established -> cancel timeout
        timeout.reset();
        REQUIRE_FALSE(timeout.has_value());
    }

    GIVEN("A timeout is cancelled when dlink becomes not ready") {
        std::optional<iso15118::Timeout> timeout;
        timeout.emplace(100);

        REQUIRE(timeout.has_value());
        // dlink not ready -> cancel timeout
        timeout.reset();
        REQUIRE_FALSE(timeout.has_value());
    }
}
