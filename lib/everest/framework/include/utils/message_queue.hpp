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
#include <string_view>
#include <thread>
#include <unordered_set>

#include <nlohmann/json.hpp>

#include <utils/types.hpp>

namespace Everest {
/// \brief Contains a payload and the topic it was received on
struct Message {
    std::string topic;   ///< The MQTT topic where this message originated from
    std::string payload; ///< The message payload

    Message(std::string_view topic_param, std::string_view payload_param) : topic(topic_param), payload(payload_param) {
    }
};

struct ParsedMessage {
    std::string topic;
    nlohmann::json data;
};

using MessageCallback = std::function<void(const Message&)>;

} // namespace Everest

#endif // UTILS_MESSAGE_QUEUE_HPP
