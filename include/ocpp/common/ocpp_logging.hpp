// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_LOGGING_HPP
#define OCPP_COMMON_LOGGING_HPP

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <ocpp/common/types.hpp>
#include <thread>

namespace ocpp {

/// Container for a formatted OCPP message with a message type
struct FormattedMessageWithType {
    std::string message_type; ///< The message type
    std::string message;      ///< The message content
};

/// Configuration for log rotation
struct LogRotationConfig {
    bool date_suffix; ///< If set to true the log rotation files use a date after the ".", if not use the traditional
                      ///< .0, .1 ... style
    uint64_t
        maximum_file_size_bytes; ///< The maximum size of the log file in bytes after which the file will be rotated
    uint64_t maximum_file_count; ///< The maximum number of log files to keep in rotation

    LogRotationConfig(bool date_suffix, uint64_t maximum_file_size_bytes, uint64_t maximum_file_count) :
        date_suffix(date_suffix),
        maximum_file_size_bytes(maximum_file_size_bytes),
        maximum_file_count(maximum_file_count) {
    }
};

enum class LogRotationStatus {
    NotRotated,
    Rotated,
    RotatedWithDeletion
};

enum class LogType {
    ChargePoint,
    CentralSystem,
    System
};

/// \brief contains a ocpp message logging abstraction
class MessageLogging {
private:
    bool log_messages;
    std::string message_log_path; // FIXME: use fs::path here
    std::string output_file_name;
    bool log_to_console;
    bool detailed_log_to_console;
    bool log_to_file;
    bool log_to_html;
    bool log_raw;
    bool log_security;
    bool session_logging;
    std::filesystem::path log_file;
    std::ofstream log_os;
    std::filesystem::path log_raw_file;
    std::ofstream log_raw_os;
    std::filesystem::path html_log_file;
    std::ofstream html_log_os;
    std::filesystem::path html_raw_log_file;
    std::ofstream html_raw_log_os;
    std::filesystem::path security_log_file;
    std::ofstream security_log_os;
    std::mutex output_file_mutex;
    std::function<void(const std::string& message, MessageDirection direction)> message_callback;
    std::function<void(LogRotationStatus status)> status_callback;
    std::map<std::string, std::string> lookup_map;
    std::recursive_mutex session_id_logging_mutex;
    std::map<std::string, std::shared_ptr<MessageLogging>> session_id_logging;
    bool rotate_logs;
    bool date_suffix;
    std::string logfile_basename;
    uint64_t maximum_file_size_bytes;
    uint64_t maximum_file_count;

    /// \brief Initialize the OCPP message logging
    void initialize();

    /// \brief Output log message to the configured targets
    void log_output(LogType typ, const std::string& message_type, const std::string& json_str, bool raw = false);

    /// \brief Format the given \p json_str with the given \p message_type
    FormattedMessageWithType format_message(const std::string& message_type, const std::string& json_str);

    /// \brief Add opening html tags to the given stream \p os
    void open_html_tags(std::ofstream& os);

    /// \brief Add closing html tags to the given stream \p os
    void close_html_tags(std::ofstream& os);

    /// \returns a datetime string in YearMonthDayHourMinuteSecond format
    std::string get_datetime_string();

    /// \returns file size of the given path or 0 if the file does not exist
    std::uintmax_t file_size(const std::filesystem::path& path);

    /// \brief Rotates the log at the given file \p file_basename and remove oldest file if there are more log files
    /// than the maximum
    LogRotationStatus rotate_log(const std::string& file_basename);

    /// \brief Rotates the log at the given \p path if needed based on the config, closing the stream \p os before
    LogRotationStatus rotate_log_if_needed(const std::filesystem::path& path, std::ofstream& os);

    /// \brief Rotates the log at the given \p path if needed based on the config, calling \p before_close_of_os before
    /// closing the stream \p os and calling \p after_open_of_os afterwards
    LogRotationStatus rotate_log_if_needed(const std::filesystem::path& path, std::ofstream& os,
                                           std::function<void(std::ofstream& os)> before_close_of_os,
                                           std::function<void(std::ofstream& os)> after_open_of_os);

public:
    /// \brief Creates a new MessageLogging object with the provided configuration
    explicit MessageLogging(
        bool log_messages, const std::string& message_log_path, const std::string& output_file_name,
        bool log_to_console, bool detailed_log_to_console, bool log_to_file, bool log_to_html, bool log_raw,
        bool log_security, bool session_logging,
        std::function<void(const std::string& message, MessageDirection direction)> message_callback);

    /// \brief Creates a new MessageLogging object with the provided configuration and enabled log rotation
    explicit MessageLogging(
        bool log_messages, const std::string& message_log_path, const std::string& output_file_name,
        bool log_to_console, bool detailed_log_to_console, bool log_to_file, bool log_to_html, bool log_raw,
        bool log_security, bool session_logging,
        std::function<void(const std::string& message, MessageDirection direction)> message_callback,
        LogRotationConfig log_rotation_config, std::function<void(LogRotationStatus status)> status_callback);
    ~MessageLogging();

    /// \brief Log a message originating from the charge point
    void charge_point(const std::string& message_type, const std::string& json_str);

    /// \brief Log a message originating from the central system
    void central_system(const std::string& message_type, const std::string& json_str);

    /// \brief Log a system message
    void sys(const std::string& msg);

    /// \brief Log a security message
    void security(const std::string& msg);

    /// \brief Log a raw OCPP message
    void raw(const std::string& msg, LogType log_type);

    /// \brief Start session logging (without log rotation)
    void start_session_logging(const std::string& session_id, const std::string& log_path);

    /// \brief Stop session logging
    void stop_session_logging(const std::string& session_id);

    /// \returns The message log path
    std::string get_message_log_path();

    /// \returns If session logging is active
    bool session_logging_active();
};

} // namespace ocpp
#endif // OCPP_COMMON_LOGGING_HPP
