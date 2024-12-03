// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_MESSAGE_QUEUE_HPP
#define UTILS_MESSAGE_QUEUE_HPP

#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_set>

#include <nlohmann/json.hpp>

#include <utils/types.hpp>

namespace Everest {
using json = nlohmann::json;

/// \brief Contains a payload and the topic it was received on
struct Message {
    std::string topic;   ///< The MQTT topic where this message originated from
    std::string payload; ///< The message payload
};

struct ParsedMessage {
    std::string topic;
    json data;
};

using MessageCallback = std::function<void(const Message&)>;

/// \brief Simple message queue that takes std::string messages, parsed them and dispatches them to handlers
class MessageQueue {

private:
    std::thread worker_thread;
    std::queue<std::unique_ptr<Message>> message_queue;
    std::mutex queue_ctrl_mutex;
    MessageCallback message_callback;
    std::condition_variable cv;
    bool running;

public:
    /// \brief Creates a message queue with the provided \p message_callback
    explicit MessageQueue(MessageCallback);
    ~MessageQueue();

    /// \brief Adds a \p message to the message queue which will then be delivered to the message callback
    void add(std::unique_ptr<Message>);

    /// \brief Stops the message queue
    void stop();
};

/// \brief Contains a message queue driven list of handler callbacks
class MessageHandler {
private:
    std::unordered_set<std::shared_ptr<TypedHandler>> handlers;
    std::thread handler_thread;
    std::queue<std::shared_ptr<ParsedMessage>> message_queue;
    std::mutex handler_ctrl_mutex;
    std::mutex handler_list_mutex;
    std::condition_variable cv;
    bool running;

public:
    /// \brief Creates the message handler
    MessageHandler();

    /// \brief Destructor
    ~MessageHandler();

    /// \brief Adds a \p message to the message queue which will be delivered to the registered handlers
    void add(std::shared_ptr<ParsedMessage>);

    /// \brief Stops the message handler
    void stop();

    /// \brief Adds a \p handler that will receive messages from the queue.
    void add_handler(std::shared_ptr<TypedHandler> handler);

    /// \brief Removes a specific \p handler
    void remove_handler(std::shared_ptr<TypedHandler> handler);

    /// \brief \returns the number of registered handlers
    size_t count_handlers();
};

} // namespace Everest

#endif // UTILS_MESSAGE_QUEUE_HPP
