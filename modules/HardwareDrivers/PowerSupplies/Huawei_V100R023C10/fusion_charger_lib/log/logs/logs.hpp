// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>
#include <string>

namespace logs {

class LogFun {
    std::function<void(const std::string&)> fn;

public:
    LogFun(std::function<void(const std::string&)> fn) : fn(fn) {
    }

    void operator<<(const std::string& message) {
        fn(message);
    }
};

struct LogIntf {
    LogFun error;
    LogFun warning;
    LogFun info;
    LogFun verbose;
};

extern LogIntf log_printf;

}; // namespace logs
