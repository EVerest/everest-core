// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include "log.hpp"

#include <cstdarg>
#include <cstdio>

void dlog_func(const dloglevel_t loglevel, const char* filename, const int linenumber, const char* functionname,
               const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    (void)std::vfprintf(stderr, format, ap);
    va_end(ap);
    (void)std::fprintf(stderr, "\n");
}
