// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include "gmock/gmock.h"

#include <ocpp/v2/database_handler.hpp>

namespace ocpp::v2 {
class DatabaseHandlerMock : public DatabaseHandlerInterface {
public:
    MOCK_METHOD(void, authorization_cache_insert_entry,
                (const std::string& id_token_hash, const IdTokenInfo& id_token_info));
    MOCK_METHOD(void, authorization_cache_update_last_used, (const std::string& id_token_hash));
    MOCK_METHOD(std::optional<AuthorizationCacheEntry>, authorization_cache_get_entry,
                (const std::string& id_token_hash));
    MOCK_METHOD(void, authorization_cache_delete_entry, (const std::string& id_token_hash));
    MOCK_METHOD(void, authorization_cache_delete_nr_of_oldest_entries, (size_t nr_to_remove));
    MOCK_METHOD(void, authorization_cache_delete_expired_entries,
                (std::optional<std::chrono::seconds> auth_cache_lifetime));
    MOCK_METHOD(void, authorization_cache_clear, ());
    MOCK_METHOD(size_t, authorization_cache_get_binary_size, ());
    MOCK_METHOD(void, insert_cs_availability, (OperationalStatusEnum operational_status, bool replace));
    MOCK_METHOD(OperationalStatusEnum, get_cs_availability, ());
    MOCK_METHOD(void, insert_evse_availability,
                (int32_t evse_id, OperationalStatusEnum operational_status, bool replace));
    MOCK_METHOD(OperationalStatusEnum, get_evse_availability, (int32_t evse_id));
    MOCK_METHOD(void, insert_connector_availability,
                (int32_t evse_id, int32_t connector_id, OperationalStatusEnum operational_status, bool replace),
                (override));
    MOCK_METHOD(OperationalStatusEnum, get_connector_availability, (int32_t evse_id, int32_t connector_id));
    MOCK_METHOD(void, insert_or_update_local_authorization_list_version, (int32_t version));
    MOCK_METHOD(int32_t, get_local_authorization_list_version, ());
    MOCK_METHOD(void, insert_or_update_local_authorization_list_entry,
                (const IdToken& id_token, const IdTokenInfo& id_token_info));
    MOCK_METHOD(void, insert_or_update_local_authorization_list,
                (const std::vector<AuthorizationData>& local_authorization_list));
    MOCK_METHOD(void, delete_local_authorization_list_entry, (const IdToken& id_token));
    MOCK_METHOD(std::optional<IdTokenInfo>, get_local_authorization_list_entry, (const IdToken& id_token));
    MOCK_METHOD(void, clear_local_authorization_list, ());
    MOCK_METHOD(int32_t, get_local_authorization_list_number_of_entries, ());
    MOCK_METHOD(void, transaction_metervalues_insert,
                (const std::string& transaction_id, const MeterValue& meter_value));
    MOCK_METHOD(std::vector<MeterValue>, transaction_metervalues_get_all, (const std::string& transaction_id),
                (override));
    MOCK_METHOD(void, transaction_metervalues_clear, (const std::string& transaction_id));
    MOCK_METHOD(void, transaction_insert, (const EnhancedTransaction& transaction, int32_t evse_id));
    MOCK_METHOD(std::unique_ptr<EnhancedTransaction>, transaction_get, (const int32_t evse_id));
    MOCK_METHOD(void, transaction_update_seq_no, (const std::string& transaction_id, int32_t seq_no));
    MOCK_METHOD(void, transaction_update_charging_state,
                (const std::string& transaction_id, const ChargingStateEnum charging_state));
    MOCK_METHOD(void, transaction_update_id_token_sent, (const std::string& transaction_id, bool id_token_sent),
                (override));
    MOCK_METHOD(void, transaction_delete, (const std::string& transaction_id));
    MOCK_METHOD(void, insert_or_update_charging_profile,
                (const int evse_id, const ChargingProfile& profile, const CiString<20> charging_limit_source),
                (override));
    MOCK_METHOD(bool, delete_charging_profile, (const int profile_id));
    MOCK_METHOD(void, delete_charging_profile_by_transaction_id, (const std::string& transaction_id));
    MOCK_METHOD(bool, clear_charging_profiles, ());
    MOCK_METHOD(bool, clear_charging_profiles_matching_criteria,
                (const std::optional<int32_t> profile_id, const std::optional<ClearChargingProfile>& criteria),
                (override));
    MOCK_METHOD(std::vector<ReportedChargingProfile>, get_charging_profiles_matching_criteria,
                (const std::optional<int32_t> evse_id, const ChargingProfileCriterion& criteria));
    MOCK_METHOD(std::vector<ChargingProfile>, get_charging_profiles_for_evse, (const int evse_id));
    MOCK_METHOD(std::vector<ChargingProfile>, get_all_charging_profiles, ());
    typedef std::map<int32_t, std::vector<ChargingProfile>> charging_profiles_grouped_by_evse;
    MOCK_METHOD(charging_profiles_grouped_by_evse, get_all_charging_profiles_group_by_evse, ());
    MOCK_METHOD(CiString<20>, get_charging_limit_source_for_profile, (const int profile_id));
    MOCK_METHOD(std::unique_ptr<common::SQLiteStatementInterface>, new_statement, (const std::string& sql));
};
} // namespace ocpp::v2
