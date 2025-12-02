// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef UTILS_MESSAGE_QUEUE_HPP
#define UTILS_MESSAGE_QUEUE_HPP

#include <condition_variable>
#include <cstddef>
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
    bool running = true;

public:
    /// \brief Creates a message queue with the provided \p message_callback
    explicit MessageQueue(MessageCallback);
    ~MessageQueue();

    /// \brief Adds a \p message to the message queue which will then be delivered to the message callback
    void add(std::unique_ptr<Message>);

    /// \brief Stops the message queue
    void stop();
};

} // namespace Everest

#endif // UTILS_MESSAGE_QUEUE_HPP
