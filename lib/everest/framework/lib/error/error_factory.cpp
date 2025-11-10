// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <utility>

#include <utils/error/error_factory.hpp>
#include <utils/error/error_type_map.hpp>

namespace Everest {
namespace error {

ErrorFactory::ErrorFactory(std::shared_ptr<ErrorTypeMap> error_type_map_) :
    ErrorFactory(error_type_map_, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                 std::nullopt) {
}

ErrorFactory::ErrorFactory(std::shared_ptr<ErrorTypeMap> error_type_map_, ImplementationIdentifier default_origin_) :
    ErrorFactory(error_type_map_, default_origin_, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                 std::nullopt) {
}

ErrorFactory::ErrorFactory(std::shared_ptr<ErrorTypeMap> error_type_map_, ImplementationIdentifier default_origin_,
                           Severity default_severity_) :
    ErrorFactory(error_type_map_, default_origin_, default_severity_, std::nullopt, std::nullopt, std::nullopt,
                 std::nullopt, std::nullopt) {
}

ErrorFactory::ErrorFactory(std::shared_ptr<ErrorTypeMap> error_type_map_,
                           std::optional<ImplementationIdentifier> default_origin_,
                           std::optional<Severity> default_severity_, std::optional<State> default_state_,
                           std::optional<ErrorType> default_type_, std::optional<ErrorSubType> default_sub_type_,
                           std::optional<std::string> default_message_, std::optional<std::string> default_vendor_id_) :
    error_type_map(error_type_map_),
    default_origin(std::move(default_origin_)),
    default_severity(default_severity_),
    default_state(default_state_),
    default_type(std::move(default_type_)),
    default_sub_type(std::move(default_sub_type_)),
    default_message(std::move(default_message_)),
    default_vendor_id(std::move(default_vendor_id_)) {
}

Error ErrorFactory::create_error() const {
    Error error;
    if (default_origin.has_value()) {
        error.origin = default_origin.value();
    }
    if (default_severity.has_value()) {
        error.severity = default_severity.value();
    }
    if (default_state.has_value()) {
        error.state = default_state.value();
    }
    if (default_type.has_value()) {
        error.type = default_type.value();
    }
    if (default_sub_type.has_value()) {
        error.sub_type = default_sub_type.value();
    }
    if (default_message.has_value()) {
        error.message = default_message.value();
    }
    if (default_vendor_id.has_value()) {
        error.vendor_id = default_vendor_id.value();
    }
    set_description(error);
    return error;
}

Error ErrorFactory::create_error(const ErrorType& type, const ErrorSubType& sub_type,
                                 const std::string& message) const {
    Error error;
    error.type = type;
    error.sub_type = sub_type;
    error.message = message;
    if (default_origin.has_value()) {
        error.origin = default_origin.value();
    }
    if (default_severity.has_value()) {
        error.severity = default_severity.value();
    }
    if (default_state.has_value()) {
        error.state = default_state.value();
    }
    if (default_vendor_id.has_value()) {
        error.vendor_id = default_vendor_id.value();
    }
    set_description(error);
    return error;
}

Error ErrorFactory::create_error(const ErrorType& type, const ErrorSubType& sub_type, const std::string& message,
                                 const Severity severity) const {
    Error error;
    error.type = type;
    error.sub_type = sub_type;
    error.message = message;
    error.severity = severity;
    if (default_origin.has_value()) {
        error.origin = default_origin.value();
    }
    if (default_state.has_value()) {
        error.state = default_state.value();
    }
    if (default_vendor_id.has_value()) {
        error.vendor_id = default_vendor_id.value();
    }
    set_description(error);
    return error;
}

Error ErrorFactory::create_error(const ErrorType& type, const ErrorSubType& sub_type, const std::string& message,
                                 const State state) const {
    Error error;
    error.type = type;
    error.sub_type = sub_type;
    error.message = message;
    error.state = state;
    if (default_origin.has_value()) {
        error.origin = default_origin.value();
    }
    if (default_severity.has_value()) {
        error.severity = default_severity.value();
    }
    if (default_vendor_id.has_value()) {
        error.vendor_id = default_vendor_id.value();
    }
    set_description(error);
    return error;
}

Error ErrorFactory::create_error(const ErrorType& type, const ErrorSubType& sub_type, const std::string& message,
                                 const Severity severity, const State state) const {
    Error error;
    error.type = type;
    error.sub_type = sub_type;
    error.message = message;
    error.severity = severity;
    error.state = state;
    if (default_origin.has_value()) {
        error.origin = default_origin.value();
    }
    if (default_vendor_id.has_value()) {
        error.vendor_id = default_vendor_id.value();
    }
    set_description(error);
    return error;
}

void ErrorFactory::set_default_origin(const ImplementationIdentifier& origin) {
    default_origin = origin;
}

void ErrorFactory::set_default_severity(Severity severity) {
    default_severity = severity;
}

void ErrorFactory::set_default_state(State state) {
    default_state = state;
}

void ErrorFactory::set_default_type(const ErrorType& type) {
    default_type = type;
}

void ErrorFactory::set_default_sub_type(const ErrorSubType& sub_type) {
    default_sub_type = sub_type;
}

void ErrorFactory::set_default_message(const std::string& message) {
    default_message = message;
}

void ErrorFactory::set_default_vendor_id(const std::string& vendor_id) {
    default_vendor_id = vendor_id;
}

void ErrorFactory::set_description(Error& error) const {
    if (error.type == "") {
        return;
    }
    error.description = error_type_map->get_description(error.type);
}

} // namespace error
} // namespace Everest
