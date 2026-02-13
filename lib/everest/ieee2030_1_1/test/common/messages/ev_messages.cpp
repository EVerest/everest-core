#include <catch2/catch_test_macros.hpp>

#include <ieee2030/common/messages/messages.hpp>

using namespace ieee2030;

SCENARIO("EV 100 message") {
    GIVEN("Create from payload") {
        std::vector<uint8_t> payload = {0x00, 0x00, 0x00, 0x00, 0x02, 0x26, 0x64, 0x00};
        auto ev_100 = messages::EV100(payload);

        THEN("EV 100 should set correct!") {
            REQUIRE(ev_100.max_battery_voltage == 550);
            REQUIRE(ev_100.charged_rate == 100);
        }
    }

    GIVEN("Set payload with operator overload") {
        auto ev_100 = messages::EV100();
        ev_100.max_battery_voltage = 550;

        std::vector<uint8_t> payload = ev_100;

        THEN("EV 100 payload should set correct!") {
            REQUIRE(payload[0] == 0x00);
            REQUIRE(payload[1] == 0x00);
            REQUIRE(payload[2] == 0x00);
            REQUIRE(payload[3] == 0x00);
            REQUIRE(payload[4] == 0x02);
            REQUIRE(payload[5] == 0x26);
            REQUIRE(payload[6] == 0x64);
            REQUIRE(payload[7] == 0x00);
        }
    }
}

SCENARIO("EV 101 message") {
    GIVEN("Create from payload") {
        std::vector<uint8_t> payload = {0x00, 0x3C, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00};
        auto ev_101 = messages::EV101(payload);

        THEN("EV 101 should set correct!") {
            REQUIRE(ev_101.max_charging_time_10s == 60);
            REQUIRE(ev_101.max_charging_time_1min == 0x00);
            REQUIRE(ev_101.estimated_charging_time_1min == 9);

            REQUIRE(ev_101.total_capacity.has_value() == false);
        }
    }

    GIVEN("Set payload with operator overload") {
        auto ev_101 = messages::EV101();
        ev_101.max_charging_time_10s = 60;
        ev_101.max_charging_time_1min = 0x00;
        ev_101.estimated_charging_time_1min = 9;
        ev_101.total_capacity = 800;

        std::vector<uint8_t> payload = ev_101;

        THEN("EV 101 payload should set correct!") {
            REQUIRE(payload[0] == 0x00);
            REQUIRE(payload[1] == 0x3C);
            REQUIRE(payload[2] == 0x00);
            REQUIRE(payload[3] == 0x09);
            REQUIRE(payload[4] == 0x00);
            REQUIRE(payload[5] == 0x03);
            REQUIRE(payload[6] == 0x20);
            REQUIRE(payload[7] == 0x00);
        }
    }
}

SCENARIO("EV 102 message") {
}