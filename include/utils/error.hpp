// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_ERROR_HPP
#define UTILS_ERROR_HPP

#include <filesystem>
#include <fmt/format.h>
#include <map>
#include <string>

#include <utils/date.hpp>
#include <utils/types.hpp>

namespace Everest {
namespace error {

enum class RequestClearErrorOption {
    ClearUUID,
    ClearAllOfTypeOfModule,
    ClearAllOfModule
};
std::string request_clear_error_option_to_string(const RequestClearErrorOption& o);
RequestClearErrorOption string_to_request_clear_error_option(const std::string& s);

class NotValidErrorType : public std::exception {
public:
    explicit NotValidErrorType(const std::string& error_type) :
        explanatory_string(fmt::format("Error type '{}' is not valid.", error_type)){};
    const char* what() const noexcept override {
        return explanatory_string.c_str();
    }

private:
    const std::string explanatory_string;
};

enum class Severity {
    Low,
    Medium,
    High
};

std::string severity_to_string(const Severity& s);
Severity string_to_severity(const std::string& s);

struct UUID {
    UUID();
    explicit UUID(const std::string& uuid);
    bool operator<(const UUID& other) const;
    std::string to_string() const;

    std::string uuid;
};

using ErrorHandle = UUID;
struct Error {
    using time_point = date::utc_clock::time_point;
    Error(const std::string& type, const std::string& message, const std::string& description,
          const ImplementationIdentifier& from, const bool persistent, const Severity& severity,
          const time_point& timestamp, const UUID& uuid);
    Error(const std::string& type, const std::string& message, const std::string& description,
          const ImplementationIdentifier& from, const Severity& severity = Severity::Low);
    Error(const std::string& type, const std::string& message, const std::string& description,
          const std::string& from_module, const std::string& from_implementation,
          const Severity& severity = Severity::Low);

    std::string type;
    std::string description;
    std::string message;
    bool persistent;
    Severity severity;
    ImplementationIdentifier from;
    time_point timestamp;
    UUID uuid;
};

class ErrorTypeMap {
public:
    ErrorTypeMap() = default;
    explicit ErrorTypeMap(std::filesystem::path error_types_dir);
    void load_error_types(std::filesystem::path error_types_dir);
    std::string get_description(const std::string& error_type) const;
    bool has(const std::string& error_type) const;

private:
    std::map<std::string, std::string> error_types;
};

using ErrorCallback = std::function<void(Error)>;

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_HPP
