// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
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
#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

#include <utils/mqtt_abstraction_impl.hpp>

namespace Everest {
sigslot::signal<std::string, std::string> signalReceived;
const auto mqtt_sync_sleep_milliseconds = 10;
const auto mqtt_keep_alive = 400;

MQTTAbstractionImpl::MQTTAbstractionImpl(std::string mqtt_server_address, std::string mqtt_server_port) :
    mqtt_server_address(std::move(mqtt_server_address)),
    mqtt_server_port(std::move(mqtt_server_port)),
    mqtt_client{},
    sendbuf{},
    recvbuf{} {
    BOOST_LOG_FUNCTION();

    EVLOG(debug) << "Initializing mqtt abstraction layer...";

    this->mqtt_is_connected = false;

    signalReceived.connect(&MQTTAbstractionImpl::on_mqtt_message, this);
}

bool MQTTAbstractionImpl::connect() {
    BOOST_LOG_FUNCTION();

    EVLOG(info) << "Connecting to mqtt broker...";

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

    std::string data = json.dump();
    publish(topic, data);
}

void MQTTAbstractionImpl::publish(const std::string& topic, const std::string& data) {
    BOOST_LOG_FUNCTION();

    MQTTErrors error = mqtt_publish(&this->mqtt_client, topic.c_str(), data.c_str(), data.size(), MQTT_PUBLISH_QOS_0);
    if (error != MQTT_OK) {
        EVLOG(error) << "MQTT Error" << mqtt_error_str(error);
    }

    EVLOG(debug) << "publishing to " << topic;
}

void MQTTAbstractionImpl::subscribe(const std::string& topic) {
    BOOST_LOG_FUNCTION();

    mqtt_subscribe(&this->mqtt_client, topic.c_str(), 0);
}

void MQTTAbstractionImpl::unsubscribe(const std::string& topic) {
    BOOST_LOG_FUNCTION();

    mqtt_unsubscribe(&this->mqtt_client, topic.c_str());
}

void MQTTAbstractionImpl::mainloop() {
    BOOST_LOG_FUNCTION();

    this->mqtt_mainloop_thread = std::thread(&MQTTAbstractionImpl::_mainloop, this);
    this->mqtt_mainloop_thread.join();
}

void MQTTAbstractionImpl::_mainloop() {
    BOOST_LOG_FUNCTION();

    try {
        while (this->mqtt_is_connected) {
            MQTTErrors error = mqtt_sync(&this->mqtt_client);
            if (error != MQTT_OK) {
                EVLOG(error) << "Error during mqtt sync: " << mqtt_error_str(error);

                on_mqtt_disconnect();

                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(mqtt_sync_sleep_milliseconds));
        }
    } catch (boost::exception& e) {
        EVLOG(critical) << "Caught mqtt mainloop boost::exception:" << std::endl << boost::diagnostic_information(e, true);
        exit(1);
    } catch (std::exception& e) {
        EVLOG(critical) << "Caught mqtt mainloop std::exception:" << std::endl << boost::diagnostic_information(e, true);
        exit(1);
    }
}

void MQTTAbstractionImpl::on_mqtt_message(std::string topic, std::string payload) {
    BOOST_LOG_FUNCTION();

    std::thread t(&MQTTAbstractionImpl::on_mqtt_message_, this, topic, payload);
    t.detach(); // FIXME(kai): is this desireable?
}

void MQTTAbstractionImpl::on_mqtt_message_(std::string topic, std::string payload) {
    BOOST_LOG_FUNCTION();

    try {
        json data;
        if (topic.find("everest/") == 0) {
            EVLOG(debug) << "topic starts with everest/";
            try {
                data = json::parse(payload);
            } catch (nlohmann::detail::parse_error& e) {
                EVLOG(warning) << "Could not decode json for incoming topic '" << topic << "': " << payload << "";
                return;
            }
        } else {
            EVLOG(debug) << "Message parsing for topic '" << topic << "' not implemented. Wrapping in json object.";
            data = json(payload);
        }

        bool found = false;

        std::unique_lock<std::mutex> lock(handlers_mutex);
        std::vector<Handler> local_handlers;
        for (auto const& ha : this->handlers) {
            std::string handler_topic = ha.first;
            if (MQTTAbstractionImpl::check_topic_matches(topic, handler_topic)) {
                found = true;
                EVLOG(debug) << "there is a handler! " << ha.first;
                for (const auto& handler : ha.second) {
                    auto f = *handler;
                    local_handlers.push_back(f);
                }
            }
        }
        lock.unlock();

        for (auto const& handler : local_handlers) {
            EVLOG(debug) << "calling handler: " << &handler;
            handler(data);
            EVLOG(debug) << "handler '" << &handler << "' called";
        }

        if (!found) {
            std::ostringstream oss;
            oss << "Internal error: topic '" << topic << "' should have a matching handler!";
            EVLOG_AND_THROW(EverestInternalError(oss.str()));
        }
    } catch (boost::exception& e) {
        EVLOG(critical) << "Caught mqtt on_message boost::exception:" << std::endl << boost::diagnostic_information(e, true);
        exit(1);
    } catch (std::exception& e) {
        EVLOG(critical) << "Caught mqtt on_message std::exception:" << std::endl << boost::diagnostic_information(e, true);
        exit(1);
    }
}

void MQTTAbstractionImpl::on_mqtt_connect() {
    BOOST_LOG_FUNCTION();

    EVLOG(info) << "Connected to mqtt broker";

    // subscribe to all topics needed by currently registered handlers
    EVLOG(info) << "Subscribing to needed mqtt topics...";
    const std::lock_guard<std::mutex> lock(handlers_mutex);
    for (auto const& ha : this->handlers) {
        std::string topic = ha.first;
        EVLOG(info) << "Subscribing to " << topic;
        subscribe(topic);
    }

    // this will allow new handlers to subscribe directly, if needed
    this->mqtt_is_connected = true;
}

void MQTTAbstractionImpl::on_mqtt_disconnect() {
    BOOST_LOG_FUNCTION();

    EVLOG_AND_THROW(EverestInternalError("Lost connection to mqtt broker"));
}

Token MQTTAbstractionImpl::register_handler(const std::string& topic, const Handler& handler,
                                            bool allow_multiple_handlers) {
    BOOST_LOG_FUNCTION();

    EVLOG(debug) << "Registering handler " << &handler << " for " << topic;

    const std::lock_guard<std::mutex> lock(handlers_mutex);
    if (!this->handlers[topic].empty() && !allow_multiple_handlers) {
        std::ostringstream oss;
        oss << "Can not register handlers for topic " << topic
            << " twice, if optional argument 'allow_multiple_handlers' it not "
               "'true'!";
        EVLOG_AND_THROW(EverestInternalError(oss.str()));
    }
    Token shared_handler = std::make_shared<Handler>(handler);
    this->handlers[topic].push_back(shared_handler);

    // only subscribe for this topic if we aren't already and the mqtt client is connected
    // if we are not connected the on_mqtt_connect() callback will subscribe to the topic
    if (this->mqtt_is_connected && this->handlers[topic].size() == 1) {
        EVLOG(debug) << "Subscribing to " << topic;
        this->subscribe(topic);
    }
    EVLOG(debug) << "#handler[" << topic << "] = " << this->handlers[topic].size();

    return shared_handler;
}

Token MQTTAbstractionImpl::register_handler(const std::string& topic, const Handler& handler) {
    BOOST_LOG_FUNCTION();

    return register_handler(topic, handler, false);
}

void MQTTAbstractionImpl::unregister_handler(const std::string& topic, const Token& token) {
    BOOST_LOG_FUNCTION();

    EVLOG(debug) << "Unregistering handler " << &token << " for " << topic;

    const std::lock_guard<std::mutex> lock(handlers_mutex);
    if (this->handlers.count(topic) != 0) {
        auto it = std::find(this->handlers[topic].begin(), this->handlers[topic].end(), token);
        this->handlers[topic].erase(it);
    }

    // unsubscribe if this was the last handler for this topic
    if (this->handlers[topic].empty()) {
        this->handlers.erase(topic);
        // TODO(kai): should we throw/log an error if we are not connected?
        if (this->mqtt_is_connected) {
            EVLOG(debug) << "Unsubscribing from " << topic;
            this->unsubscribe(topic);
        }
    }

    std::ostringstream oss;
    oss << "#handler[" << topic << "] = ";
    if (this->handlers.count(topic) == 0) {
        oss << "None";
    } else {
        oss << this->handlers[topic].size();
    }
    EVLOG(debug) << oss.str();
}

bool MQTTAbstractionImpl::connectBroker(const char* host, const char* port) {
    BOOST_LOG_FUNCTION();

    /* open the non-blocking TCP socket (connecting to the broker) */
    int sockfd = open_nb_socket(host, port);

    if (sockfd == -1) {
        perror("Failed to open socket: ");
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

    struct addrinfo hints = {0};

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
bool MQTTAbstractionImpl::check_topic_matches(std::string full_topic, std::string wildcard_topic) {
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

void MQTTAbstractionImpl::publish_callback(void** /*unused*/, struct mqtt_response_publish* published) {
    BOOST_LOG_FUNCTION();

    mqtt_response_publish message = *published;

    // note that message.topic_name is NOT null-terminated
    // (here we'll change it to a std::string which will take care of this particular conversion)

    std::string topic = std::string(static_cast<const char*>(message.topic_name), message.topic_name_size);

    std::string payload =
        std::string(static_cast<const char*>(message.application_message), message.application_message_size);

    // emit a copy of published results
    signalReceived(topic, payload);
}

} // namespace Everest
