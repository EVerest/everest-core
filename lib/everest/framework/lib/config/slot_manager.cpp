// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <utils/config/slot_manager.hpp>

#include <everest/compile_time_settings.hpp>
#include <everest/database/exceptions.hpp>
#include <everest/database/sqlite/connection.hpp>
#include <everest/database/sqlite/schema_updater.hpp>
#include <everest/logging.hpp>

#include <utils/date.hpp>

using namespace everest::db;
using namespace everest::db::sqlite;

namespace everest::config {

namespace fs = std::filesystem;

SqliteConfigSlotManager::SqliteConfigSlotManager(const std::filesystem::path& db_path,
                                                 const std::filesystem::path& migrations_path) {
    db = std::make_shared<Connection>(db_path);

    SchemaUpdater updater{db.get()};
    if (!updater.apply_migration_files(migrations_path, TARGET_MIGRATION_FILE_VERSION)) {
        if (db_path.parent_path().empty()) {
            EVLOG_error << "Could not apply migrations for database at provided path: \"" << db_path.string()
                        << "\" likely because the database path is just a filename. You MUST provide a full path to "
                           "the database.";
        }
        throw MigrationException("SQL migration failed");
    }

    if (!db->open_connection()) {
        throw std::runtime_error("Could not open database at provided path: " + db_path.string());
    } else {
        EVLOG_debug << "Established connection to database successfully: " << db_path;
    }
}

SqliteConfigSlotManager::SqliteConfigSlotManager(std::shared_ptr<everest::db::sqlite::ConnectionInterface> connection) :
    db(std::move(connection)) {
    if (!db->open_connection()) {
        throw std::runtime_error("Could not open shared database connection");
    }
}

SqliteConfigSlotManager::~SqliteConfigSlotManager() {
    db->close_connection();
}

int SqliteConfigSlotManager::next_slot_id() {
    const std::string sql = "SELECT COALESCE(MAX(ID) + 1, 0) FROM CONFIG";
    auto stmt = this->db->new_statement(sql);
    if (stmt->step() == SQLITE_ROW) {
        return stmt->column_int(0);
    }
    return 0;
}

bool SqliteConfigSlotManager::is_valid(int slot_id) {
    const std::string sql = "SELECT 1 FROM CONFIG_META WHERE ID = @config_id AND VALID = 1";
    auto stmt = this->db->new_statement(sql);
    stmt->bind_int("@config_id", slot_id);
    return stmt->step() == SQLITE_ROW;
}

GenericResponseStatus SqliteConfigSlotManager::write_config_slot(int slot_id) {
    auto transaction = this->db->begin_transaction();

    auto config_stmt = this->db->new_statement("INSERT INTO CONFIG (ID) VALUES (?) ON CONFLICT(ID) DO NOTHING;");
    config_stmt->bind_int(1, slot_id);
    if (config_stmt->step() != SQLITE_DONE) {
        return GenericResponseStatus::Failed;
    }

    transaction->commit();
    return GenericResponseStatus::OK;
}

std::vector<SlotInfo> SqliteConfigSlotManager::list_slots() {
    const std::string sql =
        "SELECT c.ID, COALESCE(cm.LAST_UPDATED, ''), COALESCE(cm.VALID, 0), cm.CONFIG_FILE_PATH, cm.DESCRIPTION "
        "FROM CONFIG c LEFT JOIN CONFIG_META cm ON cm.ID = c.ID "
        "ORDER BY c.ID";
    auto stmt = this->db->new_statement(sql);

    std::vector<SlotInfo> result;
    while (stmt->step() == SQLITE_ROW) {
        SlotInfo info;
        info.id = stmt->column_int(0);
        info.last_updated = stmt->column_text(1);
        info.is_valid = stmt->column_int(2) != 0;
        info.config_file_path = stmt->column_text_nullable(3);
        info.description = stmt->column_text_nullable(4);
        result.push_back(std::move(info));
    }
    return result;
}

DuplicateSlotResult SqliteConfigSlotManager::duplicate_slot(int source_slot_id,
                                                            std::optional<std::string> description) {
    try {
        this->db->execute_statement("PRAGMA foreign_keys = ON;");
        auto transaction = this->db->begin_transaction();

        // Verify source exists
        {
            auto check = this->db->new_statement("SELECT 1 FROM CONFIG WHERE ID = ?;");
            check->bind_int(1, source_slot_id);
            if (check->step() != SQLITE_ROW) {
                return {false, std::nullopt};
            }
        }

        const int new_id = next_slot_id();

        // Insert new CONFIG row
        {
            auto s = this->db->new_statement("INSERT INTO CONFIG (ID) VALUES (?);");
            s->bind_int(1, new_id);
            s->step();
        }

        // Copy CONFIG_META (override DESCRIPTION if provided)
        {
            auto s = this->db->new_statement(
                "INSERT INTO CONFIG_META (ID, LAST_UPDATED, VALID, CONFIG_DUMP, CONFIG_FILE_PATH, DESCRIPTION) "
                "SELECT ?, LAST_UPDATED, VALID, CONFIG_DUMP, CONFIG_FILE_PATH, ? FROM CONFIG_META WHERE ID = ?;");
            s->bind_int(1, new_id);
            if (description.has_value()) {
                s->bind_text(2, description.value());
            } else {
                s->bind_null(2);
            }
            s->bind_int(3, source_slot_id);
            s->step();
        }

        // Copy MODULE
        {
            auto s = this->db->new_statement(
                "INSERT INTO MODULE (CONFIG_ID, ID, NAME, STANDALONE, CAPABILITIES) "
                "SELECT ?, ID, NAME, STANDALONE, CAPABILITIES FROM MODULE WHERE CONFIG_ID = ?;");
            s->bind_int(1, new_id);
            s->bind_int(2, source_slot_id);
            s->step();
        }

        // Copy MODULE_FULFILLMENT
        {
            auto s = this->db->new_statement(
                "INSERT INTO MODULE_FULFILLMENT "
                "(CONFIG_ID, MODULE_ID, REQUIREMENT_NAME, IMPLEMENTATION_ID, IMPLEMENTATION_MODULE_ID) "
                "SELECT ?, MODULE_ID, REQUIREMENT_NAME, IMPLEMENTATION_ID, IMPLEMENTATION_MODULE_ID "
                "FROM MODULE_FULFILLMENT WHERE CONFIG_ID = ?;");
            s->bind_int(1, new_id);
            s->bind_int(2, source_slot_id);
            s->step();
        }

        // Copy MODULE_TIER_MAPPING
        {
            auto s = this->db->new_statement(
                "INSERT INTO MODULE_TIER_MAPPING (CONFIG_ID, MODULE_ID, IMPLEMENTATION_ID, EVSE_ID, CONNECTOR_ID) "
                "SELECT ?, MODULE_ID, IMPLEMENTATION_ID, EVSE_ID, CONNECTOR_ID "
                "FROM MODULE_TIER_MAPPING WHERE CONFIG_ID = ?;");
            s->bind_int(1, new_id);
            s->bind_int(2, source_slot_id);
            s->step();
        }

        // Copy CONFIGURATION
        {
            auto s = this->db->new_statement(
                "INSERT INTO CONFIGURATION "
                "(CONFIG_ID, PARAMETER_NAME, MODULE_ID, MODULE_IMPLEMENTATION_ID, VALUE, "
                "MUTABILITY_ID, DATATYPE_ID, UNIT, SOURCE) "
                "SELECT ?, PARAMETER_NAME, MODULE_ID, MODULE_IMPLEMENTATION_ID, VALUE, "
                "MUTABILITY_ID, DATATYPE_ID, UNIT, SOURCE FROM CONFIGURATION WHERE CONFIG_ID = ?;");
            s->bind_int(1, new_id);
            s->bind_int(2, source_slot_id);
            s->step();
        }

        // Copy CONFIG_ACCESS
        {
            auto s = this->db->new_statement(
                "INSERT INTO CONFIG_ACCESS "
                "(CONFIG_ID, MODULE_ID, ALLOW_GLOBAL_READ, ALLOW_GLOBAL_WRITE, ALLOW_SET_READ_ONLY) "
                "SELECT ?, MODULE_ID, ALLOW_GLOBAL_READ, ALLOW_GLOBAL_WRITE, ALLOW_SET_READ_ONLY "
                "FROM CONFIG_ACCESS WHERE CONFIG_ID = ?;");
            s->bind_int(1, new_id);
            s->bind_int(2, source_slot_id);
            s->step();
        }

        // Copy MODULE_CONFIG_ACCESS
        {
            auto s = this->db->new_statement(
                "INSERT INTO MODULE_CONFIG_ACCESS "
                "(CONFIG_ID, MODULE_ID, OTHER_MODULE_ID, ALLOW_READ, ALLOW_WRITE, ALLOW_SET_READ_ONLY) "
                "SELECT ?, MODULE_ID, OTHER_MODULE_ID, ALLOW_READ, ALLOW_WRITE, ALLOW_SET_READ_ONLY "
                "FROM MODULE_CONFIG_ACCESS WHERE CONFIG_ID = ?;");
            s->bind_int(1, new_id);
            s->bind_int(2, source_slot_id);
            s->step();
        }

        transaction->commit();
        this->db->execute_statement("PRAGMA foreign_keys = OFF;");
        return {true, new_id};
    } catch (const std::exception& e) {
        EVLOG_error << "Failed to duplicate slot " << source_slot_id << ": " << e.what();
        return {false, std::nullopt};
    }
}

int SqliteConfigSlotManager::get_next_boot_slot_id() {
    auto stmt = this->db->new_statement("SELECT NEXT_BOOT_SLOT_ID FROM BOOT_CONFIG;");
    if (stmt->step() == SQLITE_ROW) {
        return stmt->column_int(0);
    }
    return DEFAULT_SLOT_ID;
}

GenericResponseStatus SqliteConfigSlotManager::set_next_boot_slot_id(int slot_id) {
    // Verify the target slot exists
    {
        auto check = this->db->new_statement("SELECT 1 FROM CONFIG WHERE ID = ?;");
        check->bind_int(1, slot_id);
        if (check->step() != SQLITE_ROW) {
            return GenericResponseStatus::Failed;
        }
    }
    auto stmt = this->db->new_statement("UPDATE BOOT_CONFIG SET NEXT_BOOT_SLOT_ID = ?;");
    stmt->bind_int(1, slot_id);
    stmt->step();
    return GenericResponseStatus::OK;
}

GenericResponseStatus SqliteConfigSlotManager::delete_slot(int slot_id) {
    try {
        // Enable foreign keys so ON DELETE CASCADE propagates from SETTING to all dependent tables.
        this->db->execute_statement("PRAGMA foreign_keys = ON;");
        auto stmt = this->db->new_statement("DELETE FROM CONFIG WHERE ID = @config_id;");
        stmt->bind_int("@config_id", slot_id);
        stmt->step();
        this->db->execute_statement("PRAGMA foreign_keys = OFF;");
        return GenericResponseStatus::OK;
    } catch (const std::exception& e) {
        EVLOG_error << "Failed to delete slot with id " << slot_id << ": " << e.what();
        return GenericResponseStatus::Failed;
    }
}

} // namespace everest::config
