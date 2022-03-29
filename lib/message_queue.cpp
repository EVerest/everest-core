// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <thread>

#include <fmt/format.h>

#include <everest/logging.hpp>

#include <utils/message_queue.hpp>

namespace Everest {

Message::Message(const std::string& topic, const std::string& payload) : topic(topic), payload(payload) {
}

MessageQueue::MessageQueue(const std::function<void(std::shared_ptr<Message> message)>& message_callback) :
    message_callback(message_callback), running(true) {
    this->worker_thread = std::thread([this]() {
        while (this->running) {
            std::shared_ptr<Message> message;
            std::unique_lock<std::mutex> lock(this->message_mutex);
            this->cv.wait(lock, [this]() { return !this->message_queue.empty(); });

            message = this->message_queue.front();
            this->message_queue.pop();
            lock.unlock();

            // pass the message to the message callback
            this->message_callback(message);
        }
    });
}

void MessageQueue::add(std::shared_ptr<Message> message) {
    {
        std::lock_guard<std::mutex> lock(this->message_mutex);
        this->message_queue.push(message);
    }
    this->cv.notify_all();
}

void MessageQueue::stop() {
    this->running = false;
}

MessageHandler::MessageHandler() : running(true) {
    this->handler_thread = std::thread([this]() {
        while (this->running) {
            std::shared_ptr<json> message;
            std::unique_lock<std::mutex> lock(this->message_mutex);
            this->cv.wait(lock, [this]() { return !this->message_queue.empty(); });

            message = this->message_queue.front();
            this->message_queue.pop();
            lock.unlock();

            auto data = *message.get();

            // get the registered handlers
            std::vector<std::shared_ptr<TypedHandler>> local_handlers;
            {
                const std::lock_guard<std::mutex> handlers_lock(handlers_mutex);
                for (auto handler : this->handlers) {
                    local_handlers.push_back(handler);
                }
            }

            // distribute this message to the registered handlers
            for (auto handler_ : local_handlers) {
                auto handler = *handler_.get()->handler;

                if (handler_->type == HandlerType::Call) {
                    // unpack call
                    if (handler_->name != data.at("name")) {
                        continue;
                    }
                    if (data.at("type") == "call") {
                        handler(data.at("data"));
                    }
                } else if (handler_->type == HandlerType::Result) {
                    // unpack result
                    if (handler_->name != data.at("name")) {
                        continue;
                    }
                    if (data.at("type") == "result") {
                        // only deliver result to handler with matching id
                        if (handler_->id == data.at("data").at("id")) {
                            handler(data.at("data"));
                        }
                    }
                } else if (handler_->type == HandlerType::SubscribeVar) {
                    // unpack var
                    if (handler_->name != data.at("name")) {
                        continue;
                    }
                    handler(data.at("data"));
                } else {
                    // external or unknown, no preprocessing
                    handler(data);
                }
            }
        }
    });
}

void MessageHandler::add(std::shared_ptr<json> message) {
    {
        std::lock_guard<std::mutex> lock(this->message_mutex);
        this->message_queue.push(message);
    }
    this->cv.notify_all();
}

void MessageHandler::stop() {
    this->running = false;
}

void MessageHandler::add_handler(std::shared_ptr<TypedHandler> handler) {
    {
        std::lock_guard<std::mutex> lock(this->handlers_mutex);
        this->handlers.insert(handler);
    }
}

void MessageHandler::remove_handler(std::shared_ptr<TypedHandler> handler) {
    {
        std::lock_guard<std::mutex> lock(this->handlers_mutex);
        auto it = std::find(this->handlers.begin(), this->handlers.end(), handler);
        this->handlers.erase(it);
    }
}

size_t MessageHandler::count_handlers() {
    size_t count = 0;
    {
        std::lock_guard<std::mutex> lock(this->handlers_mutex);
        count = this->handlers.size();
    }
    return count;
}

} // namespace Everest
