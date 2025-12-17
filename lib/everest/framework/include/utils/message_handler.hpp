// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include <utils/message_queue.hpp>
#include <utils/types.hpp>

using MqttTopic = std::string;
using CmdId = std::string;

namespace Everest {

/// \brief Handles message dispatching and thread-safe queuing of different message types. This class uses two separate
/// threads and message queues: one for operation messages (vars, cmds, errors, GetConfig, ModuleReady) and one for
/// result messages (cmd results, GetConfig responses).
class MessageHandler {
public:
    MessageHandler();
    ~MessageHandler();

    /// \brief Adds given \p message to the message queue for processing
    void add(const ParsedMessage& message);

    /// \brief Stops all threads started by this handler
    void stop();

    /// \brief Registers a \p handler for a specific \p topic
    void register_handler(const std::string& topic, std::shared_ptr<TypedHandler> handler);

private:
    void run_operation_message_worker();
    void run_result_message_worker();

    void handle_operation_message(const std::string& topic, const json& payload);
    void handle_result_message(const std::string& topic, const json& payload);

    // Individual message handler methods
    void handle_var_message(const std::string& topic, const json& data);
    void handle_cmd_message(const std::string& topic, const json& data);
    void handle_external_mqtt_message(const std::string& topic, const json& data);
    void handle_error_message(const std::string& topic, const json& data);
    void handle_get_config_message(const std::string& topic, const json& data);
    void handle_module_ready_message(const std::string& topic, const json& data);
    void handle_cmd_result(const std::string& topic, const json& payload);
    void handle_get_config_response(const std::string& topic, const json& payload);

    // Helper methods for handler execution
    template <typename HandlerMap, typename ExecuteFn>
    void execute_handlers_from_vector(HandlerMap& handlers, const std::string& topic, ExecuteFn execute_fn);

    template <typename HandlerMap, typename ExecuteFn>
    void execute_single_handler(HandlerMap& handlers, const std::string& topic, ExecuteFn execute_fn);

    // Threads
    std::thread
        operation_worker_thread; // processes vars, commands, external MQTT, errors, GetConfig and ModuleReady messages
    std::thread result_worker_thread; // processes cmd results and GetConfig responses
    std::thread ready_thread;         // runs the modules ready function

    // Queues and sync primitives
    std::queue<ParsedMessage> operation_message_queue;
    std::queue<ParsedMessage> result_message_queue;

    std::mutex operation_queue_mutex;
    std::condition_variable operation_cv;

    std::mutex result_queue_mutex;
    std::condition_variable result_cv;

    std::mutex cmd_result_handler_mutex;
    std::mutex handler_mutex;

    std::atomic<bool> running = true;

    // Handler data structures
    std::map<MqttTopic, std::vector<std::shared_ptr<TypedHandler>>> var_handlers; // var handlers of module
    std::map<MqttTopic, std::shared_ptr<TypedHandler>> cmd_handlers;              // cmd handlers of module
    std::map<CmdId, std::shared_ptr<TypedHandler>> cmd_result_handlers;           // cmd result handlers of module
    std::map<MqttTopic, std::shared_ptr<TypedHandler>> error_handlers;            // error handlers of module
    std::map<MqttTopic, std::shared_ptr<TypedHandler>>
        get_module_config_handlers;                        // get module config handler of manager
    std::shared_ptr<TypedHandler> config_response_handler; // get module config response handler of module
    std::shared_ptr<TypedHandler> global_ready_handler;    // global ready handler of module
    std::map<MqttTopic, std::shared_ptr<TypedHandler>> module_ready_handlers; // module ready handlers of manager
    std::map<MqttTopic, std::vector<std::shared_ptr<TypedHandler>>>
        external_var_handlers; // external MQTT handlers of module
};

} // namespace Everest
