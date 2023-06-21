// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef DEVICE_MODEL_STORAGE_SQLITE_HPP
#define DEVICE_MODEL_STORAGE_SQLITE_HPP

#include <filesystem>
#include <sqlite3.h>

#include <ocpp/v201/device_model_storage.hpp>

namespace ocpp {
namespace v201 {

/// \brief RAII wrapper class that handles finalization, step, binding and column access of sqlite3_stmt
class SQLiteStatement {
private:
    sqlite3_stmt* stmt;

public:
    SQLiteStatement(sqlite3* db, const std::string& query) : stmt(nullptr) {
        if (sqlite3_prepare_v2(db, query.c_str(), query.size(), &stmt, nullptr) != SQLITE_OK) {
            EVLOG_error << sqlite3_errmsg(db);
            EVLOG_AND_THROW(std::runtime_error("Could not prepare statement for database."));
        }
    }

    ~SQLiteStatement() {
        if (stmt != nullptr) {
            sqlite3_finalize(stmt);
        }
    }

    sqlite3_stmt* get() const {
        return stmt;
    }

    int step() {
        return sqlite3_step(stmt);
    }

    int bind_text(const std::string& str, const int idx) {
        return sqlite3_bind_text(stmt, idx, str.c_str(), str.length(), SQLITE_TRANSIENT);
    }

    int bind_int(const int val, const int idx) {
        return sqlite3_bind_int(stmt, idx, val);
    }

    int bind_null(const int idx) {
        return sqlite3_bind_null(stmt, idx);
    }

    int column_type(const int idx) {
        return sqlite3_column_type(stmt, idx);
    }

    std::string column_text(const int idx) {
        return reinterpret_cast<const char*>(sqlite3_column_text(stmt, idx));
    }

    int column_int(const int idx) {
        return sqlite3_column_int(stmt, idx);
    }

    double column_double(const int idx) {
        return sqlite3_column_double(stmt, idx);
    }
};

class DeviceModelStorageSqlite : public DeviceModelStorage {

private:
    sqlite3* db;

    int get_component_id(const Component& component_id);

    int get_variable_id(const Component& component_id, const Variable& variable_id);

public:
    /// \brief Opens SQLite connection at given \p db_path
    /// \param db_path  path to database
    explicit DeviceModelStorageSqlite(const std::filesystem::path& db_path);

    std::map<Component, std::map<Variable, VariableMetaData>> get_device_model() final;

    std::optional<VariableAttribute> get_variable_attribute(const Component& component_id, const Variable& variable_id,
                                                            const AttributeEnum& attribute_enum) final;

    std::vector<VariableAttribute> get_variable_attributes(const Component& component_id, const Variable& variable_id,
                                                           const std::optional<AttributeEnum>& attribute_enum) final;

    bool set_variable_attribute_value(const Component& component_id, const Variable& variable_id,
                                      const AttributeEnum& attribute_enum, const std::string& value) final;
};

} // namespace v201
} // namespace ocpp

#endif // DEVICE_MODEL_STORAGE_SQLITE_HPP
