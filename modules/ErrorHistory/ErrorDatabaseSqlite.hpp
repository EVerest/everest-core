// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef ERROR_HISTORY_ERROR_DATABASE_SQLITE_HPP
#define ERROR_HISTORY_ERROR_DATABASE_SQLITE_HPP

#include <utils/error/error_database.hpp>

#include <filesystem>

namespace fs = std::filesystem;

namespace module {

class ErrorDatabaseSqlite : public Everest::error::ErrorDatabase {
public:
    explicit ErrorDatabaseSqlite(const fs::path& db_path_, const bool reset_ = false);

    std::list<Everest::error::ErrorPtr>
    get_errors(const std::list<Everest::error::ErrorFilter>& filters) const override;

    void add_error(Everest::error::ErrorPtr error) override;
    std::list<Everest::error::ErrorPtr> edit_errors(const std::list<Everest::error::ErrorFilter>& filters,
                                                    EditErrorFunc edit_func) override;
    std::list<Everest::error::ErrorPtr> remove_errors(const std::list<Everest::error::ErrorFilter>& filters) override;

private:
    void add_error_without_mutex(Everest::error::ErrorPtr error);
    std::list<Everest::error::ErrorPtr>
    remove_errors_without_mutex(const std::list<Everest::error::ErrorFilter>& filters);
    std::list<Everest::error::ErrorPtr> get_errors(const std::optional<std::string>& condition) const;
    static std::string filter_to_sql_condition(const Everest::error::ErrorFilter& filter);
    static std::optional<std::string> filters_to_sql_condition(const std::list<Everest::error::ErrorFilter>& filters);

    void reset_database();
    void check_database();
    const fs::path db_path;
    mutable std::mutex db_mutex;
};

} // namespace module

#endif // ERROR_HISTORY_ERROR_DATABASE_SQLITE_HPP
