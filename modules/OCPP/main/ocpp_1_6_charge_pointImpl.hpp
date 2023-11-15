// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_OCPP_1_6_CHARGE_POINT_IMPL_HPP
#define MAIN_OCPP_1_6_CHARGE_POINT_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/ocpp_1_6_charge_point/Implementation.hpp>

#include "../OCPP.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {};

class ocpp_1_6_charge_pointImpl : public ocpp_1_6_charge_pointImplBase {
public:
    ocpp_1_6_charge_pointImpl() = delete;
    ocpp_1_6_charge_pointImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<OCPP>& mod, Conf& config) :
        ocpp_1_6_charge_pointImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual bool handle_stop() override;
    virtual bool handle_restart() override;
    virtual types::ocpp::GetConfigurationResponse handle_get_configuration_key(Array& keys) override;
    virtual types::ocpp::ConfigurationStatus handle_set_custom_configuration_key(std::string& key,
                                                                                 std::string& value) override;
    virtual void handle_monitor_configuration_keys(Array& keys) override;
    virtual void handle_security_event(std::string& type, std::string& info) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<OCPP>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    std::mutex m;
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_OCPP_1_6_CHARGE_POINT_IMPL_HPP
