// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <mutex>
#include <optional>
#include <queue>

namespace ieee2030::events {

template <class T> class EventQueue {
public:
    std::optional<T> pop() {
        std::lock_guard<std::mutex> lck(mutex);

        if (queue.empty()) {
            return std::nullopt;
        }

        auto event = std::make_optional<T>(std::move(queue.front()));
        queue.pop();

        return event;
    };

    void push(T event) {
        std::lock_guard<std::mutex> lck(mutex);

        queue.push(std::move(event));
    };

private:
    std::queue<T> queue;
    std::mutex mutex;
};

} // namespace ieee2030::events
