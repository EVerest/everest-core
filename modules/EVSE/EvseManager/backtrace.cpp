// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "backtrace.hpp"

#ifdef EVEREST_USE_BACKTRACES

#include <cstring>
#include <execinfo.h>
#include <stdio.h>
#include <unistd.h>

/*
 Simple backtrace signal handler
*/
namespace Everest {

void signal_handler(int signo) {
    void* array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    printf("\n---------------------------------------------------------------------------------------------------\n");
    backtrace_symbols_fd(array, size, STDOUT_FILENO);
    printf("---------------------------------------------------------------------------------------------------\n\n");
}

void install_backtrace_handler() {
    struct sigaction bt_handler;
    memset(&bt_handler, 0, sizeof(bt_handler));

    bt_handler.sa_handler = signal_handler;

    if (sigaction(SIGUSR1, &bt_handler, NULL) < 0) {
        perror("sigaction");
    }
}

void request_backtrace(pthread_t id) {
    pthread_kill(id, SIGUSR1);
}

} // namespace Everest
#endif