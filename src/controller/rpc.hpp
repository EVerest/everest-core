// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef CONTROLLER_RPC_HPP
#define CONTROLLER_RPC_HPP

#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <random>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "command_api.hpp"

class RPC {
public:
    using NotificationHandler = std::function<void(const nlohmann::json&)>;

    RPC(int ipc_fd, const CommandApi::Config& config);

    nlohmann::json handle_json_rpc(const std::string& request_string);
    nlohmann::json ipc_request(const std::string& method, const nlohmann::json& params, bool only_notify);

    void run(const NotificationHandler& handler);

private:
    int ipc_fd;
    std::unique_ptr<CommandApi> api;
    NotificationHandler notification_handler;
    std::chrono::milliseconds rpc_timeout;

    // FIXME (aw): what type of cafe?
    std::mt19937 rng{0xcafe};
    std::unordered_map<std::mt19937::result_type, std::promise<nlohmann::json>> ipc_calls{};
    std::mutex ipc_mutex{};
};

#endif // CONTROLLER_RPC_HPP
