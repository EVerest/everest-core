// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <thread>

#include <fmt/format.h>

#include <everest/logging.hpp>

#include <utils/message_queue.hpp>

namespace Everest {

MessageQueue::MessageQueue(MessageCallback message_callback_) : message_callback(std::move(message_callback_)) {
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
        const std::lock_guard<std::mutex> lock(this->queue_ctrl_mutex);
        this->message_queue.push(std::move(message));
    }
    this->cv.notify_all();
}

void MessageQueue::stop() {
    {
        const std::lock_guard<std::mutex> lock(this->queue_ctrl_mutex);
        this->running = false;
    }
    this->cv.notify_all();
}

MessageQueue::~MessageQueue() {
    stop();
    worker_thread.join();
}

} // namespace Everest
