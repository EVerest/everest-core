// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef UTILS_MQTT_ABSTRACTION_IMPL_HPP
#define UTILS_MQTT_ABSTRACTION_IMPL_HPP

#include <cstddef>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <mqtt.h>
#include <nlohmann/json.hpp>

#include <utils/message_queue.hpp>
#include <utils/types.hpp>

#include <utils/thread.hpp>

constexpr auto MQTT_BUF_SIZE = 500 * std::size_t{1024};

namespace Everest {
/// \brief Contains a payload and the topic it was received on with additional QOS
struct MessageWithQOS : Message {
    QOS qos; ///< The Quality of Service level

    MessageWithQOS(const std::string& topic, const std::string& payload, QOS qos);
};

///
/// \brief Contains a C++ abstraction of MQTT-C and some convenience functionality for using MQTT in EVerest modules
///
class MQTTAbstractionImpl {
public:
    MQTTAbstractionImpl(const std::string& mqtt_server_address, const std::string& mqtt_server_port,
                        const std::string& mqtt_everest_prefix, const std::string& mqtt_external_prefix);
    MQTTAbstractionImpl(const std::string& mqtt_server_socket_path, const std::string& mqtt_everest_prefix,
                        const std::string& mqtt_external_prefix);

    ~MQTTAbstractionImpl();

    MQTTAbstractionImpl(MQTTAbstractionImpl const&) = delete;
    void operator=(MQTTAbstractionImpl const&) = delete;
    ///
    /// \brief connects to the mqtt broker using the MQTT_SERVER_ADDRESS AND MQTT_SERVER_PORT environment variables
    bool connect();

    ///
    /// \brief disconnects from the mqtt broker
    void disconnect();

    ///
    /// \brief publishes the given \p json on the given \p topic with QOS level 0
    void publish(const std::string& topic, const nlohmann::json& json);

    ///
    /// \brief publishes the given \p json on the given \p topic with the given \p qos
    void publish(const std::string& topic, const nlohmann::json& json, QOS qos, bool retain = false);

    ///
    /// \brief publishes the given \p data on the given \p topic with QOS level 0
    void publish(const std::string& topic, const std::string& data);

    ///
    /// \brief publishes the given \p data on the given \p topic with the given \p qos
    void publish(const std::string& topic, const std::string& data, QOS qos, bool retain = false);

    ///
    /// \brief subscribes to the given \p topic with QOS level 0
    void subscribe(const std::string& topic);

    ///
    /// \brief subscribes to the given \p topic with the given \p qos
    void subscribe(const std::string& topic, QOS qos);

    ///
    /// \brief unsubscribes from the given \p topic
    void unsubscribe(const std::string& topic);

    ///
    /// \brief clears any previously published topics that had the retain flag set
    void clear_retained_topics();

    ///
    /// \brief subscribe and wait for value on the subscribed topic
    nlohmann::json get(const std::string& topic, QOS qos);

    ///
    /// \brief Spawn a thread running the mqtt main loop
    /// \returns a future, which will be fulfilled on thread termination
    std::shared_future<void> spawn_main_loop_thread();

    ///
    /// \returns the main loop future, which will be fulfilled on thread termination
    std::shared_future<void> get_main_loop_future();

    ///
    /// \brief subscribes to the given \p topic and registers a callback \p handler that is called when a message
    /// arrives on the topic. With \p qos a MQTT Quality of Service level can be set.
    void register_handler(const std::string& topic, std::shared_ptr<TypedHandler> handler, QOS qos);

    ///
    /// \brief unsubscribes a handler identified by its \p token from the given \p topic
    void unregister_handler(const std::string& topic, const Token& token);

    ///
    /// \brief checks if the given \p full_topic matches the given \p wildcard_topic that can contain "+" and "#"
    /// wildcards
    ///
    /// \returns true if the topic matches, false otherwise
    static bool check_topic_matches(const std::string& full_topic, const std::string& wildcard_topic);

    ///
    /// \brief callback that is called from the mqtt implementation whenever a message is received
    static void publish_callback(void** unused, struct mqtt_response_publish* published);

private:
    static constexpr int mqtt_poll_timeout_ms{300000};
    bool mqtt_is_connected;
    std::map<std::string, MessageHandler> message_handlers;
    std::mutex handlers_mutex;
    MessageQueue message_queue;
    std::vector<std::shared_ptr<MessageWithQOS>> messages_before_connected;
    std::mutex messages_before_connected_mutex;
    std::mutex retained_topics_mutex;
    std::vector<std::string> retained_topics;

    Thread mqtt_mainloop_thread;
    std::shared_future<void> main_loop_future;

    std::string mqtt_server_socket_path;
    std::string mqtt_server_address;
    std::string mqtt_server_port;
    std::string mqtt_everest_prefix;
    std::string mqtt_external_prefix;
    struct mqtt_client mqtt_client;
    uint8_t sendbuf[MQTT_BUF_SIZE];
    uint8_t recvbuf[MQTT_BUF_SIZE];

    static int open_nb_socket(const char* addr, const char* port);
    bool connectBroker(std::string& socket_path);
    bool connectBroker(const char* host, const char* port);
    void on_mqtt_message(const Message& message);
    void on_mqtt_connect();
    static void on_mqtt_disconnect();

    void notify_write_data();

    int mqtt_socket_fd{-1};
    int event_fd{-1};
    int disconnect_event_fd{-1};
};
} // namespace Everest

#endif // UTILS_MQTT_ABSTRACTION_IMPL_HPP
