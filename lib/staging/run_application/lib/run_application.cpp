// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <everest/staging/run_application/run_application.hpp>

#include <boost/version.hpp>
#if BOOST_VERSION >= 108600
#include <boost/process/v1/args.hpp>
#include <boost/process/v1/child.hpp>
#include <boost/process/v1/cmd.hpp>
#include <boost/process/v1/io.hpp>
#include <boost/process/v1/pipe.hpp>
#include <boost/process/v1/search_path.hpp>
namespace everest_boost_process = boost::process::v1;
#else
#include <boost/process.hpp>
namespace everest_boost_process = boost::process;
#endif

#include <fmt/core.h>

#include <everest/logging.hpp>

namespace everest::staging::run_application {
CmdOutput run_application(const std::string& name, std::vector<std::string> args,
                          const std::function<CmdControl(const std::string& output_line)> output_callback) {

    // search_path requires basename and not a full path
    boost::filesystem::path path = name;

    if (path.is_relative()) {
        path = everest_boost_process::search_path(name);
    }

    if (path.empty()) {
        EVLOG_debug << fmt::format("The application '{}' could not be found", name);
        return CmdOutput{"", {}, 1};
    }

    everest_boost_process::ipstream stream;
    everest_boost_process::child cmd(path, everest_boost_process::args(args), everest_boost_process::std_out > stream);
    std::string output;
    std::vector<std::string> split_output;
    std::string temp;
    auto condition = CmdControl::Continue;
    while (std::getline(stream, temp)) {
        output += temp + "\n";
        if (output_callback != nullptr) {
            condition = output_callback(temp);
        }
        split_output.push_back(temp);
        if (condition != CmdControl::Continue) {
            break;
        }
    }
    if (condition == CmdControl::Continue) {
        cmd.wait();
    } else {
        cmd.terminate();
    }
    return CmdOutput{output, split_output, cmd.exit_code()};
}
} // namespace everest::staging::run_application
