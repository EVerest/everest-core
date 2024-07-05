// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "simulation_command.hpp"
#include "command_registry.hpp"
#include <algorithm>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

SimulationCommand::SimulationCommand(const RegisteredCommandBase* registered_command_in,
                                     const CmdArguments& arguments_in) :
    arguments{arguments_in}, registered_command{registered_command_in} {
}

bool SimulationCommand::execute() const {
    return (*registered_command)(arguments);
}

std::queue<SimulationCommand> SimulationCommand::parse_sim_commands(const std::string& value,
                                                                    const CommandRegistry& command_registry) {
    auto commands_vector{convert_commands_string_to_vector(value)};

    auto commands_with_arguments{split_into_commands_with_arguments(commands_vector)};

    return compile_commands(commands_with_arguments, command_registry);
}

SimulationCommand::RawCommands SimulationCommand::convert_commands_string_to_vector(const std::string& commands_view) {

    auto commands = std::string{commands_view};

    // convert to lower case inplace
    std::transform(commands.begin(), commands.end(), commands.begin(),
                   [](const auto& character) { return std::tolower(character); });

    // replace newlines with semicolons
    std::replace(commands.begin(), commands.end(), '\n', ';');

    // split by semicolons
    std::stringstream commands_stream{commands};
    auto command = std::string{};
    auto commands_vector = std::vector<std::string>{};

    while (std::getline(commands_stream, command, ';')) {
        commands_vector.push_back(command);
    }
    return commands_vector;
}
SimulationCommand::CommandsWithArguments
SimulationCommand::split_into_commands_with_arguments(std::vector<std::string>& commands_vector) {
    auto commands_with_arguments = std::vector<std::pair<std::string, std::vector<std::string>>>{};

    for (auto& command : commands_vector) {
        commands_with_arguments.push_back(split_into_command_with_arguments(command));
    }
    return commands_with_arguments;
}

SimulationCommand::CommandWithArguments SimulationCommand::split_into_command_with_arguments(std::string& command) {
    // replace commas with spaces
    std::replace(command.begin(), command.end(), ',', ' ');

    // get command name and arguments
    auto command_stream = std::stringstream{command};
    auto command_name = std::string{};
    auto argument = std::string{};
    auto arguments = std::vector<std::string>{};

    // get command name
    std::getline(command_stream, command_name, ' ');

    // get arguments
    while (std::getline(command_stream, argument, ' ')) {
        arguments.push_back(argument);
    }

    return {command_name, arguments};
}
std::queue<SimulationCommand> SimulationCommand::compile_commands(CommandsWithArguments& commands_with_arguments,
                                                                  const CommandRegistry& command_registry) {
    auto compiled_commands = std::queue<SimulationCommand>{};

    for (auto& [command, arguments] : commands_with_arguments) {
        compiled_commands.emplace(&command_registry.get_registered_command(command), arguments);
    }

    return compiled_commands;
}
