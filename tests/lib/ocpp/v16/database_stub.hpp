// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef DATABASE_STUB_HPP
#define DATABASE_STUB_HPP

#include <gtest/gtest.h>

#include <ocpp/v16/connector.hpp>
#include <ocpp/v16/database_handler.hpp>
#include <ocpp/v16/smart_charging.hpp>

namespace stubs {

using namespace ocpp::v16;
using namespace ocpp;
namespace fs = std::filesystem;
using json = nlohmann::json;
using namespace std::chrono;
using ocpp::common::SQLiteString;

// ----------------------------------------------------------------------------
// provide access to the SQLite database handle
struct DatabaseHandlerTest : public DatabaseHandler {
    using DatabaseHandler::DatabaseHandler;
};

struct SQLiteStatementTest : public ocpp::common::SQLiteStatementInterface {
    virtual int changes() {
        return 0;
    }
    virtual int step() {
        return SQLITE_DONE;
    }
    virtual int reset() {
        return 0;
    }
    virtual int bind_text(const int idx, const std::string& val, SQLiteString lifetime = SQLiteString::Static) {
        return 0;
    }
    virtual int bind_text(const std::string& param, const std::string& val,
                          SQLiteString lifetime = SQLiteString::Static) {
        return 0;
    }
    virtual int bind_int(const int idx, const int val) {
        return 0;
    }
    virtual int bind_int(const std::string& param, const int val) {
        return 0;
    }
    virtual int bind_datetime(const int idx, const ocpp::DateTime val) {
        return 0;
    }
    virtual int bind_datetime(const std::string& param, const ocpp::DateTime val) {
        return 0;
    }
    virtual int bind_double(const int idx, const double val) {
        return 0;
    }
    virtual int bind_double(const std::string& param, const double val) {
        return 0;
    }
    virtual int bind_null(const int idx) {
        return 0;
    }
    virtual int bind_null(const std::string& param) {
        return 0;
    }
    virtual int column_type(const int idx) {
        return SQLITE_INTEGER;
    }
    virtual std::string column_text(const int idx) {
        return std::string();
    }
    virtual std::optional<std::string> column_text_nullable(const int idx) {
        return std::nullopt;
    }
    virtual int column_int(const int idx) {
        return 0;
    }
    virtual ocpp::DateTime column_datetime(const int idx) {
        return ocpp::DateTime();
    }
    virtual double column_double(const int idx) {
        return 0.0;
    }
};

struct DatabaseConnectionTest : public common::DatabaseConnectionInterface {
    virtual bool open_connection() {
        return true;
    }
    virtual bool close_connection() {
        return true;
    }
    virtual std::unique_ptr<ocpp::common::DatabaseTransactionInterface> begin_transaction() {
        return std::unique_ptr<ocpp::common::DatabaseTransactionInterface>{};
    }
    virtual bool commit_transaction() {
        return true;
    }
    virtual bool rollback_transaction() {
        return true;
    }
    virtual bool execute_statement(const std::string& statement) {
        return true;
    }
    virtual std::unique_ptr<ocpp::common::SQLiteStatementInterface> new_statement(const std::string& sql) {
        return std::make_unique<SQLiteStatementTest>();
    }
    virtual const char* get_error_message() {
        return "";
    }
    virtual bool clear_table(const std::string& table) {
        return true;
    }
    virtual int64_t get_last_inserted_rowid() {
        return 1;
    }
};

class DbTestBase : public testing::Test {
protected:
    const std::string chargepoint_id = "12345678";
    const fs::path database_path = "/tmp/";
    const fs::path init_script_path = "./core_migrations";
    const fs::path db_filename = database_path / (chargepoint_id + ".db");

    std::map<int32_t, std::shared_ptr<Connector>> connectors;
    std::shared_ptr<stubs::DatabaseHandlerTest> database_handler;
    std::unique_ptr<ocpp::common::DatabaseConnectionInterface> database_interface;

    void add_connectors(unsigned int n) {
        for (unsigned int i = 0; i <= n; i++) {
            if (connectors[i] == nullptr) {
                // create connector
                connectors[i] = std::make_shared<Connector>(i);
            } else {
                // reset connector
                *connectors[i] = Connector(i);
            }
        }
    }

    void SetUp() override {
        database_interface = std::make_unique<stubs::DatabaseConnectionTest>();
        database_handler =
            std::make_shared<stubs::DatabaseHandlerTest>(std::move(database_interface), init_script_path, 1);
    }

    void TearDown() override {
        std::filesystem::remove(db_filename);
        connectors.clear();
    }
};

} // namespace stubs

#endif // DATABASE_STUB_HPP
