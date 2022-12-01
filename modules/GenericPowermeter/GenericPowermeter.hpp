// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef GENERIC_POWERMETER_HPP
#define GENERIC_POWERMETER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 1
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/powermeter/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/serial_communication_hub/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {};

class GenericPowermeter : public Everest::ModuleBase {
public:
    GenericPowermeter() = delete;
    GenericPowermeter(const ModuleInfo& info, std::unique_ptr<powermeterImplBase> p_main,
                      std::unique_ptr<serial_communication_hubIntf> r_serial_comm_hub, Conf& config) :
        ModuleBase(info), p_main(std::move(p_main)), r_serial_comm_hub(std::move(r_serial_comm_hub)), config(config){};

    const Conf& config;
    const std::unique_ptr<powermeterImplBase> p_main;
    const std::unique_ptr<serial_communication_hubIntf> r_serial_comm_hub;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
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

#endif // GENERIC_POWERMETER_HPP
