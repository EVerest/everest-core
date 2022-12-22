// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <ocpp/common/websocket/websocket.hpp>
#include <ocpp/v16/types.hpp>

#include <boost/algorithm/string.hpp>

using json = nlohmann::json;

namespace ocpp {

Websocket::Websocket(const WebsocketConnectionOptions& connection_options, std::shared_ptr<PkiHandler> pki_handler,
                     std::shared_ptr<MessageLogging> logging) :
    logging(logging) {
    if (connection_options.security_profile <= 1) {
        EVLOG_debug << "Creating plaintext websocket based on the provided URI: " << connection_options.cs_uri;
        this->websocket = std::make_unique<WebsocketPlain>(connection_options);
    } else if (connection_options.security_profile >= 2) {
        EVLOG_debug << "Creating TLS websocket based on the provided URI: " << connection_options.cs_uri;
        this->websocket = std::make_unique<WebsocketTLS>(connection_options, pki_handler);
    }
}

Websocket::~Websocket() {
}

bool Websocket::connect(int32_t security_profile, bool try_once) {
    return this->websocket->connect(security_profile, try_once);
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
void Websocket::register_message_callback(const std::function<void(const std::string& message)>& callback) {
    this->message_callback = callback;

    this->websocket->register_message_callback([this](const std::string& message) { this->message_callback(message); });
}

void Websocket::register_sign_certificate_callback(const std::function<void()>& callback) {
    this->sign_certificate_callback = callback;
    this->websocket->register_sign_certificate_callback(callback);
}

bool Websocket::send(const std::string& message) {
    this->logging->charge_point("Unknown", message);
    return this->websocket->send(message);
}

} // namespace ocpp
