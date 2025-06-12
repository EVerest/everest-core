// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class RegisteredCommandBase {
public:
    virtual ~RegisteredCommandBase() = default;
    virtual bool operator()(const std::vector<std::string>& /*arguments*/) const = 0;
};

class RegisteredCommand : public RegisteredCommandBase {
public:
    RegisteredCommand(std::string command_, std::size_t argument_count_,
                      std::function<bool(std::vector<std::string>)> function_) :
        command_name{std::move(command_)}, argument_count(argument_count_), function{std::move(function_)} {
    }

    ~RegisteredCommand() override = default;

    bool operator()(const std::vector<std::string>& arguments) const override {
        if (arguments.size() != argument_count) {
            throw std::invalid_argument{"Invalid number of arguments for: " + command_name + " expected " +
                                        std::to_string(argument_count) + " got " + std::to_string(arguments.size())};
        }
        return function(arguments);
    }

private:
    std::string command_name;
    std::size_t argument_count;
    std::function<bool(std::vector<std::string>)> function;
};

class CommandRegistry {
public:
    CommandRegistry() = default;

    void register_command(std::string command_name, size_t argument_count,
                          const std::function<bool(std::vector<std::string>)>& function) {
        registered_commands.try_emplace(command_name,
                                        std::make_unique<RegisteredCommand>(command_name, argument_count, function));
    }

    const RegisteredCommandBase& get_registered_command(const std::string& command_name) const {
        try {
            const auto& registered_command = registered_commands.at(command_name);
            return *registered_command.get();
        } catch (const std::out_of_range&) {
            throw std::invalid_argument{"Command not found: " + command_name};
        }
    }

private:
    using RegisteredCommands = std::unordered_map<std::string, std::unique_ptr<RegisteredCommandBase>>;

    RegisteredCommands registered_commands;
};
