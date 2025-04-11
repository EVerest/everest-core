// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "RunApplication.hpp"

#include <boost/process.hpp>
#include <fmt/core.h>

#include <everest/logging.hpp>

namespace module {

CmdOutput run_application(const std::string& name, std::vector<std::string> args) {
    // search_path requires basename and not a full path
    boost::filesystem::path path = name;

    if (path.is_relative()) {
        path = boost::process::search_path(name);
    }

    if (path.empty()) {
        EVLOG_debug << fmt::format("The application '{}' could not be found", name);
        return CmdOutput{"", {}, 1};
    }

    boost::process::ipstream stream;
    boost::process::child cmd(path, boost::process::args(args), boost::process::std_out > stream);
    std::string output;
    std::vector<std::string> split_output;
    std::string temp;
    while (std::getline(stream, temp)) {
        output += temp + "\n";
        split_output.push_back(temp);
    }
    cmd.wait();
    return CmdOutput{output, split_output, cmd.exit_code()};
}

} // namespace module
