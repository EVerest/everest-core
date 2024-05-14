// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_DATABASE_HANDLER_HPP
#define OCPP_V16_DATABASE_HANDLER_HPP

#include "sqlite3.h"
#include <fstream>
#include <iostream>

#include <ocpp/common/database/database_handler_common.hpp>
#include <ocpp/common/schemas.hpp>
#include <ocpp/common/support_older_cpp_versions.hpp>
#include <ocpp/common/types.hpp>
#include <ocpp/v16/ocpp_types.hpp>
#include <ocpp/v16/types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Struct that contains all attributes of a transaction entry in the database
struct TransactionEntry {
    std::string session_id;
    int32_t connector;
    std::string id_tag_start;
    std::string time_start;
    int32_t meter_start;
    int32_t transaction_id;
    bool csms_ack;
    int32_t meter_last;
    std::string meter_last_time;
    std::string last_update;
    std::string start_transaction_message_id;
    std::optional<std::string> stop_transaction_message_id;
    std::optional<int32_t> reservation_id = std::nullopt;
    std::optional<std::string> parent_id_tag = std::nullopt;
    std::optional<int32_t> meter_stop = std::nullopt;
    std::optional<std::string> time_end = std::nullopt;
    std::optional<std::string> id_tag_end = std::nullopt;
    std::optional<std::string> stop_reason = std::nullopt;
};

/// \brief This class handles the connection and operations of the SQLite database
class DatabaseHandler : public ocpp::common::DatabaseHandlerCommon {
private:
    const int32_t number_of_connectors;

    // Runs initialization script and initializes the CONNECTORS and AUTH_LIST_VERSION table.
    void init_sql() override;
    void init_connector_table();

public:
    DatabaseHandler(std::unique_ptr<common::DatabaseConnectionInterface> database,
                    const fs::path& sql_migration_files_path, int32_t number_of_connectors);

    // transactions
    /// \brief Inserts a transaction with the given parameter to the TRANSACTIONS table.
    void insert_transaction(const std::string& session_id, const int32_t transaction_id, const int32_t connector,
                            const std::string& id_tag_start, const std::string& time_start, const int32_t meter_start,
                            const bool csms_ack, const std::optional<int32_t> reservation_id,
                            const std::string& start_transaction_message_id);

    /// \brief Updates the given parameters for the transaction with the given \p session_id in the TRANSACTIONS table.
    void update_transaction(const std::string& session_id, int32_t transaction_id,
                            std::optional<CiString<20>> parent_id_tag = std::nullopt);

    /// \brief Updates the given parameters for the transaction with the given \p session_id in the TRANSACTIONS table.
    void update_transaction(const std::string& session_id, int32_t meter_stop, const std::string& time_end,
                            std::optional<CiString<20>> id_tag_end, std::optional<v16::Reason> stop_reason,
                            const std::string& stop_transaction_message_id);

    /// \brief Updates the CSMS_ACK column for the transaction with the given \p transaction_id in the TRANSACTIONS
    /// table
    void update_transaction_csms_ack(const int32_t transaction_id);

    /// \brief Updates the METER_LAST and METER_LAST_TIME column for the transaction with the given \p session_id in the
    /// TRANSACTIONS table
    void update_transaction_meter_value(const std::string& session_id, const int32_t value,
                                        const std::string& timestamp);

    /// \brief Returns a list of all transactions in the database. If \p filter_complete is true, only incomplete
    /// transactions will be return. If \p filter_complete is false, all transactions will be returned
    std::vector<TransactionEntry> get_transactions(bool filter_incomplete = false);

    // authorization cache
    /// \brief Inserts or updates an authorization cache entry to the AUTH_CACHE table.
    void insert_or_update_authorization_cache_entry(const CiString<20>& id_tag, const v16::IdTagInfo& id_tag_info);

    /// \brief Returns the IdTagInfo of the given \p id_tag if it exists in the AUTH_CACHE table, else std::nullopt.
    std::optional<v16::IdTagInfo> get_authorization_cache_entry(const CiString<20>& id_tag);

    /// \brief Deletes all entries of the AUTH_CACHE table.
    void clear_authorization_cache();

    // connector availability
    /// \brief Inserts or updates the given \p availability_type of the given \p connector to the CONNECTORS table.
    void insert_or_update_connector_availability(int32_t connector, const v16::AvailabilityType& availability_type);

    /// \brief Inserts or updates the given \p availability_type of the given \p connectors to the CONNECTORS table.
    void insert_or_update_connector_availability(const std::vector<int32_t>& connectors,
                                                 const v16::AvailabilityType& availability_type);

    /// \brief Returns the AvailabilityType of the given \p connector of the CONNECTORS table.
    v16::AvailabilityType get_connector_availability(int32_t connector);

    /// \brief Returns a map of all connectors and its AvailabilityTypes of the CONNECTORS table.
    std::map<int32_t, v16::AvailabilityType> get_connector_availability();

    // local auth list management

    /// \brief Inserts or ignores the given \p version in the AUTH_LIST_VERSION table.
    void insert_or_ignore_local_list_version(int32_t version);

    /// \brief Inserts or updates the given \p version in the AUTH_LIST_VERSION table.
    void insert_or_update_local_list_version(int32_t version);

    /// \brief Returns the version in the AUTH_LIST_VERSION table.
    int32_t get_local_list_version();

    /// \brief Inserts or updates a local authorization list entry to the AUTH_LIST table.
    void insert_or_update_local_authorization_list_entry(const CiString<20>& id_tag, const v16::IdTagInfo& id_tag_info);

    /// \brief Inserts or updates a local authorization list entries \p local_authorization_list to the AUTH_LIST table.
    void insert_or_update_local_authorization_list(std::vector<v16::LocalAuthorizationList> local_authorization_list);

    /// \brief Deletes the authorization list entry with the given \p id_tag
    void delete_local_authorization_list_entry(const std::string& id_tag);

    /// \brief Returns the IdTagInfo of the given \p id_tag if it exists in the AUTH_LIST table, else std::nullopt.
    std::optional<v16::IdTagInfo> get_local_authorization_list_entry(const CiString<20>& id_tag);

    /// \brief Deletes all entries of the AUTH_LIST table.
    void clear_local_authorization_list();

    /// \brief Inserts or updates the given \p profile to CHARGING_PROFILES table
    virtual void insert_or_update_charging_profile(const int connector_id, const v16::ChargingProfile& profile);

    /// \brief Deletes the profile with the given \p profile_id
    virtual void delete_charging_profile(const int profile_id);

    /// \brief Deletes all profiles from table CHARGING_PROFILES
    void delete_charging_profiles();

    /// \brief Returns a list of all charging profiles in the CHARGING_PROFILES table
    std::vector<v16::ChargingProfile> get_charging_profiles();

    /// \brief Returns the connector_id of the given \p profile_id
    int get_connector_id(const int profile_id);

    /// \brief Updates the ocsp entry in the OCSP_REQUEST table
    void insert_ocsp_update();

    /// \brief Gets the only entry in the OCSP_REQUEST table
    std::optional<DateTime> get_last_ocsp_update();
};

} // namespace v16
} // namespace ocpp

#endif // OCPP_COMMON_DATABASE_HANDLER_HPP
