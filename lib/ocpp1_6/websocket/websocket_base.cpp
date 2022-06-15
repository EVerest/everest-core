// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/websocket/websocket_base.hpp>

namespace ocpp1_6 {

WebsocketBase::WebsocketBase(std::shared_ptr<ChargePointConfiguration> configuration) :
    shutting_down(false),
    configuration(configuration),
    connected_callback(nullptr),
    disconnected_callback(nullptr),
    message_callback(nullptr) {
}

void WebsocketBase::register_connected_callback(const std::function<void()>& callback) {
    this->connected_callback = callback;
}

void WebsocketBase::register_disconnected_callback(const std::function<void()>& callback) {
    this->disconnected_callback = callback;
}

void WebsocketBase::register_message_callback(const std::function<void(const std::string& message)>& callback) {
    this->message_callback = callback;
}

bool WebsocketBase::initialized() {
    if (this->shutting_down) {
        EVLOG_error << "Not properly initialized: websocket already shutdown.";
        return false;
    }
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

} // namespace ocpp1_6
