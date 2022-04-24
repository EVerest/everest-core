// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <thread>

#include <boost/filesystem.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include <everest/logging.hpp>

#include "command_api.hpp"
#include "ipc.hpp"
#include "rpc.hpp"
#include "server.hpp"

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    namespace fs = boost::filesystem;

    if (strcmp(argv[0], MAGIC_CONTROLLER_ARG0)) {
        fmt::print(stderr, "This binary does not yet support to be started manually\n");
        return EXIT_FAILURE;
    }

    auto socket_fd = STDIN_FILENO;

    const auto message = Everest::controller_ipc::receive_message(socket_fd);

    if (message.status != Everest::controller_ipc::MESSAGE_RETURN_STATUS::OK) {
        throw std::runtime_error("Controller process could not read initial config message");
    }

    // FIXME (aw): validation
    const auto config_params = message.json.at("params");

    const std::string module_dir = config_params.at("module_dir");
    const std::string interface_dir = config_params.at("interface_dir");
    const std::string config_dir = config_params.at("config_dir");
    const std::string logging_config_file = config_params.at("logging_config_file");

    // if (!fs::is_directory(module_dir) || !fs::is_directory(config_dir) || !fs::is_directory(interface_dir)) {
    //     throw std::runtime_error("One or more of the passed directories are not valid");
    // }

    Everest::Logging::init(logging_config_file, "everest_ctrl");

    EVLOG_info << "everest controller process started ...";

    CommandApi::Config config{
        module_dir,
        interface_dir,
        config_dir,
    };

    RPC rpc(socket_fd, config);
    Server backend;

    std::thread(&Server::run, &backend, [&rpc](const nlohmann::json& request) {
        return rpc.handle_json_rpc(request);
    }).detach();

    while (true) {
        rpc.run([&backend](const nlohmann::json& notification) { backend.push(notification); });
    }

    return 0;
}
