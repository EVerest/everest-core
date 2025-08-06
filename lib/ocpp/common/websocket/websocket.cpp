// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <ocpp/common/websocket/websocket.hpp>
#include <ocpp/v16/types.hpp>

#include <ocpp/common/websocket/websocket_libwebsockets.hpp>

#include <boost/algorithm/string.hpp>

using json = nlohmann::json;

namespace ocpp {

Websocket::Websocket(const WebsocketConnectionOptions& connection_options, std::shared_ptr<EvseSecurity> evse_security,
                     std::shared_ptr<MessageLogging> logging) :
    logging(logging) {
    this->websocket = std::make_unique<WebsocketLibwebsockets>(connection_options, evse_security);
}

Websocket::~Websocket() {
}

bool Websocket::start_connecting() {
    this->logging->sys("Connecting");
    return this->websocket->start_connecting();
}

void Websocket::set_connection_options(const WebsocketConnectionOptions& connection_options) {
    this->websocket->set_connection_options(connection_options);
}

void Websocket::disconnect(const WebsocketCloseReason code) {
    this->logging->sys("Disconnecting");
    this->websocket->disconnect(code);
}

void Websocket::reconnect(long delay) {
    this->logging->sys("Reconnecting");
    this->websocket->reconnect(delay);
}

bool Websocket::is_connected() {
    return this->websocket->is_connected();
}

void Websocket::register_connected_callback(const std::function<void(OcppProtocolVersion protocol)>& callback) {
    this->connected_callback = callback;

    this->websocket->register_connected_callback([this](OcppProtocolVersion protocol) {
        this->logging->sys("Connected");
        this->connected_callback(protocol);
    });
}

void Websocket::register_disconnected_callback(const std::function<void()>& callback) {
    this->disconnected_callback = callback;

    this->websocket->register_disconnected_callback([this]() {
        this->logging->sys("Disconnected");
        this->disconnected_callback();
    });
}

void Websocket::register_stopped_connecting_callback(
    const std::function<void(const WebsocketCloseReason reason)>& callback) {
    this->stopped_connecting_callback = callback;
    this->websocket->register_stopped_connecting_callback(
        [this](const WebsocketCloseReason reason) { this->stopped_connecting_callback(reason); });
}

void Websocket::register_message_callback(const std::function<void(const std::string& message)>& callback) {
    this->message_callback = callback;

    this->websocket->register_message_callback([this](const std::string& message) { this->message_callback(message); });
}

void Websocket::register_connection_failed_callback(const std::function<void(ConnectionFailedReason)>& callback) {
    this->websocket->register_connection_failed_callback(callback);
}

bool Websocket::send(const std::string& message) {
    this->logging->charge_point("Unknown", message);
    return this->websocket->send(message);
}

void Websocket::set_websocket_ping_interval(int32_t ping_interval_s, int32_t pong_interval_s) {
    this->logging->sys("WebSocketPingInterval changed");
    this->websocket->set_websocket_ping_interval(ping_interval_s, pong_interval_s);
}

void Websocket::set_authorization_key(const std::string& authorization_key) {
    this->websocket->set_authorization_key(authorization_key);
}

} // namespace ocpp
