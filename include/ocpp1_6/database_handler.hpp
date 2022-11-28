// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_DATABASE_HANDLER_HPP
#define OCPP1_6_DATABASE_HANDLER_HPP

#include "sqlite3.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/schemas.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief This class handles the connection and operations of the SQLite database
class DatabaseHandler {
private:
    sqlite3* db;
    boost::filesystem::path db_path;          // directory where the database file is located
    boost::filesystem::path init_script_path; // full path of init sql script

    void run_sql_init();
    bool clear_table(const std::string& table_name);
    void init_connector_table(int32_t number_of_connectors);

public:
    DatabaseHandler(const std::string& chargepoint_id, const boost::filesystem::path& database_path,
                    const boost::filesystem::path& init_script_path);
    ~DatabaseHandler();

    /// \brief Opens the database connection, runs initialization script and initializes the CONNECTORS and
    /// AUTH_LIST_VERSION table.
    void open_db_connection(int32_t number_of_connectors);

    /// \brief Closes the database connection.
    void close_db_connection();

    // transactions
    /// \brief Inserts a transaction with the given parameter to the TRANSACTIONS table.
    void insert_transaction(const std::string& session_id, const int32_t transaction_id, const int32_t connector,
                            const std::string& id_tag_start, const std::string& time_start, const int32_t meter_start,
                            const boost::optional<int32_t> reservation_id);

    /// \brief Updates the given parameters for the transaction with the given \p session_id in the TRANSACTIONS table.
    void update_transaction(const std::string& session_id, int32_t transaction_id,
                            boost::optional<CiString20Type> parent_id_tag = boost::none);

    /// \brief Updates the given parameters for the transaction with the given \p session_id in the TRANSACTIONS table.
    void update_transaction(const std::string& session_id, int32_t meter_stop, const std::string& time_end,
                            boost::optional<CiString20Type> id_tag_end, boost::optional<Reason> stop_reason);

    /// \brief Returns a list of all transactions in the database. If \p filter_complete is true, only incomplete
    /// transactions will be return. If \p filter_complete is false, all transactions will be returned
    std::vector<TransactionEntry> get_transactions(bool filter_incomplete = false);

    // authorization cache
    /// \brief Inserts or updates an authorization cache entry to the AUTH_CACHE table.
    void insert_or_update_authorization_cache_entry(const CiString20Type& id_tag, const IdTagInfo& id_tag_info);

    /// \brief Returns the IdTagInfo of the given \p id_tag if it exists in the AUTH_CACHE table, else boost::none.
    boost::optional<IdTagInfo> get_authorization_cache_entry(const CiString20Type& id_tag);

    /// \brief Deletes all entries of the AUTH_CACHE table.
    bool clear_authorization_cache();

    // connector availability
    /// \brief Inserts or updates the given \p availability_type of the given \p connector to the CONNECTORS table.
    void insert_or_update_connector_availability(int32_t connector, const AvailabilityType& availability_type);

    /// \brief Inserts or updates the given \p availability_type of the given \p connectors to the CONNECTORS table.
    void insert_or_update_connector_availability(const std::vector<int32_t>& connectors,
                                                 const AvailabilityType& availability_type);

    /// \brief Returns the AvailabilityType of the given \p connector of the CONNECTORS table.
    AvailabilityType get_connector_availability(int32_t connector);

    /// \brief Returns a map of all connectors and its AvailabilityTypes of the CONNECTORS table.
    std::map<int32_t, AvailabilityType> get_connector_availability();

    // local auth list management

    /// \brief Inserts or updates the given \p version in the AUTH_LIST_VERSION table.
    void insert_or_update_local_list_version(int32_t version);

    /// \brief Returns the version in the AUTH_LIST_VERSION table.
    int32_t get_local_list_version();

    /// \brief Inserts or updates a local authorization list entry to the AUTH_LIST table.
    void insert_or_update_local_authorization_list_entry(const CiString20Type& id_tag, const IdTagInfo& id_tag_info);

    /// \brief Inserts or updates a local authorization list entries \p local_authorization_list to the AUTH_LIST table.
    void insert_or_update_local_authorization_list(std::vector<LocalAuthorizationList> local_authorization_list);

    /// \brief Deletes the authorization list entry with the given \p id_tag
    void delete_local_authorization_list_entry(const std::string& id_tag);

    /// \brief Returns the IdTagInfo of the given \p id_tag if it exists in the AUTH_LIST table, else boost::none.
    boost::optional<IdTagInfo> get_local_authorization_list_entry(const CiString20Type& id_tag);

    /// \brief Deletes all entries of the AUTH_LIST table.
    bool clear_local_authorization_list();

    /// \brief Inserts or updates the given \p profile to CHARGING_PROFILES table
    void insert_or_update_charging_profile(const int connector_id, const ChargingProfile& profile);

    /// \brief Deletes the profile with the given \p profile_id
    void delete_charging_profile(const int profile_id);

    /// \brief Deletes all profiles from table CHARGING_PROFILES
    void delete_charging_profiles();

    /// \brief Returns a list of all charging profiles in the CHARGING_PROFILES table
    std::vector<ChargingProfile> get_charging_profiles();

    /// \brief Returns the connector_id of the given \p profile_id
    int get_connector_id(const int profile_id);
};

} // namespace ocpp1_6

#endif // OCPP1_6_DATABASE_HANDLER_HPP
