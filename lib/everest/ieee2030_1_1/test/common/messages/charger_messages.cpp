// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <ieee2030/common/messages/messages.hpp>

using namespace ieee2030;

SCENARIO("Charger 108 message") {

    GIVEN("Create from payload") {
        std::vector<uint8_t> payload = {1, 0x03, 0x20, 30, 0x03, 0x20, 0, 0};
        auto charger_108 = messages::Charger108(payload);

        THEN("Charger 108 should set correct!") {
            REQUIRE(charger_108.identifier_welding_detection == 1);
            REQUIRE(charger_108.available_voltage == 800.0);
            REQUIRE(charger_108.available_current == 30.0);
            REQUIRE(charger_108.threshold_voltage == 800.0);
        }
    }

    GIVEN("Set payload with operator overload") {
        auto charger_108 = messages::Charger108();
        charger_108.identifier_welding_detection = 1;
        charger_108.available_voltage = 400;
        charger_108.available_current = 20;
        charger_108.threshold_voltage = 250;

        std::vector<uint8_t> payload = charger_108;

        THEN("Charger108 payload should set correct!") {
            REQUIRE(payload[0] == 0x01);
            REQUIRE(payload[1] == 0x01);
            REQUIRE(payload[2] == 0x90);
            REQUIRE(payload[3] == 0x14);
            REQUIRE(payload[4] == 0x00);
            REQUIRE(payload[5] == 0xFA);
            REQUIRE(payload[6] == 0x00);
            REQUIRE(payload[7] == 0x00);
        }
    }
}

SCENARIO("Charger 109 message") {

    GIVEN("Create from payload") {
        std::vector<uint8_t> payload = {1, 0x01, 0xC2, 0xC8, 0, 0x05, 0xFF, 0x1E};
        auto charger_109 = messages::Charger109(payload);

        THEN("Charger 109 should set correct!") {
            REQUIRE(charger_109.protocol == defs::ProtocolNumber::VERSION_1_X_X);
            REQUIRE(charger_109.present_voltage == 450);
            REQUIRE(charger_109.present_current == 200);

            REQUIRE(charger_109.charger_status == true);
            REQUIRE(charger_109.charger_malfunction == false);
            REQUIRE(charger_109.connector_lock == true);
            REQUIRE(charger_109.battery_incompatibility == false);
            REQUIRE(charger_109.system_malfunction == false);
            REQUIRE(charger_109.stop_control == false);

            REQUIRE(charger_109.reamining_time_10s == 0xFF);
            REQUIRE(charger_109.reamining_time_1min == 30);
        }
    }

    GIVEN("Set payload with operator overload") {
        auto charger_109 = messages::Charger109();

        charger_109.protocol = defs::ProtocolNumber::VERSION_2_0;
        charger_109.present_voltage = 450;
        charger_109.present_current = 200;

        charger_109.charger_status = true;
        charger_109.charger_malfunction = false;
        charger_109.connector_lock = true;
        charger_109.battery_incompatibility = false;
        charger_109.system_malfunction = false;
        charger_109.stop_control = false;

        charger_109.reamining_time_10s = 0xFF;
        charger_109.reamining_time_1min = 30;

        std::vector<uint8_t> payload = charger_109;

        THEN("Charger109 payload should set correct!") {
            REQUIRE(payload[0] == 0x02);
            REQUIRE(payload[1] == 0x01);
            REQUIRE(payload[2] == 0xC2);
            REQUIRE(payload[3] == 0xC8);
            REQUIRE(payload[4] == 0x00);
            REQUIRE(payload[5] == 0x05);
            REQUIRE(payload[6] == 0xFF);
            REQUIRE(payload[7] == 0x1E);
        }
    }
}