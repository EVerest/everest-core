// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVENTQUEUE_HPP
#define EVENTQUEUE_HPP

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <vector>

namespace module {

template <typename E> class EventQueue {
public:
    using events_t = std::vector<E>;

private:
    events_t pending;
    std::mutex mux;
    std::condition_variable cv;

public:
    void push(const E& event) {
        {
            std::lock_guard<std::mutex> lock(mux);
            pending.push_back(event);
        }
        cv.notify_all();
    }

    events_t get_events() {
        std::lock_guard<std::mutex> lock(mux);
        events_t active;
        pending.swap(active);
        return active;
    }

    events_t wait() {
        std::unique_lock<std::mutex> ul(mux);
        cv.wait(ul, [this]() { return !pending.empty(); });
        events_t active;
        pending.swap(active);
        ul.unlock();
        return active;
    }
};

} // namespace module
#endif
