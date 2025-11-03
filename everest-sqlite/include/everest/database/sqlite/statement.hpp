// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <string>
#include <variant>

#include <sqlite3.h>

namespace everest::db::sqlite {

/// @brief Type used to indicate if SQLite should make a internal copy of a string
enum class SQLiteString {
    Static,   /// Indicates string will be valid for the whole statement
    Transient /// Indicates string might change during statement, SQLite should make a copy
};

/// @bried Variant type for possible return value based on name getter
using SqliteVariant = std::variant<std::monostate, int, double, int64_t, std::string>;

/// \brief Interface for Statement wrapper class that handles finalization, step, binding and column access of
/// sqlite3_stmt
class StatementInterface {
public:
    virtual ~StatementInterface() = default;

    virtual int step() = 0;
    virtual int reset() = 0;
    virtual int changes() = 0;

    virtual int bind_text(const int idx, const std::string& val, SQLiteString lifetime = SQLiteString::Static) = 0;
    virtual int bind_text(const std::string& param, const std::string& val,
                          SQLiteString lifetime = SQLiteString::Static) = 0;
    virtual int bind_int(const int idx, const int val) = 0;
    virtual int bind_int(const std::string& param, const int val) = 0;
    virtual int bind_int64(const int idx, const int64_t val) = 0;
    virtual int bind_int64(const std::string& param, const int64_t val) = 0;
    virtual int bind_double(const int idx, const double val) = 0;
    virtual int bind_double(const std::string& param, const double val) = 0;
    virtual int bind_null(const int idx) = 0;
    virtual int bind_null(const std::string& param) = 0;

    virtual int get_number_of_rows() = 0;
    virtual int column_type(const int idx) = 0;
    virtual SqliteVariant column_variant(const std::string& name) = 0;
    virtual std::string column_text(const int idx) = 0;
    virtual std::optional<std::string> column_text_nullable(const int idx) = 0;
    virtual int column_int(const int idx) = 0;
    virtual int64_t column_int64(const int64_t idx) = 0;
    virtual double column_double(const int idx) = 0;
};

/// \brief RAII wrapper class that handles finalization, step, binding and column access of sqlite3_stmt
class Statement : public StatementInterface {
private:
    sqlite3_stmt* stmt;
    sqlite3* db;

public:
    Statement(sqlite3* db, const std::string& query);
    ~Statement();

    int step() override;
    int reset() override;
    int changes() override;

    int bind_text(const int idx, const std::string& val, SQLiteString lifetime = SQLiteString::Static) override;
    int bind_text(const std::string& param, const std::string& val,
                  SQLiteString lifetime = SQLiteString::Static) override;
    int bind_int(const int idx, const int val) override;
    int bind_int(const std::string& param, const int val) override;
    int bind_double(const int idx, const double val) override;
    int bind_double(const std::string& param, const double val) override;
    int bind_int64(const int idx, const int64_t val) override;
    int bind_int64(const std::string& param, const int64_t val) override;
    int bind_null(const int idx) override;
    int bind_null(const std::string& param) override;

    int get_number_of_rows() override;
    int column_type(const int idx) override;
    SqliteVariant column_variant(const std::string& name) override;
    std::string column_text(const int idx) override;
    std::optional<std::string> column_text_nullable(const int idx) override;
    int column_int(const int idx) override;
    int64_t column_int64(const int64_t) override;
    double column_double(const int idx) override;
};

} // namespace everest::db::sqlite
