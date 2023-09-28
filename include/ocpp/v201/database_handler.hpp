// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_DATABASE_HANDLER_HPP
#define OCPP_V201_DATABASE_HANDLER_HPP

#include "sqlite3.h"
#include <deque>
#include <fstream>
#include <memory>
#include <ocpp/common/support_older_cpp_versions.hpp>

#include <ocpp/common/database_handler_base.hpp>
#include <ocpp/v201/ocpp_types.hpp>

#include <everest/logging.hpp>

namespace ocpp {
namespace v201 {

class DatabaseHandler : public ocpp::common::DatabaseHandlerBase {

private:
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

    // Authorization cache management

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

    // Availability management

    void insert_availability(const int32_t evse_id, std::optional<int32_t> connector_id,
                             const OperationalStatusEnum& operational_status, const bool replace);

    OperationalStatusEnum get_availability(const int32_t evse_id, std::optional<int32_t> connector_id);

    // Local authorization list management

    /// \brief Inserts or updates the given \p version in the AUTH_LIST_VERSION table.
    void insert_or_update_local_authorization_list_version(int32_t version);

    /// \brief Returns the version in the AUTH_LIST_VERSION table.
    int32_t get_local_authorization_list_version();

    /// \brief Inserts or updates a local authorization list entry to the AUTH_LIST table.
    void insert_or_update_local_authorization_list_entry(const IdToken& id_token, const IdTokenInfo& id_token_info);

    /// \brief Inserts or updates a local authorization list entries \p local_authorization_list to the AUTH_LIST table.
    void
    insert_or_update_local_authorization_list(const std::vector<v201::AuthorizationData>& local_authorization_list);

    /// \brief Deletes the authorization list entry with the given \p id_tag
    void delete_local_authorization_list_entry(const IdToken& id_token);

    /// \brief Returns the IdTagInfo of the given \p id_tag if it exists in the AUTH_LIST table, else std::nullopt.
    std::optional<v201::IdTokenInfo> get_local_authorization_list_entry(const IdToken& id_token);

    /// \brief Deletes all entries of the AUTH_LIST table.
    bool clear_local_authorization_list();

    /// \brief Get the number of entries currently in the authorization list
    int32_t get_local_authorization_list_number_of_entries();
};

} // namespace v201
} // namespace ocpp

#endif
