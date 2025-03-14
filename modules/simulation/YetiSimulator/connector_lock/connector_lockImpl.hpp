// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/connector_lock/Implementation.hpp>

#include "../YetiSimulator.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module::connector_lock {

struct Conf {};

class connector_lockImpl : public connector_lockImplBase {
public:
    connector_lockImpl() = delete;
    connector_lockImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<YetiSimulator>& mod, Conf& config) :
        connector_lockImplBase(ev, "connector_lock"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    void handle_lock() override;
    void handle_unlock() override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<YetiSimulator>& mod;
    const Conf& config;

    void init() override;
    void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace module::connector_lock
