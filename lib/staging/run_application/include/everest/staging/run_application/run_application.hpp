// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>
#include <string>
#include <vector>

namespace everest::staging::run_application {

/// \brief Used in the output_callback of run_application to indicate if the application should Continue to run or
/// Terminate
enum class CmdControl {
    Continue, ///< Continue application executiong
    Terminate ///< Terminate application
};

/// \brief Output and exit code of an application ran by run_application
struct CmdOutput {
    std::string output;                    ///< Full stdout output
    std::vector<std::string> split_output; ///< The stdout output split after every line
    int exit_code;                         ///< Exit code of the application
};

/// \brief Run the application specified by its \p name which can either be a full path or a binary name in PATH with
/// the provided \p args.
/// A \p output_callback can be provided that gets called after every line of stdout that is also passed to this
/// callback. The callback must return a CmdContol of either Continue or Terminate, which decide if the application will
/// continue running or terminated early
CmdOutput run_application(const std::string& name, std::vector<std::string> args,
                          const std::function<CmdControl(const std::string& output_line)> output_callback = nullptr);

} // namespace everest::staging::run_application
