// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef ERROR_HISTORY_TESTS_HELPERS_HPP
#define ERROR_HISTORY_TESTS_HELPERS_HPP

#include "../ErrorDatabaseSqlite.hpp"
#include <filesystem>
#include <list>
#include <utils/error.hpp>

namespace fs = std::filesystem;

///
/// \brief get the path to the binary directory
/// \return the path to the binary directory
///
fs::path get_bin_dir();

///
/// \brief get a unique database name
/// \return a unique database name
///
std::string get_unique_db_name();

///
/// \brief get a vector of test errors
/// \return a vector of test errors
///
std::vector<Everest::error::ErrorPtr> get_test_errors();

///
/// \brief check if the given errors are equal
/// \param expected_errors the expected errors
/// \param errors the errors to check
///
void check_expected_errors_in_list(const std::vector<Everest::error::ErrorPtr>& expected_errors,
                                   const std::list<Everest::error::ErrorPtr>& errors);

///
/// \brief wrapper class for the ErrorDatabaseSqlite class
/// This class is used to test the ErrorDatabaseSqlite class
/// It proxies the ErrorDatabaseSqlite class, but
/// the destructor deletes the database file
///
class TestDatabase {
public:
    explicit TestDatabase(const fs::path& db_path_, const bool reset_ = false);
    ~TestDatabase();
    void add_error(Everest::error::ErrorPtr error);
    std::list<Everest::error::ErrorPtr> get_errors(const std::list<Everest::error::ErrorFilter>& filters) const;
    std::list<Everest::error::ErrorPtr> edit_errors(const std::list<Everest::error::ErrorFilter>& filters,
                                                    Everest::error::ErrorDatabase::EditErrorFunc edit_func);
    std::list<Everest::error::ErrorPtr> remove_errors(const std::list<Everest::error::ErrorFilter>& filters);

private:
    std::unique_ptr<module::ErrorDatabaseSqlite> db;
    const fs::path db_path;
};

#endif // ERROR_HISTORY_TESTS_HELPERS_HPP
