// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef SESSION_LOG_HPP
#define SESSION_LOG_HPP

#include <fstream>
#include <string>
#include <optional>

namespace module {
/*
 Simple session logger that outputs to one file per session and EVLOG
*/

class SessionLog {
public:
    SessionLog();
    ~SessionLog();

    void setPath(const std::string& path);
    void enable();
    std::optional<std::string> startSession(const std::string& suffix_string);
    void stopSession();

    void car(bool iso15118, const std::string& msg);
    void car(bool iso15118, const std::string& msg, const std::string& xml, const std::string& xml_hex,
             const std::string& xml_base64, const std::string& json_str);

    void evse(bool iso15118, const std::string& msg);
    void evse(bool iso15118, const std::string& msg, const std::string& xml, const std::string& xml_hex,
              const std::string& xml_base64, const std::string& json_str);

    void xmlOutput(bool e);

    void sys(const std::string& msg);

private:
    void output(unsigned int evse, bool iso15118, const std::string& msg, const std::string& xml,
                const std::string& xml_hex, const std::string& xml_base64, const std::string& json_str);
    std::string html_encode(const std::string& msg);
    bool xmloutput;
    bool session_active;
    bool enabled;
    std::string logpath_root;
    std::string logpath;
    std::string fn, fnhtml, fn_complete, fnhtml_complete;

    std::ofstream logfile_csv;
    std::ofstream logfile_html;
};

extern SessionLog session_log;

} // namespace module

#endif // SESSION_LOG_HPP
