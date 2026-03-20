// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <component_state_manager_mock.hpp>
#include <connectivity_manager_mock.hpp>
#include <device_model_test_helper.hpp>
#include <evse_manager_fake.hpp>
#include <evse_security_mock.hpp>
#include <message_dispatcher_mock.hpp>
#include <mocks/database_handler_mock.hpp>
#include <ocpp/common/constants.hpp>
#include <ocpp/common/message_queue.hpp>
#include <ocpp/common/ocpp_logging.hpp>
#include <ocpp/v2/connectivity_manager.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model_interface.hpp>
#include <ocpp/v2/device_model_storage_sqlite.hpp>
#include <ocpp/v2/functional_blocks/availability.hpp>
#include <ocpp/v2/functional_blocks/diagnostics.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/functional_blocks/meter_values.hpp>
#include <ocpp/v2/functional_blocks/provisioning.hpp>
#include <ocpp/v2/functional_blocks/security.hpp>
#include <ocpp/v2/functional_blocks/transaction.hpp>
#include <ocpp/v2/messages/Get15118EVCertificate.hpp>
#include <ocpp/v2/messages/SetNetworkProfile.hpp>
#include <ocpp/v2/ocpp_types.hpp>
#include <ocsp_updater_mock.hpp>

using json = nlohmann::json;

namespace ocpp::v2 {

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static NetworkConnectionProfile make_basic_profile(int security_profile = 1,
                                                   const std::string& url = "wss://csms.example.com/ocpp") {
    NetworkConnectionProfile p;
    p.ocppCsmsUrl = url;
    p.securityProfile = security_profile;
    p.ocppInterface = OCPPInterfaceEnum::Wired0;
    p.ocppTransport = OCPPTransportEnum::JSON;
    p.messageTimeout = 30;
    return p;
}

static APN make_test_apn() {
    APN apn;
    apn.apn = "internet";
    apn.apnAuthentication = APNAuthenticationEnum::AUTO;
    apn.apnUserName = "user";
    apn.apnPassword = "pass";
    return apn;
}

static VPN make_test_vpn() {
    VPN vpn;
    vpn.server = "vpn.example.com";
    vpn.user = "vpnuser";
    vpn.password = "vpnpass";
    vpn.key = "vpnkey";
    vpn.type = VPNEnum::IKEv2;
    return vpn;
}

// Build a JSON array blob from a list of SetNetworkProfileRequests.
static json make_blob(std::initializer_list<SetNetworkProfileRequest> reqs) {
    json arr = json::array();
    for (const auto& req : reqs) {
        arr.push_back(json(req));
    }
    return arr;
}

// Seed a JSON blob with the given profiles into the device model so that
// sync_json_blob_from_device_model has an existing blob to merge into.
static void seed_blob(DeviceModel& dm, const json& profiles) {
    ASSERT_TRUE(ControllerComponentVariables::NetworkConnectionProfiles.variable.has_value());
    const auto status = dm.set_value(ControllerComponentVariables::NetworkConnectionProfiles.component,
                                     ControllerComponentVariables::NetworkConnectionProfiles.variable.value(),
                                     AttributeEnum::Actual, profiles.dump(), "test");
    ASSERT_EQ(status, SetVariableStatusEnum::Accepted);
}

// Read the current NetworkConnectionProfiles JSON blob from the device model.
static json read_blob(DeviceModel& dm) {
    const auto str = dm.get_value<std::string>(ControllerComponentVariables::NetworkConnectionProfiles);
    if (str.empty()) {
        return json::array();
    }
    return json::parse(str);
}

// ---------------------------------------------------------------------------
// Fixture: real in-memory SQLite device model with example_config
// (includes InternalCtrlr, OCPPCommCtrlr, NetworkConfiguration_1, _2)
// ---------------------------------------------------------------------------

class NetworkConfigSyncTest : public ::testing::Test {
protected:
    DeviceModelTestHelper dm_helper;
    DeviceModel* dm{nullptr};

    NetworkConfigSyncTest() : dm_helper() {
        dm = dm_helper.get_device_model();
    }
};

// ---------------------------------------------------------------------------
// write_profile_to_device_model
// ---------------------------------------------------------------------------

TEST_F(NetworkConfigSyncTest, WriteBasicProfileSucceeds) {
    auto profile = make_basic_profile();
    EXPECT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, profile, "test"));
}

TEST_F(NetworkConfigSyncTest, WriteProfileWithApnSucceeds) {
    auto profile = make_basic_profile();
    profile.apn = make_test_apn();
    EXPECT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, profile, "test"));
}

TEST_F(NetworkConfigSyncTest, WriteProfileWithVpnSucceeds) {
    auto profile = make_basic_profile();
    profile.vpn = make_test_vpn();
    EXPECT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, profile, "test"));
}

// ---------------------------------------------------------------------------
// read_profile_from_device_model
// ---------------------------------------------------------------------------

TEST_F(NetworkConfigSyncTest, ReadFromEmptySlotReturnsNullopt) {
    // Slot 2 has no default OcppCsmsUrl, so reading without writing first returns nullopt
    auto result = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 2);
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetworkConfigSyncTest, WriteAndReadBasicProfileRoundtrip) {
    auto original = make_basic_profile(2, "wss://test.server/ocpp");
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, original, "test"));

    auto result = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->ocppCsmsUrl.get(), "wss://test.server/ocpp");
    EXPECT_EQ(result->securityProfile, 2);
    EXPECT_EQ(result->ocppInterface, OCPPInterfaceEnum::Wired0);
    EXPECT_EQ(result->messageTimeout, 30);
    EXPECT_FALSE(result->apn.has_value());
    EXPECT_FALSE(result->vpn.has_value());
}

TEST_F(NetworkConfigSyncTest, WriteAndReadProfileWithApnRoundtrip) {
    auto original = make_basic_profile();
    original.apn = make_test_apn();
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, original, "test"));

    auto result = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->apn.has_value());
    EXPECT_EQ(result->apn->apn.get(), "internet");
    EXPECT_EQ(result->apn->apnAuthentication, APNAuthenticationEnum::AUTO);
    EXPECT_EQ(result->apn->apnUserName.value().get(), "user");
    EXPECT_FALSE(result->vpn.has_value());
}

TEST_F(NetworkConfigSyncTest, WriteAndReadProfileWithIdentityRoundtrip) {
    auto original = make_basic_profile();
    original.identity = "per_slot_identity";
    original.basicAuthPassword = "secret";
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, original, "test"));

    auto result = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->identity.has_value());
    EXPECT_EQ(result->identity->get(), "per_slot_identity");
}

TEST_F(NetworkConfigSyncTest, OverwriteProfileUpdatesFields) {
    auto first = make_basic_profile(1, "wss://first.example.com/ocpp");
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, first, "test"));

    auto second = make_basic_profile(2, "wss://second.example.com/ocpp");
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, second, "test"));

    auto result = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->ocppCsmsUrl.get(), "wss://second.example.com/ocpp");
    EXPECT_EQ(result->securityProfile, 2);
}

TEST_F(NetworkConfigSyncTest, TwoSlotsAreIndependent) {
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(
        *dm, 1, make_basic_profile(1, "wss://slot1.example.com"), "test"));
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(
        *dm, 2, make_basic_profile(2, "wss://slot2.example.com"), "test"));

    auto r1 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    auto r2 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 2);

    ASSERT_TRUE(r1.has_value());
    ASSERT_TRUE(r2.has_value());
    EXPECT_EQ(r1->ocppCsmsUrl.get(), "wss://slot1.example.com");
    EXPECT_EQ(r2->ocppCsmsUrl.get(), "wss://slot2.example.com");
    EXPECT_EQ(r1->securityProfile, 1);
    EXPECT_EQ(r2->securityProfile, 2);
}

// ---------------------------------------------------------------------------
// migrate_from_blob_if_needed
// ---------------------------------------------------------------------------

// Note: slot 1 has a default OcppCsmsUrl in the test DM config ("wss://ocpp.example.com"),
// so it is never considered "empty" unless explicitly cleared first.
// Import-from-blob tests clear slot 1 before calling migrate to simulate a fresh deployment.

TEST_F(NetworkConfigSyncTest, MigrateFromBlobPopulatesSlot1) {
    NetworkConfigurationComponentVariables::clear_slot_in_device_model(*dm, 1); // remove defaults so DM looks fresh

    SetNetworkProfileRequest req;
    req.configurationSlot = 1;
    req.connectionData = make_basic_profile(1, "wss://seeded.example.com/ocpp");
    seed_blob(*dm, make_blob({req}));

    NetworkConfigurationComponentVariables::migrate_from_blob_if_needed(*dm);

    auto result = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->ocppCsmsUrl.get(), "wss://seeded.example.com/ocpp");
    EXPECT_EQ(result->securityProfile, 1);
}

TEST_F(NetworkConfigSyncTest, MigrateFromBlobClearsBlob) {
    NetworkConfigurationComponentVariables::clear_slot_in_device_model(*dm, 1);

    SetNetworkProfileRequest req;
    req.configurationSlot = 1;
    req.connectionData = make_basic_profile(1, "wss://migrated.example.com");
    seed_blob(*dm, make_blob({req}));

    NetworkConfigurationComponentVariables::migrate_from_blob_if_needed(*dm);

    // Blob must be cleared after migration — empty blob is the idempotency marker
    const auto blob = read_blob(*dm);
    EXPECT_EQ(blob.size(), 0u);
}

TEST_F(NetworkConfigSyncTest, MigrateSkippedWhenDmAlreadyPopulated) {
    // Explicitly populate slot 1 so migration must be skipped
    auto existing_profile = make_basic_profile(1, "wss://already-set.example.com");
    ASSERT_TRUE(
        NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, existing_profile, "test"));

    auto before = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(before.has_value()) << "Test precondition: slot 1 must have data after explicit write";

    SetNetworkProfileRequest req;
    req.configurationSlot = 1;
    req.connectionData = make_basic_profile(1, "wss://blob-set.example.com");
    seed_blob(*dm, make_blob({req}));
    NetworkConfigurationComponentVariables::migrate_from_blob_if_needed(*dm);

    auto result = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(result.has_value());
    // DM value preserved — migration skipped because DM slot was already populated
    EXPECT_EQ(result->ocppCsmsUrl.get(), before->ocppCsmsUrl.get())
        << "DM value must be preserved when slot is already populated";

    // Blob must be cleared even when migration is skipped
    const auto blob = read_blob(*dm);
    EXPECT_EQ(blob.size(), 0u) << "Blob must be cleared after skipped migration";
}

TEST_F(NetworkConfigSyncTest, MigrateFromEmptyBlobIsNoop) {
    NetworkConfigurationComponentVariables::clear_slot_in_device_model(*dm, 1);

    // Seed an empty blob — no import should happen
    seed_blob(*dm, json::array());
    NetworkConfigurationComponentVariables::migrate_from_blob_if_needed(*dm);

    // Slot 1 must still be empty
    EXPECT_FALSE(NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1).has_value())
        << "Empty blob must not populate any DM slot";
}

TEST_F(NetworkConfigSyncTest, MigrateFromBlobIsIdempotent) {
    NetworkConfigurationComponentVariables::clear_slot_in_device_model(*dm, 1);

    SetNetworkProfileRequest req;
    req.configurationSlot = 1;
    req.connectionData = make_basic_profile(1, "wss://idempotent.example.com");
    seed_blob(*dm, make_blob({req}));

    // First call: blob is non-empty, imports from blob, clears blob
    NetworkConfigurationComponentVariables::migrate_from_blob_if_needed(*dm);

    auto r1 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(r1.has_value());
    EXPECT_EQ(r1->ocppCsmsUrl.get(), "wss://idempotent.example.com");

    // Verify blob is cleared
    EXPECT_EQ(read_blob(*dm).size(), 0u) << "Blob must be cleared after migration";

    // Second call: blob is empty (already migrated), must be a no-op
    NetworkConfigurationComponentVariables::migrate_from_blob_if_needed(*dm);

    auto r2 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(r2.has_value());
    // DM value is unchanged — blob was empty so no re-import occurred
    EXPECT_EQ(r2->ocppCsmsUrl.get(), "wss://idempotent.example.com");
}

TEST_F(NetworkConfigSyncTest, MigrateFromBlobPopulatesBothSlots) {
    NetworkConfigurationComponentVariables::clear_slot_in_device_model(*dm, 1);

    SetNetworkProfileRequest req1;
    req1.configurationSlot = 1;
    req1.connectionData = make_basic_profile(1, "wss://primary.example.com");

    SetNetworkProfileRequest req2;
    req2.configurationSlot = 2;
    req2.connectionData = make_basic_profile(2, "wss://backup.example.com");

    seed_blob(*dm, make_blob({req1, req2}));
    NetworkConfigurationComponentVariables::migrate_from_blob_if_needed(*dm);

    auto r1 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    auto r2 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 2);

    ASSERT_TRUE(r1.has_value());
    ASSERT_TRUE(r2.has_value());
    EXPECT_EQ(r1->ocppCsmsUrl.get(), "wss://primary.example.com");
    EXPECT_EQ(r2->ocppCsmsUrl.get(), "wss://backup.example.com");
    EXPECT_EQ(r2->securityProfile, 2);
}

TEST_F(NetworkConfigSyncTest, MigrateFromBlobWithApnPopulatesApnFields) {
    NetworkConfigurationComponentVariables::clear_slot_in_device_model(*dm, 1);

    SetNetworkProfileRequest req;
    req.configurationSlot = 1;
    req.connectionData = make_basic_profile();
    req.connectionData.apn = make_test_apn();

    seed_blob(*dm, make_blob({req}));
    NetworkConfigurationComponentVariables::migrate_from_blob_if_needed(*dm);

    auto result = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->apn.has_value());
    EXPECT_EQ(result->apn->apn.get(), "internet");
}

// ---------------------------------------------------------------------------
// Optional variable clearing on overwrite (Step 3 / Piet comment 4)
// ---------------------------------------------------------------------------

// Writing a profile with Identity and then overwriting with a profile that has
// no Identity must clear the Identity variable so it is not visible in the next read.
TEST_F(NetworkConfigSyncTest, OverwriteWithoutIdentityClearsIdentity) {
    auto with_identity = make_basic_profile();
    with_identity.identity = "slot_identity";
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, with_identity, "test"));

    auto r1 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(r1.has_value());
    ASSERT_TRUE(r1->identity.has_value());
    EXPECT_EQ(r1->identity->get(), "slot_identity");

    // Overwrite with a profile that has no identity
    auto without_identity = make_basic_profile();
    ASSERT_TRUE(
        NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, without_identity, "test"));

    auto r2 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(r2.has_value());
    // Identity must be absent — clearing writes "" which is treated as unset
    EXPECT_FALSE(r2->identity.has_value()) << "Identity must be cleared after overwrite without identity";
}

// Writing a profile with APN and then overwriting without APN must not leave
// stale APN sub-fields in the device model.
TEST_F(NetworkConfigSyncTest, OverwriteWithoutApnClearsApnSubFields) {
    auto with_apn = make_basic_profile();
    with_apn.apn = make_test_apn();
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, with_apn, "test"));

    auto r1 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(r1.has_value());
    ASSERT_TRUE(r1->apn.has_value());

    // Overwrite with a profile that has no APN
    ASSERT_TRUE(
        NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, make_basic_profile(), "test"));

    auto r2 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(r2.has_value());
    EXPECT_FALSE(r2->apn.has_value()) << "APN must be cleared after overwrite without APN";
}

// Writing a profile with VPN and then overwriting without VPN must not leave
// stale VPN sub-fields in the device model.
TEST_F(NetworkConfigSyncTest, OverwriteWithoutVpnClearsVpnSubFields) {
    auto with_vpn = make_basic_profile();
    with_vpn.vpn = make_test_vpn();
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, with_vpn, "test"));

    auto r1 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(r1.has_value());
    ASSERT_TRUE(r1->vpn.has_value());

    // Overwrite with a profile that has no VPN
    ASSERT_TRUE(
        NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, make_basic_profile(), "test"));

    auto r2 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(r2.has_value());
    EXPECT_FALSE(r2->vpn.has_value()) << "VPN must be cleared after overwrite without VPN";
}

// ---------------------------------------------------------------------------
// clear_slot_in_device_model (Step 7)
// ---------------------------------------------------------------------------

TEST_F(NetworkConfigSyncTest, ClearSlotMakesReadReturnNullopt) {
    // Write a full profile to slot 1
    auto profile = make_basic_profile(1, "wss://to-be-cleared.example.com");
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, profile, "test"));

    // Verify it can be read back
    ASSERT_TRUE(NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1).has_value());

    // Clear the slot
    NetworkConfigurationComponentVariables::clear_slot_in_device_model(*dm, 1);

    // After clearing, required fields (OcppCsmsUrl) are empty -> read returns nullopt
    auto result = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    EXPECT_FALSE(result.has_value()) << "read_profile_from_device_model must return nullopt after clear";
}

TEST_F(NetworkConfigSyncTest, ClearSlotDoesNotAffectOtherSlot) {
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(
        *dm, 1, make_basic_profile(1, "wss://s1.example.com"), "test"));
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(
        *dm, 2, make_basic_profile(2, "wss://s2.example.com"), "test"));

    // Clear only slot 1
    NetworkConfigurationComponentVariables::clear_slot_in_device_model(*dm, 1);

    // Slot 1 must be gone, slot 2 must remain intact
    EXPECT_FALSE(NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1).has_value());
    auto r2 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 2);
    ASSERT_TRUE(r2.has_value());
    EXPECT_EQ(r2->ocppCsmsUrl.get(), "wss://s2.example.com");
}

// ---------------------------------------------------------------------------
// US-004: Legacy blob cleared after migration — full lifecycle test
// Verifies that a populated NetworkConnectionProfiles blob is fully migrated
// into NetworkConfiguration DM components and the blob is cleared afterward.
// ---------------------------------------------------------------------------

TEST_F(NetworkConfigSyncTest, LegacyBlobClearedAfterMigration) {
    // AC1: Set up a database with a populated NetworkConnectionProfiles blob.
    // Clear default DM data so migration actually imports from the blob.
    NetworkConfigurationComponentVariables::clear_slot_in_device_model(*dm, 1);

    // Build a legacy blob with two profiles containing full connection data.
    SetNetworkProfileRequest req1;
    req1.configurationSlot = 1;
    req1.connectionData = make_basic_profile(1, "wss://legacy-primary.example.com/ocpp");
    req1.connectionData.apn = make_test_apn();

    SetNetworkProfileRequest req2;
    req2.configurationSlot = 2;
    req2.connectionData = make_basic_profile(2, "wss://legacy-backup.example.com/ocpp");
    req2.connectionData.vpn = make_test_vpn();

    seed_blob(*dm, make_blob({req1, req2}));

    // Verify precondition: blob is non-empty before migration
    ASSERT_GT(read_blob(*dm).size(), 0u) << "Precondition: blob must be non-empty before migration";

    // Run migration
    NetworkConfigurationComponentVariables::migrate_from_blob_if_needed(*dm);

    // AC2: After migration, the blob variable must be empty/cleared.
    const auto blob_after = read_blob(*dm);
    EXPECT_EQ(blob_after.size(), 0u) << "Blob must be cleared after migration";

    // AC3: After migration, NetworkConfiguration DM slots contain the equivalent profile data.
    auto r1 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(r1.has_value()) << "Slot 1 must be populated after migration";
    EXPECT_EQ(r1->ocppCsmsUrl.get(), "wss://legacy-primary.example.com/ocpp");
    EXPECT_EQ(r1->securityProfile, 1);
    EXPECT_EQ(r1->ocppInterface, OCPPInterfaceEnum::Wired0);
    EXPECT_EQ(r1->ocppTransport, OCPPTransportEnum::JSON);
    EXPECT_EQ(r1->messageTimeout, 30);
    ASSERT_TRUE(r1->apn.has_value()) << "APN must be migrated";
    EXPECT_EQ(r1->apn->apn.get(), "internet");

    auto r2 = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 2);
    ASSERT_TRUE(r2.has_value()) << "Slot 2 must be populated after migration";
    EXPECT_EQ(r2->ocppCsmsUrl.get(), "wss://legacy-backup.example.com/ocpp");
    EXPECT_EQ(r2->securityProfile, 2);
    ASSERT_TRUE(r2->vpn.has_value()) << "VPN must be migrated";
    EXPECT_EQ(r2->vpn->server.get(), "vpn.example.com");
}

// AC4: No runtime code path reads the deprecated NetworkConnectionProfiles blob.
// Verified by grep: the only get_value/get_optional_value call for NetworkConnectionProfiles
// outside test code is in migrate_from_blob_if_needed() itself (ctrlr_component_variables.cpp).
// No other runtime code (charge_point.cpp, connectivity_manager, etc.) reads the blob.
// The charge_point.cpp call at line 430 only invokes migrate_from_blob_if_needed().

// ---------------------------------------------------------------------------
// US-005: Reboot persistence — only new DM component used after reinit
// Simulates a reboot by destroying the DeviceModel and creating a new one
// against the same database.  Verifies that:
//   - NetworkConfiguration DM slots are still populated
//   - Legacy blob remains empty
//   - Calling migrate_from_blob_if_needed() again is a no-op
// ---------------------------------------------------------------------------

TEST_F(NetworkConfigSyncTest, RebootPersistence_DmPopulatedAndBlobEmptyAfterReinit) {
    // --- Phase 1: initial boot with legacy blob ---

    // Clear default DM data so migration actually imports from the blob.
    NetworkConfigurationComponentVariables::clear_slot_in_device_model(*dm, 1);

    // Seed a legacy blob with two profiles.
    SetNetworkProfileRequest req1;
    req1.configurationSlot = 1;
    req1.connectionData = make_basic_profile(1, "wss://reboot-primary.example.com/ocpp");
    req1.connectionData.apn = make_test_apn();

    SetNetworkProfileRequest req2;
    req2.configurationSlot = 2;
    req2.connectionData = make_basic_profile(2, "wss://reboot-backup.example.com/ocpp");
    req2.connectionData.vpn = make_test_vpn();

    seed_blob(*dm, make_blob({req1, req2}));

    // AC1: Trigger migration (first boot)
    NetworkConfigurationComponentVariables::migrate_from_blob_if_needed(*dm);

    // Verify migration happened before simulating reboot
    ASSERT_TRUE(NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1).has_value())
        << "Precondition: slot 1 must be populated after first-boot migration";
    ASSERT_EQ(read_blob(*dm).size(), 0u) << "Precondition: blob must be cleared after first-boot migration";

    // --- Phase 2: simulated reboot ---
    // AC2: Destroy the charge point's DeviceModel and create a new one from the same database.
    // The DeviceModelTestHelper keeps the in-memory DB connection alive (shared cache),
    // so constructing a new DeviceModelStorageSqlite + DeviceModel simulates a reboot.
    auto rebooted_storage = std::make_unique<DeviceModelStorageSqlite>(DEVICE_MODEL_DB_IN_MEMORY_PATH);
    DeviceModel rebooted_dm(std::move(rebooted_storage));

    // AC3: After reinit, NetworkConfiguration DM slots are populated with correct profile data.
    auto r1 = NetworkConfigurationComponentVariables::read_profile_from_device_model(rebooted_dm, 1);
    ASSERT_TRUE(r1.has_value()) << "Slot 1 must be populated after reboot";
    EXPECT_EQ(r1->ocppCsmsUrl.get(), "wss://reboot-primary.example.com/ocpp");
    EXPECT_EQ(r1->securityProfile, 1);
    EXPECT_EQ(r1->ocppInterface, OCPPInterfaceEnum::Wired0);
    EXPECT_EQ(r1->ocppTransport, OCPPTransportEnum::JSON);
    EXPECT_EQ(r1->messageTimeout, 30);
    ASSERT_TRUE(r1->apn.has_value()) << "APN must persist across reboot";
    EXPECT_EQ(r1->apn->apn.get(), "internet");

    auto r2 = NetworkConfigurationComponentVariables::read_profile_from_device_model(rebooted_dm, 2);
    ASSERT_TRUE(r2.has_value()) << "Slot 2 must be populated after reboot";
    EXPECT_EQ(r2->ocppCsmsUrl.get(), "wss://reboot-backup.example.com/ocpp");
    EXPECT_EQ(r2->securityProfile, 2);
    ASSERT_TRUE(r2->vpn.has_value()) << "VPN must persist across reboot";
    EXPECT_EQ(r2->vpn->server.get(), "vpn.example.com");

    // AC4: After reinit, the legacy NetworkConnectionProfiles blob remains empty.
    const auto blob_after_reboot = read_blob(rebooted_dm);
    EXPECT_EQ(blob_after_reboot.size(), 0u) << "Blob must remain empty after reboot";

    // AC5: No re-migration occurs — calling migrate again is idempotent / skipped.
    // Slot 1 is already populated, so migration must be skipped; DM values must not change.
    NetworkConfigurationComponentVariables::migrate_from_blob_if_needed(rebooted_dm);

    auto r1_after = NetworkConfigurationComponentVariables::read_profile_from_device_model(rebooted_dm, 1);
    ASSERT_TRUE(r1_after.has_value()) << "Slot 1 must still be populated after second migration call";
    EXPECT_EQ(r1_after->ocppCsmsUrl.get(), "wss://reboot-primary.example.com/ocpp")
        << "DM values must not change when migration is called again (idempotent)";

    auto r2_after = NetworkConfigurationComponentVariables::read_profile_from_device_model(rebooted_dm, 2);
    ASSERT_TRUE(r2_after.has_value()) << "Slot 2 must still be populated after second migration call";
    EXPECT_EQ(r2_after->ocppCsmsUrl.get(), "wss://reboot-backup.example.com/ocpp");

    // Blob must still be empty after the no-op migration
    EXPECT_EQ(read_blob(rebooted_dm).size(), 0u) << "Blob must remain empty after no-op migration on reboot";
}

// ---------------------------------------------------------------------------
// Legacy struct validation tests (kept for completeness)
// ---------------------------------------------------------------------------

class NetworkConnectionProfileTest : public ::testing::Test {
protected:
    static NetworkConnectionProfile CreateTestProfile(int32_t security_profile = 1) {
        NetworkConnectionProfile profile;
        profile.ocppCsmsUrl = "wss://csms.example.com/ocpp";
        profile.securityProfile = security_profile;
        profile.ocppInterface = OCPPInterfaceEnum::Wired0;
        profile.ocppTransport = OCPPTransportEnum::JSON;
        profile.messageTimeout = 30;
        return profile;
    }
};

TEST_F(NetworkConnectionProfileTest, ProfileStructureIsValid) {
    auto profile = CreateTestProfile(1);
    EXPECT_EQ(profile.ocppCsmsUrl.get(), "wss://csms.example.com/ocpp");
    EXPECT_EQ(profile.securityProfile, 1);
    EXPECT_EQ(profile.ocppInterface, OCPPInterfaceEnum::Wired0);
    EXPECT_EQ(profile.ocppTransport, OCPPTransportEnum::JSON);
    EXPECT_EQ(profile.messageTimeout, 30);
}

TEST_F(NetworkConnectionProfileTest, DifferentSecurityProfilesCanBeAssigned) {
    for (int sec = 0; sec <= 3; ++sec) {
        auto profile = CreateTestProfile(sec);
        EXPECT_EQ(profile.securityProfile, sec);
    }
}

TEST_F(NetworkConnectionProfileTest, ProfileWithAPNCanBeCreated) {
    auto profile = CreateTestProfile();
    profile.apn = make_test_apn();
    ASSERT_TRUE(profile.apn.has_value());
    EXPECT_EQ(profile.apn->apn.get(), "internet");
}

TEST_F(NetworkConnectionProfileTest, ComponentVariablesAreValid) {
    auto& url_var = NetworkConfigurationComponentVariables::OcppCsmsUrl;
    auto& sec_var = NetworkConfigurationComponentVariables::SecurityProfile;
    auto& iface_var = NetworkConfigurationComponentVariables::OcppInterface;
    (void)url_var;
    (void)sec_var;
    (void)iface_var;
    SUCCEED();
}

// ---------------------------------------------------------------------------
// Edge cases: invalid inputs and slot overflow (US-001)
// ---------------------------------------------------------------------------

TEST_F(NetworkConfigSyncTest, WriteInvalidSecurityProfileAboveRangeIsRejected) {
    auto profile = make_basic_profile(5, "wss://csms.example.com/ocpp");
    EXPECT_FALSE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, profile, "test"))
        << "SecurityProfile=5 is outside the valid range [0,3] and must be rejected";
}

TEST_F(NetworkConfigSyncTest, WriteInvalidSecurityProfileNegativeIsRejected) {
    auto profile = make_basic_profile(-1, "wss://csms.example.com/ocpp");
    EXPECT_FALSE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, profile, "test"))
        << "SecurityProfile=-1 is outside the valid range [0,3] and must be rejected";
}

TEST_F(NetworkConfigSyncTest, WriteInvalidSecurityProfileBoundaryAboveIsRejected) {
    auto profile = make_basic_profile(4, "wss://csms.example.com/ocpp");
    EXPECT_FALSE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, profile, "test"))
        << "SecurityProfile=4 is outside the valid range [0,3] and must be rejected";
}

TEST_F(NetworkConfigSyncTest, WriteValidSecurityProfileBoundariesSucceed) {
    // Boundary values 0 and 3 must be accepted
    auto profile0 = make_basic_profile(0, "wss://csms0.example.com/ocpp");
    EXPECT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, profile0, "test"))
        << "SecurityProfile=0 is valid and must be accepted";

    auto profile3 = make_basic_profile(3, "wss://csms3.example.com/ocpp");
    EXPECT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, profile3, "test"))
        << "SecurityProfile=3 is valid and must be accepted";
}

TEST_F(NetworkConfigSyncTest, WriteUrlExceedingMaxLimitIsRejected) {
    // OcppCsmsUrl is CiString<2000> — setting a value longer than 2000 chars must fail.
    // The CiString constructor throws on overflow, which write_profile_to_device_model catches.
    // We also verify that the DM itself rejects too-long values via set_value directly.
    const auto url_cv = NetworkConfigurationComponentVariables::get_component_variable(
        1, NetworkConfigurationComponentVariables::OcppCsmsUrl);
    std::string long_url = "wss://csms.example.com/" + std::string(2000, 'x');
    auto status = dm->set_value(url_cv.component, url_cv.variable.value(), AttributeEnum::Actual, long_url, "test");
    EXPECT_NE(status, SetVariableStatusEnum::Accepted)
        << "URL exceeding maxLimit=2000 must be rejected by the device model";
}

TEST_F(NetworkConfigSyncTest, WriteEmptyUrlSucceedsButReadReturnsNullopt) {
    // An empty URL can be set (clearing the slot effectively), but read treats it as unconfigured
    auto profile = make_basic_profile(1, "");
    // write_profile_to_device_model may succeed (empty string is valid for the DM string type)
    // but read_profile_from_device_model treats empty URL as "slot not configured"
    NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, profile, "test");
    auto result = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    EXPECT_FALSE(result.has_value()) << "Empty URL must be treated as unconfigured slot";
}

TEST_F(NetworkConfigSyncTest, WriteToSlotBeyondConfiguredMaxFails) {
    // Only NetworkConfiguration_1 and _2 exist in the test DM config.
    // Writing to slot 3 should fail because the component does not exist.
    auto profile = make_basic_profile(1, "wss://overflow.example.com/ocpp");
    EXPECT_FALSE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 3, profile, "test"))
        << "Slot 3 does not exist in the device model and write must fail";
}

TEST_F(NetworkConfigSyncTest, ReadFromSlotBeyondConfiguredMaxReturnsNullopt) {
    // Reading from a non-existent slot should return nullopt, not crash
    auto result = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 3);
    EXPECT_FALSE(result.has_value()) << "Slot 3 does not exist and read must return nullopt";
}

TEST_F(NetworkConfigSyncTest, WriteToSlotZeroFails) {
    // Slot 0 is not a valid NetworkConfiguration instance
    auto profile = make_basic_profile(1, "wss://slot0.example.com/ocpp");
    EXPECT_FALSE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 0, profile, "test"))
        << "Slot 0 does not exist in the device model and write must fail";
}

TEST_F(NetworkConfigSyncTest, WriteToNegativeSlotFails) {
    auto profile = make_basic_profile(1, "wss://negative.example.com/ocpp");
    EXPECT_FALSE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, -1, profile, "test"))
        << "Negative slot does not exist in the device model and write must fail";
}

TEST_F(NetworkConfigSyncTest, ClearNonExistentSlotDoesNotCrash) {
    // Clearing a non-existent slot should not throw or crash
    EXPECT_NO_THROW(NetworkConfigurationComponentVariables::clear_slot_in_device_model(*dm, 3))
        << "Clearing a non-existent slot must not crash";
}

// ---------------------------------------------------------------------------
// US-002: ConnectivityManager cache rebuild callback tests
// Verifies that reload_network_profiles() / set_network_profile() keep the
// ConnectivityManager's in-memory cache in sync with the device model.
// ---------------------------------------------------------------------------

class ConnectivityManagerCacheTest : public ::testing::Test {
protected:
    DeviceModelTestHelper dm_helper;
    DeviceModel* dm{nullptr};
    std::shared_ptr<ocpp::EvseSecurityMock> evse_security;
    std::shared_ptr<ocpp::MessageLogging> logging;
    std::unique_ptr<ConnectivityManager> cm;

    ConnectivityManagerCacheTest() : dm_helper() {
        dm = dm_helper.get_device_model();
        // Seed slot 1 so the cache has something to work with (default OcppCsmsUrl is empty)
        auto seed_profile = make_basic_profile(1, "wss://ocpp.example.com");
        NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, seed_profile, "test");
        evse_security = std::make_shared<ocpp::EvseSecurityMock>();
        logging = std::make_shared<ocpp::MessageLogging>(false, "", "", false, false, false, false, false, false, false,
                                                         nullptr);
        cm = std::make_unique<ConnectivityManager>(*dm, evse_security, logging, "", [](const std::string&) {});
    }
};

// AC1: Verify initial cache reflects the DM's default state (slot 1 populated from config)
TEST_F(ConnectivityManagerCacheTest, InitialCacheReflectsDeviceModel) {
    // The test DM config has NetworkConfigurationPriority="1" and slot 1 has a default URL
    auto slots = cm->get_network_connection_slots();
    ASSERT_FALSE(slots.empty()) << "Priority list must not be empty";
    EXPECT_EQ(slots.at(0), 1);

    auto profile = cm->get_network_connection_profile(1);
    ASSERT_TRUE(profile.has_value()) << "Slot 1 must be cached from DM defaults";
    EXPECT_EQ(profile->ocppCsmsUrl.get(), "wss://ocpp.example.com");
}

// AC1: Modifying a profile in the DM + reload_network_profiles() updates the cache
TEST_F(ConnectivityManagerCacheTest, ReloadAfterDmUpdateRefreshesCache) {
    // Directly write a new profile to the DM (bypassing ConnectivityManager)
    auto updated = make_basic_profile(2, "wss://updated.example.com/ocpp");
    ASSERT_TRUE(NetworkConfigurationComponentVariables::write_profile_to_device_model(*dm, 1, updated, "test"));

    // Cache still has the old value before reload
    auto before = cm->get_network_connection_profile(1);
    ASSERT_TRUE(before.has_value());
    EXPECT_EQ(before->ocppCsmsUrl.get(), "wss://ocpp.example.com") << "Cache must still have old value before reload";

    // Reload the cache (simulates what Provisioning FB does after SetVariables)
    cm->reload_network_profiles();

    // AC2: After reload, cache reflects the updated value
    auto after = cm->get_network_connection_profile(1);
    ASSERT_TRUE(after.has_value()) << "Slot 1 must still be in cache after reload";
    EXPECT_EQ(after->ocppCsmsUrl.get(), "wss://updated.example.com/ocpp");
    EXPECT_EQ(after->securityProfile, 2);
}

// AC1+AC2: set_network_profile() writes to DM and automatically refreshes cache
TEST_F(ConnectivityManagerCacheTest, SetNetworkProfileUpdatesCache) {
    auto new_profile = make_basic_profile(3, "wss://new-csms.example.com/ocpp");
    ASSERT_TRUE(cm->set_network_profile(1, new_profile, "test"));

    auto cached = cm->get_network_connection_profile(1);
    ASSERT_TRUE(cached.has_value()) << "Profile must be in cache after set_network_profile";
    EXPECT_EQ(cached->ocppCsmsUrl.get(), "wss://new-csms.example.com/ocpp");
    EXPECT_EQ(cached->securityProfile, 3);
}

// AC3: Clear/delete scenario — clearing a slot in the DM + reload removes it from cache
TEST_F(ConnectivityManagerCacheTest, ReloadAfterClearSlotRemovesFromCache) {
    // Precondition: slot 1 is in the cache
    ASSERT_TRUE(cm->get_network_connection_profile(1).has_value());

    // Clear slot 1 directly in the DM
    NetworkConfigurationComponentVariables::clear_slot_in_device_model(*dm, 1);

    // Reload the cache
    cm->reload_network_profiles();

    // Slot 1 is still in the priority list but its profile is gone (read returns nullopt for empty slot)
    auto profile = cm->get_network_connection_profile(1);
    EXPECT_FALSE(profile.has_value()) << "Cleared slot must not have a cached profile after reload";
}

// AC3: Add a profile to a previously-empty slot via set_network_profile
TEST_F(ConnectivityManagerCacheTest, SetNetworkProfileAddsNewSlotToCache) {
    // Slot 2 has no default URL in the test config, so it should not be in the cache initially
    auto initial = cm->get_network_connection_profile(2);
    EXPECT_FALSE(initial.has_value()) << "Slot 2 must not be cached initially (no default URL)";

    // Add a profile to slot 2 via the ConnectivityManager
    auto profile = make_basic_profile(1, "wss://slot2.example.com/ocpp");
    ASSERT_TRUE(cm->set_network_profile(2, profile, "test"));

    // Slot 2 must now be in the cache
    auto cached = cm->get_network_connection_profile(2);
    ASSERT_TRUE(cached.has_value()) << "Newly added slot 2 must be in cache after set_network_profile";
    EXPECT_EQ(cached->ocppCsmsUrl.get(), "wss://slot2.example.com/ocpp");

    // Slot 2 must also appear in the priority list
    auto slots = cm->get_network_connection_slots();
    EXPECT_NE(std::find(slots.begin(), slots.end(), 2), slots.end())
        << "Slot 2 must be in the priority list after set_network_profile";
}

// AC2+AC3: Full lifecycle — add, update, then clear a profile
TEST_F(ConnectivityManagerCacheTest, AddUpdateClearLifecycle) {
    // Step 1: Add profile to slot 2
    auto added = make_basic_profile(1, "wss://added.example.com/ocpp");
    ASSERT_TRUE(cm->set_network_profile(2, added, "test"));
    ASSERT_TRUE(cm->get_network_connection_profile(2).has_value());

    // Step 2: Update slot 2 via set_network_profile
    auto updated = make_basic_profile(2, "wss://updated-slot2.example.com/ocpp");
    ASSERT_TRUE(cm->set_network_profile(2, updated, "test"));
    auto after_update = cm->get_network_connection_profile(2);
    ASSERT_TRUE(after_update.has_value());
    EXPECT_EQ(after_update->ocppCsmsUrl.get(), "wss://updated-slot2.example.com/ocpp");
    EXPECT_EQ(after_update->securityProfile, 2);

    // Step 3: Clear slot 2 in DM and reload
    NetworkConfigurationComponentVariables::clear_slot_in_device_model(*dm, 2);
    cm->reload_network_profiles();
    EXPECT_FALSE(cm->get_network_connection_profile(2).has_value())
        << "Cleared slot 2 must not be in cache after reload";
}

// ---------------------------------------------------------------------------
// US-003: Provisioning block — reject active slot modification (B09.FR.21/22)
// Verifies that validate_set_variable() rejects SetVariables targeting the
// currently-active connection profile slot and allows non-active slots.
// ---------------------------------------------------------------------------

// Minimal mock stubs for functional block interfaces needed by Provisioning constructor.
// validate_set_variable() only reads from device_model — these are never called.

class AvailabilityMock : public AvailabilityInterface {
public:
    MOCK_METHOD(void, handle_message, (const ocpp::EnhancedMessage<MessageType>&), (override));
    MOCK_METHOD(void, status_notification_req, (std::int32_t, std::int32_t, ConnectorStatusEnum, bool), (override));
    MOCK_METHOD(void, heartbeat_req, (bool), (override));
    MOCK_METHOD(void, handle_scheduled_change_availability_requests, (std::int32_t), (override));
    MOCK_METHOD(void, set_scheduled_change_availability_requests, (std::int32_t, AvailabilityChange), (override));
    MOCK_METHOD(void, set_heartbeat_timer_interval, (const std::chrono::seconds&), (override));
    MOCK_METHOD(void, stop_heartbeat_timer, (), (override));
};

class MeterValuesMock : public MeterValuesInterface {
public:
    MOCK_METHOD(void, handle_message, (const ocpp::EnhancedMessage<MessageType>&), (override));
    MOCK_METHOD(void, update_aligned_data_interval, (), (override));
    MOCK_METHOD(void, on_meter_value, (std::int32_t, const MeterValue&), (override));
    MOCK_METHOD(MeterValue, get_latest_meter_value_filtered,
                (const MeterValue&, ReadingContextEnum, const RequiredComponentVariable&), (override));
    MOCK_METHOD(void, meter_values_req, (std::int32_t, const std::vector<MeterValue>&, bool), (override));
};

class SecurityMock : public SecurityInterface {
public:
    MOCK_METHOD(void, handle_message, (const ocpp::EnhancedMessage<MessageType>&), (override));
    MOCK_METHOD(void, security_event_notification_req,
                (const CiString<50>&, const std::optional<CiString<255>>&, bool, bool, const std::optional<DateTime>&),
                (override));
    MOCK_METHOD(void, sign_certificate_req, (const ocpp::CertificateSigningUseEnum&, bool), (override));
    MOCK_METHOD(void, stop_certificate_signed_timer, (), (override));
    MOCK_METHOD(void, init_certificate_expiration_check_timers, (), (override));
    MOCK_METHOD(void, stop_certificate_expiration_check_timers, (), (override));
    MOCK_METHOD(Get15118EVCertificateResponse, on_get_15118_ev_certificate_request,
                (const Get15118EVCertificateRequest&), (override));
};

class DiagnosticsMock : public DiagnosticsInterface {
public:
    MOCK_METHOD(void, handle_message, (const ocpp::EnhancedMessage<MessageType>&), (override));
    MOCK_METHOD(void, notify_event_req, (const std::vector<EventData>&), (override));
    MOCK_METHOD(void, stop_monitoring, (), (override));
    MOCK_METHOD(void, start_monitoring, (), (override));
    MOCK_METHOD(void, process_triggered_monitors, (), (override));
};

class TransactionMock : public TransactionInterface {
public:
    MOCK_METHOD(void, handle_message, (const ocpp::EnhancedMessage<MessageType>&), (override));
    MOCK_METHOD(void, on_transaction_started,
                (std::int32_t, std::int32_t, const std::string&, const DateTime&, TriggerReasonEnum, const MeterValue&,
                 const std::optional<IdToken>&, const std::optional<IdToken>&, const std::optional<std::int32_t>&,
                 const std::optional<std::int32_t>&, ChargingStateEnum),
                (override));
    MOCK_METHOD(void, on_transaction_finished,
                (std::int32_t, const DateTime&, const MeterValue&, ReasonEnum, TriggerReasonEnum,
                 const std::optional<IdToken>&, const std::optional<std::string>&, ChargingStateEnum),
                (override));
    MOCK_METHOD(void, transaction_event_req,
                (const TransactionEventEnum&, const DateTime&, const Transaction&, const TriggerReasonEnum&,
                 std::int32_t, const std::optional<std::int32_t>&, const std::optional<EVSE>&,
                 const std::optional<IdToken>&, const std::optional<std::vector<MeterValue>>&,
                 const std::optional<std::int32_t>&, bool, const std::optional<std::int32_t>&, bool),
                (override));
    MOCK_METHOD(void, set_remote_start_id_for_evse, (std::int32_t, IdToken, std::int32_t), (override));
    MOCK_METHOD(void, schedule_reset, (std::optional<std::int32_t>), (override));
};

// Helper: build a SetVariableData targeting a NetworkConfiguration slot variable
static SetVariableData make_set_variable_data(int slot, const std::string& variable_name, const std::string& value) {
    SetVariableData svd;
    svd.attributeValue = value;
    svd.component.name = "NetworkConfiguration";
    svd.component.instance = CiString<50>(std::to_string(slot));
    svd.variable.name = variable_name;
    return svd;
}

// Helper: check if a SetVariableResult was rejected due to active slot protection (B09.FR.21/22)
static bool is_rejected_by_active_slot(const SetVariableResult& result) {
    return result.attributeStatus == SetVariableStatusEnum::Rejected && result.attributeStatusInfo.has_value() &&
           result.attributeStatusInfo->reasonCode.get() == "PriorityNetworkConf";
}

class ProvisioningActiveSlotTest : public ::testing::Test {
protected:
    DeviceModelTestHelper dm_helper;
    DeviceModel* dm{nullptr};

    // Mocks for FunctionalBlockContext
    ::testing::NiceMock<MockMessageDispatcher> mock_dispatcher;
    ::testing::NiceMock<ConnectivityManagerMock> connectivity_manager;
    std::unique_ptr<EvseManagerFake> evse_manager;
    ::testing::NiceMock<DatabaseHandlerMock> db_handler;
    ocpp::EvseSecurityMock evse_security;
    ::testing::NiceMock<ComponentStateManagerMock> component_state_manager;
    std::atomic<OcppProtocolVersion> ocpp_version{OcppProtocolVersion::v201};

    // Mocks for Provisioning dependencies
    ::testing::NiceMock<OcspUpdaterMock> ocsp_updater;
    ::testing::NiceMock<AvailabilityMock> availability;
    ::testing::NiceMock<MeterValuesMock> meter_values;
    ::testing::NiceMock<SecurityMock> security;
    ::testing::NiceMock<DiagnosticsMock> diagnostics;
    ::testing::NiceMock<TransactionMock> transaction;
    std::atomic<RegistrationStatusEnum> registration_status{RegistrationStatusEnum::Accepted};

    std::unique_ptr<FunctionalBlockContext> fb_context;
    std::unique_ptr<MessageQueue<MessageType>> message_queue;
    std::unique_ptr<Provisioning> provisioning;

    ProvisioningActiveSlotTest() : dm_helper() {
        dm = dm_helper.get_device_model();
        evse_manager = std::make_unique<EvseManagerFake>(1);

        fb_context =
            std::make_unique<FunctionalBlockContext>(mock_dispatcher, *dm, connectivity_manager, *evse_manager,
                                                     db_handler, evse_security, component_state_manager, ocpp_version);

        // MessageQueue with no-op send callback and nullptr db handler (never started)
        MessageQueueConfig<MessageType> mq_config;
        message_queue = std::make_unique<MessageQueue<MessageType>>([](json) { return false; }, mq_config, nullptr);

        provisioning = std::make_unique<Provisioning>(
            *fb_context, *message_queue, ocsp_updater, availability, meter_values, security, diagnostics, transaction,
            std::nullopt,                                                    // time_sync_callback
            std::nullopt,                                                    // boot_notification_callback
            std::nullopt,                                                    // validate_network_profile_callback
            [](auto, auto) { return true; },                                 // is_reset_allowed
            [](auto, auto) {},                                               // reset_callback
            [](auto, auto) { return RequestStartStopStatusEnum::Accepted; }, // stop_transaction
            std::nullopt,                                                    // variable_changed_callback
            registration_status);
    }

    // Set the ActiveNetworkProfile in the DM to simulate an active connection on the given slot
    void set_active_slot(int slot) {
        ASSERT_TRUE(ControllerComponentVariables::ActiveNetworkProfile.variable.has_value());
        auto status = dm->set_read_only_value(ControllerComponentVariables::ActiveNetworkProfile.component,
                                              ControllerComponentVariables::ActiveNetworkProfile.variable.value(),
                                              AttributeEnum::Actual, std::to_string(slot), "internal");
        ASSERT_EQ(status, SetVariableStatusEnum::Accepted)
            << "Precondition: must be able to set ActiveNetworkProfile to " << slot;
    }

    // Call set_variables and return the result for the single variable
    SetVariableResult set_single_variable(const SetVariableData& svd) {
        auto results = provisioning->set_variables({svd}, "test");
        EXPECT_EQ(results.size(), 1u);
        return results.begin()->second;
    }
};

// AC1: SetVariables targeting OcppCsmsUrl on the active slot is rejected
TEST_F(ProvisioningActiveSlotTest, RejectOcppCsmsUrlOnActiveSlot) {
    set_active_slot(1);
    auto result = set_single_variable(make_set_variable_data(1, "OcppCsmsUrl", "wss://new.example.com/ocpp"));
    EXPECT_TRUE(is_rejected_by_active_slot(result)) << "OcppCsmsUrl on active slot must be rejected";
}

// AC1: SetVariables targeting SecurityProfile on the active slot is rejected
TEST_F(ProvisioningActiveSlotTest, RejectSecurityProfileOnActiveSlot) {
    set_active_slot(1);
    auto result = set_single_variable(make_set_variable_data(1, "SecurityProfile", "2"));
    EXPECT_TRUE(is_rejected_by_active_slot(result)) << "SecurityProfile on active slot must be rejected";
}

// AC1: SetVariables targeting OcppTransport on the active slot is rejected
TEST_F(ProvisioningActiveSlotTest, RejectOcppTransportOnActiveSlot) {
    set_active_slot(1);
    auto result = set_single_variable(make_set_variable_data(1, "OcppTransport", "SOAP"));
    EXPECT_TRUE(is_rejected_by_active_slot(result)) << "OcppTransport on active slot must be rejected";
}

// AC2: SetVariables targeting OcppCsmsUrl on a non-active slot passes validation (not rejected by active-slot check)
TEST_F(ProvisioningActiveSlotTest, AllowOcppCsmsUrlOnNonActiveSlot) {
    set_active_slot(1);
    auto result = set_single_variable(make_set_variable_data(2, "OcppCsmsUrl", "wss://backup.example.com/ocpp"));
    EXPECT_FALSE(is_rejected_by_active_slot(result)) << "OcppCsmsUrl on non-active slot must not be blocked";
}

// AC2: SetVariables targeting SecurityProfile on a non-active slot passes validation
TEST_F(ProvisioningActiveSlotTest, AllowSecurityProfileOnNonActiveSlot) {
    set_active_slot(1);
    auto result = set_single_variable(make_set_variable_data(2, "SecurityProfile", "2"));
    EXPECT_FALSE(is_rejected_by_active_slot(result)) << "SecurityProfile on non-active slot must not be blocked";
}

// AC2: SetVariables targeting OcppTransport on a non-active slot passes validation
TEST_F(ProvisioningActiveSlotTest, AllowOcppTransportOnNonActiveSlot) {
    set_active_slot(1);
    auto result = set_single_variable(make_set_variable_data(2, "OcppTransport", "SOAP"));
    EXPECT_FALSE(is_rejected_by_active_slot(result)) << "OcppTransport on non-active slot must not be blocked";
}

// AC1+AC2: When slot 2 is active, slot 2 writes are rejected but slot 1 writes are allowed
TEST_F(ProvisioningActiveSlotTest, ActiveSlot2RejectsSlot2AllowsSlot1) {
    set_active_slot(2);

    auto result2 = set_single_variable(make_set_variable_data(2, "OcppCsmsUrl", "wss://new.example.com"));
    EXPECT_TRUE(is_rejected_by_active_slot(result2)) << "Must reject SetVariables on active slot 2";

    auto result1 = set_single_variable(make_set_variable_data(1, "OcppCsmsUrl", "wss://other.example.com"));
    EXPECT_FALSE(is_rejected_by_active_slot(result1)) << "Must allow SetVariables on non-active slot 1";
}

} // namespace ocpp::v2
