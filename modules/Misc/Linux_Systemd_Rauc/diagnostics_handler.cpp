// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "diagnostics_handler.hpp"
#include <everest/logging.hpp>
#include <everest/system/safe_system.hpp>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <vector>

namespace module {

DiagnosticsHandler::log_result_t DiagnosticsHandler::create_log(const std::string& filename,
                                                                const std::optional<std::string>& from,
                                                                const std::optional<std::string>& to) {
    log_result_t result = log_result_t::success;

    int fd;
    if ((fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IXUSR)) == -1) {
        EVLOG_error << "Unable to create file journal file: " << filename << " (" << errno << ")";
        result = log_result_t::error_file;
    } else {
        result = create_journal_log(fd, from, to);
        if (result == log_result_t::success) {
            result = create_ocpp_log(fd, from, to);
        }
        if (result == log_result_t::success) {
            result = create_session_log(fd, from, to);
        }
        (void)close(fd);
    }

    return result;
}

DiagnosticsHandler::log_result_t DiagnosticsHandler::collect_logs(int fd, const char* logType,
                                                                  const std::optional<std::string>& logDir,
                                                                  const std::optional<std::string>& from,
                                                                  const std::optional<std::string>& to) {
    log_result_t result = log_result_t::success;

    std::string arg_cmd = "diagnostics_collector.sh";

    std::vector<std::string> args;
    std::string cmd = script_dir + "/" + arg_cmd;

    args.push_back(arg_cmd);
    args.emplace_back(logType);

    if (logDir.has_value()) {
        args.emplace_back("--dir");
        args.push_back(logDir.value());
    }
    if (from.has_value()) {
        args.emplace_back("--since");
        args.push_back(from.value());
    }
    if (to.has_value()) {
        args.emplace_back("--until");
        args.push_back(to.value());
    }

    const auto res = everest::lib::system::safe_system(fd, cmd, &args);
    if (res.status != everest::lib::system::CommandExecutionStatus::CMD_SUCCESS || res.code != 0) {
        EVLOG_error << "Unable to extract journal logs from:" << from.value_or("<not specified>")
                    << " to:" << to.value_or("<not specified>") << " ("
                    << everest::lib::system::cmd_execution_status_to_string(res.status) << ": "
                    << std::to_string(res.code) << ")";
        EVLOG_info << "Failed command: '" << everest::lib::system::command_string_repr(cmd, args) << "'";
        result = log_result_t::error_parameter;
    }

    return result;
}

} // namespace module
