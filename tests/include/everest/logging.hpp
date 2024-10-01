// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <filesystem>
#include <iostream>

namespace {
class UTestLogger {
private:
    std::ostream& m_stream;

public:
    UTestLogger() = delete;
    UTestLogger(const char* file, int line) : UTestLogger(std::cerr, "Error: ", file, line) {
    }
    UTestLogger(std::ostream& stream, const char* level, const char* file, int line) : m_stream(stream) {
        const auto f = std::filesystem::path((file == nullptr) ? "(unknown)" : file);
        m_stream << level << f.filename().string() << ':' << line << ' ';
    }
    ~UTestLogger() {
        m_stream << std::endl;
    }
    template <typename T> constexpr std::ostream& operator<<(const T& item) {
        return m_stream << item;
    }
};

#define EVLOG_critical UTestLogger(std::cerr, "Critical: ", __FILE__, __LINE__)
#define EVLOG_error    UTestLogger(std::cerr, "Error:    ", __FILE__, __LINE__)
#define EVLOG_warning  UTestLogger(std::cout, "Warning:  ", __FILE__, __LINE__)
#define EVLOG_info     UTestLogger(std::cout, "Info:     ", __FILE__, __LINE__)
#define EVLOG_debug    UTestLogger(std::cout, "Debug:    ", __FILE__, __LINE__)
#define EVLOG_AND_THROW(ex)                                                                                            \
    do {                                                                                                               \
        try {                                                                                                          \
            throw ex;                                                                                                  \
        } catch (std::exception & e) {                                                                                 \
            EVLOG_error << e.what();                                                                                   \
            throw;                                                                                                     \
        }                                                                                                              \
    } while (0)
} // namespace
