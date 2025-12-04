// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/io/log_levels.hpp>

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>

namespace iso15118 {

void logf(const char* fmt, ...);
void logf(const LogLevel&, const char* fmt, ...);

void logf_error(const char* fmt, ...);
void logf_warning(const char* fmt, ...);
void logf_info(const char* fmt, ...);
void logf_debug(const char* fmt, ...);
void logf_trace(const char* fmt, ...);

void vlogf(const char* fmt, va_list ap);
void vlogf(const LogLevel&, const char* fmt, va_list ap);

void log(const LogLevel&, const std::string&);

void log_and_throw(const char* msg);

std::string adding_err_msg(const std::string& msg);

template <typename CallbackType, typename... Args> bool call_if_available(const CallbackType& callback, Args... args) {
    if (not callback) {
        return false;
    }

    std::invoke(callback, std::forward<Args>(args)...);
    return true;
}

// Note(sl): Copied from https://en.cppreference.com/w/cpp/utility/intcmp because libiso15118 uses C++17
template <class T, class U> constexpr bool cmp_equal(T t, U u) noexcept {
    if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
        return t == u;
    else if constexpr (std::is_signed_v<T>)
        return t >= 0 && std::make_unsigned_t<T>(t) == u;
    else
        return u >= 0 && std::make_unsigned_t<U>(u) == t;
}

} // namespace iso15118
