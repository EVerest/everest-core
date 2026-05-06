// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include <ocpp/v2/ocpp16_custom_config_mappings.hpp>

namespace ocpp::v2 {

class Ocpp16CustomConfigMappingsTest : public ::testing::Test {
protected:
    std::filesystem::path test_dir;

    void SetUp() override {
        test_dir = std::filesystem::temp_directory_path() / "ocpp16_custom_mapping_tests";
        std::filesystem::create_directories(test_dir);
    }

    void TearDown() override {
        std::error_code ec;
        std::filesystem::remove_all(test_dir, ec);
    }

    std::filesystem::path write_yaml_file(const std::string& filename, const std::string& yaml_content) {
        const auto file_path = test_dir / filename;
        std::ofstream ofs(file_path);
        ofs << yaml_content;
        ofs.close();
        return file_path;
    }
};

TEST_F(Ocpp16CustomConfigMappingsTest, schema_contains_mappings_root_property) {
    const auto schema = get_ocpp16_custom_mapping_schema();
    ASSERT_TRUE(schema.contains("properties"));
    ASSERT_TRUE(schema.at("properties").contains("mappings"));
}

TEST_F(Ocpp16CustomConfigMappingsTest, load_valid_yaml_returns_mappings) {
    const auto yaml_file = write_yaml_file("valid.yaml", R"yaml(
mappings:
  - ocpp16_key: CustomKey1
    component:
      name: CustomController1
      instance: Main
      evse:
        id: 7
        connectorId: 2
    variable:
      name: CustomVariable1
      instance: Default
  - ocpp16_key: CustomKey2
    component:
      name: CustomController2
    variable:
      name: CustomVariable2
)yaml");

    const auto mappings = load_ocpp16_custom_config_mappings_from_yaml(yaml_file);

    ASSERT_EQ(mappings.size(), 2);
    ASSERT_TRUE(mappings.find("CustomKey1") != mappings.end());
    ASSERT_TRUE(mappings.find("CustomKey2") != mappings.end());

    const auto& connector_mapping = mappings.at("CustomKey1");
    EXPECT_EQ(connector_mapping.first.name.get(), "CustomController1");
    ASSERT_TRUE(connector_mapping.first.instance.has_value());
    EXPECT_EQ(connector_mapping.first.instance.value().get(), "Main");
    ASSERT_TRUE(connector_mapping.first.evse.has_value());
    EXPECT_EQ(connector_mapping.first.evse->id, 7);
    ASSERT_TRUE(connector_mapping.first.evse->connectorId.has_value());
    EXPECT_EQ(connector_mapping.first.evse->connectorId.value(), 2);
    EXPECT_EQ(connector_mapping.second.name.get(), "CustomVariable1");
    ASSERT_TRUE(connector_mapping.second.instance.has_value());
    EXPECT_EQ(connector_mapping.second.instance.value().get(), "Default");

    const auto& foo_mapping = mappings.at("CustomKey2");
    EXPECT_EQ(foo_mapping.first.name.get(), "CustomController2");
    EXPECT_FALSE(foo_mapping.first.instance.has_value());
    EXPECT_FALSE(foo_mapping.first.evse.has_value());
    EXPECT_EQ(foo_mapping.second.name.get(), "CustomVariable2");
    EXPECT_FALSE(foo_mapping.second.instance.has_value());
}

TEST_F(Ocpp16CustomConfigMappingsTest, invalid_schema_throws) {
    const auto yaml_file = write_yaml_file("invalid_schema.yaml", R"yaml(
not_mappings:
  - ocpp16_key: CustomKey3
    component:
      name: CustomController3
    variable:
      name: CustomVariable3
)yaml");

    EXPECT_THROW((void)load_ocpp16_custom_config_mappings_from_yaml(yaml_file), Ocpp16CustomConfigMappingsError);
}

TEST_F(Ocpp16CustomConfigMappingsTest, duplicate_keys_throw) {
    const auto yaml_file = write_yaml_file("duplicate.yaml", R"yaml(
mappings:
  - ocpp16_key: DuplicateKey
    component:
      name: CustomControllerDuplicateA
    variable:
      name: CustomVariableDuplicateA
  - ocpp16_key: DuplicateKey
    component:
      name: CustomControllerDuplicateB
    variable:
      name: CustomVariableDuplicateB
)yaml");

    EXPECT_THROW((void)load_ocpp16_custom_config_mappings_from_yaml(yaml_file), Ocpp16CustomConfigMappingsError);
}

TEST_F(Ocpp16CustomConfigMappingsTest, missing_file_throws) {
    const auto missing_file = test_dir / "does_not_exist.yaml";
    EXPECT_THROW((void)load_ocpp16_custom_config_mappings_from_yaml(missing_file), Ocpp16CustomConfigMappingsError);
}

} // namespace ocpp::v2
