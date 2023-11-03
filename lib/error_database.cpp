// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <utils/error_database.hpp>

#include <utils/error.hpp>

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

#include <fmt/format.h>

namespace Everest {
namespace error {

ErrorHandle ErrorDatabase::add_error(std::shared_ptr<Error> error) {
    BOOST_LOG_FUNCTION();

    ErrorHandle handle(error->uuid);
    if (this->has_error(handle)) {
        EVTHROW(EverestErrorAlreadyExistsError(handle));
    }
    std::lock_guard<std::mutex> guard(this->errors_mutex);
    this->errors[handle] = error;
    return handle;
}

std::shared_ptr<Error> ErrorDatabase::clear_error_handle(const ImplementationIdentifier& impl,
                                                         const ErrorHandle& handle) {
    BOOST_LOG_FUNCTION();

    std::lock_guard<std::mutex> guard(this->errors_mutex);
    if (this->errors.find(handle) == this->errors.end()) {
        EVTHROW(EverestErrorDoesNotExistsError(handle));
    }
    if (this->errors.at(handle)->from != impl) {
        EVTHROW(EverestFalseRequestorError(impl, this->errors.at(handle)->from));
    }
    std::shared_ptr<Error> error = this->errors.at(handle);
    this->errors.erase(handle);
    return error;
}

std::list<std::shared_ptr<Error>>
ErrorDatabase::clear_all_errors_of_type_of_module(const ImplementationIdentifier& impl, const std::string& type) {
    BOOST_LOG_FUNCTION();

    std::lock_guard<std::mutex> guard(this->errors_mutex);
    std::list<std::shared_ptr<Error>> cleared_errors;
    for (auto it = this->errors.begin(); it != this->errors.end();) {
        if (it->second->from != impl) {
            it++;
            continue;
        }
        if (it->second->type != type) {
            it++;
            continue;
        }
        cleared_errors.push_back(it->second);
        it = this->errors.erase(it);
    }
    return cleared_errors;
}

std::list<std::shared_ptr<Error>> ErrorDatabase::clear_all_errors_of_module(const ImplementationIdentifier& impl) {
    BOOST_LOG_FUNCTION();

    std::lock_guard<std::mutex> guard(this->errors_mutex);
    std::list<std::shared_ptr<Error>> cleared_errors;
    for (auto it = this->errors.begin(); it != this->errors.end();) {
        if (it->second->from != impl) {
            it++;
            continue;
        }
        cleared_errors.push_back(it->second);
        it = this->errors.erase(it);
    }
    return cleared_errors;
}

bool ErrorDatabase::has_error(const ErrorHandle& handle) {
    std::lock_guard<std::mutex> guard(this->errors_mutex);
    return this->errors.find(handle) != this->errors.end();
}

} // namespace error
} // namespace Everest
