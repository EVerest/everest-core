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

#include <nlohmann/json.hpp>

#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/mqtt/mqtt_client.hpp>

#include <utils/config/mqtt_settings.hpp>
#include <utils/message_handler.hpp>
#include <utils/message_queue.hpp>
#include <utils/mqtt_abstraction.hpp>
#include <utils/thread.hpp>
#include <utils/types.hpp>

namespace Everest {
/// \brief Contains a payload and the topic it was received on with additional QOS
struct MessageWithQOS : Message {
    QOS qos;     ///< The Quality of Service level
    bool retain; ///< If the retain flag should be set on publishing this message

    MessageWithQOS(const std::string& topic, const std::string& payload, QOS qos, bool retain);
};

///
/// \brief Contains a C++ abstraction of MQTT-C and some convenience functionality for using MQTT in EVerest modules
///
class MQTTAbstractionImpl : public MQTTAbstraction {
public:
    MQTTAbstractionImpl(const MQTTSettings& mqtt_settings);

    ~MQTTAbstractionImpl() override;

    MQTTAbstractionImpl(MQTTAbstractionImpl const&) = delete;
    void operator=(MQTTAbstractionImpl const&) = delete;

    bool connect() override;
    void disconnect() override;
    void publish(const std::string& topic, const nlohmann::json& json) override;
    void publish(const std::string& topic, const nlohmann::json& json, QOS qos, bool retain = false) override;
    void publish(const std::string& topic, const std::string& data) override;
    void publish(const std::string& topic, const std::string& data, QOS qos, bool retain = false) override;
    void subscribe(const std::string& topic) override;
    void subscribe(const std::string& topic, QOS qos) override;
    void unsubscribe(const std::string& topic) override;
    void clear_retained_topics() override;
    nlohmann::json get(const MQTTRequest& request, std::size_t retries = 0) override;
    nlohmann::json get(const std::string& topic, QOS qos, std::size_t retries = 0) override;
    const std::string& get_everest_prefix() const override;
    const std::string& get_external_prefix() const override;
    std::shared_future<void> spawn_main_loop_thread() override;
    std::shared_future<void> get_main_loop_future() override;
    void register_handler(const std::string& topic, std::shared_ptr<TypedHandler> handler, QOS qos) override;
    void unregister_handler(const std::string& topic, const Token& token) override;

    ///
    /// \brief checks if the given \p full_topic matches the given \p wildcard_topic that can contain "+" and "#"
    /// wildcards
    ///
    /// \returns true if the topic matches, false otherwise
    static bool check_topic_matches(const std::string& full_topic, const std::string& wildcard_topic);

private:
    bool mqtt_is_connected;
    std::atomic_bool running;
    MessageHandler message_handler;
    MessageQueue message_queue;
    std::vector<std::shared_ptr<MessageWithQOS>> messages_before_connected;
    std::mutex messages_before_connected_mutex;
    std::mutex topics_mutex;
    std::mutex topic_request_mutex;
    std::vector<std::string> retained_topics;
    std::unordered_set<std::string> subscribed_topics;

    std::shared_future<void> main_loop_future;

    std::string mqtt_server_socket_path;
    std::string mqtt_server_address;
    std::uint16_t mqtt_server_port = 0;
    std::string mqtt_everest_prefix;
    std::string mqtt_external_prefix;

    std::unique_ptr<everest::lib::io::mqtt::mqtt_client> mqtt_client;
    everest::lib::io::event::event_fd disconnect_event;
    everest::lib::io::event::fd_event_handler ev_handler;

    // This must be destroyed first.
    Thread mqtt_mainloop_thread;

    void on_mqtt_message(const Message& message);
    void on_mqtt_connect();
    static void on_mqtt_disconnect();
    nlohmann::json get_internal(const MQTTRequest& request);
};
} // namespace Everest

#endif // UTILS_MQTT_ABSTRACTION_IMPL_HPP
