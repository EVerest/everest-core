// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include "comparators.hpp"
#include "database_testing_utils.hpp"
#include "ocpp/v2/enums.hpp"
#include "ocpp/v2/messages/GetChargingProfiles.hpp"
#include "ocpp/v2/ocpp_enums.hpp"
#include "ocpp/v2/ocpp_types.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ocpp/v2/database_handler.hpp>
#include <optional>

using namespace ocpp;
using namespace ocpp::v2;

const int STATION_WIDE_ID = 0;
const int DEFAULT_EVSE_ID = 1;

class DatabaseHandlerTest : public DatabaseTestingUtils {
public:
    DatabaseHandler database_handler{std::make_unique<DatabaseConnection>("file::memory:?cache=shared"),
                                     std::filesystem::path(MIGRATION_FILES_LOCATION_V2)};

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

    std::string uuid() {
        std::stringstream s;
        s << uuid_generator();
        return s.str();
    }

    boost::uuids::random_generator uuid_generator = boost::uuids::random_generator();
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
    ChargingProfile profile;
    profile.id = 1;
    profile.stackLevel = 1;
    profile.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(1, profile);

    auto sut = this->database_handler.get_all_charging_profiles_group_by_evse();

    EXPECT_EQ(sut.size(), 1);
    EXPECT_EQ(sut[1].size(), 1);        // Access the profiles at EVSE_ID 1
    EXPECT_EQ(sut[1][0].id, 1);         // Access the profiles at EVSE_ID 1 and get the first profile
    EXPECT_EQ(sut[1][0].stackLevel, 1); // Access the profiles at EVSE_ID 1 and get the first profile
}

TEST_F(DatabaseHandlerTest, KO1_FR27_DatabaseWithProfileData_UpdateProfile) {
    ChargingProfile profile1;
    profile1.id = 2;
    profile1.stackLevel = 1;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(1, profile1);
    ChargingProfile profile2;
    profile2.id = 2;
    profile2.stackLevel = 2;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(1, profile2);

    std::string sql = "SELECT COUNT(*) FROM CHARGING_PROFILES";
    auto select_stmt = this->database->new_statement(sql);

    EXPECT_EQ(select_stmt->step(), SQLITE_ROW);

    auto count = select_stmt->column_int(0);
    EXPECT_EQ(count, 1);
}

TEST_F(DatabaseHandlerTest, KO1_FR27_DatabaseWithProfileData_InsertNewProfile) {
    ChargingProfile profile1;
    profile1.id = 1;
    profile1.stackLevel = 1;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(1, profile1);
    ChargingProfile profile2;
    profile2.id = 2;
    profile2.stackLevel = 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(1, profile2);

    std::string sql = "SELECT COUNT(*) FROM CHARGING_PROFILES";
    auto select_stmt = this->database->new_statement(sql);

    EXPECT_EQ(select_stmt->step(), SQLITE_ROW);

    auto count = select_stmt->column_int(0);
    EXPECT_EQ(count, 2);
}

TEST_F(DatabaseHandlerTest, KO1_FR27_DatabaseWithProfileData_DeleteRemovesSpecifiedProfiles) {
    ChargingProfile profile1;
    profile1.id = 1;
    profile1.stackLevel = 1;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(1, profile1);
    ChargingProfile profile2;
    profile2.id = 2;
    profile2.stackLevel = 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(1, profile2);

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
    ChargingProfile profile1;
    profile1.id = 1;
    profile1.stackLevel = 1;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(1, profile1);
    ChargingProfile profile2;
    profile2.id = 2;
    profile2.stackLevel = 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(1, profile2);

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
    ChargingProfile profile;
    profile.id = 1;
    profile.stackLevel = 1;
    profile.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile.chargingProfileKind = ChargingProfileKindEnum::Absolute;
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
    ChargingProfile p1;
    p1.id = 1;
    p1.stackLevel = 1;
    p1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    p1.chargingProfileKind = ChargingProfileKindEnum::Absolute;

    this->database_handler.insert_or_update_charging_profile(1, p1);

    ChargingProfile p2;
    p2.id = 2;
    p2.stackLevel = 2;
    p2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    p2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(1, p2);

    ChargingProfile p3;
    p3.id = 3;
    p3.stackLevel = 3;
    p3.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    p3.chargingProfileKind = ChargingProfileKindEnum::Absolute;
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
    ChargingProfile p1;
    p1.id = 1;
    p1.stackLevel = 1;
    p1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    p1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(1, p1);

    ChargingProfile p2;
    p2.id = 2;
    p2.stackLevel = 2;
    p2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    p2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(1, p2);

    ChargingProfile p3;
    p3.id = 3;
    p3.stackLevel = 3;
    p3.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    p3.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(2, p3);

    ChargingProfile p4;
    p4.id = 4;
    p4.stackLevel = 4;
    p4.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    p4.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(2, p4);

    ChargingProfile p5;
    p5.id = 5;
    p5.stackLevel = 5;
    p5.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    p5.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(3, p5);

    ChargingProfile p6;
    p6.id = 6;
    p6.stackLevel = 6;
    p6.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    p6.chargingProfileKind = ChargingProfileKindEnum::Absolute;
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

TEST_F(DatabaseHandlerTest, GetAllChargingProfiles_GetsAllProfiles) {
    ChargingProfile profile1;
    profile1.id = 1;
    profile1.stackLevel = 1;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;

    ChargingProfile profile2;
    profile2.id = 2;
    profile2.stackLevel = 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;

    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID + 1, profile2);

    auto profiles = this->database_handler.get_all_charging_profiles();

    EXPECT_EQ(profiles.size(), 2);
    EXPECT_THAT(profiles, testing::Contains(profile1));
    EXPECT_THAT(profiles, testing::Contains(profile2));
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesForEvse_GetsProfilesForEVSE) {
    ChargingProfile profile1;
    profile1.id = 1;
    profile1.stackLevel = 1;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;

    ChargingProfile profile2;
    profile2.id = 2;
    profile2.stackLevel = 2;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;

    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile2);

    auto profiles = this->database_handler.get_charging_profiles_for_evse(DEFAULT_EVSE_ID);

    EXPECT_EQ(profiles.size(), 2);
    EXPECT_THAT(profiles, testing::Contains(profile1));
    EXPECT_THAT(profiles, testing::Contains(profile2));
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesForEvse_DoesNotGetProfilesOnOtherEVSE) {
    ChargingProfile profile1;
    profile1.id = 1;
    profile1.stackLevel = 1;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;

    ChargingProfile profile2;
    profile2.id = 2;
    profile2.stackLevel = 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;

    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID + 1, profile2);

    auto profiles = this->database_handler.get_charging_profiles_for_evse(DEFAULT_EVSE_ID);

    EXPECT_EQ(profiles.size(), 1);
    EXPECT_THAT(profiles, testing::Contains(profile1));
    EXPECT_THAT(profiles, testing::Not(testing::Contains(profile2)));
}

TEST_F(DatabaseHandlerTest, DeleteChargingProfileByTransactionId_DeletesByTransactionId) {
    const auto profile_id = 1;
    const auto transaction_id = uuid();
    ChargingProfile profile1;
    profile1.id = profile_id;
    profile1.stackLevel = 1;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;

    ChargingProfile profile2;
    profile2.id = profile_id + 1;
    profile2.stackLevel = 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    profile2.transactionId = transaction_id;

    this->database_handler.insert_or_update_charging_profile(1, profile1);
    this->database_handler.insert_or_update_charging_profile(1, profile2);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_THAT(profiles, testing::SizeIs(2));
    EXPECT_THAT(profiles, testing::Contains(profile1));
    EXPECT_THAT(profiles, testing::Contains(profile2));

    this->database_handler.delete_charging_profile_by_transaction_id(transaction_id);

    auto sut = this->database_handler.get_all_charging_profiles();
    EXPECT_THAT(sut, testing::SizeIs(1));
    EXPECT_THAT(sut, testing::Contains(profile1));
    EXPECT_THAT(sut, testing::Not(testing::Contains(profile2)));
}

TEST_F(DatabaseHandlerTest, ClearChargingProfilesMatchingCriteria_WhenGivenProfileId_DeletesProfile) {
    const ClearChargingProfile clear_criteria;

    const auto profile_id = 1;
    ChargingProfile p1;
    p1.id = profile_id;
    p1.stackLevel = 1;
    p1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    p1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, p1);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);

    auto sut = this->database_handler.clear_charging_profiles_matching_criteria(profile_id, clear_criteria);
    EXPECT_TRUE(sut);

    profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 0);
}

TEST_F(DatabaseHandlerTest, ClearChargingProfilesMatchingCriteria_WhenNotGivenProfileId_DoesNotDeleteProfile) {
    const ClearChargingProfile clear_criteria;

    const auto profile_id = 1;
    ChargingProfile p1;
    p1.id = profile_id;
    p1.stackLevel = 1;
    p1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    p1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, p1);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);

    auto sut = this->database_handler.clear_charging_profiles_matching_criteria({}, clear_criteria);
    EXPECT_FALSE(sut);

    profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);
    EXPECT_THAT(profiles, testing::Contains(p1));
}

TEST_F(DatabaseHandlerTest, ClearChargingProfilesMatchingCriteria_WhenNotGivenCriteria_DeletesAllProfiles) {
    const auto profile_id = 1;
    ChargingProfile p1;
    p1.id = profile_id;
    p1.stackLevel = 1;
    p1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    p1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    ChargingProfile p2;
    p2.id = profile_id + 1;
    p2.stackLevel = 1;
    p2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    p2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, p1);
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID + 1, p2);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 2);

    auto sut = this->database_handler.clear_charging_profiles_matching_criteria({}, {});
    EXPECT_TRUE(sut);

    profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 0);
}

TEST_F(DatabaseHandlerTest, ClearChargingProfilesMatchingCriteria_WhenAllCriteriaMatch_DeletesProfile) {
    const auto purpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    const auto stack_level = 1;

    ClearChargingProfile clear_criteria;
    clear_criteria.evseId = DEFAULT_EVSE_ID;
    clear_criteria.chargingProfilePurpose = purpose;
    clear_criteria.stackLevel = stack_level;

    ChargingProfile p1;
    p1.id = 1;
    p1.stackLevel = stack_level;
    p1.chargingProfilePurpose = purpose;
    p1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, p1);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);

    auto sut = this->database_handler.clear_charging_profiles_matching_criteria({}, clear_criteria);
    EXPECT_TRUE(sut);

    profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 0);
}

TEST_F(DatabaseHandlerTest, ClearChargingProfilesMatchingCriteria_UnknownPurpose_DoesNotDeleteProfile) {
    const auto different_purpose = ChargingProfilePurposeEnum::TxProfile;
    const auto stack_level = 1;

    ClearChargingProfile clear_criteria;
    clear_criteria.evseId = DEFAULT_EVSE_ID;
    clear_criteria.chargingProfilePurpose = different_purpose;
    clear_criteria.stackLevel = stack_level;

    ChargingProfile p1;
    p1.id = 1;
    p1.stackLevel = stack_level;
    p1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    p1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, p1);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);
    EXPECT_THAT(profiles, testing::Contains(p1));

    auto sut = this->database_handler.clear_charging_profiles_matching_criteria({}, clear_criteria);
    EXPECT_FALSE(sut);

    profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);
    EXPECT_THAT(profiles, testing::Contains(p1));
}

TEST_F(DatabaseHandlerTest, ClearChargingProfilesMatchingCriteria_UnknownStackLevel_DoesNotDeleteProfile) {
    const auto purpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    const auto different_stack_level = 2;

    ClearChargingProfile clear_criteria;
    clear_criteria.evseId = DEFAULT_EVSE_ID;
    clear_criteria.chargingProfilePurpose = purpose;
    clear_criteria.stackLevel = different_stack_level;

    ChargingProfile p1;
    p1.id = 1;
    p1.stackLevel = 1;
    p1.chargingProfilePurpose = purpose;
    p1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, p1);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);
    EXPECT_THAT(profiles, testing::Contains(p1));

    auto sut = this->database_handler.clear_charging_profiles_matching_criteria({}, clear_criteria);
    EXPECT_FALSE(sut);

    profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);
    EXPECT_THAT(profiles, testing::Contains(p1));
}

TEST_F(DatabaseHandlerTest, ClearChargingProfilesMatchingCriteria_UnknownEvseId_DoesNotDeleteProfile) {
    const auto purpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    const auto stack_level = 1;
    const auto different_evse_id = DEFAULT_EVSE_ID + 1;

    ClearChargingProfile clear_criteria;
    clear_criteria.evseId = different_evse_id;
    clear_criteria.chargingProfilePurpose = purpose;
    clear_criteria.stackLevel = stack_level;

    ChargingProfile p1;
    p1.id = 1;
    p1.stackLevel = stack_level;
    p1.chargingProfilePurpose = purpose;
    p1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, p1);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);
    EXPECT_THAT(profiles, testing::Contains(p1));

    auto sut = this->database_handler.clear_charging_profiles_matching_criteria({}, clear_criteria);
    EXPECT_FALSE(sut);

    profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);
    EXPECT_THAT(profiles, testing::Contains(p1));
}

TEST_F(DatabaseHandlerTest, ClearChargingProfilesMatchingCriteria_DoesNotDeleteExternalConstraints) {
    const auto purpose = ChargingProfilePurposeEnum::ChargingStationExternalConstraints;
    const auto stack_level = 1;

    ClearChargingProfile clear_criteria;
    clear_criteria.evseId = STATION_WIDE_ID;
    clear_criteria.chargingProfilePurpose = purpose;
    clear_criteria.stackLevel = stack_level;

    ChargingProfile p1;
    p1.id = 1;
    p1.stackLevel = stack_level;
    p1.chargingProfilePurpose = purpose;
    p1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(STATION_WIDE_ID, p1);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);

    auto sut = this->database_handler.clear_charging_profiles_matching_criteria({}, clear_criteria);
    EXPECT_FALSE(sut);

    profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);
    EXPECT_THAT(profiles, testing::Contains(p1));
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_NoCriteriaReturnsAll) {
    ChargingProfile profile;
    profile.id = 1;
    profile.stackLevel = 1;
    profile.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(std::nullopt, {});
    EXPECT_EQ(sut.size(), 1);
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_NoMatchingIdsReturnsEmpty) {
    auto profile_id = 1;
    ChargingProfile profile;
    profile.id = profile_id;
    profile.stackLevel = 1;
    profile.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 1);

    ChargingProfileCriterion criteria;
    criteria.chargingProfileId = std::vector<int32_t>{2};

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(std::nullopt, criteria);

    EXPECT_EQ(sut.size(), 0);
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_MatchingIdReturnsSingleProfile) {
    auto profile_id_1 = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = 1;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;

    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = 2;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;

    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile2);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 2);

    ChargingProfileCriterion criteria;
    criteria.chargingProfileId = std::vector<int32_t>{profile_id_1};

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(std::nullopt, criteria);

    EXPECT_EQ(sut.size(), 1);
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_MatchingIdsReturnsMultipleProfiles) {
    auto profile_id_1 = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = 1;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = 2;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile2);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 2);

    ChargingProfileCriterion criteria;
    criteria.chargingProfileId = std::vector<int32_t>{profile_id_1, profile_id_2};

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(std::nullopt, criteria);

    EXPECT_EQ(sut.size(), 2);
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_MatchingChargingProfilePurposeReturnsProfiles) {
    auto profile_id_1 = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = 1;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = 2;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile2);

    auto profile_id_3 = 3;
    ChargingProfile profile3;
    profile3.id = profile_id_3;
    profile3.stackLevel = 2;
    profile3.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    profile3.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile3);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 3);

    ChargingProfileCriterion criteria;
    criteria.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(std::nullopt, criteria);

    EXPECT_EQ(sut.size(), 2);
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_MatchingStackLevelReturnsProfiles) {
    auto profile_id_1 = 1;
    auto stack_level = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = stack_level;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = stack_level + 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile2);

    auto profile_id_3 = 3;
    ChargingProfile profile3;
    profile3.id = profile_id_3;
    profile3.stackLevel = stack_level + 1;
    profile3.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    profile3.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile3);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 3);

    ChargingProfileCriterion criteria;
    criteria.stackLevel = 2;

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(std::nullopt, criteria);

    EXPECT_EQ(sut.size(), 2);
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_MatchingProfileSourceReturnsProfiles) {
    auto profile_id_1 = 1;
    auto stack_level = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = stack_level;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = stack_level + 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile2);

    auto profile_id_3 = 3;
    ChargingProfile profile3;
    profile3.id = profile_id_3;
    profile3.stackLevel = stack_level + 1;
    profile3.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    profile3.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile3,
                                                             ChargingLimitSourceEnumStringType::EMS);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 3);

    ChargingProfileCriterion criteria;
    criteria.chargingLimitSource = {{ChargingLimitSourceEnumStringType::CSO}};

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(std::nullopt, criteria);

    EXPECT_EQ(sut.size(), 2);
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_MatchingProfileSourcesReturnsProfiles) {
    auto profile_id_1 = 1;
    auto stack_level = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = stack_level;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = stack_level + 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile2);

    auto profile_id_3 = 3;
    ChargingProfile profile3;
    profile3.id = profile_id_3;
    profile3.stackLevel = stack_level + 1;
    profile3.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    profile3.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile3,
                                                             ChargingLimitSourceEnumStringType::EMS);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 3);

    ChargingProfileCriterion criteria;
    criteria.chargingLimitSource = {{ChargingLimitSourceEnumStringType::CSO, ChargingLimitSourceEnumStringType::EMS}};

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(std::nullopt, criteria);

    EXPECT_EQ(sut.size(), 3);
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_AllCriteriaSetReturnsOne) {
    auto profile_id_1 = 1;
    auto stack_level = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = stack_level;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = stack_level + 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile2);

    auto profile_id_3 = 3;
    ChargingProfile profile3;
    profile3.id = profile_id_3;
    profile3.stackLevel = stack_level + 1;
    profile3.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    profile3.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile3,
                                                             ChargingLimitSourceEnumStringType::CSO);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 3);

    ChargingProfileCriterion criteria;
    criteria.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    criteria.stackLevel = 2;
    criteria.chargingLimitSource = {{ChargingLimitSourceEnumStringType::CSO}};

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(std::nullopt, criteria);

    EXPECT_EQ(sut.size(), 1);
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_AllCriteriaSetReturnsNone) {
    auto profile_id_1 = 1;
    auto stack_level = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = stack_level;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = stack_level + 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile2);

    auto profile_id_3 = 3;
    ChargingProfile profile3;
    profile3.id = profile_id_3;
    profile3.stackLevel = stack_level + 1;
    profile3.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    profile3.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile3,
                                                             ChargingLimitSourceEnumStringType::CSO);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 3);

    ChargingProfileCriterion criteria;
    criteria.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    criteria.stackLevel = 2;
    criteria.chargingLimitSource = {{ChargingLimitSourceEnumStringType::EMS}};

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(std::nullopt, criteria);

    EXPECT_EQ(sut.size(), 0);
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_AllCriteriaSetReturnsNothing) {
    auto profile_id_1 = 1;
    auto stack_level = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = stack_level;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = stack_level + 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile2);

    auto profile_id_3 = 3;
    ChargingProfile profile3;
    profile3.id = profile_id_3;
    profile3.stackLevel = stack_level + 1;
    profile3.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    profile3.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile3,
                                                             ChargingLimitSourceEnumStringType::CSO);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 3);

    ChargingProfileCriterion criteria;
    criteria.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    criteria.stackLevel = 2;
    criteria.chargingLimitSource = {{ChargingLimitSourceEnumStringType::EMS}};

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(std::nullopt, criteria);

    EXPECT_EQ(sut.size(), 0);
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteriaAndEvseId0_AllCriteriaSetReturnsNone) {
    auto profile_id_1 = 1;
    auto stack_level = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = stack_level;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = stack_level + 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile2);

    auto profile_id_3 = 3;
    ChargingProfile profile3;
    profile3.id = profile_id_3;
    profile3.stackLevel = stack_level + 1;
    profile3.chargingProfilePurpose = ChargingProfilePurposeEnum::TxProfile;
    profile3.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile3,
                                                             ChargingLimitSourceEnumStringType::CSO);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 3);

    ChargingProfileCriterion criteria;
    criteria.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    criteria.stackLevel = 2;
    criteria.chargingLimitSource = {{ChargingLimitSourceEnumStringType::CSO}};

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(0, criteria);

    EXPECT_EQ(sut.size(), 0);
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_IfProfileIdAndEvseIdGiven_ReturnsMatchingProfile) {
    auto profile_id_1 = 1;
    auto stack_level = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = stack_level;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = stack_level + 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(STATION_WIDE_ID, profile2);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 2);

    ChargingProfileCriterion criteria;
    criteria.chargingProfileId = {{profile_id_1, profile_id_2}};

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(STATION_WIDE_ID, criteria);
    EXPECT_EQ(sut.size(), 1);

    EXPECT_THAT(sut[0].profile, testing::Eq(profile2));
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_MatchingProfileIds_ReturnsEVSEAndSource) {
    auto profile_id_1 = 1;
    auto stack_level = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = stack_level;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = stack_level + 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(STATION_WIDE_ID, profile2,
                                                             ChargingLimitSourceEnumStringType::EMS);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 2);

    ChargingProfileCriterion criteria;
    criteria.chargingProfileId = {{profile_id_1, profile_id_2}};

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria({}, criteria);
    EXPECT_EQ(sut.size(), 2);

    EXPECT_THAT(
        sut, testing::Contains(testing::FieldsAre(profile1, DEFAULT_EVSE_ID, ChargingLimitSourceEnumStringType::CSO)));
    EXPECT_THAT(
        sut, testing::Contains(testing::FieldsAre(profile2, STATION_WIDE_ID, ChargingLimitSourceEnumStringType::EMS)));
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_MatchingCriteria_ReturnsEVSEAndSource) {
    auto profile_id_1 = 1;
    auto stack_level = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = stack_level;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = stack_level + 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(STATION_WIDE_ID, profile2,
                                                             ChargingLimitSourceEnumStringType::EMS);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 2);

    ChargingProfileCriterion criteria;
    criteria.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria({}, criteria);
    EXPECT_EQ(sut.size(), 2);

    EXPECT_THAT(
        sut, testing::Contains(testing::FieldsAre(profile1, DEFAULT_EVSE_ID, ChargingLimitSourceEnumStringType::CSO)));
    EXPECT_THAT(
        sut, testing::Contains(testing::FieldsAre(profile2, STATION_WIDE_ID, ChargingLimitSourceEnumStringType::EMS)));
}

TEST_F(DatabaseHandlerTest, GetChargingProfilesMatchingCriteria_OnlyEVSEIDSet_ReturnsProfileOnEVSE) {
    auto profile_id_1 = 1;
    auto stack_level = 1;
    ChargingProfile profile1;
    profile1.id = profile_id_1;
    profile1.stackLevel = stack_level;
    profile1.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile1.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(DEFAULT_EVSE_ID, profile1);

    auto profile_id_2 = 2;
    ChargingProfile profile2;
    profile2.id = profile_id_2;
    profile2.stackLevel = stack_level + 1;
    profile2.chargingProfilePurpose = ChargingProfilePurposeEnum::TxDefaultProfile;
    profile2.chargingProfileKind = ChargingProfileKindEnum::Absolute;
    this->database_handler.insert_or_update_charging_profile(STATION_WIDE_ID, profile2,
                                                             ChargingLimitSourceEnumStringType::EMS);

    auto profiles = this->database_handler.get_all_charging_profiles();
    EXPECT_EQ(profiles.size(), 2);

    std::vector<ReportedChargingProfile> sut =
        this->database_handler.get_charging_profiles_matching_criteria(DEFAULT_EVSE_ID, {});
    EXPECT_EQ(sut.size(), 1);

    EXPECT_THAT(
        sut, testing::Contains(testing::FieldsAre(profile1, DEFAULT_EVSE_ID, ChargingLimitSourceEnumStringType::CSO)));
}
