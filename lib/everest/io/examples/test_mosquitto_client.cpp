// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/**
 * @example test_mqtt_client.cpp Creates two MQTT clients that ping/pong messages to each other
 */

#include "everest/io/mqtt/mosquitto_cpp.hpp"
#include <cstdio>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/mqtt/mqtt_client.hpp>
#include <iostream>
#include <string>
#include <thread>

using namespace everest::lib::io::event;
using namespace everest::lib::io;
using namespace std::chrono_literals;

constexpr auto HOST = "localhost";
constexpr std::uint16_t PORT = 1883;

using payloadT = mqtt::mosquitto_cpp::message;
using generic_client = typename mqtt::mosquitto_cpp;

// Creates the error handler for the clients
mqtt::mqtt_client::cb_error make_error_cb(mqtt::mqtt_client& client) {
    return [&](int error, std::string const& msg) {
        std::cerr << "ERROR ( " << error << " ): " << msg << std::endl;
        if (error) {
            std::cout << "What to do?" << std::endl;
        }
    };
}

// Creates the RX handler for the client
mqtt::mqtt_client::subscribe_message_callback make_rx_callback(mqtt::mosquitto_cpp&, std::string tar) {
    return [&, tar](generic_client& client, payloadT const& data) {
        std::cout << &client << "\n"
                  << "topic: " << data.topic << "\n"
                  << "msg:   " << data.payload << std::endl;
        payloadT dataset;
        auto c = std::stoi(data.payload) + 1;
        dataset.payload = std::to_string(c);
        dataset.topic = tar;
        client.publish(dataset);
    };
}

int main(int, char*[]) {
    std::cout << "mqtt_client ping/pong demonstration" << std::endl;
    bool server_connected = false;
    bool client_connected = false;

    // Create first mqtt_client
    mqtt::mqtt_client client(2000);
    // Create and assign error handler
    client.set_error_handler(make_error_cb(client));
    // Create and assign rx handler
    client.set_callback_connect([&](auto& mqtt, auto, auto, auto const&) {
        client_connected = true;
        mqtt.subscribe("test/client/recv/#", make_rx_callback(mqtt, "test/server/recv"),
                       mqtt::mosquitto_cpp::QoS::at_most_once);
        if (server_connected) {
            mqtt.publish("test/server/recv", "1");
        }
    });
    client.connect(HOST, PORT, 1000);

    // Create second mqtt_client
    mqtt::mqtt_client server(2000);
    server.set_callback_connect([&](auto& mqtt, auto, auto, auto const&) {
        server_connected = true;
        mqtt.subscribe("test/server/recv/#", make_rx_callback(mqtt, "test/client/recv"),
                       mqtt::mosquitto_cpp::QoS::at_most_once);
        if (client_connected) {
            mqtt.publish("test/client/recv", "1");
        }
    });
    server.set_error_handler(make_error_cb(server));

    server.connect(HOST, PORT, 1000);

    // Create event handler and register clients
    fd_event_handler ev_handler;
    ev_handler.register_event_handler(&client);
    ev_handler.register_event_handler(&server);

    std::atomic_bool running = true;
    ev_handler.run(running);

    return 0;
}
