// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "charge_bridge/charge_bridge.hpp"
#include "charge_bridge/utilities/string.hpp"
#include <atomic>
#include <charge_bridge/utilities/parse_config.hpp>
#include <csignal>
#include <cstring>
#include <everest/io/event/fd_event_handler.hpp>
#include <iostream>

using namespace everest::lib::io::event;
using namespace everest::lib::API::V1_0::types;
using namespace charge_bridge;

enum class mode {
    error,
    connector,
    update,
    update_only,
};
mode parse_args(int argc, char* argv[], std::vector<std::string>& config_files) {
    // clang-format off
    auto print_msg = []() {
        std::cout << "\nUSAGE: \n";
        std::cout << "pionix_chargebridge_tool [--update][--update_only] {config_file [config_file_2 ....]} \n";
        std::cout << "\n";
        std::cout << "--update                 use this flag to execute an update at start and continue operation after\n";
        std::cout << "--update_only            use this flag to execute an update and stop the application after\n";
        std::cout << "config_file              use this configuration file\n";
        std::cout << "config_file_x            add more configuration files for each additional ChargeBridge group\n";
        std::cout << "\n";
    };
    // clang-format on

    auto mode = mode::connector;
    for (int i = 1; i < argc; ++i) {
        std::string current_arg = argv[i];
        if (current_arg == "--update_only") {
            mode = mode::update_only;
        } else if (current_arg == "--update") {
            mode = mode::update;
        } else if (utilities::string_starts_with(current_arg, "--")) {
            mode = mode::error;
            break;
        } else {
            config_files.push_back(current_arg);
        }
    }

    if (config_files.size() == 0) {
        mode = mode::error;
    }

    if (mode == mode::error) {
        print_msg();
    }
    return mode;
}

std::atomic<bool> g_run_application(true);
void signal_handler(int signum) {
    std::cout << "\nSignal " << signum << " received. Initiating graceful shutdown." << std::endl;
    g_run_application = false;
}

int main(int argc, char* argv[]) {
    std::cout << "PIONIX ChargeBridge (c) 2025\n" << std::endl;

    std::signal(SIGINT, signal_handler);
    std::signal(SIGHUP, signal_handler);

    std::vector<std::string> config_files;
    std::vector<charge_bridge_config> cb_configs;
    std::vector<std::unique_ptr<::charge_bridge::charge_bridge>> cb_handler;

    auto mode_of_operation = parse_args(argc, argv, config_files);
    if (mode_of_operation == mode::error) {
        return 1;
    }
    fd_event_handler ev_handler;

    std::set<std::string> cb_ids_in_use;

    for (auto const& elem : config_files) {
        auto config_list = utilities::parse_config_multi(elem);
        for (auto const& config : config_list) {
            print_charge_bridge_config(config);
            if (cb_ids_in_use.count(config.cb_name) > 0) {
                std::cerr << "Dublicate charge_bridge::name '" << config.cb_name << "'" << std::endl;
                return -1;
            }

            cb_ids_in_use.insert(config.cb_name);
            cb_handler.push_back(std::make_unique<::charge_bridge::charge_bridge>(config));
            auto& cb = *cb_handler.rbegin();

            auto force_update = mode_of_operation == mode::update || mode_of_operation == mode::update_only;
            if (not cb->update_firmware(force_update)) {
                return -1;
            }
            if (mode_of_operation == mode::update_only) {
                return 0;
            }
            ev_handler.register_event_handler(cb.get());
        }
    }

    ev_handler.run(g_run_application);

    return 0;
}
