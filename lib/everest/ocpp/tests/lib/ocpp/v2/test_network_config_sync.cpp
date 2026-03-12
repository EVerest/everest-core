// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <device_model_test_helper.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model_interface.hpp>
#include <ocpp/v2/messages/SetNetworkProfile.hpp>
#include <ocpp/v2/ocpp_types.hpp>

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

TEST_F(NetworkConfigSyncTest, MigrateOverwritesDmWithBlobValue) {
    // Slot 1 has a default OcppCsmsUrl — blob import must overwrite it
    ASSERT_TRUE(NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1).has_value())
        << "Test precondition: slot 1 must have default data in the test DM config";

    SetNetworkProfileRequest req;
    req.configurationSlot = 1;
    req.connectionData = make_basic_profile(1, "wss://blob-set.example.com");
    seed_blob(*dm, make_blob({req}));
    NetworkConfigurationComponentVariables::migrate_from_blob_if_needed(*dm);

    auto result = NetworkConfigurationComponentVariables::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(result.has_value());
    // Blob value wins — blob always overwrites DM on migration
    EXPECT_EQ(result->ocppCsmsUrl.get(), "wss://blob-set.example.com")
        << "Blob must overwrite DM data during migration";
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

} // namespace ocpp::v2
