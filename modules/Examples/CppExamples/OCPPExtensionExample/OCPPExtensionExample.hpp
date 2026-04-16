// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef OCPPEXTENSION_EXAMPLE_HPP
#define OCPPEXTENSION_EXAMPLE_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/ocpp_data_transfer/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/ocpp/Interface.hpp>
#include <generated/interfaces/ocpp_data_transfer/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <mutex>
#include <set>
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct RwConf {
    bool enable;
    double poll_interval;
    int id;
    std::string keys_to_monitor;
};

struct RwConfUpdate {
    using ConfigChangeResult = Everest::config::ConfigChangeResult;

    virtual ~RwConfUpdate() = default;

    // override in class OCPPExtensionExample adding the implementation to OCPPExtensionExample.cpp
    // or inline
    // e.g.
    // ConfigChangeResult on_enable_changed(const bool& value) override {
    //     rw_config.enable = value;
    //     return ConfigChangeResult::Accepted();
    // }

    virtual ConfigChangeResult on_enable_changed(const bool& /* value */) {
        return ConfigChangeResult::Rejected("handler not implemented");
    }
    virtual ConfigChangeResult on_poll_interval_changed(const double& /* value */) {
        return ConfigChangeResult::Rejected("handler not implemented");
    }
    virtual ConfigChangeResult on_id_changed(const int& /* value */) {
        return ConfigChangeResult::Rejected("handler not implemented");
    }
    virtual ConfigChangeResult on_keys_to_monitor_changed(const std::string& /* value */) {
        return ConfigChangeResult::Rejected("handler not implemented");
    }
};

struct Conf {

    const bool& enable;
    const double& poll_interval;
    const int& id;
    const std::string& keys_to_monitor;

    Conf(const RwConf& rw) :
        enable(rw.enable), poll_interval(rw.poll_interval), id(rw.id), keys_to_monitor(rw.keys_to_monitor) {
    }
};

class OCPPExtensionExample : public Everest::ModuleBase, public RwConfUpdate {
public:
    OCPPExtensionExample() = delete;
    OCPPExtensionExample(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                         std::unique_ptr<ocpp_data_transferImplBase> p_data_transfer, std::unique_ptr<ocppIntf> r_ocpp,
                         std::unique_ptr<ocpp_data_transferIntf> r_data_transfer, Conf& config, RwConf& rw_config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_data_transfer(std::move(p_data_transfer)),
        r_ocpp(std::move(r_ocpp)),
        r_data_transfer(std::move(r_data_transfer)),
        config(config),
        rw_config(rw_config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<ocpp_data_transferImplBase> p_data_transfer;
    const std::unique_ptr<ocppIntf> r_ocpp;
    const std::unique_ptr<ocpp_data_transferIntf> r_data_transfer;
    const Conf& config;
    RwConf& rw_config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    ConfigChangeResult on_enable_changed(const bool& value) override;
    ConfigChangeResult on_id_changed(const int& value) override;
    ConfigChangeResult on_keys_to_monitor_changed(const std::string& value) override;
    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1

protected:
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1
    // insert your protected definitions here
    std::set<std::string> monitored_keys;
    std::mutex mutex;

    void event_keys_to_monitor();
    void event_key_updated(const types::ocpp::EventData& event_data);
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1

private:
    friend class LdEverest;
    void init();
    void ready();

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
    // insert your private definitions here
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // OCPPEXTENSION_EXAMPLE_HPP
