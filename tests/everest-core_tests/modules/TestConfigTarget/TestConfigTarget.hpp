// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef TEST_CONFIG_TARGET_HPP
#define TEST_CONFIG_TARGET_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct RwConf {
    std::string rw_param;
    std::string rw_reboot_param;
    std::string rw_reject_param;
};

struct RwConfUpdate {
    using ConfigChangeResult = Everest::config::ConfigChangeResult;

    virtual ~RwConfUpdate() = default;

    // override in class TestConfigTarget adding the implementation to
    // TestConfigTarget.cpp or inline e.g. ConfigChangeResult
    // on_rw_param_changed(const std::string& value) override {
    //     rw_config.rw_param = value;
    //     return ConfigChangeResult::Accepted();
    // }

    virtual ConfigChangeResult on_rw_param_changed(const std::string& /* value */) {
        return ConfigChangeResult::Rejected("handler not implemented");
    }
    virtual ConfigChangeResult on_rw_reboot_param_changed(const std::string& /* value */) {
        return ConfigChangeResult::Rejected("handler not implemented");
    }
    virtual ConfigChangeResult on_rw_reject_param_changed(const std::string& /* value */) {
        return ConfigChangeResult::Rejected("handler not implemented");
    }
};

struct Conf {
    std::string ro_param;

    const std::string& rw_param;
    const std::string& rw_reboot_param;
    const std::string& rw_reject_param;

    Conf(const RwConf& rw) :
        rw_param(rw.rw_param), rw_reboot_param(rw.rw_reboot_param), rw_reject_param(rw.rw_reject_param) {
    }
};

class TestConfigTarget : public Everest::ModuleBase, public RwConfUpdate {
public:
    TestConfigTarget() = delete;
    TestConfigTarget(const ModuleInfo& info, std::unique_ptr<emptyImplBase> p_main, Conf& config, RwConf& rw_config) :
        ModuleBase(info), p_main(std::move(p_main)), config(config), rw_config(rw_config){};

    const std::unique_ptr<emptyImplBase> p_main;
    const Conf& config;
    RwConf& rw_config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    ConfigChangeResult on_rw_param_changed(const std::string& value) override;
    ConfigChangeResult on_rw_reboot_param_changed(const std::string& value) override;
    ConfigChangeResult on_rw_reject_param_changed(const std::string& value) override;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1

protected:
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1
    // insert your protected definitions here
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

#endif // TEST_CONFIG_TARGET_HPP
