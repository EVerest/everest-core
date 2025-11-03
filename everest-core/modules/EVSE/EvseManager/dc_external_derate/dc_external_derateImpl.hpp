// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef DC_EXTERNAL_DERATE_DC_EXTERNAL_DERATE_IMPL_HPP
#define DC_EXTERNAL_DERATE_DC_EXTERNAL_DERATE_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/dc_external_derate/Implementation.hpp>

#include "../EvseManager.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace dc_external_derate {

struct Conf {};

class dc_external_derateImpl : public dc_external_derateImplBase {
public:
    dc_external_derateImpl() = delete;
    dc_external_derateImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<EvseManager>& mod, Conf& config) :
        dc_external_derateImplBase(ev, "dc_external_derate"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_set_external_derating(types::dc_external_derate::ExternalDerating& derate) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<EvseManager>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace dc_external_derate
} // namespace module

#endif // DC_EXTERNAL_DERATE_DC_EXTERNAL_DERATE_IMPL_HPP
