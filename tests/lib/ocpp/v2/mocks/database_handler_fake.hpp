// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "database_handler_mock.hpp"
#include <ocpp/v2/database_handler.hpp>

namespace ocpp::v2 {
class DatabaseHandlerFake : public DatabaseHandlerMock {
    DatabaseHandler handler;

public:
    DatabaseHandlerFake(std::unique_ptr<everest::db::sqlite::ConnectionInterface> database,
                        const fs::path& sql_migration_files_path) :
        handler(std::move(database), sql_migration_files_path) {

        // Add more wrappers here as needed
        ON_CALL(*this, new_statement).WillByDefault([this](const std::string& sql) {
            return handler.new_statement(sql);
        });
    }

    void open_connection() {
        this->handler.open_connection();
    }
};
} // namespace ocpp::v2