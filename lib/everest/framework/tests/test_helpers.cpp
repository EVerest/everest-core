// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <catch2/catch_all.hpp>

#include <chrono>
#include <iostream>

#include "../lib/message_handler.cpp"

SCENARIO("Check static helper functions", "[!throws]") {
    GIVEN("Valid wildcard topic") {
        THEN("It should match") {
            CHECK(Everest::check_topic_matches("same_topic", "same_topic"));
            CHECK(not Everest::check_topic_matches("same_topic_not", "same_topic"));
            CHECK(not Everest::check_topic_matches("same_topic", "same_topic_not"));
            CHECK(Everest::check_topic_matches("full/topic/to/check", "full/#"));
            CHECK(Everest::check_topic_matches("full/topic/to/check", "full/+/to/check"));
            CHECK(Everest::check_topic_matches("full//to/check", "full/+/to/check"));
            CHECK(not Everest::check_topic_matches("full/topic/to/check", "full/topic/to/check+"));
            CHECK(not Everest::check_topic_matches("full/topic/to/check", "full/+/not/check"));
            CHECK(Everest::check_topic_matches("full/topic/to/check", "full/+/+/check"));
            CHECK(Everest::check_topic_matches("full/topic/to/check", "full/+/to/#"));
            CHECK(Everest::check_topic_matches("full/topic/to/check", "+/+/+/+"));
            // these are technically not allowed, but we treat the first # as meaning "nothing comes after this"
            CHECK(Everest::check_topic_matches("full/topic/to/check", "full/#/to/check"));
            CHECK(Everest::check_topic_matches("full/topic/to/check", "full/+/to/#/"));
        }
    }
}
