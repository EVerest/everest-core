// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef UTILS_ERROR_HPP
#define UTILS_ERROR_HPP

#include <string>

#include <utils/date.hpp>
#include <utils/types.hpp>

#define UTILS_ERROR_DEFAULTS_TYPE        "NotValidType"
#define UTILS_ERROR_DEFAULTS_SUB_TYPE    ""
#define UTILS_ERROR_DEFAULTS_DESCRIPTION "no description provided"
#define UTILS_ERROR_DEFAULTS_MESSAGE     "no message provided"
#define UTILS_ERROR_DEFAULTS_SEVERITY    Everest::error::Severity::Low
#define UTILS_ERROR_DEFAULTS_ORIGIN                                                                                    \
    ImplementationIdentifier("no-module-id-provided", "no-implementation-id-provided", std::nullopt)
#define UTILS_ERROR_DEFAULTS_TIMESTAMP date::utc_clock::now()
#define UTILS_ERROR_DEFAULTS_UUID      UUID()
#define UTILS_ERROR_DEFAULTS_STATE     Everest::error::State::Active
#define UTILS_ERROR_DEFAULTS_VENDOR_ID "everest"

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
using ErrorSubType = std::string;

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
    Error(const ErrorType& type, const ErrorSubType& sub_type, const std::string& message,
          const std::string& description, const ImplementationIdentifier& origin, const std::string& vendor_id,
          const Severity& severity, const time_point& timestamp, const UUID& uuid,
          const State& state = UTILS_ERROR_DEFAULTS_STATE);
    Error(const ErrorType& type, const ErrorSubType& sub_type, const std::string& message,
          const std::string& description, const ImplementationIdentifier& origin,
          const Severity& severity = UTILS_ERROR_DEFAULTS_SEVERITY);
    Error(const ErrorType& type, const ErrorSubType& sub_type, const std::string& message,
          const std::string& description, const std::string& origin_module, const std::string& origin_implementation,
          const Severity& severity = UTILS_ERROR_DEFAULTS_SEVERITY);
    Error();
    ErrorType type;
    ErrorSubType sub_type;
    std::string description;
    std::string message;
    Severity severity;
    ImplementationIdentifier origin;
    time_point timestamp;
    UUID uuid;
    State state;
    std::string vendor_id;
};

using ErrorHandle = UUID;
using ErrorPtr = std::shared_ptr<Error>;
using ErrorCallback = std::function<void(Error)>;

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_HPP
