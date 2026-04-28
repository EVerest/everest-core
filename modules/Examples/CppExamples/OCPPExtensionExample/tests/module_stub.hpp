// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ExtendedModuleAdapterStub.hpp>
#include <ld-ev.hpp>

namespace module::stub {

class OCPPExtensionExampleStub : public ExtendedModuleAdapter::Hooks {
private:
    using CallCallback = Result (OCPPExtensionExampleStub::*)(const Parameters& value);

    ExtendedModuleAdapter& m_adapter;
    std::map<std::string, CallCallback> m_call_callbacks;

public:
    OCPPExtensionExampleStub(ExtendedModuleAdapter& adapter) : m_adapter(adapter) {
        // register calls expected to be made by the module
        m_call_callbacks.emplace("data_transfer", &OCPPExtensionExampleStub::request_data_transfer);
        m_call_callbacks.emplace("get_variables", &OCPPExtensionExampleStub::request_get_variables);
        m_call_callbacks.emplace("set_variables", &OCPPExtensionExampleStub::request_set_variables);
        m_call_callbacks.emplace("monitor_variables", &OCPPExtensionExampleStub::request_monitor_variables);
        m_adapter.set_handler(this);
    }

    virtual ~OCPPExtensionExampleStub() {
        m_adapter.set_handler(nullptr);
    }

    // ========================================================================
    // call implementations provided by the module

    json call_fn(const std::string& topic, const json& value) override {
        json result;
        if (auto it = m_call_callbacks.find(topic); it != m_call_callbacks.end()) {
            result = std::invoke(it->second, this, value);
        } else {
            std::printf("call_fn(%s)\n", topic.c_str());
        }
        return result;
    }

    auto call_data_transfer(const json& args) {
        return m_adapter.call("data_transfer", args);
    }

    void var_event_data(const types::ocpp::EventData& data) {
        json obj = data;
        m_adapter.mqtt_publish(HandlerType::SubscribeVar, "everest/event_data", obj);
    }

protected:
    // ========================================================================
    // call implementations (requests from the module under test)

    virtual std::optional<json> request_monitor_variables(const json& value) {
        std::printf("request_monitor_variables(%s)\n", value.dump().c_str());
        return {};
    }
    virtual std::optional<json> request_set_variables(const json& value) {
        std::printf("request_set_variables(%s)\n", value.dump().c_str());
        const json res = R"(
        {
            "status":"Accepted",
            "component_variable":{"component":{"name":""},"variable":{"name":"ExampleConfigurationKey"}},
            "value":""
        }
        )"_json;
        json result;
        result.push_back(res);
        result.push_back(res);
        std::printf("request_set_variables result(%s)\n", result.dump().c_str());
        return result;
    }

    virtual std::optional<json> request_get_variables(const json& value) {
        std::printf("request_get_variables(%s)\n", value.dump().c_str());
        const json res = R"(
        {
            "status":"Accepted",
            "component_variable":{"component":{"name":""},"variable":{"name":"ExampleConfigurationKey"}},
            "value":""
        }
        )"_json;
        json result;
        result.push_back(res);
        std::printf("request_get_variables result(%s)\n", result.dump().c_str());
        return result;
    }

    virtual std::optional<json> request_data_transfer(const json& value) {
        std::printf("request_data_transfer(%s)\n", value.dump().c_str());
        const json result = R"(
        {
            "status":"Rejected"
        }
        )"_json;
        std::printf("request_data_transfer result(%s)\n", result.dump().c_str());
        return result;
    }
};

} // namespace module::stub
