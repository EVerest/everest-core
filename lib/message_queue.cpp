// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <thread>

#include <fmt/format.h>

#include <everest/logging.hpp>

#include <utils/message_queue.hpp>

namespace Everest {

MessageQueue::MessageQueue(MessageCallback message_callback_) :
    message_callback(std::move(message_callback_)), running(true) {
    this->worker_thread = std::thread([this]() {
        while (true) {
            std::unique_lock<std::mutex> lock(this->queue_ctrl_mutex);
            this->cv.wait(lock, [this]() { return !this->message_queue.empty() || this->running == false; });
            if (!this->running) {
                return;
            }

            const auto message = std::move(this->message_queue.front());
            this->message_queue.pop();
            lock.unlock();

            // pass the message to the message callback
            this->message_callback(*message);
        }
    });
}

void MessageQueue::add(std::unique_ptr<Message> message) {
    {
        std::lock_guard<std::mutex> lock(this->queue_ctrl_mutex);
        this->message_queue.push(std::move(message));
    }
    this->cv.notify_all();
}

void MessageQueue::stop() {
    {
        std::lock_guard<std::mutex> lock(this->queue_ctrl_mutex);
        this->running = false;
    }
    this->cv.notify_all();
}

MessageQueue::~MessageQueue() {
    stop();
    worker_thread.join();
}

MessageHandler::MessageHandler() : running(true) {
    this->handler_thread = std::thread([this]() {
        while (true) {
            std::unique_lock<std::mutex> lock(this->handler_ctrl_mutex);
            this->cv.wait(lock, [this]() { return !this->message_queue.empty() || this->running == false; });
            if (!this->running) {
                return;
            }

            const auto message = std::move(this->message_queue.front());
            this->message_queue.pop();
            lock.unlock();

            const auto& data = message->data;

            // get the registered handlers
            std::vector<std::shared_ptr<TypedHandler>> local_handlers;
            {
                const std::lock_guard<std::mutex> handlers_lock(handler_list_mutex);
                for (auto handler : this->handlers) {
                    local_handlers.push_back(handler);
                }
            }

            // distribute this message to the registered handlers
            for (auto& handler : local_handlers) {
                auto handler_fn = *handler->handler;

                if (handler->type == HandlerType::Call) {
                    // unpack call
                    if (handler->name != data.at("name")) {
                        continue;
                    }
                    if (data.at("type") == "call") {
                        handler_fn(message->topic, data.at("data"));
                    }
                } else if (handler->type == HandlerType::Result) {
                    // unpack result
                    if (handler->name != data.at("name")) {
                        continue;
                    }
                    if (data.at("type") == "result") {
                        // only deliver result to handler with matching id
                        if (handler->id == data.at("data").at("id")) {
                            handler_fn(message->topic, data.at("data"));
                        }
                    }
                } else if (handler->type == HandlerType::SubscribeVar) {
                    // unpack var
                    if (handler->name != data.at("name")) {
                        continue;
                    }
                    handler_fn(message->topic, data.at("data"));
                } else {
                    // external or unknown, no preprocessing
                    handler_fn(message->topic, data);
                }
            }
        }
    });
}

void MessageHandler::add(std::shared_ptr<ParsedMessage> message) {
    {
        std::lock_guard<std::mutex> lock(this->handler_ctrl_mutex);
        this->message_queue.push(std::move(message));
    }
    this->cv.notify_all();
}

void MessageHandler::stop() {
    {
        std::lock_guard<std::mutex> lock(this->handler_ctrl_mutex);
        this->running = false;
    }
    this->cv.notify_all();
}

void MessageHandler::add_handler(std::shared_ptr<TypedHandler> handler) {
    {
        std::lock_guard<std::mutex> lock(this->handler_list_mutex);
        this->handlers.insert(handler);
    }
}

void MessageHandler::remove_handler(std::shared_ptr<TypedHandler> handler) {
    {
        std::lock_guard<std::mutex> lock(this->handler_list_mutex);
        auto it = std::find(this->handlers.begin(), this->handlers.end(), handler);
        this->handlers.erase(it);
    }
}

size_t MessageHandler::count_handlers() {
    size_t count = 0;
    {
        std::lock_guard<std::mutex> lock(this->handler_list_mutex);
        count = this->handlers.size();
    }
    return count;
}

MessageHandler::~MessageHandler() {
    stop();
    handler_thread.join();
}

} // namespace Everest
