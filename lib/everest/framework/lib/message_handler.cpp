// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/message_handler.hpp>

#include <everest/logging.hpp>
#include <fmt/format.h>

namespace Everest {

namespace {
// Helper to split string by delimiter
std::vector<std::string> split_topic(const std::string& topic, char delimiter = '/') {
    std::vector<std::string> result;
    std::istringstream stream(topic);
    std::string part;
    while (std::getline(stream, part, delimiter)) {
        result.push_back(part);
    }
    return result;
}

// NOLINTNEXTLINE(misc-no-recursion)
bool check_topic_matches(const std::string& full_topic, const std::string& wildcard_topic) {
    // Verbatim match
    if (full_topic == wildcard_topic) {
        return true;
    }

    // Check if wildcard ends with "/#" and matches base
    if (wildcard_topic.size() >= 2 && wildcard_topic.compare(wildcard_topic.size() - 2, 2, "/#") == 0) {
        std::string start = wildcard_topic.substr(0, wildcard_topic.size() - 2);
        if (check_topic_matches(full_topic, start)) {
            return true;
        }
    }

    std::vector<std::string> full_split = split_topic(full_topic);
    std::vector<std::string> wildcard_split = split_topic(wildcard_topic);

    for (std::size_t partno = 0; partno < full_split.size(); ++partno) {
        if (partno >= wildcard_split.size()) {
            return false;
        }

        const std::string& full_part = full_split[partno];
        const std::string& wildcard_part = wildcard_split[partno];

        if (wildcard_part == "#") {
            return true;
        }

        if (wildcard_part == "+" || wildcard_part == full_part) {
            continue;
        }

        return false;
    }

    return full_split.size() == wildcard_split.size();
}
} // namespace

MessageHandler::MessageHandler() {
    operation_worker_thread = std::thread([this] { run_operation_message_worker(); });
    result_worker_thread = std::thread([this] { run_result_message_worker(); });
}

MessageHandler::~MessageHandler() {
    stop();
}

void MessageHandler::add(const ParsedMessage& message) {
    EVLOG_debug << "Adding message to queue: " << message.topic << " with data: " << message.data;

    MqttMessageType msg_type = MqttMessageType::ExternalMQTT; // Default to ExternalMQTT if msg_type is not present

    if (message.data.is_object() && message.data.contains("msg_type")) {
        msg_type = string_to_mqtt_message_type(message.data.at("msg_type").get<std::string>());
    }

    if (msg_type == MqttMessageType::CmdResult || msg_type == MqttMessageType::GetConfigResponse) {
        EVLOG_verbose << "Pushing cmd_result message to queue: " << message.data;
        {
            std::lock_guard<std::mutex> lock(result_queue_mutex);
            result_message_queue.push(message);
        }
        result_cv.notify_all();
    } else if (msg_type == MqttMessageType::GlobalReady) {
        const auto topic_copy = message.topic;
        const auto data_copy = message.data.at("data");

        ready_thread =
            std::thread([this, topic_copy, data_copy] { (*global_ready_handler->handler)(topic_copy, data_copy); });
    } else {
        {
            std::lock_guard<std::mutex> lock(operation_queue_mutex);
            operation_message_queue.push(message);
        }
        operation_cv.notify_all();
    }
}

void MessageHandler::stop() {
    {
        std::lock_guard<std::mutex> lock1(operation_queue_mutex);
        std::lock_guard<std::mutex> lock2(result_queue_mutex);
        running = false;
    }

    operation_cv.notify_all();
    result_cv.notify_all();

    if (operation_worker_thread.joinable()) {
        operation_worker_thread.join();
    }
    if (result_worker_thread.joinable()) {
        result_worker_thread.join();
    }
    if (ready_thread.joinable()) {
        ready_thread.join();
    }
}

void MessageHandler::run_operation_message_worker() {
    while (true) {
        std::unique_lock<std::mutex> lock(operation_queue_mutex);
        operation_cv.wait(lock, [this] { return !operation_message_queue.empty() || !running; });
        if (!running)
            return;

        ParsedMessage message = std::move(operation_message_queue.front());
        operation_message_queue.pop();
        lock.unlock();

        handle_operation_message(message.topic, message.data);
    }
    EVLOG_info << "Main worker thread stopped";
}

void MessageHandler::run_result_message_worker() {
    while (true) {
        std::unique_lock<std::mutex> lock(result_queue_mutex);
        result_cv.wait(lock, [this] { return !result_message_queue.empty() || !running; });
        if (!running) {
            return;
        }

        ParsedMessage message = std::move(result_message_queue.front());
        result_message_queue.pop();
        lock.unlock();

        handle_result_message(message.topic, message.data);
    }
    EVLOG_info << "Cmd result worker thread stopped";
}

void MessageHandler::handle_operation_message(const std::string& topic, const json& payload) {
    json data;
    MqttMessageType msg_type = MqttMessageType::ExternalMQTT;

    // Determine message type
    if (payload.contains("msg_type")) {
        msg_type = string_to_mqtt_message_type(payload.at("msg_type").get<std::string>());
    }

    if (payload.contains("data")) {
        data = payload.at("data");
    } else {
        data = payload;
    }

    switch (msg_type) {
    case MqttMessageType::Var:
        handle_var_message(topic, data);
        break;
    case MqttMessageType::Cmd:
        handle_cmd_message(topic, data);
        break;
    case MqttMessageType::ExternalMQTT:
        handle_external_mqtt_message(topic, data);
        break;
    case MqttMessageType::RaiseError:
    case MqttMessageType::ClearError:
        handle_error_message(topic, data);
        break;
    case MqttMessageType::GetConfig:
        handle_get_config_message(topic, data);
        break;
    case MqttMessageType::ModuleReady:
        handle_module_ready_message(topic, data);
        break;
    default:
        break;
    }
}

void MessageHandler::handle_result_message(const std::string& topic, const json& payload) {
    if (!payload.contains("msg_type")) {
        EVLOG_warning << "Received cmd_result message without msg_type: " << payload;
        return;
    }

    const auto msg_type = string_to_mqtt_message_type(payload.at("msg_type").get<std::string>());

    if (msg_type == MqttMessageType::CmdResult) {
        handle_cmd_result(topic, payload);
    } else if (msg_type == MqttMessageType::GetConfigResponse) {
        handle_get_config_response(topic, payload);
    } else {
        EVLOG_warning << "Received invalid cmd_result message: " << payload;
    }
}

void MessageHandler::register_handler(const std::string& topic, std::shared_ptr<TypedHandler> handler) {
    switch (handler->type) {
    case HandlerType::Call: {
        std::lock_guard<std::mutex> lg(handler_mutex);
        cmd_handlers[topic] = handler;
        break;
    }
    case HandlerType::Result: {
        std::lock_guard<std::mutex> lock(cmd_result_handler_mutex);
        cmd_result_handlers[handler->id] = handler;
        break;
    }
    case HandlerType::SubscribeVar: {
        std::lock_guard<std::mutex> lg(handler_mutex);
        var_handlers[topic].push_back(handler);
        break;
    }
    case HandlerType::SubscribeError: {
        std::lock_guard<std::mutex> lg(handler_mutex);
        error_handlers[topic] = handler;
        break;
    }
    case HandlerType::ExternalMQTT: {
        std::lock_guard<std::mutex> lg(handler_mutex);
        external_var_handlers[topic].push_back(handler);
        break;
    }
    case HandlerType::GetConfig: {
        std::lock_guard<std::mutex> lg(handler_mutex);
        get_module_config_handlers[topic] = handler;
        break;
    }
    case HandlerType::GetConfigResponse: {
        std::lock_guard<std::mutex> lg(cmd_result_handler_mutex);
        config_response_handler = handler;
        break;
    }
    case HandlerType::ModuleReady: {
        std::lock_guard<std::mutex> lg(handler_mutex);
        module_ready_handlers[topic] = handler;
        break;
    }
    case HandlerType::GlobalReady: {
        std::lock_guard<std::mutex> lg(handler_mutex);
        global_ready_handler = handler;
        break;
    }
    default:
        EVLOG_warning << "Unknown handler type for topic: " << topic;
        break;
    }
}

// Private message handler methods
void MessageHandler::handle_var_message(const std::string& topic, const json& data) {
    execute_handlers_from_vector(var_handlers, topic,
                                 [&](const auto& handler) { (*handler->handler)(topic, data.at("data")); });
}

void MessageHandler::handle_cmd_message(const std::string& topic, const json& data) {
    execute_single_handler(cmd_handlers, topic, [&](const auto& handler) { (*handler->handler)(topic, data); });
}

void MessageHandler::handle_external_mqtt_message(const std::string& topic, const json& data) {
    execute_handlers_from_vector(external_var_handlers, topic,
                                 [&](const auto& handler) { (*handler->handler)(topic, data); });
}

void MessageHandler::handle_error_message(const std::string& topic, const json& data) {
    std::vector<std::pair<std::string, std::shared_ptr<TypedHandler>>> matching_handlers;
    {
        std::lock_guard<std::mutex> lock(handler_mutex);
        for (const auto& [_topic, error_handler] : error_handlers) {
            if (check_topic_matches(topic, _topic)) {
                matching_handlers.emplace_back(_topic, error_handler);
            }
        }
    }
    for (const auto& [_topic, handler] : matching_handlers) {
        (*handler->handler)(topic, data);
    }
}

void MessageHandler::handle_get_config_message(const std::string& topic, const json& data) {
    execute_single_handler(get_module_config_handlers, topic,
                           [&](const auto& handler) { (*handler->handler)(topic, data); });
}

void MessageHandler::handle_module_ready_message(const std::string& topic, const json& data) {
    execute_single_handler(module_ready_handlers, topic,
                           [&](const auto& handler) { (*handler->handler)(topic, data); });
}

void MessageHandler::handle_cmd_result(const std::string& topic, const json& payload) {
    const auto& data = payload.at("data").at("data");
    const auto id = data.at("id").get<std::string>();

    std::shared_ptr<TypedHandler> handler_copy;
    {
        std::lock_guard<std::mutex> lock(cmd_result_handler_mutex);
        auto it = cmd_result_handlers.find(id);
        if (it != cmd_result_handlers.end()) {
            handler_copy = it->second;
            cmd_result_handlers.erase(it);
        }
    }

    if (handler_copy) {
        (*handler_copy->handler)(topic, data);
    }
}

void MessageHandler::handle_get_config_response(const std::string& topic, const json& payload) {
    std::shared_ptr<TypedHandler> handler_copy;
    {
        std::lock_guard<std::mutex> lock(cmd_result_handler_mutex);
        if (config_response_handler) {
            handler_copy = config_response_handler;
        }
    }
    if (handler_copy) {
        (*handler_copy->handler)(topic, payload.at("data"));
    }
}

// Helper methods for handler execution
template <typename HandlerMap, typename ExecuteFn>
void MessageHandler::execute_handlers_from_vector(HandlerMap& handlers, const std::string& topic,
                                                  ExecuteFn execute_fn) {
    std::vector<std::shared_ptr<TypedHandler>> handlers_copy;
    {
        std::lock_guard<std::mutex> lock(handler_mutex);
        const auto it = handlers.find(topic);
        if (it != handlers.end()) {
            handlers_copy = it->second;
        }
    }
    for (const auto& handler : handlers_copy) {
        execute_fn(handler);
    }
}

template <typename HandlerMap, typename ExecuteFn>
void MessageHandler::execute_single_handler(HandlerMap& handlers, const std::string& topic, ExecuteFn execute_fn) {
    std::shared_ptr<TypedHandler> handler_copy;
    {
        std::lock_guard<std::mutex> lock(handler_mutex);
        const auto it = handlers.find(topic);
        if (it != handlers.end()) {
            handler_copy = it->second;
        }
    }
    if (handler_copy) {
        execute_fn(handler_copy);
    }
}

} // namespace Everest
