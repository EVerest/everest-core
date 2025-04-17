// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef RUNAPPLICATION_HPP
#define RUNAPPLICATION_HPP

#include <string>
#include <vector>

namespace module {

struct CmdOutput {
    std::string output;
    std::vector<std::string> split_output;
    int exit_code;
};

CmdOutput run_application(const std::string& name, std::vector<std::string> args);

} // namespace module

#endif // RUNAPPLICATION_HPP
