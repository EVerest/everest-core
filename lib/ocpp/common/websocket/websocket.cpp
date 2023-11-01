// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <ocpp/common/websocket/websocket.hpp>
#include <ocpp/v16/types.hpp>

#include <boost/algorithm/string.hpp>

using json = nlohmann::json;

namespace ocpp {

Websocket::Websocket(const WebsocketConnectionOptions& connection_options, std::shared_ptr<EvseSecurity> evse_security,
                     std::shared_ptr<MessageLogging> logging) :
    logging(logging) {
    if (connection_options.security_profile <= 1) {
        EVLOG_debug << "Creating plaintext websocket based on the provided URI: " << connection_options.cs_uri;
        this->websocket = std::make_unique<WebsocketPlain>(connection_options);
    } else if (connection_options.security_profile >= 2) {
        EVLOG_debug << "Creating TLS websocket based on the provided URI: " << connection_options.cs_uri;
        this->websocket = std::make_unique<WebsocketTLS>(connection_options, evse_security);
    }
}

Websocket::~Websocket() {
}

bool Websocket::connect() {
    this->logging->sys("Connecting");
    return this->websocket->connect();
}

void Websocket::set_connection_options(const WebsocketConnectionOptions& connection_options) {
    this->websocket->set_connection_options(connection_options);
}

void Websocket::disconnect(websocketpp::close::status::value code) {
    this->logging->sys("Disconnecting");
    this->websocket->disconnect(code);
}

void Websocket::reconnect(std::error_code reason, long delay) {
    this->logging->sys("Reconnecting");
    this->websocket->reconnect(reason, delay);
}

bool Websocket::is_connected() {
    return this->websocket->is_connected();
}

void Websocket::register_connected_callback(const std::function<void(const int security_profile)>& callback) {
    this->connected_callback = callback;

    this->websocket->register_connected_callback([this](const int security_profile) {
        this->logging->sys("Connected");
        this->connected_callback(security_profile);
    });
}

void Websocket::register_disconnected_callback(const std::function<void()>& callback) {
    this->disconnected_callback = callback;

    this->websocket->register_disconnected_callback([this]() {
        this->logging->sys("Disconnected");
        this->disconnected_callback();
    });
}

void Websocket::register_closed_callback(
    const std::function<void(const websocketpp::close::status::value reason)>& callback) {
    this->closed_callback = callback;
    this->websocket->register_closed_callback(
        [this](const websocketpp::close::status::value reason) { this->closed_callback(reason); });
}

void Websocket::register_message_callback(const std::function<void(const std::string& message)>& callback) {
    this->message_callback = callback;

    this->websocket->register_message_callback([this](const std::string& message) { this->message_callback(message); });
}

bool Websocket::send(const std::string& message) {
    this->logging->charge_point("Unknown", message);
    return this->websocket->send(message);
}

void Websocket::set_websocket_ping_interval(int32_t interval_s) {
    this->logging->sys("WebsocketPingInterval changed");
    this->websocket->set_websocket_ping_interval(interval_s);
}

void Websocket::set_authorization_key(const std::string& authorization_key) {
    this->websocket->set_authorization_key(authorization_key);
}

} // namespace ocpp
