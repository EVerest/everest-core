// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef SQLITE_STATEMENT_HPP
#define SQLITE_STATEMENT_HPP

#include <sqlite3.h>

#include <everest/logging.hpp>
#include <ocpp/common/types.hpp>

namespace ocpp {

/// @brief Type used to indicate if SQLite should make a internal copy of a string
enum class SQLiteString {
    Static,   /// Indicates string will be valid for the whole statement
    Transient /// Indicates string might change during statement, SQLite should make a copy
};

/// \brief RAII wrapper class that handles finalization, step, binding and column access of sqlite3_stmt
class SQLiteStatement {
private:
    sqlite3_stmt* stmt;
    sqlite3* db;

public:
    SQLiteStatement(sqlite3* db, const std::string& query) : db(db), stmt(nullptr) {
        if (sqlite3_prepare_v2(db, query.c_str(), query.size(), &this->stmt, nullptr) != SQLITE_OK) {
            EVLOG_error << sqlite3_errmsg(db);
            EVLOG_AND_THROW(std::runtime_error("Could not prepare statement for database."));
        }
    }

    ~SQLiteStatement() {
        if (this->stmt != nullptr) {
            if (sqlite3_finalize(this->stmt) != SQLITE_OK) {
                EVLOG_error << "Error finalizing statement: " << sqlite3_errmsg(this->db);
            }
        }
    }

    sqlite3_stmt* get() const {
        return this->stmt;
    }

    int step() {
        return sqlite3_step(this->stmt);
    }

    int reset() {
        return sqlite3_reset(this->stmt);
    }

    int bind_text(const int idx, const std::string& val, SQLiteString lifetime = SQLiteString::Static) {
        return sqlite3_bind_text(this->stmt, idx, val.c_str(), val.length(),
                                 lifetime == SQLiteString::Static ? SQLITE_STATIC : SQLITE_TRANSIENT);
    }

    int bind_text(const std::string& param, const std::string& val, SQLiteString lifetime = SQLiteString::Static) {
        int index = sqlite3_bind_parameter_index(this->stmt, param.c_str());
        if (index <= 0) {
            throw std::out_of_range("Parameter not found in SQL query");
        }
        return bind_text(index, val, lifetime);
    }

    int bind_int(const int idx, const int val) {
        return sqlite3_bind_int(this->stmt, idx, val);
    }

    int bind_int(const std::string& param, const int val) {
        int index = sqlite3_bind_parameter_index(this->stmt, param.c_str());
        if (index <= 0) {
            throw std::out_of_range("Parameter not found in SQL query");
        }
        return bind_int(index, val);
    }

    int bind_datetime(const int idx, const ocpp::DateTime val) {
        return sqlite3_bind_int64(
            this->stmt, idx,
            std::chrono::duration_cast<std::chrono::milliseconds>(val.to_time_point().time_since_epoch()).count());
    }

    int bind_datetime(const std::string& param, const ocpp::DateTime val) {
        int index = sqlite3_bind_parameter_index(this->stmt, param.c_str());
        if (index <= 0) {
            throw std::out_of_range("Parameter not found in SQL query");
        }
        return bind_datetime(index, val);
    }

    int bind_double(const int idx, const double val) {
        return sqlite3_bind_double(this->stmt, idx, val);
    }

    int bind_double(const std::string& param, const double val) {
        int index = sqlite3_bind_parameter_index(this->stmt, param.c_str());
        if (index <= 0) {
            throw std::out_of_range("Parameter not found in SQL query");
        }
        return bind_double(index, val);
    }

    int bind_null(const int idx) {
        return sqlite3_bind_null(this->stmt, idx);
    }

    int bind_null(const std::string& param) {
        int index = sqlite3_bind_parameter_index(this->stmt, param.c_str());
        if (index <= 0) {
            throw std::out_of_range("Parameter not found in SQL query");
        }
        return bind_null(index);
    }

    int column_type(const int idx) {
        return sqlite3_column_type(this->stmt, idx);
    }

    std::string column_text(const int idx) {
        return reinterpret_cast<const char*>(sqlite3_column_text(this->stmt, idx));
    }

    std::optional<std::string> column_text_nullable(const int idx) {
        auto p = sqlite3_column_text(this->stmt, idx);
        if (p != nullptr) {
            return reinterpret_cast<const char*>(p);
        } else {
            return std::optional<std::string>{};
        }
    }

    int column_int(const int idx) {
        return sqlite3_column_int(this->stmt, idx);
    }

    ocpp::DateTime column_datetime(const int idx) {
        int64_t time = sqlite3_column_int64(this->stmt, idx);
        return DateTime(date::utc_clock::time_point(std::chrono::milliseconds(time)));
    }

    double column_double(const int idx) {
        return sqlite3_column_double(this->stmt, idx);
    }
};

} // namespace ocpp

#endif // DEVICE_MODEL_STORAGE_SQLITE_HPP
