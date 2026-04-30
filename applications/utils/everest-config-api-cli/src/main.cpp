// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include <boost/program_options.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "command_handlers.hpp"
#include "everest_api_types/utilities/Topics.hpp"
#include "mqtt_config_service_client.hpp"
#include "yaml_provider.hpp"

namespace po = boost::program_options;
using namespace everest::config_cli;

int main(int argc, char** argv) {
    std::string host;
    int port;
    std::string api_name;

    po::options_description global("Global options");
    global.add_options()("help,h", "This help message")(
        "host", po::value<std::string>(&host)->default_value("localhost"),
        "MQTT broker host")("port", po::value<int>(&port)->default_value(1883), "MQTT broker port")(
        "api-name", po::value<std::string>(&api_name)->default_value("configuration"),
        "API name to form the MQTT topic (e.g., configuration)")(
        "command", po::value<std::string>(), "Command to execute")("cmd_args", po::value<std::vector<std::string>>(),
                                                                   "Arguments for command");

    po::positional_options_description pos;
    pos.add("command", 1).add("cmd_args", -1);

    po::variables_map vm;
    try {
        po::parsed_options parsed =
            po::command_line_parser(argc, argv).options(global).positional(pos).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);
    } catch (const po::error& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    if (vm.count("help") || !vm.count("command")) {
        std::cout << "Usage: everest-config-api-cli [options] <command> [args...]\n\n";
        std::cout << global << "\n";
        std::cout << "Commands:\n"
                  << "  list_slots\n"
                  << "  show_slot_metadata <slot_id>\n"
                  << "  active_slot\n"
                  << "  mark_active_slot <slot_id>\n"
                  << "  delete_slot <no>\n"
                  << "  duplicate_slot <no> [<description>]\n"
                  << "  load_yaml <filename>[--description <desc>] [--slot-id <slot_id>]\n"
                  << "  get_configuration <slot_id>\n"
                  << "  set_config_parameters <slot_id> <filename>\n"
                  << "  monitor [--suppress-parameter-updates]\n";
        return 0;
    }

    std::string command = vm["command"].as<std::string>();
    std::vector<std::string> cmd_args = po::collect_unrecognized(
        po::command_line_parser(argc, argv).options(global).positional(pos).allow_unregistered().run().options,
        po::include_positional);
    // Remove the command itself from cmd_args
    if (!cmd_args.empty()) {
        cmd_args.erase(cmd_args.begin());
    }

    everest::lib::API::Topics topics;
    topics.setup("_", "configuration", 0);

    try {
        auto client = std::make_shared<MqttConfigServiceClient>(host, port, topics);
        auto yaml_provider = std::make_shared<YamlProvider>();
        CommandHandlers handlers(client, yaml_provider);

        if (command == "list_slots") {
            handlers.list_slots();
        } else if (command == "show_slot_metadata") {
            if (cmd_args.empty()) {
                std::cerr << "Usage: show_slot_metadata <slot_id>\n";
                return 1;
            }
            handlers.show_slot_metadata(std::stoi(cmd_args[0]));
        } else if (command == "active_slot") {
            handlers.active_slot();
        } else if (command == "mark_active_slot") {
            if (cmd_args.empty()) {
                std::cerr << "Usage: mark_active_slot <slot_id>\n";
                return 1;
            }
            handlers.mark_active_slot(std::stoi(cmd_args[0]));
        } else if (command == "delete_slot") {
            if (cmd_args.empty()) {
                std::cerr << "Usage: delete_slot <no>\n";
                return 1;
            }
            handlers.delete_slot(std::stoi(cmd_args[0]));
        } else if (command == "duplicate_slot") {
            if (cmd_args.empty()) {
                std::cerr << "Usage: duplicate_slot <no> [<description>]\n";
                return 1;
            }
            int slot_id = std::stoi(cmd_args[0]);
            std::string desc = (cmd_args.size() > 1) ? cmd_args[1] : "";
            handlers.duplicate_slot(slot_id, desc);
        } else if (command == "load_yaml") {
            std::string filename;
            int slot_id = -1;
            std::string desc;
            po::options_description load_yaml_opts("load_yaml options");
            load_yaml_opts.add_options()("filename", po::value<std::string>(&filename), "YAML file to load")(
                "slot-id", po::value<int>(&slot_id)->notifier([](int v) {
                    if (v < 0)
                        throw po::validation_error(po::validation_error::invalid_option_value, "slot-id");
                }),
                "Existing slot ID to load into (if not provided, a new slot will be created)")(
                "description,d", po::value<std::string>(&desc), "Description for the slot");
            po::positional_options_description load_yaml_pos;
            load_yaml_pos.add("filename", 1);
            po::variables_map lvm;
            po::store(po::command_line_parser(cmd_args).options(load_yaml_opts).positional(load_yaml_pos).run(), lvm);
            po::notify(lvm);
            if (filename.empty()) {
                std::cerr << "Usage: load_yaml <filename> [--description <desc>] [--slot-id <slot_id>]\n";
                return 1;
            }
            handlers.load_yaml(filename, desc, slot_id == -1 ? std::nullopt : std::optional<int>(slot_id));
        } else if (command == "get_configuration") {
            if (cmd_args.empty()) {
                std::cerr << "Usage: get_configuration <slot_id>\n";
                return 1;
            }
            handlers.get_configuration(std::stoi(cmd_args[0]));
        } else if (command == "set_config_parameters") {
            if (cmd_args.size() < 2) {
                std::cerr << "Usage: set_config_parameters <slot_id> <filename>\n";
                return 1;
            }
            handlers.set_config_parameter(std::stoi(cmd_args[0]), cmd_args[1]);
        } else if (command == "monitor") {
            bool suppress = false;
            if (!cmd_args.empty() && cmd_args[0] == "--suppress-parameter-updates") {
                suppress = true;
            }
            handlers.monitor(suppress);
        } else {
            std::cerr << "Unknown command: " << command << "\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
