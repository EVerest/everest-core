// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef SYSTEM_SPECIFIC_DATA_1_GENERIC_ARRAY_IMPL_HPP
#define SYSTEM_SPECIFIC_DATA_1_GENERIC_ARRAY_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/generic_array/Implementation.hpp>

#include "../PhyVersoBSP.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace system_specific_data_1 {

struct Conf {};

class generic_arrayImpl : public generic_arrayImplBase {
public:
    generic_arrayImpl() = delete;
    generic_arrayImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<PhyVersoBSP>& mod, Conf& config) :
        generic_arrayImplBase(ev, "system_specific_data_1"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // no commands defined for this interface

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<PhyVersoBSP>& mod;
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

} // namespace system_specific_data_1
} // namespace module

#endif // SYSTEM_SPECIFIC_DATA_1_GENERIC_ARRAY_IMPL_HPP
