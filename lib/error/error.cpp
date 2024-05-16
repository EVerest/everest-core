// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <utils/error.hpp>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <utils/error/error_exceptions.hpp>

namespace Everest {
namespace error {

UUID::UUID() {
    uuid = boost::uuids::to_string(boost::uuids::random_generator()());
}

UUID::UUID(const std::string& uuid_) : uuid(uuid_) {
}

bool UUID::operator<(const UUID& other) const {
    return uuid < other.uuid;
}

bool UUID::operator==(const UUID& other) const {
    return this->uuid == other.uuid;
}

bool UUID::operator!=(const UUID& other) const {
    return !(*this == other);
}

std::string UUID::to_string() const {
    return uuid;
}

Error::Error(const ErrorType& type_, const ErrorSubType& sub_type_, const std::string& message_,
             const std::string& description_, const ImplementationIdentifier& origin_, const Severity& severity_,
             const time_point& timestamp_, const UUID& uuid_, const State& state_) :
    type(type_),
    sub_type(sub_type_),
    message(message_),
    description(description_),
    origin(origin_),
    severity(severity_),
    timestamp(timestamp_),
    uuid(uuid_),
    state(state_) {
}

Error::Error(const ErrorType& type_, const ErrorSubType& sub_type_, const std::string& message_,
             const std::string& description_, const ImplementationIdentifier& origin_, const Severity& severity_) :
    Error(type_, sub_type_, message_, description_, origin_, severity_, UTILS_ERROR_DEFAULTS_TIMESTAMP,
          UTILS_ERROR_DEFAULTS_UUID) {
}

Error::Error(const ErrorType& type_, const ErrorSubType& sub_type_, const std::string& message_,
             const std::string& description_, const std::string& origin_module_,
             const std::string& origin_implementation_, const Severity& severity_) :
    Error(type_, sub_type_, message_, description_, ImplementationIdentifier(origin_module_, origin_implementation_),
          severity_) {
}

Error::Error() :
    Error(UTILS_ERROR_DEFAULTS_TYPE, UTILS_ERROR_DEFAULTS_SUB_TYPE, UTILS_ERROR_DEFAULTS_MESSAGE,
          UTILS_ERROR_DEFAULTS_DESCRIPTION, UTILS_ERROR_DEFAULTS_ORIGIN, UTILS_ERROR_DEFAULTS_SEVERITY) {
}

std::string severity_to_string(const Severity& s) {
    switch (s) {
    case Severity::Low:
        return "Low";
    case Severity::Medium:
        return "Medium";
    case Severity::High:
        return "High";
    }
    throw std::out_of_range("No known string conversion for provided enum of type Severity.");
}

Severity string_to_severity(const std::string& s) {
    if (s == "Low") {
        return Severity::Low;
    } else if (s == "Medium") {
        return Severity::Medium;
    } else if (s == "High") {
        return Severity::High;
    }
    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type Severity.");
}

std::string state_to_string(const State& s) {
    switch (s) {
    case State::Active:
        return "Active";
    case State::ClearedByModule:
        return "ClearedByModule";
    case State::ClearedByReboot:
        return "ClearedByReboot";
    }
    throw std::out_of_range("No known string conversion for provided enum of type State.");
}

State string_to_state(const std::string& s) {
    if (s == "Active") {
        return State::Active;
    } else if (s == "ClearedByModule") {
        return State::ClearedByModule;
    } else if (s == "ClearedByReboot") {
        return State::ClearedByReboot;
    }
    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type State.");
}

} // namespace error
} // namespace Everest
