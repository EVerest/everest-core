// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef DIAGNOSTICS_HANDLER_HPP
#define DIAGNOSTICS_HANDLER_HPP

#include <optional>
#include <string>

namespace module {

class DiagnosticsHandler {
protected:
    std::string script_dir;
    std::string ocpp_dir;
    std::string session_dir;

public:
    enum class log_result_t {
        success,
        error,
        error_file,
        error_parameter,
    };
    DiagnosticsHandler() = delete;
    DiagnosticsHandler(const std::string& scriptDir, const std::string& ocppDir, const std::string& sessionDir) :
        script_dir(scriptDir), ocpp_dir(ocppDir), session_dir(sessionDir) {
    }
    log_result_t create_log(const std::string& filename, const std::optional<std::string>& from,
                            const std::optional<std::string>& to);

protected:
    log_result_t create_journal_log(int fd, const std::optional<std::string>& from,
                                    const std::optional<std::string>& to) {
        return collect_logs(fd, "journal", std::nullopt, from, to);
    }
    log_result_t create_ocpp_log(int fd, const std::optional<std::string>& from, const std::optional<std::string>& to) {
        return collect_logs(fd, "ocpp", ocpp_dir, from, to);
    }
    log_result_t create_session_log(int fd, const std::optional<std::string>& from,
                                    const std::optional<std::string>& to) {
        return collect_logs(fd, "session", session_dir, from, to);
    }

    /// @brief collect logs between two optional dates
    /// @param fd - the file descriptor to write the logs to
    /// @param logType - journal/ocpp/session
    /// @param logDir - directory location for OCPP and session logs
    /// @param from - format: "yyyy-mm-dd hh:mm"
    /// @param to - format: "yyyy-mm-dd hh:mm"
    /// @return result
    log_result_t collect_logs(int fd, const char* logType, const std::optional<std::string>& logDir,
                              const std::optional<std::string>& from, const std::optional<std::string>& to);
};

} // namespace module

#endif // DIAGNOSTICS_HANDLER_HPP
