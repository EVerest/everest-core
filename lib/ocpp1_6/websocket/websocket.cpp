// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/types.hpp>
#include <ocpp1_6/websocket/websocket.hpp>

#include <boost/algorithm/string.hpp>

using json = nlohmann::json;

namespace ocpp1_6 {

Websocket::Websocket(std::shared_ptr<ChargePointConfiguration> configuration, int32_t security_profile,
                     std::shared_ptr<MessageLogging> logging) :
    configuration(configuration), logging(logging) {
    auto uri = this->configuration->getCentralSystemURI();
    if (security_profile <= 1) {
        EVLOG_debug << "Creating plaintext websocket based on the provided URI: " << uri;
        this->websocket = std::make_unique<WebsocketPlain>(configuration);
    } else if (security_profile >= 2) {
        EVLOG_debug << "Creating TLS websocket based on the provided URI: " << uri;
        this->websocket = std::make_unique<WebsocketTLS>(configuration);
    }
}

Websocket::~Websocket() {
}

bool Websocket::connect(int32_t security_profile) {
    this->logging->sys("Connecting to: " + this->configuration->getCentralSystemURI() +
                       " with security_profile: " + std::to_string(security_profile));
    return this->websocket->connect(security_profile);
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

void Websocket::register_connected_callback(const std::function<void()>& callback) {
    this->connected_callback = callback;

    this->websocket->register_connected_callback([this]() {
        this->logging->sys("Connected");
        this->connected_callback();
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

} // namespace ocpp1_6
