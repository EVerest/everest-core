// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/**
 * @example test_mqtt_client.cpp Creates two MQTT clients that ping/pong messages to each other
 */

#include <cstdio>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/mqtt/mqtt_client.hpp>
#include <iostream>
#include <string>
#include <thread>

using namespace everest::lib::io::event;
using namespace everest::lib::io;
using namespace std::chrono_literals;

constexpr auto HOST = "localhost";
constexpr std::uint16_t PORT = 1883;

using payloadT = mqtt::mqtt_client::ClientPayloadT;
using generic_client = typename mqtt::mqtt_client::interface;

// Creates the error handler for the clients
mqtt::mqtt_client::cb_error make_error_cb(mqtt::mqtt_client& client) {
    return [&](int error, std::string const& msg) {
        std::cerr << "ERROR ( " << error << " ): " << msg << std::endl;
        if (error) {
            std::this_thread::sleep_for(1s);
            client.reset();
        }
    };
}

// Creates the RX handler for the client
mqtt::mqtt_client::cb_rx make_rx_callback(mqtt::mqtt_client& client, std::string tar) {
    return [&, tar](payloadT const& data, generic_client& tx) {
        std::cout << &client << "\n"
                  << "topic: " << data.topic << "\n"
                  << "msg:   " << data.message << std::endl;
        payloadT dataset;
        auto c = std::stoi(data.message) + 1;
        dataset.message = std::to_string(c);
        dataset.topic = tar;
        tx.tx(dataset);
    };
}

int main(int, char*[]) {
    std::cout << "mqtt_client ping/pong demonstration" << std::endl;

    // Create first mqtt_client
    mqtt::mqtt_client client(HOST, PORT);
    // Create and assign error handler
    client.set_error_handler(make_error_cb(client));
    // Create and assign rx handler
    client.set_message_handler(make_rx_callback(client, "test/server/recv"));
    // Subscribe to topic
    client.subscribe("test/client/recv/#");

    // Create second mqtt_client
    mqtt::mqtt_client server(HOST, PORT);
    server.set_error_handler(make_error_cb(server));
    server.set_message_handler(make_rx_callback(server, "test/client/recv"));
    server.subscribe("test/server/recv/#");

    // Create event handler and register clients
    fd_event_handler ev_handler;
    ev_handler.register_event_handler(&client);
    ev_handler.register_event_handler(&server);

    // Send first message to trigger ping/pong
    client.tx({"test/server/recv", "1"});
    // event loop
    while (true) {
        // Handles all the work and blocks if there is nothing to do.
        ev_handler.poll();
    }

    return 0;
}
