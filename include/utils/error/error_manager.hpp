// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef UTILS_ERROR_MANAGER_HPP
#define UTILS_ERROR_MANAGER_HPP

#include <utils/error/error_database.hpp>

#include <list>

namespace Everest {
namespace error {

enum class RequestClearErrorOption {
    ClearUUID,
    ClearAllOfTypeOfModule,
    ClearAllOfModule
};
std::string request_clear_error_option_to_string(const RequestClearErrorOption& o);
RequestClearErrorOption string_to_request_clear_error_option(const std::string& s);

class ErrorManager {
public:
    explicit ErrorManager(std::shared_ptr<ErrorDatabase> database);

    void raise_error(ErrorPtr error);
    std::list<ErrorPtr> clear_errors(const RequestClearErrorOption clear_option, const ImplementationIdentifier& impl,
                                     const std::optional<ErrorHandle>& handle, const std::optional<ErrorType>& type);

private:
    std::shared_ptr<ErrorDatabase> database;
};

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_MANAGER_HPP
