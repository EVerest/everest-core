// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef UTILS_ERROR_DATABASE_HPP
#define UTILS_ERROR_DATABASE_HPP

#include <list>
#include <memory>

#include <utils/error/error_filter.hpp>

namespace Everest {
namespace error {

class ErrorDatabase {
public:
    using EditErrorFunc = std::function<void(ErrorPtr)>;

    ErrorDatabase() = default;

    virtual void add_error(ErrorPtr error) = 0;
    virtual std::list<ErrorPtr> get_errors(const std::list<ErrorFilter>& filters) const = 0;
    virtual std::list<ErrorPtr> edit_errors(const std::list<ErrorFilter>& filters, EditErrorFunc edit_func) = 0;
    virtual std::list<ErrorPtr> remove_errors(const std::list<ErrorFilter>& filters) = 0;
};

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_DATABASE_HPP
