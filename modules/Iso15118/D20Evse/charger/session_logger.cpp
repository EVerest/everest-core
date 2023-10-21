// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "session_logger.hpp"

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <stdexcept>

#include <time.h>

#include <date/date.h>

#include <iso15118/session/logger.hpp>

using LogEvent = iso15118::session::logging::Event;

std::string get_filename_for_current_time() {
    const auto now = std::chrono::system_clock::now();
    const auto now_t = std::chrono::system_clock::to_time_t(now);

    std::tm now_tm;
    localtime_r(&now_t, &now_tm);

    char buffer[64];
    strftime(buffer, sizeof(buffer), "%y%m%d_%H-%M-%S.yaml", &now_tm);
    return buffer;
}

// static auto timepoint_to_string(const iso15118::session::logging::TimePoint& timepoint) {
//     using namespace date;
//     return static_cast<std::string>(timepoint);
// }

std::ostream& operator<<(std::ostream& os, const iso15118::session::logging::ExiMessageDirection& direction) {
    using Direction = iso15118::session::logging::ExiMessageDirection;
    switch (direction) {
    case Direction::FROM_EV:
        return os << "FROM_EV";
    case Direction::TO_EV:
        return os << "TO_EV";
    }

    return os;
}

class SessionLog {
public:
    SessionLog(const std::string& file_name) : file(file_name.c_str(), std::ios::out) {
        if (not file.good()) {
            throw std::runtime_error("Failed to open file " + file_name + " for writing iso15118 session log");
        }

        printf("Created logfile at: %s\n", file_name.c_str());
    }
    void operator()(const iso15118::session::logging::SimpleEvent& event) {
        file << "- type: INFO\n";
        add_timestamp(event.time_point);
        file << "  info: \"" << event.info << "\"\n";
    }

    void operator()(const iso15118::session::logging::ExiMessageEvent& event) {
        file << "- type: EXI\n";
        add_timestamp(event.time_point);
        file << "  direction: " << event.direction << "\n";
        file << "  sdp_payload_type: " << event.payload_type << "\n";
        add_hex_encoded_data(event.data, event.len);
    }

    void flush() {
        file.flush();
    }

private:
    std::fstream file;

    void add_timestamp(const iso15118::session::logging::TimePoint& timestamp) {
        if (not timestamp_initialized) {
            last_timestamp = timestamp;
            timestamp_initialized = true;
        }

        const auto offset_ms = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - last_timestamp);
        file << "  timestamp_offset: " <<  offset_ms.count() << "\n";

        const auto dp = date::floor<date::days>(timestamp);
        const auto time = date::make_time(timestamp-dp);
        const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time.subseconds());
        file << "  timestamp: \"";
        file << std::setfill('0') << std::setw(2) << time.hours().count() << ":";
        file << std::setfill('0') << std::setw(2) << time.minutes().count() << ":";
        file << std::setfill('0') << std::setw(2) << time.seconds().count() << ".";
        file << std::setfill('0') << std::setw(4) << milliseconds.count();
        file << "\"\n";

        last_timestamp = timestamp;
    }

    void add_hex_encoded_data(const uint8_t* data, size_t len) {
        file << "  data: \"";

        const auto flags = file.flags();

        file << std::hex;

        for (int i = 0; i < len; ++i) {
            file << std::setfill('0') << std::setw(2) << static_cast<int>(data[i]);
        }

        file.flags(flags);
        file << "\"\n";
    }

    iso15118::session::logging::TimePoint last_timestamp;
    bool timestamp_initialized {false};
};

SessionLogger::SessionLogger(std::filesystem::path output_dir_) : output_dir(std::filesystem::absolute(output_dir_)) {
    // FIXME (aw): this is quite brute force ...
    if (not std::filesystem::exists(output_dir)) {
        std::filesystem::create_directory(output_dir);
    }

    iso15118::session::logging::set_session_log_callback([this](std::uintptr_t id, const LogEvent& event) {
        auto log_it = logs.find(id);
        if (log_it == logs.end()) {
            const auto log_file_name = output_dir / get_filename_for_current_time();
            const auto emplaced = logs.emplace(id, std::make_unique<SessionLog>(log_file_name.string()));

            log_it = emplaced.first;
        }

        auto& log = *log_it->second;

        std::visit(log, event);
        log.flush();
    });
}

SessionLogger::~SessionLogger() = default;
