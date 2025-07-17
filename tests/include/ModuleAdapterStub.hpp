// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef MODULEADAPRERSTUB_H_
#define MODULEADAPRERSTUB_H_

#include <framework/ModuleAdapter.hpp>

#include <optional>
#include <utils/error/error_database_map.hpp>
#include <utils/error/error_factory.hpp>
#include <utils/error/error_manager_impl.hpp>
#include <utils/error/error_manager_req.hpp>
#include <utils/error/error_state_monitor.hpp>
#include <utils/error/error_type_map.hpp>

//-----------------------------------------------------------------------------
namespace module::stub {

struct ModuleAdapterStub : public Everest::ModuleAdapter {

    ModuleAdapterStub() : Everest::ModuleAdapter() {
        call = [this](const Requirement& req, const std::string& str, Parameters p) {
            return this->call_fn(req, str, p);
        };
        publish = [this](std::size_t index, const std::string& s1, const std::string& s2, Value v) {
            this->publish_fn(index, s1, s2, v);
        };
        subscribe = [this](const Requirement& req, const std::string& str, ValueCallback cb) {
            this->subscribe_fn(req, str, cb);
        };
        get_error_manager_impl = [this](const std::string& str) { return this->get_error_manager_impl_fn(str); };
        get_error_state_monitor_impl = [this](const std::string& str) {
            return this->get_error_state_monitor_impl_fn(str);
        };
        get_error_factory = [this](const std::string& str) { return this->get_error_factory_fn(str); };
        get_error_manager_req = [this](const Requirement& req) { return this->get_error_manager_req_fn(req); };
        get_error_state_monitor_req = [this](const Requirement& req) {
            return this->get_error_state_monitor_req_fn(req);
        };
        get_global_error_manager = [this]() { return this->get_global_error_manager_fn(); };
        get_global_error_state_monitor = [this]() { return this->get_global_error_state_monitor_fn(); };
        ext_mqtt_publish = [this](const std::string& s1, const std::string& s2) { this->ext_mqtt_publish_fn(s1, s2); };
        ext_mqtt_subscribe = [this](const std::string& str, StringHandler sh) {
            return this->ext_mqtt_subscribe_fn(str, sh);
        };
        ext_mqtt_subscribe_pair = [this](const std::string& topic, const StringPairHandler& handler) {
            return this->ext_mqtt_subscribe_pair_fn(topic, handler);
        };
        telemetry_publish = [this](const std::string& s1, const std::string& s2, const std::string& s3,
                                   const Everest::TelemetryMap& tm) { this->telemetry_publish_fn(s1, s2, s3, tm); };
        get_mapping = [this]() { return this->get_mapping_fn(); };
    }

    virtual Result call_fn(const Requirement&, const std::string&, Parameters) {
        std::printf("call_fn\n");
        return std::nullopt;
    }
    virtual void publish_fn(std::size_t, const std::string&, const std::string&, Value) {
        std::printf("publish_fn\n");
    }
    virtual void subscribe_fn(const Requirement&, const std::string& fn, ValueCallback) {
        std::printf("subscribe_fn(%s)\n", fn.c_str());
    }
    virtual std::vector<std::shared_ptr<Everest::error::ErrorManagerImpl>>
    get_error_manager_impl_fn(const std::string&) {
        std::printf("get_error_manager_impl_fn\n");
        return {std::make_shared<Everest::error::ErrorManagerImpl>(
            std::make_shared<Everest::error::ErrorTypeMap>(), std::make_shared<Everest::error::ErrorDatabaseMap>(),
            std::list<Everest::error::ErrorType>(),
            [](const Everest::error::Error&) { std::printf("publish_raised_error\n"); },
            [](const Everest::error::Error&) { std::printf("publish_cleared_error\n"); })};
    }
    virtual std::vector<std::shared_ptr<Everest::error::ErrorStateMonitor>>
    get_error_state_monitor_impl_fn(const std::string&) {
        std::printf("get_error_state_monitor_impl_fn\n");
        return {
            std::make_shared<Everest::error::ErrorStateMonitor>(std::make_shared<Everest::error::ErrorDatabaseMap>())};
    }
    virtual std::shared_ptr<Everest::error::ErrorManagerReqGlobal> get_global_error_manager_fn() {
        std::printf("get_global_error_manager_fn\n");
        return {};
    }
    virtual std::shared_ptr<Everest::error::ErrorStateMonitor> get_global_error_state_monitor_fn() {
        std::printf("get_global_error_state_monitor_fn\n");
        return {};
    }
    virtual std::vector<std::shared_ptr<Everest::error::ErrorFactory>> get_error_factory_fn(const std::string&) {
        std::printf("get_error_factory_fn\n");
        return {std::make_shared<Everest::error::ErrorFactory>(std::make_shared<Everest::error::ErrorTypeMap>())};
    }
    virtual std::shared_ptr<Everest::error::ErrorManagerReq> get_error_manager_req_fn(const Requirement&) {
        std::printf("get_error_manager_req_fn\n");
        return std::make_shared<Everest::error::ErrorManagerReq>(
            std::make_shared<Everest::error::ErrorTypeMap>(), std::make_shared<Everest::error::ErrorDatabaseMap>(),
            std::list<Everest::error::ErrorType>(),
            [](const Everest::error::ErrorType&, const Everest::error::ErrorCallback&,
               const Everest::error::ErrorCallback&) { std::printf("subscribe_error\n"); });
    }
    virtual std::shared_ptr<Everest::error::ErrorStateMonitor> get_error_state_monitor_req_fn(const Requirement&) {
        std::printf("get_error_state_monitor_req_fn\n");
        return std::make_shared<Everest::error::ErrorStateMonitor>(
            Everest::error::ErrorStateMonitor(std::make_shared<Everest::error::ErrorDatabaseMap>()));
    }
    virtual void ext_mqtt_publish_fn(const std::string&, const std::string&) {
        std::printf("ext_mqtt_publish_fn\n");
    }
    virtual std::function<void()> ext_mqtt_subscribe_fn(const std::string&, StringHandler) {
        std::printf("ext_mqtt_subscribe_fn\n");
        return nullptr;
    }
    virtual std::function<void()> ext_mqtt_subscribe_pair_fn(const std::string& topic,
                                                             const StringPairHandler& handler) {
        std::printf("ext_mqtt_subscribe_pair_fn\n");
        return {};
    }
    virtual void telemetry_publish_fn(const std::string&, const std::string&, const std::string&,
                                      const Everest::TelemetryMap&) {
        std::printf("telemetry_publish_fn\n");
    }
    virtual std::optional<ModuleTierMappings> get_mapping_fn() {
        std::printf("get_mapping_fn\n");
        return {};
    }
};

struct QuietModuleAdapterStub : public ModuleAdapterStub {
    Result call_fn(const Requirement&, const std::string&, Parameters) override {
        return {};
    }
    void publish_fn(std::size_t, const std::string&, const std::string&, Value) override {
    }
    void subscribe_fn(const Requirement&, const std::string& fn, ValueCallback) override {
    }
    std::vector<std::shared_ptr<Everest::error::ErrorManagerImpl>>
    get_error_manager_impl_fn(const std::string&) override {
        return {std::make_shared<Everest::error::ErrorManagerImpl>(
            std::make_shared<Everest::error::ErrorTypeMap>(), std::make_shared<Everest::error::ErrorDatabaseMap>(),
            std::list<Everest::error::ErrorType>(), [](const Everest::error::Error&) {},
            [](const Everest::error::Error&) {})};
    }
    std::vector<std::shared_ptr<Everest::error::ErrorStateMonitor>>
    get_error_state_monitor_impl_fn(const std::string&) override {
        return {
            std::make_shared<Everest::error::ErrorStateMonitor>(std::make_shared<Everest::error::ErrorDatabaseMap>())};
    }
    std::shared_ptr<Everest::error::ErrorManagerReqGlobal> get_global_error_manager_fn() override {
        return {};
    }
    std::shared_ptr<Everest::error::ErrorStateMonitor> get_global_error_state_monitor_fn() override {
        return {};
    }
    std::vector<std::shared_ptr<Everest::error::ErrorFactory>> get_error_factory_fn(const std::string&) override {
        return {std::make_shared<Everest::error::ErrorFactory>(std::make_shared<Everest::error::ErrorTypeMap>())};
    }
    std::shared_ptr<Everest::error::ErrorManagerReq> get_error_manager_req_fn(const Requirement&) override {
        return std::make_shared<Everest::error::ErrorManagerReq>(
            std::make_shared<Everest::error::ErrorTypeMap>(), std::make_shared<Everest::error::ErrorDatabaseMap>(),
            std::list<Everest::error::ErrorType>(),
            [](const Everest::error::ErrorType&, const Everest::error::ErrorCallback&,
               const Everest::error::ErrorCallback&) { std::printf("subscribe_error\n"); });
    }
    std::shared_ptr<Everest::error::ErrorStateMonitor> get_error_state_monitor_req_fn(const Requirement&) override {
        return std::make_shared<Everest::error::ErrorStateMonitor>(
            Everest::error::ErrorStateMonitor(std::make_shared<Everest::error::ErrorDatabaseMap>()));
    }
    void ext_mqtt_publish_fn(const std::string&, const std::string&) override {
    }
    std::function<void()> ext_mqtt_subscribe_fn(const std::string&, StringHandler) override {
        return nullptr;
    }
    std::function<void()> ext_mqtt_subscribe_pair_fn(const std::string& topic,
                                                     const StringPairHandler& handler) override {
        return {};
    }
    void telemetry_publish_fn(const std::string&, const std::string&, const std::string&,
                              const Everest::TelemetryMap&) override {
    }
    std::optional<ModuleTierMappings> get_mapping_fn() override {
        return {};
    }
};

} // namespace module::stub

#endif
