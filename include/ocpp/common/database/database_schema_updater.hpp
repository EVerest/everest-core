// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/common/database/database_connection.hpp>
#include <ocpp/common/support_older_cpp_versions.hpp>

namespace ocpp::common {

class DatabaseSchemaUpdater {
private:
    DatabaseConnectionInterface* database;

public:
    /// \brief Class that can apply migration files to a database to update the schema
    /// \param database Interface for the database connection
    explicit DatabaseSchemaUpdater(DatabaseConnectionInterface* database) noexcept;

    /// \brief Apply migration files to a database to update the schema
    /// \param sql_migration_files_path Filesystem path to migration file folder
    /// \param target_schema_version The target schema version of the database
    /// \return True if migrations applied successfully, false otherwise. Database is not modified when the migration
    /// fails.
    bool apply_migration_files(const fs::path& migration_file_directory, uint32_t target_schema_version);
};

} // namespace ocpp::common
