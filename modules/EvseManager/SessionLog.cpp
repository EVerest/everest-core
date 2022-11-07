// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "SessionLog.hpp"
#include "everest/logging.hpp"
#include "v2gMessage.hpp"
#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <filesystem>
#include <utils/date.hpp>

#include <boost/algorithm/string.hpp>
#include <fmt/core.h>

namespace module {

SessionLog session_log;

SessionLog::SessionLog() {
    session_active = false;
    enabled = false;
    xmloutput = true;
}

SessionLog::~SessionLog() {
    if (logfile_csv.is_open()) {
        logfile_csv.close();
    }
}

void SessionLog::setPath(const std::string& path) {
    logpath = path;
}

void SessionLog::enable() {
    enabled = true;
}

void SessionLog::startSession(const std::string& session_id) {
    if (enabled) {
        if (session_active) {
            stopSession();
        }

        // create log directory if it does not exist
        if (!std::filesystem::exists(logpath))
            std::filesystem::create_directory(logpath);

        // open new file
        std::string ts = Everest::Date::to_rfc3339(date::utc_clock::now());
        const std::string fn = fmt::format("{}/everest-session-{}.log", logpath, session_id);
        const std::string fnhtml = fmt::format("{}/everest-session-{}.html", logpath, session_id);
        // const std::string fn = fmt::format("{}/everest-session.log", logpath);
        // const std::string fnhtml = fmt::format("{}/everest-session.html", logpath);
        try {
            logfile_csv.open(fn);
            logfile_html.open(fnhtml);
            session_active = true;
        } catch (const std::ofstream::failure& e) {
            EVLOG_error << fmt::format("Cannot open {} of {} for writing", fn, fnhtml);
            session_active = false;
        }
        logfile_html << fmt::format("<html><head><title>EVerest log session {}</title>\n", session_id);
        logfile_html << "<style>"
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
                        ".log tr.CAR{background-color: #E4E6F2;}"
                        ".log tr.EVSE{background-color: #F2F0E4;}"
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
        logfile_html << "</head><body><table class=\"log\">\n";
        sys("Session logging started.");
    }
}

void SessionLog::stopSession() {
    if (enabled) {
        sys("Session logging stopped.");

        logfile_html << "</table></body></html>\n";

        if (logfile_csv.is_open()) {
            logfile_csv.close();
        }
        if (logfile_html.is_open()) {
            logfile_html.close();
        }

        session_active = false;
    }
}

void SessionLog::evse(bool iso15118, const std::string& msg) {
    evse(iso15118, msg, "", "", "", "");
}

void SessionLog::car(bool iso15118, const std::string& msg) {
    car(iso15118, msg, "", "", "", "");
}

void SessionLog::evse(bool iso15118, const std::string& msg, const std::string& xml, const std::string& xml_hex,
                      const std::string& xml_base64, const std::string& json_str) {
    output(0, iso15118, msg, xml, xml_hex, xml_base64, json_str);
}

void SessionLog::car(bool iso15118, const std::string& msg, const std::string& xml, const std::string& xml_hex,
                     const std::string& xml_base64, const std::string& json_str) {
    output(1, iso15118, msg, xml, xml_hex, xml_base64, json_str);
}

void SessionLog::output(unsigned int typ, bool iso15118, const std::string& msg, const std::string& xml,
                        const std::string& xml_hex, const std::string& xml_base64, const std::string& json_str) {
    if (enabled && session_active) {
        std::string ts = Everest::Date::to_rfc3339(date::utc_clock::now());

        std::string xml_pretty;
        v2g_message v2g;
        if (!xml.empty()) {
            v2g.from_xml(xml);
            xml_pretty = v2g.to_xml();
        } else if (!json_str.empty()) {
            v2g.from_json(json_str);
            xml_pretty = v2g.to_json();
        }

        // output to EVerest log
        std::string log = msg;
        std::string origin, target;
        if (xmloutput) {
            log += xml_pretty;
        }
        if (typ == 0) {
            origin = "EVSE";
            target = "CAR";
            EVLOG_info << "\033[1;34mEVSE " << (iso15118 ? "ISO" : "IEC") << " " << log << "\033[1;0m";
        } else if (typ == 1) {
            origin = "CAR";
            target = "EVSE";
            EVLOG_info << "                                    \033[1;33mCAR " << (iso15118 ? "ISO" : "IEC") << " "
                       << log << "\033[1;0m";
        } else {
            origin = "SYS";
            target = "";
            EVLOG_info << "SYS  " << msg;
        }

        // output to session log file
        logfile_csv << fmt::format("\"{}\",\"{}\",\"{}\",\"{}\"\n", ts, origin, msg, xml_pretty);
        logfile_csv.flush();

        // output to session html file
        logfile_html << fmt::format("<tr class=\"{}\"> <td>{}</td> <td>{}</td> <td><b>{}</b></td><td><b>{}</b></td> "
                                    "<td><pre lang=\"xml\">{}</pre></td> <td><pre lang=\"xml\">{}</pre></td> <td><pre "
                                    "lang=\"xml\">{}</pre></td> </tr>\n",
                                    origin, ts, origin + "&gt;" + target, (typ == 0 || typ == 2 ? msg : ""),
                                    (typ == 1 ? msg : ""), html_encode(xml_pretty), xml_hex, xml_base64);
        logfile_html.flush();
    }
}

void SessionLog::xmlOutput(bool e) {
    xmloutput = e;
}

void SessionLog::sys(const std::string& msg) {
    output(2, false, msg, "", "", "", "");
}

std::string SessionLog::html_encode(const std::string& msg) {
    std::string out = msg;
    boost::replace_all(out, "<", "&lt;");
    boost::replace_all(out, ">", "&gt;");
    return out;
}

} // namespace module
