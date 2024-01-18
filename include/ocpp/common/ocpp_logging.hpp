// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_LOGGING_HPP
#define OCPP_COMMON_LOGGING_HPP

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <ocpp/common/types.hpp>
#include <thread>

namespace ocpp {

struct FormattedMessageWithType {
    std::string message_type;
    std::string message;
};

///
/// \brief contains a ocpp message logging abstraction
///
class MessageLogging {
private:
    bool log_messages;
    std::string message_log_path;
    std::string output_file_name;
    bool log_to_console;
    bool detailed_log_to_console;
    bool log_to_file;
    bool log_to_html;
    bool log_security;
    bool session_logging;
    std::ofstream output_file;
    std::ofstream html_log_file;
    std::ofstream security_log_file;
    std::mutex output_file_mutex;
    std::function<void(const std::string& message, MessageDirection direction)> message_callback;
    std::map<std::string, std::string> lookup_map;
    std::recursive_mutex session_id_logging_mutex;
    std::map<std::string, std::shared_ptr<MessageLogging>> session_id_logging;

    void log_output(unsigned int typ, const std::string& message_type, const std::string& json_str);
    std::string html_encode(const std::string& msg);
    FormattedMessageWithType format_message(const std::string& message_type, const std::string& json_str);

public:
    /// \brief Creates a new Websocket object with the providede \p configuration
    explicit MessageLogging(
        bool log_messages, const std::string& message_log_path, const std::string& output_file_name,
        bool log_to_console, bool detailed_log_to_console, bool log_to_file, bool log_to_html, bool log_security,
        bool session_logging,
        std::function<void(const std::string& message, MessageDirection direction)> message_callback);
    ~MessageLogging();

    void charge_point(const std::string& message_type, const std::string& json_str);
    void central_system(const std::string& message_type, const std::string& json_str);
    void sys(const std::string& msg);
    void security(const std::string& msg);
    void start_session_logging(const std::string& session_id, const std::string& log_path);
    void stop_session_logging(const std::string& session_id);
    std::string get_message_log_path();
    bool session_logging_active();
};

} // namespace ocpp
#endif // OCPP_COMMON_WEBSOCKET_HPP
