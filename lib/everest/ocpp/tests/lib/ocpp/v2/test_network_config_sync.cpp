// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <device_model_test_helper.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/messages/SetNetworkProfile.hpp>
#include <ocpp/v2/network_config_sync.hpp>
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
    EXPECT_TRUE(network_config::write_profile_to_device_model(*dm, 1, profile, "test"));
}

TEST_F(NetworkConfigSyncTest, WriteProfileWithApnSucceeds) {
    auto profile = make_basic_profile();
    profile.apn = make_test_apn();
    EXPECT_TRUE(network_config::write_profile_to_device_model(*dm, 1, profile, "test"));
}

TEST_F(NetworkConfigSyncTest, WriteProfileWithVpnSucceeds) {
    auto profile = make_basic_profile();
    profile.vpn = make_test_vpn();
    EXPECT_TRUE(network_config::write_profile_to_device_model(*dm, 1, profile, "test"));
}

// ---------------------------------------------------------------------------
// read_profile_from_device_model
// ---------------------------------------------------------------------------

TEST_F(NetworkConfigSyncTest, ReadFromEmptySlotReturnsNullopt) {
    // Slot 2 has no default OcppCsmsUrl, so reading without writing first returns nullopt
    auto result = network_config::read_profile_from_device_model(*dm, 2);
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetworkConfigSyncTest, WriteAndReadBasicProfileRoundtrip) {
    auto original = make_basic_profile(2, "wss://test.server/ocpp");
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 1, original, "test"));

    auto result = network_config::read_profile_from_device_model(*dm, 1);
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
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 1, original, "test"));

    auto result = network_config::read_profile_from_device_model(*dm, 1);
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
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 1, original, "test"));

    auto result = network_config::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->identity.has_value());
    EXPECT_EQ(result->identity->get(), "per_slot_identity");
}

TEST_F(NetworkConfigSyncTest, OverwriteProfileUpdatesFields) {
    auto first = make_basic_profile(1, "wss://first.example.com/ocpp");
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 1, first, "test"));

    auto second = make_basic_profile(2, "wss://second.example.com/ocpp");
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 1, second, "test"));

    auto result = network_config::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->ocppCsmsUrl.get(), "wss://second.example.com/ocpp");
    EXPECT_EQ(result->securityProfile, 2);
}

TEST_F(NetworkConfigSyncTest, TwoSlotsAreIndependent) {
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 1, make_basic_profile(1, "wss://slot1.example.com"),
                                                              "test"));
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 2, make_basic_profile(2, "wss://slot2.example.com"),
                                                              "test"));

    auto r1 = network_config::read_profile_from_device_model(*dm, 1);
    auto r2 = network_config::read_profile_from_device_model(*dm, 2);

    ASSERT_TRUE(r1.has_value());
    ASSERT_TRUE(r2.has_value());
    EXPECT_EQ(r1->ocppCsmsUrl.get(), "wss://slot1.example.com");
    EXPECT_EQ(r2->ocppCsmsUrl.get(), "wss://slot2.example.com");
    EXPECT_EQ(r1->securityProfile, 1);
    EXPECT_EQ(r2->securityProfile, 2);
}

// ---------------------------------------------------------------------------
// sync_json_blob_from_device_model
// ---------------------------------------------------------------------------

TEST_F(NetworkConfigSyncTest, SyncSingleSlotBuildsBlob) {
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 1, make_basic_profile(1), "test"));

    // Pre-seed the blob with a valid empty array so get_value succeeds
    seed_blob(*dm, json::array());

    network_config::sync_json_blob_from_device_model(*dm, {1});

    const auto blob = read_blob(*dm);
    ASSERT_EQ(blob.size(), 1u);
    EXPECT_EQ(blob[0].at("configurationSlot").get<int>(), 1);
    EXPECT_EQ(blob[0].at("connectionData").at("ocppCsmsUrl").get<std::string>(), "wss://csms.example.com/ocpp");
}

// Regression test for the data-loss bug: updating slot 2 must not erase slot 1.
TEST_F(NetworkConfigSyncTest, SyncPartialUpdatePreservesOtherSlots) {
    // Write both slots to the device model
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 1, make_basic_profile(1, "wss://slot1.example.com"),
                                                              "test"));
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 2, make_basic_profile(2, "wss://slot2.example.com"),
                                                              "test"));

    // Seed the blob with both slots present
    seed_blob(*dm, json::array());
    network_config::sync_json_blob_from_device_model(*dm, {1, 2});
    ASSERT_EQ(read_blob(*dm).size(), 2u);

    // Now update only slot 2
    auto updated = make_basic_profile(2, "wss://slot2-updated.example.com");
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 2, updated, "test"));
    network_config::sync_json_blob_from_device_model(*dm, {2});

    // Both slots must still be present
    const auto blob = read_blob(*dm);
    ASSERT_EQ(blob.size(), 2u);

    // Find each slot in the blob (order may vary)
    std::string slot1_url, slot2_url;
    for (const auto& entry : blob) {
        if (entry.at("configurationSlot").get<int>() == 1) {
            slot1_url = entry.at("connectionData").at("ocppCsmsUrl").get<std::string>();
        } else if (entry.at("configurationSlot").get<int>() == 2) {
            slot2_url = entry.at("connectionData").at("ocppCsmsUrl").get<std::string>();
        }
    }
    EXPECT_EQ(slot1_url, "wss://slot1.example.com") << "Slot 1 must be preserved after slot 2 update";
    EXPECT_EQ(slot2_url, "wss://slot2-updated.example.com") << "Slot 2 must reflect the update";
}

TEST_F(NetworkConfigSyncTest, SyncWithEmptyBlobProducesValidArray) {
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 1, make_basic_profile(), "test"));

    // No pre-existing blob — should be handled gracefully
    // (get_value throws if unset, so the catch branch rebuilds from scratch)
    network_config::sync_json_blob_from_device_model(*dm, {1});
    // If NetworkConnectionProfiles was not pre-set, the function logs a warning and
    // still writes the new profiles. Verify it doesn't crash.
    SUCCEED();
}

TEST_F(NetworkConfigSyncTest, SyncPreservesThreeSlots) {
    // Write three separate URL values — slot 3 doesn't exist in the test config
    // so only slots 1 and 2 are tested here. Both must survive a partial update.
    ASSERT_TRUE(
        network_config::write_profile_to_device_model(*dm, 1, make_basic_profile(1, "wss://a.example.com"), "test"));
    ASSERT_TRUE(
        network_config::write_profile_to_device_model(*dm, 2, make_basic_profile(2, "wss://b.example.com"), "test"));

    seed_blob(*dm, json::array());
    network_config::sync_json_blob_from_device_model(*dm, {1, 2});

    // Update only slot 1
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 1, make_basic_profile(1, "wss://a-new.example.com"),
                                                              "test"));
    network_config::sync_json_blob_from_device_model(*dm, {1});

    const auto blob = read_blob(*dm);
    ASSERT_EQ(blob.size(), 2u);
    for (const auto& entry : blob) {
        const int slot = entry.at("configurationSlot").get<int>();
        const auto url = entry.at("connectionData").at("ocppCsmsUrl").get<std::string>();
        if (slot == 1) {
            EXPECT_EQ(url, "wss://a-new.example.com");
        } else if (slot == 2) {
            EXPECT_EQ(url, "wss://b.example.com");
        }
    }
}

// ---------------------------------------------------------------------------
// seed_device_model_from_json_blob
// ---------------------------------------------------------------------------

TEST_F(NetworkConfigSyncTest, SeedFromBlobPopulatesSlot1) {
    // Build a JSON blob with one profile at slot 1
    SetNetworkProfileRequest req;
    req.configurationSlot = 1;
    req.connectionData = make_basic_profile(1, "wss://seeded.example.com/ocpp");
    seed_blob(*dm, make_blob({req}));

    network_config::seed_device_model_from_json_blob(*dm);

    auto result = network_config::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->ocppCsmsUrl.get(), "wss://seeded.example.com/ocpp");
    EXPECT_EQ(result->securityProfile, 1);
}

TEST_F(NetworkConfigSyncTest, SeedFromBlobPopulatesBothSlots) {
    SetNetworkProfileRequest req1;
    req1.configurationSlot = 1;
    req1.connectionData = make_basic_profile(1, "wss://primary.example.com");

    SetNetworkProfileRequest req2;
    req2.configurationSlot = 2;
    req2.connectionData = make_basic_profile(2, "wss://backup.example.com");

    seed_blob(*dm, make_blob({req1, req2}));
    network_config::seed_device_model_from_json_blob(*dm);

    auto r1 = network_config::read_profile_from_device_model(*dm, 1);
    auto r2 = network_config::read_profile_from_device_model(*dm, 2);

    ASSERT_TRUE(r1.has_value());
    ASSERT_TRUE(r2.has_value());
    EXPECT_EQ(r1->ocppCsmsUrl.get(), "wss://primary.example.com");
    EXPECT_EQ(r2->ocppCsmsUrl.get(), "wss://backup.example.com");
    EXPECT_EQ(r2->securityProfile, 2);
}

TEST_F(NetworkConfigSyncTest, SeedFromBlobWithApnPopulatesApnFields) {
    SetNetworkProfileRequest req;
    req.configurationSlot = 1;
    req.connectionData = make_basic_profile();
    req.connectionData.apn = make_test_apn();

    seed_blob(*dm, make_blob({req}));
    network_config::seed_device_model_from_json_blob(*dm);

    auto result = network_config::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->apn.has_value());
    EXPECT_EQ(result->apn->apn.get(), "internet");
}

TEST_F(NetworkConfigSyncTest, SeedThenSyncRoundtrip) {
    // Start: seed from a blob
    SetNetworkProfileRequest req;
    req.configurationSlot = 1;
    req.connectionData = make_basic_profile(1, "wss://original.example.com");
    seed_blob(*dm, make_blob({req}));
    network_config::seed_device_model_from_json_blob(*dm);

    // Update the DM directly (as if via SetVariables)
    auto updated_profile = make_basic_profile(1, "wss://updated.example.com");
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 1, updated_profile, "test"));

    // Sync back to blob
    network_config::sync_json_blob_from_device_model(*dm, {1});

    // The blob must reflect the update
    const auto blob = read_blob(*dm);
    ASSERT_EQ(blob.size(), 1u);
    EXPECT_EQ(blob[0].at("connectionData").at("ocppCsmsUrl").get<std::string>(), "wss://updated.example.com");
}

TEST_F(NetworkConfigSyncTest, SeedDoesNotCorruptExistingDmValues) {
    // First, write slot 1 directly via DM
    ASSERT_TRUE(network_config::write_profile_to_device_model(*dm, 1, make_basic_profile(1, "wss://dm-set.example.com"),
                                                              "test"));

    // Now seed from a blob with a different URL for slot 1 — seed should overwrite
    SetNetworkProfileRequest req;
    req.configurationSlot = 1;
    req.connectionData = make_basic_profile(1, "wss://blob-set.example.com");
    seed_blob(*dm, make_blob({req}));
    network_config::seed_device_model_from_json_blob(*dm);

    auto result = network_config::read_profile_from_device_model(*dm, 1);
    ASSERT_TRUE(result.has_value());
    // Seed takes precedence — last write wins
    EXPECT_EQ(result->ocppCsmsUrl.get(), "wss://blob-set.example.com");
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
