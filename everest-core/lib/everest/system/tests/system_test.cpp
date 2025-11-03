// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <everest/system/safe_system.hpp>

#include <gtest/gtest.h>

namespace {

TEST(SplitCommand, single_word) {
    std::string cmd{"command"};
    std::pair<std::string, std::vector<std::string>> expected_result = {"command", {"command"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, multi_word) {
    std::string cmd{"command and arg"};
    std::pair<std::string, std::vector<std::string>> expected_result = {"command", {"command", "and", "arg"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, leading_and_trailing_spaces) {
    std::string cmd{"  leading  and   trailing spaces  "};
    std::pair<std::string, std::vector<std::string>> expected_result = {"leading",
                                                                        {"leading", "and", "trailing", "spaces"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, quoted) {
    std::string cmd{"  \"leading\"  \"and\"   'trailing' 'spaces'  "};
    std::pair<std::string, std::vector<std::string>> expected_result = {"leading",
                                                                        {"leading", "and", "trailing", "spaces"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, quoted_whitespace) {
    std::string cmd{"  \"leading  and\"   'trailing spaces'  "};
    std::pair<std::string, std::vector<std::string>> expected_result = {"leading  and",
                                                                        {"leading  and", "trailing spaces"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, string_with_double_quotes_inside) {
    std::string cmd{"echo 'A string with \"double quotes\" inside'"};
    std::pair<std::string, std::vector<std::string>> expected_result = {
        "echo", {"echo", "A string with \"double quotes\" inside"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, string_with_single_quotes_inside) {
    std::string cmd{"echo \"A string with 'single quotes' inside\""};
    std::pair<std::string, std::vector<std::string>> expected_result = {
        "echo", {"echo", "A string with 'single quotes' inside"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, non_separated_double_quotes) {
    std::string cmd{"command arg1\"foo\"bar arg2"};
    std::pair<std::string, std::vector<std::string>> expected_result = {"command", {"command", "arg1foobar", "arg2"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, non_separated_single_quotes) {
    std::string cmd{"command arg1'foo'bar arg2"};
    std::pair<std::string, std::vector<std::string>> expected_result = {"command", {"command", "arg1foobar", "arg2"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, empty_double_quoted_arg) {
    std::string cmd{"command \"\" arg2"};
    std::pair<std::string, std::vector<std::string>> expected_result = {"command", {"command", "", "arg2"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, empty_single_quoted_arg) {
    std::string cmd{"command \'\' arg2"};
    std::pair<std::string, std::vector<std::string>> expected_result = {"command", {"command", "", "arg2"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, non_separated_empty_quoted) {
    std::string cmd{"command arg1\'\' arg2\"\""};
    std::pair<std::string, std::vector<std::string>> expected_result = {"command", {"command", "arg1", "arg2"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, real_life_case_01) {
    std::string cmd{"/usr/bin/ls -l /home/user"};
    std::pair<std::string, std::vector<std::string>> expected_result = {"/usr/bin/ls", {"ls", "-l", "/home/user"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, real_life_case_02) {
    std::string cmd{"/usr/bin/cp \"/path with spaces/to/file.txt\" \"/another path/\""};
    std::pair<std::string, std::vector<std::string>> expected_result = {
        "/usr/bin/cp", {"cp", "/path with spaces/to/file.txt", "/another path/"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, real_life_case_03) {
    std::string cmd{"/usr/bin/ls -l /home/user\"   \""};
    std::pair<std::string, std::vector<std::string>> expected_result = {"/usr/bin/ls", {"ls", "-l", "/home/user   "}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, real_life_case_04) {
    std::string cmd{"/usr/sbin/shutdown   -r now"};
    std::pair<std::string, std::vector<std::string>> expected_result = {"/usr/sbin/shutdown",
                                                                        {"shutdown", "-r", "now"}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, empty) {
    std::string cmd{""};

    EXPECT_THROW(everest::lib::system::split_command_line(cmd), std::runtime_error);
}

TEST(SplitCommand, whitespace_only) {
    std::string cmd{"    "};

    EXPECT_THROW(everest::lib::system::split_command_line(cmd), std::runtime_error);
}

TEST(SplitCommand, quoted_whitespace_only) {
    std::string cmd{"\"    \""};
    std::pair<std::string, std::vector<std::string>> expected_result = {"    ", {"    "}};

    auto result = everest::lib::system::split_command_line(cmd);

    EXPECT_EQ(result, expected_result);
}

TEST(SplitCommand, literal_backslash) {
    std::string cmd{"ls \\\" file \\\""};

    EXPECT_THROW(everest::lib::system::split_command_line(cmd), std::runtime_error);
}

TEST(SplitCommand, unmatched_quote) {
    std::string cmd{"/usr/bin/ls -l /home/user\"   "};

    EXPECT_THROW(everest::lib::system::split_command_line(cmd), std::runtime_error);
}

} // namespace
