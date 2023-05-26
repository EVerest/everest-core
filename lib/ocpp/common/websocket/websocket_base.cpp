// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <ocpp/common/websocket/websocket_base.hpp>

namespace ocpp {

WebsocketBase::WebsocketBase(const WebsocketConnectionOptions& connection_options) :
    shutting_down(false),
    m_is_connected(false),
    connection_options(connection_options),
    connected_callback(nullptr),
    disconnected_callback(nullptr),
    message_callback(nullptr),
    reconnect_timer(nullptr) {
    this->ping_timer = std::make_unique<Everest::SteadyTimer>();
}

WebsocketBase::~WebsocketBase() {
    this->websocket_thread->detach();
}

void WebsocketBase::register_connected_callback(const std::function<void(const int security_profile)>& callback) {
    this->connected_callback = callback;
}

void WebsocketBase::register_disconnected_callback(const std::function<void()>& callback) {
    this->disconnected_callback = callback;
}

void WebsocketBase::register_message_callback(const std::function<void(const std::string& message)>& callback) {
    this->message_callback = callback;
}

bool WebsocketBase::initialized() {
    if (this->connected_callback == nullptr) {
        EVLOG_error << "Not properly initialized: please register connected callback.";
        return false;
    }
    if (this->disconnected_callback == nullptr) {
        EVLOG_error << "Not properly initialized: please register disconnected callback.";
        return false;
    }
    if (this->message_callback == nullptr) {
        EVLOG_error << "Not properly initialized: please register message callback.";
        return false;
    }

    return true;
}

void WebsocketBase::disconnect(websocketpp::close::status::value code) {
    if (!this->initialized()) {
        EVLOG_error << "Cannot disconnect a websocket that was not initialized";
        return;
    }
    this->shutting_down = true;
    if (this->reconnect_timer) {
        this->reconnect_timer.get()->cancel();
    }
    if (this->ping_timer) {
        this->ping_timer->stop();
    }

    EVLOG_info << "Disconnecting websocket...";
    this->close(code, "");
}

bool WebsocketBase::is_connected() {
    return this->m_is_connected;
}

std::optional<std::string> WebsocketBase::getAuthorizationHeader() {
    std::optional<std::string> auth_header = std::nullopt;
    const auto authorization_key = this->connection_options.authorization_key;
    if (authorization_key) {
        EVLOG_debug << "AuthorizationKey present, encoding authentication header";
        std::string plain_auth_header = this->connection_options.chargepoint_id + ":" + authorization_key.value();
        auth_header.emplace(std::string("Basic ") + websocketpp::base64_encode(plain_auth_header));

        EVLOG_info << "Basic Auth header: " << auth_header.value();
    }

    return auth_header;
}

void WebsocketBase::set_websocket_ping_interval(int32_t interval_s) {
    if (this->ping_timer) {
        this->ping_timer->stop();
    }
    if (interval_s > 0) {
        this->ping_timer->interval([this]() { this->ping(); }, std::chrono::seconds(interval_s));
    }
}

} // namespace ocpp
