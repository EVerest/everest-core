// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/**
 * @example test_mqtt_pty_can_bridge.cpp Forwarding between can and pty via two mqtt clients
 */

#include "everest/io/can/can_payload.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <everest/io/can/socket_can.hpp>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/mqtt/mqtt_client.hpp>
#include <everest/io/serial/event_pty.hpp>
#include <iostream>
#include <string>

using namespace everest::lib::io::event;
using namespace everest::lib::io;
using namespace std::chrono_literals;

constexpr auto HOST = "localhost";
constexpr std::uint16_t PORT = 1883;

using payloadT = mqtt::mqtt_client::ClientPayloadT;
using generic_client = typename mqtt::mqtt_client::interface;

void run_basic_client() {
    std::cout << "mqtt_pty_can_bridge test" << std::endl;
    mqtt::mqtt_client mqtt_to_pty(HOST, PORT);
    mqtt::mqtt_client mqtt_to_can(HOST, PORT);
    serial::event_pty pty;
    can::socket_can can_dev("vcan0");

    mqtt_to_pty.subscribe("pty_to_mqtt");
    mqtt_to_pty.set_message_handler([&](auto const& pl, auto&) {
        if (pl.topic == "pty_to_mqtt") {
            pty.tx({pl.message.begin(), pl.message.end()});
        }
    });
    pty.set_data_handler([&](auto const& pl, auto&) {
        auto msg = std::string(pl.begin(), pl.end());
        mqtt_to_pty.tx({"can_to_mqtt", msg});
    });

    mqtt_to_can.subscribe("can_to_mqtt");
    mqtt_to_can.set_message_handler([&](auto const& pl, auto&) {
        if (pl.topic == "can_to_mqtt") {
            can::can_dataset out;
            out.set_can_id(0x01);
            auto limit = std::min<uint16_t>(pl.message.size(), 8);
            out.payload.assign(pl.message.begin(), pl.message.begin() + limit);
            can_dev.tx(out);
        }
    });
    can_dev.set_rx_handler([&](auto const& pl, auto&) {
        auto msg = std::string(pl.payload.begin(), pl.payload.end());
        mqtt_to_can.tx({"pty_to_mqtt", msg});
    });

    fd_event_handler ev_handler;

    ev_handler.register_event_handler(mqtt_to_pty.get_poll_fd(), [&](auto&) { mqtt_to_pty.sync(); },
                                      {poll_events::read});
    ev_handler.register_event_handler(&pty);
    ev_handler.register_event_handler(&can_dev);
    ev_handler.register_event_handler(&mqtt_to_can);

    while (true) {
        ev_handler.poll();
    }
}

int main(int, char*[]) {
    run_basic_client();

    return 0;
}
