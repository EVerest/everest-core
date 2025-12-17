// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef MODULE_ADAPTER_HPP
#define MODULE_ADAPTER_HPP

#include "everest.hpp"
#include <everest/logging.hpp>
#include <utils/conversions.hpp>
#include <utils/date.hpp>
#include <utils/error.hpp>

#include <iomanip>
#include <iostream>
#include <memory>

namespace Everest {

// FIXME (aw): does the standard library already has something like this?
template <typename T> class PtrContainer {
public:
    PtrContainer(){};
    // disable copy constructor, because in general it should be used as a reference
    PtrContainer(const PtrContainer& obj) = delete;

    T* operator->() const {
        return ptr;
    }

    operator bool() const {
        return ptr != nullptr;
    }

    void set(T* ptr) {
        this->ptr = ptr;
    }

private:
    T* ptr{nullptr};
};

struct ModuleAdapter;
struct ModuleBase;

class ImplementationBase {
public:
    friend class ModuleAdapter; // for accessing gather_cmds
    friend class ModuleBase;    // for accessing init & ready
    virtual ~ImplementationBase() = default;

private:
    virtual void _gather_cmds(std::vector<cmd>&) = 0;
    virtual void init() = 0;
    virtual void ready() = 0;
};

class ModuleBase {
public:
    ModuleBase(const ModuleInfo& info);
    virtual ~ModuleBase() = default;

    const ModuleInfo& info;

protected:
    void invoke_init(ImplementationBase& impl);

    void invoke_ready(ImplementationBase& impl);
};

namespace error {
struct ErrorManagerImpl;
struct ErrorManagerReq;
struct ErrorManagerReqGlobal;
struct ErrorStateMonitor;
struct ErrorFactory;
} // namespace error
struct ModuleAdapter {
    using CallFunc = std::function<Result(const Requirement&, const std::string&, Parameters)>;
    using PublishFunc = std::function<void(const std::string&, const std::string&, Value)>;
    using SubscribeFunc = std::function<void(const Requirement&, const std::string&, ValueCallback)>;
    using GetErrorManagerImplFunc = std::function<std::shared_ptr<error::ErrorManagerImpl>(const std::string&)>;
    using GetErrorStateMonitorImplFunc = std::function<std::shared_ptr<error::ErrorStateMonitor>(const std::string&)>;
    using GetErrorFactoryFunc = std::function<std::shared_ptr<error::ErrorFactory>(const std::string&)>;
    using GetErrorManagerReqFunc = std::function<std::shared_ptr<error::ErrorManagerReq>(const Requirement&)>;
    using GetGlobalErrorManagerFunc = std::function<std::shared_ptr<error::ErrorManagerReqGlobal>()>;
    using GetGlobalErrorStateMonitorFunc = std::function<std::shared_ptr<error::ErrorStateMonitor>()>;
    using GetErrorStateMonitorReqFunc = std::function<std::shared_ptr<error::ErrorStateMonitor>(const Requirement&)>;
    using ExtMqttPublishFunc = std::function<void(const std::string&, const std::string&)>;
    using ExtMqttSubscribeFunc = std::function<UnsubscribeToken(const std::string&, StringHandler)>;
    using ExtMqttSubscribePairFunc = std::function<UnsubscribeToken(const std::string&, StringPairHandler)>;
    using TelemetryPublishFunc =
        std::function<void(const std::string&, const std::string&, const std::string&, const TelemetryMap&)>;
    using GetMappingFunc = std::function<std::optional<ModuleTierMappings>()>;
    using GetConfigServiceClientFunc = std::function<std::shared_ptr<config::ConfigServiceClient>()>;

    CallFunc call;
    PublishFunc publish;
    SubscribeFunc subscribe;
    GetErrorManagerImplFunc get_error_manager_impl;
    GetErrorStateMonitorImplFunc get_error_state_monitor_impl;
    GetErrorFactoryFunc get_error_factory;
    GetErrorManagerReqFunc get_error_manager_req;
    GetErrorStateMonitorReqFunc get_error_state_monitor_req;
    GetGlobalErrorManagerFunc get_global_error_manager;
    GetGlobalErrorStateMonitorFunc get_global_error_state_monitor;
    ExtMqttPublishFunc ext_mqtt_publish;
    ExtMqttSubscribeFunc ext_mqtt_subscribe;
    ExtMqttSubscribePairFunc ext_mqtt_subscribe_pair;
    std::vector<cmd> registered_commands;
    TelemetryPublishFunc telemetry_publish;
    GetMappingFunc get_mapping;
    GetConfigServiceClientFunc get_config_service_client;

    void check_complete();

    void gather_cmds(ImplementationBase& impl);
};

class MqttProvider {
public:
    MqttProvider(ModuleAdapter& ev);

    void publish(const std::string& topic, const std::string& data);

    void publish(const std::string& topic, const char* data);

    void publish(const std::string& topic, bool data);

    void publish(const std::string& topic, int data);

    void publish(const std::string& topic, double data, int precision);

    void publish(const std::string& topic, double data);

    UnsubscribeToken subscribe(const std::string& topic, StringHandler handler) const;

    UnsubscribeToken subscribe(const std::string& topic, StringPairHandler handler) const;

private:
    ModuleAdapter& ev;
};

class TelemetryProvider {
public:
    TelemetryProvider(ModuleAdapter& ev);

    void publish(const std::string& category, const std::string& subcategory, const std::string& type,
                 const TelemetryMap& telemetry);

    void publish(const std::string& category, const std::string& subcategory, const TelemetryMap& telemetry);

private:
    ModuleAdapter& ev;
};

} // namespace Everest

#endif // MODULE_ADAPTER_HPP
