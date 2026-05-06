// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include "configuration_stub.hpp"
#include "ocpp/v16/known_keys.hpp"
#include "ocpp/v2/device_model.hpp"
#include "ocpp/v2/ocpp_enums.hpp"
#include "ocpp/v2/ocpp_types.hpp"
#include "ocpp16_test_config.hpp"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include <ocpp/v16/charge_point_configuration_base.hpp>
#include <ocpp/v2/device_model_storage_sqlite.hpp>
#include <ocpp/v2/init_device_model_db.hpp>
#include <ocpp/v2/ocpp16_component_config_patcher.hpp>
#include <optional>

namespace {
using namespace ocpp::v16::stubs;

using json = nlohmann::json;

struct KeyValueNormalized {
    bool readonly;
    std::optional<std::string> value;
};

std::optional<std::string> key_value_to_string(const std::optional<ocpp::CiString<500>>& value) {
    if (!value.has_value()) {
        return std::nullopt;
    }
    return value->get();
}

bool values_equal(const std::optional<std::string>& lhs, const std::optional<std::string>& rhs) {
    if (!lhs.has_value() || !rhs.has_value()) {
        return lhs == rhs;
    }

    if (lhs.value() == rhs.value()) {
        return true;
    }

    try {
        return json::parse(lhs.value()) == json::parse(rhs.value());
    } catch (const json::parse_error&) {
        return false;
    }
}

std::map<std::string, KeyValueNormalized> to_map(const std::vector<ocpp::v16::KeyValue>& key_values) {
    static const std::set<std::string> excluded_keys = {
        // Not patched yet in patch_component_config_with_ocpp16.
        "ConnectorPhaseRotation",
        "ConnectorEvseIds",
    };

    std::map<std::string, KeyValueNormalized> result;
    for (const auto& kv : key_values) {
        const auto key = kv.key.get();
        if (excluded_keys.find(key) != excluded_keys.end()) {
            continue;
        }

        const auto inserted =
            result.emplace(key, KeyValueNormalized{kv.readonly, key_value_to_string(kv.value)}).second;
        EXPECT_TRUE(inserted) << "Duplicate key in get_all_key_value(): " << key;
    }
    return result;
}

// run tests against V16 JSON and V2 database
// gtest_filter: Config/Configuration.*
INSTANTIATE_TEST_SUITE_P(Config, Configuration, testing::Values("sql", "json"));
INSTANTIATE_TEST_SUITE_P(Config, ConfigurationFull, testing::Values("sql", "json"));

TEST(ConnectorID, Extract) {
    using CPCB = ocpp::v16::ChargePointConfigurationBase;

    EXPECT_EQ(CPCB::extractConnectorIdFromMeterPublicKey(""), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorIdFromMeterPublicKey("1234"), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorIdFromMeterPublicKey("A"), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorIdFromMeterPublicKey("ABC"), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorIdFromMeterPublicKey("A1"), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorIdFromMeterPublicKey("A12"), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorIdFromMeterPublicKey("A123"), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorIdFromMeterPublicKey("MeterPublicKeyMeterPublicKey123"), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorIdFromMeterPublicKey("MeterPublicKey1"), 1);
    EXPECT_EQ(CPCB::extractConnectorIdFromMeterPublicKey("MeterPublicKey12"), 12);
    EXPECT_EQ(CPCB::extractConnectorIdFromMeterPublicKey("MeterPublicKey123"), 123);
}

TEST(ConnectorID, Build) {
    using CPCB = ocpp::v16::ChargePointConfigurationBase;

    EXPECT_EQ(CPCB::meterPublicKeyString(0), "MeterPublicKey0");
    EXPECT_EQ(CPCB::meterPublicKeyString(1), "MeterPublicKey1");
    EXPECT_EQ(CPCB::meterPublicKeyString(12), "MeterPublicKey12");
    EXPECT_EQ(CPCB::meterPublicKeyString(123), "MeterPublicKey123");
}

TEST(ConnectorID, PhaseRotation) {
    using CPCB = ocpp::v16::ChargePointConfigurationBase;
    const char* no_phase_rotation = "0.NotApplicable,1.Unknown,2.NotApplicable,3.Unknown";
    const char* valid_phase_rotation = "0.RST,1.RTS,2.SRT,3.STR,4.TRS,5.TSR";
    const char* valid_phase_rotation_extended = "8.RST,9.RTS,10.SRT,11.STR,12.TRS,13.TSR";

    EXPECT_TRUE(CPCB::isConnectorPhaseRotationValid(5, no_phase_rotation));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(1, no_phase_rotation));

    EXPECT_TRUE(CPCB::isConnectorPhaseRotationValid(5, valid_phase_rotation));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(3, valid_phase_rotation));

    EXPECT_TRUE(CPCB::isConnectorPhaseRotationValid(15, valid_phase_rotation_extended));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(11, valid_phase_rotation_extended));

    EXPECT_TRUE(CPCB::isConnectorPhaseRotationValid(5, ""));

    // error cases
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "123456"));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "abcdef"));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, ".abcd"));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "abcd."));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "1."));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "11."));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "111."));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "1a.RST"));
}

using namespace ocpp;

TEST(V2Mapping, V16ToV2) {
    using namespace ocpp::v16::keys;
    using namespace ocpp::v2;

    auto res = convert_v2(valid_keys::CpoName);
    ASSERT_TRUE(res);
    EXPECT_EQ(res->first.name, "SecurityCtrlr");
    EXPECT_EQ(res->second.name, "OrganizationName");

    res = convert_v2("CpoName");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->first.name, "SecurityCtrlr");
    EXPECT_EQ(res->second.name, "OrganizationName");
}

TEST(V2Mapping, V2ToV16) {
    using namespace ocpp::v16::keys;
    using namespace ocpp::v2;

    Component comp;
    comp.name = "SecurityCtrlr";
    Variable var;
    var.name = "OrganizationName";
    auto res = convert_v2(comp, var, ocpp::v2::AttributeEnum::Actual);
    EXPECT_EQ(res, "CpoName");

    // MaxChargingProfilesInstalled maps to VariableCharacteristics.maxLimit
    comp.name = "SmartChargingCtrlr";
    var.name = "Entries";
    var.instance = "ChargingProfiles";
    EXPECT_FALSE(convert_v2(comp, var, ocpp::v2::AttributeEnum::Actual));
    EXPECT_FALSE(convert_v2(comp, var, ocpp::v2::AttributeEnum::MaxSet));

    // It is reachable via convert_v2.
    const auto cv = convert_v2(valid_keys::MaxChargingProfilesInstalled);
    ASSERT_TRUE(cv);
    EXPECT_EQ(cv->first.name, "SmartChargingCtrlr");
    EXPECT_EQ(cv->second.name, "Entries");
    EXPECT_EQ(cv->second.instance, "ChargingProfiles");
}

// Tests for get_all_key_value() with maxLimit keys.
// These keys map to VariableCharacteristics.maxLimit rather than VariableAttribute,
// so they are populated via a separate code path in get_all_key_value().
class MaxLimitGetAll : public ConfigurationBase {};

TEST_F(MaxLimitGetAll, MaxLimitKeysIncludedWhenSet) {
    ASSERT_TRUE(device_model);
    device_model->set("Core", "MeterValuesAlignedDataMaxLength", "10");
    device_model->set("Core", "MeterValuesSampledDataMaxLength", "20");
    device_model->set("Core", "StopTxnAlignedDataMaxLength", "30");
    device_model->set("Core", "StopTxnSampledDataMaxLength", "40");
    device_model->set("LocalAuthListManagement", "LocalAuthListMaxLength", "50");
    device_model->set("LocalAuthListManagement", "SendLocalListMaxLength", "60");
    device_model->set("SmartCharging", "MaxChargingProfilesInstalled", "70");

    const auto all = v2_config->get_all_key_value();
    const auto find = [&](const std::string& key) {
        return std::find_if(all.begin(), all.end(), [&](const auto& kv) { return kv.key == key.c_str(); });
    };

    struct Expected {
        const char* key;
        const char* value;
    };

    // SendLocalListMaxLength not included because it is a VariableAttribute.

    const Expected expected[] = {
        {"MeterValuesAlignedDataMaxLength", "10"}, {"MeterValuesSampledDataMaxLength", "20"},
        {"StopTxnAlignedDataMaxLength", "30"},     {"StopTxnSampledDataMaxLength", "40"},
        {"LocalAuthListMaxLength", "50"},          {"MaxChargingProfilesInstalled", "70"},
    };

    for (const auto& e : expected) {
        const auto it = find(e.key);
        ASSERT_NE(it, all.end()) << e.key << " missing from get_all_key_value()";
        EXPECT_EQ(it->value, e.value) << e.key;
        EXPECT_TRUE(it->readonly) << e.key << " should be readonly";
    }
}

TEST_F(MaxLimitGetAll, MaxLimitKeysAbsentWhenNotSet) {
    ASSERT_TRUE(device_model);
    // Do not configure any maxLimit keys — they should not appear in the result.
    const auto all = v2_config->get_all_key_value();
    const auto find = [&](const std::string& key) {
        return std::find_if(all.begin(), all.end(), [&](const auto& kv) { return kv.key == key.c_str(); });
    };

    // LocalAuthListMaxLength, SendLocalListMaxLength, MaxChargingProfilesInstalled are present because they are part of
    // the example config

    const char* keys[] = {
        "MeterValuesAlignedDataMaxLength",
        "MeterValuesSampledDataMaxLength",
        "StopTxnAlignedDataMaxLength",
        "StopTxnSampledDataMaxLength",
    };

    for (const auto* key : keys) {
        EXPECT_EQ(find(key), all.end()) << key << " should be absent when maxLimit is not set";
    }
}

using MigrationAssertions =
    std::function<void(ocpp::v16::ChargePointConfiguration&, ocpp::v16::ChargePointConfigurationDeviceModel&)>;

void with_migrated_device_model_config(const std::string& ocpp16_config_json, const MigrationAssertions& assertions) {
    auto json_config = std::make_unique<ocpp::v16::ChargePointConfiguration>(ocpp16_config_json, CONFIG_DIR_V16,
                                                                             USER_CONFIG_FILE_LOCATION_V16);

    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::filesystem::path db_path =
        std::filesystem::temp_directory_path() / ("ocpp16_config_parity_" + std::to_string(timestamp) + ".db");
    std::filesystem::remove(db_path);

    ocpp::v2::InitDeviceModelDb db(db_path.string(), "./resources/v2/device_model_migration_files");
    auto component_configs = ocpp::v2::get_all_component_configs("./resources/example_config/v2/component_config");
    ocpp::v2::patch_component_config_with_ocpp16(component_configs, *json_config);
    ASSERT_NO_THROW(db.initialize_database(component_configs, true));

    std::unique_ptr<ocpp::v2::DeviceModelInterface> storage =
        std::make_unique<ocpp::v2::DeviceModel>(std::make_unique<ocpp::v2::DeviceModelStorageSqlite>(db_path.string()));
    auto device_model_config =
        std::make_unique<ocpp::v16::ChargePointConfigurationDeviceModel>(CONFIG_DIR_V16, std::move(storage));

    assertions(*json_config, *device_model_config);

    std::filesystem::remove(db_path);
}

void expect_config_migration_get_all_key_value_parity(const std::string& ocpp16_config_json) {
    with_migrated_device_model_config(
        ocpp16_config_json, [](ocpp::v16::ChargePointConfiguration& json_config,
                               ocpp::v16::ChargePointConfigurationDeviceModel& device_model_config) {
            const auto json_values = to_map(json_config.get_all_key_value());
            const auto dm_values = to_map(device_model_config.get_all_key_value());

            for (const auto& [key, _] : json_values) {
                if (dm_values.find(key) == dm_values.end()) {
                    ADD_FAILURE() << "Missing in ChargePointConfigurationDeviceModel: " << key;
                }
            }

            EXPECT_LE(json_values.size(), dm_values.size());

            for (const auto& [key, json_kv] : json_values) {
                const auto dm_it = dm_values.find(key);
                ASSERT_NE(dm_it, dm_values.end()) << key << " missing in ChargePointConfigurationDeviceModel";
                EXPECT_EQ(json_kv.readonly, dm_it->second.readonly) << key;
                EXPECT_TRUE(values_equal(json_kv.value, dm_it->second.value)) << key;
            }
        });
}

void expect_config_migration_direct_get_parity(const std::string& ocpp16_config_json) {
    with_migrated_device_model_config(
        ocpp16_config_json, [](ocpp::v16::ChargePointConfiguration& json_config,
                               ocpp::v16::ChargePointConfigurationDeviceModel& device_model_config) {
            const auto json_values = to_map(json_config.get_all_key_value());

            for (const auto& [key, _] : json_values) {
                const auto json_kv = json_config.get(key);
                if (!json_kv.has_value()) {
                    ADD_FAILURE() << key << " missing in ChargePointConfiguration";
                    continue;
                }

                const auto dm_kv = device_model_config.get(key);
                if (!dm_kv.has_value()) {
                    ADD_FAILURE() << key << " missing in ChargePointConfigurationDeviceModel";
                    continue;
                }

                EXPECT_EQ(json_kv->key, dm_kv->key) << key;
                EXPECT_EQ(json_kv->readonly, dm_kv->readonly) << key;
                EXPECT_TRUE(values_equal(key_value_to_string(json_kv->value), key_value_to_string(dm_kv->value)))
                    << key;
            }
        });
}

TEST(ConfigMigration, FullJsonGetAllKeyValueParity) {
    expect_config_migration_get_all_key_value_parity(ocpp::tests::make_ocpp16_test_config_full());
}

TEST(ConfigMigration, SparseJsonGetAllKeyValueParity) {
    const auto sparse = json::parse(ocpp::tests::make_ocpp16_test_config_missing_optionals());
    ASSERT_TRUE(sparse.contains("Internal"));
    ASSERT_TRUE(sparse.contains("Security"));
    ASSERT_TRUE(sparse.contains("CostAndPrice"));

    // Ensure this fixture represents a true sparse migration scenario.
    EXPECT_FALSE(sparse["Internal"].contains("HostName"));
    EXPECT_FALSE(sparse["Security"].contains("AuthorizationKey"));
    EXPECT_FALSE(sparse["CostAndPrice"].contains("DefaultPriceText"));

    expect_config_migration_get_all_key_value_parity(ocpp::tests::make_ocpp16_test_config_missing_optionals());
}

TEST(ConfigMigration, FullJsonDirectGetParity) {
    expect_config_migration_direct_get_parity(ocpp::tests::make_ocpp16_test_config_full());
}

TEST(ConfigMigration, SparseJsonDirectGetParity) {
    expect_config_migration_direct_get_parity(ocpp::tests::make_ocpp16_test_config_missing_optionals());
}

} // namespace
