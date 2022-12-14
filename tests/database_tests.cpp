// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <iostream>
#include <ocpp1_6/database_handler.hpp>
#include <boost/optional/optional_io.hpp>
#include <thread>

namespace ocpp1_6 {
class DatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        this->db_handler = std::make_unique<DatabaseHandler>(CP_ID, boost::filesystem::path("/tmp"),
                                                             boost::filesystem::path("../aux/init.sql"));
        this->db_handler->open_db_connection(2);
    }

    void TearDown() override {
        boost::filesystem::remove("/tmp/" + CP_ID + ".db");
    }

    std::unique_ptr<DatabaseHandler> db_handler;
    const std::string CP_ID = "cp001";
};

TEST_F(DatabaseTest, test_init_connector_table) {
    auto availability_type = this->db_handler->get_connector_availability(1);
    ASSERT_EQ(AvailabilityType::Operative, availability_type);

    auto availability_map = this->db_handler->get_connector_availability();
    ASSERT_EQ(AvailabilityType::Operative, availability_map.at(1));
    ASSERT_EQ(AvailabilityType::Operative, availability_map.at(2));
}

TEST_F(DatabaseTest, test_list_version) {
    int32_t exp_list_version = 42;
    this->db_handler->insert_or_update_local_list_version(exp_list_version);
    int32_t list_version = this->db_handler->get_local_list_version();
    ASSERT_EQ(exp_list_version, list_version);

    exp_list_version = 17;
    this->db_handler->insert_or_update_local_list_version(exp_list_version);
    list_version = this->db_handler->get_local_list_version();
    ASSERT_EQ(exp_list_version, list_version);
}

TEST_F(DatabaseTest, test_local_authorization_list_entry_1) {

    const auto id_tag = CiString20Type(std::string("DEADBEEF"));
    const auto unknown_id_tag = CiString20Type(std::string("BEEFBEEF"));

    IdTagInfo exp_id_tag_info;
    exp_id_tag_info.status = AuthorizationStatus::Accepted;

    this->db_handler->insert_or_update_local_authorization_list_entry(id_tag, exp_id_tag_info);
    auto id_tag_info = this->db_handler->get_local_authorization_list_entry(id_tag);
    ASSERT_EQ(exp_id_tag_info.status, id_tag_info.get().status);

    id_tag_info = this->db_handler->get_local_authorization_list_entry(unknown_id_tag);
    ASSERT_EQ(boost::none, id_tag_info);
}

TEST_F(DatabaseTest, test_local_authorization_list_entry_2) {

    const auto id_tag = CiString20Type(std::string("DEADBEEF"));
    const auto unknown_id_tag = CiString20Type(std::string("BEEFBEEF"));
    const auto parent_id_tag = CiString20Type(std::string("PARENT"));

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

    const auto id_tag_1 = CiString20Type(std::string("DEADBEEF"));
    const auto id_tag_2 = CiString20Type(std::string("BEEFBEEF"));

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
    ASSERT_EQ(boost::none, received_id_tag_info);
}

TEST_F(DatabaseTest, test_clear_authorization_list) {

    const auto id_tag = CiString20Type(std::string("DEADBEEF"));
    const auto parent_id_tag = CiString20Type(std::string("PARENT"));

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
    ASSERT_EQ(boost::none, id_tag_info);
}

TEST_F(DatabaseTest, test_authorization_cache_entry) {

    const auto id_tag = CiString20Type(std::string("DEADBEEF"));
    const auto unknown_id_tag = CiString20Type(std::string("BEEFBEEF"));

    IdTagInfo exp_id_tag_info;
    exp_id_tag_info.status = AuthorizationStatus::Accepted;

    this->db_handler->insert_or_update_authorization_cache_entry(id_tag, exp_id_tag_info);
    auto id_tag_info = this->db_handler->get_authorization_cache_entry(id_tag);
    ASSERT_EQ(exp_id_tag_info.status, id_tag_info.get().status);

    id_tag_info = this->db_handler->get_authorization_cache_entry(unknown_id_tag);
    ASSERT_EQ(boost::none, id_tag_info);
}

TEST_F(DatabaseTest, test_authorization_cache_entry_2) {

    const auto id_tag = CiString20Type(std::string("DEADBEEF"));
    const auto unknown_id_tag = CiString20Type(std::string("BEEFBEEF"));
    const auto parent_id_tag = CiString20Type(std::string("PARENT"));

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

    const auto id_tag = CiString20Type(std::string("DEADBEEF"));
    const auto parent_id_tag = CiString20Type(std::string("PARENT"));

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
    ASSERT_EQ(boost::none, id_tag_info);
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

    boost::optional<CiString20Type> id_tag;
    id_tag.emplace(CiString20Type(std::string("DEADBEEF")));

    this->db_handler->insert_transaction("id-42", -1, 1, "DEADBEEF", "2022-08-18T09:42:41", 42, 42);
    this->db_handler->update_transaction("id-42", 42);
    this->db_handler->update_transaction("id-42", 5000, "2022-08-18T10:42:41", id_tag,
                                         Reason::EVDisconnected);
    
    this->db_handler->insert_transaction("id-43", -1, 1, "BEEFDEAD", "2022-08-18T09:42:41", 43, 43);
    

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

    boost::optional<ocpp1_6::CiString20Type> id_tag;
    this->db_handler->insert_transaction("id-42", -1, 1, "DEADBEEF", "2022-08-18T09:42:41", 42, 42);
    this->db_handler->update_transaction("id-42", 42);
    this->db_handler->update_transaction("id-42", 5000, "2022-08-18T10:42:41", id_tag,
                                         Reason::EVDisconnected);
    
    this->db_handler->insert_transaction("id-43", -1, 1, "BEEFDEAD", "2022-08-18T09:42:41", 43, 43);
    
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

} // namespace ocpp1_6

