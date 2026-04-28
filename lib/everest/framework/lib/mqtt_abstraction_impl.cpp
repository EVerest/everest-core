// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <thread>

#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fmt/format.h>

#include <everest/exceptions.hpp>
#include <everest/io/mqtt/mosquitto_cpp.hpp>
#include <everest/io/mqtt/mqtt_client.hpp>
#include <everest/logging.hpp>

#include <utils/mqtt_abstraction_impl.hpp>

namespace Everest {
constexpr auto mqtt_keep_alive = 600;
constexpr auto mqtt_get_timeout_ms = 5000;       ///< Timeout for MQTT get in milliseconds
constexpr auto mqtt_reconnect_timeout_ms = 2000; ///< MQTT reconnect timeout

namespace {
everest::lib::io::mqtt::mqtt_client::QoS to_io_qos(Everest::QOS qos,
                                                   everest::lib::io::mqtt::mqtt_client::QoS default_qos) {
    switch (qos) {
    case QOS::QOS0:
        return everest::lib::io::mqtt::mqtt_client::QoS::at_most_once;
        break;
    case QOS::QOS1:
        return everest::lib::io::mqtt::mqtt_client::QoS::at_least_once;
        break;
    case QOS::QOS2:
        return everest::lib::io::mqtt::mqtt_client::QoS::exactly_once;
        break;

    default:
        break;
    }
    return default_qos;
}
} // namespace

MessageWithQOS::MessageWithQOS(const std::string& topic, const std::string& payload, QOS qos, bool retain) :
    Message{topic, payload}, qos(qos), retain(retain) {
}

MQTTAbstractionImpl::MQTTAbstractionImpl(const MQTTSettings& mqtt_settings) :
    message_queue(([this](const Message& message) { this->on_mqtt_message(message); })),
    mqtt_everest_prefix(mqtt_settings.everest_prefix),
    mqtt_external_prefix(mqtt_settings.external_prefix),
    running(true) {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << "Initializing MQTT abstraction layer...";

    if (mqtt_settings.uses_socket()) {
        this->mqtt_server_socket_path = mqtt_settings.broker_socket_path;
    } else {
        this->mqtt_server_address = mqtt_settings.broker_host;
        this->mqtt_server_port = mqtt_settings.broker_port;
    }

    this->mqtt_is_connected = false;

    this->mqtt_client = std::make_unique<everest::lib::io::mqtt::mqtt_client>(mqtt_reconnect_timeout_ms);
    this->mqtt_client->set_error_handler([](int error, std::string const& msg) {
        if (error) {
            EVLOG_error << fmt::format("MQTT error: {}", msg);
        }
    });
    this->mqtt_client->set_callback_connect([this](auto& mqtt, auto, auto, auto const&) { this->on_mqtt_connect(); });
    this->mqtt_client->set_callback_disconnect([this](auto& mqtt, auto, auto const&) { this->on_mqtt_disconnect(); });
}

MQTTAbstractionImpl::~MQTTAbstractionImpl() {
    // Ensure the event loop thread is stopped before members are destroyed.
    // Otherwise `Thread` may try to join while dependencies used by the thread
    // (event handler/event fd/mqtt client) are already being torn down.
    disconnect();
    mqtt_mainloop_thread.stop();
}

bool MQTTAbstractionImpl::connect() {
    BOOST_LOG_FUNCTION();

    if (this->mqtt_is_connected) {
        return true;
    }

    if (!this->mqtt_server_socket_path.empty()) {
        EVLOG_debug << fmt::format("Connecting to MQTT broker: {}", this->mqtt_server_socket_path);
        const auto result = this->mqtt_client->connect(this->mqtt_server_socket_path, mqtt_keep_alive);
        return (result == everest::lib::io::mqtt::ErrorCode::Success);
    } else {
        EVLOG_debug << fmt::format("Connecting to MQTT broker: {}:{}", this->mqtt_server_address,
                                   this->mqtt_server_port);
        try {
            const auto result =
                this->mqtt_client->connect("", this->mqtt_server_address, this->mqtt_server_port, mqtt_keep_alive);
            return (result == everest::lib::io::mqtt::ErrorCode::Success);
        } catch (std::exception& e) {
            EVLOG_critical << fmt::format("Could not connect to MQTT broker: {}", e.what());
        }

        return false;
    }
}

void MQTTAbstractionImpl::disconnect() {
    BOOST_LOG_FUNCTION();

    this->running = false;
    this->disconnect_event.notify();
    if (this->mqtt_client) {
        this->mqtt_client->disconnect();
    }

    // FIXME(kai): always set connected to false for the moment
    this->mqtt_is_connected = false;
}

void MQTTAbstractionImpl::publish(const std::string& topic, const json& json) {
    BOOST_LOG_FUNCTION();

    publish(topic, json, QOS::QOS2);
}

void MQTTAbstractionImpl::publish(const std::string& topic, const json& json, QOS qos, bool retain) {
    BOOST_LOG_FUNCTION();

    publish(topic, json.dump(), qos, retain);
}

void MQTTAbstractionImpl::publish(const std::string& topic, const std::string& data) {
    BOOST_LOG_FUNCTION();

    publish(topic, data, QOS::QOS0);
}

void MQTTAbstractionImpl::publish(const std::string& topic, const std::string& data, QOS qos, bool retain) {
    BOOST_LOG_FUNCTION();

    if (topic.empty()) {
        return;
    }

    auto mqtt_qos = to_io_qos(qos, everest::lib::io::mqtt::mqtt_client::QoS::at_most_once);

    if (retain) {
        if (not(data.empty() and qos == QOS::QOS0)) {
            // topic should be retained, so save the topic in retained_topics
            // do not save the topic when the payload is empty and QOS is set to 0 which means a retained topic is to be
            // cleared
            const std::lock_guard<std::mutex> lock(topics_mutex);
            this->retained_topics.push_back(topic);
        }
    }

    if (!this->mqtt_is_connected) {
        const std::lock_guard<std::mutex> lock(messages_before_connected_mutex);
        this->messages_before_connected.push_back(std::make_shared<MessageWithQOS>(topic, data, qos, retain));
        return;
    }

    const auto error = this->mqtt_client->publish(topic, data, mqtt_qos, retain, {});
    if (error != everest::lib::io::mqtt::ErrorCode::Success) {
        EVLOG_error << "MQTT error during publishing";
    }

    EVLOG_verbose << fmt::format("publishing to topic: {} with payload: {} and qos: {} and retain: {}", topic, data,
                                 static_cast<int>(qos), static_cast<int>(retain));
}

void MQTTAbstractionImpl::subscribe(const std::string& topic) {
    BOOST_LOG_FUNCTION();

    subscribe(topic, QOS::QOS2);
}

void MQTTAbstractionImpl::subscribe(const std::string& topic, QOS qos) {
    BOOST_LOG_FUNCTION();
    const std::lock_guard<std::mutex> lock(topics_mutex);

    auto max_qos_level = to_io_qos(qos, everest::lib::io::mqtt::mqtt_client::QoS::at_most_once);

    this->subscribed_topics.insert(topic);

    this->ev_handler.add_action([this, topic, max_qos_level]() {
        const auto result = this->mqtt_client->subscribe(
            topic,
            [this, topic](everest::lib::io::mqtt::mosquitto_cpp& client,
                          everest::lib::io::mqtt::mosquitto_cpp::message const& message) {
                this->message_queue.add(std::make_unique<Message>(topic, message.payload));
            },
            max_qos_level);
    });
}

void MQTTAbstractionImpl::unsubscribe(const std::string& topic) {
    BOOST_LOG_FUNCTION();
    const std::lock_guard<std::mutex> lock(topics_mutex);

    if (this->subscribed_topics.find(topic) == this->subscribed_topics.end()) {
        EVLOG_warning << fmt::format("Tried to unsubscribe from topic {} but it was not subscribed", topic);
        return;
    }

    EVLOG_verbose << fmt::format("Unsubscribing from topic: {}", topic);

    this->subscribed_topics.erase(topic);
    this->ev_handler.add_action([this, topic]() { this->mqtt_client->unsubscribe(topic, {}); });
}

void MQTTAbstractionImpl::clear_retained_topics() {
    BOOST_LOG_FUNCTION();
    const std::lock_guard<std::mutex> lock(topics_mutex);

    for (const auto& retained_topic : retained_topics) {
        this->publish(retained_topic, std::string(), QOS::QOS0, true);
        EVLOG_verbose << "Cleared retained topic: " << retained_topic;
    }

    retained_topics.clear();
}

json MQTTAbstractionImpl::get(const MQTTRequest& request, std::size_t retries) {
    BOOST_LOG_FUNCTION();
    std::size_t attempt = 0;
    while (attempt <= retries) {
        try {
            return this->get_internal(request);
        } catch (const EverestTimeoutError& error) {
            if (attempt < retries) {
                attempt += 1;
            } else {
                std::rethrow_exception(std::current_exception());
            }
        }
    }
    EVLOG_AND_THROW(
        EverestInternalError(fmt::format("Unknown error while waiting for result of get({})", request.response_topic)));
}

std::unique_ptr<MQTTAbstraction> make_mqtt_abstraction(const MQTTSettings& mqtt_settings) {
    return std::make_unique<MQTTAbstractionImpl>(mqtt_settings);
}

json MQTTAbstractionImpl::get(const std::string& topic, QOS qos, std::size_t retries) {
    BOOST_LOG_FUNCTION();
    const MQTTRequest request = {topic, qos};
    return this->get(request, retries);
}

const std::string& MQTTAbstractionImpl::get_everest_prefix() const {
    BOOST_LOG_FUNCTION();
    return mqtt_everest_prefix;
}

const std::string& MQTTAbstractionImpl::get_external_prefix() const {
    BOOST_LOG_FUNCTION();
    return mqtt_external_prefix;
}

nlohmann::json MQTTAbstractionImpl::get_internal(const MQTTRequest& request) {
    BOOST_LOG_FUNCTION();
    std::lock_guard<std::mutex> lock(topic_request_mutex);

    std::promise<json> res_promise;
    std::future<json> res_future = res_promise.get_future();

    const auto res_handler = [&res_promise](const std::string& /*topic*/, json response) {
        res_promise.set_value(std::move(response));
    };

    // FIXME: use configurable HandlerType?
    const std::shared_ptr<TypedHandler> res_token =
        std::make_shared<TypedHandler>(HandlerType::GetConfigResponse, std::make_shared<Handler>(res_handler));
    this->register_handler(request.response_topic, res_token, request.qos);
    if (request.request_topic.has_value()) {
        json req_data;
        if (request.request_data.has_value()) {
            req_data = json::parse(request.request_data.value());
        }

        MqttMessagePayload payload{MqttMessageType::GetConfig, req_data};
        this->publish(request.request_topic.value(), payload, request.qos);
    }
    // wait for result future
    const std::chrono::time_point<std::chrono::steady_clock> res_wait =
        std::chrono::steady_clock::now() + request.timeout;
    std::future_status res_future_status = std::future_status::deferred;
    do {
        res_future_status = res_future.wait_until(res_wait);
    } while (res_future_status == std::future_status::deferred);

    json result;
    if (res_future_status == std::future_status::timeout) {
        this->unregister_handler(request.response_topic, res_token);
        EVLOG_AND_THROW(
            EverestTimeoutError(fmt::format("Timeout while waiting for result of get({})", request.response_topic)));
    }
    if (res_future_status == std::future_status::ready) {
        result = res_future.get();
    }

    this->unregister_handler(request.response_topic, res_token);

    return result;
}

std::shared_future<void> MQTTAbstractionImpl::spawn_main_loop_thread() {
    BOOST_LOG_FUNCTION();

    std::packaged_task<void(void)> task([this]() {
        try {
            this->ev_handler.register_event_handler(this->mqtt_client.get());
            this->ev_handler.register_event_handler(&this->disconnect_event,
                                                    [this](const auto&) { this->running = false; });

            this->ev_handler.run(this->running);
        } catch (boost::exception& e) {
            EVLOG_critical << fmt::format("Caught MQTT mainloop boost::exception:\n{}",
                                          boost::diagnostic_information(e, true));
            exit(1);
        } catch (std::exception& e) {
            EVLOG_critical << fmt::format("Caught MQTT mainloop std::exception:\n{}",
                                          boost::diagnostic_information(e, true));
            exit(1);
        }
    });

    this->main_loop_future = task.get_future();
    this->mqtt_mainloop_thread = std::thread(std::move(task));
    return this->main_loop_future;
}

std::shared_future<void> MQTTAbstractionImpl::get_main_loop_future() {
    BOOST_LOG_FUNCTION();
    return this->main_loop_future;
}

void MQTTAbstractionImpl::on_mqtt_message(const Message& message) {
    BOOST_LOG_FUNCTION();

    EVLOG_verbose << "Incoming MQTT message. topic: " << message.topic << " payload: " << message.payload;

    const auto& topic = message.topic;
    const auto& payload = message.payload;

    try {
        json data;
        bool is_everest_topic = false;
        if (topic.find(mqtt_everest_prefix) == 0) {
            EVLOG_verbose << fmt::format("topic {} starts with {}", topic, mqtt_everest_prefix);
            is_everest_topic = true;
            try {
                data = json::parse(payload);
            } catch (nlohmann::detail::parse_error& e) {
                EVLOG_warning << fmt::format("Could not decode json for incoming topic '{}': {}", topic, payload);
                return;
            }
        } else {
            EVLOG_debug << fmt::format("Message parsing for topic '{}' not implemented. Wrapping in json object.",
                                       topic);
            data = json(payload);
        }

        bool found = false;

        this->message_handler.add(ParsedMessage{topic, std::move(data)});
    } catch (boost::exception& e) {
        EVLOG_critical << fmt::format("Caught MQTT on_message boost::exception:\n{}",
                                      boost::diagnostic_information(e, true));
        exit(1);
    } catch (std::exception& e) {
        EVLOG_critical << fmt::format("Caught MQTT on_message std::exception:\n{}",
                                      boost::diagnostic_information(e, true));
        exit(1);
    }
}

void MQTTAbstractionImpl::on_mqtt_connect() {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << "Connected to MQTT broker";

    // this will allow new handlers to subscribe directly, if needed
    {
        const std::lock_guard<std::mutex> lock(messages_before_connected_mutex);
        this->mqtt_is_connected = true;
        for (auto& message : this->messages_before_connected) {
            this->publish(message->topic, message->payload, message->qos, message->retain);
        }
        this->messages_before_connected.clear();
    }
}

void MQTTAbstractionImpl::on_mqtt_disconnect() {
    BOOST_LOG_FUNCTION();

    EVLOG_AND_THROW(EverestInternalError("Lost connection to MQTT broker"));
}

void MQTTAbstractionImpl::register_handler(const std::string& topic, std::shared_ptr<TypedHandler> handler, QOS qos) {
    BOOST_LOG_FUNCTION();

    auto subscription_required = [this](const std::string& topic) {
        const std::lock_guard<std::mutex> lock(topics_mutex);
        return std::find(this->subscribed_topics.begin(), this->subscribed_topics.end(), topic) ==
               this->subscribed_topics.end();
    };

    this->message_handler.register_handler(topic, handler);

    if (subscription_required(topic)) {
        EVLOG_verbose << fmt::format("Subscribing to {}", topic);
        this->subscribe(topic, qos);
    }
}

void MQTTAbstractionImpl::unregister_handler(const std::string& topic, const Token& token) {
    BOOST_LOG_FUNCTION();

    EVLOG_verbose << fmt::format("Unregistering handler {} for {}", fmt::ptr(&token), topic);

    if (this->mqtt_is_connected) {
        this->unsubscribe(topic);
    }
}

} // namespace Everest
