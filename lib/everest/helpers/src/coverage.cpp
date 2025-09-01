// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <everest/helpers/coverage.hpp>

#include <atomic>

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

extern "C" void __gcov_dump();

namespace {

static std::atomic_flag going_to_terminate = ATOMIC_FLAG_INIT;

void terminate_handler(int signal) {
    if (going_to_terminate.test_and_set()) {
        return;
    }

    __gcov_dump();

    _exit(EXIT_FAILURE);
};

} // namespace

namespace everest::helpers {

void install_signal_handlers_for_gcov() {
    struct sigaction action {};
    action.sa_handler = &terminate_handler;
    // action.sa_mask should be zero, so no blocked signals within the signal handler
    // action.sa_flags should be fine with being zero
    sigaction(SIGINT, &action, nullptr);
    sigaction(SIGTERM, &action, nullptr);
}

} // namespace everest::helpers
