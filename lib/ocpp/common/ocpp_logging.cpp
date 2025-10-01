// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <algorithm>

#include <everest/logging.hpp>

#include <ocpp/common/call_types.hpp>
#include <ocpp/common/ocpp_logging.hpp>
#include <ocpp/common/types.hpp>

#include <boost/algorithm/string.hpp>

using json = nlohmann::json;

namespace ocpp {

MessageLogging::MessageLogging(
    bool log_messages, const std::string& message_log_path, const std::string& output_file_name, bool log_to_console,
    bool detailed_log_to_console, bool log_to_file, bool log_to_html, bool log_raw, bool log_security,
    bool session_logging,
    std::function<void(const std::string& message, MessageDirection direction)> message_callback) :
    log_messages(log_messages),
    message_log_path(message_log_path),
    output_file_name(output_file_name),
    log_to_console(log_to_console),
    detailed_log_to_console(detailed_log_to_console),
    log_to_file(log_to_file),
    log_to_html(log_to_html),
    log_raw(log_raw),
    log_security(log_security),
    session_logging(session_logging),
    message_callback(message_callback),
    rotate_logs(false),
    date_suffix(false),
    maximum_file_size_bytes(0),
    maximum_file_count(0) {
    this->initialize();
}

MessageLogging::MessageLogging(
    bool log_messages, const std::string& message_log_path, const std::string& output_file_name, bool log_to_console,
    bool detailed_log_to_console, bool log_to_file, bool log_to_html, bool log_raw, bool log_security,
    bool session_logging, std::function<void(const std::string& message, MessageDirection direction)> message_callback,
    LogRotationConfig log_rotation_config, std::function<void(LogRotationStatus status)> status_callback) :
    log_messages(log_messages),
    message_log_path(message_log_path),
    output_file_name(output_file_name),
    log_to_console(log_to_console),
    detailed_log_to_console(detailed_log_to_console),
    log_to_file(log_to_file),
    log_to_html(log_to_html),
    log_raw(log_raw),
    log_security(log_security),
    session_logging(session_logging),
    message_callback(message_callback),
    rotate_logs(true),
    date_suffix(log_rotation_config.date_suffix),
    maximum_file_size_bytes(log_rotation_config.maximum_file_size_bytes),
    maximum_file_count(log_rotation_config.maximum_file_count),
    status_callback(status_callback) {
    this->initialize();
}

void MessageLogging::initialize() {
    if (this->log_messages) {
        if (this->rotate_logs) {
            EVLOG_info << "Log rotation enabled";
        }
        if (this->log_to_console) {
            EVLOG_info << "Logging OCPP messages to console";
        }
        if (this->message_callback != nullptr) {
            EVLOG_info << "Logging OCPP messages to callback";
        }
        if (this->log_to_file) {
            auto output_file_path = message_log_path + "/";
            output_file_path += output_file_name;
            if (this->log_raw) {
                auto raw_output_file_path = output_file_path + "_raw.log";
                EVLOG_info << "Logging raw OCPP messages to log file: " << raw_output_file_path;
                this->log_raw_file = std::filesystem::path(raw_output_file_path);
                this->log_raw_os.open(raw_output_file_path, std::ofstream::app);
                this->rotate_log_if_needed(this->log_raw_file, this->log_raw_os);
            }
            output_file_path += +".log";
            EVLOG_info << "Logging OCPP messages to log file: " << output_file_path;
            this->log_file = std::filesystem::path(output_file_path);
            this->log_os.open(output_file_path, std::ofstream::app);
            this->rotate_log_if_needed(this->log_file, this->log_os);
        }

        if (this->log_to_html) {
            auto html_file_path = message_log_path + "/";
            html_file_path += output_file_name;
            if (this->log_raw) {
                auto raw_html_file_path = html_file_path + "_raw.html";
                EVLOG_info << "Logging raw OCPP messages to html file: " << raw_html_file_path;
                this->html_raw_log_file = std::filesystem::path(raw_html_file_path);
                this->html_raw_log_os.open(html_raw_log_file, std::ofstream::app);
                this->rotate_log_if_needed(
                    this->html_raw_log_file, this->html_raw_log_os,
                    [this](std::ofstream& os) { this->close_html_tags(os); },
                    [this](std::ofstream& os) { this->open_html_tags(os); });

                if (this->file_size(this->html_raw_log_file) > 0) {
                    // TODO: try to remove the end tags in the HTML if present
                } else {
                    this->open_html_tags(this->html_raw_log_os);
                }
            }
            html_file_path += ".html";
            EVLOG_info << "Logging OCPP messages to html file: " << html_file_path;
            this->html_log_file = std::filesystem::path(html_file_path);
            this->html_log_os.open(html_log_file, std::ofstream::app);
            this->rotate_log_if_needed(
                this->html_log_file, this->html_log_os, [this](std::ofstream& os) { this->close_html_tags(os); },
                [this](std::ofstream& os) { this->open_html_tags(os); });

            if (this->file_size(this->html_log_file) > 0) {
                // TODO: try to remove the end tags in the HTML if present
            } else {
                this->open_html_tags(this->html_log_os);
            }
        }
        if (this->log_security) {
            auto security_file_path = message_log_path + "/";
            security_file_path += output_file_name;
            security_file_path += ".security.log";
            EVLOG_info << "Logging SecurityEvents to file: " << security_file_path;
            this->security_log_file = std::filesystem::path(security_file_path);
            this->security_log_os.open(security_log_file, std::ofstream::app);
            this->rotate_log_if_needed(this->security_log_file, this->security_log_os);
        }
        sys("Session logging started.");
    }
}

void MessageLogging::open_html_tags(std::ofstream& os) {
    os << "<html><head><title>EVerest OCPP log session</title>\n";
    os << "<style>"
          ".log {"
          "  font-family: Arial, Helvetica, sans-serif;"
          "  border-collapse: collapse;"
          "  width: 100%;"
          "}"
          ".log td, .log th {"
          "  border: 1px solid #ddd;"
          "  padding: 8px;"
          "  vertical-align: top;"
          "}"
          ".log tr.CentralSystem{background-color: #E4E6F2;}"
          ".log tr.ChargePoint{background-color: #F2F0E4;}"
          ".log tr.SYS{background-color: white;}"
          ".log th {"
          "  padding-top: 12px;"
          "  padding-bottom: 12px;"
          "  text-align: left;"
          "  vertical-align: top;"
          "  background-color: #04AA6D;"
          "  color: white;"
          "}"
          "</style>";
    os << "</head><body><table class=\"log\">\n";
    os.flush();
}

void MessageLogging::close_html_tags(std::ofstream& os) {
    os << "</table></body></html>\n";
    os.flush();
}

std::string MessageLogging::get_datetime_string() {
    return date::format("%Y%m%d%H%M%S", std::chrono::time_point_cast<std::chrono::seconds>(date::utc_clock::now()));
}

std::uintmax_t MessageLogging::file_size(const std::filesystem::path& path) {
    try {
        return std::filesystem::file_size(path);
    } catch (...) {
        return 0;
    }
}

LogRotationStatus MessageLogging::rotate_log(const std::string& file_basename) {
    LogRotationStatus status = LogRotationStatus::NotRotated;
    auto path = std::filesystem::path(this->message_log_path);
    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        const auto& file_path = entry.path();
        if (std::filesystem::is_regular_file(entry)) {
            // check file
            if (file_path.filename() == file_basename or file_path.stem() == file_basename) {
                files.push_back(file_path);
            }
        }
    }
    std::sort(files.begin(), files.end(), std::greater<std::filesystem::path>());

    if (this->maximum_file_count > 0 and files.size() >= this->maximum_file_count) {
        // drop the oldest file
        EVLOG_info << "Removing oldest log file: " << files.front();
        std::filesystem::remove(files.front());
        files.erase(files.begin());
        status = LogRotationStatus::RotatedWithDeletion;
    }

    // rename the oldest file first
    for (auto& file : files) {
        if (file.filename() == file_basename) {
            std::filesystem::path new_file_name;
            if (this->date_suffix) {
                new_file_name = std::filesystem::path(file.string() + "." + this->get_datetime_string());
            } else {
                // traditional .0 .1 ... suffix
                // does not have a .0 or .1, so needs a new one
                new_file_name = std::filesystem::path(file.string() + ".0");
            }

            std::filesystem::rename(file, new_file_name);
            EVLOG_info << "Renaming: " << file.string() << " -> " << new_file_name.string();
            if (status == LogRotationStatus::NotRotated) {
                status = LogRotationStatus::Rotated;
            }
        } else {
            // try parsing the .x extension and log an error if this was not possible
            try {
                if (not this->date_suffix) {
                    auto extension_str = file.extension().string();
                    boost::replace_all(extension_str, ".", ""); // .extension() comes with the "."
                    auto extension = std::stoi(extension_str);
                    extension += 1;
                    auto new_extension = std::to_string(extension);

                    std::filesystem::path new_file_name = file;
                    new_file_name.replace_extension(new_extension);
                    EVLOG_info << "Renaming: " << file.string() << " -> " << new_file_name.string();
                    std::filesystem::rename(file, new_file_name);
                    if (status == LogRotationStatus::NotRotated) {
                        status = LogRotationStatus::Rotated;
                    }
                }

            } catch (...) {
                EVLOG_warning << "Could not rename logfile: " << file.string();
            }
        }
    }
    return status;
}

LogRotationStatus MessageLogging::rotate_log_if_needed(const std::filesystem::path& path, std::ofstream& os) {
    return rotate_log_if_needed(path, os, nullptr, nullptr);
}

LogRotationStatus MessageLogging::rotate_log_if_needed(const std::filesystem::path& path, std::ofstream& os,
                                                       std::function<void(std::ofstream& os)> before_close_of_os,
                                                       std::function<void(std::ofstream& os)> after_open_of_os) {
    LogRotationStatus status = LogRotationStatus::NotRotated;
    if (not this->rotate_logs) {
        // do nothing if log rotation is turned off
        return LogRotationStatus::NotRotated;
    }
    if (maximum_file_size_bytes <= 0) {
        // do nothing if no maximum file size is set
        return LogRotationStatus::NotRotated;
    }
    auto log_file_size = this->file_size(path);
    if (log_file_size >= maximum_file_size_bytes) {
        EVLOG_info << "Logfile: " << path.filename().string() << " file size (" << log_file_size << " bytes) >= ("
                   << maximum_file_size_bytes << " bytes) rotating log.";
        if (before_close_of_os != nullptr) {
            before_close_of_os(os);
        }
        os.close();
        os.clear();
        status = rotate_log(path.filename().string());
        os.open(path.string(), std::ofstream::app);
        if (after_open_of_os != nullptr) {
            after_open_of_os(os);
        }
    }
    return status;
}

MessageLogging::~MessageLogging() {
    if (this->log_messages) {
        if (this->log_to_file) {
            this->log_os.close();
        }

        if (this->log_to_html) {
            this->close_html_tags(this->html_log_os);
            this->html_log_os.close();
        }

        if (this->log_security) {
            this->security_log_os.close();
        }
    }
}

void MessageLogging::charge_point(const std::string& message_type, const std::string& json_str) {
    if (this->message_callback != nullptr) {
        this->message_callback(json_str, MessageDirection::ChargingStationToCSMS);
    }
    auto formatted = format_message(message_type, json_str);
    log_output(LogType::ChargePoint, formatted.message_type, formatted.message);
    if (this->session_logging) {
        std::scoped_lock lock(this->session_id_logging_mutex);
        for (auto const& [session_id, logging] : this->session_id_logging) {
            logging->charge_point(message_type, json_str);
        }
    }
}

void MessageLogging::central_system(const std::string& message_type, const std::string& json_str) {
    if (this->message_callback != nullptr) {
        this->message_callback(json_str, MessageDirection::CSMSToChargingStation);
    }
    auto formatted = format_message(message_type, json_str);
    log_output(LogType::CentralSystem, formatted.message_type, formatted.message);
    if (this->session_logging) {
        std::scoped_lock lock(this->session_id_logging_mutex);
        for (auto const& [session_id, logging] : this->session_id_logging) {
            logging->central_system(message_type, json_str);
        }
    }
}

void MessageLogging::sys(const std::string& msg) {
    log_output(LogType::System, msg, "");
    if (this->session_logging) {
        std::scoped_lock lock(this->session_id_logging_mutex);
        for (auto const& [session_id, logging] : this->session_id_logging) {
            log_output(LogType::System, msg, "");
        }
    }
}

void MessageLogging::security(const std::string& msg) {
    std::lock_guard<std::mutex> lock(this->output_file_mutex);
    auto status = this->rotate_log_if_needed(this->security_log_file, this->security_log_os);
    if (status_callback != nullptr) {
        status_callback(status);
    }
    this->security_log_os << msg << "\n";
    this->security_log_os.flush();
}

void MessageLogging::raw(const std::string& msg, LogType log_type) {
    log_output(log_type, msg, "", true);
    if (this->session_logging) {
        std::scoped_lock lock(this->session_id_logging_mutex);
        for (auto const& [session_id, logging] : this->session_id_logging) {
            log_output(log_type, "", msg, true);
        }
    }
}

std::string html_encode(const std::string& msg) {
    std::string out = msg;
    boost::replace_all(out, "<", "&lt;");
    boost::replace_all(out, ">", "&gt;");
    return out;
}

void write_log_to_file(std::ofstream& log_os, LogType typ, const std::string& ts, const std::string& origin,
                       const std::string& target, const std::string& message_type, const std::string& json_str) {
    log_os << ts << ": " << origin + ">" + target << " "
           << (typ == LogType::ChargePoint || typ == LogType::System ? message_type : "") << " "
           << (typ == LogType::CentralSystem ? message_type : "") << "\n"
           << json_str << "\n\n";
    log_os.flush();
}

void write_html_log_to_file(std::ofstream& html_log_os, LogType typ, const std::string& ts, const std::string& origin,
                            const std::string& target, const std::string& message_type, const std::string& json_str) {
    html_log_os << "<tr class=\"" << origin << "\"> <td>" << ts << "</td> <td>" << origin + "&gt;" + target
                << "</td> <td><b>" << (typ == LogType::ChargePoint || typ == LogType::System ? message_type : "")
                << "</b></td><td><b>" << (typ == LogType::CentralSystem ? message_type : "")
                << "</b></td> <td><pre lang=\"json\">" << html_encode(json_str) << "</pre></td> </tr>\n";
    html_log_os.flush();
}

void MessageLogging::log_output(LogType typ, const std::string& message_type, const std::string& json_str, bool raw) {
    if (this->log_messages) {
        std::lock_guard<std::mutex> lock(this->output_file_mutex);

        std::string ts = DateTime().to_rfc3339();

        std::string origin, target;

        if (typ == LogType::ChargePoint) {
            origin = "ChargePoint";
            target = "CentralSystem";
            if (this->detailed_log_to_console) {
                EVLOG_info << "\033[1;35mChargePoint: " << json_str << "\033[1;0m";
            } else if (this->log_to_console) {
                EVLOG_info << "\033[1;35mChargePoint: " << message_type << "\033[1;0m";
            }
        } else if (typ == LogType::CentralSystem) {
            origin = "CentralSystem";
            target = "ChargePoint";
            if (this->detailed_log_to_console) {
                EVLOG_info << "\033[1;36mCentralSystem: " << json_str << "\033[1;0m";
            } else if (this->log_to_console) {
                EVLOG_info << "                                    \033[1;36mCentralSystem: " << message_type
                           << "\033[1;0m";
            }
        } else if (typ == LogType::System) {
            origin = "SYS";
            target = "";
            if (this->detailed_log_to_console || this->log_to_console) {
                EVLOG_info << "\033[1;32mSYS:  " << message_type << "\033[1;0m";
            }
        }

        if (this->log_to_file) {
            if (raw and this->log_raw) {
                this->rotate_log_if_needed(this->log_raw_file, this->log_raw_os);
                write_log_to_file(this->log_raw_os, typ, ts, origin, target, message_type, json_str);
            } else {
                this->rotate_log_if_needed(this->log_file, this->log_os);
                write_log_to_file(this->log_os, typ, ts, origin, target, message_type, json_str);
            }
        }
        if (this->log_to_html) {
            if (raw and this->log_raw) {
                this->rotate_log_if_needed(
                    this->html_raw_log_file, this->html_raw_log_os,
                    [this](std::ofstream& os) { this->close_html_tags(os); },
                    [this](std::ofstream& os) { this->open_html_tags(os); });
                write_html_log_to_file(this->html_raw_log_os, typ, ts, origin, target, message_type, json_str);
            } else {
                this->rotate_log_if_needed(
                    this->html_log_file, this->html_log_os, [this](std::ofstream& os) { this->close_html_tags(os); },
                    [this](std::ofstream& os) { this->open_html_tags(os); });
                write_html_log_to_file(this->html_log_os, typ, ts, origin, target, message_type, json_str);
            }
        }
    }
}

FormattedMessageWithType MessageLogging::format_message(const std::string& message_type, const std::string& json_str) {
    auto extracted_message_type = message_type;
    auto formatted_message = json_str;

    try {
        auto json_object = json::parse(json_str);
        if (json_object.at(MESSAGE_TYPE_ID) == MessageTypeId::CALL) {
            extracted_message_type = json_object.at(CALL_ACTION);
            this->lookup_map[json_object.at(MESSAGE_ID)] = extracted_message_type + "Response";
        } else if (json_object.at(MESSAGE_TYPE_ID) == MessageTypeId::CALLRESULT) {
            extracted_message_type = this->lookup_map[json_object.at(MESSAGE_ID)];
            this->lookup_map[json_object.at(MESSAGE_ID)].erase();
        }
        formatted_message = json_object.dump(2);
    } catch (const std::exception& e) {
        EVLOG_warning << "Error parsing OCPP message " << message_type << ": " << e.what();
    }

    return {extracted_message_type, formatted_message};
}

void MessageLogging::start_session_logging(const std::string& session_id, const std::string& log_path) {
    std::scoped_lock lock(this->session_id_logging_mutex);
    this->session_id_logging[session_id] = std::make_shared<ocpp::MessageLogging>(
        true, log_path, "incomplete-ocpp", false, false, false, true, true, false, false, nullptr);
}

void MessageLogging::stop_session_logging(const std::string& session_id) {
    std::scoped_lock lock(this->session_id_logging_mutex);
    if (this->session_id_logging.count(session_id)) {
        auto old_file_path =
            this->session_id_logging.at(session_id)->get_message_log_path() + "/" + "incomplete-ocpp.html";
        auto new_file_path = this->session_id_logging.at(session_id)->get_message_log_path() + "/" + "ocpp.html";
        std::rename(old_file_path.c_str(), new_file_path.c_str());
        this->session_id_logging.erase(session_id);
    }
}

std::string MessageLogging::get_message_log_path() {
    return this->message_log_path;
}

bool MessageLogging::session_logging_active() {
    return this->session_logging;
}

} // namespace ocpp
