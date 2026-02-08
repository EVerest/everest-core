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
const auto mqtt_keep_alive = 600;
const auto mqtt_get_timeout_ms = 5000;       ///< Timeout for MQTT get in milliseconds
const auto mqtt_reconnect_timeout_ms = 2000; ///< MQTT reconnect timeout

MessageWithQOS::MessageWithQOS(const std::string& topic, const std::string& payload, QOS qos, bool retain) :
    Message{topic, payload}, qos(qos), retain(retain) {
}

MQTTAbstractionImpl::MQTTAbstractionImpl(const std::string& mqtt_server_address, const std::string& mqtt_server_port,
                                         const std::string& mqtt_everest_prefix,
                                         const std::string& mqtt_external_prefix) :
    message_queue(([this](const Message& message) { this->on_mqtt_message(message); })),
    mqtt_server_address(mqtt_server_address),
    mqtt_server_port(mqtt_server_port),
    mqtt_everest_prefix(mqtt_everest_prefix),
    mqtt_external_prefix(mqtt_external_prefix) {
    BOOST_LOG_FUNCTION();

    init();
}

MQTTAbstractionImpl::MQTTAbstractionImpl(const std::string& mqtt_server_socket_path,
                                         const std::string& mqtt_everest_prefix,
                                         const std::string& mqtt_external_prefix) :
    message_queue(([this](const Message& message) { this->on_mqtt_message(message); })),
    mqtt_server_socket_path(mqtt_server_socket_path),
    mqtt_everest_prefix(mqtt_everest_prefix),
    mqtt_external_prefix(mqtt_external_prefix) {
    BOOST_LOG_FUNCTION();

    init();
}

MQTTAbstractionImpl::~MQTTAbstractionImpl() {
    // FIXME (aw): verify that disconnecting is thread-safe!
    if (this->mqtt_is_connected) {
        disconnect();
    }
    // this->mqtt_mainloop_thread.join();
}

void MQTTAbstractionImpl::init() {
    {
        EVLOG_debug << "Initializing MQTT abstraction layer...";

        this->mqtt_is_connected = false;

        this->mqtt_client = std::make_unique<everest::lib::io::mqtt::mqtt_client>(mqtt_reconnect_timeout_ms);
        this->mqtt_client->set_error_handler([&](int error, std::string const& msg) {
            if (error) {
                EVLOG_error << fmt::format("MQTT error: {}", msg);
            }
        });
        this->mqtt_client->set_callback_connect(
            [this](auto& mqtt, auto, auto, auto const&) { this->on_mqtt_connect(); });
    }
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
        EVLOG_info << fmt::format("Connecting to MQTT broker: {}:{}", this->mqtt_server_address,
                                  this->mqtt_server_port);
        try {
            const auto port_uint = std::stoul(this->mqtt_server_port);
            if (port_uint > std::numeric_limits<std::uint16_t>::max()) {
                EVLOG_critical << fmt::format("Could not connect to MQTT broker, port {} is out of range.", port_uint);
                return false;
            }
            // FIXME: find out a good default for the first paramter, the "bind_address"
            const auto result = this->mqtt_client->connect("127.0.0.1", this->mqtt_server_address,
                                                           static_cast<std::uint16_t>(port_uint), mqtt_keep_alive);
            return (result == everest::lib::io::mqtt::ErrorCode::Success);
        } catch (std::exception& e) {
            EVLOG_critical << fmt::format("Could not connect to MQTT broker: {}", e.what());
        }

        return false;
    }
}

void MQTTAbstractionImpl::disconnect() {
    BOOST_LOG_FUNCTION();

    this->mqtt_client->disconnect();
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

    auto mqtt_qos = everest::lib::io::mqtt::mqtt_client::QoS::at_most_once;
    switch (qos) {
    case QOS::QOS0:
        mqtt_qos = everest::lib::io::mqtt::mqtt_client::QoS::at_most_once;
        break;
    case QOS::QOS1:
        mqtt_qos = everest::lib::io::mqtt::mqtt_client::QoS::at_least_once;
        break;
    case QOS::QOS2:
        mqtt_qos = everest::lib::io::mqtt::mqtt_client::QoS::exactly_once;
        break;

    default:
        break;
    }

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

    const auto error =
        this->mqtt_client->publish(topic, data, mqtt_qos, retain, everest::lib::io::mqtt::PropertiesBase{});
    if (error != everest::lib::io::mqtt::ErrorCode::Success) {
        EVLOG_error << fmt::format("MQTT Error");
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

    auto max_qos_level = everest::lib::io::mqtt::mqtt_client::QoS::at_most_once;
    switch (qos) {
    case QOS::QOS0:
        max_qos_level = everest::lib::io::mqtt::mqtt_client::QoS::at_most_once;
        break;
    case QOS::QOS1:
        max_qos_level = everest::lib::io::mqtt::mqtt_client::QoS::at_least_once;
        break;
    case QOS::QOS2:
        max_qos_level = everest::lib::io::mqtt::mqtt_client::QoS::exactly_once;
        break;
    }

    this->subscribed_topics.insert(topic);

    const auto result = this->mqtt_client->subscribe(
        topic,
        [this](everest::lib::io::mqtt::mosquitto_cpp& client,
               everest::lib::io::mqtt::mosquitto_cpp::message const& message) {
            this->message_queue.add(std::unique_ptr<Message>(new Message{message.topic, message.payload}));
        },
        max_qos_level);
}

void MQTTAbstractionImpl::unsubscribe(const std::string& topic) {
    BOOST_LOG_FUNCTION();
    const std::lock_guard<std::mutex> lock(topics_mutex);

    if (this->subscribed_topics.find(topic) == this->subscribed_topics.end()) {
        EVLOG_warning << fmt::format("Tried to unsubscribe from topic {} but it was not subscribed", topic);
        return;
    }

    EVLOG_debug << fmt::format("Unsubscribing from topic: {}", topic);

    this->subscribed_topics.erase(topic);

    this->mqtt_client->unsubscribe(topic, everest::lib::io::mqtt::PropertiesBase{});
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

json MQTTAbstractionImpl::get(const std::string& topic, QOS qos) {
    BOOST_LOG_FUNCTION();
    std::lock_guard<std::mutex> lock(topic_request_mutex);

    std::promise<json> res_promise;
    std::future<json> res_future = res_promise.get_future();

    const auto res_handler = [this, &res_promise](const std::string& /*topic*/, json data) {
        res_promise.set_value(std::move(data));
    };

    const std::shared_ptr<TypedHandler> res_token =
        std::make_shared<TypedHandler>(HandlerType::GetConfigResponse, std::make_shared<Handler>(res_handler));
    this->register_handler(
        topic, res_token,
        QOS::QOS2); // without the lock guard, response handlers could be overriden if called from different threads

    // wait for result future
    const std::chrono::time_point<std::chrono::steady_clock> res_wait =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(mqtt_get_timeout_ms);
    std::future_status res_future_status = std::future_status::deferred;
    do {
        res_future_status = res_future.wait_until(res_wait);
    } while (res_future_status == std::future_status::deferred);

    json result;
    if (res_future_status == std::future_status::timeout) {
        this->unregister_handler(topic, res_token);
        EVLOG_AND_THROW(EverestTimeoutError(fmt::format("Timeout while waiting for result of get()")));
    }
    if (res_future_status == std::future_status::ready) {
        result = res_future.get();
    }
    this->unregister_handler(topic, res_token);

    return result;
}

json MQTTAbstractionImpl::get(const MQTTRequest& request) {
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
            // Create event handler and register clients
            everest::lib::io::event::fd_event_handler ev_handler;
            ev_handler.register_event_handler(&*this->mqtt_client);

            ev_handler.run(this->running);
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
        EVLOG_debug << fmt::format("Subscribing to {}", topic);
        this->subscribe(topic, qos);
    }
}

void MQTTAbstractionImpl::unregister_handler(const std::string& topic, const Token& token) {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << fmt::format("Unregistering handler {} for {}", fmt::ptr(&token), topic);

    if (this->mqtt_is_connected) {
        this->unsubscribe(topic);
    }
}

} // namespace Everest
