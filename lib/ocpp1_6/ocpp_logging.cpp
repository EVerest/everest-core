// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <ocpp1_6/ocpp_logging.hpp>
#include <ocpp1_6/types.hpp>

#include <boost/algorithm/string.hpp>

using json = nlohmann::json;

namespace ocpp1_6 {

MessageLogging::MessageLogging(bool log_messages, const std::string& message_log_path, bool log_to_console,
                               bool detailed_log_to_console, bool log_to_file, bool log_to_html) :
    log_messages(log_messages),
    log_to_console(log_to_console),
    detailed_log_to_console(detailed_log_to_console),
    log_to_file(log_to_file),
    log_to_html(log_to_html) {

    if (this->log_messages) {
        if (this->log_to_console) {
            EVLOG_info << "Logging OCPP messages to console";
        }
        auto output_file_name = DateTime().to_rfc3339();
        if (this->log_to_file) {
            auto output_file_path = message_log_path + "/" + output_file_name + ".log";
            EVLOG_info << "Logging OCPP messages to log file: " << output_file_path;
            this->output_file.open(output_file_path);
        }

        if (this->log_to_html) {
            auto html_file_path = message_log_path + "/" + output_file_name + ".html";
            EVLOG_info << "Logging OCPP messages to html file: " << html_file_path;
            this->html_log_file.open(html_file_path);
            this->html_log_file << "<html><head><title>EVerest OCPP log session</title>\n";
            this->html_log_file << "<style>"
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
            this->html_log_file << "</head><body><table class=\"log\">\n";
        }
        sys("Session logging started.");
    }
}

MessageLogging::~MessageLogging() {
    if (this->log_messages) {
        if (this->log_to_file) {
            this->output_file.close();
        }

        if (this->log_to_html) {
            this->html_log_file << "</table></body></html>\n";
            this->html_log_file.close();
        }
    }
}

void MessageLogging::charge_point(const std::string& message_type, const std::string& json_str) {
    auto formatted = format_message(message_type, json_str);
    log_output(0, formatted.message_type, formatted.message);
}

void MessageLogging::central_system(const std::string& message_type, const std::string& json_str) {
    auto formatted = format_message(message_type, json_str);
    log_output(1, formatted.message_type, formatted.message);
}

void MessageLogging::sys(const std::string& msg) {
    log_output(2, msg, "");
}

void MessageLogging::log_output(unsigned int typ, const std::string& message_type, const std::string& json_str) {
    if (this->log_messages) {
        std::lock_guard<std::mutex> lock(this->output_file_mutex);

        std::string ts = DateTime().to_rfc3339();

        std::string origin, target;

        if (typ == 0) {
            origin = "ChargePoint";
            target = "CentralSystem";
            if (this->detailed_log_to_console) {
                EVLOG_info << "\033[1;35mChargePoint: " << json_str
                           << "\033[1;0m";
            }
            else if (this->log_to_console) {
                EVLOG_info << "\033[1;35mChargePoint: " << message_type << "\033[1;0m";
            }
        } else if (typ == 1) {
            origin = "CentralSystem";
            target = "ChargePoint";
            if (this->detailed_log_to_console) {
                EVLOG_info << "\033[1;36mCentralSystem: " << json_str
                           << "\033[1;0m";
            }
            else if (this->log_to_console) {
                EVLOG_info << "                                    \033[1;36mCentralSystem: " << message_type
                           << "\033[1;0m";
            }
        } else {
            origin = "SYS";
            target = "";
            if (this->detailed_log_to_console || this->log_to_console) {
                EVLOG_info << "\033[1;32mSYS:  " << message_type << "\033[1;0m";
            }
        }

        if (this->log_to_file) {
            this->output_file << ts << ": " << origin + ">" + target << " "
                              << (typ == 0 || typ == 2 ? message_type : "") << " " << (typ == 1 ? message_type : "")
                              << "\n"
                              << json_str << "\n\n";
            this->output_file.flush();
        }
        if (this->log_to_html) {
            this->html_log_file << "<tr class=\"" << origin << "\"> <td>" << ts << "</td> <td>"
                                << origin + "&gt;" + target << "</td> <td><b>"
                                << (typ == 0 || typ == 2 ? message_type : "") << "</b></td><td><b>"
                                << (typ == 1 ? message_type : "") << "</b></td> <td><pre lang=\"json\">"
                                << html_encode(json_str) << "</pre></td> </tr>\n";
            this->html_log_file.flush();
        }
    }
}

std::string MessageLogging::html_encode(const std::string& msg) {
    std::string out = msg;
    boost::replace_all(out, "<", "&lt;");
    boost::replace_all(out, ">", "&gt;");
    return out;
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

} // namespace ocpp1_6
