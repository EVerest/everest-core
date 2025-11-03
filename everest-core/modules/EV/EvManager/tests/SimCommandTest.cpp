// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "../main/command_registry.hpp"
#include "../main/simulation_command.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <string>
#include <vector>

SCENARIO("SimCommands can be created", "[SimCommand]") {
    GIVEN("A command with 0 arguments called test_command is registered") {
        const auto command_name = std::string{"test_command"};
        const auto argument_count = 0;
        const auto test_command_function = [](const std::vector<std::string>& arguments) { return arguments.empty(); };
        auto command_registry = CommandRegistry();
        command_registry.register_command(command_name, argument_count, test_command_function);

        WHEN("The SimCommand is created") {
            const auto sim_command = SimulationCommand{&command_registry.get_registered_command(command_name), {}};

            THEN("The command can be executed") {
                CHECK(sim_command.execute() == true);
            }
        }

        WHEN("The SimCommand is created with the wrong number of arguments") {
            const auto sim_command =
                SimulationCommand{&command_registry.get_registered_command(command_name), {"arg1"}};

            THEN("The command throws") {
                CHECK_THROWS_WITH(sim_command.execute(),
                                  "Invalid number of arguments for: test_command expected 0 got 1");
            }
        }
    }
}

SCENARIO("SimCommands can be parsed", "[SimCommand]") {
    GIVEN("A few commands registered and a command string") {
        auto command_registry = CommandRegistry();
        const auto command_name_a = std::string{"commanda"};
        const auto agrument_count_a = 0;
        const auto command_function_a = [](const std::vector<std::string>& arguments) { return arguments.empty(); };
        command_registry.register_command(command_name_a, agrument_count_a, command_function_a);

        const auto command_name_b = std::string{"commandb"};
        const auto argument_count_b = 1;
        const auto command_function_b = [](const std::vector<std::string>& arguments) { return arguments.size() == 1; };
        command_registry.register_command(command_name_b, argument_count_b, command_function_b);

        const auto command_name_c = std::string{"commandc"};
        const auto argument_count_c = 2;
        const auto command_function_c = [](const std::vector<std::string>& arguments) { return arguments.size() == 2; };
        command_registry.register_command(command_name_c, argument_count_c, command_function_c);

        WHEN("A correct command string is to be parsed") {
            const auto command_string = "commanda;commandb 0;commandc abc 0.0";
            auto parsed_commands = SimulationCommand::parse_sim_commands(command_string, command_registry);

            THEN("A queue of executable SimCommands exists.") {
                CHECK(parsed_commands.front().execute());
                parsed_commands.pop();
                CHECK(parsed_commands.front().execute());
                parsed_commands.pop();
                CHECK(parsed_commands.front().execute());
                parsed_commands.pop();
                CHECK(parsed_commands.empty());
            }

            THEN("A queue of executable SimCommands exists.") {
                CHECK(parsed_commands.front().execute());
                parsed_commands.pop();
                CHECK(parsed_commands.front().execute());
                parsed_commands.pop();
                CHECK(parsed_commands.front().execute());
                parsed_commands.pop();
                CHECK(parsed_commands.empty());
            }
        }

        WHEN("A command string with wrong arguments is to be parsed") {
            const auto command_string = "commanda 1;commandb;commandc abc 0.0 def";
            auto parsed_commands = SimulationCommand::parse_sim_commands(command_string, command_registry);

            THEN("The execution of the commands should fail.") {
                CHECK_THROWS_WITH(parsed_commands.front().execute(),
                                  "Invalid number of arguments for: commanda expected 0 got 1");
                parsed_commands.pop();
                CHECK_THROWS_WITH(parsed_commands.front().execute(),
                                  "Invalid number of arguments for: commandb expected 1 got 0");
                parsed_commands.pop();
                CHECK_THROWS_WITH(parsed_commands.front().execute(),
                                  "Invalid number of arguments for: commandc expected 2 got 3");
                parsed_commands.pop();
                CHECK(parsed_commands.empty());
            }
        }

        WHEN("A command string with unregistered arguments is to be parsed") {
            const auto command_string = "commandd;commande;commandf";

            THEN("Parsing should fail") {
                CHECK_THROWS_WITH(SimulationCommand::parse_sim_commands(command_string, command_registry),
                                  "Command not found: commandd");
            }
        }

        WHEN("An empty string is to be parsed") {
            const auto command_string = "";
            const auto parsed_commands = SimulationCommand::parse_sim_commands(command_string, command_registry);

            THEN("It should create an empty queue") {
                CHECK(parsed_commands.empty());
            }
        }
    }
}
