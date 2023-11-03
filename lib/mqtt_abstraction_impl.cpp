// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <sstream>
#include <thread>

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fmt/format.h>

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

#include <utils/mqtt_abstraction_impl.hpp>

namespace Everest {
const auto mqtt_sync_sleep_milliseconds = 10;
const auto mqtt_keep_alive = 400;

MessageWithQOS::MessageWithQOS(const std::string& topic, const std::string& payload, QOS qos) :
    Message(topic, payload), qos(qos) {
}

MQTTAbstractionImpl::MQTTAbstractionImpl(const std::string& mqtt_server_address, const std::string& mqtt_server_port,
                                         const std::string& mqtt_everest_prefix,
                                         const std::string& mqtt_external_prefix) :
    message_queue(([this](std::shared_ptr<Message> message) { this->on_mqtt_message(message); })),
    mqtt_server_address(mqtt_server_address),
    mqtt_server_port(mqtt_server_port),
    mqtt_everest_prefix(mqtt_everest_prefix),
    mqtt_external_prefix(mqtt_external_prefix),
    mqtt_client{},
    sendbuf{},
    recvbuf{} {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << "Initializing MQTT abstraction layer...";

    this->mqtt_is_connected = false;

    this->mqtt_client.publish_response_callback_state = &this->message_queue;
}

MQTTAbstractionImpl::~MQTTAbstractionImpl() {
    // FIXME (aw): verify that disconnecting is thread-safe!
    if (this->mqtt_is_connected) {
        disconnect();
    }
    // this->mqtt_mainloop_thread.join();
}

bool MQTTAbstractionImpl::connect() {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << fmt::format("Connecting to MQTT broker: {}:{}", this->mqtt_server_address, this->mqtt_server_port);

    return connectBroker(this->mqtt_server_address.c_str(), this->mqtt_server_port.c_str());
}

void MQTTAbstractionImpl::disconnect() {
    BOOST_LOG_FUNCTION();

    mqtt_disconnect(&this->mqtt_client);
    // FIXME(kai): always set connected to false for the moment
    this->mqtt_is_connected = false;
}

void MQTTAbstractionImpl::publish(const std::string& topic, const json& json) {
    BOOST_LOG_FUNCTION();

    publish(topic, json, QOS::QOS2);
}

void MQTTAbstractionImpl::publish(const std::string& topic, const json& json, QOS qos) {
    BOOST_LOG_FUNCTION();

    std::string data = json.dump();
    publish(topic, data, qos);
}

void MQTTAbstractionImpl::publish(const std::string& topic, const std::string& data) {
    BOOST_LOG_FUNCTION();

    publish(topic, data, QOS::QOS0);
}

void MQTTAbstractionImpl::publish(const std::string& topic, const std::string& data, QOS qos) {
    BOOST_LOG_FUNCTION();

    auto publish_flags = 0;
    switch (qos) {
    case QOS::QOS0:
        publish_flags = MQTT_PUBLISH_QOS_0;
        break;
    case QOS::QOS1:
        publish_flags = MQTT_PUBLISH_QOS_1;
        break;
    case QOS::QOS2:
        publish_flags = MQTT_PUBLISH_QOS_2;
        break;

    default:
        break;
    }

    if (!this->mqtt_is_connected) {
        const std::lock_guard<std::mutex> lock(messages_before_connected_mutex);
        this->messages_before_connected.push_back(std::make_shared<MessageWithQOS>(topic, data, qos));
        return;
    }

    MQTTErrors error = mqtt_publish(&this->mqtt_client, topic.c_str(), data.c_str(), data.size(), publish_flags);
    if (error != MQTT_OK) {
        EVLOG_error << fmt::format("MQTT Error {}", mqtt_error_str(error));
    }

    EVLOG_debug << fmt::format("publishing to {}", topic);
}

void MQTTAbstractionImpl::subscribe(const std::string& topic) {
    BOOST_LOG_FUNCTION();

    subscribe(topic, QOS::QOS2);
}

void MQTTAbstractionImpl::subscribe(const std::string& topic, QOS qos) {
    BOOST_LOG_FUNCTION();

    auto max_qos_level = 0;
    switch (qos) {
    case QOS::QOS0:
        max_qos_level = 0;
        break;
    case QOS::QOS1:
        max_qos_level = 1;
        break;
    case QOS::QOS2:
        max_qos_level = 2;
        break;
    default:
        break;
    }

    mqtt_subscribe(&this->mqtt_client, topic.c_str(), max_qos_level);
}

void MQTTAbstractionImpl::unsubscribe(const std::string& topic) {
    BOOST_LOG_FUNCTION();

    mqtt_unsubscribe(&this->mqtt_client, topic.c_str());
}

std::future<void> MQTTAbstractionImpl::spawn_main_loop_thread() {
    BOOST_LOG_FUNCTION();

    std::packaged_task<void(void)> task([this]() {
        try {
            while (this->mqtt_is_connected) {
                MQTTErrors error = mqtt_sync(&this->mqtt_client);
                if (error != MQTT_OK) {
                    EVLOG_error << fmt::format("Error during MQTT sync: {}", mqtt_error_str(error));

                    on_mqtt_disconnect();

                    return;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(mqtt_sync_sleep_milliseconds));
            }
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

    auto future = task.get_future();
    this->mqtt_mainloop_thread = std::thread(std::move(task));
    return future;
}

void MQTTAbstractionImpl::on_mqtt_message(std::shared_ptr<Message> message) {
    BOOST_LOG_FUNCTION();

    const std::string& topic = message->topic;
    const std::string& payload = message->payload;

    try {
        std::shared_ptr<json> data;
        bool is_everest_topic = false;
        if (topic.find(mqtt_everest_prefix) == 0) {
            EVLOG_debug << fmt::format("topic {} starts with {}", topic, mqtt_everest_prefix);
            is_everest_topic = true;
            try {
                data = std::make_shared<json>(json::parse(payload));
            } catch (nlohmann::detail::parse_error& e) {
                EVLOG_warning << fmt::format("Could not decode json for incoming topic '{}': {}", topic, payload);
                return;
            }
        } else {
            EVLOG_debug << fmt::format("Message parsing for topic '{}' not implemented. Wrapping in json object.",
                                       topic);
            data = std::make_shared<json>(json(payload));
        }

        bool found = false;

        std::unique_lock<std::mutex> lock(handlers_mutex);
        std::vector<Handler> local_handlers;
        for (auto& [handler_topic, handler] : this->message_handlers) {
            bool topic_matches = false;
            if (is_everest_topic) {
                // everest topics never contain wildcards, so a direct comparison is enough
                if (topic == handler_topic) {
                    topic_matches = true;
                }
            } else {
                topic_matches = MQTTAbstractionImpl::check_topic_matches(topic, handler_topic);
            }
            if (topic_matches) {
                found = true;
                handler.add(data);
            }
        }
        lock.unlock();

        if (!found) {
            EVLOG_AND_THROW(
                EverestInternalError(fmt::format("Internal error: topic '{}' should have a matching handler!", topic)));
        }
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

    // subscribe to all topics needed by currently registered handlers
    EVLOG_debug << "Subscribing to needed MQTT topics...";
    const std::lock_guard<std::mutex> lock(handlers_mutex);
    for (auto const& [topic, handler] : this->message_handlers) {
        EVLOG_debug << fmt::format("Subscribing to {}", topic);
        subscribe(topic); // FIXME(kai): get QOS from handler
    }

    // this will allow new handlers to subscribe directly, if needed
    {
        const std::lock_guard<std::mutex> lock(messages_before_connected_mutex);
        this->mqtt_is_connected = true;
        for (auto& message : this->messages_before_connected) {
            this->publish(message->topic, message->payload, message->qos);
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

    switch (handler->type) {
    case HandlerType::Call:
        EVLOG_debug << fmt::format("Registering call handler {} for command {} on topic {}",
                                   fmt::ptr(&handler->handler), handler->name, topic);
        break;
    case HandlerType::Result:
        EVLOG_debug << fmt::format("Registering result handler {} for command {} on topic {}",
                                   fmt::ptr(&handler->handler), handler->name, topic);
        break;
    case HandlerType::SubscribeVar:
        EVLOG_debug << fmt::format("Registering subscribe handler {} for variable {} on topic {}",
                                   fmt::ptr(&handler->handler), handler->name, topic);
        break;
    case HandlerType::SubscribeError:
        EVLOG_debug << fmt::format("Registering error handler {} for variable {} on topic {}",
                                   fmt::ptr(&handler->handler), handler->name, topic);
        break;
    case HandlerType::ClearErrorRequest:
        EVLOG_debug << fmt::format("Registering clear error handler {} for variable {} on topic {}",
                                   fmt::ptr(&handler->handler), handler->name, topic);
        break;
    case HandlerType::ExternalMQTT:
        EVLOG_debug << fmt::format("Registering external MQTT handler {} on topic {}", fmt::ptr(&handler->handler),
                                   topic);
        break;
    default:
        EVLOG_warning << fmt::format("Registering unknown handler {} on topic {}", fmt::ptr(&handler->handler), topic);
        break;
    }

    const std::lock_guard<std::mutex> lock(handlers_mutex);

    if (this->message_handlers.count(topic) == 0) {
        this->message_handlers.emplace(std::piecewise_construct, std::forward_as_tuple(topic), std::forward_as_tuple());
    }
    this->message_handlers[topic].add_handler(handler);

    // only subscribe for this topic if we aren't already and the mqtt client is connected
    // if we are not connected the on_mqtt_connect() callback will subscribe to the topic
    if (this->mqtt_is_connected && this->message_handlers[topic].count_handlers() == 1) {
        EVLOG_debug << fmt::format("Subscribing to {}", topic);
        this->subscribe(topic, qos);
    }
    EVLOG_debug << fmt::format("#handler[{}] = {}", topic, this->message_handlers[topic].count_handlers());
}

void MQTTAbstractionImpl::unregister_handler(const std::string& topic, const Token& token) {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << fmt::format("Unregistering handler {} for {}", fmt::ptr(&token), topic);

    const std::lock_guard<std::mutex> lock(handlers_mutex);
    auto number_of_handlers = 0;
    if (this->message_handlers.find(topic) != this->message_handlers.end()) {
        auto& topic_message_handler = this->message_handlers.at(topic);
        if (topic_message_handler.count_handlers() != 0) {
            topic_message_handler.remove_handler(token);
            number_of_handlers = topic_message_handler.count_handlers();
        }
    }

    // unsubscribe if this was the last handler for this topic
    if (number_of_handlers == 0) {
        // TODO(kai): should we throw/log an error if we are not connected?
        if (this->mqtt_is_connected) {
            EVLOG_debug << fmt::format("Unsubscribing from {}", topic);
            this->unsubscribe(topic);
        }
    }

    const std::string handler_count = (number_of_handlers == 0) ? "None" : std::to_string(number_of_handlers);
    EVLOG_debug << fmt::format("#handler[{}] = {}", topic, handler_count);
}

bool MQTTAbstractionImpl::connectBroker(const char* host, const char* port) {
    BOOST_LOG_FUNCTION();

    /* open the non-blocking TCP socket (connecting to the broker) */
    int sockfd = open_nb_socket(host, port);

    if (sockfd == -1) {
        EVLOG_error << fmt::format("Failed to open socket: {}", strerror(errno));
        return false;
    }

    mqtt_init(&this->mqtt_client, sockfd, static_cast<uint8_t*>(this->sendbuf), sizeof(this->sendbuf),
              static_cast<uint8_t*>(this->recvbuf), sizeof(this->recvbuf), MQTTAbstractionImpl::publish_callback);
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    /* Send connection request to the broker. */
    if (mqtt_connect(&this->mqtt_client, nullptr, nullptr, nullptr, 0, nullptr, nullptr, connect_flags,
                     mqtt_keep_alive) == MQTT_OK) {
        // TODO(kai): async?
        on_mqtt_connect();
        return true;
    }

    return false;
}

int MQTTAbstractionImpl::open_nb_socket(const char* addr, const char* port) {
    BOOST_LOG_FUNCTION();

    struct addrinfo hints = {0, 0, 0, 0, 0, 0, 0, 0};

    hints.ai_family = AF_UNSPEC;     /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Must be TCP */
    int sockfd = -1;
    int rv = 0;
    struct addrinfo* p = nullptr;
    struct addrinfo* servinfo = nullptr;

    /* get address information */
    rv = getaddrinfo(addr, port, &hints, &servinfo);
    if (rv != 0) {
        // fprintf(stderr, "Failed to open socket (getaddrinfo): %s\n",
        // gai_strerror(rv));
        return -1;
    }

    /* open the first possible socket */
    for (p = servinfo; p != nullptr; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            continue;
        }

        /* connect to server */
        rv = ::connect(sockfd, p->ai_addr, p->ai_addrlen);
        if (rv == -1) {
            close(sockfd);
            sockfd = -1;
            continue;
        }
        break;
    }

    /* free servinfo */
    freeaddrinfo(servinfo);

    /* make non-blocking */
    if (sockfd != -1) {
        fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK); // NOLINT: We have no good alternative to fcntl
    }

    /* return the new socket fd */
    return sockfd;
}

// NOLINTNEXTLINE(misc-no-recursion)
bool MQTTAbstractionImpl::check_topic_matches(const std::string& full_topic, const std::string& wildcard_topic) {
    BOOST_LOG_FUNCTION();

    // verbatim topic
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
    if (full_topic == wildcard_topic) {
        return true;
    }

    // check if the last /# matches a zero part of the topic
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
    if (wildcard_topic.size() >= 2) {
        std::string start = wildcard_topic.substr(0, wildcard_topic.size() - 2);
        std::string end = wildcard_topic.substr(wildcard_topic.size() - 2);
        if (end == "/#" && check_topic_matches(full_topic, start)) {
            return true;
        }
    }

    std::vector<std::string> full_split;
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
    boost::split(full_split, full_topic, boost::is_any_of("/"));

    std::vector<std::string> wildcard_split;
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
    boost::split(wildcard_split, wildcard_topic, boost::is_any_of("/"));

    for (size_t partno = 0; partno < full_split.size(); partno++) {
        if (wildcard_split.size() <= partno) {
            return false;
        }

        if (full_split[partno] == wildcard_split[partno]) {
            continue;
        }

        if (wildcard_split[partno] == "+") {
            continue;
        }

        return wildcard_split[partno] == "#";
    }

    // either all wildcards match, or there are more wildcard parts than topic parts
    return full_split.size() == wildcard_split.size();
}

void MQTTAbstractionImpl::publish_callback(void** state, struct mqtt_response_publish* published) {
    BOOST_LOG_FUNCTION();

    auto message_queue = static_cast<MessageQueue*>(*state);

    // topic_name and application_message are NOT null-terminated, hence copy construct strings
    message_queue->add(std::make_shared<Message>(
        std::string(static_cast<const char*>(published->topic_name), published->topic_name_size),
        std::string(static_cast<const char*>(published->application_message), published->application_message_size)));
}

} // namespace Everest
