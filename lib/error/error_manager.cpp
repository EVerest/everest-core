// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/error/error_manager.hpp>

#include <utils/error/error_exceptions.hpp>
#include <utils/error/error_filter.hpp>

namespace Everest {
namespace error {

std::string request_clear_error_option_to_string(const RequestClearErrorOption& o) {
    switch (o) {
    case RequestClearErrorOption::ClearUUID:
        return "ClearUUID";
    case RequestClearErrorOption::ClearAllOfTypeOfModule:
        return "ClearAllOfTypeOfModule";
    case RequestClearErrorOption::ClearAllOfModule:
        return "ClearAllOfModule";
    }
    throw std::out_of_range("No known string conversion for provided enum of type RequestClearErrorOption.");
}

RequestClearErrorOption string_to_request_clear_error_option(const std::string& s) {
    if (s == "ClearUUID") {
        return RequestClearErrorOption::ClearUUID;
    } else if (s == "ClearAllOfTypeOfModule") {
        return RequestClearErrorOption::ClearAllOfTypeOfModule;
    } else if (s == "ClearAllOfModule") {
        return RequestClearErrorOption::ClearAllOfModule;
    }
    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type RequestClearErrorOption.");
}

ErrorManager::ErrorManager(std::shared_ptr<ErrorDatabase> database_) : database(database_) {
}

void ErrorManager::raise_error(ErrorPtr error) {
    this->database->add_error(error);
}

std::list<ErrorPtr> ErrorManager::clear_errors(const RequestClearErrorOption clear_option,
                                               const ImplementationIdentifier& impl,
                                               const std::optional<ErrorHandle>& handle,
                                               const std::optional<ErrorType>& type) {
    std::list<ErrorFilter> filters;
    filters.push_back(ErrorFilter(OriginFilter(impl)));
    switch (clear_option) {
    case RequestClearErrorOption::ClearUUID: {
        if (!handle.has_value()) {
            throw EverestArgumentError("RequestClearErrorOption::ClearUUID requires a handle");
        }
        if (type.has_value()) {
            throw EverestArgumentError("RequestClearErrorOption::ClearUUID does not support type");
        }
        filters.push_back(ErrorFilter(HandleFilter(handle.value())));
    } break;
    case RequestClearErrorOption::ClearAllOfTypeOfModule: {
        if (!type.has_value()) {
            throw EverestArgumentError("RequestClearErrorOption::ClearAllOfTypeOfModule requires a type");
        }
        if (handle.has_value()) {
            throw EverestArgumentError("RequestClearErrorOption::ClearAllOfTypeOfModule does not support handle");
        }
        filters.push_back(ErrorFilter(TypeFilter(type.value())));
    } break;
    case RequestClearErrorOption::ClearAllOfModule: {
        if (type.has_value()) {
            throw EverestArgumentError("RequestClearErrorOption::ClearAllOfModule does not support type");
        }
        if (handle.has_value()) {
            throw EverestArgumentError("RequestClearErrorOption::ClearAllOfModule does not support handle");
        }
    } break;
    default: {
        throw std::out_of_range("No known input validation for provided enum of type RequestClearErrorOption.");
    }
    }
    return this->database->remove_errors(filters);
}

} // namespace error
} // namespace Everest
