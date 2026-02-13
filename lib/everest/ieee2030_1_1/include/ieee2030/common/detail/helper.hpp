// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdarg>
#include <string>

namespace ieee2030 {

void logf(const char* fmt, ...);

void vlogf(const char* fmt, va_list ap);

void log(const std::string&);

template <typename CallbackType, typename... Args> bool call_if_available(const CallbackType& callback, Args... args) {
    if (not callback) {
        false;
    }

    callback(std::forward<Args>(args)...);
    return true;
}

} // namespace ieee2030
