// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <ieee2030/charger/v20/control_event.hpp>
#include <ieee2030/common/v20/event_queue.hpp>

using namespace ieee2030;

SCENARIO("Testing event queue") {
    GIVEN("Create event queue and check if queue is empty") {
        events::EventQueue<charger::events::Event> queue;

        THEN("Queue should be empty") {
            REQUIRE(queue.pop() == std::nullopt);
        }
    }

    GIVEN("Queue pop one element") {
        events::EventQueue<charger::events::Event> queue;

        queue.push(charger::events::StopCharging{true});

        THEN("Queue should have one element") {
            auto event = queue.pop();
            REQUIRE(event.has_value());
            REQUIRE(std::holds_alternative<charger::events::StopCharging>(event.value()) == true);
            REQUIRE(queue.pop() == std::nullopt);
        }
    }

    GIVEN("Queue pop 4 element") {
        events::EventQueue<charger::events::Event> queue;

        queue.push(charger::events::StopCharging{true});
        queue.push(charger::events::CS1{false});
        queue.push(charger::events::CS1{true});
        queue.push(charger::events::ChargePermission{true});

        THEN("Queue should have 4 elements in the correct order") {
            auto event = queue.pop();
            REQUIRE(event.has_value());
            REQUIRE(std::holds_alternative<charger::events::StopCharging>(event.value()) == true);
            const auto stop_charging = std::get<charger::events::StopCharging>(event.value());
            REQUIRE(stop_charging == charger::events::StopCharging{true});

            event = queue.pop();
            REQUIRE(event.has_value());
            REQUIRE(std::holds_alternative<charger::events::CS1>(event.value()) == true);
            auto cs1 = std::get<charger::events::CS1>(event.value());
            REQUIRE(cs1 == charger::events::CS1{false});

            event = queue.pop();
            REQUIRE(event.has_value());
            REQUIRE(std::holds_alternative<charger::events::CS1>(event.value()) == true);
            cs1 = std::get<charger::events::CS1>(event.value());
            REQUIRE(cs1 == charger::events::CS1{true});

            event = queue.pop();
            REQUIRE(event.has_value());
            REQUIRE(std::holds_alternative<charger::events::ChargePermission>(event.value()) == true);
            const auto permission = std::get<charger::events::ChargePermission>(event.value());
            REQUIRE(permission == charger::events::ChargePermission{true});

            REQUIRE(queue.pop() == std::nullopt);
        }
    }
}