// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef UTILS_ERROR_FACTORY_HPP
#define UTILS_ERROR_FACTORY_HPP

#include <memory>
#include <optional>
#include <string>

#include <utils/error.hpp>

namespace Everest {
namespace error {

struct ErrorTypeMap;

class ErrorFactory {
public:
    explicit ErrorFactory(std::shared_ptr<ErrorTypeMap> error_type_map);
    ErrorFactory(std::shared_ptr<ErrorTypeMap> error_type_map, ImplementationIdentifier default_origin);
    ErrorFactory(std::shared_ptr<ErrorTypeMap> error_type_map, ImplementationIdentifier default_origin,
                 Severity default_severity);
    ErrorFactory(std::shared_ptr<ErrorTypeMap> error_type_map, std::optional<ImplementationIdentifier> default_origin,
                 std::optional<Severity> default_severity, std::optional<State> default_state,
                 std::optional<ErrorType> default_type, std::optional<ErrorSubType> default_sub_type,
                 std::optional<std::string> default_message, std::optional<std::string> default_vendor_id);

    Error create_error() const;
    Error create_error(const ErrorType& type, const ErrorSubType& sub_type, const std::string& message) const;
    Error create_error(const ErrorType& type, const ErrorSubType& sub_type, const std::string& message,
                       const Severity severity) const;
    Error create_error(const ErrorType& type, const ErrorSubType& sub_type, const std::string& message,
                       const State state) const;
    Error create_error(const ErrorType& type, const ErrorSubType& sub_type, const std::string& message,
                       const Severity severity, const State state) const;

    void set_default_origin(ImplementationIdentifier origin);
    void set_default_severity(Severity severity);
    void set_default_state(State state);
    void set_default_type(ErrorType type);
    void set_default_sub_type(ErrorSubType sub_type);
    void set_default_message(std::string message);
    void set_default_vendor_id(std::string vendor_id);

private:
    std::optional<ImplementationIdentifier> default_origin;
    std::optional<Severity> default_severity;
    std::optional<State> default_state;
    std::optional<ErrorType> default_type;
    std::optional<ErrorSubType> default_sub_type;
    std::optional<std::string> default_message;
    std::optional<std::string> default_vendor_id;

    const std::shared_ptr<ErrorTypeMap> error_type_map;

    void set_description(Error& error) const;
};

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_FACTORY_HPP
