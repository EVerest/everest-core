// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utils/config/storage_types.hpp>
#include <vector>

namespace everest::db::sqlite {
class ConnectionInterface;
} // namespace everest::db::sqlite

namespace everest::config {

struct DuplicateSlotResult {
    bool success = false;
    std::optional<int> slot_id;
};

struct SlotInfo {
    int id;
    std::string last_updated;
    bool is_valid;
    std::optional<std::string> config_file_path;
    std::optional<std::string> description;
};

using StoredSlotInfo = SlotInfo;

class SqliteConfigSlotManager {
public:
    static constexpr int DEFAULT_SLOT_ID = 0;

    /// \brief Opens its own Connection and applies migrations.
    SqliteConfigSlotManager(const std::filesystem::path& db_path, const std::filesystem::path& migrations_path);

    /// \brief Shares an already-migrated Connection.
    /// Calls open_connection() on the shared connection; the destructor calls close_connection().
    /// \param connection Shared database connection (already migrated)
    explicit SqliteConfigSlotManager(std::shared_ptr<everest::db::sqlite::ConnectionInterface> connection);

    ~SqliteConfigSlotManager();

    bool is_valid(int slot_id = DEFAULT_SLOT_ID);
    /// \brief Returns the next available slot ID (MAX(ID) + 1, or 0 if no slots exist).
    int next_slot_id();
    GenericResponseStatus write_config_slot(int slot_id);
    std::vector<SlotInfo> list_slots();
    GenericResponseStatus delete_slot(int slot_id);

    /// \brief Duplicates all data belonging to \p source_slot_id into a new slot.
    /// \returns DuplicateSlotResult with the new slot_id on success.
    DuplicateSlotResult duplicate_slot(int source_slot_id, std::optional<std::string> description = std::nullopt);

    /// \brief Returns the slot ID that will be used on the next boot (from the BOOT_CONFIG table).
    int get_next_boot_slot_id();

    /// \brief Persists \p slot_id as the next boot slot in the BOOT_CONFIG table.
    /// \returns Failed if \p slot_id does not exist in the CONFIG table.
    GenericResponseStatus set_next_boot_slot_id(int slot_id);

private:
    std::shared_ptr<everest::db::sqlite::ConnectionInterface> db;
};

} // namespace everest::config
