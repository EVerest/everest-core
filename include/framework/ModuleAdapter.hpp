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
    ModuleBase(const ModuleInfo& info) : info(info){};

    const ModuleInfo& info;

protected:
    void invoke_init(ImplementationBase& impl) {
        impl.init();
    }

    void invoke_ready(ImplementationBase& impl) {
        impl.ready();
    }
};

struct ModuleAdapter {
    using CallFunc = std::function<Result(const Requirement&, const std::string&, Parameters)>;
    using PublishFunc = std::function<void(const std::string&, const std::string&, Value)>;
    using SubscribeFunc = std::function<void(const Requirement&, const std::string&, ValueCallback)>;
    using SubscribeErrorFunc = std::function<void(const Requirement&, const std::string&, error::ErrorCallback)>;
    using SubscribeAllErrorsFunc = std::function<void(error::ErrorCallback)>;
    using SubscribeErrorClearedFunc = SubscribeErrorFunc;
    using SubscribeAllErrorsClearedFunc = SubscribeAllErrorsFunc;
    using RaiseErrorFunc = std::function<error::ErrorHandle(const std::string&, const std::string&, const std::string&,
                                                            const error::Severity&)>;
    using RequestClearErrorUUIDFunc = std::function<Result(const std::string&, const error::ErrorHandle&)>;
    using RequestClearAllErrorsOfMouduleFunc = std::function<Result(const std::string&)>;
    using RequestClearAllErrorsOfTypeOfModuleFunc = std::function<Result(const std::string&, const std::string&)>;
    using ExtMqttPublishFunc = std::function<void(const std::string&, const std::string&)>;
    using ExtMqttSubscribeFunc = std::function<void(const std::string&, StringHandler)>;
    using TelemetryPublishFunc =
        std::function<void(const std::string&, const std::string&, const std::string&, const TelemetryMap&)>;

    CallFunc call;
    PublishFunc publish;
    SubscribeFunc subscribe;
    SubscribeErrorFunc subscribe_error;
    SubscribeAllErrorsFunc subscribe_all_errors;
    SubscribeErrorClearedFunc subscribe_error_cleared;
    SubscribeAllErrorsClearedFunc subscribe_all_errors_cleared;
    RaiseErrorFunc raise_error;
    RequestClearErrorUUIDFunc request_clear_error_uuid;
    RequestClearAllErrorsOfMouduleFunc request_clear_all_errors_of_module;
    RequestClearAllErrorsOfTypeOfModuleFunc request_clear_all_errors_of_type_of_module;
    ExtMqttPublishFunc ext_mqtt_publish;
    ExtMqttSubscribeFunc ext_mqtt_subscribe;
    std::vector<cmd> registered_commands;
    TelemetryPublishFunc telemetry_publish;

    void check_complete() {
        // FIXME (aw): I should throw if some of my handlers are not set
        return;
    }

    void gather_cmds(ImplementationBase& impl) {
        impl._gather_cmds(registered_commands);
    }
};

class MqttProvider {
public:
    MqttProvider(ModuleAdapter& ev) : ev(ev){};

    void publish(const std::string& topic, const std::string& data) {
        ev.ext_mqtt_publish(topic, data);
    }

    void publish(const std::string& topic, const char* data) {
        ev.ext_mqtt_publish(topic, std::string(data));
    }

    void publish(const std::string& topic, bool data) {
        if (data) {
            ev.ext_mqtt_publish(topic, "true");
        } else {
            ev.ext_mqtt_publish(topic, "false");
        }
    }

    void publish(const std::string& topic, int data) {
        ev.ext_mqtt_publish(topic, std::to_string(data));
    }

    void publish(const std::string& topic, double data, int precision) {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(precision) << data;
        ev.ext_mqtt_publish(topic, stream.str());
    }

    void publish(const std::string& topic, double data) {
        this->publish(topic, data, 5);
    }

    void subscribe(const std::string& topic, StringHandler handler) {
        ev.ext_mqtt_subscribe(topic, handler);
    }

private:
    ModuleAdapter& ev;
};

class TelemetryProvider {
public:
    TelemetryProvider(ModuleAdapter& ev) : ev(ev){};

    void publish(const std::string& category, const std::string& subcategory, const std::string& type,
                 const TelemetryMap& telemetry) {
        ev.telemetry_publish(category, subcategory, type, telemetry);
    }

    void publish(const std::string& category, const std::string& subcategory, const TelemetryMap& telemetry) {
        publish(category, subcategory, subcategory, telemetry);
    }

private:
    ModuleAdapter& ev;
};

} // namespace Everest

#endif // MODULE_ADAPTER_HPP
