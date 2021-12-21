// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef FSM_SPECIALIZATION_POSIX_HPP
#define FSM_SPECIALIZATION_POSIX_HPP

#include <chrono>

#include <fsm/sync.hpp>

namespace fsm {
namespace sync {
namespace posix {
class Repeatable {
public:
    // duration in ms
    Repeatable(int duration) :
        timeout(std::chrono::system_clock::now() + std::chrono::milliseconds(duration)), duration(duration) {
    }
    Repeatable() : Repeatable(0){};

    // ms left until timeout
    int ticks_left() {
        auto diff =
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout - std::chrono::system_clock::now()).count();
        return (diff < 0) ? 0 : diff;
    }

    void repeat_from_now() {
        timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(duration);
    }

    void repeat_from_last() {
        timeout += std::chrono::milliseconds(duration);
    }

private:
    std::chrono::time_point<std::chrono::system_clock> timeout;
    int duration;
};
} // namespace posix

template <typename StateHandleType, void (*LogFunction)(const std::string&) = nullptr>
using PosixController = BasicController<StateHandleType, posix::Repeatable, LogFunction>;

} // namespace sync
} // namespace fsm

#endif // FSM_SPECIALIZATION_POSIX_HPP
