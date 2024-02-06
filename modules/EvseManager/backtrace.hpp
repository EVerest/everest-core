// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVEREST_BACKTRACE
#define EVEREST_BACKTRACE
#ifdef __linux__
#include <signal.h>

/*
 Simple backtrace signal handler
*/
namespace Everest {
void signal_handler(int signo);
void install_backtrace_handler();
void request_backtrace(pthread_t id);
} // namespace Everest
#endif
#endif