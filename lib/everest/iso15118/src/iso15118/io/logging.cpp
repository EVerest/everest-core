// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/io/logging.hpp>

#include <cstdarg>
#include <cstdio>
#include <iostream>

static std::function<void(iso15118::LogLevel, std::string)> logging_callback = [](const iso15118::LogLevel& level,
                                                                                  const std::string& msg) {
    std::cout << msg << ", level: " << static_cast<int>(level);
};

namespace iso15118 {

void log(const LogLevel& level, const std::string& msg) {
    logging_callback(level, msg);
}

void vlogf(const char* fmt, va_list ap) {
    static constexpr auto MAX_FMT_LOG_BUFSIZE = 1024;
    char msg_buf[MAX_FMT_LOG_BUFSIZE];

    vsnprintf(msg_buf, MAX_FMT_LOG_BUFSIZE, fmt, ap);

    log(LogLevel::Info, msg_buf);
}

void vlogf(const LogLevel& level, const char* fmt, va_list ap) {
    static constexpr auto MAX_FMT_LOG_BUFSIZE = 1024;
    char msg_buf[MAX_FMT_LOG_BUFSIZE];

    vsnprintf(msg_buf, MAX_FMT_LOG_BUFSIZE, fmt, ap);

    log(level, msg_buf);
}

void logf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    vlogf(fmt, args);

    va_end(args);
}

void logf(const LogLevel& level, const char* fmt, ...) {

    va_list args;
    va_start(args, fmt);

    vlogf(level, fmt, args);

    va_end(args);
}

void logf_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vlogf(LogLevel::Error, fmt, args);
    va_end(args);
}
void logf_warning(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vlogf(LogLevel::Warning, fmt, args);
    va_end(args);
}
void logf_info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vlogf(LogLevel::Info, fmt, args);
    va_end(args);
}
void logf_debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vlogf(LogLevel::Debug, fmt, args);
    va_end(args);
}
void logf_trace(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vlogf(LogLevel::Trace, fmt, args);
    va_end(args);
}

namespace io {
void set_logging_callback(const std::function<void(LogLevel, std::string)>& callback) {
    logging_callback = callback;
}
} // namespace io

} // namespace iso15118
