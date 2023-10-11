// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_THREAD_HPP
#define UTILS_THREAD_HPP

#include <chrono>
#include <future>
#include <thread>

namespace Everest {
class Thread {
public:
    ~Thread();

    void stop();

    bool shouldExit();
    void operator=(std::thread&&);

private:
    std::thread handle;
    std::atomic_bool exit_signal{false};
};
} // namespace Everest

#endif // UTILS_THREAD_HPP
