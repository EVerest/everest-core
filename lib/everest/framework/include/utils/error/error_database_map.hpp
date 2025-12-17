// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef ERROR_DATABASE_MAP_HPP
#define ERROR_DATABASE_MAP_HPP

#include <list>
#include <utils/error.hpp>
#include <utils/error/error_database.hpp>

namespace Everest {
namespace error {

class ErrorDatabaseMap : public ErrorDatabase {
public:
    ErrorDatabaseMap() = default;

    void add_error(ErrorPtr error) override;
    std::list<ErrorPtr> get_errors(const std::list<ErrorFilter>& filters) const override;
    std::list<ErrorPtr> edit_errors(const std::list<ErrorFilter>& filters, EditErrorFunc edit_func) override;
    std::list<ErrorPtr> remove_errors(const std::list<ErrorFilter>& filters) override;

private:
    std::list<ErrorPtr> get_errors_no_mutex(const std::list<ErrorFilter>& filters) const;
    std::map<ErrorHandle, ErrorPtr> errors;
    mutable std::mutex errors_mutex;
};

} // namespace error
} // namespace Everest

#endif // ERROR_DATABASE_MAP_HPP
