// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef MODULE_ADAPTER_HPP
#define MODULE_ADAPTER_HPP

#include "everest.hpp"
#include <iostream>

#include <iomanip>

namespace Everest {

// FIXME (aw): proper namespacing for these utility classes and functions

namespace detail_module_adapter {
template <class Ret> bool any_to_variant_impl(Ret& var, const boost::any& val) noexcept {
    // we didn't find any proper type to convert to :(
    return false;
}

template <class Ret, class F, class... R> bool any_to_variant_impl(Ret& var, const boost::any& val) noexcept {
    if (val.type() == typeid(F)) {
        var = boost::any_cast<F>(val);
        return true;
    }

    return any_to_variant_impl<Ret, R...>(var, val);
}

class ToAnyVisitor : public boost::static_visitor<boost::any> {
public:
    template <typename T> boost::any operator()(const T& value) const {
        return value;
    }
};
} // namespace detail_module_adapter

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
    using CallFunc = std::function<Result(const std::string&, const std::string&, Parameters)>;
    using PublishFunc = std::function<void(const std::string&, const std::string&, Value)>;
    using SubscribeFunc = std::function<void(const std::string&, const std::string&, ValueCallback)>;
    using ExtMqttPublishFunc = std::function<void(const std::string&, const std::string&)>;
    using ExtMqttSubscribeFunc = std::function<void(const std::string&, StringHandler)>;

    CallFunc call;
    PublishFunc publish;
    SubscribeFunc subscribe;
    ExtMqttPublishFunc ext_mqtt_publish;
    ExtMqttSubscribeFunc ext_mqtt_subscribe;
    std::vector<cmd> registered_commands;

    void check_complete() {
        // FIXME (aw): I should throw if some of my handlers are not set
        return;
    }

    void gather_cmds(ImplementationBase& impl) {
        impl._gather_cmds(registered_commands);
    }

    // FIXME (aw): Depending on the unrolling performance and usage with different types
    //             this template function should automatically generated for the Exports hpps
    template <class... Ts> static boost::variant<Ts...> any_to_variant(const boost::any& val) {
        boost::variant<Ts...> var;
        if (detail_module_adapter::any_to_variant_impl<boost::variant<Ts...>, Ts...>(var, val))
            return var;

        throw std::runtime_error(
            // FIXME (aw): propably we could be more explicit on the types here
            "The given boost::any object doesn't contain any type, the boost::variant is aware of");
    }

    template <typename Variant> static boost::any variant_to_any(const Variant& var) {
        static auto visitor = detail_module_adapter::ToAnyVisitor();
        return boost::apply_visitor(visitor, var);
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

} // namespace Everest

#endif // MODULE_ADAPTER_HPP
