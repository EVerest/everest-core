// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/websocket/websocket.hpp>

namespace ocpp1_6 {

Websocket::Websocket(std::shared_ptr<ChargePointConfiguration> configuration) : shutting_down(false) {

    auto uri = configuration->getCentralSystemURI();
    if (uri.find("ws://") == 0) {
        EVLOG(debug) << "Creating plaintext websocket based on the provided URI: " << uri;
        this->websocket = std::make_unique<WebsocketPlain>(configuration);
    }
    if (uri.find("wss://") == 0) {
        EVLOG(debug) << "Creating TLS websocket based on the provided URI: " << uri;
        this->websocket = std::make_unique<WebsocketTLS>(configuration);
    }
}

bool Websocket::connect() {
    return this->websocket->connect();
}

void Websocket::disconnect() {
    this->shutting_down = true; // FIXME(kai): this makes the websocket inoperable after a disconnect, however this
                                // might not be a bad thing.

    this->websocket->disconnect();
}
void Websocket::register_connected_callback(const std::function<void()>& callback) {
    this->websocket->register_connected_callback(callback);
}
void Websocket::register_disconnected_callback(const std::function<void()>& callback) {
    this->websocket->register_disconnected_callback(callback);
}
void Websocket::register_message_callback(const std::function<void(const std::string& message)>& callback) {
    this->websocket->register_message_callback(callback);
}

bool Websocket::send(const std::string& message) {
    return this->websocket->send(message);
}

} // namespace ocpp1_6
