// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <memory>
#include <string_view>

#include "OCPPExtensionExample.hpp"
#include "generated/types/ocpp.hpp"
#include "module_stub.hpp"

namespace {

std::string json_get(const json& obj, const std::string_view& key) {
    try {
        return obj.at(key);
    } catch (...) {
    }
    return {};
}

// ----------------------------------------------------------------------------
// test class - sets up the module ready for each test

class OCPPExtensionTest : public testing::Test {
protected:
    // ld-ev.cpp has static objects with pointer to adapter
    // so it must be consistent for all tests
    static module::stub::ExtendedModuleAdapter adapter;

    static void infrastructure_init() {
        static bool once{false};
        if (!once) {
            // module initialisation in ld-ev.cpp (generated code)
            RequirementInitialization req;
            module::register_module_adapter(adapter);
            const auto commands = module::everest_register(req);
            adapter.register_commands(commands);
            once = true;
        }
    }

    ModuleInfo module_info{"ocpp_extension", {}, "Apache-2.0", "ocpp_ext", {"/etc", "/libexec", "/share"}, false, false,
                           std::nullopt};
    module::stub::OCPPExtensionExampleStub module;

    OCPPExtensionTest() : module(adapter) {
    }

    void SetUp() override {
        infrastructure_init();
        adapter.clear();
    }

    void TearDown() override {
    }

    void publish_variable_updated(const std::string_view& name, const std::string& value) {
        types::ocpp::EventData data;
        data.component_variable.variable.name = name;
        data.event_id = 0;
        data.trigger = types::ocpp::EventTriggerEnum::Delta;
        data.actual_value = value;
        data.event_notification_type = types::ocpp::EventNotificationType::HardWiredNotification;
        module.var_event_data(data);
    }
};

module::stub::ExtendedModuleAdapter OCPPExtensionTest::adapter;

// ----------------------------------------------------------------------------
// the tests

TEST_F(OCPPExtensionTest, DataTransfer) {
    // call module->init() which is private
    ModuleConfigs configs = R"({
        "data_transfer": {},
        "!module":{
            "enable": true,
            "poll_interval": 0.0,
            "id": 0,
            "keys_to_monitor": ""
        }
    })"_json;
    module::LdEverest::init(configs, module_info);
    // call module->ready() which is private
    module::LdEverest::ready();

    // test the provided data_transfer interface
    auto result = module.call_data_transfer(R"({"request":{"data":"Hello","vendor_id":"Pionix"}})"_json);
    EXPECT_EQ(json_get(result, "status"), "UnknownVendorId");
}

TEST_F(OCPPExtensionTest, DataTransfer2) {
    // test the provided data_transfer interface
    auto result = module.call_data_transfer(R"({"request":{"data":"Hello","vendor_id":"EVerest"}})"_json);
    EXPECT_EQ(json_get(result, "status"), "Accepted");
}

TEST_F(OCPPExtensionTest, UpdateKeys) {
    publish_variable_updated("Heartbeat", "60");
    // nothing published since no variables are being monitored
    auto log = adapter.get_module_publish_log();
    ASSERT_EQ(log.size(), 0);

    adapter.runtime_config_set("keys_to_monitor", "Heartbeat,SecurityProfile");
    log = adapter.get_module_publish_log();
    ASSERT_EQ(log.size(), 1);
    EXPECT_EQ(
        log[0].msg,
        R"({"data":{"response":{"status":"Accepted"},"status":"Ok","status_info":"","type":"Set"},"msg_type":"SetConfigResponse"})");

    publish_variable_updated("SecurityProfile", "2");
    // not expecting an additional publish
    log = adapter.get_module_publish_log();
    ASSERT_EQ(log.size(), 1);

    publish_variable_updated("Heartbeat", "60");
    // expecting an additional publish
    log = adapter.get_module_publish_log();
    ASSERT_EQ(log.size(), 2);
    EXPECT_EQ(log[1].topic, "external/heartbeat-updated");
}

TEST_F(OCPPExtensionTest, RebootRequired) {
    adapter.runtime_config_set("id", "99");
    const auto log = adapter.get_module_publish_log();
    ASSERT_EQ(log.size(), 1);
    EXPECT_EQ(
        log[0].msg,
        R"({"data":{"response":{"status":"RebootRequired"},"status":"Ok","status_info":"","type":"Set"},"msg_type":"SetConfigResponse"})");
}

TEST_F(OCPPExtensionTest, Rejected) {
    adapter.runtime_config_set("poll_interval", "1024");
    const auto log = adapter.get_module_publish_log();
    ASSERT_EQ(log.size(), 1);
    EXPECT_EQ(
        log[0].msg,
        R"({"data":{"response":{"status":"Rejected"},"status":"Ok","status_info":"handler not implemented","type":"Set"},"msg_type":"SetConfigResponse"})");
}

} // namespace
