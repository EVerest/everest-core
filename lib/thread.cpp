// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <utils/thread.hpp>

namespace Everest {
Thread::Thread() {
    exitFuture = exitSignal.get_future();
}

Thread::~Thread() {
    stop();
}

void Thread::stop() {
    exitSignal.set_value();
    if (handle.joinable())
        handle.join();
}

bool Thread::shouldExit() {
    if (exitFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        return true;
    return false;
}

void Thread::operator=(std::thread&& t) {
    handle = std::move(t);
}

} // namespace Everest
