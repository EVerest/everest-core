// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <optional>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "../lib/util.hpp"

using namespace ev::util;

struct SenseOfLive {
    int value;
    static constexpr auto correct_value = 42;
};

struct LatchCheck {
    bool valid;
    bool got_checked;
    static constexpr auto return_value = -42;
};
struct Matcher {
    using return_type = std::optional<int>;

    return_type operator()(const SenseOfLive& item) {
        if (item.value == SenseOfLive::correct_value) {
            return item.value;
        }

        return std::nullopt;
    }

    return_type operator()(LatchCheck& item) {
        item.got_checked = true;
        if (item.valid) {
            return LatchCheck::return_value;
        }

        return std::nullopt;
    }
};

SCENARIO("feeding find_first_match with various arguments", "[util::find_first_match]") {
    WHEN("no matchables are supplied") {
        const auto result = find_first_match(Matcher());

        THEN("the result should be the default constructed return value") {
            REQUIRE(result == Matcher::return_type());
        }
    }
    WHEN("all matchables are false") {
        const auto result = find_first_match(Matcher(), SenseOfLive{23}, SenseOfLive{12});

        THEN("there should be no match") {
            REQUIRE(not result);
        }
    }

    WHEN("a matchable is correct") {
        const auto result =
            find_first_match(Matcher(), SenseOfLive{13}, SenseOfLive{SenseOfLive::correct_value}, SenseOfLive{12});

        THEN("there should be a match") {
            REQUIRE(result);
        }

        AND_THEN("it should have the correct value") {
            REQUIRE(*result == SenseOfLive::correct_value);
        }
    }

    WHEN("the first matchable is correct") {
        LatchCheck latch_check{true};

        const auto result = find_first_match(Matcher(), SenseOfLive{SenseOfLive::correct_value}, latch_check);

        THEN("the second matchable shouldn't have been tested") {
            REQUIRE(latch_check.got_checked == false);
        }
    }

    WHEN("the first matchable is not correct") {
        LatchCheck latch_check{true};

        const auto result = find_first_match(Matcher(), SenseOfLive{0}, latch_check);

        THEN("the second matchabe should be checked") {
            REQUIRE(latch_check.got_checked == true);
        }

        AND_THEN("it should have the correct value") {
            REQUIRE(result);
            REQUIRE(*result == LatchCheck::return_value);
        }
    }
}
