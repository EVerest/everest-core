// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef UTILS_ERROR_HPP
#define UTILS_ERROR_HPP

#include <string>

#include <utils/date.hpp>
#include <utils/types.hpp>

namespace Everest {
namespace error {

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
    bool operator==(const UUID& other) const;
    bool operator!=(const UUID& other) const;
    std::string to_string() const;

    std::string uuid;
};

using ErrorType = std::string;

enum class State {
    Active,
    ClearedByModule,
    ClearedByReboot
};
std::string state_to_string(const State& s);
State string_to_state(const std::string& s);

///
/// \brief The Error struct represents an error object
///
struct Error {
    using time_point = date::utc_clock::time_point;
    Error(const ErrorType& type, const std::string& message, const std::string& description,
          const ImplementationIdentifier& from, const Severity& severity, const time_point& timestamp, const UUID& uuid,
          const State& state = State::Active);
    Error(const ErrorType& type, const std::string& message, const std::string& description,
          const ImplementationIdentifier& from, const Severity& severity = Severity::Low);
    Error(const ErrorType& type, const std::string& message, const std::string& description,
          const std::string& from_module, const std::string& from_implementation,
          const Severity& severity = Severity::Low);
    ErrorType type;
    std::string description;
    std::string message;
    Severity severity;
    ImplementationIdentifier from;
    time_point timestamp;
    UUID uuid;
    State state;
};

using ErrorHandle = UUID;
using ErrorPtr = std::shared_ptr<Error>;
using ErrorCallback = std::function<void(Error)>;

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_HPP
