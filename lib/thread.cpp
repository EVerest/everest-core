// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <utils/thread.hpp>

namespace Everest {

Thread::~Thread() {
    stop();
}

void Thread::stop() {
    if (handle.joinable()) {
        exit_signal = true;
        handle.join();
    }
    exit_signal = false;
}

bool Thread::shouldExit() {
    return exit_signal;
}

void Thread::operator=(std::thread&& t) {
    stop();
    handle = std::move(t);
}

} // namespace Everest
