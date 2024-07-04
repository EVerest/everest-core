// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>
#include <ocpp/v16/database_handler.hpp>
#include <optional>
#include <thread>

namespace ocpp {
namespace v16 {

ChargingProfile get_sample_charging_profile() {
    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 10;
    period1.numberPhases.emplace(3);

    ChargingSchedulePeriod period2;
    period2.startPeriod = 30;
    period2.limit = 16;
    period2.numberPhases.emplace(3);

    std::vector<ChargingSchedulePeriod> periods;
    periods.push_back(period1);
    periods.push_back(period2);

    ChargingSchedule schedule;
    schedule.chargingRateUnit = ChargingRateUnit::A;
    schedule.chargingSchedulePeriod = periods;
    schedule.duration = 100;
    schedule.startSchedule.emplace(DateTime(date::utc_clock::now()));
    schedule.minChargingRate.emplace(6.4);

    DateTime valid_from = DateTime(date::utc_clock::now());
    DateTime valid_to = DateTime(valid_from.to_time_point() + std::chrono::hours(3600));

    ChargingProfile profile;
    profile.chargingProfileId = 1;
    profile.stackLevel = 2;
    profile.chargingProfilePurpose = ChargingProfilePurposeType::TxProfile;
    profile.chargingProfileKind = ChargingProfileKindType::Recurring;
    profile.recurrencyKind.emplace(RecurrencyKindType::Daily);
    profile.validFrom.emplace(valid_from);
    profile.validTo.emplace(valid_to);
    profile.chargingSchedule = schedule;

    return profile;
}

class DatabaseTest : public ::testing::Test {
public:
    DatabaseTest() {
        auto database_connection = std::make_unique<common::DatabaseConnection>("file::memory:?cache=shared");
        database_connection->open_connection(); // Open connection so memory stays shared
        this->db_handler = std::make_unique<DatabaseHandler>(std::move(database_connection),
                                                             std::filesystem::path(MIGRATION_FILES_LOCATION_V16), 2);
        this->db_handler->open_connection();
    }

    std::unique_ptr<DatabaseHandler> db_handler;
};

TEST_F(DatabaseTest, test_init_connector_table) {
    auto availability_type = this->db_handler->get_connector_availability(1);
    ASSERT_EQ(AvailabilityType::Operative, availability_type);

    auto availability_map = this->db_handler->get_connector_availability();
    ASSERT_EQ(AvailabilityType::Operative, availability_map.at(1));
    ASSERT_EQ(AvailabilityType::Operative, availability_map.at(2));
}

TEST_F(DatabaseTest, test_list_version) {

    int32_t list_version = this->db_handler->get_local_list_version();
    ASSERT_EQ(0, list_version);

    int32_t exp_list_version = 42;
    this->db_handler->insert_or_update_local_list_version(exp_list_version);
    list_version = this->db_handler->get_local_list_version();
    ASSERT_EQ(exp_list_version, list_version);

    exp_list_version = 17;
    this->db_handler->insert_or_update_local_list_version(exp_list_version);
    list_version = this->db_handler->get_local_list_version();
    ASSERT_EQ(exp_list_version, list_version);
}

TEST_F(DatabaseTest, test_local_authorization_list_entry_1) {

    const auto id_tag = CiString<20>("DEADBEEF");
    const auto unknown_id_tag = CiString<20>("BEEFBEEF");

    IdTagInfo exp_id_tag_info;
    exp_id_tag_info.status = AuthorizationStatus::Accepted;

    this->db_handler->insert_or_update_local_authorization_list_entry(id_tag, exp_id_tag_info);
    auto id_tag_info = this->db_handler->get_local_authorization_list_entry(id_tag);
    ASSERT_EQ(exp_id_tag_info.status, id_tag_info.value().status);

    id_tag_info = this->db_handler->get_local_authorization_list_entry(unknown_id_tag);
    ASSERT_EQ(std::nullopt, id_tag_info);
}

TEST_F(DatabaseTest, test_local_authorization_list_entry_2) {

    const auto id_tag = CiString<20>("DEADBEEF");
    const auto unknown_id_tag = CiString<20>("BEEFBEEF");
    const auto parent_id_tag = CiString<20>("PARENT");

    IdTagInfo exp_id_tag_info;
    exp_id_tag_info.status = AuthorizationStatus::Accepted;
    exp_id_tag_info.expiryDate.emplace(DateTime());
    exp_id_tag_info.parentIdTag = parent_id_tag;

    this->db_handler->insert_or_update_local_authorization_list_entry(id_tag, exp_id_tag_info);
    auto id_tag_info = this->db_handler->get_local_authorization_list_entry(id_tag);
    // expired because expiry date was set to now
    ASSERT_EQ(AuthorizationStatus::Expired, id_tag_info.value().status);
    ASSERT_EQ(exp_id_tag_info.parentIdTag.value().get(), parent_id_tag.get());
}

TEST_F(DatabaseTest, test_local_authorization_list) {

    std::vector<LocalAuthorizationList> local_authorization_list;

    const auto id_tag_1 = CiString<20>("DEADBEEF");
    const auto id_tag_2 = CiString<20>("BEEFBEEF");

    IdTagInfo id_tag_info;
    id_tag_info.status = AuthorizationStatus::Accepted;
    id_tag_info.expiryDate = DateTime(DateTime().to_time_point() + std::chrono::hours(24));

    // inserting id_tag_2 manually with id_tag_info
    this->db_handler->insert_or_update_local_authorization_list_entry(id_tag_2, id_tag_info);
    auto received_id_tag_info = this->db_handler->get_local_authorization_list_entry(id_tag_2);
    ASSERT_EQ(id_tag_info.status, received_id_tag_info.value().status);

    LocalAuthorizationList entry_1;
    entry_1.idTag = id_tag_1;
    entry_1.idTagInfo = id_tag_info;

    // idTagInfo of entry_2 is not set
    LocalAuthorizationList entry_2;
    entry_2.idTag = id_tag_2;

    local_authorization_list.push_back(entry_1);
    local_authorization_list.push_back(entry_2);

    this->db_handler->insert_or_update_local_authorization_list(local_authorization_list);

    received_id_tag_info = this->db_handler->get_local_authorization_list_entry(id_tag_1);
    ASSERT_EQ(id_tag_info.status, received_id_tag_info.value().status);

    // entry_2 had no idTagInfo so it is not set so it is deleted from the list
    received_id_tag_info = this->db_handler->get_local_authorization_list_entry(id_tag_2);
    ASSERT_EQ(std::nullopt, received_id_tag_info);
}

TEST_F(DatabaseTest, test_clear_authorization_list) {

    const auto id_tag = CiString<20>("DEADBEEF");
    const auto parent_id_tag = CiString<20>("PARENT");

    IdTagInfo exp_id_tag_info;
    exp_id_tag_info.status = AuthorizationStatus::Accepted;
    exp_id_tag_info.expiryDate.emplace(DateTime(DateTime().to_time_point() + std::chrono::hours(24)));
    exp_id_tag_info.parentIdTag = parent_id_tag;

    this->db_handler->insert_or_update_local_authorization_list_entry(id_tag, exp_id_tag_info);
    auto id_tag_info = this->db_handler->get_local_authorization_list_entry(id_tag);

    ASSERT_EQ(exp_id_tag_info.status, id_tag_info.value().status);
    ASSERT_EQ(exp_id_tag_info.parentIdTag.value().get(), parent_id_tag.get());

    this->db_handler->clear_local_authorization_list();

    id_tag_info = this->db_handler->get_local_authorization_list_entry(id_tag);
    ASSERT_EQ(std::nullopt, id_tag_info);
}

TEST_F(DatabaseTest, test_authorization_cache_entry) {

    const auto id_tag = CiString<20>("DEADBEEF");
    const auto unknown_id_tag = CiString<20>("BEEFBEEF");

    IdTagInfo exp_id_tag_info;
    exp_id_tag_info.status = AuthorizationStatus::Accepted;

    this->db_handler->insert_or_update_authorization_cache_entry(id_tag, exp_id_tag_info);
    auto id_tag_info = this->db_handler->get_authorization_cache_entry(id_tag);
    ASSERT_EQ(exp_id_tag_info.status, id_tag_info.value().status);

    id_tag_info = this->db_handler->get_authorization_cache_entry(unknown_id_tag);
    ASSERT_EQ(std::nullopt, id_tag_info);
}

TEST_F(DatabaseTest, test_authorization_cache_entry_2) {

    const auto id_tag = CiString<20>("DEADBEEF");
    const auto unknown_id_tag = CiString<20>("BEEFBEEF");
    const auto parent_id_tag = CiString<20>("PARENT");

    IdTagInfo exp_id_tag_info;
    exp_id_tag_info.status = AuthorizationStatus::Accepted;
    exp_id_tag_info.expiryDate.emplace(DateTime());
    exp_id_tag_info.parentIdTag = parent_id_tag;

    this->db_handler->insert_or_update_authorization_cache_entry(id_tag, exp_id_tag_info);
    auto id_tag_info = this->db_handler->get_authorization_cache_entry(id_tag);
    // expired because expiry date was set to now
    ASSERT_EQ(AuthorizationStatus::Expired, id_tag_info.value().status);
    ASSERT_EQ(exp_id_tag_info.parentIdTag.value().get(), parent_id_tag.get());
}

TEST_F(DatabaseTest, test_clear_authorization_cache) {

    const auto id_tag = CiString<20>("DEADBEEF");
    const auto parent_id_tag = CiString<20>("PARENT");

    IdTagInfo exp_id_tag_info;
    exp_id_tag_info.status = AuthorizationStatus::Accepted;
    exp_id_tag_info.expiryDate.emplace(DateTime(DateTime().to_time_point() + std::chrono::hours(24)));
    exp_id_tag_info.parentIdTag = parent_id_tag;

    this->db_handler->insert_or_update_authorization_cache_entry(id_tag, exp_id_tag_info);
    auto id_tag_info = this->db_handler->get_authorization_cache_entry(id_tag);

    ASSERT_EQ(exp_id_tag_info.status, id_tag_info.value().status);
    ASSERT_EQ(exp_id_tag_info.parentIdTag.value().get(), parent_id_tag.get());

    this->db_handler->clear_authorization_cache();

    id_tag_info = this->db_handler->get_authorization_cache_entry(id_tag);
    ASSERT_EQ(std::nullopt, id_tag_info);
}

TEST_F(DatabaseTest, test_connector_availability) {

    std::vector<int32_t> connectors;
    connectors.push_back(1);
    connectors.push_back(2);

    this->db_handler->insert_or_update_connector_availability(connectors, AvailabilityType::Inoperative);

    auto availability_type_1 = this->db_handler->get_connector_availability(1);
    auto availability_type_2 = this->db_handler->get_connector_availability(2);

    ASSERT_EQ(AvailabilityType::Inoperative, availability_type_1);
    ASSERT_EQ(AvailabilityType::Inoperative, availability_type_2);
}

TEST_F(DatabaseTest, test_insert_and_get_transaction) {

    std::optional<CiString<20>> id_tag;
    id_tag.emplace(CiString<20>("DEADBEEF"));

    this->db_handler->insert_transaction("id-42", -1, 1, "DEADBEEF", "2022-08-18T09:42:41", 42, false, 42, "xyz");
    this->db_handler->update_transaction("id-42", 42);
    this->db_handler->update_transaction("id-42", 5000, "2022-08-18T10:42:41", id_tag, Reason::EVDisconnected, "xyz");
    this->db_handler->update_transaction_csms_ack(42);

    this->db_handler->insert_transaction("id-43", -1, 1, "BEEFDEAD", "2022-08-18T09:42:41", 43, false, 43, "xyz");

    auto incomplete_transactions = this->db_handler->get_transactions(true);

    ASSERT_EQ(incomplete_transactions.size(), 1);
    auto transaction = incomplete_transactions.at(0);

    ASSERT_EQ(transaction.session_id, "id-43");
    ASSERT_EQ(transaction.transaction_id, -1);

    auto all_transactions = this->db_handler->get_transactions();
    ASSERT_EQ(all_transactions.size(), 2);
    transaction = all_transactions.at(0);
    ASSERT_EQ(transaction.id_tag_end.value(), "DEADBEEF");
    ASSERT_EQ(transaction.connector, 1);
    ASSERT_EQ(transaction.id_tag_start, "DEADBEEF");
    ASSERT_EQ(transaction.meter_start, 42);
    ASSERT_EQ(transaction.meter_stop.value(), 5000);
    ASSERT_EQ(transaction.stop_reason.value(), "EVDisconnected");
}

TEST_F(DatabaseTest, test_insert_and_get_transaction_without_id_tag) {

    std::optional<CiString<20>> id_tag;
    this->db_handler->insert_transaction("id-42", -1, 1, "DEADBEEF", "2022-08-18T09:42:41", 42, false, 42, "xyz");
    this->db_handler->update_transaction("id-42", 42);
    this->db_handler->update_transaction("id-42", 5000, "2022-08-18T10:42:41", id_tag, Reason::EVDisconnected, "xyz");
    this->db_handler->update_transaction_csms_ack(42);

    this->db_handler->insert_transaction("id-43", -1, 1, "BEEFDEAD", "2022-08-18T09:42:41", 43, false, 43, "xyz");

    auto incomplete_transactions = this->db_handler->get_transactions(true);

    ASSERT_EQ(incomplete_transactions.size(), 1);
    auto transaction = incomplete_transactions.at(0);

    ASSERT_EQ(transaction.session_id, "id-43");
    ASSERT_EQ(transaction.transaction_id, -1);

    auto all_transactions = this->db_handler->get_transactions();
    ASSERT_EQ(all_transactions.size(), 2);

    transaction = all_transactions.at(0);
    ASSERT_FALSE(transaction.id_tag_end);
}

TEST_F(DatabaseTest, test_insert_and_get_profiles) {
    // TODO enable again on fixing https://github.com/EVerest/libocpp/issues/384
    GTEST_SKIP() << "validFrom/validTo checks are failing. See https://github.com/EVerest/libocpp/issues/384";

    const auto profile = get_sample_charging_profile();

    this->db_handler->insert_or_update_charging_profile(1, profile);

    const auto profiles = this->db_handler->get_charging_profiles();

    ASSERT_EQ(profiles.size(), 1);
    const auto db_profile = profiles.at(0);

    ASSERT_EQ(db_profile.chargingProfileId, profile.chargingProfileId);
    ASSERT_EQ(db_profile.stackLevel, profile.stackLevel);
    ASSERT_EQ(db_profile.chargingProfilePurpose, profile.chargingProfilePurpose);
    ASSERT_EQ(db_profile.chargingProfileKind, profile.chargingProfileKind);
    ASSERT_EQ(db_profile.recurrencyKind.value(), profile.recurrencyKind.value());
    ASSERT_EQ(db_profile.validFrom.value().to_rfc3339(), profile.validFrom.value().to_rfc3339());
    ASSERT_EQ(db_profile.validTo.value().to_rfc3339(), profile.validTo.value().to_rfc3339());

    ASSERT_EQ(db_profile.chargingSchedule.chargingRateUnit, ChargingRateUnit::A);
    ASSERT_EQ(db_profile.chargingSchedule.duration, profile.chargingSchedule.duration);
    ASSERT_EQ(db_profile.chargingSchedule.startSchedule.value().to_rfc3339(),
              profile.chargingSchedule.startSchedule.value().to_rfc3339());
    ASSERT_EQ(db_profile.chargingSchedule.minChargingRate.value(), profile.chargingSchedule.minChargingRate.value());

    for (size_t i = 0; i < profile.chargingSchedule.chargingSchedulePeriod.size(); i++) {
        ASSERT_EQ(db_profile.chargingSchedule.chargingSchedulePeriod.at(i).startPeriod,
                  profile.chargingSchedule.chargingSchedulePeriod.at(i).startPeriod);
        ASSERT_EQ(db_profile.chargingSchedule.chargingSchedulePeriod.at(i).limit,
                  profile.chargingSchedule.chargingSchedulePeriod.at(i).limit);
        ASSERT_EQ(db_profile.chargingSchedule.chargingSchedulePeriod.at(i).numberPhases.value(),
                  profile.chargingSchedule.chargingSchedulePeriod.at(i).numberPhases.value());
    }
}

TEST_F(DatabaseTest, test_update_profile_same_profile_id) {
    const auto profile1 = get_sample_charging_profile();
    const auto profile2 = get_sample_charging_profile();

    this->db_handler->insert_or_update_charging_profile(1, profile1);
    this->db_handler->insert_or_update_charging_profile(2, profile2);

    const auto profiles = this->db_handler->get_charging_profiles();

    ASSERT_EQ(profiles.size(), 1);
}

TEST_F(DatabaseTest, test_delete_profile) {
    const auto profile1 = get_sample_charging_profile();
    auto profile2 = get_sample_charging_profile();

    profile2.chargingProfileId = 2;

    this->db_handler->insert_or_update_charging_profile(1, profile1);
    this->db_handler->insert_or_update_charging_profile(2, profile2);

    auto profiles = this->db_handler->get_charging_profiles();
    ASSERT_EQ(profiles.size(), 2);

    this->db_handler->delete_charging_profile(1);

    profiles = this->db_handler->get_charging_profiles();
    ASSERT_EQ(profiles.size(), 1);

    this->db_handler->delete_charging_profiles();

    profiles = this->db_handler->get_charging_profiles();
    ASSERT_EQ(profiles.size(), 0);
}

TEST_F(DatabaseTest, test_unknown_connector) {
    ASSERT_THROW(this->db_handler->get_connector_availability(5), ocpp::common::RequiredEntryNotFoundException);
    ASSERT_THROW(this->db_handler->get_connector_id(5), ocpp::common::RequiredEntryNotFoundException);

    auto database_connection = std::make_unique<common::DatabaseConnection>("file::memory:?cache=shared");
    database_connection->open_connection(); // Open connection so memory stays shared

    database_connection->execute_statement("DROP TABLE CHARGING_PROFILES");

    ASSERT_THROW(this->db_handler->get_charging_profiles(), ocpp::common::QueryExecutionException);
}

} // namespace v16
} // namespace ocpp
