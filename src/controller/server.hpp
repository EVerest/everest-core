// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef CONTROLLER_SERVER_HPP
#define CONTROLLER_SERVER_HPP

#include <memory>
#include <functional>

#include <nlohmann/json.hpp>

#include "command_api.hpp"


class Server {
public:
    using IncomingMessageHandler = std::function<nlohmann::json(const nlohmann::json&)>;
    Server();
    ~Server();
    void run(const IncomingMessageHandler& handler, const std::string& html_origin);
    void push(const nlohmann::json& msg);

private:
    class Impl; // forward declaration
    std::unique_ptr<Impl> pimpl;
};

#endif // CONTROLLER_SERVER_HPP
