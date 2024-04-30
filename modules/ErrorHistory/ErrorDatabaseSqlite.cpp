// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ErrorDatabaseSqlite.hpp"

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

#include <SQLiteCpp/SQLiteCpp.h>
#include <utils/date.hpp>

namespace module {

ErrorDatabaseSqlite::ErrorDatabaseSqlite(const fs::path& db_path_, const bool reset_) :
    db_path(fs::absolute(db_path_)) {
    BOOST_LOG_FUNCTION();
    std::lock_guard<std::mutex> lock(this->db_mutex);

    bool reset = reset_ || !fs::exists(this->db_path);
    if (reset) {
        EVLOG_info << "Resetting database";
        this->reset_database();
    } else {
        EVLOG_info << "Using database at " << this->db_path;
        this->check_database();
    }
}

void ErrorDatabaseSqlite::check_database() {
    BOOST_LOG_FUNCTION();
    EVLOG_info << "Checking database";
    std::shared_ptr<SQLite::Database> db;
    try {
        db = std::make_shared<SQLite::Database>(this->db_path.string(), SQLite::OPEN_READONLY);
    } catch (std::exception& e) {
        EVLOG_error << "Error opening database: " << e.what();
        throw;
    }
    try {
        std::string sql = "SELECT name";
        sql += " FROM sqlite_schema";
        sql += " WHERE type = 'table' AND name NOT LIKE 'sqlite_%';";
        SQLite::Statement stmt(*db, sql);
        bool has_errors_table = false;
        while (stmt.executeStep()) {
            std::string table_name = stmt.getColumn(0);
            if (table_name == "errors") {
                if (has_errors_table) {
                    throw Everest::EverestConfigError("Database contains multiple errors tables");
                }
                has_errors_table = true;
                EVLOG_debug << "Found errors table";
            } else {
                EVLOG_warning << "Found unknown table: " << table_name;
            }
        }
        if (!has_errors_table) {
            throw Everest::EverestConfigError("Database does not contain errors table");
        }
    } catch (std::exception& e) {
        EVLOG_error << "Error checking whether table 'errors' exist" << e.what();
        throw;
    }
}

void ErrorDatabaseSqlite::reset_database() {
    BOOST_LOG_FUNCTION();
    fs::path database_directory = this->db_path.parent_path();
    if (!fs::exists(database_directory)) {
        fs::create_directories(database_directory);
    }
    if (fs::exists(this->db_path)) {
        fs::remove(this->db_path);
    }
    try {
        SQLite::Database db(this->db_path.string(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        std::string sql = "CREATE TABLE errors("
                          "uuid TEXT PRIMARY      KEY     NOT NULL,"
                          "type                   TEXT    NOT NULL,"
                          "description            TEXT    NOT NULL,"
                          "message                TEXT    NOT NULL,"
                          "from_module            TEXT    NOT NULL,"
                          "from_implementation    TEXT    NOT NULL,"
                          "timestamp              TEXT    NOT NULL,"
                          "severity               TEXT    NOT NULL,"
                          "state                  TEXT    NOT NULL,"
                          "sub_type               TEXT    NOT NULL);";
        db.exec(sql);
    } catch (std::exception& e) {
        EVLOG_error << "Error creating database: " << e.what();
        throw;
    }
}

void ErrorDatabaseSqlite::add_error(Everest::error::ErrorPtr error) {
    std::lock_guard<std::mutex> lock(this->db_mutex);
    this->add_error_without_mutex(error);
}

void ErrorDatabaseSqlite::add_error_without_mutex(Everest::error::ErrorPtr error) {
    BOOST_LOG_FUNCTION();
    try {
        SQLite::Database db(this->db_path.string(), SQLite::OPEN_READWRITE);
        std::string sql = "INSERT INTO errors(uuid, type, description, message, from_module, from_implementation, "
                          "timestamp, severity, state, sub_type) VALUES(";
        sql += "?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10);";
        SQLite::Statement stmt(db, sql);
        stmt.bind(1, error->uuid.to_string());
        stmt.bind(2, error->type);
        stmt.bind(3, error->description);
        stmt.bind(4, error->message);
        stmt.bind(5, error->from.module_id);
        stmt.bind(6, error->from.implementation_id);
        stmt.bind(7, Everest::Date::to_rfc3339(error->timestamp));
        stmt.bind(8, Everest::error::severity_to_string(error->severity));
        stmt.bind(9, Everest::error::state_to_string(error->state));
        stmt.bind(10, error->sub_type.has_value() ? error->sub_type.value() : "");
        stmt.exec();
    } catch (std::exception& e) {
        EVLOG_error << "Error adding error to database: " << e.what();
        throw;
    }
}

std::string ErrorDatabaseSqlite::filter_to_sql_condition(const Everest::error::ErrorFilter& filter) {
    std::string condition = "";
    switch (filter.get_filter_type()) {
    case Everest::error::FilterType::State: {
        condition = "(state = '" + Everest::error::state_to_string(filter.get_state_filter()) + "')";
    } break;
    case Everest::error::FilterType::Origin: {
        condition = "(from_module = '" + filter.get_origin_filter().module_id + "' AND " + "from_implementation = '" +
                    filter.get_origin_filter().implementation_id + "')";
    } break;
    case Everest::error::FilterType::Type: {
        condition = "(type = '" + filter.get_type_filter() + "')";
    } break;
    case Everest::error::FilterType::Severity: {
        switch (filter.get_severity_filter()) {
        case Everest::error::SeverityFilter::LOW_GE: {
            condition = "(severity = '" + Everest::error::severity_to_string(Everest::error::Severity::Low) +
                        "' OR severity = '" + Everest::error::severity_to_string(Everest::error::Severity::Medium) +
                        "' OR severity = '" + Everest::error::severity_to_string(Everest::error::Severity::High) + "')";
        } break;
        case Everest::error::SeverityFilter::MEDIUM_GE: {
            condition = "(severity = '" + Everest::error::severity_to_string(Everest::error::Severity::Medium) +
                        "' OR severity = '" + Everest::error::severity_to_string(Everest::error::Severity::High) + "')";
        } break;
        case Everest::error::SeverityFilter::HIGH_GE: {
            condition = "(severity = '" + Everest::error::severity_to_string(Everest::error::Severity::High) + "')";
        } break;
        }
    } break;
    case Everest::error::FilterType::TimePeriod: {
        condition = "(timestamp BETWEEN '" + Everest::Date::to_rfc3339(filter.get_time_period_filter().from) +
                    "' AND '" + Everest::Date::to_rfc3339(filter.get_time_period_filter().to) + "')";
    } break;
    case Everest::error::FilterType::Handle: {
        condition = "(uuid = '" + filter.get_handle_filter().to_string() + "')";
    } break;
    case Everest::error::FilterType::SubType: {
        condition = "(sub_type = '" + filter.get_sub_type_filter() + "')";
    } break;
    }
    return condition;
}

std::optional<std::string>
ErrorDatabaseSqlite::filters_to_sql_condition(const std::list<Everest::error::ErrorFilter>& filters) {
    std::optional<std::string> condition = std::nullopt;
    if (!filters.empty()) {
        auto it = filters.begin();
        condition = filter_to_sql_condition(*it);
        it++;
        while (it != filters.end()) {
            condition = condition.value() + " AND " + ErrorDatabaseSqlite::filter_to_sql_condition(*it);
            it++;
        }
    }
    return condition;
}

std::list<Everest::error::ErrorPtr>
ErrorDatabaseSqlite::get_errors(const std::list<Everest::error::ErrorFilter>& filters) const {
    std::lock_guard<std::mutex> lock(this->db_mutex);
    return this->get_errors(ErrorDatabaseSqlite::filters_to_sql_condition(filters));
}

std::list<Everest::error::ErrorPtr> ErrorDatabaseSqlite::get_errors(const std::optional<std::string>& condition) const {
    BOOST_LOG_FUNCTION();
    std::list<Everest::error::ErrorPtr> result;
    try {
        SQLite::Database db(this->db_path.string(), SQLite::OPEN_READONLY);
        std::string sql = "SELECT * FROM errors";
        if (condition.has_value()) {
            sql += " WHERE " + condition.value();
        }
        EVLOG_debug << "Executing SQL statement: " << sql;
        SQLite::Statement stmt(db, sql);
        while (stmt.executeStep()) {
            const Everest::error::ErrorType err_type(stmt.getColumn("type").getText());
            const std::string err_description = stmt.getColumn("description").getText();
            const std::string err_msg = stmt.getColumn("message").getText();
            const std::string err_from_module_id = stmt.getColumn("from_module").getText();
            const std::string err_from_impl_id = stmt.getColumn("from_implementation").getText();
            const ImplementationIdentifier err_from(err_from_module_id, err_from_impl_id);
            const Everest::error::Error::time_point err_timestamp =
                Everest::Date::from_rfc3339(stmt.getColumn("timestamp").getText());
            const Everest::error::Severity err_severity =
                Everest::error::string_to_severity(stmt.getColumn("severity").getText());
            const Everest::error::State err_state = Everest::error::string_to_state(stmt.getColumn("state").getText());
            const Everest::error::ErrorHandle err_handle(Everest::error::ErrorHandle(stmt.getColumn("uuid").getText()));
            const Everest::error::ErrorSubType err_sub_type(stmt.getColumn("sub_type").getText());
            Everest::error::ErrorPtr error = std::make_shared<Everest::error::Error>(
                err_type, err_sub_type, err_msg, err_description, err_from, err_severity, err_timestamp, err_handle, err_state);
            result.push_back(error);
        }
    } catch (std::exception& e) {
        EVLOG_error << "Error getting errors from database: " << e.what();
        throw;
    }
    return result;
}

std::list<Everest::error::ErrorPtr>
ErrorDatabaseSqlite::edit_errors(const std::list<Everest::error::ErrorFilter>& filters, EditErrorFunc edit_func) {
    std::lock_guard<std::mutex> lock(this->db_mutex);
    std::list<Everest::error::ErrorPtr> result = this->remove_errors_without_mutex(filters);
    for (Everest::error::ErrorPtr& error : result) {
        edit_func(error);
        this->add_error_without_mutex(error);
    }
    return result;
}

std::list<Everest::error::ErrorPtr>
ErrorDatabaseSqlite::remove_errors(const std::list<Everest::error::ErrorFilter>& filters) {
    std::lock_guard<std::mutex> lock(this->db_mutex);
    return this->remove_errors_without_mutex(filters);
}

std::list<Everest::error::ErrorPtr>
ErrorDatabaseSqlite::remove_errors_without_mutex(const std::list<Everest::error::ErrorFilter>& filters) {
    BOOST_LOG_FUNCTION();
    std::optional<std::string> condition = ErrorDatabaseSqlite::filters_to_sql_condition(filters);
    std::list<Everest::error::ErrorPtr> result = this->get_errors(condition);
    try {
        SQLite::Database db(this->db_path.string(), SQLite::OPEN_READWRITE);
        std::string sql = "DELETE FROM errors";
        if (condition.has_value()) {
            sql += " WHERE " + condition.value();
        }
        db.exec(sql);
    } catch (std::exception& e) {
        EVLOG_error << "Error removing errors from database: " << e.what();
        throw;
    }
    return result;
}

} // namespace module
