// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "logs/logs.hpp"

namespace logs {

logs::LogIntf log_printf{
    logs::LogFun([](const std::string& message) { printf("ERROR: %s\n", message.c_str()); }),
    logs::LogFun([](const std::string& message) { printf("WARNING: %s\n", message.c_str()); }),
    logs::LogFun([](const std::string& message) { printf("INFO: %s\n", message.c_str()); }),
    logs::LogFun([](const std::string& message) { printf("DEBUG: %s\n", message.c_str()); }),
    .verbose = logs::LogFun([](const std::string& message) { printf("VERBOSE: %s\n", message.c_str()); }),
};

}; // namespace logs
