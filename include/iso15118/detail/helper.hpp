// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <string>

namespace iso15118 {

void logf(const char* fmt, ...);

void vlogf(const char* fmt, va_list ap);

void log(const std::string&);

void log_and_throw(const char* msg);

void log_and_raise_mbed_error(const std::string& error_msg, int error_code);

void log_and_raise_openssl_error(const std::string& error_msg);

std::string adding_err_msg(const std::string& msg);

template <typename CallbackType, typename... Args> bool call_if_available(const CallbackType& callback, Args... args) {
    if (not callback) {
        return false;
    }

    callback(std::forward<Args>(args)...);
    return true;
}

} // namespace iso15118
