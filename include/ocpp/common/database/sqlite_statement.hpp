// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef SQLITE_STATEMENT_HPP
#define SQLITE_STATEMENT_HPP

#include <sqlite3.h>

#include <everest/logging.hpp>
#include <ocpp/common/types.hpp>

namespace ocpp::common {

/// \brief Exception for errors during query execution
class QueryExecutionException : public std::exception {
public:
    explicit QueryExecutionException(const std::string& message) : msg(message) {
    }

    virtual ~QueryExecutionException() noexcept {
    }

    virtual const char* what() const noexcept {
        return msg.c_str();
    }

protected:
    std::string msg;
};

/// @brief Type used to indicate if SQLite should make a internal copy of a string
enum class SQLiteString {
    Static,   /// Indicates string will be valid for the whole statement
    Transient /// Indicates string might change during statement, SQLite should make a copy
};

/// \brief Interface for SQLiteStatement wrapper class that handles finalization, step, binding and column access of
/// sqlite3_stmt
class SQLiteStatementInterface {
public:
    virtual ~SQLiteStatementInterface() = default;

    virtual int step() = 0;
    virtual int reset() = 0;

    virtual int bind_text(const int idx, const std::string& val, SQLiteString lifetime = SQLiteString::Static) = 0;
    virtual int bind_text(const std::string& param, const std::string& val,
                          SQLiteString lifetime = SQLiteString::Static) = 0;
    virtual int bind_int(const int idx, const int val) = 0;
    virtual int bind_int(const std::string& param, const int val) = 0;
    virtual int bind_datetime(const int idx, const ocpp::DateTime val) = 0;
    virtual int bind_datetime(const std::string& param, const ocpp::DateTime val) = 0;
    virtual int bind_double(const int idx, const double val) = 0;
    virtual int bind_double(const std::string& param, const double val) = 0;
    virtual int bind_null(const int idx) = 0;
    virtual int bind_null(const std::string& param) = 0;

    virtual int column_type(const int idx) = 0;
    virtual std::string column_text(const int idx) = 0;
    virtual std::optional<std::string> column_text_nullable(const int idx) = 0;
    virtual int column_int(const int idx) = 0;
    virtual ocpp::DateTime column_datetime(const int idx) = 0;
    virtual double column_double(const int idx) = 0;
};

/// \brief RAII wrapper class that handles finalization, step, binding and column access of sqlite3_stmt
class SQLiteStatement : public SQLiteStatementInterface {
private:
    sqlite3_stmt* stmt;
    sqlite3* db;

public:
    SQLiteStatement(sqlite3* db, const std::string& query);
    ~SQLiteStatement();

    int step() override;
    int reset() override;

    int bind_text(const int idx, const std::string& val, SQLiteString lifetime = SQLiteString::Static) override;
    int bind_text(const std::string& param, const std::string& val,
                  SQLiteString lifetime = SQLiteString::Static) override;
    int bind_int(const int idx, const int val) override;
    int bind_int(const std::string& param, const int val) override;
    int bind_datetime(const int idx, const ocpp::DateTime val) override;
    int bind_datetime(const std::string& param, const ocpp::DateTime val) override;
    int bind_double(const int idx, const double val) override;
    int bind_double(const std::string& param, const double val) override;
    int bind_null(const int idx) override;
    int bind_null(const std::string& param) override;

    int column_type(const int idx) override;
    std::string column_text(const int idx) override;
    std::optional<std::string> column_text_nullable(const int idx) override;
    int column_int(const int idx) override;
    ocpp::DateTime column_datetime(const int idx) override;
    double column_double(const int idx) override;
};

} // namespace ocpp::common

#endif // DEVICE_MODEL_STORAGE_SQLITE_HPP
