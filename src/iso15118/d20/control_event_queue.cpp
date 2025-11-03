// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/control_event_queue.hpp>

namespace iso15118::d20 {

std::optional<ControlEvent> ControlEventQueue::pop() {
    std::lock_guard<std::mutex> lck(mutex);

    if (queue.empty()) {
        return std::nullopt;
    }

    auto event = std::make_optional<ControlEvent>(std::move(queue.front()));
    queue.pop();

    return event;
}

void ControlEventQueue::push(ControlEvent event) {
    std::lock_guard<std::mutex> lck(mutex);

    queue.push(std::move(event));
}

} // namespace iso15118::d20
