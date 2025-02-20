// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V2_DATABASE_HANDLER_HPP
#define OCPP_V2_DATABASE_HANDLER_HPP

#include "ocpp/v2/types.hpp"
#include "sqlite3.h"
#include <memory>
#include <ocpp/common/support_older_cpp_versions.hpp>

#include <ocpp/common/database/database_connection.hpp>
#include <ocpp/common/database/database_handler_common.hpp>
#include <ocpp/v2/ocpp_types.hpp>
#include <ocpp/v2/transaction.hpp>

#include <everest/logging.hpp>

namespace ocpp {
namespace v2 {

/// \brief Helper class for retrieving authorization cache entries from the database
struct AuthorizationCacheEntry {
    IdTokenInfo id_token_info;
    DateTime last_used;
};

class DatabaseHandlerInterface {
public:
    virtual ~DatabaseHandlerInterface() = default;

    // Authorization cache management

    /// \brief Inserts cache entry
    /// \param id_token_hash
    /// \param id_token_info
    virtual void authorization_cache_insert_entry(const std::string& id_token_hash,
                                                  const IdTokenInfo& id_token_info) = 0;

    /// \brief Updates the last_used field in the entry
    ///
    /// \param id_token_hash
    /// \retval true if entry was updated
    virtual void authorization_cache_update_last_used(const std::string& id_token_hash) = 0;

    /// \brief Gets cache entry for given \p id_token_hash if present
    /// \param id_token_hash
    /// \return
    virtual std::optional<AuthorizationCacheEntry> authorization_cache_get_entry(const std::string& id_token_hash) = 0;

    /// \brief Deletes the cache entry for the given \p id_token_hash
    /// \param id_token_hash
    virtual void authorization_cache_delete_entry(const std::string& id_token_hash) = 0;

    /// \brief Removes up to \p nr_to_remove items from the cache starting from the least recently used
    ///
    /// \param nr_to_remove Number of items to remove from the database
    /// \retval True if succeeded
    virtual void authorization_cache_delete_nr_of_oldest_entries(size_t nr_to_remove) = 0;

    /// \brief Removes all entries from the cache that have passed their expiry date or auth cache lifetime
    ///
    /// \param auth_cache_lifetime The maximum time tokens can stay in the cache without being used
    /// \retval True if succeeded
    virtual void
    authorization_cache_delete_expired_entries(std::optional<std::chrono::seconds> auth_cache_lifetime) = 0;

    /// \brief Deletes all entries of the AUTH_CACHE table. Returns true if the operation was successful, else false
    virtual void authorization_cache_clear() = 0;

    /// \brief Get the binary size of the authorization cache table
    ///
    /// \retval The size of the authorization cache table in bytes
    virtual size_t authorization_cache_get_binary_size() = 0;

    // Availability

    /// \brief Persist operational settings for the charging station
    virtual void insert_cs_availability(OperationalStatusEnum operational_status, bool replace) = 0;
    /// \brief Retrieve persisted operational settings for the charging station
    virtual OperationalStatusEnum get_cs_availability() = 0;

    /// \brief Persist operational settings for an EVSE
    virtual void insert_evse_availability(int32_t evse_id, OperationalStatusEnum operational_status, bool replace) = 0;
    /// \brief Retrieve persisted operational settings for an EVSE
    virtual OperationalStatusEnum get_evse_availability(int32_t evse_id) = 0;

    /// \brief Persist operational settings for a connector
    virtual void insert_connector_availability(int32_t evse_id, int32_t connector_id,
                                               OperationalStatusEnum operational_status, bool replace) = 0;
    /// \brief Retrieve persisted operational settings for a connector
    virtual OperationalStatusEnum get_connector_availability(int32_t evse_id, int32_t connector_id) = 0;

    // Local authorization list management

    /// \brief Inserts or updates the given \p version in the AUTH_LIST_VERSION table.
    virtual void insert_or_update_local_authorization_list_version(int32_t version) = 0;

    /// \brief Returns the version in the AUTH_LIST_VERSION table.
    virtual int32_t get_local_authorization_list_version() = 0;

    /// \brief Inserts or updates a local authorization list entry to the AUTH_LIST table.
    virtual void insert_or_update_local_authorization_list_entry(const IdToken& id_token,
                                                                 const IdTokenInfo& id_token_info) = 0;

    /// \brief Inserts or updates a local authorization list entries \p local_authorization_list to the AUTH_LIST table.
    virtual void
    insert_or_update_local_authorization_list(const std::vector<v2::AuthorizationData>& local_authorization_list) = 0;

    /// \brief Deletes the authorization list entry with the given \p id_tag
    virtual void delete_local_authorization_list_entry(const IdToken& id_token) = 0;

    /// \brief Returns the IdTagInfo of the given \p id_tag if it exists in the AUTH_LIST table, else std::nullopt.
    virtual std::optional<v2::IdTokenInfo> get_local_authorization_list_entry(const IdToken& id_token) = 0;

    /// \brief Deletes all entries of the AUTH_LIST table.
    virtual void clear_local_authorization_list() = 0;

    /// \brief Get the number of entries currently in the authorization list
    virtual int32_t get_local_authorization_list_number_of_entries() = 0;

    // Transaction metervalues

    /// \brief Inserts a \p meter_value to the database linked to transaction with id \p transaction_id
    virtual void transaction_metervalues_insert(const std::string& transaction_id, const MeterValue& meter_value) = 0;

    /// \brief Get all metervalues linked to transaction with id \p transaction_id
    virtual std::vector<MeterValue> transaction_metervalues_get_all(const std::string& transaction_id) = 0;

    /// \brief Remove all metervalue entries linked to transaction with id \p transaction_id
    virtual void transaction_metervalues_clear(const std::string& transaction_id) = 0;

    // transactions

    /// \brief Inserts a transaction with the given parameters to the TRANSACTIONS table
    /// \param transaction
    /// \param evse_id
    virtual void transaction_insert(const EnhancedTransaction& transaction, int32_t evse_id) = 0;

    /// \brief Gets a transaction from the database if one can be found using \p evse_id
    /// \param evse_id The evse id to get the transaction for
    /// \return nullptr if not found, otherwise an enhanced transaction object.
    virtual std::unique_ptr<EnhancedTransaction> transaction_get(const int32_t evse_id) = 0;

    /// \brief Update the sequence number of the given transaction id in the database.
    /// \param transaction_id
    /// \param seq_no
    virtual void transaction_update_seq_no(const std::string& transaction_id, int32_t seq_no) = 0;

    /// \brief Update the charging state of the given transaction id in the database.
    /// \param transaction_id
    /// \param charging_state
    virtual void transaction_update_charging_state(const std::string& transaction_id,
                                                   const ChargingStateEnum charging_state) = 0;

    /// \brief Update the id_token_sent of the given transaction id in the database.
    /// \param transaction_id
    /// \param id_token_sent
    virtual void transaction_update_id_token_sent(const std::string& transaction_id, bool id_token_sent) = 0;

    /// \brief Clear all the transactions from the TRANSACTIONS table.
    /// \param transaction_id transaction id of the transaction to clear from.
    /// \return true if succeeded
    virtual void transaction_delete(const std::string& transaction_id) = 0;

    /// charging profiles

    /// \brief Inserts or updates the given \p profile to CHARGING_PROFILES table
    virtual void insert_or_update_charging_profile(
        const int evse_id, const v2::ChargingProfile& profile,
        const ChargingLimitSourceEnum charging_limit_source = ChargingLimitSourceEnum::CSO) = 0;

    /// \brief Deletes the profile with the given \p profile_id
    virtual bool delete_charging_profile(const int profile_id) = 0;

    /// \brief Deletes the profiles with the given \p transaction_id
    virtual void delete_charging_profile_by_transaction_id(const std::string& transaction_id) = 0;

    /// \brief Deletes all profiles from table CHARGING_PROFILES
    virtual bool clear_charging_profiles() = 0;

    /// \brief Deletes all profiles from table CHARGING_PROFILES matching \p profile_id or \p criteria
    virtual bool clear_charging_profiles_matching_criteria(const std::optional<int32_t> profile_id,
                                                           const std::optional<ClearChargingProfile>& criteria) = 0;

    /// \brief Get all profiles from table CHARGING_PROFILES matching \p profile_id or \p criteria
    virtual std::vector<ReportedChargingProfile>
    get_charging_profiles_matching_criteria(const std::optional<int32_t> evse_id,
                                            const ChargingProfileCriterion& criteria) = 0;

    /// \brief Retrieves the charging profiles stored on \p evse_id
    virtual std::vector<v2::ChargingProfile> get_charging_profiles_for_evse(const int evse_id) = 0;

    /// \brief Retrieves all ChargingProfiles
    virtual std::vector<v2::ChargingProfile> get_all_charging_profiles() = 0;

    /// \brief Retrieves all ChargingProfiles grouped by EVSE ID
    virtual std::map<int32_t, std::vector<v2::ChargingProfile>> get_all_charging_profiles_group_by_evse() = 0;

    virtual ChargingLimitSourceEnum get_charging_limit_source_for_profile(const int profile_id) = 0;

    virtual std::unique_ptr<common::SQLiteStatementInterface> new_statement(const std::string& sql) = 0;
};

class DatabaseHandler : public DatabaseHandlerInterface, public common::DatabaseHandlerCommon {
private:
    void init_sql() override;

    void inintialize_enum_tables();
    void init_enum_table_inner(const std::string& table_name, const int begin, const int end,
                               std::function<std::string(int)> conversion);
    template <typename T>
    void init_enum_table(const std::string& table_name, T begin, T end, std::function<std::string(T)> conversion);

    // Availability management (internal helpers)
    // Setting evse_id to 0 addresses the whole CS, setting evse_id > 0 and connector_id=0 addresses a whole EVSE
    void insert_availability(int32_t evse_id, int32_t connector_id, OperationalStatusEnum operational_status,
                             bool replace);
    OperationalStatusEnum get_availability(int32_t evse_id, int32_t connector_id);

public:
    DatabaseHandler(std::unique_ptr<common::DatabaseConnectionInterface> database,
                    const fs::path& sql_migration_files_path);

    // Authorization cache management
    void authorization_cache_insert_entry(const std::string& id_token_hash, const IdTokenInfo& id_token_info) override;
    void authorization_cache_update_last_used(const std::string& id_token_hash) override;
    std::optional<AuthorizationCacheEntry> authorization_cache_get_entry(const std::string& id_token_hash) override;
    void authorization_cache_delete_entry(const std::string& id_token_hash) override;
    void authorization_cache_delete_nr_of_oldest_entries(size_t nr_to_remove) override;
    void authorization_cache_delete_expired_entries(std::optional<std::chrono::seconds> auth_cache_lifetime) override;
    void authorization_cache_clear() override;
    size_t authorization_cache_get_binary_size() override;

    // Availability
    virtual void insert_cs_availability(OperationalStatusEnum operational_status, bool replace) override;
    virtual OperationalStatusEnum get_cs_availability() override;
    virtual void insert_evse_availability(int32_t evse_id, OperationalStatusEnum operational_status,
                                          bool replace) override;
    virtual OperationalStatusEnum get_evse_availability(int32_t evse_id) override;
    virtual void insert_connector_availability(int32_t evse_id, int32_t connector_id,
                                               OperationalStatusEnum operational_status, bool replace) override;
    virtual OperationalStatusEnum get_connector_availability(int32_t evse_id, int32_t connector_id) override;

    // Local authorization list management
    void insert_or_update_local_authorization_list_version(int32_t version) override;
    int32_t get_local_authorization_list_version() override;
    void insert_or_update_local_authorization_list_entry(const IdToken& id_token,
                                                         const IdTokenInfo& id_token_info) override;
    void insert_or_update_local_authorization_list(
        const std::vector<v2::AuthorizationData>& local_authorization_list) override;
    void delete_local_authorization_list_entry(const IdToken& id_token) override;
    std::optional<v2::IdTokenInfo> get_local_authorization_list_entry(const IdToken& id_token) override;
    void clear_local_authorization_list() override;
    int32_t get_local_authorization_list_number_of_entries() override;

    // Transaction metervalues
    void transaction_metervalues_insert(const std::string& transaction_id, const MeterValue& meter_value) override;
    std::vector<MeterValue> transaction_metervalues_get_all(const std::string& transaction_id) override;
    void transaction_metervalues_clear(const std::string& transaction_id) override;

    // transactions
    void transaction_insert(const EnhancedTransaction& transaction, int32_t evse_id) override;
    std::unique_ptr<EnhancedTransaction> transaction_get(const int32_t evse_id) override;
    void transaction_update_seq_no(const std::string& transaction_id, int32_t seq_no) override;
    void transaction_update_charging_state(const std::string& transaction_id,
                                           const ChargingStateEnum charging_state) override;
    void transaction_update_id_token_sent(const std::string& transaction_id, bool id_token_sent) override;
    void transaction_delete(const std::string& transaction_id) override;

    /// charging profiles
    void insert_or_update_charging_profile(
        const int evse_id, const v2::ChargingProfile& profile,
        const ChargingLimitSourceEnum charging_limit_source = ChargingLimitSourceEnum::CSO) override;
    bool delete_charging_profile(const int profile_id) override;
    void delete_charging_profile_by_transaction_id(const std::string& transaction_id) override;
    bool clear_charging_profiles() override;
    bool clear_charging_profiles_matching_criteria(const std::optional<int32_t> profile_id,
                                                   const std::optional<ClearChargingProfile>& criteria) override;
    std::vector<ReportedChargingProfile>
    get_charging_profiles_matching_criteria(const std::optional<int32_t> evse_id,
                                            const ChargingProfileCriterion& criteria) override;
    std::vector<v2::ChargingProfile> get_charging_profiles_for_evse(const int evse_id) override;
    std::vector<v2::ChargingProfile> get_all_charging_profiles() override;
    virtual std::map<int32_t, std::vector<v2::ChargingProfile>> get_all_charging_profiles_group_by_evse() override;
    ChargingLimitSourceEnum get_charging_limit_source_for_profile(const int profile_id) override;

    std::unique_ptr<common::SQLiteStatementInterface> new_statement(const std::string& sql) override;
};

} // namespace v2
} // namespace ocpp

#endif
