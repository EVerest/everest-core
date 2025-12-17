// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace ocpp {

/// \brief Thread safe message queue. Holds a conditional variable
/// that can be waited upon. Will take up the waiting thread on each
/// operation of push/pop/clear
template <typename T> class SafeQueue {
public:
    /// \return True if the queue is empty
    bool empty() const {
        const std::lock_guard lock(mutex);
        return queue.empty();
    }

    /// \brief We return a copy here, since while might be accessing the
    /// reference while another thread uses pop and makes the reference stale
    T front() {
        const std::lock_guard lock(mutex);
        return queue.front();
    }

    /// \return retrieves and removes the first element in the queue. Undefined behavior if the queue is empty
    T pop() {
        std::unique_lock<std::mutex> lock(mutex);

        T front = std::move(queue.front());
        queue.pop();

        // Unlock here and notify
        lock.unlock();

        notify_waiting_thread();

        return front;
    }

    /// \brief Queues an element and notifies any threads waiting on the internal conditional variable
    void push(T&& value) {
        {
            const std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::move(value));
        }

        notify_waiting_thread();
    }

    /// \brief Queues an element and notifies any threads waiting on the internal conditional variable
    void push(const T& value) {
        {
            const std::lock_guard<std::mutex> lock(mutex);
            queue.push(value);
        }

        notify_waiting_thread();
    }

    /// \brief Clears the queue
    void clear() {
        {
            const std::lock_guard<std::mutex> lock(mutex);

            std::queue<T> empty;
            empty.swap(queue);
        }

        notify_waiting_thread();
    }

    /// \brief Waits for the queue to receive an element
    /// \param timeout to wait for an element, pass in a value <= 0 to wait indefinitely
    void wait_on_queue_element(std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
        wait_on_queue_element_or_predicate([]() { return false; }, timeout);
    }

    /// \brief Same as 'wait_on_queue' but receives an additional predicate to wait upon
    template <class Predicate>
    void wait_on_queue_element_or_predicate(Predicate pred,
                                            std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
        std::unique_lock<std::mutex> lock(mutex);

        if (timeout.count() > 0) {
            cv.wait_for(lock, timeout, [&]() { return (false == queue.empty()) or pred(); });
        } else {
            cv.wait(lock, [&]() { return (false == queue.empty()) or pred(); });
        }
    }

    /// \brief Waits on the queue for a custom event
    /// \param timeout to wait for an element, pass in a value <= 0 to wait indefinitely
    template <class Predicate>
    void wait_on_custom_event(Predicate pred, std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
        std::unique_lock<std::mutex> lock(mutex);

        if (timeout.count() > 0) {
            cv.wait_for(lock, timeout, [&]() { return pred(); });
        } else {
            cv.wait(lock, [&]() { return pred(); });
        }
    }

    /// \brief Notifies a single waiting thread to wake up
    void notify_waiting_thread() {
        cv.notify_one();
    }

private:
    std::queue<T> queue;

    mutable std::mutex mutex;
    std::condition_variable cv;
};

} // namespace ocpp