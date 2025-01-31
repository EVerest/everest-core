// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstring>
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
#include <everest/logging.hpp>

#include <utils/mqtt_abstraction_impl.hpp>

namespace Everest {
const auto mqtt_keep_alive = 600;
const auto mqtt_get_timeout_ms = 5000; ///< Timeout for MQTT get in milliseconds

MessageWithQOS::MessageWithQOS(const std::string& topic, const std::string& payload, QOS qos) :
    Message{topic, payload}, qos(qos) {
}

MQTTAbstractionImpl::MQTTAbstractionImpl(const std::string& mqtt_server_address, const std::string& mqtt_server_port,
                                         const std::string& mqtt_everest_prefix,
                                         const std::string& mqtt_external_prefix) :
    message_queue(([this](const Message& message) { this->on_mqtt_message(message); })),
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

    this->disconnect_event_fd = eventfd(0, 0);
    if (this->disconnect_event_fd == -1) {
        throw EverestInternalError("Could not setup eventfd for disconnect event");
    }
}

MQTTAbstractionImpl::MQTTAbstractionImpl(const std::string& mqtt_server_socket_path,
                                         const std::string& mqtt_everest_prefix,
                                         const std::string& mqtt_external_prefix) :
    message_queue(([this](const Message& message) { this->on_mqtt_message(message); })),
    mqtt_server_socket_path(mqtt_server_socket_path),
    mqtt_everest_prefix(mqtt_everest_prefix),
    mqtt_external_prefix(mqtt_external_prefix),
    mqtt_client{},
    sendbuf{},
    recvbuf{} {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << "Initializing MQTT abstraction layer...";

    this->mqtt_is_connected = false;

    this->mqtt_client.publish_response_callback_state = &this->message_queue;

    this->disconnect_event_fd = eventfd(0, 0);
    if (this->disconnect_event_fd == -1) {
        throw EverestInternalError("Could not setup eventfd for disconnect event");
    }
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

    if (this->mqtt_is_connected) {
        return true;
    }

    if (!this->mqtt_server_socket_path.empty()) {
        EVLOG_debug << fmt::format("Connecting to MQTT broker: {}", this->mqtt_server_socket_path);
        return connectBroker(this->mqtt_server_socket_path);
    } else {
        EVLOG_debug << fmt::format("Connecting to MQTT broker: {}:{}", this->mqtt_server_address,
                                   this->mqtt_server_port);
        return connectBroker(this->mqtt_server_address.c_str(), this->mqtt_server_port.c_str());
    }
}

void MQTTAbstractionImpl::disconnect() {
    BOOST_LOG_FUNCTION();

    mqtt_disconnect(&this->mqtt_client);
    // FIXME(kai): always set connected to false for the moment
    this->mqtt_is_connected = false;
    eventfd_write(this->disconnect_event_fd, 1);
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

    if (retain) {
        publish_flags |= MQTT_PUBLISH_RETAIN;
        if (not(data.empty() and qos == QOS::QOS0)) {
            // topic should be retained, so save the topic in retained_topics
            // do not save the topic when the payload is empty and QOS is set to 0 which means a retained topic is to be
            // cleared
            const std::lock_guard<std::mutex> lock(retained_topics_mutex);
            this->retained_topics.push_back(topic);
        }
    }

    if (!this->mqtt_is_connected) {
        const std::lock_guard<std::mutex> lock(messages_before_connected_mutex);
        this->messages_before_connected.push_back(std::make_shared<MessageWithQOS>(topic, data, qos));
        return;
    }

    const MQTTErrors error = mqtt_publish(&this->mqtt_client, topic.c_str(), data.c_str(), data.size(), publish_flags);
    if (error != MQTT_OK) {
        EVLOG_error << fmt::format("MQTT Error {}", mqtt_error_str(error));
    }
    notify_write_data();

    EVLOG_verbose << fmt::format("publishing to {}", topic);
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
    notify_write_data();
}

void MQTTAbstractionImpl::unsubscribe(const std::string& topic) {
    BOOST_LOG_FUNCTION();

    mqtt_unsubscribe(&this->mqtt_client, topic.c_str());
    notify_write_data();
}

void MQTTAbstractionImpl::clear_retained_topics() {
    BOOST_LOG_FUNCTION();
    const std::lock_guard<std::mutex> lock(retained_topics_mutex);

    for (const auto& retained_topic : retained_topics) {
        this->publish(retained_topic, std::string(), QOS::QOS0, true);
        EVLOG_verbose << "Cleared retained topic: " << retained_topic;
    }

    retained_topics.clear();
}

json MQTTAbstractionImpl::get(const std::string& topic, QOS qos) {
    BOOST_LOG_FUNCTION();
    std::promise<json> res_promise;
    std::future<json> res_future = res_promise.get_future();

    const auto res_handler = [this, &res_promise](const std::string& topic, json data) {
        res_promise.set_value(std::move(data));
    };

    const std::shared_ptr<TypedHandler> res_token =
        std::make_shared<TypedHandler>(HandlerType::GetConfig, std::make_shared<Handler>(res_handler));
    this->register_handler(topic, res_token, QOS::QOS2);

    // wait for result future
    const std::chrono::time_point<std::chrono::steady_clock> res_wait =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(mqtt_get_timeout_ms);
    std::future_status res_future_status;
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

void MQTTAbstractionImpl::notify_write_data() {
    // FIXME (aw): error handling
    eventfd_write(this->event_fd, 1);
}

std::shared_future<void> MQTTAbstractionImpl::spawn_main_loop_thread() {
    BOOST_LOG_FUNCTION();

    std::packaged_task<void(void)> task([this]() {
        try {
            while (this->mqtt_is_connected) {

                eventfd_t eventfd_buffer;
                const int nfds = 3;
                struct pollfd pollfds[nfds] = {{this->mqtt_socket_fd, POLLIN, 0},
                                               {this->event_fd, POLLIN, 0},
                                               {this->disconnect_event_fd, POLLIN, 0}};
                auto retval = ::poll(pollfds, nfds, mqtt_poll_timeout_ms);

                if (retval >= 0) {
                    // data available to send (the notifier writes, we should be ready to read)
                    if (retval > 0) {
                        // check for disconnect event
                        if (pollfds[2].revents & POLLIN) {
                            break;
                        }
                        // check for write notification and reset it
                        if (pollfds[1].revents & POLLIN) {
                            // FIXME (aw): check for failure
                            eventfd_read(this->event_fd, &eventfd_buffer);
                        }
                    }

                    if (retval == 0) {
                        // nothing to send or receive however, need to send the keep alive message
                        // otherwise the brocker will disconnect the client
                        // mqtt_sync might send this automatically but ocasionally might miss it
                        mqtt_ping(&this->mqtt_client);
                    }

                    // send and receive messages
                    const MQTTErrors error = mqtt_sync(&this->mqtt_client);
                    if (error != MQTT_OK) {
                        EVLOG_error << fmt::format("Error during MQTT sync: {}", mqtt_error_str(error));

                        on_mqtt_disconnect();

                        return;
                    }
                } else {
                    // probably we got hit by a signal, nothing to do, just reloop
                }
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

        std::unique_lock<std::mutex> lock(handlers_mutex);
        std::shared_ptr<ParsedMessage> parsed_message{nullptr};
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
                if (not parsed_message) {
                    parsed_message.reset(new ParsedMessage{topic, std::move(data)});
                }
                handler.add(parsed_message);
            }
        }
        lock.unlock();

        // It can happen that we unsubscribe from a topic and have removed the message handler but the MQTT unsubscribe
        // didn't complete yet and we still receive messages on this topic that we can just ignore
        if (!found) {
            EVLOG_verbose << fmt::format("Topic '{}' should have a matching handler!", topic);
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
        EVLOG_verbose << fmt::format("Registering result handler {} for command {} on topic {}",
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
    case HandlerType::GetConfig:
        EVLOG_debug << fmt::format("Registering get config MQTT handler {} on topic {}", fmt::ptr(&handler->handler),
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

    const auto subscription_necessary =
        (this->mqtt_is_connected && this->message_handlers.at(topic).count_handlers() == 0);

    this->message_handlers.at(topic).add_handler(handler);

    if (subscription_necessary) {
        EVLOG_verbose << fmt::format("Subscribing to {}", topic);
        this->subscribe(topic, qos);
    }
    EVLOG_verbose << fmt::format("#handler[{}] = {}", topic, this->message_handlers.at(topic).count_handlers());
}

void MQTTAbstractionImpl::unregister_handler(const std::string& topic, const Token& token) {
    BOOST_LOG_FUNCTION();

    EVLOG_verbose << fmt::format("Unregistering handler {} for {}", fmt::ptr(&token), topic);

    const std::lock_guard<std::mutex> lock(handlers_mutex);
    std::size_t number_of_handlers = 0;
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
            EVLOG_verbose << fmt::format("Unsubscribing from {}", topic);
            this->unsubscribe(topic);
        }
        this->message_handlers.erase(topic);
    }

    const std::string handler_count = (number_of_handlers == 0) ? "None" : std::to_string(number_of_handlers);
    EVLOG_verbose << fmt::format("#handler[{}] = {}", topic, handler_count);
}

bool MQTTAbstractionImpl::connectBroker(std::string& socket_path) {
    BOOST_LOG_FUNCTION();

    /* open the non-blocking TCP socket (connecting to the broker) */
    mqtt_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (mqtt_socket_fd == -1) {
        EVLOG_error << fmt::format("Failed to open socket: {}", strerror(errno));
        return false;
    }

    // Initialize the address structure
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    if (socket_path.size() > (sizeof(addr.sun_path) - 1)) {
        EVLOG_error << fmt::format("the given path for the unix domain socket: {} is too big", socket_path);
        close(mqtt_socket_fd);
        return false;
    }
    // no need to set the terminating null due to memset
    std::strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    // make non-blocking
    auto retval = fcntl(mqtt_socket_fd, F_SETFL,
                        fcntl(mqtt_socket_fd, F_GETFL) | O_NONBLOCK); // NOLINT: We have no good alternative to fcntl
    if (retval != 0) {
        EVLOG_error << fmt::format("Failed to set nonblock for unix domain socket: {}", socket_path);
        close(mqtt_socket_fd);
        return false;
    }

    // conect the socket
    if (::connect(mqtt_socket_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(struct sockaddr_un)) == -1) {
        EVLOG_error << fmt::format("Failed to connect to unix domain socket: {}", socket_path);
        close(mqtt_socket_fd);
        return false;
    }

    this->event_fd = eventfd(0, 0);
    if (this->event_fd == -1) {
        close(this->mqtt_socket_fd);
        EVLOG_error << "Could not setup eventfd for mqttc io";
        return false;
    }

    mqtt_init(&this->mqtt_client, mqtt_socket_fd, static_cast<uint8_t*>(this->sendbuf), sizeof(this->sendbuf),
              static_cast<uint8_t*>(this->recvbuf), sizeof(this->recvbuf), MQTTAbstractionImpl::publish_callback);
    const uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    /* Send connection request to the broker. */
    if (mqtt_connect(&this->mqtt_client, nullptr, nullptr, nullptr, 0, nullptr, nullptr, connect_flags,
                     mqtt_keep_alive) != MQTT_OK) {
        return false;
    }
    // TODO(kai): async?
    const auto error = mqtt_sync(&this->mqtt_client);
    if (error != MQTT_OK) {
        EVLOG_error << fmt::format("Error during MQTT sync: {}", mqtt_error_str(error));
        return false;
    }

    on_mqtt_connect();
    return true;
}

bool MQTTAbstractionImpl::connectBroker(const char* host, const char* port) {
    BOOST_LOG_FUNCTION();

    /* open the non-blocking TCP socket (connecting to the broker) */
    mqtt_socket_fd = open_nb_socket(host, port);

    if (mqtt_socket_fd == -1) {
        EVLOG_error << fmt::format("Failed to open socket: {}", strerror(errno));
        return false;
    }

    this->event_fd = eventfd(0, 0);
    if (this->event_fd == -1) {
        close(this->mqtt_socket_fd);
        EVLOG_error << "Could not setup eventfd for mqttc io";
        return false;
    }

    // Set TCP_NODELAY option. To take full advantage, this should also be set in mosquitto config.
    // This avoids about 40ms latency on small MQTT publishes
    int enable = 1;
    setsockopt(mqtt_socket_fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));

    mqtt_init(&this->mqtt_client, mqtt_socket_fd, static_cast<uint8_t*>(this->sendbuf), sizeof(this->sendbuf),
              static_cast<uint8_t*>(this->recvbuf), sizeof(this->recvbuf), MQTTAbstractionImpl::publish_callback);
    const uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    /* Send connection request to the broker. */
    if (mqtt_connect(&this->mqtt_client, nullptr, nullptr, nullptr, 0, nullptr, nullptr, connect_flags,
                     mqtt_keep_alive) != MQTT_OK) {
        return false;
    }
    // TODO(kai): async?
    const auto error = mqtt_sync(&this->mqtt_client);
    if (error != MQTT_OK) {
        EVLOG_error << fmt::format("Error during MQTT sync: {}", mqtt_error_str(error));
        return false;
    }

    on_mqtt_connect();
    return true;
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
        const std::string start = wildcard_topic.substr(0, wildcard_topic.size() - 2);
        const std::string end = wildcard_topic.substr(wildcard_topic.size() - 2);
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

    for (std::size_t partno = 0; partno < full_split.size(); partno++) {
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
    message_queue->add(std::unique_ptr<Message>(new Message{
        std::string(static_cast<const char*>(published->topic_name), published->topic_name_size),
        std::string(static_cast<const char*>(published->application_message), published->application_message_size)}));
}

} // namespace Everest
