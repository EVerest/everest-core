// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_ERROR_DATABASE_HPP
#define UTILS_ERROR_DATABASE_HPP

#include <utils/error.hpp>
#include <utils/types.hpp>

#include <list>
#include <map>
#include <memory>
#include <mutex>

#include <everest/exceptions.hpp>

namespace Everest {
namespace error {

class EverestErrorAlreadyExistsError : public EverestBaseLogicError {
public:
    explicit EverestErrorAlreadyExistsError(const ErrorHandle& handle) :
        EverestBaseLogicError("Error with handle '" + handle.to_string() + "' already exists."){};
};

class EverestErrorDoesNotExistsError : public EverestBaseLogicError {
public:
    explicit EverestErrorDoesNotExistsError(const ErrorHandle& handle) :
        EverestBaseLogicError("Error with handle '" + handle.to_string() + "' does not exists."){};
};

class EverestFalseRequestorError : public EverestBaseLogicError {
public:
    explicit EverestFalseRequestorError(const ImplementationIdentifier& impl_req,
                                        const ImplementationIdentifier& impl_val) :
        EverestBaseLogicError("Request came from " + impl_req.to_string() + ", but should be from " +
                              impl_val.to_string()){};
};

class ErrorDatabase {
public:
    ErrorDatabase() = default;
    ErrorHandle add_error(std::shared_ptr<Error> error);

    std::shared_ptr<Error> clear_error_handle(const ImplementationIdentifier& impl, const ErrorHandle& handle);
    std::list<std::shared_ptr<Error>> clear_all_errors_of_type_of_module(const ImplementationIdentifier& impl,
                                                                         const std::string& type);
    std::list<std::shared_ptr<Error>> clear_all_errors_of_module(const ImplementationIdentifier& impl);

private:
    bool has_error(const ErrorHandle& handle);

    std::map<ErrorHandle, std::shared_ptr<Error>> errors;
    std::mutex errors_mutex;
};

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_DATABASE_HPP
