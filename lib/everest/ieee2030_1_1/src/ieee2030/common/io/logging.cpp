// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <ieee2030/common/io/logging.hpp>

#include <cstdarg>
#include <iostream>

static std::function<void(std::string)> logging_callback = [](const std::string& msg) { std::cout << msg; };

namespace ieee2030 {

void log(const std::string& msg) {
    logging_callback(msg);
}

void vlogf(const char* fmt, va_list ap) {
    static constexpr auto MAX_FMT_LOG_BUFSIZE = 1024;
    char msg_buf[MAX_FMT_LOG_BUFSIZE];

    vsnprintf(msg_buf, MAX_FMT_LOG_BUFSIZE, fmt, ap);

    log(msg_buf);
}

void logf(const char* fmt, ...) {

    va_list args;
    va_start(args, fmt);

    vlogf(fmt, args);

    va_end(args);
}

namespace io {
void set_logging_callback(const std::function<void(std::string)>& callback) {
    logging_callback = callback;
}
} // namespace io

} // namespace ieee2030