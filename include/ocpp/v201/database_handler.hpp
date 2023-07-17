// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_DATABASE_HANDLER_HPP
#define OCPP_V201_DATABASE_HANDLER_HPP

#include "sqlite3.h"
#include <filesystem>
#include <fstream>
#include <memory>

#include <ocpp/v201/ocpp_types.hpp>

#include <everest/logging.hpp>

namespace fs = std::filesystem;

namespace ocpp {
namespace v201 {

class DatabaseHandler {

private:
    sqlite3* db;
    fs::path database_file_path;
    fs::path sql_init_path;

    void sql_init();
    bool clear_table(const std::string& table_name);

public:
    DatabaseHandler(const fs::path& database_path, const fs::path& sql_init_path);
    ~DatabaseHandler();

    /// \brief Opens connection to database file
    void open_connection();

    /// \brief Closes connection to database file
    void close_connection();

    /// \brief Inserts cache entry
    /// \param id_token_hash
    /// \param id_token_info
    void insert_auth_cache_entry(const std::string& id_token_hash, const IdTokenInfo& id_token_info);

    /// \brief Gets cache entry for given \p id_token_hash if present
    /// \param id_token_hash
    /// \return
    std::optional<IdTokenInfo> get_auth_cache_entry(const std::string& id_token_hash);

    /// \brief Deletes the cache entry for the given \p id_token_hash
    /// \param id_token_hash
    void delete_auth_cache_entry(const std::string& id_token_hash);

    /// \brief Deletes all entries of the AUTH_CACHE table. Returns true if the operation was successful, else false
    /// \return
    bool clear_authorization_cache();

    void insert_availability(const int32_t evse_id, std::optional<int32_t> connector_id,
                             const OperationalStatusEnum& operational_status, const bool replace);

    OperationalStatusEnum get_availability(const int32_t evse_id, std::optional<int32_t> connector_id);
};

} // namespace v201
} // namespace ocpp

#endif
