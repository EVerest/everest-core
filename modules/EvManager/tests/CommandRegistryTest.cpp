// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "../main/command_registry.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <string>
#include <vector>

SCENARIO("Commands can be registered", "[RegisteredCommand]") {
    GIVEN("A command with 0 arguments") {
        auto command_registry = CommandRegistry();
        const auto command_name = std::string{"test_command0"};
        const auto argument_count = 0;
        const auto test_comand_0_function = [](const std::vector<std::string>& arguments) { return arguments.empty(); };

        WHEN("The command is registered") {
            command_registry.register_command(command_name, argument_count, test_comand_0_function);

            THEN("The command can be retrieved") {
                const auto& registered_command = command_registry.get_registered_command(command_name);
                THEN("The command can be executed") {
                    CHECK(registered_command({}) == true);
                }
                THEN("The command throws when the number of arguments is invalid") {
                    CHECK(registered_command({}) == true);
                    CHECK_THROWS_WITH(registered_command({"arg1"}), "Invalid number of arguments for: test_command0");
                    CHECK_THROWS_WITH(registered_command({"arg1", "arg2"}),
                                      "Invalid number of arguments for: test_command0");
                    CHECK_THROWS_WITH(registered_command({"arg1", "arg2", "arg3"}),
                                      "Invalid number of arguments for: test_command0");
                }
            }
        }
    }

    GIVEN("A command with 1 argument") {
        auto command_registry = CommandRegistry();
        const auto command_name = std::string{"test_command1"};
        const auto argument_count = 1;
        const auto test_comand_1_function = [](const std::vector<std::string>& arguments) {
            return arguments.size() == 1;
        };

        WHEN("The command is registered") {
            command_registry.register_command(command_name, argument_count, test_comand_1_function);

            THEN("The command can be retrieved") {
                const auto& registered_command = command_registry.get_registered_command(command_name);
                THEN("The command can be executed") {
                    CHECK(registered_command({"arg1"}) == true);
                }
                THEN("The command throws when the number of arguments is invalid") {
                    CHECK_THROWS_WITH(registered_command({}), "Invalid number of arguments for: test_command1");
                    CHECK_THROWS_WITH(registered_command({"arg1", "arg2"}),
                                      "Invalid number of arguments for: test_command1");
                }
            }
        }
    }

    GIVEN("A command with 2 arguments") {
        auto command_registry = CommandRegistry();
        const auto command_name = std::string{"test_command2"};
        const auto argument_count = 2;
        const auto test_comand_2_function = [](const std::vector<std::string>& arguments) {
            return arguments.size() == 2;
        };

        WHEN("The command is registered") {
            command_registry.register_command(command_name, argument_count, test_comand_2_function);

            THEN("The command can be retrieved") {
                const auto& registered_command = command_registry.get_registered_command(command_name);
                THEN("The command can be executed") {
                    CHECK(registered_command({"arg1", "arg2"}) == true);
                }
                THEN("The command throws when the number of arguments is invalid") {
                    CHECK_THROWS_WITH(registered_command({}), "Invalid number of arguments for: test_command2");
                    CHECK_THROWS_WITH(registered_command({"arg1"}), "Invalid number of arguments for: test_command2");
                    CHECK_THROWS_WITH(registered_command({"arg1", "arg2", "arg3"}),
                                      "Invalid number of arguments for: test_command2");
                }
            }
        }
    }
}
