// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <gtest/gtest.h>

#include <fstream>
#include <memory>

#include <ocpp/v16/charge_point_configuration.hpp>
#include <ocpp/v16/charge_point_configuration_devicemodel.hpp>
#include <ocpp/v16/utils.hpp>

#include "memory_storage.hpp"

namespace ocpp::v16::stubs {

// create instances for v16 and v2 configuration
class ConfigurationBase : public testing::Test {
protected:
    std::unique_ptr<ocpp::v16::ChargePointConfigurationInterface> v16_config;
    std::unique_ptr<ocpp::v16::ChargePointConfigurationInterface> v2_config;
    std::unique_ptr<MemoryStorage> device_model;

    void SetUp() override {
        std::ifstream ifs(CONFIG_FILE_LOCATION_V16);
        const std::string config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        v16_config = std::make_unique<ocpp::v16::ChargePointConfiguration>(config_file, CONFIG_DIR_V16,
                                                                           USER_CONFIG_FILE_LOCATION_V16);
        device_model = std::make_unique<MemoryStorage>();
        std::unique_ptr<ocpp::v2::DeviceModelStorageInterface> proxy =
            std::make_unique<MemoryStorageProxy>(*device_model);
        v2_config = std::make_unique<ocpp::v16::ChargePointConfigurationDeviceModel>(std::move(proxy));
    }

    // void TearDown() override {
    // }
};

// support parameterised tests so the same test can be run against:
// - the v16 JSON configuration
// - the v2 database interface (via an in-memory implementation)
class Configuration : public ConfigurationBase, public testing::WithParamInterface<std::string_view> {
public:
    ocpp::v16::ChargePointConfigurationInterface* get() {
        ocpp::v16::ChargePointConfigurationInterface* result{nullptr};
        if (GetParam() == "sql") {
            result = v2_config.get();
        } else if (GetParam() == "json") {
            result = v16_config.get();
        }
        return result;
    }
};

} // namespace ocpp::v16::stubs
