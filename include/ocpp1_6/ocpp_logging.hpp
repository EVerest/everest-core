// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_LOGGING_HPP
#define OCPP_LOGGING_HPP

#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

namespace ocpp1_6 {

struct FormattedMessageWithType {
    std::string message_type;
    std::string message;
};

///
/// \brief contains a ocpp message logging abstraction
///
class MessageLogging {
private:
    std::ofstream output_file;
    std::ofstream html_log_file;
    std::mutex output_file_mutex;
    bool log_messages;
    bool log_to_console;
    bool detailed_log_to_console;
    bool log_to_file;
    bool log_to_html;
    std::map<std::string, std::string> lookup_map;

    void log_output(unsigned int typ, const std::string& message_type, const std::string& json_str);
    std::string html_encode(const std::string& msg);
    FormattedMessageWithType format_message(const std::string& message_type, const std::string& json_str);

public:
    /// \brief Creates a new Websocket object with the providede \p configuration
    explicit MessageLogging(bool log_messages, const std::string& message_log_path, bool log_to_console,
                            bool detailed_log_to_console, bool log_to_file, bool log_to_html);
    ~MessageLogging();

    void charge_point(const std::string& message_type, const std::string& json_str);
    void central_system(const std::string& message_type, const std::string& json_str);
    void sys(const std::string& msg);
};

} // namespace ocpp1_6
#endif // OCPP_WEBSOCKET_HPP
