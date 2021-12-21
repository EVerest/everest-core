// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_MQTT_ABSTRACTION_IMPL_HPP
#define UTILS_MQTT_ABSTRACTION_IMPL_HPP

#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include <mqtt.h>
#include <sigslot/signal.hpp>

#include <nlohmann/json.hpp>

#include <utils/types.hpp>

#define MQTT_BUF_SIZE 150 * 1024

namespace Everest {
using json = nlohmann::json;

///
/// \brief Contains a C++ abstraction of MQTT-C and some convenience functionality for using MQTT in EVerest modules
///
class MQTTAbstractionImpl {

private:
    bool mqtt_is_connected;
    std::map<std::string, std::vector<Token>> handlers;
    std::mutex handlers_mutex;

    std::thread mqtt_mainloop_thread;

    struct mqtt_client mqtt_client;
    uint8_t sendbuf[MQTT_BUF_SIZE];
    uint8_t recvbuf[MQTT_BUF_SIZE];
    std::string mqtt_server_address;
    std::string mqtt_server_port;

    MQTTAbstractionImpl(std::string mqtt_server_address, std::string mqtt_server_port);

    static int open_nb_socket(const char* addr, const char* port);
    bool connectBroker(const char* host, const char* port);
    void _mainloop();
    void on_mqtt_message(std::string topic, std::string payload);
    void on_mqtt_message_(std::string topic, std::string payload);
    void on_mqtt_connect();
    static void on_mqtt_disconnect();

public:
    ///
    /// \brief connects to the mqtt broker using the MQTT_SERVER_ADDRESS AND MQTT_SERVER_PORT environment variables
    bool connect();

    ///
    /// \brief disconnects from the mqtt broker
    void disconnect();

    ///
    /// \brief publishes the given \p json on the given \p topic
    void publish(const std::string& topic, const json& json);

    ///
    /// \brief publishes the given \p data on the given \p topic
    void publish(const std::string& topic, const std::string& data);

    ///
    /// \brief subscribes to the given \p topic
    void subscribe(const std::string& topic);

    ///
    /// \brief unsubscribes from the given \p topic
    void unsubscribe(const std::string& topic);

    ///
    /// \brief starts the mqtt main loop in its own tread and immediately joins it, making this a blocking operation
    void mainloop();

    ///
    /// \brief subscribes to the given \p topic and registers a callback \p handler that is called when a message
    /// arrives on the topic. This function only allows one handler per topic.
    ///
    /// \returns a Token with which the handler can be unregistered later
    Token register_handler(const std::string& topic, const Handler& handler);

    ///
    /// \brief subscribes to the given \p topic and registers a callback \p handler that is called when a message
    /// arrives on the topic. If \p allow_multiple_handlers is set to true, multiple handlers can be registered for the
    /// same topic.
    ///
    /// \returns a Token with which the handler can be unregistered later
    Token register_handler(const std::string& topic, const Handler& handler, bool allow_multiple_handlers);

    ///
    /// \brief unsubscribes a handler identified by its \p token from the given \p topic
    void unregister_handler(const std::string& topic, const Token& token);

    ///
    /// \brief checks if the given \p full_topic matches the given \p wildcard_topic that can contain "+" and "#"
    /// wildcards
    ///
    /// \returns true if the topic matches, false otherwise
    static bool check_topic_matches(std::string full_topic, std::string wildcard_topic);

    ///
    /// \brief callback that is called from the mqtt implementation whenever a message is received
    static void publish_callback(void** unused, struct mqtt_response_publish* published);

    ///
    /// \returns the instance of the MQTTAbstractionImpl singleton taking a \p mqtt_server_address and \p
    /// mqtt_server_port as parameters
    static MQTTAbstractionImpl& get_instance(const std::string& mqtt_server_address,
                                             const std::string& mqtt_server_port) {
        static MQTTAbstractionImpl instance(mqtt_server_address, mqtt_server_port);

        return instance;
    }

    MQTTAbstractionImpl(MQTTAbstractionImpl const&) = delete;
    void operator=(MQTTAbstractionImpl const&) = delete;
};
} // namespace Everest

#endif // UTILS_MQTT_ABSTRACTION_IMPL_HPP
