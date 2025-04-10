// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ErrorDatabaseSqlite.hpp"

#include <everest/database/exceptions.hpp>
#include <everest/database/sqlite/connection.hpp>
#include <everest/database/sqlite/statement.hpp>
#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

#include <utils/date.hpp>

#include <set>

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
        try {
            this->check_database();
        } catch (const std::exception& e) {
            EVLOG_error << "Error checking database: " << e.what();
            EVLOG_info << "Resetting database";
            this->reset_database();
        }
    }
}

void ErrorDatabaseSqlite::check_database() {
    BOOST_LOG_FUNCTION();
    EVLOG_info << "Checking database";
    try {
        std::shared_ptr<everest::db::sqlite::Connection> db;
        db = std::make_shared<everest::db::sqlite::Connection>(this->db_path);
        if (!db->open_connection()) {
            EVLOG_error << "Error opening database";
            throw everest::db::ConnectionException(db->get_error_message());
        }
        std::string sql = "SELECT name";
        sql += " FROM sqlite_schema";
        sql += " WHERE type = 'table' AND name NOT LIKE 'sqlite_%';";
        auto stmt = db->new_statement(sql);
        bool has_errors_table = false;
        int status;
        while ((status = stmt->step()) == SQLITE_ROW) {
            std::string table_name = stmt->column_text(0);
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
        if (status != SQLITE_DONE) {
            throw Everest::EverestConfigError(db->get_error_message());
        }
        if (!has_errors_table) {
            throw Everest::EverestConfigError("Database does not contain errors table");
        }
        sql = "PRAGMA table_info(errors);";
        auto stmt2 = db->new_statement(sql);
        std::set<std::string> columns;
        while ((status = stmt2->step()) == SQLITE_ROW) {
            auto variant = stmt2->column_variant("name");
            columns.insert(std::get<std::string>(variant));
        }
        std::set<std::string> required_columns = {
            "uuid",      "type",     "description", "message",  "origin_module", "origin_implementation",
            "timestamp", "severity", "state",       "sub_type", "vendor_id"};
        if (status != SQLITE_DONE) {
            throw Everest::EverestConfigError(db->get_error_message());
        }
        if (columns != required_columns) {
            throw Everest::EverestConfigError("Errors table does not contain all required columns");
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Error checking database: " << e.what();
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
        everest::db::sqlite::Connection db(this->db_path);
        if (!db.open_connection()) {
            EVLOG_error << "Error opening database during reset";
            throw everest::db::ConnectionException(db.get_error_message());
        }
        std::string sql = "CREATE TABLE errors("
                          "uuid TEXT PRIMARY      KEY     NOT NULL,"
                          "type                   TEXT    NOT NULL,"
                          "description            TEXT    NOT NULL,"
                          "message                TEXT    NOT NULL,"
                          "origin_module          TEXT    NOT NULL,"
                          "origin_implementation  TEXT    NOT NULL,"
                          "timestamp              TEXT    NOT NULL,"
                          "severity               TEXT    NOT NULL,"
                          "state                  TEXT    NOT NULL,"
                          "sub_type               TEXT    NOT NULL,"
                          "vendor_id              TEXT    NOT NULL);";

        if (!db.execute_statement(sql)) {
            EVLOG_error << "Error creating database during reset";
            throw everest::db::QueryExecutionException(db.get_error_message());
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Error resetting the database: " << e.what();
    }
}

void ErrorDatabaseSqlite::add_error(Everest::error::ErrorPtr error) {
    std::lock_guard<std::mutex> lock(this->db_mutex);
    this->add_error_without_mutex(error);
}

void ErrorDatabaseSqlite::add_error_without_mutex(Everest::error::ErrorPtr error) {
    BOOST_LOG_FUNCTION();
    try {
        everest::db::sqlite::Connection db(this->db_path);
        if (!db.open_connection()) {
            EVLOG_error << "Error opening database";
            throw everest::db::ConnectionException(db.get_error_message());
        }
        std::string sql = "INSERT INTO errors(uuid, type, description, message, origin_module, origin_implementation, "
                          "timestamp, severity, state, sub_type, vendor_id) VALUES(";
        sql += "?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11);";
        auto stmt = db.new_statement(sql);
        stmt->bind_text(1, error->uuid.to_string(), everest::db::sqlite::SQLiteString::Transient);
        stmt->bind_text(2, error->type);
        stmt->bind_text(3, error->description);
        stmt->bind_text(4, error->message);
        stmt->bind_text(5, error->origin.module_id);
        stmt->bind_text(6, error->origin.implementation_id);
        stmt->bind_text(7, Everest::Date::to_rfc3339(error->timestamp), everest::db::sqlite::SQLiteString::Transient);
        stmt->bind_text(8, Everest::error::severity_to_string(error->severity),
                        everest::db::sqlite::SQLiteString::Transient);
        stmt->bind_text(9, Everest::error::state_to_string(error->state), everest::db::sqlite::SQLiteString::Transient);
        stmt->bind_text(10, error->sub_type);
        stmt->bind_text(11, error->vendor_id);
        if (stmt->step() != SQLITE_DONE) {
            throw everest::db::QueryExecutionException(db.get_error_message());
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Error adding error to database: " << e.what();
    }
}

std::string ErrorDatabaseSqlite::filter_to_sql_condition(const Everest::error::ErrorFilter& filter) {
    std::string condition{};
    switch (filter.get_filter_type()) {
    case Everest::error::FilterType::State: {
        condition = "(state = '" + Everest::error::state_to_string(filter.get_state_filter()) + "')";
    } break;
    case Everest::error::FilterType::Origin: {
        condition = "(origin_module = '" + filter.get_origin_filter().module_id + "' AND " +
                    "origin_implementation = '" + filter.get_origin_filter().implementation_id + "')";
    } break;
    case Everest::error::FilterType::Type: {
        condition = "(type = '" + filter.get_type_filter().value + "')";
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
        condition = "(sub_type = '" + filter.get_sub_type_filter().value + "')";
    } break;
    case Everest::error::FilterType::VendorId: {
        condition = "(vendor_id = '" + filter.get_vendor_id_filter().value + "')";
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
        everest::db::sqlite::Connection db(this->db_path);
        if (!db.open_connection()) {
            EVLOG_error << "Error opening database";
            throw everest::db::ConnectionException(db.get_error_message());
        }
        std::string sql = "SELECT * FROM errors";
        if (condition.has_value()) {
            sql += " WHERE " + condition.value();
        }
        EVLOG_debug << "Executing SQL statement: " << sql;
        auto stmt = db.new_statement(sql);
        int status;
        while ((status = stmt->step()) == SQLITE_ROW) {
            const Everest::error::ErrorType err_type(std::get<std::string>(stmt->column_variant("type")));
            const std::string err_description = std::get<std::string>(stmt->column_variant("description"));
            const std::string err_msg = std::get<std::string>(stmt->column_variant("message"));
            const std::string err_origin_module_id = std::get<std::string>(stmt->column_variant("origin_module"));
            const std::string err_origin_impl_id = std::get<std::string>(stmt->column_variant("origin_implementation"));
            const ImplementationIdentifier err_origin(err_origin_module_id, err_origin_impl_id);
            const Everest::error::Error::time_point err_timestamp =
                Everest::Date::from_rfc3339(std::get<std::string>(stmt->column_variant("timestamp")));
            const Everest::error::Severity err_severity =
                Everest::error::string_to_severity(std::get<std::string>(stmt->column_variant("severity")));
            const Everest::error::State err_state =
                Everest::error::string_to_state(std::get<std::string>(stmt->column_variant("state")));
            const Everest::error::ErrorHandle err_handle(
                Everest::error::ErrorHandle(std::get<std::string>(stmt->column_variant("uuid"))));
            const Everest::error::ErrorSubType err_sub_type(std::get<std::string>(stmt->column_variant("sub_type")));
            const std::string err_vendor_id = std::get<std::string>(stmt->column_variant("vendor_id"));
            Everest::error::ErrorPtr error = std::make_shared<Everest::error::Error>(
                err_type, err_sub_type, err_msg, err_description, err_origin, err_vendor_id, err_severity,
                err_timestamp, err_handle, err_state);
            result.push_back(error);
        }
        if (status != SQLITE_DONE) {
            throw everest::db::QueryExecutionException(db.get_error_message());
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Error getting errors from database: " << e.what();
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
        everest::db::sqlite::Connection db(this->db_path);
        if (!db.open_connection()) {
            EVLOG_error << "Error opening database";
            throw everest::db::ConnectionException(db.get_error_message());
        }
        std::string sql = "DELETE FROM errors";
        if (condition.has_value()) {
            sql += " WHERE " + condition.value();
        }
        db.execute_statement(sql);
    } catch (const std::exception& e) {
        EVLOG_error << "Error removing errors from database: " << e.what();
    }
    return result;
}

} // namespace module
