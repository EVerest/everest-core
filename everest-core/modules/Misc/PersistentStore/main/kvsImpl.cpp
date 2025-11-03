// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest

#include "kvsImpl.hpp"

#include <filesystem>

namespace fs = std::filesystem;

namespace module {
namespace main {

/**
 * Wrapper class around a sqlite3_stmt pointer to ensure it is always
 * finalised via sqlite3_finalize()
 */
class Sqlite3_Stmt {
private:
    sqlite3_stmt* m_statement_ptr = nullptr;

public:
    void finalize(const char* error_message = nullptr) {
        const auto res = sqlite3_finalize(m_statement_ptr);
        m_statement_ptr = nullptr; // prevent double free
        if (res != SQLITE_OK) {
            if (error_message != nullptr) {
                EVLOG_error << error_message;
            }
            throw std::runtime_error("PersistentStore db access error");
        }
    }

    ~Sqlite3_Stmt() {
        (void)sqlite3_finalize(m_statement_ptr);
    }

    constexpr operator sqlite3_stmt*() {
        return m_statement_ptr;
    }

    constexpr operator sqlite3_stmt**() {
        return &m_statement_ptr;
    }

    constexpr sqlite3_stmt** operator&() {
        return &m_statement_ptr;
    }
};

void kvsImpl::init() {
    // open and initialize database
    fs::path sqlite_db_path = fs::absolute(fs::path(mod->config.sqlite_db_file_path));
    fs::path database_directory = sqlite_db_path.parent_path();
    if (!fs::exists(database_directory)) {
        fs::create_directories(database_directory);
    }

    int ret = sqlite3_open(sqlite_db_path.c_str(), &this->db);

    if (ret != SQLITE_OK) {
        EVLOG_error << "Error opening PersistentStore database '" << sqlite_db_path << "': " << sqlite3_errmsg(db);
        throw std::runtime_error("Could not open PersistentStore database at provided path.");
    }

    EVLOG_debug << "Using SQLite version " << sqlite3_libversion();

    // prepare the database
    std::string create_sql = "CREATE TABLE IF NOT EXISTS KVS ("
                             "KEY   TEXT UNIQUE,"
                             "VALUE TEXT,"
                             "TYPE  TEXT);";

    Sqlite3_Stmt create_statement;
    sqlite3_prepare_v2(this->db, create_sql.c_str(), create_sql.size(), &create_statement, NULL);
    int res = sqlite3_step(create_statement);
    if (res != SQLITE_DONE) {
        EVLOG_error << "Could not create KVS table: " << res << sqlite3_errmsg(this->db);
        throw std::runtime_error("PersistentStore db access error");
    }

    create_statement.finalize("Error creating KVS table");
}

void kvsImpl::ready() {
}

class TypeNameVisitor {
public:
    std::string operator()(std::nullptr_t t) const {
        return "nullptr_t";
    }

    std::string operator()(const Array& t) const {
        return "Array";
    }

    std::string operator()(const Object& t) const {
        return "Object";
    }

    std::string operator()(const bool& t) const {
        return "bool";
    }

    std::string operator()(const double& t) const {
        return "double";
    }

    std::string operator()(const int& t) const {
        return "int";
    }

    std::string operator()(const std::string& t) const {
        return "std::string";
    }
};

class StringValueVisitor {
public:
    std::string operator()(std::nullptr_t t) const {
        return "";
    }

    std::string operator()(Array t) const {
        json a = t;
        return a.dump();
    }

    std::string operator()(Object t) const {
        json o = t;
        return o.dump();
    }

    std::string operator()(bool t) const {
        if (t) {
            return "true";
        }
        return "false";
    }

    std::string operator()(double t) const {
        return std::to_string(t);
    }

    std::string operator()(int t) const {
        return std::to_string(t);
    }

    std::string operator()(std::string t) const {
        return t;
    }
};

void kvsImpl::handle_store(std::string& key,
                           std::variant<std::nullptr_t, Array, Object, bool, double, int, std::string>& value) {
    std::string type = std::visit(TypeNameVisitor(), value);
    std::string string_value = std::visit(StringValueVisitor(), value);
    ;

    std::string insert_sql_str = "INSERT OR REPLACE INTO KVS (KEY, VALUE, TYPE) VALUES "
                                 "(@key, @value, @type)";
    Sqlite3_Stmt insert_statement;
    sqlite3_prepare_v2(db, insert_sql_str.c_str(), insert_sql_str.size(), &insert_statement, NULL);

    sqlite3_bind_text(insert_statement, 1, key.c_str(), -1, NULL);
    sqlite3_bind_text(insert_statement, 2, string_value.c_str(), -1, NULL);
    sqlite3_bind_text(insert_statement, 3, type.c_str(), -1, NULL);

    int res = sqlite3_step(insert_statement);
    if (res != SQLITE_DONE) {
        EVLOG_error << "Could not insert into KVS table: " << res << sqlite3_errmsg(db);
        throw std::runtime_error("PersistentStore db access error");
    }

    insert_statement.finalize("Error inserting into KVS table");
};

std::variant<std::nullptr_t, Array, Object, bool, double, int, std::string> kvsImpl::handle_load(std::string& key) {
    std::string select_sql_str = "SELECT KEY, VALUE, TYPE FROM KVS WHERE KEY = @key";
    Sqlite3_Stmt select_statement;
    sqlite3_prepare_v2(db, select_sql_str.c_str(), select_sql_str.size(), &select_statement, NULL);

    sqlite3_bind_text(select_statement, 1, key.c_str(), -1, NULL);

    int res = sqlite3_step(select_statement);
    if (res != SQLITE_ROW) {
        // no key with that name exists in the database
        return {};
    }

    std::variant<std::nullptr_t, Array, Object, bool, double, int, std::string> value;

    auto value_ptr = sqlite3_column_text(select_statement, 1);
    if (value_ptr != nullptr) {
        std::string value_str = std::string(reinterpret_cast<const char*>(value_ptr));
        auto type_ptr = sqlite3_column_text(select_statement, 2);
        if (type_ptr != nullptr) {
            std::string type_str = std::string(reinterpret_cast<const char*>(type_ptr));
            if (type_str == "Array") {
                Array value_array = json::parse(value_str);
                value = value_array;
            } else if (type_str == "Object") {
                Object value_object = json::parse(value_str);
                value = value_object;
            } else if (type_str == "bool") {
                if (value_str == "true") {
                    value = true;
                } else {
                    value = false;
                }
            } else if (type_str == "double") {
                value = std::stod(value_str);
            } else if (type_str == "int") {
                value = std::stoi(value_str);
            } else if (type_str == "std::string") {
                value = value_str;
            }
        }
    }

    select_statement.finalize("Error selecting from KVS table");
    return value;
};

void kvsImpl::handle_delete(std::string& key) {
    std::string delete_sql_str = "DELETE FROM KVS WHERE KEY = @key";
    Sqlite3_Stmt delete_statement;
    sqlite3_prepare_v2(db, delete_sql_str.c_str(), delete_sql_str.size(), &delete_statement, NULL);

    sqlite3_bind_text(delete_statement, 1, key.c_str(), -1, NULL);

    int res = sqlite3_step(delete_statement);
    if (res != SQLITE_DONE) {
        EVLOG_error << "Could not delete from KVS table: " << res << sqlite3_errmsg(db);
        throw std::runtime_error("PersistentStore db access error");
    }

    delete_statement.finalize("Error deleting from KVS table");
};

bool kvsImpl::handle_exists(std::string& key) {
    std::string select_sql_str = "SELECT KEY FROM KVS WHERE KEY = @key";
    Sqlite3_Stmt select_statement;
    sqlite3_prepare_v2(db, select_sql_str.c_str(), select_sql_str.size(), &select_statement, NULL);

    sqlite3_bind_text(select_statement, 1, key.c_str(), -1, NULL);

    int res = sqlite3_step(select_statement);
    if (res != SQLITE_ROW) {
        // no key with that name exists in the database
        return false;
    }

    return true;
};

} // namespace main
} // namespace module
