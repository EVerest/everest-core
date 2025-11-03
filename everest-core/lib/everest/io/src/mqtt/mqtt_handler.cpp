// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "everest/io/mqtt/dataset.hpp"
#include <everest/io/mqtt/mqtt_handler.hpp>
#include <everest/io/socket/socket.hpp>
#include <functional>
#include <mqtt.h>
#include <stdexcept>

namespace everest::lib::io::mqtt {

namespace {
auto get_string_topic(mqtt_response_publish const& published) {
    const auto ptr = reinterpret_cast<const char*>(published.topic_name);
    return std::string(ptr, ptr + published.topic_name_size);
}

auto get_string_message(mqtt_response_publish const& published) {
    const auto ptr = reinterpret_cast<const char*>(published.application_message);
    return std::string(ptr, ptr + published.application_message_size);
}

bool is_message(mqtt_response_publish const& published) {
    return published.topic_name_size > 0;
}

bool contains_newline(std::string const& str) {
    return str.find('\n') != std::string::npos;
}

} // namespace

class MqttClientHandle {
public:
    using RxCallback = std::function<void(mqtt_response_publish&)>;

    MqttClientHandle(RxCallback callback, int socket, std::size_t rx_size, std::size_t tx_size) :
        m_callback(callback), m_rx_buffer(rx_size), m_tx_buffer(tx_size) {

        m_client.publish_response_callback_state = this;

        mqtt_init(&m_client, socket, m_tx_buffer.data(), m_tx_buffer.size(), m_rx_buffer.data(), m_rx_buffer.size(),
                  rx_trampoline);

        uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;

        mqtt_connect(&m_client, "", nullptr, nullptr, 0, nullptr, nullptr, connect_flags, 400);

        if (m_client.error != MQTT_OK) {
            throw std::runtime_error(std::string("mqtt-c error: ") + mqtt_error_str(m_client.error));
        }
    }

    // NOTE (aw): putting the mqtt_client into unique_ptr (with a custom
    // deleter that executes mqtt_disconnect) and using unique_ptr
    // arrays for the buffers would make this class also default
    // moveable
    MqttClientHandle(const MqttClientHandle&) = delete;
    MqttClientHandle(MqttClientHandle&&) = delete;
    MqttClientHandle& operator=(const MqttClientHandle&) = delete;
    MqttClientHandle& operator=(MqttClientHandle&&) = delete;

    ~MqttClientHandle() {
        mqtt_disconnect(&m_client);
    }

    mqtt_client* get() {
        return &m_client;
    }

private:
    static void rx_trampoline(void** instance, mqtt_response_publish* published) {
        if (not instance) {
            return;
        }

        auto client = *reinterpret_cast<MqttClientHandle**>(instance);
        client->m_callback(*published);
    }

    RxCallback m_callback{nullptr};

    std::vector<std::uint8_t> m_rx_buffer;
    std::vector<std::uint8_t> m_tx_buffer;

    mqtt_client m_client{};
};

mqtt_handler::mqtt_handler() = default;
mqtt_handler::~mqtt_handler() = default;

bool mqtt_handler::open(std::string const& server, std::uint16_t port) {
    try {
        m_socket = socket::open_tcp_socket(server, port);
        socket::set_non_blocking(m_socket);
        socket::enable_tcp_no_delay(m_socket);

        auto cb = [this](mqtt_response_publish& dataset) {
            rx_data_received = is_message(dataset);
            if (rx_buffer && rx_data_received) {
                rx_buffer->message = get_string_message(dataset);
                rx_buffer->topic = get_string_topic(dataset);
                rx_buffer->qos = Qos(dataset.qos_level);
            }
        };

        m_handle = std::make_unique<MqttClientHandle>(std::move(cb), m_socket, 8 * 1024, 8 * 1024);
        mqtt_sync();
    } catch (...) {
        return false;
    }
    return true;
}

bool mqtt_handler::tx(PayloadT const& data) {
    if (not m_handle) {
        return false;
    }
    if (contains_newline(data.topic)) {
        return false;
    }
    auto result = mqtt_publish(m_handle->get(), data.topic.c_str(), data.message.c_str(), data.message.size(),
                               static_cast<int>(data.qos)) == MQTT_OK;
    return mqtt_sync() && result;
}

bool mqtt_handler::rx(PayloadT& data) {
    rx_buffer = &data;
    auto result = mqtt_sync();
    rx_buffer = nullptr;
    return result;
}

int mqtt_handler::get_fd() const {
    return m_socket;
}

int mqtt_handler::get_error() const {
    return socket::get_pending_error(m_socket);
}

bool mqtt_handler::subscribe(std::string const& topic) {
    if (not m_handle) {
        return false;
    }
    if (contains_newline(topic)) {
        return false;
    }
    auto result = mqtt_subscribe(m_handle->get(), topic.c_str(), 0) == MQTT_OK;
    return mqtt_sync() && result;
}

bool mqtt_handler::unsubscribe(std::string const& topic) {
    if (not m_handle) {
        return false;
    }
    if (contains_newline(topic)) {
        return false;
    }
    auto result = mqtt_unsubscribe(m_handle->get(), topic.c_str()) == MQTT_OK;
    return mqtt_sync() && result;
}

bool mqtt_handler::mqtt_sync() {
    if (not m_handle) {
        return false;
    }
    return ::mqtt_sync(m_handle->get()) == MQTT_OK;
}

bool mqtt_handler::ping() {
    if (not m_handle) {
        return false;
    }
    auto result = mqtt_ping(m_handle->get()) == MQTT_OK;
    return mqtt_sync() && result;
}

bool mqtt_handler::reset_current_item_is_data() {
    auto result = rx_data_received;
    rx_data_received = false;
    return result;
}

} // namespace everest::lib::io::mqtt
