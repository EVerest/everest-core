// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "command_registry.hpp"
#include <cstddef>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using CmdArguments = std::vector<std::string>;

class SimulationCommand {
public:
    SimulationCommand(const RegisteredCommandBase* registered_command_in, const CmdArguments& arguments_in);

    bool execute() const;

    static std::queue<SimulationCommand> parse_sim_commands(const std::string& value,
                                                            const CommandRegistry& command_registry);

private:
    using RawCommands = std::vector<std::string>;
    using CommandWithArguments = std::pair<std::string, CmdArguments>;
    using CommandsWithArguments = std::vector<CommandWithArguments>;

    static RawCommands convert_commands_string_to_vector(const std::string& commands_view);
    static CommandsWithArguments split_into_commands_with_arguments(std::vector<std::string>& commands_vector);
    static CommandWithArguments split_into_command_with_arguments(std::string& command);
    static std::queue<SimulationCommand> compile_commands(CommandsWithArguments& commands_with_arguments,
                                                          const CommandRegistry& command_registry);
    std::vector<std::string> arguments;

    const RegisteredCommandBase* registered_command;
};
