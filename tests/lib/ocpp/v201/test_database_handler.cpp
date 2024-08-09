// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include "comparators.hpp"
#include "database_testing_utils.hpp"
#include "ocpp/v201/enums.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ocpp/v201/database_handler.hpp>

using namespace ocpp;
using namespace ocpp::v201;

class DatabaseHandlerTest : public DatabaseTestingUtils {
public:
    DatabaseHandler database_handler{std::make_unique<DatabaseConnection>("file::memory:?cache=shared"),
                                     std::filesystem::path(MIGRATION_FILES_LOCATION_V201)};

    DatabaseHandlerTest() {
        this->database_handler.open_connection();
    }

    std::unique_ptr<EnhancedTransaction> default_transaction() {
        auto transaction = std::make_unique<EnhancedTransaction>(this->database_handler, true);
        transaction->transactionId = "txId";
        transaction->connector_id = 1;
        transaction->start_time = DateTime{"2024-07-15T08:01:02Z"};
        transaction->seq_no = 10;
        transaction->chargingState = ChargingStateEnum::SuspendedEV;
        transaction->id_token_sent = true;

        return transaction;
    }
};

TEST_F(DatabaseHandlerTest, TransactionInsertAndGet) {
    constexpr int32_t evse_id = 1;

    auto transaction = default_transaction();

    this->database_handler.transaction_insert(*transaction, evse_id);

    auto transaction_get = this->database_handler.transaction_get(evse_id);

    EXPECT_NE(transaction_get, nullptr);
    EXPECT_EQ(transaction->transactionId, transaction_get->transactionId);
    EXPECT_EQ(transaction->connector_id, transaction_get->connector_id);
    EXPECT_EQ(transaction->start_time, transaction_get->start_time);
    EXPECT_EQ(transaction->seq_no, transaction_get->seq_no);
    EXPECT_EQ(transaction->chargingState, transaction_get->chargingState);
    EXPECT_EQ(transaction->id_token_sent, transaction_get->id_token_sent);
}

TEST_F(DatabaseHandlerTest, TransactionGetNotFound) {
    constexpr int32_t evse_id = 1;
    auto transaction_get = this->database_handler.transaction_get(evse_id);
    EXPECT_EQ(transaction_get, nullptr);
}

TEST_F(DatabaseHandlerTest, TransactionInsertDuplicateTransactionId) {
    constexpr int32_t evse_id = 1;

    auto transaction = default_transaction();

    this->database_handler.transaction_insert(*transaction, evse_id);

    EXPECT_THROW(this->database_handler.transaction_insert(*transaction, evse_id + 1), DatabaseException);
}

TEST_F(DatabaseHandlerTest, TransactionInsertDuplicateEvseId) {
    constexpr int32_t evse_id = 1;

    auto transaction = default_transaction();

    this->database_handler.transaction_insert(*transaction, evse_id);

    transaction->transactionId = "txId2";

    EXPECT_THROW(this->database_handler.transaction_insert(*transaction, evse_id), DatabaseException);
}

TEST_F(DatabaseHandlerTest, TransactionUpdateSeqNo) {
    constexpr int32_t evse_id = 1;
    constexpr int32_t new_seq_no = 20;

    auto transaction = default_transaction();

    transaction->seq_no = 10;

    this->database_handler.transaction_insert(*transaction, evse_id);
    this->database_handler.transaction_update_seq_no(transaction->transactionId, new_seq_no);

    auto transaction_get = this->database_handler.transaction_get(evse_id);

    EXPECT_NE(transaction_get, nullptr);
    EXPECT_EQ(transaction_get->seq_no, new_seq_no);
}

TEST_F(DatabaseHandlerTest, TransactionUpdateChargingState) {
    constexpr int32_t evse_id = 1;
    constexpr auto new_state = ChargingStateEnum::Charging;

    auto transaction = default_transaction();

    transaction->chargingState = ChargingStateEnum::SuspendedEV;

    this->database_handler.transaction_insert(*transaction, evse_id);
    this->database_handler.transaction_update_charging_state(transaction->transactionId, new_state);

    auto transaction_get = this->database_handler.transaction_get(evse_id);

    EXPECT_NE(transaction_get, nullptr);
    EXPECT_EQ(transaction_get->chargingState, new_state);
}

TEST_F(DatabaseHandlerTest, TransactionUpdateIdTokenSent) {
    constexpr int32_t evse_id = 1;
    constexpr bool new_state = true;

    auto transaction = default_transaction();

    transaction->id_token_sent = false;

    this->database_handler.transaction_insert(*transaction, evse_id);
    this->database_handler.transaction_update_id_token_sent(transaction->transactionId, new_state);

    auto transaction_get = this->database_handler.transaction_get(evse_id);

    EXPECT_NE(transaction_get, nullptr);
    EXPECT_EQ(transaction_get->id_token_sent, new_state);
}

TEST_F(DatabaseHandlerTest, TransactionDelete) {
    constexpr int32_t evse_id = 1;

    auto transaction = default_transaction();

    this->database_handler.transaction_insert(*transaction, evse_id);

    EXPECT_NE(this->database_handler.transaction_get(evse_id), nullptr);

    this->database_handler.transaction_delete(transaction->transactionId);

    EXPECT_EQ(this->database_handler.transaction_get(evse_id), nullptr);
}

TEST_F(DatabaseHandlerTest, TransactionDeleteNotFound) {
    EXPECT_NO_THROW(this->database_handler.transaction_delete("txIdNotFound"));
}

TEST_F(DatabaseHandlerTest, KO1_FR27_DatabaseWithNoData_InsertProfile) {
    this->database_handler.insert_or_update_charging_profile(
        1, ChargingProfile{
               .id = 1, .stackLevel = 1, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile});

    auto sut = this->database_handler.get_all_charging_profiles_group_by_evse();

    EXPECT_EQ(sut.size(), 1);
    EXPECT_EQ(sut[1].size(), 1);        // Access the profiles at EVSE_ID 1
    EXPECT_EQ(sut[1][0].id, 1);         // Access the profiles at EVSE_ID 1 and get the first profile
    EXPECT_EQ(sut[1][0].stackLevel, 1); // Access the profiles at EVSE_ID 1 and get the first profile
}

TEST_F(DatabaseHandlerTest, KO1_FR27_DatabaseWithProfileData_UpdateProfile) {
    this->database_handler.insert_or_update_charging_profile(
        1, ChargingProfile{
               .id = 2, .stackLevel = 1, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile});
    this->database_handler.insert_or_update_charging_profile(
        1, ChargingProfile{
               .id = 2, .stackLevel = 2, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile});

    std::string sql = "SELECT COUNT(*) FROM CHARGING_PROFILES";
    auto select_stmt = this->database->new_statement(sql);

    EXPECT_EQ(select_stmt->step(), SQLITE_ROW);

    auto count = select_stmt->column_int(0);
    EXPECT_EQ(count, 1);
}

TEST_F(DatabaseHandlerTest, KO1_FR27_DatabaseWithProfileData_InsertNewProfile) {
    this->database_handler.insert_or_update_charging_profile(
        1, ChargingProfile{
               .id = 1, .stackLevel = 1, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile});
    this->database_handler.insert_or_update_charging_profile(
        1, ChargingProfile{
               .id = 2, .stackLevel = 1, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile});

    std::string sql = "SELECT COUNT(*) FROM CHARGING_PROFILES";
    auto select_stmt = this->database->new_statement(sql);

    EXPECT_EQ(select_stmt->step(), SQLITE_ROW);

    auto count = select_stmt->column_int(0);
    EXPECT_EQ(count, 2);
}

TEST_F(DatabaseHandlerTest, KO1_FR27_DatabaseWithProfileData_DeleteRemovesSpecifiedProfiles) {
    this->database_handler.insert_or_update_charging_profile(
        1, ChargingProfile{
               .id = 1, .stackLevel = 1, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile});
    this->database_handler.insert_or_update_charging_profile(
        1, ChargingProfile{
               .id = 2, .stackLevel = 1, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile});

    auto sql = "SELECT COUNT(*) FROM CHARGING_PROFILES";

    auto select_stmt = this->database->new_statement(sql);

    EXPECT_NE(select_stmt->step(), SQLITE_DONE);
    auto count = select_stmt->column_int(0);
    EXPECT_EQ(count, 2);

    select_stmt->step();

    this->database_handler.delete_charging_profile(1);

    select_stmt->reset();

    EXPECT_NE(select_stmt->step(), SQLITE_DONE);
    count = select_stmt->column_int(0);
    EXPECT_EQ(count, 1);
    select_stmt->step();
}

TEST_F(DatabaseHandlerTest, KO1_FR27_DatabaseWithProfileData_DeleteAllRemovesAllProfiles) {
    this->database_handler.insert_or_update_charging_profile(
        1, ChargingProfile{
               .id = 1, .stackLevel = 1, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile});
    this->database_handler.insert_or_update_charging_profile(
        1, ChargingProfile{
               .id = 2, .stackLevel = 1, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile});

    auto sql = "SELECT COUNT(*) FROM CHARGING_PROFILES";

    auto select_stmt = this->database->new_statement(sql);

    EXPECT_NE(select_stmt->step(), SQLITE_DONE);
    auto count = select_stmt->column_int(0);
    EXPECT_EQ(count, 2);
    select_stmt->step();

    this->database_handler.clear_charging_profiles();
    select_stmt->reset();

    EXPECT_NE(select_stmt->step(), SQLITE_DONE);
    count = select_stmt->column_int(0);
    EXPECT_EQ(count, 0);
    select_stmt->step();
}

TEST_F(DatabaseHandlerTest, KO1_FR27_DatabaseWithNoProfileData_DeleteAllDoesNotFail) {

    auto sql = "SELECT COUNT(*) FROM CHARGING_PROFILES";

    auto select_stmt = this->database->new_statement(sql);

    EXPECT_NE(select_stmt->step(), SQLITE_DONE);
    auto count = select_stmt->column_int(0);
    EXPECT_EQ(count, 0);
    select_stmt->step();

    this->database_handler.clear_charging_profiles();
    select_stmt->reset();

    EXPECT_NE(select_stmt->step(), SQLITE_DONE);
    count = select_stmt->column_int(0);
    EXPECT_EQ(count, 0);
    select_stmt->step();
}

TEST_F(DatabaseHandlerTest, KO1_FR27_DatabaseWithSingleProfileData_LoadsChargingProfile) {
    auto profile = ChargingProfile{
        .id = 1, .stackLevel = 1, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile};
    this->database_handler.insert_or_update_charging_profile(1, profile);

    auto sut = this->database_handler.get_all_charging_profiles_group_by_evse();

    EXPECT_EQ(sut.size(), 1);

    // The evse id is found
    EXPECT_NE(sut.find(1), sut.end());

    auto profiles = sut[1];

    EXPECT_EQ(profiles.size(), 1);
    EXPECT_EQ(profile, profiles[0]);
}

TEST_F(DatabaseHandlerTest, KO1_FR27_DatabaseWithMultipleProfileSameEvse_LoadsChargingProfile) {
    auto p1 = ChargingProfile{
        .id = 1, .stackLevel = 1, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile};

    this->database_handler.insert_or_update_charging_profile(1, p1);

    auto p2 = ChargingProfile{
        .id = 2, .stackLevel = 2, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile};
    this->database_handler.insert_or_update_charging_profile(1, p2);

    auto p3 = ChargingProfile{
        .id = 3, .stackLevel = 3, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile};
    this->database_handler.insert_or_update_charging_profile(1, p3);

    auto sut = this->database_handler.get_all_charging_profiles_group_by_evse();

    EXPECT_EQ(sut.size(), 1);

    // The evse id is found
    EXPECT_NE(sut.find(1), sut.end());

    auto profiles = sut[1];

    EXPECT_EQ(profiles.size(), 3);
    EXPECT_EQ(profiles[0], p1);
    EXPECT_EQ(profiles[1], p2);
    EXPECT_EQ(profiles[2], p3);
}

TEST_F(DatabaseHandlerTest, KO1_FR27_DatabaseWithMultipleProfileDiffEvse_LoadsChargingProfile) {
    auto p1 = ChargingProfile{
        .id = 1, .stackLevel = 1, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile};
    this->database_handler.insert_or_update_charging_profile(1, p1);

    auto p2 =
        ChargingProfile{.id = 2, .stackLevel = 2, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile};
    this->database_handler.insert_or_update_charging_profile(1, p2);

    auto p3 = ChargingProfile{
        .id = 3, .stackLevel = 3, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile};
    this->database_handler.insert_or_update_charging_profile(2, p3);
    auto p4 =
        ChargingProfile{.id = 4, .stackLevel = 4, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile};
    this->database_handler.insert_or_update_charging_profile(2, p4);

    auto p5 = ChargingProfile{
        .id = 5, .stackLevel = 5, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile};
    this->database_handler.insert_or_update_charging_profile(3, p5);

    auto p6 =
        ChargingProfile{.id = 6, .stackLevel = 6, .chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile};
    this->database_handler.insert_or_update_charging_profile(3, p6);

    auto sut = this->database_handler.get_all_charging_profiles_group_by_evse();

    EXPECT_EQ(sut.size(), 3);

    EXPECT_NE(sut.find(1), sut.end());
    EXPECT_NE(sut.find(2), sut.end());
    EXPECT_NE(sut.find(3), sut.end());

    auto profiles1 = sut[1];
    auto profiles2 = sut[2];
    auto profiles3 = sut[3];

    EXPECT_EQ(profiles1.size(), 2);
    EXPECT_EQ(profiles1[0], p1);
    EXPECT_EQ(profiles1[1], p2);

    EXPECT_EQ(profiles2.size(), 2);
    EXPECT_EQ(profiles2[0], p3);
    EXPECT_EQ(profiles2[1], p4);

    EXPECT_EQ(profiles3.size(), 2);
    EXPECT_EQ(profiles3[0], p5);
    EXPECT_EQ(profiles3[1], p6);
}
