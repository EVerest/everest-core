// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef CONTROLLER_COMMAND_API_HPP
#define CONTROLLER_COMMAND_API_HPP

#include <string>

#include <nlohmann/json.hpp>

// forward declaration
class RPC;

class CommandApiParamsError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class CommandApiMethodNotFound : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class CommandApi {
public:
    struct Config {
        std::string module_dir;
        std::string interface_dir;
        std::string configs_dir;
        int controller_rpc_timeout_ms;
    };

    CommandApi(const Config& config, RPC& rpc);

    nlohmann::json handle(const std::string& cmd, const nlohmann::json& params);

private:
    Config config;
    RPC& rpc;
};

#endif // CONTROLLER_COMMAND_API_HPP
