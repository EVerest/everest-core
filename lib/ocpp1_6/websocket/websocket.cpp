// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/types.hpp>
#include <ocpp1_6/websocket/websocket.hpp>

namespace ocpp1_6 {

Websocket::Websocket(std::shared_ptr<ChargePointConfiguration> configuration, int32_t security_profile) {

    auto uri = configuration->getCentralSystemURI();

    this->log_messages = configuration->getLogMessages();
    if (this->log_messages) {
        std::string output_file_name = "/tmp/libocpp_messages_" + DateTime().to_rfc3339() + ".txt";
        this->output_file.open(output_file_name);
    }

    if (security_profile <= 1) {
        EVLOG_debug << "Creating plaintext websocket based on the provided URI: " << uri;
        this->websocket = std::make_unique<WebsocketPlain>(configuration);
    } else if (security_profile >= 2) {
        EVLOG_debug << "Creating TLS websocket based on the provided URI: " << uri;
        this->websocket = std::make_unique<WebsocketTLS>(configuration);
    }
}

Websocket::~Websocket() {
    if (this->log_messages) {
        this->output_file.close();
    }
}

bool Websocket::connect(int32_t security_profile) {
    return this->websocket->connect(security_profile);
}

void Websocket::disconnect(websocketpp::close::status::value code) {
    this->websocket->disconnect(code);
}

void Websocket::reconnect(std::error_code reason, long delay) {
    this->websocket->reconnect(reason, delay);
}

void Websocket::register_connected_callback(const std::function<void()>& callback) {
    this->connected_callback = callback;
    this->websocket->register_connected_callback(callback);
}
void Websocket::register_disconnected_callback(const std::function<void()>& callback) {
    this->disconnected_callback = callback;
    this->websocket->register_disconnected_callback(callback);
}
void Websocket::register_message_callback(const std::function<void(const std::string& message)>& callback) {
    this->message_callback = callback;

    this->websocket->register_message_callback([this](const std::string& message) {
        if (this->log_messages) {
            {
                std::lock_guard<std::mutex> lock(this->output_file_mutex);
                this->output_file << "Received @ " << DateTime().to_rfc3339() << ":\n" << message << std::endl;
            }
        }
        this->message_callback(message);
    });
}

void Websocket::register_sign_certificate_callback(const std::function<void()>& callback) {
    this->sign_certificate_callback = callback;
    this->websocket->register_sign_certificate_callback(callback);
}

bool Websocket::send(const std::string& message) {
    if (this->log_messages) {
        {
            std::lock_guard<std::mutex> lock(this->output_file_mutex);
            this->output_file << "Sending @ " << DateTime().to_rfc3339() << ":\n" << message << std::endl;
        }
    }
    return this->websocket->send(message);
}

} // namespace ocpp1_6
